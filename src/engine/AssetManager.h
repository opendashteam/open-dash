#pragma once

#include <unordered_map>
#include <filesystem>
#include "core/types.h"
#include "core/Singleton.h"
#include "Texture.h"
#include "core/SpriteFrame.h"
#include "utilities/log.h"

namespace opendash::engine
{

/*
    Singleton class that handles loading/releasing assets. Use to preload
    textures and sounds on the loading screen.
*/
class AssetManager : public Singleton<AssetManager> {
    friend class Singleton<AssetManager>;
public:
    std::filesystem::path getFullPath(const std::filesystem::path& relative);
    bool readFileAsString(const std::filesystem::path& relativePath, std::string& outputString);
    bool readFileAsBinaryData(const std::filesystem::path& relativePath, std::vector<u8>& outputData);
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

    Texture* fetchTexture(const std::filesystem::path& path);

    inline const std::unordered_map<std::string, SpriteFrame*>& getSpriteFrames() const {
        return spriteFrames_;
    }

    inline SpriteFrame* getSpriteFrameByName(const std::string& name) const {
        auto it = spriteFrames_.find(name);
        if (it != spriteFrames_.end())
            return it->second;
        return nullptr;
    }

protected:
    bool init() override;
private:
    std::unordered_map<std::filesystem::path, Texture*, PathHash> cachedTextures_;

    std::unordered_map<std::string, SpriteFrame*> spriteFrames_;
};

}