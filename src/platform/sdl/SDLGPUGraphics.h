#pragma once

#include "engine/core/Graphics.h"

/*
    Because this header file includes SDL, it is important
    to include this header file from Window.cpp or
    other related files only.
*/
#include <SDL3/SDL.h>

namespace opendash::platform
{

class SDLGPUGraphics : public engine::Graphics
{
public:
    ~SDLGPUGraphics();

    virtual bool beginDraw();

    virtual void finishDraw();

    virtual void setViewProjectionMatrix(const glm::mat4& viewProjection);

    virtual engine::InternalTexture textureCreate(
        engine::u32 width,
        engine::u32 height,
        TextureFormat format,
        void* data
    );
    virtual void textureDestroy(engine::InternalTexture texture);

    virtual engine::InternalSpriteBatch spriteBatchCreate();
    virtual void spriteBatchResize(engine::InternalSpriteBatch batch, engine::u32 spriteCount);
    virtual void spriteBatchSetSprite(
        engine::InternalSpriteBatch batch,
        engine::u32 spriteIndex,
        const glm::mat4& positionTransform,
        const glm::mat3& textureTransform,
        const engine::Color4F& color
    );
    virtual void spriteBatchDestroy(engine::InternalSpriteBatch batch);

    virtual void drawSprite(
        engine::InternalTexture texture,
        const glm::mat4& positionTransform,
        const glm::mat3& textureTransform,
        const engine::Color4F& color,
        const engine::TextureWrapParameters& wrapParams
    );

    virtual void drawSpriteBatch(
        engine::InternalSpriteBatch raw,
        engine::InternalTexture rawTexture,
        const glm::mat4& positionTransform,
        engine::u32 count
    );

public:
    static SDLGPUGraphics* create(SDL_Window* window);

private:
    struct VertexAttribute {
        engine::u32 location;
        SDL_GPUVertexElementFormat type;
    };

    bool init();

    SDL_GPUShader* loadShader(const char* path, SDL_GPUShaderStage stage, Uint32 numSamplers, Uint32 numUniformBuffers);

    SDL_GPUGraphicsPipeline* createGraphicsPipeline(
        SDL_GPUPrimitiveType primitive,
        const std::vector<VertexAttribute>& attributes,
        SDL_GPUShader* vertexShader,
        SDL_GPUShader* fragmentShader
    );

    bool setupPipelines();

    SDL_GPUBuffer* createGPUBuffer(engine::u32 size, SDL_GPUBufferUsageFlags usage);

    SDL_GPUTransferBuffer* createGPUUploadBuffer(engine::u32 size, void* data = nullptr);

    inline void* mapBuffer(SDL_GPUTransferBuffer* buffer) {
        return SDL_MapGPUTransferBuffer(device_, buffer, false);
    }

    inline void unmapBuffer(SDL_GPUTransferBuffer* buffer) {
        SDL_UnmapGPUTransferBuffer(device_, buffer);
    }

    inline void release(SDL_GPUTransferBuffer* buffer) { SDL_ReleaseGPUTransferBuffer(device_, buffer); }
    inline void release(SDL_GPUBuffer* buffer) { SDL_ReleaseGPUBuffer(device_, buffer); }
    inline void release(SDL_GPUTexture* texture) { SDL_ReleaseGPUTexture(device_, texture); }
    inline void release(SDL_GPUShader* shader) { SDL_ReleaseGPUShader(device_, shader); }
    inline void release(SDL_GPUGraphicsPipeline* pipeline) { SDL_ReleaseGPUGraphicsPipeline(device_, pipeline); }

    inline SDL_GPUCommandBuffer* acquireCommandBuffer() { return SDL_AcquireGPUCommandBuffer(device_); }
    inline void submit(SDL_GPUCommandBuffer* cbuffer) { SDL_SubmitGPUCommandBuffer(cbuffer); }

    inline void releaseAllShaders() {
        for (auto shader : shaders_)
            release(shader);
        shaders_.clear();
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

    SDL_GPUBuffer* createStaticGPUBuffer(engine::u32 size, SDL_GPUBufferUsageFlags usage, void* data);

    SDL_GPUSampler* fetchSampler(const engine::TextureWrapParameters& params);

private:
    engine::u32 numTexturesAllocated_ = 0;

    SDL_Window* window_ = nullptr;
    SDL_GPUDevice* device_ = nullptr;

    SDL_GPUGraphicsPipeline* defaultSpritePipeline_ = nullptr;
    SDL_GPUGraphicsPipeline* spriteBatchPipeline_ = nullptr;

    std::unordered_map<engine::u32, SDL_GPUSampler*> samplers_;

    SDL_GPUBuffer* quadVertexBuffer_ = nullptr;

    std::vector<SDL_GPUShader*> shaders_;

    // DRAW PASS VARIABLES //
    SDL_GPUCommandBuffer* commandBuffer_ = nullptr;
    SDL_GPUTexture* swapchainTexture_ = nullptr;
    SDL_GPURenderPass* renderPass_ = nullptr;
};

}