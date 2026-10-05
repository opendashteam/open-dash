#pragma once

#include "types.h"
#include "../Texture.h"

namespace opendash::engine {

using InternalMesh = void*;

struct MeshVertex {
    glm::vec2 position;
    Color4B   color;
    glm::vec2 texCoord;
};

class Mesh {
public:
    ~Mesh();

    void reserve(u32 vertexCount);

    inline void resize(u32 vertexCount) {
        if (vertexCount > capacity_)
            reserve(vertexCount);
        vertexCount_ = vertexCount;
    }

    inline u32 getVertexCount() const {
        return vertexCount_;
    }

    /*
        The returned buffer WILL change
        after calling resize()
    */
    MeshVertex* getBuffer();

    void flushRange(u32 index, u32 count);

    void drawSolid(
        const glm::mat4& positionTransform,
        const Color4F& globalColor = {1, 1, 1, 1},
        bool blending = false
    ) const;

    void drawTextured(
        const glm::mat4& positionTransform,
        Texture* texture,
        const Color4F& globalColor = {1, 1, 1, 1},
        bool blending = false
    ) const;

public:
    static std::unique_ptr<Mesh> create();

private:
    InternalMesh internal_;
    u32 vertexCount_ = 0;
    u32 capacity_ = 0;
};

};