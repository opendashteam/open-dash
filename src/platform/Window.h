#pragma once

#include <string>
#include <engine/types.h>
#include <engine/Graphics.h>

namespace opendash::platform
{

class Window {
public:
    static bool init(const std::string& title, int width, int height);

    static void destroy();

    // in seconds
    static double getTime();

    static bool shouldClose();

    static void pollEvents();

    static engine::Graphics* getGraphics();
};

};