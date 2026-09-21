#pragma once

#include "Node.h"
#include "filesystem"

namespace opendash::engine
{

class Sprite : public Node {
public:
    static std::unique_ptr<Sprite> create(const std::filesystem::path& path);
    static SDL_GPUBuffer* getSharedQuadBuffer();
protected:
    bool initWithPath(const std::filesystem::path& path);
    void draw(SDL_GPURenderPass* pass, SDL_GPUGraphicsPipeline* pipeline, SDL_GPUCommandBuffer* commandBuffer) override;
    bool init() override;
private:
    Texture* texture_;

    static SDL_GPUBuffer* sharedQuadVertexBuffer_;
    static bool sharedQuadBufferInitialized_;

    static bool ensureSharedQuadBuffer();
};

}