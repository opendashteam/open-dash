#pragma once

#include "types.h"

namespace opendash::engine {

class Node;

enum class MouseButton {
    LEFT   = 0,
    MIDDLE = 1,
    RIGHT  = 2
};

/*
    Making a node inherit this (as a secondary base class) allows you to handle inputs.
    Input positions are in world position.
*/
class InputDelegate {
public:
    InputDelegate();
    ~InputDelegate();

    /*
        This gets called on any mouse movement
    */
    virtual void onMouseMove(const Point& pos);

    /*
        This gets called when you click on a mouse button.
        If the function returns true, this node will swallow
        the input. In other words, all other Nodes below this
        node will not receive any mouse UNLESS `shouldReceiveAnyInput`
        returns true.
    */
    virtual bool onMouseDown(const Point& pos, MouseButton button);

    /*
        This gets called on any mouse release.
    */
    virtual void onMouseUp(const Point& pos, MouseButton button);

    /*
        If this function returns true, this node should receive
        any mouse clicks no matter if they have been swallowed
        by a node above it or not.
    */
    virtual bool shouldReceiveAnyInput();
};

};