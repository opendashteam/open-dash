#include "Application.h"
#include "../../platform/Window.h"
#include "../utilities/log.h"

namespace opendash::engine
{

Application* Application::instance_ = nullptr;

Application *Application::get() {
    return instance_;
}

std::unique_ptr<Application> Application::create(const std::string& title, int width, int height)
{
    auto ret = std::make_unique<Application>();

    if (!ret->init(title, width, height)) {
        return nullptr;
    }

    instance_ = ret.get();
    return ret;
}

bool Application::init(const std::string& title, int width, int height)
{
    if (!platform::Window::init(title, width, height))
        return false;

    // claim ownership of certain stuff
    assetManager_ = AssetManager::create();
    if (!assetManager_) return false;

    return true;
}

void Application::run() {
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

void Application::quit() {
    AssetManager::get()->releaseAllTextures();
    platform::Window::destroy();
}

void Application::setScene(std::unique_ptr<Scene> scene) {
    currentScene_ = std::move(scene);
}

const glm::mat4 &Application::getProjectionMatrix() {
    return projectionMatrix_;
}

}