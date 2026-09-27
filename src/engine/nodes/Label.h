#pragma once

#include "ColorNode.h"

#include "../core/BMFont.h"

namespace opendash::engine
{

class Label : public ColorNode
{
public:
    void setText(const std::string& text);
    void setFont(BMFont* font);
    void setFont(const std::string& font);

    virtual void draw(Graphics* gfx) override;

private:
    bool init(const std::string& text, BMFont* font);

    void updateContentSize();

public:
    static std::unique_ptr<Label> create(const std::string& text, const std::string& font);
    static std::unique_ptr<Label> create(const std::string& text, BMFont* font);
    static std::unique_ptr<Label> createBigFont(const std::string& text);

private:
    bool labelDirty_ = true;
    std::string text_;
    BMFont* font_;
};

};