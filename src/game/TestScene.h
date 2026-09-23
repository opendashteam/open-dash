#pragma once

#include "engine.h"

using namespace opendash::engine;

namespace opendash
{

class TestScene : public engine::Scene {
public:
    CREATE_FUNC(TestScene)
    void update(float dt) override;
protected:
    virtual bool init() override;
private:
    Sprite* cubeSprite_ = nullptr;
};

}