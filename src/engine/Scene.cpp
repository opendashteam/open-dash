#include "Scene.h"
#include "types.h"
#include "Application.h"
#include <SDL3/SDL.h>

namespace opendash::engine
{

bool Scene::init()
{
    // setAnchorPoint(0.0f, 0.0f);
    return setupShaders() && setupPipeline();
}

void Scene::render(SDL_GPUGraphicsPipeline* pipeline, SDL_GPUCommandBuffer* commandBuffer, SDL_GPUTexture* swapchainTexture) {

    SDL_GPUColorTargetInfo colorTargetInfo{};
    colorTargetInfo.texture = swapchainTexture;
    colorTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
    colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;
    colorTargetInfo.clear_color = {0.0f, 0.0f, 0.0f, 1.0f};

    SDL_GPURenderPass* pass = SDL_BeginGPURenderPass(commandBuffer, &colorTargetInfo, 1, nullptr);

    for (auto& child : getChildren()) {
        child->visit(pass, pipeline, commandBuffer);
    }

    SDL_EndGPURenderPass(pass);
    SDL_SubmitGPUCommandBuffer(commandBuffer);
}

bool Scene::setupShaders() {
    auto app = Application::get();

    vertexShader_ = loadShader(app->getDevice(), "shaders/sprite.vert.spv", SDL_GPU_SHADERSTAGE_VERTEX, 0, 1);
    fragmentShader_ = loadShader(app->getDevice(), "shaders/sprite.frag.spv", SDL_GPU_SHADERSTAGE_FRAGMENT, 1, 0);

    if (!vertexShader_ || !fragmentShader_) {
        SDL_Log("Shader load failed, aborting");
        return false;
    }

    return true;
}

bool Scene::setupPipeline() {
    auto app = Application::get();
    auto device = app->getDevice();

    SDL_GPUVertexBufferDescription vertexBufferDesc{};
    vertexBufferDesc.slot = 0;
    vertexBufferDesc.input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;
    vertexBufferDesc.pitch = sizeof(Vertex);

    SDL_GPUVertexAttribute attributes[2]{};
    attributes[0].buffer_slot = 0;
    attributes[0].location = 0;
    attributes[0].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2;
    attributes[0].offset = 0;
    attributes[1].buffer_slot = 0;
    attributes[1].location = 1;
    attributes[1].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2;
    attributes[1].offset = sizeof(float) * 2;

    SDL_GPUColorTargetDescription colorTarget{};
    colorTarget.format = SDL_GetGPUSwapchainTextureFormat(device, app->getWindow());
    colorTarget.blend_state.enable_blend = true;
    colorTarget.blend_state.color_blend_op = SDL_GPU_BLENDOP_ADD;
    colorTarget.blend_state.alpha_blend_op = SDL_GPU_BLENDOP_ADD;
    colorTarget.blend_state.src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
    colorTarget.blend_state.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    colorTarget.blend_state.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
    colorTarget.blend_state.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;

    SDL_GPUGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.vertex_shader = vertexShader_;
    pipelineInfo.fragment_shader = fragmentShader_;
    pipelineInfo.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
    pipelineInfo.vertex_input_state.num_vertex_buffers = 1;
    pipelineInfo.vertex_input_state.vertex_buffer_descriptions = &vertexBufferDesc;
    pipelineInfo.vertex_input_state.num_vertex_attributes = 2;
    pipelineInfo.vertex_input_state.vertex_attributes = attributes;
    pipelineInfo.target_info.num_color_targets = 1;
    pipelineInfo.target_info.color_target_descriptions = &colorTarget;

    SDL_GPUGraphicsPipeline* pipeline = SDL_CreateGPUGraphicsPipeline(device, &pipelineInfo);
    SDL_ReleaseGPUShader(device, vertexShader_);
    SDL_ReleaseGPUShader(device, fragmentShader_);

    if (!pipeline) {
        SDL_Log("Pipeline creation failed: %s", SDL_GetError());
        return false;
    }

    app->setPipeline(pipeline);
    return true;
}

SDL_GPUShader* Scene::loadShader(SDL_GPUDevice* dev, const char* path, SDL_GPUShaderStage stage, Uint32 numSamplers, Uint32 numUniformBuffers)
{
    size_t codeSize;
    void* code = SDL_LoadFile(path, &codeSize);
    if (!code) {
        SDL_Log("Failed to load shader %s: %s", path, SDL_GetError());
        return nullptr;
    }

    SDL_GPUShaderCreateInfo info{};
    info.code = (Uint8*)code;
    info.code_size = codeSize;
    info.entrypoint = "main";
    info.format = SDL_GPU_SHADERFORMAT_SPIRV;
    info.stage = stage;
    info.num_samplers = numSamplers;
    info.num_uniform_buffers = numUniformBuffers; // this was missing entirely

    SDL_GPUShader* shader = SDL_CreateGPUShader(dev, &info);
    SDL_free(code);
    return shader;
}

}