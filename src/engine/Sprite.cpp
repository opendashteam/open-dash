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

    gfx->drawSprite(texture_, transformation, {}, renderColor_);
}

bool Sprite::init() {
    return Node::init();
}

void Sprite::setColor(const Color3B &color) {
    color_ = color;
    renderColor_ = Color4F::fromColor3B(color, renderColor_.a);
}

void Sprite::setColor(u8 r, u8 g, u8 b) {
    setColor(Color3B{r, g, b});
}

void Sprite::setColor(const Color4F &color) {
    color_ = Color3B::fromColor4F(color);
    renderColor_ = color;
    opacity_ = static_cast<u8>(color.a * 255.0f);
}

void Sprite::setOpacity(u8 opacity) {
    opacity_ = opacity;
    renderColor_.a = opacity / 255.0f;
}

const Color3B &Sprite::getColor() const {
    return color_;
}

u8 Sprite::getOpacity() const {
    return opacity_;
}

const Color4F &Sprite::getRenderColor() const {
    return renderColor_;
}

}