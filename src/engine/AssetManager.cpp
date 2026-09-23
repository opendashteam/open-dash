#include "AssetManager.h"
#include "core/Application.h"
#include "../platform/Window.h"
#include "utilities/log.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../third_party/stb_image.h"

#include <fstream>

namespace opendash::engine
{

AssetManager* AssetManager::instance_ = nullptr;

AssetManager* AssetManager::get() {
    return instance_;
}

std::unique_ptr<AssetManager> AssetManager::create() {
    auto ret = std::make_unique<AssetManager>();

    if (!ret->init()) {
        return nullptr;
    }

    instance_ = ret.get();
    return ret;
}

std::filesystem::path AssetManager::getFullPath(const std::filesystem::path& relative) {
    std::filesystem::path texturePath = std::filesystem::path(kParentDirectory) / relative;
    return texturePath;
}

bool AssetManager::readFileAsString(const std::filesystem::path& relativePath, std::string& outputString) {
    std::ifstream file = std::ifstream(getFullPath(relativePath));
    if (!file.is_open())
        return false;

    file.seekg(0, std::ios::end);
    size_t size = file.tellg();
    outputString.resize(size);
    file.seekg(0);
    file.read(outputString.data(), size); 
    file.close();
    return true;
}

/*
    Store a texture file from disk given a path relative to the assets directory, e.g. "cube.png".
    The resulting texture is stored under "<assets_dir>/<path>".
*/
bool AssetManager::cacheTexture(const std::filesystem::path& relativePath) {
    if (isTextureCached(relativePath)) return true;

    int width;
    int height;
    auto texturePath = getFullPath(relativePath);

    stbi_uc* pixels = stbi_load(texturePath.string().c_str(), &width, &height, nullptr, STBI_rgb_alpha);
    if (!pixels) {
        log::err("Could not cache texture {}, failed to load image: {}", relativePath.string(), stbi_failure_reason());
        return false;
    }

    Graphics* gfx = platform::Window::getGraphics();

    InternalTexture texture = gfx->createTexture(
        width,
        height,
        Graphics::TextureFormat::RGBA_UBYTE, // it only supports this lol
        pixels
    );

    stbi_image_free(pixels);

    if (!texture) {
        log::err("Failed to upload texture {} to gpu", relativePath.string());
        return false;
    }

    cachedTextures_[texturePath] = new Texture(width, height, texture);

    return true;
}

bool AssetManager::isTextureCached(const std::filesystem::path &relativePath) {
    return cachedTextures_.contains(getFullPath(relativePath));
}

Texture* AssetManager::getCachedTexture(const std::filesystem::path &relativePath) {
    auto it = cachedTextures_.find(getFullPath(relativePath));
    if (it != cachedTextures_.end())
        return it->second;

    log::err("Failed to get cached texture from path, texture is not cached: {}", relativePath.string());
    return nullptr;
}

void AssetManager::releaseAllTextures()
{
    Graphics* gfx = platform::Window::getGraphics();

    for (auto& [path, tex] : cachedTextures_) {
        gfx->destroyTexture(tex->getInternalObject());

    }
    cachedTextures_.clear();
}

bool AssetManager::init()
{
    return true;
}
}