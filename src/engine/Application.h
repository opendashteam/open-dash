#pragma once

#include <SDL3/SDL.h>
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

    static std::unique_ptr<Application> create(const char* title, int width, int height);
    bool init(const char* title, int width, int height);
    void run();
    void quit();
    void pollEvents();
    bool tryGetSwapchainTexture();

    // getters
    SDL_GPUDevice* getDevice(); 
    SDL_Window* getWindow();
    SDL_GPUTexture* getSwapchainTexture();
    SDL_GPUCommandBuffer* getCommandBuffer();
    SDL_GPUGraphicsPipeline* getPipeline();
    const glm::mat4& getProjectionMatrix();

    // setters
    void setScene(std::unique_ptr<Scene> scene);
    void setPipeline(SDL_GPUGraphicsPipeline* pipeline);

private:
    static Application* instance_;

    SDL_Window* window_ = nullptr;
    SDL_GPUDevice* device_ = nullptr;
    SDL_GPUGraphicsPipeline* pipeline_ = nullptr;
    SDL_GPUCommandBuffer* commandBuffer_ = nullptr;
    bool running_ = false;
    SDL_Event _event{};
    SDL_GPUTexture* swapchainTexture_ = nullptr;

    std::unique_ptr<Scene> currentScene_ = nullptr;

    glm::mat4 projectionMatrix_{1.0f};
    int windowWidth_ = 0;
    int windowHeight_ = 0;

    // ownership
    std::unique_ptr<AssetManager> assetManager_ = nullptr;
};

}