#include "SpriteBatch.h"
#include "../../platform/Window.h"
#include "Graphics.h"

namespace opendash::engine
{

SpriteBatch::~SpriteBatch() {
    Graphics* gfx = platform::Window::getGraphics();
    gfx->spriteBatchDestroy(internal_);
}

void SpriteBatch::resize(u32 newSize) {
    Graphics* gfx = platform::Window::getGraphics();

    if (newSize > capacity_) {
        if (capacity_ == 0)
            capacity_ = 1;
        while (capacity_ < newSize)
            capacity_ *= 2;

        gfx->spriteBatchResize(internal_, capacity_);
    }

    size_ = newSize;
}

void SpriteBatch::setSprite(
    u32 index,
    const glm::mat4& positionTransform,
    const glm::mat3& textureTransform,
    const Color4F& color
) {
    assert(index < size_);

    Graphics* gfx = platform::Window::getGraphics();
    gfx->spriteBatchSetSprite(internal_, index, positionTransform, textureTransform, color);
}

void SpriteBatch::setSpriteFrame(
    u32 index,
    SpriteFrame* spriteFrame,
    const Color4F& color,
    const glm::mat4& positionTransform
) {
    setSprite(
        index,
        positionTransform * spriteFrame->getPositionTransform(),
        spriteFrame->getTextureTransform(),
        color
    );
}

void SpriteBatch::setSpriteFrame(
    u32 index,
    SpriteFrame* spriteFrame,
    const Color4F& color,
    const glm::vec2& position
) {
    glm::mat4 transform(1.0f);
    transform = glm::translate(transform, glm::vec3(position, 0.0f));
    setSpriteFrame(index, spriteFrame, color, transform);
}

void SpriteBatch::draw(Texture* texture, const glm::mat4& transform) {
    if (size_ == 0)
        return;

    Graphics* gfx = platform::Window::getGraphics();
    gfx->drawSpriteBatch(internal_, texture->getInternalObject(), transform, size_);
}

std::unique_ptr<SpriteBatch> SpriteBatch::create(u32 capacity) {
    assert(capacity > 0);

    Graphics* gfx = platform::Window::getGraphics();
    InternalSpriteBatch internal = gfx->spriteBatchCreate();
    gfx->spriteBatchResize(internal, capacity);

    auto ret = std::unique_ptr<SpriteBatch>(new SpriteBatch);

    ret->internal_ = internal;
    ret->capacity_ = capacity;

    return ret;
}

};