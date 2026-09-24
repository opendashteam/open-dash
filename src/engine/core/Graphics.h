#pragma once

#include "types.h"
#include "../Texture.h"
#include "SpriteBatch.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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

    virtual void drawSprite(
        InternalTexture texture,
        const glm::mat4& positionTransform,
        const glm::mat3& textureTransform,
        const Color4F& color
    ) = 0;

    inline void drawSprite(
        Texture* texture,
        const glm::mat4& positionTransform,
        const glm::mat3& textureTransform,
        const Color4F& color
    ) {
        drawSprite(texture->getInternalObject(), positionTransform, textureTransform, color);
    }

    virtual void drawSpriteBatch(
        InternalSpriteBatch raw,
        InternalTexture rawTexture,
        const glm::mat4& positionTransform,
        u32 count
    ) = 0;
};

};