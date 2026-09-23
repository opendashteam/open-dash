#include "Scene.h"
#include "../core/types.h"
#include "../core/Application.h"

namespace opendash::engine
{

bool Scene::init()
{
    setAnchorPoint(0.0f, 0.0f);
    return true;
}

void Scene::render(Graphics* gfx) {
    for (auto& child : getChildren()) {
        child->visit(gfx);
    }
}

void Scene::update(float dt) {
    // override me
}

void Scene::setClearColor(const Color4F &clearColor) {
    clearColor_ = clearColor;
}

const Color4F& Scene::getClearColor() const {
    return clearColor_;
}

}