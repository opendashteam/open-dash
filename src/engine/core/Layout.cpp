#include "Layout.h"
#include "../nodes/Node.h"
#include "Director.h"
#include "../utilities/log.h"

namespace opendash::engine
{

float getItemAlignOffset(ItemAlign align, float innerSize, float outerSize) {
    switch (align) {
    default:
    case ItemAlign::Start: return 0;
    case ItemAlign::Center: return outerSize / 2 - innerSize / 2;
    case ItemAlign::End: return outerSize - innerSize;
    }
}

void Layout::dirtyLayout() {
    if (node_)
        Director::get()->dirtyLayout();
}

void Layout::recalculateMinSize() {
    minSize_ = project(calculateMinSizeProjected());
    minSizeDirty_ = false;
}

Size Layout::calculateMinSizeProjected() {
    assert(node_);

    Size minSize = project(padding_.toSize());

    bool firstChild = true;

    for (auto& child : node_->getChildren()) {
        if (child->isIgnoreLayout())
            continue;

        Size childMinSize = getChildMinSizeProjected(child.get());

        if (!firstChild)
            minSize.width += gap_;
        firstChild = false;
    
        minSize.width += childMinSize.width;
        if (childMinSize.height > minSize.height)
            minSize.height = childMinSize.height;
    }

    return minSize;
}

Size Layout::getChildMinSizeProjected(Node* child) {
    Layout* layout = child->getLayout();

    Size minSizeUnprojected;

    AutoSize autoWidth  = child->getAutoWidth();
    AutoSize autoHeight = child->getAutoHeight();

    if (!layout) {
        minSizeUnprojected = { 0, 0 };
    } else {
        minSizeUnprojected = layout->getMinSize();

        if (autoWidth  == AutoSize::FillContainer) minSizeUnprojected.width  = 0;
        if (autoHeight == AutoSize::FillContainer) minSizeUnprojected.height = 0;
    }

    if (autoWidth  == AutoSize::Off) minSizeUnprojected.width  = child->getContentWidth();
    if (autoHeight == AutoSize::Off) minSizeUnprojected.height = child->getContentHeight();

    return project(minSizeUnprojected);
}

bool Layout::doesChildFillMainAxis(Node* child) {
    if (direction_ == LayoutDirection::Row)
        return child->getAutoWidth() == AutoSize::FillContainer;
    else
        return child->getAutoHeight() == AutoSize::FillContainer;
}

bool Layout::doesChildFillCrossAxis(Node* child) {
    if (direction_ == LayoutDirection::Row)
        return child->getAutoHeight() == AutoSize::FillContainer;
    else
        return child->getAutoWidth() == AutoSize::FillContainer;
}

void Layout::placeChildProjected(Node* child, const Point& newPosition, const Size& newSize) {
    Size unprojectedSize = project(newSize);

    child->setAnchorPoint(.5, .5);
    child->setContentSize(unprojectedSize);
    child->setPosition(padding_.addToPoint(project(newPosition)) + unprojectedSize * 0.5f);
    // Might wanna only set content size if it has auto size enabled
}

void Layout::layoutProjected(const Size& innerSize) {
    float incompressibleSpace = 0;
    int numMainFillingChildren = 0;
    bool firstChild = true;

    for (auto& child : node_->getChildren()) {
        if (child->isIgnoreLayout())
            continue;

        if (doesChildFillMainAxis(child.get()))
            numMainFillingChildren++;
        else
            incompressibleSpace += getChildMinSizeProjected(child.get()).width;

        if (!firstChild)
            incompressibleSpace += gap_;
        firstChild = false;
    }

    log::info("Incompressible space: {}", incompressibleSpace);

    float freeSpace = innerSize.width - incompressibleSpace;
    float mainFillingChildrenSize = 0;
    if (numMainFillingChildren > 0)
        mainFillingChildrenSize = freeSpace / (float)numMainFillingChildren;

    float mainPos = 0;
    if (numMainFillingChildren == 0)
        mainPos = getItemAlignOffset(mainAlign_, incompressibleSpace, innerSize.width);

    int index = 0;

    for (auto& child : node_->getChildren()) {
        if (child->isIgnoreLayout())
            continue;

        Size size = getChildMinSizeProjected(child.get());
        
        if (doesChildFillMainAxis(child.get()))
            size.width  = mainFillingChildrenSize;
        
        float crossPos;
        if (doesChildFillCrossAxis(child.get())){ 
            size.height = innerSize.height;
            crossPos = 0;
        } else
            crossPos = getItemAlignOffset(crossAlign_, size.height, innerSize.height);

        float mainPosRaw = mainPos;
        if (direction_ == LayoutDirection::Column)
            mainPosRaw = innerSize.width - mainPos - size.width;

        placeChildProjected(child.get(), {mainPosRaw, crossPos}, size);

        log::info("Child {} placed at {} | {}", index, Point {mainPos, crossPos}, size);
        index++;

        mainPos += size.width + gap_;
        firstChild = false;
    }
}

void Layout::layout() {
    assert(node_);
    auto innerSize = padding_.removeFromSize(node_->getContentSize());
    log::info("Layouting: size = {}, innerSize = {}", node_->getContentSize(), innerSize);
    layoutProjected(project(innerSize));
}

};