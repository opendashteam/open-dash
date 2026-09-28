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
    Color3B color = Color3B::WHITE;
    u8 opacity = 255;
    bool blending = true;
    int lineWidth = 2;
    CircleMode circleMode = CircleMode::Filled;
    Node* followTarget = nullptr;
};

class CircleWave : public ColorNode {
public:
    /*
        Recommended: use a designated initializer
    */
    static std::unique_ptr<CircleWave> create(const CircleWaveOptions& opt);
protected:
    bool init(const CircleWaveOptions& opt);
    void draw(Graphics* gfx) override;
private:
    float radius_ = 0.0f;
    int lineWidth_ = 2;
    CircleMode circleMode_ = CircleMode::Filled;
    Node* followTarget_ = nullptr;
    bool blending_ = true;
};

}