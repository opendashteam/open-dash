#pragma once

#include "types.h"
#include "../Texture.h"
#include "../utilities/PList.h"

namespace opendash::engine
{

class SpriteFrame
{
public:
    inline const std::string& getName() const { return name_; }

    inline Texture* getTexture() const { return texture_; }
    inline bool isRotated() const { return rotated_; }
    inline const Rect& getTextureRect() const { return textureRect_; }

    inline const Point& getSpriteOffset() const { return spriteOffset_; }
    inline const Size& getSpriteSize() const { return spriteSize_; }
    inline const Size& getSpriteSourceSize() const { return spriteSourceSize_; }

    const glm::mat3& getTextureTransform();

    static SpriteFrame* loadFromPListNode(Texture* texture, const std::string& name, PList* node);

private:
    std::string name_;

    Texture* texture_;
    Rect textureRect_;
    bool rotated_;

    Point spriteOffset_;
    Size spriteSize_;
    Size spriteSourceSize_;

    bool textureTransformCreated = false;
    glm::mat3 textureTransform;
};

};