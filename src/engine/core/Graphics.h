#pragma once

#include "types.h"
#include "../Texture.h"
#include "SpriteBatch.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Mesh.h"

namespace opendash::engine
{

// Abstract class
class Graphics
{
public:
    enum class TextureFormat {
        RGB_UBYTE,
        RGBA_UBYTE
    };
    
    virtual bool beginDraw() = 0;

    virtual void finishDraw() = 0;

    // Should only be called between beginDraw() and finishDraw()
    virtual void setViewProjectionMatrix(const glm::mat4& viewProjection) = 0;

    virtual InternalTexture textureCreate(u32 width, u32 height, TextureFormat format, void* data) = 0;
    virtual void textureDestroy(InternalTexture texture) = 0;

    virtual InternalSpriteBatch spriteBatchCreate() = 0;
    virtual void spriteBatchResize(InternalSpriteBatch batch, u32 spriteCount) = 0;
    virtual void spriteBatchSetSprite(
        InternalSpriteBatch batch,
        u32 spriteIndex,
        const glm::mat4& positionTransform,
        const glm::mat3& textureTransform,
        const Color4F& color
    ) = 0;
    virtual void spriteBatchDestroy(InternalSpriteBatch batch) = 0;

    virtual void drawSpriteBatch(
        InternalSpriteBatch raw,
        InternalTexture rawTexture,
        const glm::mat4& positionTransform,
        const Color4F& globalColor,
        bool blending,
        u32 count
    ) = 0;

    virtual InternalMesh meshCreate() = 0;
    virtual void meshResize(InternalMesh mesh, u32 vertexCount) = 0;
    virtual MeshVertex* meshGetBuffer(InternalMesh mesh) = 0;
    virtual void meshFlushBufferRange(InternalMesh mesh, u32 firstVertex, u32 vertexCount) = 0;
    virtual void meshDestroy(InternalMesh mesh) = 0;

    virtual void drawMesh(
        InternalMesh mesh,
        const glm::mat4& positionTransform,
        const Color4F& globalColor,
        InternalTexture rawTexture,
        u32 vertexCount,
        bool blending
    ) = 0;

    virtual void drawSprite(
        InternalTexture texture,
        const glm::mat4& positionTransform,
        const glm::mat3& textureTransform,
        const Color4F& color,
        const engine::TextureWrapParameters& wrapParams,
        bool blending
    ) = 0;

    inline void drawSprite(
        Texture* texture,
        const glm::mat4& positionTransform,
        const glm::mat3& textureTransform,
        const Color4F& color,
        const engine::TextureWrapParameters& wrapParams,
        bool blending
    ) {
        drawSprite(texture->getInternalObject(), positionTransform, textureTransform, color, wrapParams, blending);
    }

    virtual void drawFilledCircle(
        const glm::mat4& positionTransform,
        const engine::Color4F& color,
        engine::u32 segments,
        bool blending
    ) = 0;

    virtual void drawOutlineCircle(
        const glm::mat4& positionTransform,
        const engine::Color4F& color,
        engine::u32 segments,
        engine::u32 lineWidthPx,
        bool blending
    ) = 0;

public:
    static Graphics* get();
};

};