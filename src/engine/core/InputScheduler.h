#pragma once

#include "InputDelegate.h"
#include <set>

namespace opendash::engine {

class InputScheduler {
public:
    void onRawMouseMove(const Point& screenPos);

    void onRawMouseInput(const Point& screenPos, MouseButton button, bool pressed);

private:
    std::set<InputDelegate*> delegates_;

    friend class InputDelegate;
};

};