#pragma once

#include "engine.h"

using namespace opendash::engine;

namespace opendash
{

class SpriteBatchTest : public Sprite {
public:
    CREATE_FUNC(SpriteBatchTest)
    
protected:
    virtual bool init() override;
    virtual void draw(Graphics* gfx) override;

private:
    std::unique_ptr<SpriteBatch> batch;
    Texture* texture;
};

}