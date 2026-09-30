#pragma once

#include "ColorNode.h"
#include <memory>

namespace opendash::engine
{

enum class CircleMode {
    Filled, Outline
};

struct CircleWaveOptions {

    // Required
    float startRadius;
    float endRadius;
    float duration;
    bool fadeIn;

    // Optional
    bool easeOut = true;
    bool blending = true;
    int lineWidth = 2; // Screen pixels, not points
    CircleMode circleMode = CircleMode::Filled;
    Node* followTarget = nullptr;

    // Mainly for constants::presets
    Color3B color = Color3B::White;
};

class CircleWave : public ColorNode {
public:
    /*
        Recommended: use a designated initializer.

        Order to follow:
        ---
        ### REQUIRED:

        float startRadius;
        float endRadius;
        float duration;
        bool fadeIn;

        ### OPTIONAL:

        bool easeOut = true;
        bool blending = true;
        int lineWidth = 2;
        CircleMode circleMode = CircleMode::Filled;
        Node* followTarget = nullptr;
    */
    static std::unique_ptr<CircleWave> create(const CircleWaveOptions& opt);

    void setOpacityMod(float opacityMod);
    void setFollowTarget(Node* followTarget);

    float getOpacityMod() const;
    Node* getFollowTarget() const;

    void onAllTweensFinished() override;
protected:
    bool init(const CircleWaveOptions& opt);
    void draw(Graphics* gfx) override;
private:
    float radius_ = 0.0f;
    int lineWidth_ = 2;
    CircleMode circleMode_ = CircleMode::Filled;
    Node* followTarget_ = nullptr;
    bool blending_ = true;
    float opacityMod_ = 1.0f;
};

}