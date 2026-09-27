#include "Label.h"
#include "../AssetManager.h"
#include "../utilities/log.h"
#include "../core/Director.h"
#include <cuchar>

namespace opendash::engine
{

static constexpr TextureWrapParameters sharedLabelWrapParameters {
    WrapMode::Clamp,
    WrapMode::Clamp
};

void Label::setText(std::string_view text) {
    text_ = text;
    updateContentSize();
    spriteBatchDirty_ = true;
}

void Label::setFont(BMFont* font) {
    assert(font);
    font_ = font;
    updateContentSize();
    spriteBatchDirty_ = true;
}

void Label::setFont(std::string_view fontName) {
    auto font = AssetManager::get()->fetchFont(fontName);
    if (!font)
        log::err("Could not set font of Label, could not fetch font {}", fontName);
    else
        setFont(font);
}

void Label::setAlign(TextAlign align) {
    align_ = align;
    // No need to update content size
    spriteBatchDirty_ = true;
}

void Label::draw(Graphics* gfx) {
    assert(font_);

    if (spriteBatchDirty_)
        generateBatch();

    batch_->draw(font_->getTexture(), getWorldTransform(), renderColor_);
}

bool Label::init(std::string_view text, BMFont* font) {
    assert(font);
    text_ = text;
    font_ = font;
    updateContentSize();
    return true;
}

void Label::updateContentSize() {
    std::mbstate_t state {};
    const char* ptr = text_.c_str();
    const char* end = text_.c_str() + text_.size() + 1;

    float maxWidth = 0;
    float width = 0, height = font_->getLineHeight();
    int codepoint = 0, lastCodepoint = 0;

    lineWidths_.clear();
    numCharactersToDraw_ = 0;

    while (std::size_t size = std::mbrtoc32((char32_t*)&codepoint, ptr, end - ptr, &state)) {
        if (!lastCodepoint)
            width += font_->getKerning(lastCodepoint, codepoint);

        auto glyph = font_->getGlyph(codepoint);
        if (glyph)
            width += glyph->advance;

        if (glyph && glyph->crop.size != Size(0, 0))
            numCharactersToDraw_++;
        
        if (codepoint == '\n') {
            lineWidths_.push_back(width);
            maxWidth = std::max(maxWidth, width);
            width = 0;
            height += font_->getLineHeight();
        }

        lastCodepoint = codepoint;
        ptr += size;
    }
    
    lineWidths_.push_back(width);
    maxWidth = std::max(maxWidth, width);

    setContentSize(Size(maxWidth, height).toPoints());
}

float Label::getStartXAtLine(u32 lineIndex) {
    assert(lineIndex < lineWidths_.size());
    float lineWidth    = lineWidths_[lineIndex];
    float contentWidth = getContentWidth() * Director::get()->getContentScaleFactor();

    switch (align_) {
    default:
    case TextAlign::LEFT:   return 0;
    case TextAlign::CENTER: return contentWidth / 2 - lineWidth / 2;
    case TextAlign::RIGHT:  return contentWidth - lineWidth;
    }
}

void Label::generateBatch() {
    if (!batch_)
        batch_ = SpriteBatch::create();

    batch_->resize(numCharactersToDraw_);

    std::mbstate_t state {};
    const char* ptr = text_.c_str();
    const char* end = text_.c_str() + text_.size() + 1;

    int codepoint = 0, lastCodepoint = 0;

    Point pos;
    
    pos.x = getStartXAtLine(0);
    pos.y = getContentHeight() * Director::get()->getContentScaleFactor() - (float)font_->getLineHeight();

    u32 batchSpriteIndex = 0;
    u32 lineIndex = 0;

                                              // v is this fine?
    while (std::size_t size = std::mbrtoc32((char32_t*)&codepoint, ptr, end - ptr, &state)) {
        if (lastCodepoint != 0)
            pos.x += font_->getKerning(lastCodepoint, codepoint);

        auto glyph = font_->getGlyph(codepoint);

        if (glyph && glyph->crop.size != Size(0, 0)) {
            float yOffset = font_->getLineHeight() - glyph->offset.y;
            Point offset { glyph->offset.x, yOffset - glyph->crop.size.height };

            glm::vec2 position = (pos + offset).toPoints().toGLM();
            auto glyphTransform = glm::translate(glm::mat4(1.0f), { position, 0 });
            glyphTransform = glm::scale(glyphTransform, { glyph->crop.size.toPoints().toGLM(), 0 });

            batch_->setSprite(batchSpriteIndex, glyphTransform, glyph->textureTransform);
            batchSpriteIndex++;
        }

        if (glyph)
            pos.x += glyph->advance;
        
        if (codepoint == '\n') {
            lineIndex++;
            pos.x = getStartXAtLine(lineIndex);
            pos.y -= font_->getLineHeight();
        }

        lastCodepoint = codepoint;
        ptr += size;
    }

    spriteBatchDirty_ = false;
}

std::unique_ptr<Label> Label::create(std::string_view text, std::string_view fontName) {
    auto font = AssetManager::get()->fetchFont(fontName);
    if (!font) {
        log::err("Could not create Label, could not fetch font {}", fontName);
        return nullptr;
    }
    return create(text, font);
}

std::unique_ptr<Label> Label::create(std::string_view text, BMFont* font) {
    auto node = std::unique_ptr<Label>(new Label);
    if (node && node->init(text, font))
        return node;
    return nullptr;
}

static BMFont* bigFont = nullptr;

std::unique_ptr<Label> Label::createBigFont(std::string_view text) {
    if (!bigFont) {
        bigFont = AssetManager::get()->fetchFont("bigFont");
        if (!bigFont) {
            log::err("Could not create bigFont Label, could not fetch bigFont");
            return nullptr;
        }
    }
    return create(text, bigFont);
}

};