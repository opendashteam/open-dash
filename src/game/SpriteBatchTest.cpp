#include "SpriteBatchTest.h"

namespace opendash
{

bool SpriteBatchTest::init() {
    batch = SpriteBatch::create();
    batch->resize(16 * 16);

    auto spriteFrame = AssetManager::get()->getSpriteFrameByName("diffIcon_02_btn_001.png");
    if (!spriteFrame)
        return false;

    texture = spriteFrame->getTexture();

    for (int x = 0; x < 16; x++)
        for (int y = 0; y < 16; y++) {
            batch->setSpriteFrame(y * 16 + x, spriteFrame, {1, 1, 1, 1}, {x * 32, y * 32});
        }

    setContentSize(17 * 32, 17 * 32);
    setAnchorPoint(0.5f, 0.5f);

    return true;
}

void SpriteBatchTest::draw(Graphics* gfx) {
    batch->draw(texture, getWorldTransform());
}

};