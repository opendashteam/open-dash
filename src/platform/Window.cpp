#include "Window.h"
#include "engine/log.h"
#include <SDL3/SDL.h>
#include "SDLGPUGraphics.h"

using namespace opendash::engine;

namespace opendash::platform
{

static SDL_Window* window;
static bool shouldCloseWindow = false;
static u64 startTick = 0;

static SDLGPUGraphics* graphics = nullptr;

bool Window::init(const std::string& title, int width, int height)
{
    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow(title.c_str(), width, height, 0);
    if (!window)
    {
        log::err("failed to create a SDL window");
        return false;
    }

    startTick = SDL_GetTicksNS();

    graphics = SDLGPUGraphics::create(window);
    if (!graphics) {
        destroy();
        return false;
    }

    return true;
}

void Window::destroy()
{
    if (graphics)
        delete graphics;
    if (window)
        SDL_DestroyWindow(window);
    graphics = nullptr;
    window = nullptr;
}

double Window::getTime() {
    return (double)(SDL_GetTicksNS() - startTick) / 1'000'000'000.0;
}

bool Window::shouldClose()
{
    return shouldCloseWindow;
}

void Window::pollEvents()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED)
        {
            shouldCloseWindow = true;
            continue;
        }
        // later: pass other events (input, etc.) down to the current scene (or keep them here idk)
    }
}

engine::Graphics* Window::getGraphics()
{
    return graphics;
}

};