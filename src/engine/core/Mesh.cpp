#include "Mesh.h"
#include "Graphics.h"

namespace opendash::engine {

Mesh::~Mesh() {
    Graphics::get()->meshDestroy(internal_);
}

void Mesh::resize(u32 vertexCount) {
    Graphics::get()->meshResize(internal_, vertexCount);
    vertexCount_ = vertexCount;
}

MeshVertex* Mesh::getBuffer() {
    return Graphics::get()->meshGetBuffer(internal_);
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