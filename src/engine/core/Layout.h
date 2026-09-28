#pragma once

#include "types.h"

namespace opendash::engine
{

class Node;

enum class AutoSize {
    Off,
    /*
        Automatically extends the size of
        a node to fill its parent size.
    */
    FillContainer,
    /*
        Automatically contracts the size of
        a node to snugly fit all its children.

        Only works if the node has a layout.
    */
    HugContents
};

struct EdgeInsets {
    float top, right, bottom, left;

    inline Size toSize() const { return { left + right, top + bottom }; }

    inline Size addToSize(const Size& size) const {
        return size + toSize();
    }

    inline Size removeFromSize(const Size& size) const {
        return size - toSize();
    }

    inline Point addToPoint(const Point& point) const {
        return {point.x + right, point.y + bottom};
    }

    inline Point removeFromPoint(const Point& point) const {
        return {point.x - right, point.y - bottom};
    }
};

enum class LayoutDirection {
    /*
        Items are layed out horizontally from left
        to right.
        
        X = main axis, Y = cross axis.
    */
    Row,
    /*
        Items are layed out vertically from top
        to bottom.
        
        X = cross axis, Y = main axis.
    */
    Column
};

enum class ItemAlign {
    Start,
    Center,
    End
};

float getItemAlignOffset(ItemAlign align, float innerSize, float outerSize);

struct Layout {
public:
    /*
        All setters will not include "set" as these functions
        will be used so frequently, it's best to make them
        easier to write. They also return refernces to
        themselves to allow you to make a chain.
    */

    inline Layout& direction(LayoutDirection direction) { direction_ = direction; dirtyLayout(); return *this; }
    inline Layout& mainAlign(ItemAlign align) { mainAlign_ = align; dirtyLayout(); return *this; }
    inline Layout& crossAlign(ItemAlign align) { crossAlign_ = align; dirtyLayout(); return *this; }
    inline Layout& padding(const EdgeInsets& insets) { padding_ = insets; dirtyLayout(); return *this; }
    inline Layout& gap(float gap) { gap_ = gap; dirtyLayout(); return *this; }

    /*
        Helper functions to make setting easier
    */

    inline Layout& mainAlignStart() { return mainAlign(ItemAlign::Start); }
    inline Layout& mainAlignCenter() { return mainAlign(ItemAlign::Center); }
    inline Layout& mainAlignEnd() { return mainAlign(ItemAlign::End); }
    inline Layout& crossAlignStart() { return crossAlign(ItemAlign::Start); }
    inline Layout& crossAlignCenter() { return crossAlign(ItemAlign::Center); }
    inline Layout& crossAlignEnd() { return crossAlign(ItemAlign::End); }

    inline Layout& padding(float top, float right, float bottom, float left) { return padding({top, right, bottom, left}); }
    inline Layout& padding(float topBottom, float leftRight) { return padding(topBottom, leftRight, topBottom, leftRight); }
    inline Layout& padding(float value) { return padding({value, value, value, value}); }

private:
    /*
        If a function ends in "Projected", it means that
        all position & size inputs are (main axis, cross axis)
        instead of (X, Y).

        "project" functions turn (X, Y) into (main axis, cross axis)
        and vice-a-versa.

        The "minSize" of a node is the minimum size that a node can
        be compressed to. It cannot be compressed beyond it because
        there is incompressible space like padding, margin and nodes
        where autoSize is off.

        The "innerSize" of a node is its size without the padding.
    */

    inline Size getMinSize() {
        if (minSizeDirty_)
            recalculateMinSize();
        return minSize_;
    }

    void dirtyLayout();

    Size calculateMinSizeProjected();

    Size getChildMinSizeProjected(Node* child);

    bool doesChildFillMainAxis(Node* child);

    bool doesChildFillCrossAxis(Node* child);

    void placeChildProjected(Node* child, const Point& newPosition, const Size& newSize);

    void layoutProjected(const Size& innerSize);

    inline Size project(Size size) {
        return direction_ == LayoutDirection::Row ? size : Size(size.height, size.width);
    }

    inline Point project(Point point) {
        return direction_ == LayoutDirection::Row ? point : Point(point.y, point.x);
    }

protected:
    // To be called by Node
    inline void setNode(Node* node) { node_ = node; }

    friend class Node;

    // To be called by Director
    void recalculateMinSize();
    void layout();

    friend class Director;

private:
    LayoutDirection direction_ = LayoutDirection::Row;

    ItemAlign mainAlign_ = ItemAlign::Start;
    ItemAlign crossAlign_ = ItemAlign::Start;

    EdgeInsets padding_ = {0, 0, 0, 0};

    float gap_ = 0;

    Node* node_ = nullptr;

    bool minSizeDirty_ = true;
    Size minSize_;
};

};