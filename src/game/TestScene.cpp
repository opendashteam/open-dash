#include "TestScene.h"
#include "constants.h"

namespace opendash
{

void TestScene::update(float dt) {
    if (cubeSprite_) {
        cubeSprite_->rotateBy((180.0f / constants::player::kRotationDuration) * dt);
    }
}

bool TestScene::init() {
    if (!Scene::init()) { // Always super init() first
        return false; 
    }

    // setClearColor({0.0f, 1.0f, 0.0f, 1.0f});

    // Always call addChild first
    cubeSprite_ = addChild(Sprite::create("cube.png"));
    cubeSprite_->setPosition(1280.0/2.0, 720.0/2.0);
    
    return true;
}

}