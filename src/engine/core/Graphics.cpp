#include "Graphics.h"
#include "../../platform/Window.h"

namespace opendash::engine {

Graphics* Graphics::get() {
    return platform::Window::getGraphics();
}

};