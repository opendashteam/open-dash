#include "ColorNode.h"

namespace opendash::engine
{

void ColorNode::setColor(const Color3B &color) {
    color_ = color;
    renderColor_ = Color4F::fromColor3B(color, renderColor_.a);
}

void ColorNode::setColor(u8 r, u8 g, u8 b) {
    setColor(Color3B{r, g, b});
}

void ColorNode::setColor(const Color4F &color) {
    color_ = Color3B::fromColor4F(color);
    renderColor_ = color;
    opacity_ = static_cast<u8>(color.a * 255.0f);
}

void ColorNode::setOpacity(u8 opacity) {
    opacity_ = opacity;
    renderColor_.a = opacity / 255.0f;
}

const Color3B &ColorNode::getColor() const {
    return color_;
}

u8 ColorNode::getOpacity() const {
    return opacity_;
}

const Color4F &ColorNode::getRenderColor() const {
    return renderColor_;
}

}