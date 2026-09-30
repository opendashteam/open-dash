#include "Director.h"
#include "../../platform/Window.h"
#include "../AssetManager.h"
#include <SDL3/SDL.h>
namespace opendash::engine
{

void Director::start() {
    lastTime_ = getTime();
}

void Director::end() {
    AssetManager::get()->releaseAllTextures();
}

void Director::computeDeltaTime() {
    double now = getTime();
    deltaTime_ = now - lastTime_;
    lastTime_ = now;
}

void Director::updateLayouts() {
    if (!currentScene_) {
        layoutDirty_ = false;
        return;
    }

    currentScene_->traversePostorder([](Node* node) {
        if (node && node->getLayout())
            node->getLayout()->calculateMinSizeProjected();
    });

    currentScene_->layout();
    layoutDirty_ = false;
}

void Director::tick() {
    computeDeltaTime();

    float scaledDeltaTime = deltaTime_ * timeScale_;

    for (auto& tween : tweens_) {
        if (tween) tween->update(scaledDeltaTime);
    }

    for (Tween* target : pendingTweenRemovals_) {
        std::erase_if(tweens_, [target](const std::unique_ptr<Tween>& t) {
            return t.get() == target;
        });
    }
    pendingTweenRemovals_.clear();

    Graphics* gfx = platform::Window::getGraphics();

    if (!gfx->beginDraw()) return;

    // projectionMatrix_ = glm::ortho(0.0f, windowSize.width, 0.0f, windowSize.height, -1.0f, 1.0f);

    gfx->setViewProjectionMatrix(getProjectionMatrix());
    
    currentScene_->update(scaledDeltaTime);

    if (layoutDirty_)
        updateLayouts();

    currentScene_->render(gfx);

    gfx->finishDraw();
}

void Director::setScene(std::unique_ptr<Scene> &scene) {
  currentScene_ = std::move(scene);
}

void Director::setContentScaleFactor(float contentScaleFactor) {
    if (contentScaleFactor <= 0.0f)
        return;
    contentScaleFactor_ = contentScaleFactor;
    invertedContentScaleFactor_ = 1.0f / contentScaleFactor;
}

void Director::setDesignResolutionSize(const Size &designResolutionSize) {
    designResolutionSize_ = designResolutionSize;
    updateScreenScale();
}
void Director::setTimeScale(float timeScale) {
    timeScale_ = timeScale;
}

glm::mat4 Director::getProjectionMatrix() const {
    return projectionMatrix_;
}

Scene* Director::getRunningScene() const {
    return currentScene_.get();
}

const Size &Director::getDesignResolutionSize() const {
    return designResolutionSize_;
}

double Director::getTime() const {
    return platform::Window::getTime();
}

double Director::getLastTime() const {
    return lastTime_;
}

float Director::getDeltaTime() const {
    return deltaTime_;
}

float Director::getScreenScale() const {
    return screenScale_;
}

float Director::getScreenScaleFactorMax() const {
    return screenScaleFactorMax_;
}

float Director::getTimeScale() const {
    return timeScale_;
}

const Size& Director::getVisibleSize() const {
    return visibleSize_;
}

const Size &Director::getFrameSize() const { return frameSize_; }

Tween* Director::createTween(const TweenOptions &opt) {
    auto tween = Tween::create(opt);
    Tween* ret = tween.get();
    
    tweens_.push_back(std::move(tween)); 
    return ret;
}

void Director::removeTween(Tween *tween) {
    // Queue instead of deleting immediately
    pendingTweenRemovals_.push_back(tween);
}

void Director::createBlinkTween(Node* target, float duration, u32 blinks, Callback<> onComplete) {
    if (!target) return;

    auto tween = Tween::create({
        .from = 0.0f,
        .to = 1.0f,
        .duration = duration,
        .easingType = EasingType::Linear,
        .onUpdate = [target, blinks](float time) {
            if (!target) return;

            float slice = 1.0f / blinks;
            float m = fmodf(time, slice);
            target->setVisible(m > slice / 2);
        },
        .onComplete = [onComplete]() {
            if (onComplete) onComplete();
        },
        .deleteSelf = true
    });

    tween->start();
    tweens_.push_back(std::move(tween));
}

bool Director::init() {
    return true;
}

void Director::setVisibleSize(const Size &visibleSize) {
    visibleSize_ = visibleSize;
}

void Director::setFrameSize(const Size &frameSize) {
    frameSize_ = frameSize;
}

void Director::updateScreenScale()
{
    if (frameSize_.width <= 0.0f || frameSize_.height <= 0.0f) return;
    if (designResolutionSize_.width <= 0.0f || designResolutionSize_.height <= 0.0f) return;

    const Size& design = designResolutionSize_;
    const float scaleX = frameSize_.width  / design.width;
    const float scaleY = frameSize_.height / design.height;

    if (scaleY <= scaleX) { // ResolutionPolicy Fixed Height
        screenScale_ = scaleY;
        visibleSize_ = { frameSize_.width / scaleY, design.height };
    }
    else { // ResolutionPolicy Fixed Width
        screenScale_ = scaleX;
        visibleSize_ = { design.width, frameSize_.height / scaleX };
    }

    screenScaleFactorW_ = visibleSize_.width  / design.width;
    screenScaleFactorH_ = visibleSize_.height / design.height;
    screenScaleFactor_    = std::min(screenScaleFactorW_, screenScaleFactorH_);
    screenScaleFactorMax_ = std::max(screenScaleFactorW_, screenScaleFactorH_);

    projectionMatrix_ = glm::ortho(0.0f, visibleSize_.width, 0.0f, visibleSize_.height, -1.0f, 1.0f);
}

void Director::onWindowResized(const Size &frameSize)
{
    if (frameSize.width <= 0.0f || frameSize.height <= 0.0f)
        return; // minimized
    if (frameSize.width == frameSize_.width && frameSize.height == frameSize_.height)
        return;

    setFrameSize(frameSize);
    updateScreenScale();

    if (currentScene_)
        currentScene_->onViewResized();
}

}