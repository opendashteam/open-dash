#pragma once

#include <memory>
#include "AssetManager.h"
#include "Scene.h"

namespace opendash::engine
{
    
/*
    Represents the application process and its window.
*/
class Application {
public:
    static Application* get();

    static std::unique_ptr<Application> create(const std::string& title, int width, int height);
    bool init(const std::string& title, int width, int height);
    void run();
    void quit();
    const glm::mat4& getProjectionMatrix();
    
    void setScene(std::unique_ptr<Scene> scene);

private:
    static Application* instance_;

    std::unique_ptr<Scene> currentScene_ = nullptr;

    glm::mat4 projectionMatrix_{1.0f};
    int windowWidth_ = 0;
    int windowHeight_ = 0;

    std::unique_ptr<AssetManager> assetManager_ = nullptr;
};

}