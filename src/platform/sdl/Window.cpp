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
static bool graphicsLibsDirty = true;
static std::set<GraphicsLibrary> graphicsLibraries;


static std::filesystem::path assetsDirectory;
static std::filesystem::path gameSaveDirectory;

bool Window::init() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        log::err("SDL_Init failed: ", SDL_GetError());
        return false;
    }

    const char* path = SDL_GetBasePath();
    if (!path) {
        log::err("SDL_GetBasePath failed: {}", SDL_GetError());
        return false;
    }
    assetsDirectory = std::filesystem::path(path) / "assets";

    path = SDL_GetPrefPath(GAME_SAVE_ORG, GAME_SAVE_NAME);
    if (!path) {
        log::err("SDL_GetPrefPath failed: {}", SDL_GetError());
        return false;
    }
    gameSaveDirectory = path;

    return true;
}

void Window::quit() {
    SDL_Quit();
}

bool Window::create(
    std::string_view title,
    GraphicsLibrary library,
    int width,
    int height
) {
    std::string titleString{title};
    window = SDL_CreateWindow(titleString.c_str(), width, height, SDL_WINDOW_RESIZABLE);
    if (!window)
    {
        log::err("Failed to create a SDL window");
        return false;
    }

    startTick = SDL_GetTicksNS();

    graphics = SDLGPUGraphics::create(window, library);
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

const std::filesystem::path& Window::getAssetsDirectoryPath() {
    return assetsDirectory;
}

const std::filesystem::path& Window::getGameSavePath() {
    return gameSaveDirectory;
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

const std::set<GraphicsLibrary>& Window::getSupportedGraphicsLibraries() {
    if (graphicsLibsDirty) {
        graphicsLibraries.clear();

        u32 index = 0;
        while (const char* name = SDL_GetGPUDriver(index)) {
            if (strcmp(name, "vulkan") == 0)
                graphicsLibraries.insert(GraphicsLibrary::Vulkan);
            else if (strcmp(name, "metal") == 0)
                graphicsLibraries.insert(GraphicsLibrary::Metal);
            if (strcmp(name, "direct3d12") == 0)
                graphicsLibraries.insert(GraphicsLibrary::Direct3D12);
            index++;
        }

        graphicsLibsDirty = false;
    }
    return graphicsLibraries;
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