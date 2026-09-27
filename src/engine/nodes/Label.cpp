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

void Label::setText(const std::string& text) {
    text_ = text;
    updateContentSize();
    labelDirty_ = true;
}

void Label::setFont(BMFont* font) {
    assert(font);
    font_ = font;
    updateContentSize();
    labelDirty_ = true;
}

void Label::setFont(const std::string& fontName) {
    auto font = AssetManager::get()->fetchFont(fontName);
    if (!font)
        log::err("Could not set font of Label, could not fetch font {}", fontName);
    else
        setFont(font);
}

void Label::draw(Graphics* gfx) {
    assert(font_);

    std::mbstate_t state {};
    const char* ptr = text_.c_str();
    const char* end = text_.c_str() + text_.size() + 1;

    int codepoint = 0, lastCodepoint = 0;

    Point pos = { 0, getContentHeight() - (float)font_->getLineHeight() * Director::get()->getInvertedContentScaleFactor() };

    const auto& worldTransform = getWorldTransform();

                                              // v is this fine?
    while (std::size_t size = std::mbrtoc32((char32_t*)&codepoint, ptr, end - ptr, &state)) {
        if (!lastCodepoint)
            pos.x += font_->getKerning(lastCodepoint, codepoint);

        auto glyph = font_->getGlyph(codepoint);

        if (glyph) {
            float yOffset = font_->getLineHeight() - glyph->offset.y;
            Point offset { glyph->offset.x, yOffset - glyph->crop.size.height };

            glm::vec2 position = (pos + offset).toPoints().toGLM();
            auto glyphTransform = glm::translate(glm::mat4(1.0f), { position, 0 });
            glyphTransform = glm::scale(glyphTransform, { glyph->crop.size.toPoints().toGLM(), 0 });

            gfx->drawSprite(
                font_->getTexture(),
                worldTransform * glyphTransform,
                glyph->textureTransform,
                {1, 1, 1, 1},
                sharedLabelWrapParameters
            );
            pos.x += glyph->advance;
        }
        
        if (codepoint == '\n') {
            pos.x = 0;
            pos.y -= font_->getLineHeight();
        }

        lastCodepoint = codepoint;
        ptr += size;
    }
}

bool Label::init(const std::string& text, BMFont* font) {
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

    while (std::size_t size = std::mbrtoc32((char32_t*)&codepoint, ptr, end - ptr, &state)) {
        if (!lastCodepoint)
            width += font_->getKerning(lastCodepoint, codepoint);

        auto glyph = font_->getGlyph(codepoint);
        if (glyph)
            width += glyph->advance;
        
        if (codepoint == '\n') {
            maxWidth = std::max(maxWidth, width);
            width = 0;
            height += font_->getLineHeight();
        }

        lastCodepoint = codepoint;
        ptr += size;
    }

    setContentSize(Size(std::max(width, maxWidth), height).toPoints());
}

std::unique_ptr<Label> Label::create(const std::string& text, const std::string& fontName) {
    auto font = AssetManager::get()->fetchFont(fontName);
    if (!font) {
        log::err("Could not create Label, could not fetch font {}", fontName);
        return nullptr;
    }
    return create(text, font);
}

std::unique_ptr<Label> Label::create(const std::string& text, BMFont* font) {
    auto node = std::unique_ptr<Label>(new Label);
    if (node && node->init(text, font))
        return node;
    return nullptr;
}

static BMFont* bigFont = nullptr;

std::unique_ptr<Label> Label::createBigFont(const std::string& text) {
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