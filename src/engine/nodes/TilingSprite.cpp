#include "TilingSprite.h"

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

bool TilingSprite::initWithPath(const std::filesystem::path &path) {
    return true;
}

void TilingSprite::draw(Graphics *gfx) {

}

bool TilingSprite::init() {
    return Node::init();
}

}