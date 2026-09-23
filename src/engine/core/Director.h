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
    // setters
    void setScene(std::unique_ptr<Scene>& scene);

    // getters
    glm::mat4 getProjectionMatrix() const;
    Scene* getRunningScene() const;

    /*
        Get the dimensions of the visible portion of
        the world after all transformations (in points).
    */
    const Size& getVisibleSize() const;

    /*
        Get the base dimensions of the window in pixels.
    */
    const Size& getWindowSize() const;

    // internal methods, do not call outside of the engine
    void start();
    void end();
    void setVisibleSize(const Size& visibleSize);
    void setWindowSize(const Size& windowSize);
    
protected:
    bool init() override;
private:
    inline static Director* instance_ = nullptr;

    glm::mat4 projectionMatrix_{1.0f};
    std::unique_ptr<Scene> currentScene_ = nullptr;
    Size visibleSize_ = {0.0f, 0.0f};
    Size windowSize_  = {0.0f, 0.0f};
};

}