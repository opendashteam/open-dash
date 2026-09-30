#include "ExampleScene.h"
#include "constants.h"
// #include "BounceButton.h"

namespace opendash
{

void ExampleScene::update(float dt) {

    if (exampleSprite_) {
        // exampleSprite_->rotateBy((180.0f / constants::player::kRotationDuration) * dt);
    }
    if (batchTest_) {
        batchTest_->rotateBy((180.0f / constants::player::kRotationDuration) * dt * 0.128);
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

    // auto fireInTheHole = addChild(Sprite::createWithFrame("diffIcon_04_btn_001.png"));
    // fireInTheHole->setPosition(Director::get()->getVisibleSize() / 2);
    // fireInTheHole->enableBlending();

    // TIMEWARP
    Director::get()->setTimeScale(1.0f);

    Director::get()->scheduleOnce([this]() {
        addChild(CircleWave::create(
            constants::presets::kCircleEffectPortalWave
        ))->setFollowTarget(exampleSprite_);

        auto circleEffect = addChild(CircleWave::create(
            constants::presets::kCircleEffectPortalWaveExtra
        ));

        circleEffect->setPosition(exampleSprite_->getPosition());
        circleEffect->setColor(Color3B::Green);        
    }, 1.0f);


    // Director::get()->createBlinkTween(
    //     exampleSprite_,
    //     constants::player::kRespawnBlinkDuration,
    //     constants::player::kRespawnBlinks,
    //     [this]() {
    //         exampleSprite_->setVisible(true);
    //     }
    // );

    // tilingSprite_ = addChild(TilingSprite::create("cube.png"));
    // tilingSprite_->setContentSize(Director::get()->getVisibleSize());
    // tilingSprite_->setPosition(Director::get()->getVisibleSize() / 2);
    // tilingSprite_->setAnchorPoint({.5f, .5f});
    // tilingSprite_->setColor(255, 0, 0);
    // tilingSprite_->setOpacity(60);

    // batchTest_ = addChild(SpriteBatchTest::create());
    // batchTest_->setPosition(Director::get()->getVisibleSize() / 2);

    // exampleSprite_ = addChild(Sprite::createWithFrame("GJ_levelComplete_001.png"));
    // exampleSprite_->setPosition(Director::get()->getVisibleSize() / 2);
    // exampleSprite_->setColor(255, 0, 0);

    // auto label = addChild(Label::createBigFont("Now with\nmultiline centered text!"));
    // label->setPosition(Point(Director::get()->getVisibleSize() / 2) - Point(0, 60));
    // label->setAnchorPoint(0.5, 0.5);
    // label->setColor(255, 0, 0);

    // auto ncs = exampleSprite_->addChild(Sprite::createWithFrame("ncs_med_001.png"));

    // LAYOUT TEST

    // auto bottomBar = addChild(Node::create());
    // bottomBar->makeWidthHugContents();
    // bottomBar->makeHeightHugContents();
    // bottomBar->setAnchorPoint({.5, 0});
    // bottomBar->setPosition(Director::get()->getVisibleSize().width / 2, 5);
    // bottomBar->useRowLayout()
    //     .gap(5);

    /*
    bottomBar->addChild(BounceButton::createWithSpriteFrame("GJ_achBtn_001.png"));
    bottomBar->addChild(BounceButton::createWithSpriteFrame("GJ_optionsBtn_001.png"));
    bottomBar->addChild(BounceButton::createWithSpriteFrame("GJ_statsBtn_001.png"));
    bottomBar->addChild(BounceButton::createWithSpriteFrame("GJ_ngBtn_001.png"));

    auto panel = addChild(SpritePanel::create("square01_001-uhd.png"));
    panel->setPosition(Director::get()->getVisibleSize() / 2);
    panel->setAnchorPoint({.5, .5});
    panel->setContentSize({300, 200});
    */

    return true;
}

}