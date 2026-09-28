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

bool CircleWave::init(const CircleWaveOptions& opt) {
    if (!ColorNode::init()) {
        return false;
    }

    radius_ = opt.startRadius;
    setColor(opt.color);
    setOpacity(opt.opacity);
    lineWidth_ = opt.lineWidth;
    blending_ = opt.blending;
    followTarget_ = opt.followTarget;

    if (followTarget_) {
        setPosition(followTarget_->getPosition());
    }

    auto radiusTween = Director::get()->createTween({
        .from       = opt.startRadius,
        .to         = opt.endRadius,
        .duration   = opt.duration,
        .easingType = opt.easeOut ? EasingType::EaseOut : EasingType::Linear,
        .onUpdate   = [this](float value) {
            radius_ = value;
        },
        .onComplete = [this]() {
            // TODO
        }
    })
    ->start();

    return true;
}

void CircleWave::draw(Graphics *gfx) {
    glm::mat4 transform = getWorldTransform();
    glm::mat4 radiusScale = glm::scale(glm::mat4(1.0f), glm::vec3(radius_, radius_, 1.0f));
    glm::mat4 finalTransform = transform * radiusScale;

    gfx->drawFilledCircle(finalTransform, getRenderColor(), getLODForRadius(radius_), blending_);
}

}