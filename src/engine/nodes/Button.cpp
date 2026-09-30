#include "Button.h"
#include "../utilities/log.h"

namespace opendash::engine {

bool Button::init() {
    useColumnLayout();
    makeWidthHugContents(); 
    makeHeightHugContents();

    return true;
}

void Button::onActivate() {}
void Button::onDeactivate() {}
void Button::onClick() {}

bool Button::onMouseDown(const Point& pos, MouseButton button) {
    Point localPos = pointToLocalTransform(pos);

    if (
        localPos.x < 0 || localPos.x > getContentWidth() ||
        localPos.y < 0 || localPos.y > getContentHeight()
    ) {
        return false;
    }

    isHeldDown_ = true;
    onActivate();
    if (activateFn_)
        activateFn_();

    return true;
}

void Button::onMouseUp(const Point& pos, MouseButton button) {
    if (isHeldDown_) {
        isHeldDown_ = false;
        onDeactivate();
        if (deactivateFn_)
            deactivateFn_();
        
        Point localPos = pointToLocalTransform(pos);

        if (
            localPos.x >= 0 && localPos.x <= getContentWidth() &&
            localPos.y >= 0 && localPos.y <= getContentHeight()
        ) {
            onClick();
            if (clickFn_)
                clickFn_();
        }
    }
}

};