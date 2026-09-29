#pragma once

#include "InputDelegate.h"
#include <set>

namespace opendash::engine {

class InputScheduler {
public:
    void onRawMouseMove(const Point& screenPos);

    void onRawMouseInput(const Point& screenPos, MouseButton button, bool pressed);

    inline bool isMouseDown(MouseButton button) {
        return mouseButtonPressed_[(int)button];
    }

private:
    std::set<InputDelegate*> delegates_;

    bool mouseButtonPressed_[(int)MouseButton::Count] = { false };

    friend class InputDelegate;
};

};