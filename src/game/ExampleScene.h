#pragma once

#include "engine.h" // Always prefer this over individual includes in game files (do not use in engine code!)

using namespace opendash::engine;

namespace opendash
{

class ExampleScene : public engine::Scene {
public:
    CREATE_FUNC(ExampleScene)
    void update(float dt) override;
protected:
    virtual bool init() override;
private:
    Sprite* exampleSprite_ = nullptr;
};

}