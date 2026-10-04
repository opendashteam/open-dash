#include "gpu.h"

#include "engine/utilities/log.h"
#include "engine/AssetManager.h"

using namespace opendash::engine;

namespace opendash::platform::gpu {

static SDL_Window* window;
SDL_GPUDevice* device = nullptr;

static bool allPipelinesSucceeded = true;
static bool allShadersSucceeded = true;

static std::vector<SDL_GPUGraphicsPipeline*> graphicsPipelines;
static std::vector<SDL_GPUShader*> shaders;

static GraphicsLibrary graphicsLibrary;
static SDL_GPUShaderFormat shaderFormat;

static std::unordered_map<engine::u32, SDL_GPUSampler*> samplers;

void init(
    SDL_Window* window_,
    SDL_GPUDevice* device_,
    GraphicsLibrary library_
) {
    window = window_;
    device = device_;
    graphicsLibrary = library_;

    switch (library_) {
    case GraphicsLibrary::Vulkan: shaderFormat = SDL_GPU_SHADERFORMAT_SPIRV; break;
    case GraphicsLibrary::Metal: shaderFormat = SDL_GPU_SHADERFORMAT_MSL; break;
    case GraphicsLibrary::Direct3D12: shaderFormat = SDL_GPU_SHADERFORMAT_DXIL; break;
    default:
        assert(false && "invalid graphics api");
    }
}

SDL_GPUShader* loadShader(const std::string& path, SDL_GPUShaderStage stage, u32 numSamplers, u32 numUniformBuffers) {
    std::string rawPath;
    switch (graphicsLibrary) {
        case GraphicsLibrary::Vulkan: rawPath = "shaders/spv/" + path + ".spv"; break;
        case GraphicsLibrary::Direct3D12: rawPath = "shaders/dxil/" + path + ".dxil"; break;
        case GraphicsLibrary::Metal: rawPath = "shaders/msl/" + path + ".msl"; break;
        default:
            assert(false && "invalid graphics library");
            return nullptr;
    }

    std::vector<u8> code;

    if (!AssetManager::get()->readFileAsBinaryData(rawPath, code)) {
        log::err("Failed to load shader {}", path);
        allShadersSucceeded = false;
        return nullptr;
    }

    SDL_GPUShaderCreateInfo info{};
    info.code = code.data();
    info.code_size = code.size();
    info.entrypoint = graphicsLibrary == GraphicsLibrary::Metal ? "main0" : "main";
    info.format = shaderFormat;
    info.stage = stage;
    info.num_samplers = numSamplers;
    info.num_uniform_buffers = numUniformBuffers;

    SDL_GPUShader* shader = SDL_CreateGPUShader(device, &info);
    if (!shader) {
        log::err("Failed to compile shader {}:\n{}", path, SDL_GetError());
        allShadersSucceeded = false;
        return nullptr;
    }

    shaders.push_back(shader);
    return shader;
}

static u32 getVertexFormatType(SDL_GPUVertexElementFormat format) {
    switch (format) {
        case SDL_GPU_VERTEXELEMENTFORMAT_FLOAT:  return sizeof(float) * 1;
        case SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2: return sizeof(float) * 2;
        case SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3: return sizeof(float) * 3;
        case SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4: return sizeof(float) * 4;
        case SDL_GPU_VERTEXELEMENTFORMAT_UBYTE4_NORM: return sizeof(u8) * 4;
        default:
            assert(false && "vertex format unknown");
    }
    return 0;
}

static SDL_GPUGraphicsPipeline* createGraphicsPipeline(
    SDL_GPUPrimitiveType primitive,
    const std::vector<VertexAttribute>& attributes,
    SDL_GPUShader* vertexShader,
    SDL_GPUShader* fragmentShader,
    engine::BlendMode blendMode
) {
    u32 pitch = 0;
    for (const auto& attrib : attributes)
        pitch += getVertexFormatType((SDL_GPUVertexElementFormat)attrib.type);

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
        raw.format = (SDL_GPUVertexElementFormat)attrib.type;
        raw.offset = offset;

        offset += getVertexFormatType(raw.format);
    }

    bool additiveBlending = blendMode == BlendMode::Additive;
    bool multiplicativeBlending = blendMode == BlendMode::Multiplicative;

