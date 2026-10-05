#pragma once

#include "ColorNode.h"
#include "../core/Mesh.h"
#include <span>

namespace opendash::engine {

class DrawNode : public ColorNode {
public:
    CREATE_FUNC(DrawNode);

    inline void clear() {
        mesh_->resize(0);
    }

    void drawPolygon(
        std::span<Point> points,
        const Color4B& fillColor,
        float borderWidth,
        const Color4F& borderColor
    );

    void draw(Graphics* gfx) override;

    inline void setTexture(Texture* texture) { texture_ = texture; }
    inline Texture* getTexture() const { return texture_; } 

    inline void setAdditiveBlending(bool value) { additiveBlending_ = value; }
    inline bool isAdditiveBlending() const { return additiveBlending_; }

protected:
    bool init() override;

private:
    inline void addVertex(const Point& pos, const Color4B& color, const Point& uv) {
        u32 count = mesh_->getVertexCount();
        mesh_->resize(count + 1);
        mesh_->getBuffer()[count] = { pos.toGLM(), color, uv.toGLM() };
    }

    inline void increaseCount(u32 count) {
        mesh_->resize(mesh_->getVertexCount() + count);
    }

    inline MeshVertex* getVertexPointer() {
        return mesh_->getBuffer();
    }

    void markRangeDirty(u32 index, u32 count);

    void flushChanges();

private:
    Texture* texture_;
    bool additiveBlending_ = false;

    std::unique_ptr<Mesh> mesh_;

    bool dirty_ = false;
    u32 dirtyRangeMin_ = 0, dirtyRangeMax_ = 0;

};

};