#include "AssetManager.h"
#include "core/Graphics.h"
#include "../platform/Window.h"
#include "utilities/log.h"
#include "utilities/PList.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../third_party/stb_image.h"

#include <fstream>

namespace opendash::engine
{

std::filesystem::path AssetManager::getFullPath(const std::filesystem::path& relative) {
    std::filesystem::path texturePath = std::filesystem::path(PARENT_DIRECTORY) / relative;
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

    InternalTexture texture = gfx->textureCreate(
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

bool AssetManager::loadSpriteSheet(const std::filesystem::path& relativePath) {
    auto texturePath = relativePath;
    texturePath.replace_extension(".png");

    if (!cacheTexture(texturePath))
        return false;

    Texture* texture = getCachedTexture(texturePath);

    auto plistPath = relativePath;
    plistPath.replace_extension(".plist");

    std::unique_ptr<PList> plist = PList::load(plistPath);
    if (!plist) {
        log::err("{}: Failed to load .plist file", plistPath.string());
        return false;
    }

    if (!plist->isDict()) {
        log::err("{}: PList root node is not a dictionary", plistPath.string());
        return false;
    }

    PList* frames = plist->getNode("frames");

    if (!frames || !frames->isDict()) {
        log::err("{}: Expected 'frames' node inside of PList root", plistPath.string());
        return false;
    }

    std::vector<SpriteFrame*> spriteFrames;
    spriteFrames.reserve(frames->getDict().size());

    for (const auto& [key, value] : frames->getDict()) {
        SpriteFrame* frame = SpriteFrame::loadFromPListNode(texture, key, value);
        if (!frame) {
            log::err("{}: Failed to parse sprite frame '{}'", plistPath.string(), key);
            return false;
        }
        spriteFrames.push_back(frame);
    }

    for (auto frame : spriteFrames)
        spriteFrames_[frame->getName()] = frame;

    return true;
}

void AssetManager::releaseAllTextures()
{
    for (auto [key, value] : spriteFrames_)
        delete value;

    spriteFrames_.clear();

    Graphics* gfx = platform::Window::getGraphics();

    for (auto& [path, texture] : cachedTextures_) {
        gfx->textureDestroy(texture->getInternalObject());
        delete texture;
    }

    cachedTextures_.clear();
}

bool AssetManager::init()
{
    return true;
}
}