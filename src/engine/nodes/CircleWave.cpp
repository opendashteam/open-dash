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

std::unique_ptr<CircleWave> CircleWave::create(const CircleWaveOptions &opt) {
    std::unique_ptr<CircleWave> ret = std::make_unique<CircleWave>();

    if (!ret->init(opt)) {
        return nullptr;
    }

    return ret;
}

void CircleWave::setOpacityMod(float opacityMod) {
    opacityMod_ = opacityMod;
}

void CircleWave::setFollowTarget(Node *followTarget) {
    followTarget_ = followTarget;
    if (followTarget_) {
        setPosition(followTarget_->getPosition());
    }
}

float CircleWave::getOpacityMod() const {
    return opacityMod_;
}

bool CircleWave::init(const CircleWaveOptions& opt) {
    if (!ColorNode::init()) {
        return false;
    }

    radius_ = opt.startRadius;
    lineWidth_ = opt.lineWidth;
    blending_ = opt.blending;
    followTarget_ = opt.followTarget;
    circleMode_ = opt.circleMode;
    setColor(opt.color);

    if (followTarget_) {
        setPosition(followTarget_->getPosition());
    }

    EasingType type = (opt.easeOut && !opt.fadeIn)
                    ? EasingType::EaseOut
                    : EasingType::Linear;

    incrementTweenCount(2);

    auto radiusTween = Director::get()->createTween({
        .from       = opt.startRadius,
        .to         = opt.endRadius,
        .duration   = opt.duration,
        .easingType = type,
        .onUpdate   = [opt, this](float value) {
            radius_ = value;
            if (opt.followTarget) setPosition(opt.followTarget->getPosition());
        },
        .onComplete = [this]() {
            decrementTweenCount();
        },
        .deleteSelf = true
    })
    ->start();

    float startOpacity = opt.fadeIn ? 0.0f : 1.0f;
    float endOpacity   = opt.fadeIn ? 1.0f : 0.0f;
    float duration  = opt.fadeIn ? opt.duration * 0.5f : opt.duration;

    auto opacityTween = Director::get()->createTween({
        .from       = startOpacity,
        .to         = endOpacity,
        .duration   = duration,
        .easingType = type,
        .onUpdate   = [this](float value) {
            setOpacityF(value * std::clamp(opacityMod_, 0.0f, 1.0f));
        },
        .onComplete = [this]() {
            decrementTweenCount();
        },
        .deleteSelf = true
    });

    if (opt.fadeIn) opacityTween->enableYoyo();

    opacityTween->start();

    return true;
}

Node *CircleWave::getFollowTarget() const {
    return followTarget_;
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