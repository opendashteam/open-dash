#include "Scene.h"
#include "types.h"
#include "Application.h"

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

}