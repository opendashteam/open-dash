#include "Node.h"
#include "../core/Director.h"

namespace opendash::engine
{

void Node::removeFromParentAndCleanup() {
    if (parent_) {
        parent_->removeChild(this);

        // Cleanup...
    }
}

void Node::incrementTweenCount(u32 increment) {
    activeTweenCount_+= increment;
}

void Node::decrementTweenCount(u32 decrement) {
    activeTweenCount_-= decrement;

    if (activeTweenCount_ == 0) {
        onAllTweensFinished();
    }
}

void Node::onAllTweensFinished() {
    // Override me
}

bool Node::init() {
    return true;
}

void Node::markLocalTransformDirty() {
    isLocalTransformDirty_ = true;
    markWorldTransformDirty();
}

void Node::markWorldTransformDirty() {
    isWorldTransformDirty_ = true;
    for (auto& child : children_) {
        child->markWorldTransformDirty();
    }
}

void Node::markLayoutDirty() {
    Director::get()->markLayoutsDirty();
}

void Node::layout() {
    if (layout_) {
        layout_->layout();
    } else {
        // Otherwise, just make sure hug contents in the children work
        for (auto& child : children_) {
            if (!child->layout_)
                continue;

            auto size    = child->getContentSize();
            auto minSize = child->layout_->getMinSize();

            child->setContentSize({
                child->autoWidth_  == AutoSize::HugContents ? minSize.width  : size.width,
                child->autoHeight_ == AutoSize::HugContents ? minSize.height : size.height
            });
        }
    }

    for (auto& child : children_)
        child->layout();
}

const std::vector<std::unique_ptr<Node>>& Node::getChildren() const
{
    return children_;
}

void Node::removeChild(Node* child) {
    if (!child) return;

    child->parent_ = nullptr;

    // Find a faster solution if performance is an issue
    std::erase_if(children_, [child](const std::unique_ptr<Node>& c) {
        return c.get() == child;
    });
}

Node::~Node() {}

void Node::draw(Graphics* gfx) {
    // override me
}

void Node::visit(Graphics* gfx) {
    if (!isVisible_) return;

    this->draw(gfx); // draw self
    
    // draw children
    for (auto& child : children_) {
        child->visit(gfx);
    }
}

void Node::setPosition(const Point& position)
{
    if (position.x == position_.x && position.y == position_.y)
        return;
        
	position_ = position;
    markLocalTransformDirty();
}

void Node::setPosition(const Size &sizeAsPosition)
{
    setPosition(sizeAsPosition.width, sizeAsPosition.height);
}

void Node::setPosition(float x, float y)
{
	setPosition(Point(x, y));
}

void Node::setPositionX(float positionX)
{
	setPosition(Point(positionX, position_.y));
}

void Node::setPositionY(float positionY)
{
	setPosition(Point(position_.x, positionY));
}

void Node::setScale(float scale)
{
    setScale({scale, scale});
}

void Node::setScaleX(float scaleX)
{
	setScale({scaleX, scale_.y});
}

void Node::setScaleY(float scaleY)
{
	setScale({scale_.x, scaleY});
}

void Node::setScale(const Point& scale)
{
	if (scale.x == scale_.x && scale.y == scale_.y)
        return;

	scale_ = scale;
    markLocalTransformDirty();
}

void Node::setScale(float x, float y) {
    setScale({x, y});
}

void Node::setRotation(float degrees)
{
    if (degrees == rotation_)
        return;

	rotation_ = degrees;
    markLocalTransformDirty();
}

void Node::setAnchorPoint(const Point& anchorPoint)
{
    if (anchorPoint.x == anchorPoint_.x && anchorPoint.y == anchorPoint_.y)
        return;

	anchorPoint_ = anchorPoint;
    markLocalTransformDirty();
}

void Node::setAnchorPointX(float anchorPointX)
{
	setAnchorPoint({anchorPointX, anchorPoint_.y});
}

void Node::setAnchorPointY(float anchorPointY)
{
	setAnchorPoint({anchorPoint_.x, anchorPointY});
}

void Node::setAnchorPoint(float x, float y)
{
	setAnchorPoint({x, y});
}

void Node::setSkew(const Point& skew)
{
    if (skew.x == skew_.x && skew.y == skew_.y)
        return;

	skew_ = skew;
    markLocalTransformDirty();
}

void Node::setSkewX(float skewX)
{
	setSkew({skewX, skew_.y});
}

void Node::setSkewY(float skewY)
{
	setSkew({skew_.x, skewY});
}

void Node::setSkew(float x, float y)
{
	setSkew({x, y});
}

const Point& Node::getPosition() const
{
	return position_;
}

float Node::getPositionX() const
{
	return position_.x;
}

float Node::getPositionY() const
{
	return position_.y;
}

const Point& Node::getScale() const
{
	return scale_;
}

float Node::getScaleX() const
{
	return scale_.x;
}

float Node::getScaleY() const
{
	return scale_.y;
}

float Node::getRotation() const
{
	return rotation_;
}

const Point& Node::getAnchorPoint() const
{
	return anchorPoint_;
}

float Node::getAnchorPointX() const
{
	return anchorPoint_.x;
}

float Node::getAnchorPointY() const
{
	return anchorPoint_.y;
}

const Point& Node::getSkew() const
{
	return skew_;
}

float Node::getSkewX() const
{
	return skew_.x;
}

float Node::getSkewY() const
{
	return skew_.y;
}

void Node::setContentSize(const Size& contentSize)
{
    if (contentSize.width == contentSize_.width && contentSize.height == contentSize_.height)
        return;

    contentSize_ = contentSize;
    markLocalTransformDirty();
    markLayoutDirty();
}

void Node::setContentSize(float x, float y)
{
    setContentSize({x, y});
}

void Node::setContentWidth(float contentWidth)
{
    setContentSize({contentWidth, contentSize_.height});
}

void Node::setContentHeight(float contentHeight)
{
    setContentSize({contentSize_.width, contentHeight});
}

void Node::setVisible(bool visible)
{
    isVisible_ = visible;
}

// TODO: Dirty layout
void Node::setAutoWidth(AutoSize autoWidth)
{
    autoWidth_ = autoWidth;
    markLayoutDirty();
}

void Node::setAutoHeight(AutoSize autoHeight)
{
    autoHeight_ = autoHeight;
    markLayoutDirty();
}

void Node::setLayout(std::unique_ptr<Layout> layout)
{
    layout->setNode(this);
    layout_ = std::move(layout);
    markLayoutDirty();
}

void Node::setIgnoreLayout(bool ignore)
{
    isIgnoreLayout_ = ignore;
    markLayoutDirty();
}

const Size& Node::getContentSize() const
{
    return contentSize_;
}

float Node::getContentWidth() const
{
    return contentSize_.width;
}

float Node::getContentHeight() const
{
    return contentSize_.height;
}

const glm::mat4& Node::getLocalTransform() {
    if (isLocalTransformDirty_) {
        localTransform_ = computeLocalTransformMatrix();
        isLocalTransformDirty_ = false;
    }
    return localTransform_;
}

bool Node::isVisible() const
{
    return isVisible_;
}

AutoSize Node::getAutoWidth() const
{
    return autoWidth_;
}

AutoSize Node::getAutoHeight() const
{
    return autoHeight_;
}

Layout* Node::getLayout() const
{
    return layout_.get();
}

bool Node::isIgnoreLayout() const
{
    return isIgnoreLayout_;
}

int Node::getChildCount() const {
    return children_.size();
}

glm::mat4 Node::computeLocalTransformMatrix() {
    glm::mat4 transform(1.0f);

    transform = glm::translate(transform, glm::vec3(position_.x, position_.y, 0.0f));
    transform = glm::rotate(transform, glm::radians(-rotation_), glm::vec3(0.0f, 0.0f, 1.0f));

    if (skew_.x != 0.0f || skew_.y != 0.0f) {
        glm::mat4 skewMatrix(1.0f);
        skewMatrix[1][0] = tanf(glm::radians(skew_.x));
        skewMatrix[0][1] = tanf(glm::radians(skew_.y));
        transform = transform * skewMatrix;
    }

    transform = glm::scale(transform, glm::vec3(scale_.x, scale_.y, 1.0f));

    transform = glm::translate(transform, glm::vec3(
        -anchorPoint_.x * contentSize_.width,
        -anchorPoint_.y * contentSize_.height,
        0.0f
    ));

    return transform;
}

const glm::mat4& Node::getWorldTransform() {
    if (isWorldTransformDirty_) {
        if (parent_)
            worldTransform_ = parent_->getWorldTransform() * getLocalTransform();
        else
            worldTransform_ = getLocalTransform();

        inverseWorldTransform_ = glm::inverse(worldTransform_);
        isWorldTransformDirty_ = false;
    }
    return worldTransform_;
}

void Node::rotateBy(float deltaDegrees) {
    setRotation(rotation_ + deltaDegrees);
}

void Node::moveBy(float deltaX, float deltaY) {
    setPosition(position_.x + deltaX, position_.y + deltaY);
}

void Node::moveBy(Point deltaPosition) {
    setPosition(position_ + deltaPosition);
}

void Node::moveByX(float deltaX) {
    setPositionX(position_.x + deltaX);
}

void Node::moveByY(float deltaY) {
    setPositionY(position_.y + deltaY);
}

void Node::scaleBy(float modX, float modY) {
    setScale(scale_.x * modX, scale_.y * modY);
}

void Node::scaleByX(float modX) {
    setScaleX(scale_.x * modX);
}

void Node::scaleByY(float modY) {
    setScaleY(scale_.y * modY);
}

void Node::scaleBy(float mod) {
    setScale(scale_ * mod);
}

void Node::scaleBy(Point mod) {
    setScale(scale_ * mod);
}

Point Node::pointToWorldTransform(const Point& localPoint) {
    auto ret = getWorldTransform() * glm::vec4(localPoint.toGLM(), 0, 1);
    return {ret.x, ret.y};
}

Point Node::pointToLocalTransform(const Point& worldPoint) {
    if (isWorldTransformDirty_)
        getWorldTransform();
    auto ret = inverseWorldTransform_ * glm::vec4(worldPoint.toGLM(), 0, 1);
    return {ret.x, ret.y};
}

void Node::traversePreorder(VisitChild visitFn) {
    visitFn(this);
    for (auto& child : children_)
        child->traversePreorder(visitFn);
}

void Node::traversePostorder(VisitChild visitFn) {
    for (auto& child : children_)
        child->traversePreorder(visitFn);
    visitFn(this);
}

}