#include "ExampleScene.h"
#include "constants.h"

namespace opendash
{

void ExampleScene::update(float dt) {
    if (exampleSprite_) {
        exampleSprite_->rotateBy((180.0f / constants::player::kRotationDuration) * dt);
    }
}

bool ExampleScene::init() {
    if (!Scene::init()) { // Always super init() first
        return false; 
    }

    // setClearColor({0.0f, 1.0f, 0.0f, 1.0f});

    // Always call addChild first 
    exampleSprite_ = addChild(Sprite::createWithFrame("GJ_levelComplete_001.png"));
    exampleSprite_->setPosition(Director::get()->getWindowSize() / 2);
    exampleSprite_->setScale(0.5f);
    
    return true;
}

}