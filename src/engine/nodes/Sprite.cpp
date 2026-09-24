#include "Sprite.h"
#include "../AssetManager.h"
#include "../utilities/log.h"
#include "../core/Director.h"
namespace opendash::engine
{

std::unique_ptr<Sprite> Sprite::create(const std::filesystem::path& path) {
    auto ret = std::make_unique<Sprite>();

    if (!ret->initWithPath(path))
        return nullptr;

    return ret;
}

std::unique_ptr<Sprite> Sprite::createWithFrame(SpriteFrame* spriteFrame) {
    auto ret = std::make_unique<Sprite>();

    if (!ret->initWithSpriteFrame(spriteFrame))
        return nullptr;

    return ret;
}

std::unique_ptr<Sprite> Sprite::createWithFrame(const std::string& spriteFrameName) {
    SpriteFrame* spriteFrame = AssetManager::get()->getSpriteFrameByName(spriteFrameName);
    if (!spriteFrame) {
        log::err("Failed to initialize sprite, could not find sprite frame with name: {}", spriteFrameName);
        return nullptr;
    }

    return createWithFrame(spriteFrame);
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

    Size pixelSize = texture_->getSize();
    setupSizes(pixelSize, pixelSize);

    // sprite-specific defaults
    setAnchorPoint({0.5f, 0.5f});

    return true;
}

bool Sprite::initWithSpriteFrame(SpriteFrame* frame) {
    assert(frame != nullptr);

    spriteFrame_ = frame;

    setupSizes(frame->getSpriteSourceSize(), frame->getSpriteSize());
    internalSpriteTransform_ = frame->getPositionTransform();

    // sprite-specific defaults
    setAnchorPoint({0.5f, 0.5f});

    return true;
}

void Sprite::draw(Graphics* gfx) {
    assert(texture_ || spriteFrame_);

    glm::mat4 positionTransform = getWorldTransform() * internalSpriteTransform_;
    glm::mat3 textureTransform(1.0f);

    Texture* texture = texture_;

    if (spriteFrame_) {
        texture = spriteFrame_->getTexture();
        textureTransform = spriteFrame_->getTextureTransform();
    }

    gfx->drawSprite(texture, positionTransform, textureTransform, renderColor_);
}

bool Sprite::init() {
    return Node::init();
}

void Sprite::setupSizes(const Size &contentPixels, const Size &quadPixels)
{
    float inv = 1.0f / Director::get()->getContentScaleFactor();

    setContentSize(contentPixels.width * inv, contentPixels.height * inv);
    internalSpriteTransform_ = glm::scale(glm::mat4(1.0f), glm::vec3(quadPixels.width * inv, quadPixels.height * inv, 1.0f));
}

}