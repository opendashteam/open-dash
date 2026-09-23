#include "ExampleScene.h"
#include "../engine/utilities/log.h"

using namespace opendash;

int main(int argc, char* argv[]) {

    // TODO: store the previous window dimensions somewhere on disk and load them here
    auto app = engine::Application::create("Open Dash", 1280, 720);
    if (!app)
        return 1;

    auto scene = ExampleScene::create();
    if (!scene) {
        return 1;
    }

    if (AssetManager::get()->loadSpriteSheet("GJ_GameSheet03-uhd")) {
        const auto& frames = AssetManager::get()->getSpriteFrames();

        for (const auto& [k, v] : frames) {
            log::info("{}:", k);
            log::info("  - rotated: {}", v->isRotated());
            log::info("  - textureRect: {}", v->getTextureRect());
            log::info("  - spriteOffset: {}", v->getSpriteOffset());
            log::info("  - spriteSize: {}", v->getSpriteSize());
            log::info("  - spriteSourceSize: {}", v->getSpriteSourceSize());
        }
    }

    app->setScene(std::move(scene));
    app->run();
    app->quit();

    return 0;
}