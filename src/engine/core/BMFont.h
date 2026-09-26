#pragma once

#include "types.h"
#include <map>
#include "../Texture.h"

namespace opendash::engine
{

class AssetManager;
class TextReader;

struct BMFGlyph {
    int codepoint = -1;
    Rect crop;
    Point offset;
    int advance, page;
    glm::mat3 textureTransform;
};

class BMFont {
private:
    inline BMFont() {};

public:
    static BMFont* load(const std::filesystem::path& path);

    void print();

    inline const BMFGlyph* getGlyph(int codepoint) const {
        auto it = glyphs.find(codepoint);
        if (it != glyphs.end())
            return &it->second;
        return nullptr;
    }

    inline int getKerning(int first, int second) const {
        auto it = kernings.find({first, second});
        if (it != kernings.end())
            return it->second;
        return 0;
    }

    inline int getLineHeight() const { return lineHeight; }

    inline Texture* getTexture() const { return texture; }

private:
    friend class AssetManager;

    bool parseInfoLine(TextReader& rd);
    bool parseCommonLine(TextReader& rd);
    bool parsePageLine(TextReader& rd);
    bool parseCharLine(TextReader& rd);
    bool parseKerningLine(TextReader& rd);
    bool skipLine(TextReader& rd);

private:
    Texture* texture;
    std::map<int, std::string> pageFilenames;
    std::map<int, BMFGlyph> glyphs;
    std::map<std::pair<int, int>, int> kernings;
    std::string faceName;
    int size, lineHeight;
};


};