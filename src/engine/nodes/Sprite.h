#pragma once

#include "Node.h"
#include <filesystem>
#include "../core/types.h"
#include "../core/SpriteFrame.h"

namespace opendash::engine
{

class Sprite : public Node {
public:
    static std::unique_ptr<Sprite> create(const std::filesystem::path& path);
    static std::unique_ptr<Sprite> createWithFrame(SpriteFrame* spriteFrame);
    static std::unique_ptr<Sprite> createWithFrame(const std::string& spriteFrameName);

    // setters
    void setColor(const Color3B& color);
    void setColor(u8 r, u8 g, u8 b);
    void setColor(const Color4F& color);
    void setOpacity(u8 opacity);

    // getters
    const Color3B& getColor() const;
    u8 getOpacity() const;
    const Color4F& getRenderColor() const;

protected:
    bool initWithPath(const std::filesystem::path& path);
    bool initWithSpriteFrame(SpriteFrame* frame);
    void draw(Graphics* gfx) override;
    bool init() override;
private:
    /*
        If we are using a full texture, texture_ must contain it and spriteFrame_ must be nullptr
        If we are using a sprite frame, spriteFrame_ must contain it and texture_ must be nullptr
    */
    Texture* texture_ = nullptr;
    SpriteFrame* spriteFrame_ = nullptr;

    Color4F renderColor_ = {1.0f, 1.0f, 1.0f, 1.0f};
    Color3B color_ = {255, 255, 255};
    u8 opacity_ = 255;
};

}