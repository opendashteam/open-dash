#include "Sprite.h"
#include "AssetManager.h"
#include "Application.h"
#include "log.h"

namespace opendash::engine
{

std::unique_ptr<Sprite> Sprite::create(const std::filesystem::path& path) {
    auto ret = std::make_unique<Sprite>();

    if (!ret->initWithPath(path))
        return nullptr;

    return ret;
}

bool Sprite::initWithPath(const std::filesystem::path& path) {
    auto* am = AssetManager::get();
    if (!am->isTextureCached(path) && !am->cacheTexture(path)) {
        log::err("Failed to initialize sprite, could not load texture at path: {}", path.string());
        return false;
    }

    texture_ = am->getCachedTexture(path);
    if (!texture_) {
        return false;
    }

    setContentSize(static_cast<float>(texture_->getWidth()), static_cast<float>(texture_->getHeight()));
    return true;
}

void Sprite::draw(Graphics* gfx) {
    auto* app = Application::get();
    glm::mat4 contentScale = glm::scale(glm::mat4(1.0f), glm::vec3(getContentWidth(), getContentHeight(), 1.0f));
    glm::mat4 transformation = app->getProjectionMatrix() * getWorldTransform() * contentScale;

    gfx->drawSprite(texture_, transformation, {}, {1, 0, 1, 1});
}

bool Sprite::init() {
    return Node::init();
}

}