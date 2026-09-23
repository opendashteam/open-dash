#pragma once

#include "../core/types.h"
#include "../core/macros.h"
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../core/Graphics.h"

namespace opendash::engine
{

class Node {
public:
    CREATE_FUNC(Node)

    /*
        Need to call std::move(child) for this to work.
    */
    template<typename T>
    T* addChild(std::unique_ptr<T> child) {
        if (child) {
            child->parent_ = this;
        }

        T* raw = child.get();
        children_.push_back(std::move(child));
        return raw;
    }
    
    virtual void draw(Graphics* gfx);
    virtual void visit(Graphics* gfx);

    // setters
    virtual void setPosition(const Point& position);
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

    // computation
    glm::mat4 computeLocalTransformMatrix();

protected:
    virtual bool init();
    void markLocalTransformDirty();
    void markWorldTransformDirty();
private:
    std::vector<std::unique_ptr<Node>> children_ = {};
    Node* parent_ = nullptr;

    // transform
    Point position_{0.0f, 0.0f};
    Point scale_{1.0f, 1.0f};
    float rotation_ = 0.0f;
    Point anchorPoint_{0.5f, 0.5f};
    Point skew_{0.0f, 0.0f};
    Size contentSize_{0.0f, 0.0f};

    // transform cache and dirty flags
    glm::mat4 localTransform_{1.0f};
    glm::mat4 worldTransform_{1.0f};
    bool isLocalTransformDirty_ = true;
    bool isWorldTransformDirty_ = true;

    // state
    bool isVisible_ = true;
};

}