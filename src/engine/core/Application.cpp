#include "Application.h"
#include "../../platform/Window.h"

namespace opendash::engine
{

Application* Application::instance_ = nullptr;

Application *Application::get() {
    return instance_;
}

std::unique_ptr<Application> Application::create(std::string_view title, int width, int height)
{
    auto ret = std::make_unique<Application>();

    if (!ret->init(title, width, height)) {
        return nullptr;
    }

    instance_ = ret.get();
    return ret;
}

bool Application::init(std::string_view title, int width, int height)
{
    if (!platform::Window::init(title, width, height))
        return false;

    // claim ownership of singleton managers and stuff
    assetManager_ = AssetManager::create();
    if (!assetManager_) return false;

    director_ = Director::create();
    if (!director_) return false;

    director_->setFrameSize({
        static_cast<float>(width),
        static_cast<float>(height)
    });

    gameSetup();

    platform::Window::setResizeCallback([this](float w, float h) {
        director_->onWindowResized({w, h});
    });
    director_->onWindowResized(platform::Window::getPixelSize());

    return true;
}

void Application::run() {
    director_->start();
}

void Application::tick() {
    director_->tick();
}

void Application::quit() {
    director_->end();
    platform::Window::destroy();
}

void Application::setScene(std::unique_ptr<Scene> scene) {
    if (director_) director_->setScene(scene);  
}

void Application::gameSetup() { // Run game-specific setup code

                                 // uhd for now
    director_->setContentScaleFactor(4.0f); // to be read from save file
    director_->setDesignResolutionSize({480.0f, 320.0f}); // GD's own size, do not change
    
}

}