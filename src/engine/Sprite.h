#pragma once

#include "Node.h"
#include "filesystem"

namespace opendash::engine
{

class Sprite : public Node {
public:
    static std::unique_ptr<Sprite> create(const std::filesystem::path& path);
protected:
    bool initWithPath(const std::filesystem::path& path);
    void draw(Graphics* gfx) override;
    bool init() override;
private:
    Texture* texture_;
};

}