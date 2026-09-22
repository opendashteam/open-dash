#include "AssetManager.h"
#include "Application.h"

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
    if (isTextureCached(relativePath)) return true; // caching succeeded

    int texWidth;
    int texHeight;

    auto device = Application::get()->getDevice();
    if (!device) return false;

    // load from disk
    auto texturePath = getFullPath(relativePath);

    stbi_uc* pixels = stbi_load(texturePath.string().c_str(), &texWidth, &texHeight, nullptr, STBI_rgb_alpha);

    if (!pixels) {
        SDL_Log("Could not cache texture, failed to load image: %s", stbi_failure_reason());
        return false;
    }

    // create the gpu texture
    SDL_GPUTextureCreateInfo textureInfo{};
    textureInfo.type = SDL_GPU_TEXTURETYPE_2D;
    textureInfo.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
    textureInfo.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER;
    textureInfo.width = texWidth;
    textureInfo.height = texHeight;
    textureInfo.layer_count_or_depth = 1;
    textureInfo.num_levels = 1;

    SDL_GPUTexture* texture = SDL_CreateGPUTexture(device, &textureInfo);
    if (!texture) {
        SDL_Log("Could not cache texture, GPU texture creation failed: %s", SDL_GetError());
        stbi_image_free(pixels);
        return false;
    }

    // transfer info
    SDL_GPUTransferBufferCreateInfo texTransferInfo{};
    texTransferInfo.size = texWidth * texHeight * 4;
    texTransferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
    SDL_GPUTransferBuffer* texTransfer = SDL_CreateGPUTransferBuffer(device, &texTransferInfo);

    void* texMapped = SDL_MapGPUTransferBuffer(device, texTransfer, false);
    SDL_memcpy(texMapped, pixels, texWidth * texHeight * 4);
    SDL_UnmapGPUTransferBuffer(device, texTransfer);
    stbi_image_free(pixels);

    // copy pass
    SDL_GPUCommandBuffer* uploadCmd = SDL_AcquireGPUCommandBuffer(device);
    SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(uploadCmd);

    SDL_GPUTextureTransferInfo texTransferInfo2{};
    texTransferInfo2.transfer_buffer = texTransfer;

    SDL_GPUTextureRegion texRegion{};
    texRegion.texture = texture;
    texRegion.w = texWidth;
    texRegion.h = texHeight;
    texRegion.d = 1;

    SDL_UploadToGPUTexture(copyPass, &texTransferInfo2, &texRegion, false);

    SDL_EndGPUCopyPass(copyPass);
    SDL_SubmitGPUCommandBuffer(uploadCmd);

    SDL_ReleaseGPUTransferBuffer(device, texTransfer);

    // create the sampler
    SDL_GPUSamplerCreateInfo samplerInfo{};
    samplerInfo.min_filter = SDL_GPU_FILTER_LINEAR; // TODO create a method on Sprite to change filter to nearest
    samplerInfo.mag_filter = SDL_GPU_FILTER_LINEAR;
    samplerInfo.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_LINEAR;
    samplerInfo.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    samplerInfo.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;

    SDL_GPUSampler* sampler = SDL_CreateGPUSampler(device, &samplerInfo);
    if (!sampler) {
        SDL_Log("Could not cache texture, sampler creation failed: %s", SDL_GetError());
        SDL_ReleaseGPUTexture(device, texture);
        return false;
    }

    Texture tex{texture, sampler, texWidth, texHeight};

    cachedTextures_.emplace(texturePath, tex);
    return true;
}

bool AssetManager::isTextureCached(const std::filesystem::path &relativePath) {
    return cachedTextures_.contains(getFullPath(relativePath));
}

Texture* AssetManager::getCachedTexture(const std::filesystem::path &relativePath) {
        
    auto it = cachedTextures_.find(getFullPath(relativePath));
    if (it != cachedTextures_.end()) {
        return &it->second;
    }

    SDL_Log("Failed to get cached texture from path, texture is not cached: %s", relativePath.string().c_str());
    return nullptr;
}

void AssetManager::releaseAllTextures(SDL_GPUDevice* device)
{
    for (auto& [path, tex] : cachedTextures_) {
        SDL_ReleaseGPUSampler(device, tex.sampler);
        SDL_ReleaseGPUTexture(device, tex.gpuTexture);
    }
    cachedTextures_.clear();
}

bool AssetManager::init()
{
    return true;
}
}