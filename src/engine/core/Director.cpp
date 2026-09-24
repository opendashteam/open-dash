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

    Size windowSize;
    if (!gfx->beginDraw(currentScene_->getClearColor(), windowSize))
        return;

    projectionMatrix_ = glm::ortho(0.0f, windowSize.width, 0.0f, windowSize.height, -1.0f, 1.0f);
    
    currentScene_->update(deltaTime_);
    currentScene_->render(gfx);

    gfx->finishDraw();
}

void Director::setScene(std::unique_ptr<Scene> &scene) {
  currentScene_ = std::move(scene);
}

void Director::setResolutionPolicy(const ResolutionPolicy &policy) {
    resolutionPolicy_ = policy;
}

void Director::setContentScaleFactor(float contentScaleFactor) {
    contentScaleFactor_ = contentScaleFactor;
}

void Director::setDesignResolutionSize(const Size &designResolutionSize) {
    designResolutionSize_ = designResolutionSize;
}

glm::mat4 Director::getProjectionMatrix() const {
    return projectionMatrix_;
}

Scene* Director::getRunningScene() const {
    return currentScene_.get();
}

const ResolutionPolicy &Director::getResolutionPolicy() const {
    return resolutionPolicy_;
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

const Size& Director::getVisibleSize() const {
    return visibleSize_;
}

const Size& Director::getWindowSize() const {
    return windowSize_;
}

bool Director::init() {
    return true;
}

void Director::setVisibleSize(const Size &visibleSize) {
    visibleSize_ = visibleSize;
}

void Director::setWindowSize(const Size &windowSize) {
    windowSize_ = windowSize;
}

}