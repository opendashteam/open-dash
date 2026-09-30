#include "InputDelegate.h"
#include "Director.h"

namespace opendash::engine {
    
InputDelegate::InputDelegate() {
    Director::get()->getInputScheduler().delegates_.insert(this);
}

InputDelegate::~InputDelegate() {
    Director::get()->getInputScheduler().delegates_.erase(this);
}

void InputDelegate::onMouseMove(const Point& pos) {}

bool InputDelegate::onMouseDown(const Point& pos, MouseButton button) {
    return false;
}

void InputDelegate::onMouseUp(const Point& pos, MouseButton button) {}

bool InputDelegate::shouldReceiveAnyInput() {
    return false;
}

};