#pragma once

#include "Node.h"
#include <SDL3/SDL.h>
#include "macros.h"

namespace opendash::engine
{
/*
    Equivalent of cocos2d's CCScene. Only one scene can be running at a time.
*/
class Scene : public Node {
public:
    CREATE_FUNC(Scene)
    virtual void render(SDL_GPUGraphicsPipeline* pipeline, SDL_GPUCommandBuffer* commandBuffer, SDL_GPUTexture* swapchainTexture);
    virtual void update(float dt);
protected:
    bool init() override;
    virtual SDL_GPUShader* loadShader(SDL_GPUDevice* dev, const char* path, SDL_GPUShaderStage stage, Uint32 numSamplers, Uint32 numUniformBuffers);
    virtual bool setupShaders();
    virtual bool setupPipeline();
private:
    SDL_GPUShader* vertexShader_ = nullptr;
    SDL_GPUShader* fragmentShader_ = nullptr;
};

}