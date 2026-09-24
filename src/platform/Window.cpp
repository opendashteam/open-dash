#include "Window.h"
#include "engine/utilities/log.h"
#include <SDL3/SDL.h>
#include "SDLGPUGraphics.h"

using namespace opendash::engine;

namespace opendash::platform
{

static SDL_Window* window;
static u64 startTick = 0;

static SDLGPUGraphics* graphics = nullptr;

bool Window::init(std::string_view title, int width, int height)
{
    SDL_Init(SDL_INIT_VIDEO);

    std::string titleString{title};
    window = SDL_CreateWindow(titleString.c_str(), width, height, 0);
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

bool Window::handleEvent(const SDL_Event& event) {
    if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED)
        return false;

    // later: forward input to the Director
    return true;
}

engine::Graphics* Window::getGraphics()
{
    return graphics;
}

};