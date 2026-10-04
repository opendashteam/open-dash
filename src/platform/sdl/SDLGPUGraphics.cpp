#include "SDLGPUGraphics.h"
#include "engine/utilities/log.h"
#include <glm/glm.hpp>
#include <set>
#include "../../../assets/shaders/common.h"
#include "../../engine/AssetManager.h"
#include "../../engine/utilities/MathUtils.h"
#include "../../engine/core/Director.h"

using namespace opendash::engine;

#define std140_vec2 alignas(8)  glm::vec2
#define std140_vec3 alignas(16) glm::vec3
#define std140_vec4 alignas(16) glm::vec4
#define std140_mat2 alignas(8)  glm::mat2
#define std140_mat3 alignas(16) glm::mat3x4
#define std140_mat4 alignas(16) glm::mat4
#define std140_bool alignas(4) bool

namespace opendash::platform
{

SDLGPUGraphics::~SDLGPUGraphics()
{
    assert(numTexturesAllocated_ == 0);

    gpu::releaseAllObjects();
    
    for (const auto& [_, circleBuffer] : circleBuffers_)
        gpu::release(circleBuffer);

    for (const auto& [_, circleBuffer] : outlineCircleBuffers_) {
        gpu::release(circleBuffer.vertexBuffer);
        gpu::release(circleBuffer.indexBuffer);
    }

    if (quadVertexBuffer_)
        gpu::release(quadVertexBuffer_);

    SDL_ReleaseWindowFromGPUDevice(device_, window_);
    SDL_DestroyGPUDevice(device_);
}

bool SDLGPUGraphics::beginDraw()
{
    assert(commandBuffer_ == nullptr);

    commandBuffer_ = gpu::acquireCommandBuffer();

    u32 w, h;

    SDL_GPUTexture* swapchainTexture;
    SDL_WaitAndAcquireGPUSwapchainTexture(commandBuffer_, window_, &swapchainTexture, &w, &h);

    if (!swapchainTexture) {
        SDL_CancelGPUCommandBuffer(commandBuffer_);
        commandBuffer_ = nullptr;
        return false;
    }

    SDL_GPUColorTargetInfo colorTargetInfo{};
    colorTargetInfo.texture = swapchainTexture;
    colorTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
    colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;
    colorTargetInfo.clear_color = {0.0f, 0.0f, 0.0f, 1.0f};

    renderPass_ = SDL_BeginGPURenderPass(commandBuffer_, &colorTargetInfo, 1, nullptr);
    
    return true;
}

void SDLGPUGraphics::finishDraw()
{
    assert(commandBuffer_ != nullptr);
    assert(renderPass_ != nullptr);

    SDL_EndGPURenderPass(renderPass_);
    gpu::submit(commandBuffer_);
    commandBuffer_ = nullptr;
    renderPass_ = nullptr;
}

struct TextureContainer {
    SDL_GPUTexture* texture;
    u32 width, height;
};

void SDLGPUGraphics::setViewProjectionMatrix(const glm::mat4& viewProjection) {
    assert(commandBuffer_ != nullptr);
    SDL_PushGPUVertexUniformData(commandBuffer_, UNIFORM_SLOT_VIEW_PROJECTION, &viewProjection, sizeof(viewProjection));
}

InternalTexture SDLGPUGraphics::textureCreate(
    u32 width,
    u32 height,
    TextureFormat format,
    void* data
) {
    SDL_GPUTextureFormat sdlFormat = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;

    SDL_GPUTextureCreateInfo textureInfo{};
    textureInfo.type = SDL_GPU_TEXTURETYPE_2D;
    textureInfo.format = sdlFormat;
    textureInfo.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER;
    textureInfo.width = width;
    textureInfo.height = height;
    textureInfo.layer_count_or_depth = 1;
    textureInfo.num_levels = 1;

    SDL_GPUTexture* texture = SDL_CreateGPUTexture(device_, &textureInfo);
    if (!texture)
        return nullptr;

    auto uploadBuffer = gpu::createGPUUploadBuffer(width * height * 4, data);   
    if (!uploadBuffer) {
        gpu::release(texture);
        return nullptr;
    }

    SDL_GPUCommandBuffer* uploadCmd = gpu::acquireCommandBuffer();
    SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(uploadCmd);

    SDL_GPUTextureTransferInfo texTransferInfo{};
    texTransferInfo.transfer_buffer = uploadBuffer;

    SDL_GPUTextureRegion texRegion{};
    texRegion.texture = texture;
    texRegion.w = width;
    texRegion.h = height;
    texRegion.d = 1;

    SDL_UploadToGPUTexture(copyPass, &texTransferInfo, &texRegion, false);

    SDL_EndGPUCopyPass(copyPass);
    gpu::submit(uploadCmd);

    gpu::release(uploadBuffer);

    numTexturesAllocated_++;
    return (InternalTexture)new TextureContainer { texture, width, height };
}

void SDLGPUGraphics::textureDestroy(InternalTexture raw)
{
    numTexturesAllocated_--;

    auto texture = (TextureContainer*)raw;

    SDL_ReleaseGPUTexture(device_, texture->texture);
    delete texture;
}

static const float quadVerticies[] = {
    0.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 0.0f,
    1.0f, 1.0f
};

struct SpriteQuadVertex {
    glm::vec2 position;
    glm::vec2 texCoord;
    glm::vec4 color;
};

using SpriteQuadVertexSet = SpriteQuadVertex[4];
using SpriteQuadIndexSet  = u32[6];

enum class SpriteBatchDirtyType { None, Set, Range };

#define DIRTY_SPRITE_SET_MAX_SIZE 8

struct SpriteBatchContainer {
    u32 capacity = 0;
    SpriteQuadVertexSet* vertexBufferMapped = nullptr;
    SDL_GPUTransferBuffer* vertexUploadBuffer = nullptr;
    SDL_GPUBuffer* vertexBuffer = nullptr;
    SDL_GPUBuffer* indexBuffer = nullptr;

