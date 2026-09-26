#include "BMFont.h"
#include "../AssetManager.h"
#include "../utilities/log.h"

namespace opendash::engine
{

// not the level
inline bool isWhitespace(char ch) {
    return ch == ' ' || ch == '\t' || ch == '\r';
}

inline bool isNumber(char ch)  {
    return ch >= '0' && ch <= '9';
}

inline bool isLetter(char ch) {
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
}

class TextReader {
public:
    // str has to exist for as long as TextReader is used
    inline TextReader(const std::string& str, const std::string& sourceName)
        : ptr(str.c_str()), sourceName(sourceName) {}

    bool consumeNumber(int& out) {
        consumeWhitespace();
        bool negative = match('-');
        if (!negative && !isNumber(peek()))
            return false;

        out = 0;
        while (isNumber(peek()))
            out = out * 10 + (consume() - '0');
        if (negative) out = -out;
        return true;
    }

    bool consumeWord(std::string& out) {
        consumeWhitespace();
        if (!isLetter(peek()))
            return false;

        out = "";
        while (isLetter(peek()))
            out += consume();
        return true;
    }

    bool consumeString(std::string& out) {
        consumeWhitespace();
        if (!match('"')) return false;

        out = "";
        while (peek() != '"')
            out += consume();
        consume();
        match('"'); // stupid edge case
        return true;
    }

    bool consumeNumberOrError(int& out) {
        if (!consumeNumber(out)) {
            err("expected number");
            return false;
        }
        return true;
    }

    bool consumeFloatOrError(float& out) {
        int num;
        if (!consumeNumberOrError(num))
            return false;
        out = num;
        return true;
    }

    bool consumeStringOrError(std::string& out) {
        if (!consumeString(out)) {
            err("expected string");
            return false;
        }
        return true;
    }

    bool parsePropertyKey(std::string& out) {
        if (!consumeWord(out)) {
            err("expected property");
            return false;
        }

        consumeWhitespace();
        if (!match('=')) {
            err("expected '='");
            return false;
        }
        return true;
    }

    bool isEnd() const { return peek() == 0; }

    void skipPropertyValue() {
        std::string str;
        int num;
        if (consumeString(str)) return;
        if (consumeWord(str)) return;
        if (consumeNumber(num)) {
            while (match(','))
                consumeNumber(num);
        }
    }

    inline bool match(char ch) {
        if (peek() == ch) {
            consume();
            return true;
        }
        return false;
    }

    inline void err(const std::string& msg) {
        log::err("{}:{}:{}: {}", sourceName, line + 1, col + 1, msg);
    }

    inline bool matchLineEnding() {
        consumeWhitespace();
        return match('\n');
    }

