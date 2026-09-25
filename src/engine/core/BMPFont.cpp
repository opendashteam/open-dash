#include "BMPFont.h"
#include "../AssetManager.h"

namespace opendash::engine
{

// not the level
inline bool isWhitespace(char ch) {
    return ch == ' ' || ch == '\t' || ch == '\r';
}

class TokenReader {
public:


private:
    inline char peek() const { return *ptr; }
    inline char consume() { return *(ptr++); }

    void consumeWhitespace() {
        while (isWhitespace(peek()))
            consume();
    }

private:
    const char* ptr;
};

BMPFont* BMPFont::load(const std::filesystem::path& path) {
    std::string data;
    if (!AssetManager::get()->readFileAsString(path, data)) {
        log::info("Could not load BMPFont {}", path.string());
        return nullptr;
    }

    return nullptr;
}

}

