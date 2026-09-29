#pragma once

#include "Singleton.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../nodes/Scene.h"
#include "InputScheduler.h"

namespace opendash::engine
{

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

    // getters
    glm::mat4 getProjectionMatrix() const;
    Scene* getRunningScene() const;
    inline float getContentScaleFactor() const { return contentScaleFactor_; }
    inline float getInvertedContentScaleFactor() const { return invertedContentScaleFactor_; }
    const Size& getDesignResolutionSize() const;
    double getTime() const;
    float getDeltaTime() const;
    float getScreenScaleFactorMax() const;
    inline InputScheduler& getInputScheduler() { return inputScheduler_; }

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
    std::unique_ptr<Scene> currentScene_ = nullptr;

    bool areLayoutsDirty_ = true;

    Size visibleSize_ = {0.0f, 0.0f};
    Size frameSize_  = {0.0f, 0.0f};
    Size designResolutionSize_ = {0.0f, 0.0f};

    float contentScaleFactor_ = 1.0f;
    float invertedContentScaleFactor_ = 1.0f;

    double lastTime_ = 0.0;
    float deltaTime_ = 0.0f;

    float screenScaleFactorW_   = 0.0f;
    float screenScaleFactorH_   = 0.0f;
    float screenScaleFactor_    = 0.0f;
    float screenScaleFactorMax_ = 0.0f;
    float screenScale_          = 0.0f;
};

}