    SDL_GPUColorTargetDescription colorTarget{};
    colorTarget.format = SDL_GetGPUSwapchainTextureFormat(device, window);
    colorTarget.blend_state.enable_blend = true;
    colorTarget.blend_state.color_blend_op = SDL_GPU_BLENDOP_ADD;
    colorTarget.blend_state.alpha_blend_op = SDL_GPU_BLENDOP_ADD;
    colorTarget.blend_state.src_color_blendfactor = multiplicativeBlending ? SDL_GPU_BLENDFACTOR_DST_COLOR : SDL_GPU_BLENDFACTOR_SRC_ALPHA;
    colorTarget.blend_state.dst_color_blendfactor = multiplicativeBlending ? SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA :
                                                    additiveBlending ? SDL_GPU_BLENDFACTOR_ONE : SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
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

    SDL_GPUGraphicsPipeline* pipeline = SDL_CreateGPUGraphicsPipeline(device, &pipelineInfo);
    if (!pipeline) {
        log::err("failed to create graphics pipeline: {}", SDL_GetError());
        allPipelinesSucceeded = false;
    } else
        graphicsPipelines.push_back(pipeline);
    return pipeline;
}

Pipelines createPipelines(const PipelineOptions& opts) {
    Pipelines pipelines = {
        .normal   = createGraphicsPipeline((SDL_GPUPrimitiveType)opts.primitive, opts.attributes, opts.vertexShader, opts.fragmentShader, BlendMode::Normal),
        .additive = createGraphicsPipeline((SDL_GPUPrimitiveType)opts.primitive, opts.attributes, opts.vertexShader, opts.fragmentShader, BlendMode::Additive)
    };

    if (opts.includeMultiplicative)
        pipelines.multiplicative = createGraphicsPipeline((SDL_GPUPrimitiveType)opts.primitive, opts.attributes, opts.vertexShader, opts.fragmentShader, BlendMode::Multiplicative);
    return pipelines;
};

bool hasFailedLoadingShaders() {
    return !allShadersSucceeded;
}

bool hasFailedLoadingPipelines() {
    return !allPipelinesSucceeded;
}

void releaseAllShaders() {
    for (auto shader : shaders)
        release(shader);
    shaders.clear();
}

void releaseAllObjects() {
    for (const auto& [_, sampler] : samplers)
        SDL_ReleaseGPUSampler(device, sampler);

    for (auto pipeline : graphicsPipelines)
        release(pipeline);
}

SDL_GPUBuffer* createGPUBuffer(u32 size, SDL_GPUBufferUsageFlags usage) {
    SDL_GPUBufferCreateInfo info{};
    info.size = size;
    info.usage = usage;
    return SDL_CreateGPUBuffer(device, &info);
}

SDL_GPUTransferBuffer* createGPUUploadBuffer(u32 size, void* data) {
    SDL_GPUTransferBufferCreateInfo info{};
    info.size = size;
    info.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
    SDL_GPUTransferBuffer* buffer = SDL_CreateGPUTransferBuffer(device, &info);
    if (!buffer)
        return nullptr;

    if (data) {
        memcpy(SDL_MapGPUTransferBuffer(device, buffer, false), data, size);
        SDL_UnmapGPUTransferBuffer(device, buffer);
    }

    return buffer;
}

SDL_GPUBuffer* createStaticGPUBuffer(u32 size, SDL_GPUBufferUsageFlags usage, void* data) {
    SDL_GPUBuffer* buffer = createGPUBuffer(size, usage);
    if (!buffer)
        return nullptr;

    SDL_GPUTransferBuffer* uploadBuffer = createGPUUploadBuffer(size, data);
    if (!uploadBuffer) {
        SDL_ReleaseGPUBuffer(device, buffer);
        return nullptr;
    }

    uploadBufferData(NULL, uploadBuffer, buffer, size);
    release(uploadBuffer);
    return buffer;
}

void uploadBufferData(
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

void copyBuffer(
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

SDL_GPUSampler* fetchSampler(const engine::TextureWrapParameters& params) {
    u32 code = params.asBitCode();
    auto it = samplers.find(code);
    if (it != samplers.end())
        return it->second;

    SDL_GPUSamplerCreateInfo samplerInfo{};
    samplerInfo.min_filter = SDL_GPU_FILTER_LINEAR;
    samplerInfo.mag_filter = SDL_GPU_FILTER_LINEAR;
    samplerInfo.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_LINEAR;
    samplerInfo.address_mode_u = toSDLAddressMode(params.u);
    samplerInfo.address_mode_v = toSDLAddressMode(params.v);

    SDL_GPUSampler* sampler = SDL_CreateGPUSampler(device, &samplerInfo);
    if (!sampler) {
        SDL_Log("Could not create sampler: %s", SDL_GetError());
        return nullptr;
    }

    samplers[code] = sampler;
    return sampler;
}

};