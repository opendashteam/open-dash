#include "Background.h"
#include "../constants.h"

namespace opendash
{

using namespace engine;

bool backgroundUsesMirroredRepeat(int backgroundID) {
    switch (backgroundID) {
        case 16:
        case 35:
        case 37:
        case 40:
        case 41:
        case 42:
        case 43:
        case 44:
        case 45:
        case 46:
        case 47:
        case 48:
        case 49:
        case 50:
        case 51:
        case 53:
        case 54:
        case 55:
        case 56:
        case 57:
        case 58:
        case 59:
            return false;
        default:
            return true;
    }
}

std::unique_ptr<Background> Background::create(int backgroundID) {
    auto ret = std::make_unique<Background>();

    if (!ret->init(backgroundID)) {
        return nullptr;
    }

    return ret;
}

void Background::setScrollX(float scrollX) {
    if (sprite_) sprite_->setTileOffsetX(scrollX);
}

void Background::setScrollY(float scrollY) {
    if (sprite_) sprite_->setTileOffsetY(scrollY);
}

bool Background::init(int backgroundID) {
    sprite_ = addChild(TilingSprite::create(std::format("game_bg_{:02d}_001-uhd.png", backgroundID)));

    sprite_->setMirroredRepeatY(
        backgroundUsesMirroredRepeat(backgroundID)
    );

    sprite_->setAnchorPoint(PointZero);
    sprite_->setScale(Director::get()->getScreenScaleFactorMax());
    sprite_->setColor(constants::colors::kDefaultBackgroundColor);
    sprite_->setContentSize(Director::get()->getVisibleSize() / sprite_->getScale());

    setIgnoreCameraPosition(true);

    return true;
}

void Background::onRenderColorChanged() {
    if (sprite_) sprite_->setColor(renderColor_);
}

}