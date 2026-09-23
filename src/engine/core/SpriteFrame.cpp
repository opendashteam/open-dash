#include "SpriteFrame.h"

namespace opendash::engine
{

static inline bool isNumberCharacter(char ch) {
    return (ch >= '0' && ch <= '9') || ch == '.';
}

static inline bool parseNumberAndBracePattern(const std::string& string, float* out, int outExpected) {
    std::string numberString = "";

    int outIndex = 0;

    for (int i = 0; i < string.length(); i++) {
        char ch = string[i];

        if (isNumberCharacter(ch))
            numberString += ch;
        else if (!numberString.empty()) {
            if (outIndex == outExpected)
                return false;

            out[outIndex] = std::stof(numberString);
            outIndex++;
            numberString = "";
        }
    }

    if (!numberString.empty()) {
        if (outIndex == outExpected)
            return false;
        out[outIndex] = std::stof(numberString);
    }
    return outIndex == outExpected;
}

static inline bool parsePoint(const std::string& string, Point& out) {
    float values[2];
    if (!parseNumberAndBracePattern(string, values, 2))
        return false;
    out = { values[0], values[1] };
    return true;
}

static inline bool parseSize(const std::string& string, Size& out) {
    float values[2];
    if (!parseNumberAndBracePattern(string, values, 2))
        return false;
    out = { values[0], values[1] };
    return true;
}

static inline bool parseRect(const std::string& string, Rect& out) {
    float values[4];
    if (!parseNumberAndBracePattern(string, values, 4))
        return false;
    out = { values[0], values[1], values[2], values[3] };
    return true;
}

SpriteFrame* SpriteFrame::loadFromPListNode(Texture* texture, const std::string& name, PList* node)
{
    if (!node->isDict())
        return nullptr;

    PList* spriteOffsetNode = node->getNode("spriteOffset");
    PList* spriteSizeNode = node->getNode("spriteSize");
    PList* spriteSourceSizeNode = node->getNode("spriteSourceSize");
    PList* textureRectNode = node->getNode("textureRect");
    PList* rotatedNode = node->getNode("textureRotated");

    if (!spriteOffsetNode || !spriteOffsetNode->isString()) return nullptr;
    if (!spriteSizeNode || !spriteSizeNode->isString()) return nullptr;
    if (!spriteSourceSizeNode || !spriteSourceSizeNode->isString()) return nullptr;
    if (!textureRectNode || !textureRectNode->isString()) return nullptr;
    if (!rotatedNode || !rotatedNode->isBoolean()) return nullptr;

    Point spriteOffset;
    Size spriteSize, spriteSourceSize;
    Rect textureRect;
    bool rotated;

    if (!parsePoint(spriteOffsetNode->getString(), spriteOffset)) return nullptr;
    if (!parseSize(spriteSizeNode->getString(), spriteSize)) return nullptr;
    if (!parseSize(spriteSourceSizeNode->getString(), spriteSourceSize)) return nullptr;
    if (!parseRect(textureRectNode->getString(), textureRect)) return nullptr;
    rotated = rotatedNode->getBoolean();

    auto frame = new SpriteFrame;
    frame->name_ = name;
    frame->texture_ = texture;
    frame->textureRect_ = textureRect;
    frame->rotated_ = rotated;
    frame->spriteOffset_ = spriteOffset;
    frame->spriteSize_ = spriteSize;
    frame->spriteSourceSize_ = spriteSourceSize;

    return frame;
}

}