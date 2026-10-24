#include "CircleWave.h"
// #include "../core/Director.h"
#include "../utilities/Tween.h"

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

std::unique_ptr<CircleWave> CircleWave::create(const CircleWavePreset &preset) {
    std::unique_ptr<CircleWave> ret = std::make_unique<CircleWave>();

    if (!ret->init(preset)) {
        return nullptr;
    }

    return ret;
}

void CircleWave::follow(Node *followTarget) {
    followTarget_ = followTarget;
}

void CircleWave::setOpacityMod(float opacityMod) {
    opacityMod_ = opacityMod;
}

bool CircleWave::init(const CircleWavePreset &preset) {
    if (!ColorNode::init()) {
        return false;
    }

    radius_ = preset.startRadius;
    blending_ = preset.blending;
    lineWidth_ = preset.lineWidth;
    circleMode_ = preset.circleMode;
    setColor(preset.color);

    if (followTarget_) {
        setPosition(followTarget_->getPosition());
    }

    EasingType type = (preset.easeOut && !preset.fadeIn)
                    ? EasingType::EaseOut
                    : EasingType::Linear;

    float startOpacity = preset.fadeIn ? 0.0f : 1.0f;
    float endOpacity   = preset.fadeIn ? 1.0f : 0.0f;
    float opacityTweenDuration  = preset.fadeIn ? preset.duration * 0.5f : preset.duration;

    Tween::create(preset.startRadius)
        .to(preset.duration, preset.endRadius, {
            .easing = type,
            .onUpdate = [this](float value) {
                radius_ = value;
                if (followTarget_) setPosition(followTarget_->getPosition());
            }
        })
        .start();

    Tween::create(startOpacity)
        .to(opacityTweenDuration, endOpacity, {
            .easing = type,
            .onUpdate = [this](float value) {
                setOpacityF(value * std::clamp(opacityMod_, 0.0f, 1.0f));
            },
            .onComplete = [this] {
                removeFromParentAndCleanup();
            },
            .yoyo = preset.fadeIn
        })
        .start();

    return true;
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