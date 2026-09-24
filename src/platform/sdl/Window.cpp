#include "../Window.h"
#include "engine/utilities/log.h"
#include <SDL3/SDL.h>
#include "SDLGPUGraphics.h"

using namespace opendash::engine;

namespace opendash::platform
{

static SDL_Window* window;
static u64 startTick = 0;
static Window::ResizeCallback resizeCallback;
static SDLGPUGraphics* graphics = nullptr;

bool Window::init(std::string_view title, int width, int height)
{
    SDL_Init(SDL_INIT_VIDEO);

    std::string titleString{title};
    window = SDL_CreateWindow(titleString.c_str(), width, height, SDL_WINDOW_RESIZABLE);
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
    resizeCallback = nullptr;
}

void Window::setResizeCallback(ResizeCallback callback)
{
    resizeCallback = std::move(callback);
}

engine::Size Window::getPixelSize()
{
    int w = 0, h = 0;
    if (window)
        SDL_GetWindowSizeInPixels(window, &w, &h);
    return { static_cast<float>(w), static_cast<float>(h) };
}

double Window::getTime() {
    return (double)(SDL_GetTicksNS() - startTick) / 1'000'000'000.0;
}

engine::Graphics* Window::getGraphics()
{
    return graphics;
}

bool windowHandleEvent(const SDL_Event& event) {
    if (!window)
        return false;

    switch (event.type)
    {
        case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
            return false;

        case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
            if (resizeCallback)
                resizeCallback(static_cast<float>(event.window.data1), static_cast<float>(event.window.data2));
            break;
    }
    return true;
}

};