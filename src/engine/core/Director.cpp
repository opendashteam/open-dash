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
    AssetManager::get()->releaseAllTextures();
}

void Director::computeDeltaTime() {
    double now = getTime();
    deltaTime_ = now - lastTime_;
    lastTime_ = now;
}

void Director::tick() {
    computeDeltaTime();

    Graphics* gfx = platform::Window::getGraphics();

    if (!gfx->beginDraw()) return;

    // projectionMatrix_ = glm::ortho(0.0f, windowSize.width, 0.0f, windowSize.height, -1.0f, 1.0f);
    
    currentScene_->update(deltaTime_);
    currentScene_->render(gfx);

    gfx->finishDraw();
}

void Director::setScene(std::unique_ptr<Scene> &scene) {
  currentScene_ = std::move(scene);
}

void Director::setContentScaleFactor(float contentScaleFactor) {
    contentScaleFactor_ = contentScaleFactor;
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

float Director::getContentScaleFactor() const {
    return contentScaleFactor_;
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

bool Director::init() {
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