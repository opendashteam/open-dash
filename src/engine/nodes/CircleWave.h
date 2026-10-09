#pragma once

#include "ColorNode.h"
#include <memory>

namespace opendash::engine
{

enum class CircleMode {
    Filled, Outline
};

struct CircleWavePreset {

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
    Color3B color = Color3B::White;
};

class CircleWave : public ColorNode {
public:
    static std::unique_ptr<CircleWave> create(const CircleWavePreset& opt);

    // CircleWave& startRadius(float startRadius);
    // CircleWave& endRadius(float endRadius);
    // CircleWave& duration(float duration);
    // CircleWave& fadeIn(bool fadeIn);
    // CircleWave& easeOut(bool easeOut);
    // CircleWave& blending(bool blending);
    // CircleWave& lineWidth(int lineWidth);
    // CircleWave& mode(const CircleMode& mode);

    CircleWave& follow(Node* followTarget);
    CircleWave& opacityMod(float opacityMod);

    void onAllTweensFinished() override;
protected:
    bool init(const CircleWavePreset& opt);
    void draw(Graphics* gfx) override;
private:

    float startRadius_ = 0.0f;
    float endRadius_ = 0.0f;
    float duration_ = 0.0f;
    bool fadeIn_ = false;
    bool easeOut_ = true;
    bool blending_ = true;
    int lineWidth_ = 2; // Screen pixels, not points
    CircleMode circleMode_= CircleMode::Filled;
    Node* followTarget_ = nullptr;
    Color3B color_ = Color3B::White;
    float radius_ = 0.0f;
    float opacityMod_ = 1.0f;
};

}