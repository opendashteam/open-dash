#pragma once

#include <unordered_map>
#include <filesystem>
#include "types.h"
#include "Texture.h"

namespace opendash::engine
{

/*
    Singleton class that handles loading/releasing assets. Use to preload
    textures and sounds on the loading screen.
*/
class AssetManager {
public:
    static constexpr const char* kParentDirectory = "assets/";

    static AssetManager* get();
    static std::unique_ptr<AssetManager> create();

    std::filesystem::path getFullPath(const std::filesystem::path& relative);

    bool readFileAsString(
        const std::filesystem::path& relativePath,
        std::string& outputString
    );

    bool cacheTexture(const std::filesystem::path& relativePath);

    bool isTextureCached(const std::filesystem::path& relativePath);

    /*
        Get a texture from cache given a path relative to the assets directory.
        Returns nullptr if the texture doesn't exist in the cache.
    */
    Texture* getCachedTexture(const std::filesystem::path& relativePath);

    /*
        Loads a cocos2d spritesheet that has .plist and .png.
        The extension should not be written in relativePath.
    */
    bool loadSpriteSheet(const std::filesystem::path& relativePath);

    /*
        Free all GPU textures and samplers that are stored.
    */
    void releaseAllTextures();

protected:
    virtual bool init();
private:
    static AssetManager* instance_;
    std::unordered_map<std::filesystem::path, Texture*, PathHash> cachedTextures_;
};

}