#pragma once

#include "Singleton.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../nodes/Scene.h"

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
    float getContentScaleFactor() const;
    const Size& getDesignResolutionSize() const;
    double getTime() const;
    float getDeltaTime() const;
    float getScreenScaleFactorMax() const;

    /*
        Get the dimensions of the visible portion of
        the world after all transformations (in points).
    */
    const Size& getVisibleSize() const;

    /*
        Get the base dimensions of the window in pixels.
    */
    const Size& getFrameSize() const;

    // internal methods, do not call outside of the engine
    void start();
    void end();
    void setVisibleSize(const Size& visibleSize);
    void setFrameSize(const Size& frameSize);
    void updateScreenScale();
    void onWindowResized(const Size& frameSize);
    
protected:
    bool init() override;
    void computeDeltaTime();
private:
    inline static Director* instance_ = nullptr;

    glm::mat4 projectionMatrix_{1.0f};
    std::unique_ptr<Scene> currentScene_ = nullptr;

    Size visibleSize_ = {0.0f, 0.0f};
    Size frameSize_  = {0.0f, 0.0f};
    Size designResolutionSize_ = {0.0f, 0.0f};

    float contentScaleFactor_ = 1.0f;

    double lastTime_ = 0.0;
    float deltaTime_ = 0.0f;

    float screenScaleFactorW_   = 0.0f;
    float screenScaleFactorH_   = 0.0f;
    float screenScaleFactor_    = 0.0f;
    float screenScaleFactorMax_ = 0.0f;
    float screenScale_          = 0.0f;
};

}