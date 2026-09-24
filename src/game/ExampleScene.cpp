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

    // Always call addChild first when creating a node
    // to not have to deal with a stale pointer after the 
    // unique_ptr's move operation
    exampleSprite_ = addChild(Sprite::create("cube.png"));
    exampleSprite_->setPosition(Director::get()->getVisibleSize() / 2);
    
    return true;
}

}