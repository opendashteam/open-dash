#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "engine/game.h"

namespace opendash::platform
{
// This is implemented in sdl/Window.cpp
bool windowHandleEvent(const SDL_Event& event);
};

SDL_AppResult SDL_AppInit(void** appstate, int argc, char** argv) {
    if (!game::init()) 
        return SDL_APP_FAILURE;

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
    return opendash::platform::windowHandleEvent(*event)
        ? SDL_APP_CONTINUE
        : SDL_APP_SUCCESS; // false = quit requested
}

SDL_AppResult SDL_AppIterate(void* appstate) {
    return game::loop() ? SDL_APP_CONTINUE : SDL_APP_SUCCESS;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {
    game::quit();
}