#pragma once

#include "engine.h" // Always prefer this over individual includes in game files (do not use in engine code!)
#include "SpriteBatchTest.h"
#include "nodes/Ground.h"
#include "nodes/WaveTrail.h"

using namespace opendash::engine;

namespace opendash
{

class ExampleScene : public engine::Scene, public engine::InputDelegate {
public:
    CREATE_FUNC(ExampleScene)
    void update(float dt) override;
    void onCameraMoved() override;

    virtual bool onMouseDown(const Point &pos, MouseButton button) override;
    virtual void onMouseUp(const Point& pos, MouseButton button) override;

protected:
    virtual bool init() override;
private:
    Sprite* exampleSprite_ = nullptr;
    TilingSprite* tilingSprite_ = nullptr;
    SpriteBatchTest* batchTest_ = nullptr;
    Ground* ground_ = nullptr;
    WaveTrail* waveTrail_ = nullptr;
    bool spriteMovingRight_ = true;

    bool inputDown_ = false;
};

}