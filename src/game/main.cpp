#include "ExampleScene.h"

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

    app->setScene(std::move(scene));
    app->run();
    app->quit();

    return 0;
}