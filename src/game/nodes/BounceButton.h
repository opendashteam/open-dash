#pragma once

#include "../engine.h"

using namespace opendash::engine;

namespace opendash {

/*
    The common GD button that when you click on it
    plays an animation where it turns big with a
    bounce.
*/
class BounceButton : public Button {
public:
    void onActivate() override;

    void onDeactivate() override;

public:
    CREATE_FUNC(BounceButton);

    static std::unique_ptr<BounceButton> createWithSprite(const std::filesystem::path& path);
    static std::unique_ptr<BounceButton> createWithSpriteFrame(const std::string& frameName);
};

};