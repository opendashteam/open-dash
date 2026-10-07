#include "ExampleScene.h"
#include "constants.h"
#include "nodes/Ground.h"

namespace opendash
{

static Point moveVector = {1, 0};

void ExampleScene::update(float dt) {
    float movementAmount = constants::player::kSpeedNormal *
                           constants::player::kTimeModNormal *
                           constants::player::kPhysicsFrameRate *
                           dt;

    Director::get()->moveCameraByX(movementAmount);
    
    // const float minY = 105.0f;
    // const float maxY = Director::get()->getVisibleSize().height - exampleSprite_->getContentHeight() / 2;

    // float y = exampleSprite_->getPositionY();
    // if (inputDown_)
    //     y += movementAmount;
    // else
    //     y -= movementAmount;

    // auto oldPos = exampleSprite_->getPosition();

    // exampleSprite_->setPositionY(std::clamp(y, minY, maxY));

    // Point currentMoveVector = (exampleSprite_->getPosition() - oldPos).normalize();

    // if (
    //     std::abs(moveVector.x - currentMoveVector.x) > 0.01 ||
    //     std::abs(moveVector.y - currentMoveVector.y) > 0.01
    // ) {
    //     moveVector = currentMoveVector;
    //     waveTrail_->addPoint(oldPos);
    // }

    // waveTrail_->setPosition(exampleSprite_->getPosition());
    // waveTrail_->setCurrentPoint(exampleSprite_->getPosition());
    // waveTrail_->clearBehindXPos(Director::get()->getCameraPositionX() - 500.0f);
}

void ExampleScene::onCameraMoved() {
    // HACK: TEMPORARY, DO NOT RELY ON THIS, IT IS WRONG!!!!!
    ground_->setScrollX(Director::get()->getCameraPositionX());
    ground_->setPositionY(-Director::get()->getCameraPositionY() + 91.0f);
    
    background_->setScrollX(Director::get()->getCameraPositionX() * 0.1f);
    background_->setScrollY(-Director::get()->getCameraPositionY());

    exampleSprite_->setPositionX(Director::get()->getVisibleSize().width / 2 - 75.0f + Director::get()->getCameraPositionX());
}

bool ExampleScene::onMouseDown(const Point &pos, MouseButton button) {
    if (button == MouseButton::Left) {
        inputDown_ = true;
        return true;
    }
    return false;
}

void ExampleScene::onMouseUp(const Point& pos, MouseButton button) {
    if (button == MouseButton::Left)
        inputDown_ = false;
}

bool ExampleScene::init() {
    if (!Scene::init()) { // Always super init() first
        return false; 
    }

    // auto ps = addChild(ParticleSystem::create("speedEffect.plist"));
    // ps->positionType = PositionType::Relative;
    // ps = addChild(ParticleSystem::create("portalEffect01.plist"));
    // ps->positionType = PositionType::Relative;
    // ps = addChild(ParticleSystem::create("ringEffect.plist"));
    // ps->positionType = PositionType::Relative;

    // auto visibleSize = Director::get()->getVisibleSize();

    // // TIMEWARP
    // Director::get()->setTimeScale(1.0f);

    // Director::get()->scheduleOnce([this]() {
    //     addChild(CircleWave::create(
    //         constants::presets::kCircleEffectPortalWave
    //     ))->setFollowTarget(exampleSprite_);

    //     auto circleEffect = addChild(CircleWave::create(
    //         constants::presets::kCircleEffectPortalWaveExtra
    //     ));

    //     circleEffect->setPosition(exampleSprite_->getPosition());
    //     circleEffect->setColor(Color3B::Green);        
    // }, 1.0f);

    background_ = addChild(Background::create(1));

    // Always call addChild first when creating a node
    // to not have to deal with a stale pointer after the 
    // unique_ptr's move operation
    exampleSprite_ = addChild(Sprite::create("cube.png"));
    exampleSprite_->setPositionY(90.0f + 15.0f);

    ground_ = addChild(Ground::create({
        .groundID = 1,
        .lineType = GroundLineType::Normal
    }));

    ground_->setPositionY(91.0f); // Correct

    // waveTrail_ = addChild(WaveTrail::create());
    // waveTrail_->scheduleAutoUpdate();
    // waveTrail_->resumeStroke();
    // waveTrail_->addPoint(exampleSprite_->getPosition());
    // waveTrail_->setSolid(false);
    // waveTrail_->setColor(Color3B {0, 125, 255});
    // waveTrail_->setAdditiveBlending(true);

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