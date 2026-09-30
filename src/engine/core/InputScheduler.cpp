#include "InputScheduler.h"
#include "Director.h"
#include "../utilities/log.h"

namespace opendash::engine {

void InputScheduler::onRawMouseMove(const Point& screenPos) {
    Point pos = Director::get()->toWorldPosition(screenPos);
    for (const auto& delegate : delegates_)
        delegate->onMouseMove(pos);
}

void InputScheduler::onRawMouseInput(const Point& screenPos, MouseButton button, bool pressed) {
    if ((u32)button >= (u32)MouseButton::Count)
        return;

    mouseButtonPressed_[(int)button] = pressed;

    Point pos = Director::get()->toWorldPosition(screenPos);
    if (pressed) {
        bool isSwallowed = false;

        Scene* scene = Director::get()->getRunningScene();

        scene->traverseDrawOrderReverse([&](Node* node) {
            InputDelegate* delegate = dynamic_cast<InputDelegate*>(node);

            if (delegate && (!isSwallowed || delegate->shouldReceiveAnyInput())) {
                if (delegate->onMouseDown(pos, button))
                    isSwallowed = true;
            }
        });
    } else {
        for (const auto& delegate : delegates_)
            delegate->onMouseUp(pos, button);
    }
}

};