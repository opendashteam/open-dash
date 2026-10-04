#pragma once

#include "Node.h"
#include "../core/macros.h"

namespace opendash::engine
{
/*
    Equivalent of cocos2d's CCScene. Only one scene can be running at a time.
*/
class Scene : public Node {
public:
    CREATE_FUNC(Scene)
    virtual void render(Graphics* gfx);
    virtual void update(float dt) override;
    virtual void onViewResized();
    virtual void onCameraMoved();
protected:
    bool init() override;
};

}