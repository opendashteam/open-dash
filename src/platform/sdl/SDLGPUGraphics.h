#pragma once

#include "engine/core/Graphics.h"
#include "../Window.h"
#include "engine/core/types.h"
#include "gpu.h"

/*
    Because this header file includes SDL, it is important
    to include this header file from Window.cpp or
    other related files only.
*/
#include <SDL3/SDL.h>

namespace opendash::platform
{

struct OutlineMeshBuffers {
    SDL_GPUBuffer* vertexBuffer;
    SDL_GPUBuffer* indexBuffer;
    engine::u32 indexCount;
};

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

    virtual engine::InternalMesh meshCreate();
    virtual void meshResize(engine::InternalMesh mesh, engine::u32 vertexCount);
    virtual engine::MeshVertex* meshGetBuffer(engine::InternalMesh mesh);
    virtual void meshFlushBufferRange(engine::InternalMesh mesh, engine::u32 firstVertex, engine::u32 vertexCount);
    virtual void meshDestroy(engine::InternalMesh mesh);

    virtual void drawMesh(
        engine::InternalMesh mesh,
        const glm::mat4& positionTransform,
        const engine::Color4F& globalColor,
        engine::InternalTexture rawTexture,
        engine::u32 vertexCount,
        bool blending
    );

    virtual void drawSprite(
        engine::InternalTexture texture,
        const glm::mat4& positionTransform,
        const glm::mat3& textureTransform,
        const engine::Color4F& color,
        const engine::TextureWrapParameters& wrapParams,
        engine::BlendMode blendMode
    );

    virtual void drawSpriteBatch(
        engine::InternalSpriteBatch raw,
        engine::InternalTexture rawTexture,
        const glm::mat4& positionTransform,
        const engine::Color4F& globalColor,
        bool blending,
        engine::u32 count
    );

    virtual void drawFilledCircle(
        const glm::mat4& positionTransform,
        const engine::Color4F& color,
        engine::u32 segments,
        bool blending
    );

    virtual void drawOutlineCircle(
        const glm::mat4& positionTransform,
        const engine::Color4F& color,
        engine::u32 segments,
        engine::u32 lineWidthPx,
        bool blending
    );

public:
    static SDLGPUGraphics* create(SDL_Window* window, GraphicsLibrary library);

private:
    bool init();

    bool setupPipelines();

    OutlineMeshBuffers createOutlineCircleBuffers(engine::u32 segments);

private:
    GraphicsLibrary graphicsLibrary_;

    engine::u32 numTexturesAllocated_ = 0;

    SDL_Window* window_ = nullptr;
    SDL_GPUDevice* device_ = nullptr;

    SDL_GPUBuffer* quadVertexBuffer_ = nullptr;

    // DRAW PASS VARIABLES //
    SDL_GPUCommandBuffer* commandBuffer_ = nullptr;
    SDL_GPURenderPass* renderPass_ = nullptr;

    // PIPELINES //
    Pipelines defaultSpritePipeline_;
    Pipelines spriteBatchPipeline_;
    Pipelines circlePipeline_;
    Pipelines outlineCirclePipeline_;

    Pipelines solidMeshPipeline_;
    Pipelines textureMeshPipeline_;

    // CACHE //
    std::unordered_map<engine::u32, SDL_GPUBuffer*>  circleBuffers_; // segment count, buffer
    std::unordered_map<engine::u32, OutlineMeshBuffers>  outlineCircleBuffers_; // segment count, buffers
};

}