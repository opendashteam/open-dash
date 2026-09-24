#pragma once

#include <string_view>
#include <functional>
#include "engine/core/types.h"        // wherever Size lives; adjust the path

namespace opendash::engine { class Graphics; }

namespace opendash::platform
{

class Window
{
public:
    using ResizeCallback = std::function<void(float width, float height)>;

    static bool init(std::string_view title, int width, int height);
    static void destroy();

    static double getTime();

    // called with the new size in pixels whenever it really changes
    static void setResizeCallback(ResizeCallback callback);

    // current drawable size in pixels (use this for the initial size)
    static engine::Size getPixelSize();

    static engine::Graphics* getGraphics();
};

}