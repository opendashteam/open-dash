#include "BounceButton.h"

namespace opendash {

void BounceButton::onActivate() {
    // This is a test
    setScale(1.3f);
}

void BounceButton::onDeactivate() {
    setScale(1.0f);
}

std::unique_ptr<BounceButton> BounceButton::createWithSprite(const std::filesystem::path& path) {
    auto sprite = Sprite::create(path);
    if (!sprite) return nullptr;
    auto button = BounceButton::create();
    if (!button) return nullptr;
    button->addChild(std::move(sprite));
    return button;
}

std::unique_ptr<BounceButton> BounceButton::createWithSpriteFrame(const std::string& frameName) {
    auto sprite = Sprite::createWithFrame(frameName);
    if (!sprite) return nullptr;
    auto button = BounceButton::create();
    if (!button) return nullptr;
    button->addChild(std::move(sprite));
    return button;
}

};