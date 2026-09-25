#include "ExampleScene.h"
#include "constants.h"

namespace opendash
{

void ExampleScene::update(float dt) {

    if (exampleSprite_) {
        // exampleSprite_->rotateBy((180.0f / constants::player::kRotationDuration) * dt);
    }
    if (batchTest_) {
        batchTest_->rotateBy((180.0f / constants::player::kRotationDuration) * dt * 0.25);
    }
}

bool ExampleScene::init() {
    if (!Scene::init()) { // Always super init() first
        return false; 
    }

    // setClearColor({0.0f, 1.0f, 0.0f, 1.0f});

    // Always call addChild first when creating a node
    // to not have to deal with a stale pointer after the 
    // unique_ptr's move operation
    /*
    exampleSprite_ = addChild(Sprite::create("cube.png"));
    exampleSprite_->setPosition(Director::get()->getVisibleSize() / 2);
    */

    /*
    exampleSprite_ = addChild(Sprite::createWithFrame("GJ_levelComplete_001.png"));
    exampleSprite_->setPosition(Director::get()->getVisibleSize() / 2);

    auto ncs = exampleSprite_->addChild(Sprite::createWithFrame("ncs_med_001.png"));

    batchTest_ = addChild(SpriteBatchTest::create());
    batchTest_->setPosition(Director::get()->getVisibleSize() / 2);
    */

    tilingSprite_ = addChild(TilingSprite::create("cube.png"));
    tilingSprite_->setContentSize({232, 65});
    tilingSprite_->setAnchorPoint({.5f, .5f});
    tilingSprite_->setTileOffset(15, 15);
    tilingSprite_->setTileScale(2.0, 1.5);
    tilingSprite_->setPosition(Director::get()->getVisibleSize() / 2);

    return true;
}

}