#pragma once

#include "../engine.h"

namespace opendash
{

class Background : public engine::ColorNode {
public:
    static std::unique_ptr<Background> create(int backgroundID);
    void setScrollX(float scrollX);
    void setScrollY(float scrollY);
protected:
    bool init(int backgroundID);
    void onRenderColorChanged() override;
private:
    engine::TilingSprite* sprite_ = nullptr;
};

}