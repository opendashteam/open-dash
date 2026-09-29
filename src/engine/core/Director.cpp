#include "Director.h"
#include "../../platform/Window.h"
#include "../AssetManager.h"
#include <SDL3/SDL.h>
namespace opendash::engine
{

void Director::start() {
    lastTime_ = getTime();
}

void Director::end() {
    platform::Window::setInputScheduler(nullptr);

    AssetManager::get()->releaseAllTextures();
}

void Director::computeDeltaTime() {
    double now = getTime();
    deltaTime_ = now - lastTime_;
    lastTime_ = now;
}

void Director::updateLayouts() {
    if (!currentScene_) {
        areLayoutsDirty_ = false;
        return;
    }

    currentScene_->traversePostorder([](Node* node) {
        if (node->getLayout())
            node->getLayout()->calculateMinSizeProjected();
    });

    currentScene_->layout();
    areLayoutsDirty_ = false;
}

void Director::tick() {
    computeDeltaTime();

    Graphics* gfx = platform::Window::getGraphics();

    if (!gfx->beginDraw()) return;

    // projectionMatrix_ = glm::ortho(0.0f, windowSize.width, 0.0f, windowSize.height, -1.0f, 1.0f);

    gfx->setViewProjectionMatrix(getProjectionMatrix());
    
    currentScene_->update(deltaTime_);

    if (areLayoutsDirty_)
        updateLayouts();

    currentScene_->render(gfx);

    gfx->finishDraw();
}

void Director::setScene(std::unique_ptr<Scene> &scene) {
  currentScene_ = std::move(scene);
}

void Director::setContentScaleFactor(float contentScaleFactor) {
    if (contentScaleFactor <= 0.0f)
        return;
    contentScaleFactor_ = contentScaleFactor;
    invertedContentScaleFactor_ = 1.0f / contentScaleFactor;
}

void Director::setDesignResolutionSize(const Size &designResolutionSize) {
    designResolutionSize_ = designResolutionSize;
    updateScreenScale();
}

glm::mat4 Director::getProjectionMatrix() const {
    return projectionMatrix_;
}

Scene* Director::getRunningScene() const {
    return currentScene_.get();
}

const Size &Director::getDesignResolutionSize() const {
    return designResolutionSize_;
}

double Director::getTime() const {
    return platform::Window::getTime();
}

float Director::getDeltaTime() const {
    return deltaTime_;
}

float Director::getScreenScaleFactorMax() const
{
    return screenScaleFactorMax_;
}

const Size& Director::getVisibleSize() const {
    return visibleSize_;
}

const Size& Director::getFrameSize() const {
    return frameSize_;
}

Point Director::toWorldPosition(const Point& screenPos) const {
    glm::vec4 ret = inverseProjectionMatrix_ * glm::vec4((screenPos / frameSize_ * 2.0f - 1.0f).toGLM(), 0, 1);
    return {ret.x, ret.y};
}

Point Director::toScreenPosition(const Point& worldPos) const {
    glm::vec4 ret = projectionMatrix_ * glm::vec4(worldPos.toGLM(), 0, 1);
    return (Point(ret.x, ret.y) + 1.0f) / 2.0f * frameSize_;
}

bool Director::init() {
    platform::Window::setInputScheduler(&inputScheduler_);

    return true;
}

void Director::setVisibleSize(const Size &visibleSize) {
    visibleSize_ = visibleSize;
}

void Director::setFrameSize(const Size &frameSize) {
    frameSize_ = frameSize;
}

void Director::updateScreenScale()
{
    if (frameSize_.width <= 0.0f || frameSize_.height <= 0.0f) return;
    if (designResolutionSize_.width <= 0.0f || designResolutionSize_.height <= 0.0f) return;

    const Size& design = designResolutionSize_;
    const float scaleX = frameSize_.width  / design.width;
    const float scaleY = frameSize_.height / design.height;

    if (scaleY <= scaleX) { // ResolutionPolicy Fixed Height
        screenScale_ = scaleY;
        visibleSize_ = { frameSize_.width / scaleY, design.height };
    }
    else { // ResolutionPolicy Fixed Width
        screenScale_ = scaleX;
        visibleSize_ = { design.width, frameSize_.height / scaleX };
    }

    screenScaleFactorW_ = visibleSize_.width  / design.width;
    screenScaleFactorH_ = visibleSize_.height / design.height;
    screenScaleFactor_    = std::min(screenScaleFactorW_, screenScaleFactorH_);
    screenScaleFactorMax_ = std::max(screenScaleFactorW_, screenScaleFactorH_);

    projectionMatrix_ = glm::ortho(0.0f, visibleSize_.width, 0.0f, visibleSize_.height, -1.0f, 1.0f);
    inverseProjectionMatrix_ = glm::inverse(projectionMatrix_);
}

void Director::onWindowResized(const Size &frameSize)
{
    if (frameSize.width <= 0.0f || frameSize.height <= 0.0f)
        return; // minimized
    if (frameSize.width == frameSize_.width && frameSize.height == frameSize_.height)
        return;

    setFrameSize(frameSize);
    updateScreenScale();

    if (currentScene_)
        currentScene_->onViewResized();
}

}