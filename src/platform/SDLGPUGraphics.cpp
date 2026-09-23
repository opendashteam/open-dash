#include "SDLGPUGraphics.h"
#include "../engine/utilities/log.h"
#include <glm/glm.hpp>

using namespace opendash::engine;

namespace opendash::platform
{

SDLGPUGraphics::~SDLGPUGraphics()
{
    assert(numTexturesAllocated == 0);

    if (defaultSpritePipeline_)
        SDL_ReleaseGPUGraphicsPipeline(device_, defaultSpritePipeline_);
    if (quadVertexBuffer_)
        SDL_ReleaseGPUBuffer(device_, quadVertexBuffer_);

    SDL_ReleaseWindowFromGPUDevice(device_, window_);
    SDL_DestroyGPUDevice(device_);
}

bool SDLGPUGraphics::beginDraw(const Color4F& clearColor, Size& windowSizeOut)
{
    assert(commandBuffer_ == nullptr);

    commandBuffer_ = SDL_AcquireGPUCommandBuffer(device_);

    u32 w, h;
    SDL_WaitAndAcquireGPUSwapchainTexture(commandBuffer_, window_, &swapchainTexture_, &w, &h);

    windowSizeOut = {(float)w, (float)h};

    if (!swapchainTexture_) {
        SDL_CancelGPUCommandBuffer(commandBuffer_);
        commandBuffer_ = nullptr;
        return false;
    }

    SDL_GPUColorTargetInfo colorTargetInfo{};
    colorTargetInfo.texture = swapchainTexture_;
    colorTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
    colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;

    const auto& cc = clearColor;
    colorTargetInfo.clear_color = {cc.r, cc.g, cc.b, cc.a};

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

struct InternalTextureContainer {
    SDL_GPUTexture* texture;
    SDL_GPUSampler* sampler;
    u32 width, height;
};

InternalTexture SDLGPUGraphics::createTexture(
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

    SDL_GPUTransferBufferCreateInfo bufferTransferInfo{};
    bufferTransferInfo.size = width * height * 4;
    bufferTransferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
    SDL_GPUTransferBuffer* texTransfer = SDL_CreateGPUTransferBuffer(device_, &bufferTransferInfo);

    void* texMapped = SDL_MapGPUTransferBuffer(device_, texTransfer, false);
    SDL_memcpy(texMapped, data, width * height * 4);
    SDL_UnmapGPUTransferBuffer(device_, texTransfer);

    SDL_GPUCommandBuffer* uploadCmd = SDL_AcquireGPUCommandBuffer(device_);
    SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(uploadCmd);

    SDL_GPUTextureTransferInfo texTransferInfo{};
    texTransferInfo.transfer_buffer = texTransfer;

    SDL_GPUTextureRegion texRegion{};
    texRegion.texture = texture;
    texRegion.w = width;
    texRegion.h = height;
    texRegion.d = 1;

    SDL_UploadToGPUTexture(copyPass, &texTransferInfo, &texRegion, false);

    SDL_EndGPUCopyPass(copyPass);
    SDL_SubmitGPUCommandBuffer(uploadCmd);

    SDL_ReleaseGPUTransferBuffer(device_, texTransfer);

    SDL_GPUSamplerCreateInfo samplerInfo{};
    samplerInfo.min_filter = SDL_GPU_FILTER_LINEAR;
    samplerInfo.mag_filter = SDL_GPU_FILTER_LINEAR;
    samplerInfo.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_LINEAR;
    samplerInfo.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    samplerInfo.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;

    SDL_GPUSampler* sampler = SDL_CreateGPUSampler(device_, &samplerInfo);
    if (!sampler) {
        SDL_Log("Could not cache texture, sampler creation failed: %s", SDL_GetError());
        SDL_ReleaseGPUTexture(device_, texture);
        return nullptr;
    }

    numTexturesAllocated++;
    return (InternalTexture)new InternalTextureContainer { texture, sampler, width, height };
}

void SDLGPUGraphics::destroyTexture(InternalTexture raw)
{
    numTexturesAllocated--;

    auto texture = (InternalTextureContainer*)raw;

    SDL_ReleaseGPUSampler(device_, texture->sampler);
    SDL_ReleaseGPUTexture(device_, texture->texture);

    delete texture;
}

struct VertexUBO {
    glm::mat4 positionTransform;
    Color4F color;
};

void SDLGPUGraphics::drawSprite(
    InternalTexture raw,
    glm::mat4 positionTransform,
    const Rect& textureFrame, // texture frame rect is in pixels
    const Color4F& color
)
{
    assert(commandBuffer_);

    auto texture = (InternalTextureContainer*)raw;

    VertexUBO ubo = { positionTransform, color };

    SDL_PushGPUVertexUniformData(commandBuffer_, 0, &ubo, sizeof(ubo));
    SDL_BindGPUGraphicsPipeline(renderPass_, defaultSpritePipeline_);

    SDL_GPUBufferBinding vertexBinding{};
    vertexBinding.buffer = quadVertexBuffer_;

    SDL_GPUTextureSamplerBinding textureBinding{};
    textureBinding.texture = texture->texture;
    textureBinding.sampler = texture->sampler;

    SDL_BindGPUVertexBuffers(renderPass_, 0, &vertexBinding, 1);
    SDL_BindGPUFragmentSamplers(renderPass_, 0, &textureBinding, 1);
    SDL_DrawGPUPrimitives(renderPass_, 4, 1, 0, 0);
}

SDLGPUGraphics* SDLGPUGraphics::create(SDL_Window* window)
{
    SDL_GPUDevice* device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, true, nullptr);
    if (!device)
    {
        log::err("failed to create SDL_GPUDevice");
        return nullptr;
    }

    if (!SDL_ClaimWindowForGPUDevice(device, window))
    {
        log::err("failed to claim window for gpu device");
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

static const float quadVerticies[] = {
    -0.5f, -0.5f,
    -0.5f,  0.5f,
     0.5f, -0.5f,
     0.5f,  0.5f
};

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
        return nullptr;
    }
    SDL_free(code);
    return shader;
}

bool SDLGPUGraphics::setupPipelines()
{
    SDL_GPUShader* vertexShader   = loadShader("assets/shaders/sprite.vert.spv", SDL_GPU_SHADERSTAGE_VERTEX,   0, 1);
    SDL_GPUShader* fragmentShader = loadShader("assets/shaders/sprite.frag.spv", SDL_GPU_SHADERSTAGE_FRAGMENT, 1, 0);

    if (!vertexShader || !fragmentShader) {
        if (vertexShader)
            SDL_ReleaseGPUShader(device_, vertexShader);
        if (fragmentShader)
            SDL_ReleaseGPUShader(device_, fragmentShader);
        
        return false;
    }

    SDL_GPUVertexBufferDescription vertexBufferDesc{};
    vertexBufferDesc.slot = 0;
    vertexBufferDesc.input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;
    vertexBufferDesc.pitch = sizeof(float) * 2;

    SDL_GPUVertexAttribute attribute;
    attribute.buffer_slot = 0;
    attribute.location = 0;
    attribute.format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2;
    attribute.offset = 0;

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
    pipelineInfo.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLESTRIP;
    pipelineInfo.vertex_input_state.num_vertex_buffers = 1;
    pipelineInfo.vertex_input_state.vertex_buffer_descriptions = &vertexBufferDesc;
    pipelineInfo.vertex_input_state.num_vertex_attributes = 1;
    pipelineInfo.vertex_input_state.vertex_attributes = &attribute;
    pipelineInfo.target_info.num_color_targets = 1;
    pipelineInfo.target_info.color_target_descriptions = &colorTarget;

    defaultSpritePipeline_ = SDL_CreateGPUGraphicsPipeline(device_, &pipelineInfo);
    if (!defaultSpritePipeline_)
        log::err("failed to create graphics pipeline: {}", SDL_GetError());

    SDL_ReleaseGPUShader(device_, vertexShader);
    SDL_ReleaseGPUShader(device_, fragmentShader);

    return defaultSpritePipeline_ != nullptr;
}

SDL_GPUBuffer* SDLGPUGraphics::createStaticGPUBuffer(u32 size, SDL_GPUBufferUsageFlags usage, void* data) {
    SDL_GPUCommandBuffer* uploadCmd = SDL_AcquireGPUCommandBuffer(device_);
    SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(uploadCmd);

    SDL_GPUBufferCreateInfo bufferInfo{};
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    SDL_GPUBuffer* buffer = SDL_CreateGPUBuffer(device_, &bufferInfo);
    if (!buffer)
        return nullptr;

    SDL_GPUTransferBufferCreateInfo vbTransferInfo{};
    vbTransferInfo.size = size;
    vbTransferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
    SDL_GPUTransferBuffer* vbTransfer = SDL_CreateGPUTransferBuffer(device_, &vbTransferInfo);
    if (!vbTransfer) {
        SDL_ReleaseGPUBuffer(device_, buffer);
        return nullptr;
    }

    void* vbMapped = SDL_MapGPUTransferBuffer(device_, vbTransfer, false);
    SDL_memcpy(vbMapped, data, size);
    SDL_UnmapGPUTransferBuffer(device_, vbTransfer);

    SDL_GPUTransferBufferLocation vbLocation{};
    vbLocation.transfer_buffer = vbTransfer;
    SDL_GPUBufferRegion vbRegion{};
    vbRegion.buffer = buffer;
    vbRegion.size = size;

    SDL_UploadToGPUBuffer(copyPass, &vbLocation, &vbRegion, false);
    SDL_EndGPUCopyPass(copyPass);
    SDL_SubmitGPUCommandBuffer(uploadCmd);
    SDL_ReleaseGPUTransferBuffer(device_, vbTransfer);

    return buffer;
}

};