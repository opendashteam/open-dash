#include "ExampleScene.h"
#include "constants.h"

namespace opendash
{

void ExampleScene::update(float dt) {
    if (exampleSprite_) {
        exampleSprite_->rotateBy((180.0f / constants::player::kRotationDuration) * dt);

        Size windowSize = Director::get()->getWindowSize();
        const float halfSpriteWidth = (exampleSprite_->getContentWidth() * std::sqrt(2)) * 0.5f;
        const float leftLimit = halfSpriteWidth;
        const float rightLimit = windowSize.width - halfSpriteWidth;
        float nextX = exampleSprite_->getPositionX() +
            (spriteMovingRight_ ? 800.0f : -800.0f) * dt;

        if (nextX >= rightLimit) {
            nextX = rightLimit;
            spriteMovingRight_ = false;
        }
        else if (nextX <= leftLimit) {
            nextX = leftLimit;
            spriteMovingRight_ = true;
        }

        exampleSprite_->setPositionX(nextX);
    }
}

bool ExampleScene::init() {
    if (!Scene::init()) { // Always super init() first
        return false; 
    }

    // setClearColor({0.0f, 1.0f, 0.0f, 1.0f});

    // Always call addChild first 
    exampleSprite_ = addChild(Sprite::createWithFrame("GJ_gkBtn_001.png"));
    exampleSprite_->setPosition(Director::get()->getWindowSize() / 2);

    auto* extra1 = exampleSprite_->addChild(Sprite::create("cube.png"));
    auto* extra2 = exampleSprite_->addChild(Sprite::create("cube.png"));
    auto* extra3 = exampleSprite_->addChild(Sprite::create("cube.png"));
    auto* extra4 = exampleSprite_->addChild(Sprite::create("cube.png"));

    float w = exampleSprite_->getContentWidth();
    float h = exampleSprite_->getContentHeight();

    extra2->setPosition(w, 0);
    extra3->setPosition(0, h);
    extra4->setPosition(w, h);

    extra2->setColor(Color3B::RED);
    extra3->setColor(Color3B::RED);
    extra4->setColor(Color3B::GREEN);
    extra1->setColor(Color3B::GREEN);
    
    return true;
}

}