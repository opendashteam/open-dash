#pragma once

#include "../engine/Scene.h"
#include "../engine/macros.h"

namespace opendash
{

class TestScene : public engine::Scene {
public:
    CREATE_FUNC(TestScene)
protected:
    virtual bool init() override;
};

}