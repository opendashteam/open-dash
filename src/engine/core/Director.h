#pragma once

#include "Singleton.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../nodes/Scene.h"
#include "InputScheduler.h"
#include "../utilities/Tween.h"

namespace opendash::engine
{

struct ScheduledCallback {
    Callback<> function;
    float delayRemaining;
};

/*
    CCDirector equivalent. Handles the scene stack and engine state.
*/
class Director : public Singleton<Director> {
    friend class Singleton<Director>;
public:
    void tick();

    // setters
    void setScene(std::unique_ptr<Scene>& scene);
    void setContentScaleFactor(float contentScaleFactor);
    void setDesignResolutionSize(const Size& designResolutionSize);
    void setTimeScale(float timeScale);

    // Camera shenanigans
    void updateCameraViewProjection();
    void setCameraPosition(const Point& cameraPosition);
    void setCameraPosition(float x, float y);
    void setCameraPositionX(float cameraPositionX);
    void setCameraPositionY(float cameraPositionY);
    void moveCameraBy(float deltaX, float deltaY);
    void moveCameraByX(float deltaX);
    void moveCameraByY(float deltaY);
    const Point& getCameraPosition() const;
    float getCameraPositionX() const;
    float getCameraPositionY() const;
    void setUseCameraIndependentProjection(Graphics* gfx, bool cameraIndependent);

    // getters
    glm::mat4 getProjectionMatrix() const;
    Scene* getRunningScene() const;
    inline float getContentScaleFactor() const { return contentScaleFactor_; }
    inline float getInvertedContentScaleFactor() const { return invertedContentScaleFactor_; }
    const Size& getDesignResolutionSize() const;
    double getTime() const;
    double getLastTime() const;
    float getDeltaTime() const;
    float getScreenScale() const;
    float getScreenScaleFactorMax() const;
    inline InputScheduler& getInputScheduler() { return inputScheduler_; }
    float getTimeScale() const;
    float getScaledDeltaTime() const;

    /*
        Get the dimensions of the visible portion of
        the world after all transformations (in points).
    */
    const Size& getVisibleSize() const;

    /*
        Get the base dimensions of the window in pixels.
    */
    const Size& getFrameSize() const;

    Point toWorldPosition(const Point& screenPos) const;
    Point toScreenPosition(const Point& worldPos) const;

    // Tween stuff
    Tween& createTween();
    void removeTween(Tween* tween);
    void handleTweens();

    // Types of tweens
    void createBlinkTween(Node* target, float duration, u32 blinks, Callback<> onComplete);

    // Scheduling-related
    void scheduleNextFrame(Callback<> callback);
    void scheduleOnce(Callback<> callback, float delaySeconds);
    void handleScheduledCallbacks();

    // internal methods, do not call outside of the engine
    void start();
    void end();
    void setVisibleSize(const Size& visibleSize);
    void setFrameSize(const Size& frameSize);
    void updateScreenScale();
    void onWindowResized(const Size& frameSize);
    
    inline void markLayoutsDirty() { areLayoutsDirty_ = true; }
    
protected:
    bool init() override;
    void computeDeltaTime();
    void updateLayouts();
private:
    inline static Director* instance_ = nullptr;

    InputScheduler inputScheduler_;

    glm::mat4 projectionMatrix_{1.0f};
    glm::mat4 inverseProjectionMatrix_{1.0f};
    glm::mat4 projectionMatrixCameraIndependent_{1.0f};
    glm::mat4 inverseProjectionMatrixCameraIndependent_{1.0f};

    bool useIndependantCameraProjection_ = false;

    std::unique_ptr<Scene> currentScene_ = nullptr;

    bool areLayoutsDirty_ = true;

    Point cameraPosition_ = PointZero;
    Size visibleSize_ = PointZero;
    Size frameSize_  = PointZero;
    Size designResolutionSize_ = PointZero;

    float contentScaleFactor_ = 1.0f;
    float invertedContentScaleFactor_ = 1.0f;

    double lastTime_ = 0.0;
    float deltaTime_ = 0.0f;
    float timeScale_ = 1.0f;
    float scaledDeltaTime_ = 0.0f;

    float screenScaleFactorW_   = 0.0f;
    float screenScaleFactorH_   = 0.0f;
    float screenScaleFactor_    = 0.0f;
    float screenScaleFactorMax_ = 0.0f;
    float screenScale_          = 0.0f;

    std::vector<std::unique_ptr<Tween>> tweens_{};
    std::vector<Tween*> pendingTweenRemovals_;

    std::vector<ScheduledCallback> scheduledCallbacks_{};
};

}