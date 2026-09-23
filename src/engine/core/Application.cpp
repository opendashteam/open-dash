#include "Application.h"
#include "../../platform/Window.h"

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

    // claim ownership of singleton managers and stuff
    assetManager_ = AssetManager::create();
    if (!assetManager_) return false;

    director_ = Director::create();
    if (!director_) return false;

    director_->setWindowSize({
        static_cast<float>(width),
        static_cast<float>(height)
    });

    return true;
}

void Application::run() {
    director_->start();
}

void Application::quit() {
    director_->end();
    platform::Window::destroy();
}

void Application::setScene(std::unique_ptr<Scene> scene) {
    if (director_) director_->setScene(scene);  
}

}