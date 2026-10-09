#include "CircleWave.h"
#include "../core/Director.h"

namespace opendash::engine
{

/*
    Level of detail (LOD) given a radius.
*/
u32 getLODForRadius(float radius) {
    if      (radius < 10.0f)  return 10;
    else if (radius < 20.0f)  return 15;
    else if (radius < 40.0f)  return 20;
    else if (radius < 200.0f) return 30;
    else                      return 50;
}

std::unique_ptr<CircleWave> CircleWave::create(const CircleWavePreset &opt) {
    std::unique_ptr<CircleWave> ret = std::make_unique<CircleWave>();

    if (!ret->init(opt)) {
        return nullptr;
    }

    return ret;
}

CircleWave &CircleWave::follow(Node *followTarget) {
    followTarget_ = followTarget;
    return *this;    
}

CircleWave &CircleWave::opacityMod(float opacityMod) {
    opacityMod_ = opacityMod;
    return *this;
}

bool CircleWave::init(const CircleWavePreset &opt) {
    if (!ColorNode::init()) {
        return false;
    }

    radius_ = opt.startRadius;
    startRadius_ = opt.startRadius;
    endRadius_ = opt.endRadius;
    duration_ = opt.duration;
    fadeIn_ = opt.fadeIn;
    easeOut_ = opt.easeOut;
    blending_ = opt.blending;
    lineWidth_ = opt.lineWidth;
    blending_ = opt.blending;
    followTarget_ = opt.followTarget;
    circleMode_ = opt.circleMode;
    setColor(opt.color);

    if (followTarget_) {
        setPosition(followTarget_->getPosition());
    }

    EasingType type = (easeOut_ && !fadeIn_)
                    ? EasingType::EaseOut
                    : EasingType::Linear;

    incrementTweenCount(2); // TODO: Might need a more robust system than this

    auto& radiusTween = Director::get()->createTween()
        .from(startRadius_)
        .to(endRadius_)
        .duration(duration_)
        .type(type)
        .onUpdate([this](float value) {
            radius_ = value;
            if (followTarget_) setPosition(followTarget_->getPosition());
        })
        .onComplete([this]() {
            decrementTweenCount();
        })
        .deleteSelf()
        .start();

    float startOpacity = fadeIn_ ? 0.0f : 1.0f;
    float endOpacity   = fadeIn_ ? 1.0f : 0.0f;
    float duration  = fadeIn_ ? duration_ * 0.5f : duration_;

    auto& opacityTween = Director::get()->createTween()
        .from(startOpacity)
        .to(endOpacity)
        .duration(duration)
        .type(type)
        .onUpdate([this](float value) {
            setOpacityF(value * std::clamp(opacityMod_, 0.0f, 1.0f));
        })
        .onComplete([this]() {
            decrementTweenCount();
        })
        .deleteSelf();

    if (fadeIn_) opacityTween.enableYoyo();

    opacityTween.start();

    return true;
}

void CircleWave::onAllTweensFinished() {
    removeFromParentAndCleanup();
}

void CircleWave::draw(Graphics *gfx) {
    glm::mat4 transform = getWorldTransform();
    glm::mat4 radiusScale = glm::scale(glm::mat4(1.0f), glm::vec3(radius_, radius_, 1.0f));
    glm::mat4 finalTransform = transform * radiusScale;

    if (circleMode_ == CircleMode::Outline)
        gfx->drawOutlineCircle(finalTransform, getRenderColor(), getLODForRadius(radius_), lineWidth_, blending_);
    else
        gfx->drawFilledCircle(finalTransform, getRenderColor(), getLODForRadius(radius_), blending_);
}

}