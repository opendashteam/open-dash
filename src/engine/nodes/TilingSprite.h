#pragma once

#include "ColorNode.h"
#include <filesystem>

namespace opendash::engine
{

class TilingSprite : public ColorNode {
public:
    static std::unique_ptr<TilingSprite> create(const std::filesystem::path& path);

    // setters
    void setTileOffset(const Point& tileOffset);
    void setTileOffset(float x, float y);
    void setTileOffsetX(float tileOffsetX);
    void setTileOffsetY(float tileOffsetY);
    void setTileScale(const Point& tileScale);
    void setTileScale(float x, float y);
    void setTileScaleX(float tileScaleX);
    void setTileScaleY(float tileScaleY);
    void setMirroredRepeatX(bool mirroredX);
    void setMirroredRepeatY(bool mirroredY);

    // getters
    const Point& getTileOffset() const;
    float getTileOffsetX() const;
    float getTileOffsetY() const;
    const Point& getTileScale() const;
    float getTileScaleX() const;
    float getTileScaleY() const;
    bool getMirroredRepeatX() const;
    bool getMirroredRepeatY() const;

protected:
    bool initWithPath(const std::filesystem::path& path);
    void draw(Graphics* gfx) override;
    bool init() override;
private:
    Point tileOffset_ = {0.0f, 0.0f};
    Point tileScale_  = {1.0f, 1.0f};

    bool uvDirty_ = true;
    bool mirroredRepeatX_ = false;
    bool mirroredRepeatY_ = false;
};

}