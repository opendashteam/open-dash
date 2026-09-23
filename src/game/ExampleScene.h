#pragma once

#include "engine.h"

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
    Sprite* cubeSprite_ = nullptr;
};

}