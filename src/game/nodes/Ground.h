#pragma once
#include "../engine.h"

namespace opendash
{

enum class GroundLineType {
    Normal = 1,
    Solid = 2,
    Thick = 3
};

struct GroundOptions {
    int groundID;
    GroundLineType lineType = GroundLineType::Normal;
};

class Ground : public engine::ColorNode {
public:
    static std::unique_ptr<Ground> create(const GroundOptions& opt);
    void setSecondaryColor(const engine::Color3B& secondaryColor);
    const engine::Color3B& getSecondaryColor() const;

    void setScrollX(float scrollX);
protected:
    bool init(const GroundOptions& opt);
    void onRenderColorChanged() override;
private:
    void createShadows();
    void createTiles();
    void createLine();
private:
    GroundOptions options_{};
    engine::TilingSprite* primarySprite_ = nullptr;
    engine::TilingSprite* secondarySprite_ = nullptr;
    engine::Sprite* lineSprite_ = nullptr;
    float textureWidth_ = 0.0f;
    float groundWidth_ = 0.0f;
    bool blendLine_ = false;
    float groundOffset_ = 0.0f;
    bool showGround_ = false;
    engine::Color3B secondaryColor_;
};

}