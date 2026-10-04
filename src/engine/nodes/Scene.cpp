#include "Scene.h"

namespace opendash::engine
{

bool Scene::init()
{
    setAnchorPoint(0.0f, 0.0f);
    return true;
}

void Scene::render(Graphics* gfx) {
    visit(gfx);
}

void Scene::update(float dt) {
    // Override me
}

void Scene::onViewResized() {
    // Override me

    // E.g. UI layout refresh on window resize
}

void Scene::onCameraMoved() {
    // Override me
}

}