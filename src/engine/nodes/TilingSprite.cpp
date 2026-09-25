#include "TilingSprite.h"
#include "../AssetManager.h"
#include "../core/Director.h"

namespace opendash::engine
{

std::unique_ptr<TilingSprite> TilingSprite::create(const std::filesystem::path &path)
{
    auto ret = std::make_unique<TilingSprite>();

    if (!ret->initWithPath(path)) {
        return nullptr;
    }

    return ret;
}

void TilingSprite::setTileOffset(const Point &tileOffset) {
    if (tileOffset == tileOffset_)
        return;

    tileOffset_ = tileOffset;
    uvDirty_ = true;
}

void TilingSprite::setTileOffset(float x, float y) {
    setTileOffset({x, y});
}

void TilingSprite::setTileOffsetX(float tileOffsetX) {
    setTileOffset({tileOffsetX, tileOffset_.y});
}

void TilingSprite::setTileOffsetY(float tileOffsetY) {
    setTileOffset({tileOffset_.x, tileOffsetY});
}

void TilingSprite::setTileScale(const Point &tileScale) {
    if (tileScale == tileScale_)
        return;

    tileScale_ = tileScale;
    uvDirty_ = true;
}

void TilingSprite::setTileScale(float x, float y) {
    setTileScale({x, y});
}

void TilingSprite::setTileScaleX(float tileScaleX) {
    setTileScale({tileScaleX, tileScale_.y});
}

void TilingSprite::setTileScaleY(float tileScaleY) {
    setTileScale({tileScale_.x, tileScaleY});
}

void TilingSprite::setMirroredRepeatX(bool mirroredX) {
    if (mirroredX == mirroredRepeatX_) return;
    mirroredRepeatX_ = mirroredX;
    uvDirty_ = true;
}

void TilingSprite::setMirroredRepeatY(bool mirroredY) {
    if (mirroredY == mirroredRepeatY_) return;
    mirroredRepeatY_ = mirroredY;
    uvDirty_ = true;
}

const Point &TilingSprite::getTileOffset() const {
    return tileOffset_;
}

float TilingSprite::getTileOffsetX() const {
    return tileOffset_.x;
}

float TilingSprite::getTileOffsetY() const {
    return tileOffset_.y;
}

const Point &TilingSprite::getTileScale() const {
    return tileScale_;
}

float TilingSprite::getTileScaleX() const {
    return tileScale_.x;
}

float TilingSprite::getTileScaleY() const {
    return tileScale_.y;
}

bool TilingSprite::getMirroredRepeatX() const {
    return mirroredRepeatX_;
}

bool TilingSprite::getMirroredRepeatY() const {
    return mirroredRepeatY_;
}

void TilingSprite::setContentSize(const Size& contentSize) {
    ColorNode::setContentSize(contentSize);
    spriteSizeTransformDirty_ = true;
}

bool TilingSprite::initWithPath(const std::filesystem::path &path) {
    texture_ = AssetManager::get()->fetchTexture(path);
    if (!texture_) {
        log::err("Failed to initialize tiling sprite, could not load texture at path: {}", path.string());
        return false;
    }

    textureWorldSize_ = texture_->getSize().inUnits();
    setContentSize(textureWorldSize_);
    return true;
}

void TilingSprite::draw(Graphics *gfx) {
    Size contentSize = getContentSize();

    glm::vec2 uvPos  = tileOffset_.toGLM() / textureWorldSize_.toGLM();
    glm::vec2 uvSize = (contentSize / textureWorldSize_).toGLM() / tileScale_.toGLM();

    glm::mat3 texTransform;
    texTransform[0] = { uvSize.x, 0,        0 };
    texTransform[1] = { 0,        uvSize.y, 0 };
    texTransform[2] = { uvPos.x,  uvPos.y,  1 };

    if (spriteSizeTransformDirty_) {
        spriteSizeTransform_ = glm::scale(glm::mat4(1.0f), glm::vec3(contentSize.toGLM(), 1.0f));
        spriteSizeTransformDirty_ = false;
    }

    glm::mat4 posTransform = getWorldTransform() * spriteSizeTransform_;

    TextureWrapParameters wrapParameters {
        mirroredRepeatX_ ? WrapMode::MirroredRepeat : WrapMode::Repeat,
        mirroredRepeatY_ ? WrapMode::MirroredRepeat : WrapMode::Repeat
    };

    gfx->drawSprite(texture_, posTransform, texTransform, renderColor_, wrapParameters);
}

bool TilingSprite::init() {
    return Node::init();
}

}