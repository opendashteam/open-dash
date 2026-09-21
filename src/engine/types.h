#pragma once

#include <filesystem>
#include <SDL3/SDL.h>

namespace opendash::engine
{

struct Vertex {
    float x, y;
    float u, v;
};    

struct PathHash {
    std::size_t operator()(const std::filesystem::path& p) const {
        return std::filesystem::hash_value(p);
    }
};

struct Color3B {
    unsigned char r;
    unsigned char g;
    unsigned char b;
};

struct Color4B {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

struct Size {
    float width;
    float height;
};

struct Point {
    float x;
    float y;
};

struct Rect {
    Point origin;
    Size size;

    // TODO more stuff
};

struct Texture {
    SDL_GPUTexture* gpuTexture = nullptr;
    SDL_GPUSampler* sampler = nullptr;
    int width;
    int height;
};

}

