#include "TestScene.h"
#include "constants.h"

namespace opendash
{

void TestScene::update(float dt) {
    if (cubeSprite_) {
        cubeSprite_->setRotation(cubeSprite_->getRotation() + (180.0f / constants::player::kRotationDuration) * dt);
    }
}

bool TestScene::init() {
    if (!Scene::init()) { // Always super init() first
        return false; 
    }

    // Always call addChild first
    cubeSprite_ = addChild(Sprite::create("cube.png"));
    cubeSprite_->setPosition(1280.0/2.0, 720.0/2.0);
    
    return true;
}

}