#pragma once

#include <string_view>
#include "../engine/core/Graphics.h"

union SDL_Event; // Forward decl
namespace opendash::platform
{

class Window {
public:
    static bool init(std::string_view title, int width, int height);

    static void destroy();

    // in seconds
    static double getTime();

    static bool handleEvent(const SDL_Event& event);

    static engine::Graphics* getGraphics();
};

};