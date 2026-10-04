#pragma once

#include <SDL3/SDL.h>

#include "engine/core/types.h"
#include "../Window.h"

/*
    This file contains SDL_GPU helper functions.
    It is only to be included from SDL related source files!!
*/

namespace opendash::platform {

// Alias enums because the SDL_GPU ones are ridiculously long
enum class VertexFormat {
    Float  = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT,
    Float2 = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2,
    Float3 = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3,
    Float4 = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4,
    // 4 unsigned bytes with each value normalized [0, 255] => [0.0, 1.0]
    UByte4_Norm = SDL_GPU_VERTEXELEMENTFORMAT_UBYTE4_NORM
};

enum class Primitive {
    TriangleList  = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
    TriangleStrip = SDL_GPU_PRIMITIVETYPE_TRIANGLESTRIP
};

struct VertexAttribute {
    engine::u32 location;
    VertexFormat type;
};

struct PipelineOptions {
    Primitive primitive;
    std::vector<VertexAttribute> attributes;
    SDL_GPUShader* vertexShader;
    SDL_GPUShader* fragmentShader;
    bool includeMultiplicative = false;
};

struct Pipelines {
    SDL_GPUGraphicsPipeline* normal = nullptr;
    SDL_GPUGraphicsPipeline* additive = nullptr;
    SDL_GPUGraphicsPipeline* multiplicative = nullptr;

    inline SDL_GPUGraphicsPipeline* getWithAddBlend(bool useAdditive) {
        return useAdditive ? additive : normal;
    }

    inline SDL_GPUGraphicsPipeline* getWithBlendMode(engine::BlendMode mode) {
        switch (mode) {
        default:
        case engine::BlendMode::Normal: return normal;
        case engine::BlendMode::Additive: return additive;
        case engine::BlendMode::Multiplicative: return multiplicative;
        }
    }
};

namespace gpu {

extern SDL_GPUDevice* device;

void init(
    SDL_Window* window,
    SDL_GPUDevice* device,
    GraphicsLibrary library
);

SDL_GPUShader* loadShader(const std::string& path, SDL_GPUShaderStage stage, engine::u32 numSamplers, engine::u32 numUniformBuffers);

Pipelines createPipelines(const PipelineOptions& options);

bool hasFailedLoadingShaders();
bool hasFailedLoadingPipelines();

void releaseAllShaders();
void releaseAllObjects();

SDL_GPUBuffer* createGPUBuffer(engine::u32 size, SDL_GPUBufferUsageFlags usage);
SDL_GPUTransferBuffer* createGPUUploadBuffer(engine::u32 size, void* data = nullptr);
SDL_GPUBuffer* createStaticGPUBuffer(engine::u32 size, SDL_GPUBufferUsageFlags usage, void* data);

inline void* mapBuffer(SDL_GPUTransferBuffer* buffer) {
    return SDL_MapGPUTransferBuffer(device, buffer, false);
}

inline void unmapBuffer(SDL_GPUTransferBuffer* buffer) {
    SDL_UnmapGPUTransferBuffer(device, buffer);
}

void uploadBufferData(
    SDL_GPUCommandBuffer* cbuffer,
    SDL_GPUTransferBuffer* src,
    SDL_GPUBuffer* dst,
    engine::u32 size,
    engine::u32 srcOffset = 0,
    engine::u32 dstOffset = 0
);

void copyBuffer(
    SDL_GPUCommandBuffer* cbuffer,
    SDL_GPUBuffer* src,
    SDL_GPUBuffer* dst,
    engine::u32 size,
    engine::u32 srcOffset = 0,
    engine::u32 dstOffset = 0
);

SDL_GPUSampler* fetchSampler(const engine::TextureWrapParameters& params);

inline SDL_GPUCommandBuffer* acquireCommandBuffer() { return SDL_AcquireGPUCommandBuffer(device); }
inline void submit(SDL_GPUCommandBuffer* cbuffer) { SDL_SubmitGPUCommandBuffer(cbuffer); }

inline void release(SDL_GPUTransferBuffer* buffer) { SDL_ReleaseGPUTransferBuffer(device, buffer); }
inline void release(SDL_GPUBuffer* buffer) { SDL_ReleaseGPUBuffer(device, buffer); }
inline void release(SDL_GPUTexture* texture) { SDL_ReleaseGPUTexture(device, texture); }
inline void release(SDL_GPUShader* shader) { SDL_ReleaseGPUShader(device, shader); }
inline void release(SDL_GPUGraphicsPipeline* pipeline) { SDL_ReleaseGPUGraphicsPipeline(device, pipeline); }

};

};