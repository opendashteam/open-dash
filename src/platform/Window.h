#pragma once

#include <string_view>
#include <functional>
#include <set>
#include "engine/core/types.h"        // wherever Size lives; adjust the path

namespace opendash::engine { class Graphics; }

namespace opendash::platform
{

enum class GraphicsLibrary {
    Vulkan,
    Metal,
    Direct3D12,
};

inline const char* graphicsLibraryToString(GraphicsLibrary lib) {
    switch (lib) {
    case GraphicsLibrary::Vulkan: return "Vulkan";
    case GraphicsLibrary::Metal: return "Metal";
    case GraphicsLibrary::Direct3D12: return "Direct3D 12";
    default:
        return "<invalid>";
    }
}

class Window
{
public:
    using ResizeCallback = std::function<void(float width, float height)>;

    static bool init();
    static void quit();

    static bool create(
        std::string_view title,
        GraphicsLibrary library,
        int width,
        int height
    );
    static void destroy();

    static double getTime();

    // should be able to run before create() but after init()
    static const std::set<GraphicsLibrary>& getSupportedGraphicsLibraries();

    // called with the new size in pixels whenever it really changes
    static void setResizeCallback(ResizeCallback callback);

    static const std::filesystem::path& getAssetsDirectoryPath();

    // current drawable size in pixels (use this for the initial size)
    static engine::Size getPixelSize();

    static engine::Graphics* getGraphics();
};

}