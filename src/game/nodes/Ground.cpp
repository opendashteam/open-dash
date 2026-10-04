#include "Ground.h"
#include "../constants.h"

using namespace opendash::engine;

namespace opendash
{

std::unique_ptr<Ground> Ground::create(const GroundOptions &opt) {
    auto ret = std::make_unique<Ground>();

    if (!ret->init(opt)) {
        return nullptr;
    }

    return ret;
}

void Ground::setSecondaryColor(const engine::Color3B &secondaryColor) {
    secondaryColor_ = secondaryColor;
    secondarySprite_->setColor(secondaryColor);
}

const engine::Color3B& Ground::getSecondaryColor() const {
    return secondaryColor_;
}

void Ground::setScrollX(float scrollX) {
    primarySprite_->setTileOffsetX(scrollX);
    secondarySprite_->setTileOffsetX(scrollX);
}

void Ground::createShadows() {
    Size visibleSize = Director::get()->getVisibleSize();

    auto createShadow = [this, visibleSize](bool right){
        auto shadow = addChild(Sprite::createWithFrame("groundSquareShadow_001.png"));
        shadow->setAnchorPoint(0.0f, 1.0f);
        shadow->setPosition(right ? visibleSize.width + 1.0f : -1.0f, 0.0f);
        shadow->setOpacity(100);
        shadow->setScaleX(right ? -0.7f : 0.7f);
        shadow->setBlendMode(BlendMode::Multiplicative);
    };

    createShadow(false);
    createShadow(true);
}

void Ground::createTiles() {
    Size visibleSize = Director::get()->getVisibleSize();

    primarySprite_ = addChild(TilingSprite::create(std::format("groundSquare_{:02d}_001-uhd.png", options_.groundID)));
    primarySprite_->setContentWidth(visibleSize.width);
    primarySprite_->setColor(constants::colors::kDefaultGroundColor);
    primarySprite_->setAnchorPointY(1.0f);

    secondarySprite_ = (options_.groundID >= constants::kMinSecondaryGroundID)
        ? addChild(TilingSprite::create(std::format("groundSquare_{:02d}_2_001-uhd.png", options_.groundID)))
        : addChild(TilingSprite::createEmpty());
    secondarySprite_->setContentWidth(visibleSize.width);
    secondarySprite_->setColor(constants::colors::kDefaultGroundColor);
    secondarySprite_->setAnchorPointY(1.0f);
}

void Ground::createLine() {
    Size visibleSize = Director::get()->getVisibleSize();

    float yPos = options_.lineType == GroundLineType::Normal ? 0.5f : 0.2f;
    int style  = options_.lineType == GroundLineType::Normal ? 1 : 2;

    lineSprite_ = addChild(Sprite::createWithFrame(std::format("floorLine_{:02d}_001.png", style)));
    lineSprite_->setPosition(visibleSize.width * 0.5f, yPos);
    lineSprite_->setAnchorPoint(0.5f, 1.0f);

    if (blendLine_)
        lineSprite_->setBlendMode(BlendMode::Additive);
        
    if (options_.lineType == GroundLineType::Thick)
        lineSprite_->setScaleY(2.0f);

    lineSprite_->setScaleX((visibleSize.width + 10.0f) / lineSprite_->getContentWidth());
}

bool Ground::init(const GroundOptions &opt) {
    if (!ColorNode::init()) {
        return false;
    }

    options_ = opt;

    setContentSize(SizeZero);
    
    createTiles();
    createLine();
    createShadows();

    setIgnoreCameraPosition(true);

    return true;
}

void Ground::onRenderColorChanged() {
    primarySprite_->setColor(renderColor_);
}

}