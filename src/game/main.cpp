#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "ExampleScene.h"
#include "../platform/Window.h"

using namespace opendash;

SDL_AppResult SDL_AppInit(void** appstate, int argc, char** argv) {

    auto app = engine::Application::create("Open Dash", 1280, 720);
    if (!app) 
        return SDL_APP_FAILURE;

    if (!AssetManager::get()->loadSpriteSheet("GJ_GameSheet03-uhd"))
        return SDL_APP_FAILURE;

    auto scene = ExampleScene::create();
    if (!scene) 
        return SDL_APP_FAILURE;

    app->setScene(std::move(scene));
    app->run();

    *appstate = app.release(); // must outlive this function
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
    return platform::Window::handleEvent(*event)
        ? SDL_APP_CONTINUE
        : SDL_APP_SUCCESS; // false = quit requested
}

SDL_AppResult SDL_AppIterate(void* appstate) {
    static_cast<engine::Application*>(appstate)->tick();
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {
    if (auto* app = static_cast<engine::Application*>(appstate)) {
        app->quit();
        delete app;
    }
}