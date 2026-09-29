#pragma once

#include "ColorNode.h"

namespace opendash::engine {

/*
    Equivalent to cocos2d's CCScale9Sprite
*/
class SpritePanel : public ColorNode {
public:
    void draw(Graphics* gfx) override;

    void setContentSize(const Size& size) override;

    void setCapInsets(Rect capInsets);

public:
    static std::unique_ptr<SpritePanel> create(const std::filesystem::path& texturePath, const Rect& capInsets);
    static std::unique_ptr<SpritePanel> create(const std::filesystem::path& texturePath);

protected:
    bool init(Texture* texture, const Rect& capInsets);

private:
    void generateBatch();

private:
    bool isBatchDirty_ = true;

    std::unique_ptr<SpriteBatch> batch_ = nullptr;

    Texture* texture_;
    Rect capInsets_;

    Point textureOffsets[4];
};

};