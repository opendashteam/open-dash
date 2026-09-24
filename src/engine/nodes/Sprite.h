#pragma once

#include "ColorNode.h"
#include <filesystem>
#include "../core/types.h"
#include "../core/SpriteFrame.h"

namespace opendash::engine
{

class Sprite : public ColorNode {
public:
    static std::unique_ptr<Sprite> create(const std::filesystem::path& path);
    static std::unique_ptr<Sprite> createWithFrame(SpriteFrame* spriteFrame);
    static std::unique_ptr<Sprite> createWithFrame(const std::string& spriteFrameName);

protected:
    bool initWithPath(const std::filesystem::path& path);
    bool initWithSpriteFrame(SpriteFrame* frame);
    void draw(Graphics* gfx) override;
    bool init() override;
private:
    void setupSizes(const Size& contentPixels, const Size& quadPixels);

    /*
        If we are using a full texture, texture_ must contain it and spriteFrame_ must be nullptr
        If we are using a sprite frame, spriteFrame_ must contain it and texture_ must be nullptr
    */
    Texture* texture_ = nullptr;
    SpriteFrame* spriteFrame_ = nullptr;
    glm::mat4 quadScale_{1.0f}; // unit quad -> size in points
};

}