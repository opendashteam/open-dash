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
    // claim ownership of singleton managers and stuff
    // (asset manager needs to be created first)
    assetManager_ = AssetManager::create();
    if (!assetManager_) return false;

    if (!platform::Window::init())
        return false;

    initTitle = title;
    initWidth = width;
    initHeight = height;

    if (!initWindow()) {
        platform::Window::quit();
        return false;
    }

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
    // Making sure director gets deleted before we finalize graphics
    director_ = nullptr;
    platform::Window::destroy();
    platform::Window::quit();
}

void Application::setScene(std::unique_ptr<Scene> scene) {
    if (director_) director_->setScene(scene);  
}

void Application::gameSetup() { // Run game-specific setup code

                                 // uhd for now
    director_->setContentScaleFactor(4.0f); // to be read from save file
    director_->setDesignResolutionSize({480.0f, 320.0f}); // GD's own size, do not change
    
}

bool Application::initWindow() {
    auto gfxLibs = platform::Window::getSupportedGraphicsLibraries();

    log::info("Available graphics libraries:");
    for (const auto& lib : gfxLibs)
        log::info("- {}", platform::graphicsLibraryToString(lib));

    if (graphicsLibrary_ && gfxLibs.contains(graphicsLibrary_.value())) {
        if (attemptInitWithGfxLib(graphicsLibrary_.value()))
            return true;
        gfxLibs.erase(graphicsLibrary_.value());
    }

    if (gfxLibs.contains(platform::GraphicsLibrary::Vulkan)) {
        // attempt with vulkan first
        if (attemptInitWithGfxLib(platform::GraphicsLibrary::Vulkan))
            return true;
        gfxLibs.erase(platform::GraphicsLibrary::Vulkan);
    }

    for (const auto& lib : gfxLibs) {
        if (attemptInitWithGfxLib(lib))
            return true;
    }

    log::err("Failed to find a working graphics library to start with");
    return false;
}

bool Application::attemptInitWithGfxLib(platform::GraphicsLibrary lib) {
    log::info("Initializing graphics with {}", platform::graphicsLibraryToString(lib));

    return platform::Window::create(initTitle, lib, initWidth, initHeight);
}

}