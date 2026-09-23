#pragma once

#include "Node.h"
#include "filesystem"
#include "types.h"

namespace opendash::engine
{

class Sprite : public Node {
public:
    static std::unique_ptr<Sprite> create(const std::filesystem::path& path);

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
    void draw(Graphics* gfx) override;
    bool init() override;
private:
    Texture* texture_ = nullptr;
    Color4F renderColor_ = {1.0f, 1.0f, 1.0f, 1.0f};
    Color3B color_ = {255, 255, 255};
    u8 opacity_ = 255;
};

}