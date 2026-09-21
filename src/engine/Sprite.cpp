#include "Sprite.h"
#include "AssetManager.h"
#include "Application.h"

namespace opendash::engine
{

SDL_GPUBuffer* Sprite::sharedQuadVertexBuffer_ = nullptr;
bool Sprite::sharedQuadBufferInitialized_ = false;

std::unique_ptr<Sprite> Sprite::create(const std::filesystem::path& path) {
    auto ret = std::make_unique<Sprite>();

    if (!ret->initWithPath(path)) {
        return nullptr;
    }

    return ret;
}

SDL_GPUBuffer *Sprite::getSharedQuadBuffer()
{
    return Sprite::sharedQuadVertexBuffer_;
}

bool Sprite::initWithPath(const std::filesystem::path& path) {

    auto* am = AssetManager::get();
    if (!am->isTextureCached(path) && !am->cacheTexture(path)) {
        SDL_Log("Failed to initialize sprite, could not load texture at path: %s", path.string().c_str());
        return false;
    }

    texture_ = am->getCachedTexture(path);
    if (!texture_) {
        return false;
    }

    setContentSize(static_cast<float>(texture_->width), static_cast<float>(texture_->height));

    return ensureSharedQuadBuffer();
}

void Sprite::draw(SDL_GPURenderPass* pass, SDL_GPUGraphicsPipeline* pipeline, SDL_GPUCommandBuffer* commandBuffer) {
    auto* app = Application::get();
    glm::mat4 contentScale = glm::scale(glm::mat4(1.0f), glm::vec3(getContentWidth(), getContentHeight(), 1.0f));
    glm::mat4 mvp = app->getProjectionMatrix() * getWorldTransform() * contentScale;
    SDL_PushGPUVertexUniformData(commandBuffer, 0, &mvp, sizeof(mvp));

    SDL_BindGPUGraphicsPipeline(pass, pipeline);

    SDL_GPUBufferBinding vertexBinding{};
    vertexBinding.buffer = Sprite::getSharedQuadBuffer();
    SDL_BindGPUVertexBuffers(pass, 0, &vertexBinding, 1);

    SDL_GPUTextureSamplerBinding textureBinding{};
    textureBinding.texture = texture_->gpuTexture;
    textureBinding.sampler = texture_->sampler;
    SDL_BindGPUFragmentSamplers(pass, 0, &textureBinding, 1);

    SDL_DrawGPUPrimitives(pass, 6, 1, 0, 0);
}

bool Sprite::init() {
    return Node::init();
}

bool Sprite::ensureSharedQuadBuffer() {
    if (sharedQuadBufferInitialized_) return true;

    auto app = Application::get();
    if (!app) return false;

    auto device = app->getDevice();
    if (!device) return false;

    static const Vertex quadVertices[] = {
        {-0.5f,  0.5f, 0.0f, 0.0f},
        { 0.5f,  0.5f, 1.0f, 0.0f},
        {-0.5f, -0.5f, 0.0f, 1.0f},
        { 0.5f, -0.5f, 1.0f, 1.0f},
        {-0.5f, -0.5f, 0.0f, 1.0f},
        { 0.5f,  0.5f, 1.0f, 0.0f}
    };

    SDL_GPUCommandBuffer* uploadCmd = SDL_AcquireGPUCommandBuffer(device);
    SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(uploadCmd);

    SDL_GPUBufferCreateInfo bufferInfo{};
    bufferInfo.size = sizeof(quadVertices);
    bufferInfo.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
    sharedQuadVertexBuffer_ = SDL_CreateGPUBuffer(device, &bufferInfo);
    if (!sharedQuadVertexBuffer_) {
        SDL_Log("Failed to create shared quad vertex buffer: %s", SDL_GetError());
        return false;
    }

    SDL_GPUTransferBufferCreateInfo vbTransferInfo{};
    vbTransferInfo.size = sizeof(quadVertices);
    vbTransferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
    SDL_GPUTransferBuffer* vbTransfer = SDL_CreateGPUTransferBuffer(device, &vbTransferInfo);
    if (!vbTransfer) {
        SDL_Log("Failed to create transfer buffer for shared quad: %s", SDL_GetError());
        SDL_ReleaseGPUBuffer(device, sharedQuadVertexBuffer_);
        sharedQuadVertexBuffer_ = nullptr;
        return false;
    }

    void* vbMapped = SDL_MapGPUTransferBuffer(device, vbTransfer, false);
    SDL_memcpy(vbMapped, quadVertices, sizeof(quadVertices));
    SDL_UnmapGPUTransferBuffer(device, vbTransfer);

    SDL_GPUTransferBufferLocation vbLocation{};
    vbLocation.transfer_buffer = vbTransfer;
    SDL_GPUBufferRegion vbRegion{};
    vbRegion.buffer = sharedQuadVertexBuffer_;
    vbRegion.size = sizeof(quadVertices);

    SDL_UploadToGPUBuffer(copyPass, &vbLocation, &vbRegion, false);
    SDL_EndGPUCopyPass(copyPass);
    SDL_SubmitGPUCommandBuffer(uploadCmd);
    SDL_ReleaseGPUTransferBuffer(device, vbTransfer);


    sharedQuadBufferInitialized_ = true;
    return sharedQuadVertexBuffer_ != nullptr;
}

}