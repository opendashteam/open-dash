#pragma once

#include "../engine/core/Graphics.h"

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

    virtual engine::InternalTexture createTexture(
        engine::u32 width,
        engine::u32 height,
        TextureFormat format,
        void* data
    );

    virtual void destroyTexture(engine::InternalTexture texture);

    virtual void drawSprite(
        engine::InternalTexture texture,
        const glm::mat4& positionTransform,
        const glm::mat3& textureTransform,
        const engine::Color4F& color
    );

public:
    static SDLGPUGraphics* create(SDL_Window* window);

private:
    bool init();

    SDL_GPUShader* loadShader(const char* path, SDL_GPUShaderStage stage, Uint32 numSamplers, Uint32 numUniformBuffers);

    bool setupPipelines();

    SDL_GPUBuffer* createStaticGPUBuffer(engine::u32 size, SDL_GPUBufferUsageFlags usage, void* data);

private:
    engine::u32 numTexturesAllocated = 0;

    SDL_Window* window_ = nullptr;
    SDL_GPUDevice* device_ = nullptr;

    SDL_GPUGraphicsPipeline* defaultSpritePipeline_ = nullptr;

    SDL_GPUBuffer* quadVertexBuffer_ = nullptr;

    // DRAW PASS VARIABLES //
    SDL_GPUCommandBuffer* commandBuffer_ = nullptr;
    SDL_GPUTexture* swapchainTexture_ = nullptr;
    SDL_GPURenderPass* renderPass_ = nullptr;
};

}