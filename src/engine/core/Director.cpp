#include "Director.h"
#include "../../platform/Window.h"
#include "../AssetManager.h"

namespace opendash::engine
{

void Director::start() {
    double lastTime = platform::Window::getTime();

    while (!platform::Window::shouldClose()) {
        // compute delta time
        double now = platform::Window::getTime();
        float deltaTime = now - lastTime;
        lastTime = now;

        platform::Window::pollEvents();

        Graphics* gfx = platform::Window::getGraphics();

        Size windowSize;
        if (!gfx->beginDraw(currentScene_->getClearColor(), windowSize)) {
            continue;
        }

        projectionMatrix_ = glm::ortho(0.0f, windowSize.width, 0.0f, windowSize.height, -1.0f, 1.0f);

        currentScene_->update(deltaTime);
        currentScene_->render(gfx);

        gfx->finishDraw();
    }
}

void Director::end() {
    AssetManager::get()->releaseAllTextures();
}

void Director::setScene(std::unique_ptr<Scene>& scene) {
    currentScene_ = std::move(scene);
}

glm::mat4 Director::getProjectionMatrix() const {
    return projectionMatrix_;
}

Scene* Director::getRunningScene() const {
    return currentScene_.get();
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