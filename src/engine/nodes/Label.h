#pragma once

#include "ColorNode.h"

#include "../core/BMFont.h"

namespace opendash::engine
{

enum class TextAlign {
    LEFT,
    CENTER,
    RIGHT
};

class Label : public ColorNode
{
public:
    void setText(std::string_view text);
    void setFont(BMFont* font);
    void setFont(std::string_view font);
    void setAlign(TextAlign align);

    inline const std::string& getText() const { return text_; }
    inline BMFont* getFont() const { return font_; }
    inline TextAlign getAlign() const { return align_; }

    virtual void draw(Graphics* gfx) override;

private:
    bool init(std::string_view text, BMFont* font);

    void updateContentSize();

    float getStartXAtLine(u32 lineIndex);

    void generateBatch();

public:
    static std::unique_ptr<Label> create(std::string_view text, std::string_view font);
    static std::unique_ptr<Label> create(std::string_view text, BMFont* font);
    static std::unique_ptr<Label> createBigFont(std::string_view text);

private:
    bool spriteBatchDirty_ = true;
    std::unique_ptr<SpriteBatch> batch_ = nullptr;
    std::vector<float> lineWidths_;
    u32 numCharactersToDraw_;

    TextAlign align_ = TextAlign::CENTER;
    std::string text_;
    BMFont* font_;
};

};