    SpriteBatchDirtyType dirtyType = SpriteBatchDirtyType::None;
    std::set<u32> dirtySpriteSet;
    u32 dirtySpriteRangeStart;
    u32 dirtySpriteRangeEnd;
};

InternalSpriteBatch SDLGPUGraphics::spriteBatchCreate() {
    return (InternalSpriteBatch)new SpriteBatchContainer;
}

void SDLGPUGraphics::spriteBatchResize(InternalSpriteBatch raw, u32 capacity) {
    auto batch = (SpriteBatchContainer*)raw;
    if (capacity <= batch->capacity)
        return;

    auto newVertexBuffer = gpu::createGPUBuffer(sizeof(SpriteQuadVertexSet) * capacity, SDL_GPU_BUFFERUSAGE_VERTEX);
    auto newIndexBuffer  = gpu::createGPUBuffer(sizeof(SpriteQuadIndexSet) * capacity,  SDL_GPU_BUFFERUSAGE_INDEX);

    if (batch->capacity > 0) {
        auto cbuf = gpu::acquireCommandBuffer();

        gpu::copyBuffer(
            cbuf,
            batch->vertexBuffer,
            newVertexBuffer,
            sizeof(SpriteQuadVertexSet) * batch->capacity,
            0, 0
        );
        gpu::copyBuffer(
            cbuf,
            batch->indexBuffer,
            newIndexBuffer,
            sizeof(SpriteQuadIndexSet) * batch->capacity,
            0, 0
        );

        gpu::submit(cbuf);

        gpu::unmapBuffer(batch->vertexUploadBuffer);
        gpu::release(batch->vertexUploadBuffer);
        gpu::release(batch->vertexBuffer);
        gpu::release(batch->indexBuffer);
    }

    u32 newIndiciesSize = (capacity - batch->capacity) * sizeof(SpriteQuadIndexSet);
    auto indexUploadBuffer = gpu::createGPUUploadBuffer(newIndiciesSize);
    auto indicies = (SpriteQuadIndexSet*)gpu::mapBuffer(indexUploadBuffer);

    // TODO: FIX CONTENTS OF UPLOAD BUFFER NOT BEING COPIED!!!!

    auto indexPtr = indicies;
    for (u32 i = batch->capacity; i < capacity; i++) {
        u32 vertexIndex = i * 4;
        (*indexPtr)[0] = vertexIndex + 0;
        (*indexPtr)[1] = vertexIndex + 1;
        (*indexPtr)[2] = vertexIndex + 2;
        (*indexPtr)[3] = vertexIndex + 1;
        (*indexPtr)[4] = vertexIndex + 3;
        (*indexPtr)[5] = vertexIndex + 2;
        indexPtr++;
    }

    gpu::unmapBuffer(indexUploadBuffer);
    gpu::uploadBufferData(NULL, indexUploadBuffer, newIndexBuffer, newIndiciesSize, 0, batch->capacity * sizeof(SpriteQuadIndexSet));
    gpu::release(indexUploadBuffer);

    batch->vertexUploadBuffer = gpu::createGPUUploadBuffer(sizeof(SpriteQuadVertexSet) * capacity);
    batch->vertexBufferMapped = (SpriteQuadVertexSet*)gpu::mapBuffer(batch->vertexUploadBuffer);
    batch->vertexBuffer = newVertexBuffer;
    batch->indexBuffer = newIndexBuffer;
    batch->capacity = capacity;
}

void SDLGPUGraphics::spriteBatchSetSprite(
    InternalSpriteBatch raw,
    u32 spriteIndex,
    const glm::mat4& positionTransform,
    const glm::mat3& textureTransform,
    const Color4F& color
) {
    auto batch = (SpriteBatchContainer*)raw;
    if (spriteIndex >= batch->capacity)
        return;

    auto quad = batch->vertexBufferMapped + spriteIndex;

    for (u32 i = 0; i < 4; i++) {
        glm::vec2 pos = { quadVerticies[i * 2 + 0], quadVerticies[i * 2 + 1] };

        (*quad)[i].color = { color.r, color.g, color.b, color.a };
        (*quad)[i].position = glm::vec2(positionTransform * glm::vec4(pos, 0.0f, 1.0f));
        (*quad)[i].texCoord = glm::vec2(textureTransform * glm::vec3(pos, 1.0f));
    }

    if (batch->dirtyType == SpriteBatchDirtyType::None) {
        batch->dirtyType = SpriteBatchDirtyType::Set;
        batch->dirtySpriteRangeStart = spriteIndex;
        batch->dirtySpriteRangeEnd   = spriteIndex + 1;
    } else {
        batch->dirtySpriteRangeStart = std::min(batch->dirtySpriteRangeStart, spriteIndex);
        batch->dirtySpriteRangeEnd   = std::max(batch->dirtySpriteRangeEnd,   spriteIndex + 1);
    }

    if (batch->dirtyType == SpriteBatchDirtyType::Set) {
        if (batch->dirtySpriteSet.size() == DIRTY_SPRITE_SET_MAX_SIZE)
            batch->dirtyType = SpriteBatchDirtyType::Range;
        else
            batch->dirtySpriteSet.insert(spriteIndex);
    }
}

void SDLGPUGraphics::spriteBatchDestroy(InternalSpriteBatch raw) {
    auto batch = (SpriteBatchContainer*)raw;
    if (batch->capacity == 0)
        return;

    gpu::unmapBuffer(batch->vertexUploadBuffer);
    gpu::release(batch->vertexUploadBuffer);
    gpu::release(batch->vertexBuffer);
    gpu::release(batch->indexBuffer);
}

struct MeshContainer {
    SDL_GPUTransferBuffer* uploadBuffer = nullptr;
    SDL_GPUBuffer* vertexBuffer = nullptr;
    void* mappedData = nullptr;
    u32 vertexCapacity = 0;
};

InternalMesh SDLGPUGraphics::meshCreate() {
    return new MeshContainer;
};

void SDLGPUGraphics::meshResize(InternalMesh rawMesh, u32 vertexCapacity) {
    auto mesh = (MeshContainer*)rawMesh;
    if (vertexCapacity <= mesh->vertexCapacity)
        return;

    u32 oldBufferSize = mesh->vertexCapacity * sizeof(MeshVertex);
    u32 newBufferSize = vertexCapacity * sizeof(MeshVertex);

    auto newVertexBuffer = gpu::createGPUBuffer(newBufferSize, SDL_GPU_BUFFERUSAGE_VERTEX);
    auto newUploadBuffer = gpu::createGPUUploadBuffer(newBufferSize);

    auto mappedData = gpu::mapBuffer(newUploadBuffer);

    if (mesh->vertexBuffer) {
        gpu::copyBuffer(nullptr, mesh->vertexBuffer, newVertexBuffer, oldBufferSize);
        memcpy(mappedData, mesh->mappedData, oldBufferSize);

        gpu::unmapBuffer(mesh->uploadBuffer);
        gpu::release(mesh->uploadBuffer);
        gpu::release(mesh->vertexBuffer);
    }

    mesh->vertexBuffer = newVertexBuffer;
    mesh->uploadBuffer = newUploadBuffer;
    mesh->mappedData = mappedData;
};

MeshVertex* SDLGPUGraphics::meshGetBuffer(InternalMesh mesh) {
    return (MeshVertex*)((MeshContainer*)mesh)->mappedData;
};

void SDLGPUGraphics::meshFlushBufferRange(InternalMesh rawMesh, u32 firstVertex, u32 vertexCount) {
    auto mesh = (MeshContainer*)rawMesh;
    if (!mesh->vertexBuffer || firstVertex >= mesh->vertexCapacity)
        return;

    vertexCount = std::min(vertexCount, mesh->vertexCapacity - firstVertex);
    u32 offset = firstVertex * sizeof(MeshVertex);

    gpu::uploadBufferData(nullptr, mesh->uploadBuffer, mesh->vertexBuffer, vertexCount * sizeof(MeshVertex), offset, offset);
};

void SDLGPUGraphics::meshDestroy(InternalMesh rawMesh) {
    auto mesh = (MeshContainer*)rawMesh;
    if (mesh->vertexBuffer) {
        gpu::unmapBuffer(mesh->uploadBuffer);
        gpu::release(mesh->uploadBuffer);
        gpu::release(mesh->vertexBuffer);
    }
    delete mesh;
};

struct MeshUBO {
    std140_mat4 positionTransform;
    std140_vec4 color;
};

static TextureWrapParameters spriteBatchWrapParams = {
    WrapMode::Clamp,
    WrapMode::Clamp
};

void SDLGPUGraphics::drawMesh(
    InternalMesh rawMesh,
    const glm::mat4& positionTransform,
    const engine::Color4F& globalColor,
    InternalTexture rawTexture,
    u32 vertexCount,
    bool blending
) {
    auto mesh = (MeshContainer*)rawMesh;
    if (!mesh->vertexBuffer || vertexCount == 0)
        return;

    SDL_GPUBufferBinding vertexBinding{};
    vertexBinding.buffer = mesh->vertexBuffer;

    MeshUBO ubo { positionTransform, Color4F::toVector(globalColor) };

    SDL_PushGPUVertexUniformData(commandBuffer_, UNIFORM_SLOT_MESH_UBO, &ubo, sizeof(ubo));
    if (rawTexture == nullptr)
        SDL_BindGPUGraphicsPipeline(renderPass_, solidMeshPipeline_.getWithAddBlend(blending));
    else {
        auto texture = (TextureContainer*)rawTexture;
        SDL_BindGPUGraphicsPipeline(renderPass_, textureMeshPipeline_.getWithAddBlend(blending));

        SDL_GPUTextureSamplerBinding textureBinding{};
        textureBinding.texture = texture->texture;
        // TODO: Customize wrap params
        textureBinding.sampler = gpu::fetchSampler(spriteBatchWrapParams);

        if (!textureBinding.sampler)
            return;

        SDL_BindGPUFragmentSamplers(renderPass_, 0, &textureBinding, 1);
    }
    
    SDL_BindGPUVertexBuffers(renderPass_, 0, &vertexBinding, 1);
    SDL_DrawGPUPrimitives(renderPass_, vertexCount, 1, 0, 0);
}

struct SpriteBatchPropertiesUBO {
    std140_mat4 positionTransform;
    std140_vec4 globalColor;
};

void SDLGPUGraphics::drawSpriteBatch(
    InternalSpriteBatch raw,
    InternalTexture rawTexture,
    const glm::mat4& positionTransform,
    const engine::Color4F& globalColor,
    bool blending,
    u32 count
) {
    auto batch = (SpriteBatchContainer*)raw;
    if (batch->capacity == 0)
        return;

    if (batch->dirtyType == SpriteBatchDirtyType::Set) {
        auto cbuf = gpu::acquireCommandBuffer();

        for (u32 index : batch->dirtySpriteSet) {
            gpu::uploadBufferData(
                cbuf,
                batch->vertexUploadBuffer,
                batch->vertexBuffer,
                sizeof(SpriteQuadVertexSet),
                sizeof(SpriteQuadVertexSet) * index
            );
        }

        gpu::submit(cbuf);
    } else if (batch->dirtyType == SpriteBatchDirtyType::Range) {
        gpu::uploadBufferData(
            nullptr,
            batch->vertexUploadBuffer,
            batch->vertexBuffer,
            sizeof(SpriteQuadVertexSet) * (batch->dirtySpriteRangeEnd - batch->dirtySpriteRangeStart),
            sizeof(SpriteQuadVertexSet) * batch->dirtySpriteRangeStart
        );
    }

    auto texture = (TextureContainer*)rawTexture;

    SDL_BindGPUGraphicsPipeline(renderPass_, spriteBatchPipeline_.getWithAddBlend(blending));

    SDL_GPUBufferBinding vertexBinding{};
    vertexBinding.buffer = batch->vertexBuffer;
    SDL_GPUBufferBinding indexBinding{};
    indexBinding.buffer = batch->indexBuffer;

    SDL_GPUTextureSamplerBinding textureBinding{};
    textureBinding.texture = texture->texture;
    textureBinding.sampler = gpu::fetchSampler(spriteBatchWrapParams);

    if (!textureBinding.sampler)
        return;

    SpriteBatchPropertiesUBO ubo = { positionTransform, Color4F::toVector(globalColor) };
    
    SDL_PushGPUVertexUniformData(commandBuffer_, UNIFORM_SLOT_SPRITE_BATCH_PROPERTIES, &ubo, sizeof(ubo));

    SDL_BindGPUVertexBuffers(renderPass_, 0, &vertexBinding, 1);
    SDL_BindGPUIndexBuffer(renderPass_, &indexBinding, SDL_GPU_INDEXELEMENTSIZE_32BIT);
    SDL_BindGPUFragmentSamplers(renderPass_, 0, &textureBinding, 1);
    SDL_DrawGPUIndexedPrimitives(renderPass_, count * 6, 1, 0, 0, 0);
}

void SDLGPUGraphics::drawFilledCircle(const glm::mat4 &positionTransform, const Color4F &color, u32 segments, bool blending)
{
    assert(commandBuffer_);

    // Cache buffer per segment count lazily
    SDL_GPUBuffer* buffer = nullptr;

    if (!circleBuffers_.contains(segments)) {
        auto points = computeCirclePoints(segments);
        buffer = gpu::createStaticGPUBuffer(points.size() * sizeof(Point), SDL_GPU_BUFFERUSAGE_VERTEX, (void*)points.data());

        circleBuffers_[segments] = buffer; // Cache it once created
    }
    else buffer = circleBuffers_[segments];

    if (!buffer)
        return;

    SDL_GPUBufferBinding vertexBinding{};
    vertexBinding.buffer = buffer;

    MeshUBO ubo{ positionTransform, Color4F::toVector(color) };

    SDL_PushGPUVertexUniformData(commandBuffer_, UNIFORM_SLOT_MESH_UBO, &ubo, sizeof(ubo));
    SDL_BindGPUGraphicsPipeline(renderPass_, circlePipeline_.getWithAddBlend(blending));
    
    SDL_BindGPUVertexBuffers(renderPass_, 0, &vertexBinding, 1);
    SDL_DrawGPUPrimitives(renderPass_, segments, 1, 0, 0);
}

OutlineMeshBuffers SDLGPUGraphics::createOutlineCircleBuffers(u32 segments) {
    OutlineMesh mesh = computeOutlineCirclePoints(segments);

    SDL_GPUBuffer* vertexBuffer = gpu::createStaticGPUBuffer(
        mesh.vertices.size() * sizeof(OutlineVertex),
        SDL_GPU_BUFFERUSAGE_VERTEX,
        mesh.vertices.data()
    );

    SDL_GPUBuffer* indexBuffer = gpu::createStaticGPUBuffer(
        mesh.indices.size() * sizeof(u32),
        SDL_GPU_BUFFERUSAGE_INDEX,
        mesh.indices.data()
    );

    return { vertexBuffer, indexBuffer, (u32)mesh.indices.size() };
}

struct OutlineCircleUBO {
    std140_mat4 positionTransform;
    std140_vec4 color;
    float lineWidth;
};

void SDLGPUGraphics::drawOutlineCircle(const glm::mat4 &positionTransform, const engine::Color4F &color, engine::u32 segments, engine::u32 lineWidthPx, bool blending) {
    assert(commandBuffer_ && Director::get() != nullptr);

    // Cache buffer per segment count lazily
    OutlineMeshBuffers buffers{};

    if (!outlineCircleBuffers_.contains(segments)) {
        buffers = createOutlineCircleBuffers(segments);

        outlineCircleBuffers_[segments] = buffers; // Cache it once created
    }
    else buffers = outlineCircleBuffers_[segments];

    float lineWidthPhysicalPixels = lineWidthPx / Director::get()->getScreenScale();

    OutlineCircleUBO ubo{ positionTransform, Color4F::toVector(color), lineWidthPhysicalPixels };

    SDL_BindGPUGraphicsPipeline(renderPass_, outlineCirclePipeline_.getWithAddBlend(blending));
    SDL_PushGPUVertexUniformData(commandBuffer_, UNIFORM_SLOT_OUTLINE_CIRCLE_UBO, &ubo, sizeof(ubo));

    SDL_GPUBufferBinding indexBinding{};
    indexBinding.buffer = buffers.indexBuffer;
    indexBinding.offset = 0;

    SDL_GPUBufferBinding vertexBinding{};
    vertexBinding.buffer = buffers.vertexBuffer;

    SDL_BindGPUIndexBuffer(renderPass_, &indexBinding, SDL_GPU_INDEXELEMENTSIZE_32BIT);
    SDL_BindGPUVertexBuffers(renderPass_, 0, &vertexBinding, 1);

    SDL_DrawGPUIndexedPrimitives(renderPass_, buffers.indexCount, 1, 0, 0, 0);
}

struct SpritePropertiesUBO {
    std140_mat4 positionTransform;
    std140_mat3 textureTransform;
    std140_vec4 color;
};

void SDLGPUGraphics::drawSprite(
    InternalTexture raw,
    const glm::mat4& positionTransform,
    const glm::mat3& textureTransform,
    const Color4F& color,
    const TextureWrapParameters& wrapParams,
    engine::BlendMode blendMode
) {
    assert(commandBuffer_);

    auto texture = (TextureContainer*)raw;

    SpritePropertiesUBO ubo = { positionTransform, textureTransform, Color4F::toVector(color) };

    SDL_PushGPUVertexUniformData(commandBuffer_, UNIFORM_SLOT_SPRITE_PROPERTIES, &ubo, sizeof(ubo));
    SDL_BindGPUGraphicsPipeline(renderPass_, defaultSpritePipeline_.getWithBlendMode(blendMode));

    SDL_GPUBufferBinding vertexBinding{};
    vertexBinding.buffer = quadVertexBuffer_;

    SDL_GPUTextureSamplerBinding textureBinding{};
    textureBinding.texture = texture->texture;
    textureBinding.sampler = gpu::fetchSampler(wrapParams);

    if (!textureBinding.sampler)
        return;

    SDL_BindGPUVertexBuffers(renderPass_, 0, &vertexBinding, 1);
    SDL_BindGPUFragmentSamplers(renderPass_, 0, &textureBinding, 1);
    SDL_DrawGPUPrimitives(renderPass_, 4, 1, 0, 0);
}

SDLGPUGraphics* SDLGPUGraphics::create(SDL_Window* window, GraphicsLibrary lib) {
    SDL_GPUShaderFormat format;
    const char* api;
    switch (lib) {
    case GraphicsLibrary::Vulkan:
        format = SDL_GPU_SHADERFORMAT_SPIRV;
        api = "vulkan";
        break;
    case GraphicsLibrary::Metal:
        format = SDL_GPU_SHADERFORMAT_MSL;
        api = "metal";
        break;
    case GraphicsLibrary::Direct3D12:
        format = SDL_GPU_SHADERFORMAT_DXIL;
        api = "direct3d12";
        break;
    default:
        assert(false && "invalid graphics api");
    }

    SDL_GPUDevice* device = SDL_CreateGPUDevice(format, true, api);
    if (!device) {
        log::err("Failed to create SDL_GPUDevice: {}", SDL_GetError());
        SDL_DestroyGPUDevice(device);
        return nullptr;
    }

    if (!SDL_ClaimWindowForGPUDevice(device, window)) {
        log::err("Failed to claim window for gpu device: {}", SDL_GetError());
        SDL_DestroyGPUDevice(device);
        return nullptr;
    }

    auto graphics = new SDLGPUGraphics;

    graphics->graphicsLibrary_ = lib;
    graphics->window_ = window;
    graphics->device_ = device;

    if (!graphics->init()) {
        delete graphics;
        return nullptr;
    }

    return graphics;
}

bool SDLGPUGraphics::init() {
    gpu::init(window_, device_, graphicsLibrary_);

    if (!setupPipelines()) {
        log::err("failed to setup graphics pipelines");
        return false;
    }

    quadVertexBuffer_ = gpu::createStaticGPUBuffer(sizeof(quadVerticies), SDL_GPU_BUFFERUSAGE_VERTEX, (void*)quadVerticies);
    if (!quadVertexBuffer_) {
        log::err("failed to create vertex buffer");
        return false;
    }

    // Disable v-sync
    if (SDL_WindowSupportsGPUPresentMode(device_, window_, SDL_GPU_PRESENTMODE_MAILBOX))
        SDL_SetGPUSwapchainParameters(device_, window_, SDL_GPU_SWAPCHAINCOMPOSITION_SDR, SDL_GPU_PRESENTMODE_MAILBOX);

    return true;
}

bool SDLGPUGraphics::setupPipelines() {
    SDL_GPUShader* batchVertexShader           = gpu::loadShader("spriteBatch.vert",   SDL_GPU_SHADERSTAGE_VERTEX,   0, 2);
    SDL_GPUShader* spriteVertexShader          = gpu::loadShader("sprite.vert",        SDL_GPU_SHADERSTAGE_VERTEX,   0, 2);
    SDL_GPUShader* spriteFragmentShader        = gpu::loadShader("sprite.frag",        SDL_GPU_SHADERSTAGE_FRAGMENT, 1, 0);
    SDL_GPUShader* solidMeshVertexShader       = gpu::loadShader("solidMesh.vert",     SDL_GPU_SHADERSTAGE_VERTEX,   0, 2);
    SDL_GPUShader* solidMeshFragmentShader     = gpu::loadShader("solidMesh.frag",     SDL_GPU_SHADERSTAGE_FRAGMENT, 0, 0);
    SDL_GPUShader* circleVertexShader          = gpu::loadShader("circle.vert",        SDL_GPU_SHADERSTAGE_VERTEX,   0, 2);
    SDL_GPUShader* textureMeshVertexShader     = gpu::loadShader("textureMesh.vert",   SDL_GPU_SHADERSTAGE_VERTEX,   0, 2);
    SDL_GPUShader* textureMeshFragmentShader   = gpu::loadShader("textureMesh.frag",   SDL_GPU_SHADERSTAGE_FRAGMENT, 1, 0);
    SDL_GPUShader* outlineCircleVertexShader   = gpu::loadShader("outlineCircle.vert", SDL_GPU_SHADERSTAGE_VERTEX,   0, 2);
    SDL_GPUShader* outlineCircleFragmentShader = gpu::loadShader("outlineCircle.frag", SDL_GPU_SHADERSTAGE_FRAGMENT, 0, 2);

    if (gpu::hasFailedLoadingShaders()) {
        gpu::releaseAllShaders();
        return false;
    }

    defaultSpritePipeline_ = gpu::createPipelines({
        .primitive = Primitive::TriangleStrip,
        .attributes = {
            {0, VertexFormat::Float2}
        },
        .vertexShader = spriteVertexShader,
        .fragmentShader = spriteFragmentShader,
        .includeMultiplicative = true
    });

    spriteBatchPipeline_ = gpu::createPipelines({
        .primitive = Primitive::TriangleList,
        .attributes = {
            {0, VertexFormat::Float2},
            {1, VertexFormat::Float2},
            {2, VertexFormat::Float4},
        },
        .vertexShader = batchVertexShader,
        .fragmentShader = spriteFragmentShader
    });

    circlePipeline_ = gpu::createPipelines({
        .primitive = Primitive::TriangleStrip,
        .attributes = {
            {0, VertexFormat::Float2}
        },
        .vertexShader = circleVertexShader,
        .fragmentShader = solidMeshFragmentShader
    });

    outlineCirclePipeline_ = gpu::createPipelines({
        .primitive = Primitive::TriangleList,
        .attributes = {
            { 0, VertexFormat::Float2 }, // a_segStart
            { 1, VertexFormat::Float2 }, // a_segEnd
            { 2, VertexFormat::Float  }, // a_side
            { 3, VertexFormat::Float  }, // a_endpoint
        },
        .vertexShader = outlineCircleVertexShader,
        .fragmentShader = outlineCircleFragmentShader
    });

    solidMeshPipeline_ = gpu::createPipelines({
        .primitive = Primitive::TriangleList,
        .attributes = {
            {0, VertexFormat::Float2},
            {1, VertexFormat::UByte4_Norm},
            {2, VertexFormat::Float2}
        },
        .vertexShader = solidMeshVertexShader,
        .fragmentShader = solidMeshFragmentShader
    });

    textureMeshPipeline_ = gpu::createPipelines({
        .primitive = Primitive::TriangleList,
        .attributes = {
            {0, VertexFormat::Float2},
            {1, VertexFormat::UByte4_Norm},
            {2, VertexFormat::Float2}
        },
        .vertexShader = textureMeshVertexShader,
        .fragmentShader = textureMeshFragmentShader
    });

    gpu::releaseAllShaders();
    return !gpu::hasFailedLoadingPipelines();
}

};