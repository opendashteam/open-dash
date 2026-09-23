#pragma once

#include "core/types.h"

namespace opendash::engine
{

class AssetManager;

using InternalTexture = void*;

class Texture
{
private:
    inline Texture(u32 width, u32 height, InternalTexture internal)
        : width_(width), height_(height), internal_(internal) {}
    inline ~Texture() {}

    friend class AssetManager;

public:
    inline u32 getWidth() const { return width_; }
    inline u32 getHeight() const { return height_; }

    inline InternalTexture getInternalObject() const { return internal_; }

private:
    u32 width_, height_;
    InternalTexture internal_;
};

};