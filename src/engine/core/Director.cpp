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
    platform::Window::setInputScheduler(nullptr);

    AssetManager::get()->releaseAllTextures();
    AssetManager::get()->releaseAllFonts();
}

void Director::computeDeltaTime() {
    double now = getTime();
    deltaTime_ = now - lastTime_;
    lastTime_ = now;
    scaledDeltaTime_ = deltaTime_ * timeScale_;
}

void Director::updateLayouts() {
    if (!currentScene_) {
        areLayoutsDirty_ = false;
        return;
    }

    currentScene_->traversePostorder([](Node* node) {
        if (node && node->getLayout())
            node->getLayout()->calculateMinSizeProjected();
    });

    currentScene_->layout();
    areLayoutsDirty_ = false;
}

void Director::handleTweens() {
    for (auto& tween : tweens_) {
        if (tween) tween->update(scaledDeltaTime_);
    }

    for (Tween* target : pendingTweenRemovals_) {
        std::erase_if(tweens_, [target](const std::unique_ptr<Tween>& t) {
            return t.get() == target;
        });
    }
    pendingTweenRemovals_.clear();
}

void Director::tick() {
    computeDeltaTime();
    handleTweens();
    handleScheduledCallbacks();

    Graphics* gfx = platform::Window::getGraphics();

    if (!gfx->beginDraw()) return;

    // projectionMatrix_ = glm::ortho(0.0f, windowSize.width, 0.0f, windowSize.height, -1.0f, 1.0f);

    gfx->setViewProjectionMatrix(getProjectionMatrix());
    
    currentScene_->update(scaledDeltaTime_);

    if (areLayoutsDirty_)
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

void Director::updateCameraViewProjection() {
    projectionMatrix_ = glm::ortho(
        cameraPosition_.x, cameraPosition_.x + visibleSize_.width,
        cameraPosition_.y, cameraPosition_.y + visibleSize_.height,
        -1.0f, 1.0f
    );
    inverseProjectionMatrix_ = glm::inverse(projectionMatrix_);
}

void Director::setCameraPosition(const Point &cameraPosition) {
    if (cameraPosition.x == cameraPosition_.x && cameraPosition.y == cameraPosition_.y)
        return;

    cameraPosition_ = cameraPosition;

    if (currentScene_) {
        currentScene_->onCameraMoved();
    }

    updateCameraViewProjection();
}

void Director::setCameraPosition(float x, float y) {
    setCameraPosition({x, y});
}

void Director::setCameraPositionX(float cameraPositionX) {
    setCameraPosition({cameraPositionX, cameraPosition_.y});
}

void Director::setCameraPositionY(float cameraPositionY) {
    setCameraPosition({cameraPosition_.x, cameraPositionY});
}

void Director::moveCameraBy(float deltaX, float deltaY) {
    setCameraPosition({cameraPosition_.x + deltaX, cameraPosition_.y + deltaY});
}

void Director::moveCameraByX(float deltaX) {
    setCameraPosition({cameraPosition_.x + deltaX, cameraPosition_.y});
}

void Director::moveCameraByY(float deltaY) {
    setCameraPosition({cameraPosition_.x, cameraPosition_.y + deltaY});
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

float Director::getScaledDeltaTime() const {
    return scaledDeltaTime_;
}

const Point &Director::getCameraPosition() const { return cameraPosition_; }
float Director::getCameraPositionX() const { return cameraPosition_.x; }
float Director::getCameraPositionY() const { return cameraPosition_.y; }

void Director::setUseCameraIndependentProjection(Graphics *gfx, bool cameraIndependent) {
    if (cameraIndependent == useIndependantCameraProjection_)
        return;

    useIndependantCameraProjection_ = cameraIndependent;

    gfx->setViewProjectionMatrix(
        cameraIndependent
            ? projectionMatrixCameraIndependent_
            : projectionMatrix_
    );
}

const Size& Director::getVisibleSize() const {
    return visibleSize_;
}

const Size &Director::getFrameSize() const { return frameSize_; }

Tween& Director::createTween() {
    auto tween = Tween::create();
    Tween* ret = tween.get();
    
    tweens_.push_back(std::move(tween)); 
    return *ret;
}

void Director::removeTween(Tween *tween) {
    // Queue instead of deleting immediately
    pendingTweenRemovals_.push_back(tween);
}

void Director::createBlinkTween(Node* target, float duration, u32 blinks, Callback<> onComplete) {
    if (!target) return;

    createTween()
        .from(0.0f)
        .to(0.0f)
        .duration(duration)
        .type(EasingType::Linear)
        .onUpdate([target, blinks](float time) {
            if (!target) return;

            float slice = 1.0f / blinks;
            float m = fmodf(time, slice);
            target->setVisible(m > slice / 2);
        })
        .onComplete([onComplete]() {
            if (onComplete) onComplete();
        })
        .deleteSelf()
        .start();
}

void Director::scheduleNextFrame(Callback<> callback) {
    scheduleOnce(callback, 0.0f);
    // Using a delay of zero means it will be automatically picked up
    // the next iteration of Director::tick, since this function can't be called
    // before scheduled callbacks are handled.
}

// WARNING: You will run into bugs if you schedule from inside another callback
// TODO: make sequence system
void Director::scheduleOnce(Callback<> callback, float delaySeconds) {
    if (!callback) return;

    scheduledCallbacks_.push_back({callback, delaySeconds});
}

void Director::handleScheduledCallbacks() {
    for (auto& callback : scheduledCallbacks_) {
        callback.delayRemaining -= scaledDeltaTime_;
        if (callback.delayRemaining <= 0.0f)
            callback.function();
    }

    std::erase_if(scheduledCallbacks_, [](const ScheduledCallback& callback) {
        return callback.delayRemaining <= 0.0f;
    });
}

Point Director::toWorldPosition(const Point& screenPos) const {
    glm::vec4 ndc = {
        screenPos.x / frameSize_.width * 2.0f - 1.0f,
        1.0 - screenPos.y / frameSize_.height * 2.0f,
        0.0f,
        1.0f
    };
    glm::vec4 ret = inverseProjectionMatrix_ * ndc;
    return {ret.x, ret.y};
}

Point Director::toScreenPosition(const Point& worldPos) const {
    glm::vec4 ndc = projectionMatrix_ * glm::vec4(worldPos.toGLM(), 0, 1);
    return Point(
        (ndc.x + 1.0f) / 2.0f * frameSize_.width,
        (1.0f - ndc.y) / 2.0f * frameSize_.height
    );
}

bool Director::init() {
    platform::Window::setInputScheduler(&inputScheduler_);

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

    updateCameraViewProjection();

    projectionMatrixCameraIndependent_ = glm::ortho(
        0.0f, visibleSize_.width,
        0.0f, visibleSize_.height,
        -1.0f, 1.0f
    );
    inverseProjectionMatrixCameraIndependent_ = glm::inverse(projectionMatrixCameraIndependent_);
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