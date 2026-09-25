#include "SDLGPUGraphics.h"
#include "engine/utilities/log.h"
#include <glm/glm.hpp>
#include <set>
#include "../../../assets/shaders/common.h"

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

    for (const auto& [_, sampler] : samplers_)
        SDL_ReleaseGPUSampler(device_, sampler);

    if (defaultSpritePipeline_)
        release(defaultSpritePipeline_);
    if (spriteBatchPipeline_)
        release(spriteBatchPipeline_);
    if (quadVertexBuffer_)
        release(quadVertexBuffer_);

    SDL_ReleaseWindowFromGPUDevice(device_, window_);
    SDL_DestroyGPUDevice(device_);
}

bool SDLGPUGraphics::beginDraw()
{
    assert(commandBuffer_ == nullptr);

    commandBuffer_ = SDL_AcquireGPUCommandBuffer(device_);

    u32 w, h;
    SDL_WaitAndAcquireGPUSwapchainTexture(commandBuffer_, window_, &swapchainTexture_, &w, &h);

    if (!swapchainTexture_) {
        SDL_CancelGPUCommandBuffer(commandBuffer_);
        commandBuffer_ = nullptr;
        return false;
    }

    SDL_GPUColorTargetInfo colorTargetInfo{};
    colorTargetInfo.texture = swapchainTexture_;
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
    SDL_SubmitGPUCommandBuffer(commandBuffer_);
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
)
{
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

    auto uploadBuffer = createGPUUploadBuffer(width * height * 4, data);   
    if (!uploadBuffer) {
        release(texture);
        return nullptr;
    }

    SDL_GPUCommandBuffer* uploadCmd = SDL_AcquireGPUCommandBuffer(device_);
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
    SDL_SubmitGPUCommandBuffer(uploadCmd);

    release(uploadBuffer);

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

engine::InternalSpriteBatch SDLGPUGraphics::spriteBatchCreate() {
    return (engine::InternalSpriteBatch)new SpriteBatchContainer;
}

void SDLGPUGraphics::spriteBatchResize(engine::InternalSpriteBatch raw, engine::u32 capacity) {
    auto batch = (SpriteBatchContainer*)raw;
    if (capacity <= batch->capacity)
        return;

    auto newVertexBuffer = createGPUBuffer(sizeof(SpriteQuadVertexSet) * capacity, SDL_GPU_BUFFERUSAGE_VERTEX);
    auto newIndexBuffer  = createGPUBuffer(sizeof(SpriteQuadIndexSet) * capacity,  SDL_GPU_BUFFERUSAGE_INDEX);

    if (batch->capacity > 0) {
        auto cbuf = acquireCommandBuffer();

        copyBuffer(
            cbuf,
            batch->vertexBuffer,
            newVertexBuffer,
            sizeof(SpriteQuadVertexSet) * batch->capacity,
            0, 0
        );
        copyBuffer(
            cbuf,
            batch->indexBuffer,
            newIndexBuffer,
            sizeof(SpriteQuadIndexSet) * batch->capacity,
            0, 0
        );

        submit(cbuf);

        unmapBuffer(batch->vertexUploadBuffer);
        release(batch->vertexUploadBuffer);
        release(batch->vertexBuffer);
        release(batch->indexBuffer);
    }

    u32 newIndiciesSize = (capacity - batch->capacity) * sizeof(SpriteQuadIndexSet);
    auto indexUploadBuffer = createGPUUploadBuffer(newIndiciesSize);
    auto indicies = (SpriteQuadIndexSet*)mapBuffer(indexUploadBuffer);

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

    unmapBuffer(indexUploadBuffer);
    uploadBufferData(NULL, indexUploadBuffer, newIndexBuffer, newIndiciesSize, 0, batch->capacity * sizeof(SpriteQuadIndexSet));
    release(indexUploadBuffer);

    batch->vertexUploadBuffer = createGPUUploadBuffer(sizeof(SpriteQuadVertexSet) * capacity);
    batch->vertexBufferMapped = (SpriteQuadVertexSet*)mapBuffer(batch->vertexUploadBuffer);
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

void SDLGPUGraphics::spriteBatchDestroy(engine::InternalSpriteBatch raw) {
    auto batch = (SpriteBatchContainer*)raw;
    if (batch->capacity == 0)
        return;

    unmapBuffer(batch->vertexUploadBuffer);
    release(batch->vertexUploadBuffer);
    release(batch->vertexBuffer);
    release(batch->indexBuffer);
}

static TextureWrapParameters spriteBatchWrapParams = {
    WrapMode::Clamp,
    WrapMode::Clamp
};

void SDLGPUGraphics::drawSpriteBatch(
    InternalSpriteBatch raw,
    InternalTexture rawTexture,
    const glm::mat4& positionTransform,
    u32 count
) {
    auto batch = (SpriteBatchContainer*)raw;
    if (batch->capacity == 0)
        return;

    if (batch->dirtyType == SpriteBatchDirtyType::Set) {
        auto cbuf = acquireCommandBuffer();

        for (u32 index : batch->dirtySpriteSet) {
            uploadBufferData(
                cbuf,
                batch->vertexUploadBuffer,
                batch->vertexBuffer,
                sizeof(SpriteQuadVertexSet),
                sizeof(SpriteQuadVertexSet) * index
            );
        }

        submit(cbuf);
    } else if (batch->dirtyType == SpriteBatchDirtyType::Range) {
        uploadBufferData(
            nullptr,
            batch->vertexUploadBuffer,
            batch->vertexBuffer,
            sizeof(SpriteQuadVertexSet) * (batch->dirtySpriteRangeEnd - batch->dirtySpriteRangeStart),
            sizeof(SpriteQuadVertexSet) * batch->dirtySpriteRangeStart
        );
    }

    auto texture = (TextureContainer*)rawTexture;

    SDL_BindGPUGraphicsPipeline(renderPass_, spriteBatchPipeline_);

    SDL_GPUBufferBinding vertexBinding{};
    vertexBinding.buffer = batch->vertexBuffer;
    SDL_GPUBufferBinding indexBinding{};
    indexBinding.buffer = batch->indexBuffer;

    SDL_GPUTextureSamplerBinding textureBinding{};
    textureBinding.texture = texture->texture;
    textureBinding.sampler = fetchSampler(spriteBatchWrapParams);

    if (!textureBinding.sampler)
        return;
    
    SDL_PushGPUVertexUniformData(commandBuffer_, UNIFORM_SLOT_SPRITE_BATCH_PROPERTIES, &positionTransform, sizeof(positionTransform));

    SDL_BindGPUVertexBuffers(renderPass_, 0, &vertexBinding, 1);
    SDL_BindGPUIndexBuffer(renderPass_, &indexBinding, SDL_GPU_INDEXELEMENTSIZE_32BIT);
    SDL_BindGPUFragmentSamplers(renderPass_, 0, &textureBinding, 1);
    SDL_DrawGPUIndexedPrimitives(renderPass_, count * 6, 1, 0, 0, 0);
}

struct SpritePropertiesUBO {
    std140_mat4 positionTransform;
    std140_mat3 textureTransform;
    std140_vec4 color;
};

void SDLGPUGraphics::drawSprite(
    engine::InternalTexture raw,
    const glm::mat4& positionTransform,
    const glm::mat3& textureTransform,
    const engine::Color4F& color,
    const engine::TextureWrapParameters& wrapParams
)
{
    assert(commandBuffer_);

    auto texture = (TextureContainer*)raw;

    glm::vec4 colorVector = {color.r, color.g, color.b, color.a};

    SpritePropertiesUBO ubo = { positionTransform, textureTransform, colorVector };

    SDL_PushGPUVertexUniformData(commandBuffer_, UNIFORM_SLOT_SPRITE_PROPERTIES, &ubo, sizeof(ubo));
    SDL_BindGPUGraphicsPipeline(renderPass_, defaultSpritePipeline_);

    SDL_GPUBufferBinding vertexBinding{};
    vertexBinding.buffer = quadVertexBuffer_;

    SDL_GPUTextureSamplerBinding textureBinding{};
    textureBinding.texture = texture->texture;
    textureBinding.sampler = fetchSampler(wrapParams);

    if (!textureBinding.sampler)
        return;

    SDL_BindGPUVertexBuffers(renderPass_, 0, &vertexBinding, 1);
    SDL_BindGPUFragmentSamplers(renderPass_, 0, &textureBinding, 1);
    SDL_DrawGPUPrimitives(renderPass_, 4, 1, 0, 0);
}

SDLGPUGraphics* SDLGPUGraphics::create(SDL_Window* window, GraphicsLibrary lib)
{
    const char* api;
    switch (lib) {
    case GraphicsLibrary::Vulkan: api = "vulkan"; break;
    case GraphicsLibrary::Metal: api = "metal"; break;
    case GraphicsLibrary::Direct3D12: api = "direct3d12"; break;
    default:
        assert(false && "invalid graphics api");
    }

    SDL_GPUDevice* device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, true, api);
    if (!device)
    {
        log::err("Failed to create SDL_GPUDevice: {}", SDL_GetError());
        SDL_DestroyGPUDevice(device);
        return nullptr;
    }

    if (!SDL_ClaimWindowForGPUDevice(device, window))
    {
        log::err("Failed to claim window for gpu device: {}", SDL_GetError());
        SDL_DestroyGPUDevice(device);
        return nullptr;
    }

    auto graphics = new SDLGPUGraphics;

    graphics->window_ = window;
    graphics->device_ = device;

    if (!graphics->init()) {
        delete graphics;
        return nullptr;
    }

    return graphics;
}

bool SDLGPUGraphics::init()
{
    if (!setupPipelines()) {
        log::err("failed to setup graphics pipelines");
        return false;
    }

    quadVertexBuffer_ = createStaticGPUBuffer(sizeof(quadVerticies), SDL_GPU_BUFFERUSAGE_VERTEX, (void*)quadVerticies);
    if (!quadVertexBuffer_) {
        log::err("failed to create vertex buffer");
        return false;
    }

    // Disable v-sync
    if (SDL_WindowSupportsGPUPresentMode(device_, window_, SDL_GPU_PRESENTMODE_MAILBOX)) {
        SDL_SetGPUSwapchainParameters(device_, window_, SDL_GPU_SWAPCHAINCOMPOSITION_SDR, SDL_GPU_PRESENTMODE_MAILBOX);
    }

    return true;
}

SDL_GPUShader* SDLGPUGraphics::loadShader(const char* path, SDL_GPUShaderStage stage, u32 numSamplers, u32 numUniformBuffers)
{
    size_t codeSize;
    void* code = SDL_LoadFile(path, &codeSize);
    if (!code)
    {
        log::err("failed to load shader {}: {}", path, SDL_GetError());
        return nullptr;
    }

    SDL_GPUShaderCreateInfo info{};
    info.code = (u8*)code;
    info.code_size = codeSize;
    info.entrypoint = "main";
    info.format = SDL_GPU_SHADERFORMAT_SPIRV;
    info.stage = stage;
    info.num_samplers = numSamplers;
    info.num_uniform_buffers = numUniformBuffers;

    SDL_GPUShader* shader = SDL_CreateGPUShader(device_, &info);
    if (!shader)
    {
        log::err("failed to compile shader {}:\n{}", path, SDL_GetError());
        SDL_free(code);
        return nullptr;
    }
    SDL_free(code);

    shaders_.push_back(shader);
    return shader;
}

static u32 getVertexFormatType(SDL_GPUVertexElementFormat format) {
    switch (format) {
    case SDL_GPU_VERTEXELEMENTFORMAT_FLOAT:  return sizeof(float) * 1;
    case SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2: return sizeof(float) * 2;
    case SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3: return sizeof(float) * 3;
    case SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4: return sizeof(float) * 4;
    default:
        assert(false && "vertex format unknown");
    }
    return 0;
}

SDL_GPUGraphicsPipeline* SDLGPUGraphics::createGraphicsPipeline(
    SDL_GPUPrimitiveType primitive,
    const std::vector<VertexAttribute>& attributes,
    SDL_GPUShader* vertexShader,
    SDL_GPUShader* fragmentShader
) {
    u32 pitch = 0;
    for (const auto& attrib : attributes)
        pitch += getVertexFormatType(attrib.type);

    SDL_GPUVertexBufferDescription vertexBufferDesc{};
    vertexBufferDesc.slot = 0;
    vertexBufferDesc.input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;
    vertexBufferDesc.pitch = pitch;

    u32 offset = 0;

    std::vector<SDL_GPUVertexAttribute> rawAttributes;
    for (const auto& attrib : attributes) {
        rawAttributes.push_back({});
        auto& raw = rawAttributes.back();
        raw.buffer_slot = 0;
        raw.location = attrib.location;
        raw.format = attrib.type;
        raw.offset = offset;

        offset += getVertexFormatType(raw.format);
    }

    SDL_GPUColorTargetDescription colorTarget{};
    colorTarget.format = SDL_GetGPUSwapchainTextureFormat(device_, window_);
    colorTarget.blend_state.enable_blend = true;
    colorTarget.blend_state.color_blend_op = SDL_GPU_BLENDOP_ADD;
    colorTarget.blend_state.alpha_blend_op = SDL_GPU_BLENDOP_ADD;
    colorTarget.blend_state.src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
    colorTarget.blend_state.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    colorTarget.blend_state.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
    colorTarget.blend_state.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;

    SDL_GPUGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.vertex_shader = vertexShader;
    pipelineInfo.fragment_shader = fragmentShader;
    pipelineInfo.primitive_type = primitive;
    pipelineInfo.vertex_input_state.num_vertex_buffers = 1;
    pipelineInfo.vertex_input_state.vertex_buffer_descriptions = &vertexBufferDesc;
    pipelineInfo.vertex_input_state.num_vertex_attributes = rawAttributes.size();
    pipelineInfo.vertex_input_state.vertex_attributes = rawAttributes.data();
    pipelineInfo.target_info.num_color_targets = 1;
    pipelineInfo.target_info.color_target_descriptions = &colorTarget;

    SDL_GPUGraphicsPipeline* pipeline = SDL_CreateGPUGraphicsPipeline(device_, &pipelineInfo);
    if (!pipeline)
        log::err("failed to create graphics pipeline: {}", SDL_GetError());
    return pipeline;
}

bool SDLGPUGraphics::setupPipelines()
{
    SDL_GPUShader* batchVertexShader  = loadShader("assets/shaders/spriteBatch.vert.spv", SDL_GPU_SHADERSTAGE_VERTEX,   0, 2);
    SDL_GPUShader* spriteVertexShader = loadShader("assets/shaders/sprite.vert.spv",      SDL_GPU_SHADERSTAGE_VERTEX,   0, 2);
    SDL_GPUShader* fragmentShader     = loadShader("assets/shaders/sprite.frag.spv",      SDL_GPU_SHADERSTAGE_FRAGMENT, 1, 0);

    if (
        !batchVertexShader ||
        !spriteVertexShader ||
        !fragmentShader
    ) {
        releaseAllShaders();
        return false;
    }

    defaultSpritePipeline_ = createGraphicsPipeline(
        SDL_GPU_PRIMITIVETYPE_TRIANGLESTRIP,
        { {0, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2} },
        spriteVertexShader,
        fragmentShader
    );

    spriteBatchPipeline_ = createGraphicsPipeline(
        SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
        {
            {0, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2},
            {1, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2},
            {2, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4},
        },
        batchVertexShader,
        fragmentShader
    );

    releaseAllShaders();
    return defaultSpritePipeline_ != nullptr || spriteBatchPipeline_ != nullptr;
}

SDL_GPUBuffer* SDLGPUGraphics::createGPUBuffer(engine::u32 size, SDL_GPUBufferUsageFlags usage) {
    SDL_GPUBufferCreateInfo info{};
    info.size = size;
    info.usage = usage;
    return SDL_CreateGPUBuffer(device_, &info);
}

SDL_GPUTransferBuffer* SDLGPUGraphics::createGPUUploadBuffer(engine::u32 size, void* data) {
    SDL_GPUTransferBufferCreateInfo info{};
    info.size = size;
    info.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
    SDL_GPUTransferBuffer* buffer = SDL_CreateGPUTransferBuffer(device_, &info);
    if (!buffer)
        return nullptr;

    if (data) {
        memcpy(SDL_MapGPUTransferBuffer(device_, buffer, false), data, size);
        SDL_UnmapGPUTransferBuffer(device_, buffer);
    }

    return buffer;
}

void SDLGPUGraphics::uploadBufferData(
    SDL_GPUCommandBuffer* cbuffer,
    SDL_GPUTransferBuffer* src,
    SDL_GPUBuffer* dst,
    engine::u32 size,
    engine::u32 srcOffset,
    engine::u32 dstOffset
) {
    bool ownCBuffer = cbuffer == nullptr;
    if (ownCBuffer)
        cbuffer = acquireCommandBuffer();

    SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(cbuffer);

    SDL_GPUTransferBufferLocation location{};
    location.transfer_buffer = src;
    location.offset = srcOffset;
    SDL_GPUBufferRegion region{};
    region.buffer = dst;
    region.size = size;
    region.offset = dstOffset;
    SDL_UploadToGPUBuffer(copyPass, &location, &region, false);
    SDL_EndGPUCopyPass(copyPass);

    if (ownCBuffer)
        submit(cbuffer);
}

void SDLGPUGraphics::copyBuffer(
    SDL_GPUCommandBuffer* cbuffer,
    SDL_GPUBuffer* src,
    SDL_GPUBuffer* dst,
    engine::u32 size,
    engine::u32 srcOffset,
    engine::u32 dstOffset
) {
    bool ownCBuffer = cbuffer == nullptr;
    if (ownCBuffer)
        cbuffer = acquireCommandBuffer();

    SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(cbuffer);
    SDL_GPUBufferLocation srcLocation{};
    srcLocation.buffer = src;
    srcLocation.offset = srcOffset;
    SDL_GPUBufferLocation dstLocation{};
    dstLocation.buffer = dst;
    dstLocation.offset = dstOffset;
    SDL_CopyGPUBufferToBuffer(copyPass, &srcLocation, &dstLocation, size, false);
    SDL_EndGPUCopyPass(copyPass);

    if (ownCBuffer)
        submit(cbuffer);
}

SDL_GPUBuffer* SDLGPUGraphics::createStaticGPUBuffer(u32 size, SDL_GPUBufferUsageFlags usage, void* data) {
    SDL_GPUBuffer* buffer = createGPUBuffer(size, usage);
    if (!buffer)
        return nullptr;

    SDL_GPUTransferBuffer* uploadBuffer = createGPUUploadBuffer(size, data);
    if (!uploadBuffer) {
        SDL_ReleaseGPUBuffer(device_, buffer);
        return nullptr;
    }

    uploadBufferData(NULL, uploadBuffer, buffer, size);
    release(uploadBuffer);
    return buffer;
}

static inline SDL_GPUSamplerAddressMode toSDLAddressMode(WrapMode mode) {
    switch (mode) {
    case WrapMode::Clamp: return SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    case WrapMode::Repeat: return SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
    case WrapMode::MirroredRepeat: return SDL_GPU_SAMPLERADDRESSMODE_MIRRORED_REPEAT;
    default:
        assert(false && "invalid WrapMode");
    }
    return SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
}

SDL_GPUSampler* SDLGPUGraphics::fetchSampler(const engine::TextureWrapParameters& params) {
    u32 code = params.asBitCode();
    auto it = samplers_.find(code);
    if (it != samplers_.end())
        return it->second;

    SDL_GPUSamplerCreateInfo samplerInfo{};
    samplerInfo.min_filter = SDL_GPU_FILTER_LINEAR;
    samplerInfo.mag_filter = SDL_GPU_FILTER_LINEAR;
    samplerInfo.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_LINEAR;
    samplerInfo.address_mode_u = toSDLAddressMode(params.u);
    samplerInfo.address_mode_v = toSDLAddressMode(params.v);

    SDL_GPUSampler* sampler = SDL_CreateGPUSampler(device_, &samplerInfo);
    if (!sampler) {
        SDL_Log("Could not create sampler: %s", SDL_GetError());
        return nullptr;
    }

    samplers_[code] = sampler;
    return sampler;
}

};