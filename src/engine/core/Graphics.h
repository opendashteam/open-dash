#pragma once

#include "types.h"
#include "../Texture.h"
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
    
    virtual bool beginDraw(const Color4F& clearColor, Size& windowSizeOut) = 0;

    virtual void finishDraw() = 0;

    virtual InternalTexture createTexture(u32 width, u32 height, TextureFormat format, void* data) = 0;

    virtual void destroyTexture(InternalTexture texture) = 0;

    virtual void drawSprite(
        InternalTexture texture,
        glm::mat4 positionTransform,
        const Rect& textureFrame, // texture frame rect is in pixels
        const Color4F& color
    ) = 0;

    inline void drawSprite(
        Texture* texture,
        glm::mat4 positionTransform,
        const Rect& textureFrame, // texture frame rect is in pixels
        const Color4F& color
    ) {
        drawSprite(texture->getInternalObject(), positionTransform, textureFrame, color);
    }
};

};