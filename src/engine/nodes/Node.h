#pragma once

#include "../core/types.h"
#include "../core/macros.h"
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../core/Graphics.h"
#include "../core/Layout.h"
#include <functional>

namespace opendash::engine
{

using VisitChild = std::function<void(Node*)>;

class Node {
public:
    CREATE_FUNC(Node)

    template<typename T>
    T* addChild(std::unique_ptr<T> child) {
        // TODO: might need to check if child already exists, maybe not
        if (child) {
            child->parent_ = this;
        }

        T* raw = child.get();
        children_.push_back(std::move(child));
        markLayoutDirty();
        return raw;
    }

    void removeChild(Node* child);

    virtual ~Node();
    
    virtual void draw(Graphics* gfx);
    virtual void visit(Graphics* gfx);

    // setters
    virtual void setPosition(const Point& position);
    virtual void setPosition(const Size& sizeAsPosition);
    virtual void setPosition(float x, float y);
    virtual void setPositionX(float positionX);
    virtual void setPositionY(float positionY);
    virtual void setScale(float scale);
    virtual void setScaleX(float scaleX);
    virtual void setScaleY(float scaleY);
    virtual void setScale(const Point& scale);
    virtual void setScale(float x, float y);
    virtual void setRotation(float degrees);
    virtual void setAnchorPoint(const Point& anchorPoint);
    virtual void setAnchorPointX(float anchorPointX);
    virtual void setAnchorPointY(float anchorPointY);
    virtual void setAnchorPoint(float x, float y);
    virtual void setSkew(const Point& skew);
    virtual void setSkewX(float skewX);
    virtual void setSkewY(float skewY);
    virtual void setSkew(float x, float y);
    virtual void setContentSize(const Size& contentSize);
    virtual void setContentSize(float x, float y);
    virtual void setContentWidth(float contentWidth);
    virtual void setContentHeight(float contentHeight);
    virtual void setVisible(bool visible);
    virtual void setAutoWidth(AutoSize autoWidth);
    virtual void setAutoHeight(AutoSize autoHeight);
    virtual void setLayout(std::unique_ptr<Layout> layout);
    virtual void setIgnoreLayout(bool ignore);

    /*
        Items will be layed out horizontally from
        left to right.
    */
    inline Layout& useRowLayout() {
        setLayout(std::make_unique<Layout>());
        return layout_->direction(LayoutDirection::Row);
    }
    /*
        Items will be layed out vertically from
        top to bottom.
    */
    inline Layout& useColumnLayout() {
        setLayout(std::make_unique<Layout>());
        return layout_->direction(LayoutDirection::Column);
    }

    // getters
    virtual const Point& getPosition() const;
    virtual float getPositionX() const;
    virtual float getPositionY() const;
    virtual const Point& getScale() const;
    virtual float getScaleX() const;
    virtual float getScaleY() const;
    virtual float getRotation() const;
    virtual const Point& getAnchorPoint() const;
    virtual float getAnchorPointX() const;
    virtual float getAnchorPointY() const;
    virtual const Point& getSkew() const;
    virtual float getSkewX() const;
    virtual float getSkewY() const;
    virtual const Size& getContentSize() const;
    virtual float getContentWidth() const;
    virtual float getContentHeight() const; 
    virtual const std::vector<std::unique_ptr<Node>>& getChildren() const;
    const glm::mat4& getWorldTransform();
    const glm::mat4& getLocalTransform();
    virtual bool isVisible() const;
    virtual AutoSize getAutoWidth() const;
    virtual AutoSize getAutoHeight() const;
    virtual Layout* getLayout() const;
    virtual bool isIgnoreLayout() const;
    virtual int getChildCount() const;

    // relative transformations
    virtual void rotateBy(float deltaDegrees);
    virtual void moveBy(float deltaX, float deltaY);
    virtual void moveBy(Point deltaPosition);
    virtual void moveByX(float deltaX);
    virtual void moveByY(float deltaY);
    virtual void scaleBy(float modX, float modY);
    virtual void scaleByX(float modX);
    virtual void scaleByY(float modY);
    virtual void scaleBy(float mod);
    virtual void scaleBy(Point mod);

    Point pointToWorldTransform(const Point& localPoint);
    Point pointToLocalTransform(const Point& worldPoint);

    // First the node, then its children
    void traversePreorder(VisitChild visitFn);
    // First the node's children, then the node
    void traversePostorder(VisitChild visitFn);

    // Change this when zorder is added
    inline void traverseDrawOrderReverse(VisitChild visitFn) {
        traversePostorder(visitFn);
    }

    // computation
    glm::mat4 computeLocalTransformMatrix();

    // Makes the node auto fill the parents node's width if the parent has a layout
    inline void makeWidthFillContainer() { setAutoWidth(AutoSize::FillContainer); }
    // Makes the node auto fill the parents node's height if the parent has a layout
    inline void makeHeightFillContainer() { setAutoHeight(AutoSize::FillContainer); }
    // Makes the node auto retract in width so that it's children fit snugly if the node has a layout
    inline void makeWidthHugContents() { setAutoWidth(AutoSize::HugContents); }
    // Makes the node auto retract in height so that it's children fit snugly if the node has a layout
    inline void makeHeightHugContents() { setAutoHeight(AutoSize::HugContents); }

    void removeFromParentAndCleanup();
    void incrementTweenCount(u32 increment = 1);
    void decrementTweenCount(u32 decrement = 1);

    virtual void onAllTweensFinished();

protected:
    virtual bool init();
    void markLocalTransformDirty();
    void markWorldTransformDirty();
    void markLayoutDirty();

private:
    // To be called by Director
    void layout();

    friend class Director;

private:
    std::vector<std::unique_ptr<Node>> children_ = {};
    Node* parent_ = nullptr;

    // transform
    Point position_{0.0f, 0.0f};
    Point scale_{1.0f, 1.0f};
    float rotation_ = 0.0f;
    Point anchorPoint_{0.0f, 0.0f};
    Point skew_{0.0f, 0.0f};
    Size contentSize_{0.0f, 0.0f};

    // transform cache and dirty flags
    glm::mat4 localTransform_{1.0f};
    glm::mat4 worldTransform_{1.0f};
    glm::mat4 inverseWorldTransform_{1.0f};
    bool isLocalTransformDirty_ = true;
    bool isWorldTransformDirty_ = true;

    // layout & auto size
    bool isIgnoreLayout_ = false;
    AutoSize autoWidth_ = AutoSize::Off;
    AutoSize autoHeight_ = AutoSize::Off;
    std::unique_ptr<Layout> layout_ = nullptr;

    // state
    bool isVisible_ = true;

    u32 activeTweenCount_ = 0;
};

}