#pragma once

#include "Node.h"
#include "macros.h"

namespace opendash::engine
{
/*
    Equivalent of cocos2d's CCScene. Only one scene can be running at a time.
*/
class Scene : public Node {
public:
    CREATE_FUNC(Scene)
    virtual void render(Graphics* gfx);
    virtual void update(float dt);
protected:
    bool init() override;
};

}