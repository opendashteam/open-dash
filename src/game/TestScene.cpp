#include "TestScene.h"
#include "Sprite.h"

namespace opendash
{

bool TestScene::init() {
    if (!Scene::init()) { // Always super init() first
        return false; 
    }

    auto cubeSprite = engine::Sprite::create("cube.png");

    if (!cubeSprite) {
        SDL_Log("Sprite fail!!");
        return false;
    }

    cubeSprite->setRotation(10);
    cubeSprite->setAnchorPoint(0.0f, 0.0f);

    // Retrieve the value back since cubeSprite becomes nullptr after std::move is called
    // Might need an easier-to-type version of this soon
    engine::Sprite* cube = static_cast<engine::Sprite*>(addChild(std::move(cubeSprite)));

    SDL_Log("rotation: %f", cube->getRotation());
    
    return true;
}

}