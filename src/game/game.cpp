#include "ExampleScene.h"
#include "../engine/game.h"
#include "../engine/core/Application.h"

using namespace opendash;

static std::unique_ptr<Application> application = nullptr;

bool game::init() {
    auto app = Application::create("Open Dash", 1600, 900);
    if (!app) 
        return false;

    if (!AssetManager::get()->loadSpriteSheet("GJ_GameSheet03-uhd"))
        return false;

    auto scene = ExampleScene::create();
    if (!scene) 
        return false;

    app->setScene(std::move(scene));
    app->run();

    application = std::move(app);
    return true;
}

bool game::loop() {
    application->tick();
    return true;
}

void game::quit() {
    application->quit();
    application = nullptr;
}