    inline void consumeWhitespace() {
        while (isWhitespace(peek()))
            consume();
    }

private:
    inline char peek() const { return *ptr; }
    inline char consume() {
        col++;
        if (*ptr == '\n') {
            line++;
            col = 0;
        }
        return *(ptr++);
    }

private:
    const char* ptr;
    std::string sourceName;
    u32 line = 0, col = 0;
};

BMFont* BMFont::load(const std::filesystem::path& path) {
    std::string data;
    if (!AssetManager::get()->readFileAsString(path, data)) {
        log::info("Could not load BMPFont {}", path.string());
        return nullptr;
    }

    TextReader rd(data, path.string());

    auto ret = std::unique_ptr<BMFont>(new BMFont);

    while (!rd.isEnd()) {
        std::string type;
        if (!rd.consumeWord(type)) {
            if (!rd.isEnd()) {
                rd.err("line type expected");
                return nullptr;
            }
            break;
        }

        if (type == "info") {
            if (!ret->parseInfoLine(rd)) return nullptr;
        } else if (type == "page") {
            if (!ret->parsePageLine(rd)) return nullptr;
        } else if (type == "char") {
            if (!ret->parseCharLine(rd)) return nullptr;
        } else if (type == "common") {
            if (!ret->parseCommonLine(rd)) return nullptr;
        } else if (type == "kerning") {
            if (!ret->parseKerningLine(rd)) return nullptr;
        } else
            ret->skipLine(rd);
    }

    if (ret->pageFilenames.size() != 1) {
        log::err("{}: too many pages (or too little)", path.string());
        return nullptr;
    }

    ret->texture = AssetManager::get()->fetchTexture(ret->pageFilenames.begin()->second);
    if (!ret->texture) {
        log::err("{}: failed to load, cannot find texture at {}", path.string(), ret->pageFilenames.begin()->second);
        return nullptr;
    }

    for (auto& [id, glyph] : ret->glyphs) {
        glm::vec2 pos = glyph.crop.origin.toGLM() / ret->texture->getSize().toGLM();
        Size size = glyph.crop.size / ret->texture->getSize();
        pos.y += size.height;
        glyph.textureTransform[0] = {size.width, 0, 0};
        glyph.textureTransform[1] = {0, -size.height, 0};
        glyph.textureTransform[2] = {pos, 1};
    }

    return ret.release();
}

void BMFont::print() {
    log::info("faceName: {}", faceName);
    log::info("size: {}", size);
    log::info("lineHeight: {}", lineHeight);

    log::info("pages:");
    for (const auto& [id, name] : pageFilenames)
        log::info("  - {}: {}", id, name);

    log::info("kernings:");
    for (const auto& [pair, amount] : kernings) {
        log::info("  - {} => {}: {}", pair.first, pair.second, amount);
    }

    log::info("glyphs:");
    for (const auto& [first, glyph] : glyphs) {
        log::info("- codepoint: {}", glyph.codepoint);
        log::info("  - crop: {}", glyph.crop);
        log::info("  - offset: {}", glyph.offset);
        log::info("  - advance: {}", glyph.advance);
        log::info("  - page: {}", glyph.page);
    }
}

bool BMFont::parseInfoLine(TextReader& rd) {
    while (!rd.matchLineEnding()) {
        std::string prop;
        if (!rd.parsePropertyKey(prop)) return false;

        if (prop == "face") {
            if (!rd.consumeStringOrError(faceName)) return false;
        } else if (prop == "size") {
            if (!rd.consumeNumberOrError(size)) return false;
        } else
            rd.skipPropertyValue();
    }
    return true;
}

bool BMFont::parseCommonLine(TextReader& rd) {
    while (!rd.matchLineEnding()) {
        std::string prop;
        if (!rd.parsePropertyKey(prop)) return false;

        if (prop == "lineHeight") {
            if (!rd.consumeNumberOrError(lineHeight)) return false;
        } else
            rd.skipPropertyValue();
    }
    return true;
}

bool BMFont::parsePageLine(TextReader& rd) {
    int id = -1;
    std::string name = "";

    while (!rd.matchLineEnding()) {
        std::string prop;
        if (!rd.parsePropertyKey(prop)) return false;

        if (prop == "id") {
            if (!rd.consumeNumberOrError(id)) return false;
        } else if (prop == "file") {
            if (!rd.consumeStringOrError(name)) return false;
        } else
            rd.skipPropertyValue();
    }

    if (id != -1 && name != "")
        pageFilenames[id] = name;
    return true;
}

bool BMFont::parseCharLine(TextReader& rd) {
    BMFGlyph glyph;

    while (!rd.matchLineEnding()) {
        std::string prop;
        if (!rd.parsePropertyKey(prop)) return false;

        if (prop == "id") {
            if (!rd.consumeNumberOrError(glyph.codepoint)) return false;
        } else if (prop == "x") {
            if (!rd.consumeFloatOrError(glyph.crop.origin.x)) return false;
        } else if (prop == "y") {
            if (!rd.consumeFloatOrError(glyph.crop.origin.y)) return false;
        } else if (prop == "width") {
            if (!rd.consumeFloatOrError(glyph.crop.size.width)) return false;
        } else if (prop == "height") {
            if (!rd.consumeFloatOrError(glyph.crop.size.height)) return false;
        } else if (prop == "xoffset") {
            if (!rd.consumeFloatOrError(glyph.offset.x)) return false;
        } else if (prop == "yoffset") {
            if (!rd.consumeFloatOrError(glyph.offset.y)) return false;
        } else if (prop == "xadvance") {
            if (!rd.consumeNumberOrError(glyph.advance)) return false;
        } else if (prop == "page") {
            if (!rd.consumeNumberOrError(glyph.page)) return false;
        } else
            rd.skipPropertyValue();
    }

    if (glyph.codepoint != -1)
        glyphs[glyph.codepoint] = glyph;
    return true;
}

bool BMFont::parseKerningLine(TextReader& rd) {
    int first = -1, second = -1, amount = 0;

    while (!rd.matchLineEnding()) {
        std::string prop;
        if (!rd.parsePropertyKey(prop)) return false;

        if (prop == "first") {
            if (!rd.consumeNumberOrError(first)) return false;
        } else if (prop == "second") {
            if (!rd.consumeNumberOrError(second)) return false;
        } else if (prop == "amount") {
            if (!rd.consumeNumberOrError(amount)) return false;
        } else
            rd.skipPropertyValue();
    }

    if (first != -1 && second != -1)
        kernings[{first, second}] = amount;

    return true;
}

bool BMFont::skipLine(TextReader& rd) {
    while (!rd.matchLineEnding()) {
        std::string prop;
        if (!rd.parsePropertyKey(prop)) return false;
        rd.skipPropertyValue();
    }
    return true;
}

}

