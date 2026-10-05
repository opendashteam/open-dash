#include "Mesh.h"
#include "Graphics.h"

#define CAPACITY_INCREMENTS 128

namespace opendash::engine {

Mesh::~Mesh() {
    Graphics::get()->meshDestroy(internal_);
}

void Mesh::reserve(u32 vertexCount) {
    if (vertexCount <= capacity_)
        return;
    capacity_ = (vertexCount / CAPACITY_INCREMENTS + 1) * CAPACITY_INCREMENTS;
    Graphics::get()->meshResize(internal_, capacity_);
}

MeshVertex* Mesh::getBuffer() {
    return Graphics::get()->meshGetBuffer(internal_);
}

void Mesh::flushRange(u32 index, u32 count) {
    Graphics::get()->meshFlushBufferRange(internal_, index, count);
}

void Mesh::drawSolid(
    const glm::mat4& positionTransform,
    const Color4F& globalColor,
    bool blending
) const {
    Graphics::get()->drawMesh(internal_, positionTransform, globalColor, nullptr, vertexCount_, blending);
}

void Mesh::drawTextured(
    const glm::mat4& positionTransform,
    Texture* texture,
    const Color4F& globalColor,
    bool blending
) const {
    assert(texture);
    Graphics::get()->drawMesh(internal_, positionTransform, globalColor, texture->getInternalObject(), vertexCount_, blending);
}

std::unique_ptr<Mesh> Mesh::create() {
    auto gfx = Graphics::get();
    assert(gfx);
    
    auto mesh = std::unique_ptr<Mesh>(new Mesh);
    mesh->internal_ = gfx->meshCreate();
    return mesh;
}

};