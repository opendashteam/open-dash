#pragma once

#include "Node.h"

namespace opendash::engine
{

class ColorNode : public Node {
public:
    // setters
    virtual void setColor(const Color3B& color);
    virtual void setColor(u8 r, u8 g, u8 b);
    virtual void setColor(const Color4F& color);
    virtual void setOpacity(u8 opacity);

    //getters
    const Color3B& getColor() const;
    u8 getOpacity() const;
    const Color4F& getRenderColor() const;

protected:
    Color4F renderColor_ = {1.0f, 1.0f, 1.0f, 1.0f};
    Color3B color_ = {255, 255, 255};
    u8 opacity_ = 255;
};

}