#pragma once

#include "types.h"
#include "SpriteFrame.h"

namespace opendash::engine
{

using InternalSpriteBatch = void*;

class SpriteBatch
{
private:
    inline SpriteBatch() {}

public:
    ~SpriteBatch();

    inline u32 getSize() const { return size_; }
    inline u32 getCapacity() const { return capacity_; }

    void resize(u32 newSize);

    void setSprite(
        u32 index,
        const glm::mat4& positionTransform,
        const glm::mat3& textureTransform,
        const Color4F& color
    );

    void setSpriteFrame(
        u32 index,
        SpriteFrame* spriteFrame,
        const Color4F& color,
        const glm::mat4& positionTransform
    );

    void setSpriteFrame(
        u32 index,
        SpriteFrame* spriteFrame,
        const Color4F& color,
        const glm::vec2& position = {0, 0}
    );

    void draw(Texture* texture, const glm::mat4& transform);

    static std::unique_ptr<SpriteBatch> create(u32 capacity = 32);

private:
    InternalSpriteBatch internal_;
    u32 size_ = 0;
    u32 capacity_ = 0;
};

};