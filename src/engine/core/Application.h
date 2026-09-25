#pragma once

#include <memory>
#include "../AssetManager.h"
#include "Director.h"
#include "../../platform/Window.h"
#include <optional>

namespace opendash::engine
{
    
/*
    Represents the application process and its window.
*/
class Application {
public:
    static Application* get();

    static std::unique_ptr<Application> create(std::string_view title, int width, int height);
    bool init(std::string_view title, int width, int height);
    void run();
    void tick();
    void quit();
    void setScene(std::unique_ptr<Scene> scene);
    void gameSetup();

private:
    bool initWindow();

    bool attemptInitWithGfxLib(platform::GraphicsLibrary lib);

private:
    static Application* instance_;

    std::unique_ptr<AssetManager> assetManager_ = nullptr;
    std::unique_ptr<Director>     director_     = nullptr;

    std::optional<platform::GraphicsLibrary> graphicsLibrary_ = platform::GraphicsLibrary::Direct3D12;

    std::string_view initTitle;
    int initWidth, initHeight;
};

}