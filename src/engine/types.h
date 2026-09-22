#pragma once

#include <filesystem>
#include <cstdint>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace opendash::engine
{

using u8  = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;
using i8  = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

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
    u8 r, g, b;
};

struct Color4B {
    u8 r, g, b, a;
};

struct Color4F {
    float r, g, b, a;
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

/*
struct Texture {
    SDL_GPUTexture* gpuTexture = nullptr;
    SDL_GPUSampler* sampler = nullptr;
    int width;
    int height;
};
*/

}

