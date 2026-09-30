#pragma once

#include "Node.h"
#include "../core/InputDelegate.h"
#include <functional>

namespace opendash::engine {

using ButtonFunction = std::function<void()>;

class Button : public Node, public InputDelegate {
public:
    CREATE_FUNC(Button);

    /*
        Called when the button has been pressed on (but not yet released)
    */
    inline void onActivate(ButtonFunction fn) { activateFn_ = fn; }

    /*
        Called when the button has been released after being pressed on.
    */
    inline void onDeactivate(ButtonFunction fn) { deactivateFn_ = fn; }

    /*
        Called when the button has been pressed and release on
        with both cases having the cursor over it. This registers
        a click on the button.
    */
    inline void onClick(ButtonFunction fn) { clickFn_ = fn; }

protected:
    bool init() override;

    virtual void onActivate();
    virtual void onDeactivate();
    virtual void onClick();

    bool onMouseDown(const Point& pos, MouseButton button) override;

    void onMouseUp(const Point& pos, MouseButton button) override;

private:
    ButtonFunction activateFn_ = nullptr;
    ButtonFunction deactivateFn_ = nullptr;
    ButtonFunction clickFn_ = nullptr;

    bool isHeldDown_ = false;
};

};