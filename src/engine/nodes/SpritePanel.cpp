#include "SpritePanel.h"
#include "../AssetManager.h"
#include "../utilities/log.h"

namespace opendash::engine {

void SpritePanel::draw(Graphics* gfx) {
    if (isBatchDirty_)
        generateBatch();

    batch_->draw(texture_, getWorldTransform(), getRenderColor());
}

void SpritePanel::setContentSize(const Size& size) {
    ColorNode::setContentSize(size);
    isBatchDirty_ = true;
}

void SpritePanel::setCapInsets(Rect capInsets) {
    capInsets_ = capInsets;

    auto size = texture_->getSize();
    textureOffsets[0] = {0.0f, 0.0f};
    textureOffsets[1] = capInsets.origin;
    textureOffsets[2] = capInsets.origin + capInsets.size;
    textureOffsets[3] = texture_->getSize();

    log::info("textureSize: {}", size);
    log::info("capInsets: {}", capInsets);
    log::info("textureOffsets: {}, {}, {}, {}", textureOffsets[0], textureOffsets[1], textureOffsets[2], textureOffsets[3]);

    isBatchDirty_ = true;
}

std::unique_ptr<SpritePanel> SpritePanel::create(const std::filesystem::path& texturePath, const Rect& capInsets) {
    Texture* texture = AssetManager::get()->fetchTexture(texturePath);
    if (!texture) {
        log::err("Failed to initialize SpritePanel, could not load texture at path: {}", texturePath.string());
        return nullptr;
    }

    auto ret = std::unique_ptr<SpritePanel>(new SpritePanel);
    if (!ret->init(texture, capInsets))
        return nullptr;

    return ret;
}

std::unique_ptr<SpritePanel> SpritePanel::create(const std::filesystem::path& texturePath) {
    Texture* texture = AssetManager::get()->fetchTexture(texturePath);
    if (!texture) {
        log::err("Failed to initialize SpritePanel, could not load texture at path: {}", texturePath.string());
        return nullptr;
    }

    auto textureSizeThird = texture->getSize() / 3.0f;

    Rect capInsets = { textureSizeThird, textureSizeThird };

    auto ret = std::unique_ptr<SpritePanel>(new SpritePanel);
    if (!ret->init(texture, capInsets))
        return nullptr;

    return ret;
}

bool SpritePanel::init(Texture* texture, const Rect& capInsets) {
    assert(texture);
    texture_ = texture;
    setCapInsets(capInsets);
    return true;
}

void SpritePanel::generateBatch() {
    if (!batch_) {
        batch_ = SpriteBatch::create();
        batch_->resize(9);
    }

    Size texSize = texture_->getSize();

    Size totalEdgesSizeInPixels = {
        capInsets_.getMinX() + (texSize.width  - capInsets_.getMaxX()),
        capInsets_.getMinY() + (texSize.height - capInsets_.getMaxY())
    };

    Size middleSectionOffset = capInsets_.origin.toPoints();
    Size middleSectionSize   = getContentSize() - totalEdgesSizeInPixels.toPoints();

    Point offsets[4] = {
        Point(0, 0),
        middleSectionOffset,
        middleSectionOffset + middleSectionSize,
        getContentSize()
    };

    for (int x = 0; x < 3; x++) {
        for (int y = 0; y < 3; y++) {
            Point posOffset  = Point( offsets[x].x, offsets[y].y );
            Point posSize    = Point( offsets[x + 1].x, offsets[y + 1].y ) - posOffset;
            Point cropOffset = Point( textureOffsets[x].x, textureOffsets[y].y );
            Point cropSize   = Point( textureOffsets[x + 1].x, textureOffsets[y + 1].y ) - cropOffset;

            glm::mat3 texTransform = {
                cropSize.x / texSize.width, 0, 0,
                0, cropSize.y / texSize.height, 0,
                cropOffset.x / texSize.width, cropOffset.y / texSize.height, 0,
            };

            glm::mat4 posTransform{1.0f};

            posTransform = glm::translate(posTransform, glm::vec3(posOffset.toGLM(), 0.0f));
            posTransform = glm::scale(posTransform, glm::vec3(posSize.toGLM(), 0.0f));

            batch_->setSprite(y * 3 + x, posTransform, texTransform);
        }
    }

    isBatchDirty_ = false;
}

};