#include "Application.h"
#include "Sprite.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace opendash::engine
{

Application* Application::instance_ = nullptr;

Application *Application::get() {
    return instance_;
}

std::unique_ptr<Application> Application::create(const char* title, int width, int height)
{
    auto ret = std::make_unique<Application>();

    if (!ret->init(title, width, height)) {
        return nullptr;
    }

    instance_ = ret.get();
    return ret;
}

bool Application::init(const char* title, int width, int height)
{
    SDL_Init(SDL_INIT_VIDEO);

    window_ = SDL_CreateWindow(title, width, height, 0);
    if (!window_) return false;

    windowWidth_ = width;
    windowHeight_ = height;
    projectionMatrix_ = glm::ortho(0.0f, (float)windowWidth_, (float)windowHeight_, 0.0f, -1.0f, 1.0f);

    device_ = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, true, nullptr);
    if (!device_) return false;

    if (!SDL_ClaimWindowForGPUDevice(device_, window_))
        return false;

    // claim ownership of certain stuff
    assetManager_ = AssetManager::create();
    if (!assetManager_) return false;

    return true;
}

void Application::run() {
    running_ = true;

    while (running_) {
        pollEvents();

        commandBuffer_ = SDL_AcquireGPUCommandBuffer(device_);

        if (!tryGetSwapchainTexture()) continue;

        currentScene_->render(pipeline_, commandBuffer_, swapchainTexture_);
    }
}

void Application::quit() {
    if (assetManager_)
        assetManager_->releaseAllTextures(device_);

    if (auto sharedQuadBuffer = Sprite::getSharedQuadBuffer())
        SDL_ReleaseGPUBuffer(device_, sharedQuadBuffer);

    SDL_ReleaseGPUGraphicsPipeline(device_, pipeline_);
    SDL_ReleaseWindowFromGPUDevice(device_, window_);
    SDL_DestroyGPUDevice(device_);
    SDL_DestroyWindow(window_);
}

void Application::pollEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED) {
            running_ = false;
        }
        // later: pass other events (input, etc.) down to the current scene (or keep them here idk)
    }
}

bool Application::tryGetSwapchainTexture() {
    bool success = true;
    Uint32 w, h;

    SDL_WaitAndAcquireGPUSwapchainTexture(commandBuffer_, window_, &swapchainTexture_, &w, &h);

    if (!swapchainTexture_) {
        SDL_SubmitGPUCommandBuffer(commandBuffer_);
        success = false;
    }
    
    return success;
}

SDL_GPUDevice* Application::getDevice() {
    return device_;
}

SDL_Window* Application::getWindow() {
    return window_;
}

SDL_GPUTexture *Application::getSwapchainTexture() {
    return swapchainTexture_;
}

SDL_GPUCommandBuffer *Application::getCommandBuffer() {
    return commandBuffer_;
}

SDL_GPUGraphicsPipeline *Application::getPipeline()
{
    return pipeline_;
}

const glm::mat4 &Application::getProjectionMatrix() {
    return projectionMatrix_;
}

void Application::setScene(std::unique_ptr<Scene> scene) {
    currentScene_ = std::move(scene);
}

void Application::setPipeline(SDL_GPUGraphicsPipeline *pipeline) {
    pipeline_ = pipeline;
}

}