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
    virtual void update(float dt);
    virtual void onViewResized();

    void setClearColor(const Color4F& clearColor);
    const Color4F& getClearColor() const;
protected:
    bool init() override;
private:
    Color4F clearColor_ = {0.0f, 0.0f, 0.0f, 1.0f};
};

}