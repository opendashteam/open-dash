#pragma once

#include "types.h"

namespace opendash::engine
{

class AssetManager;

class BMPFont {
private:
    ~BMPFont();

private:
    static BMPFont* load(const std::filesystem::path& path);

    friend class AssetManager;

private:

};


};