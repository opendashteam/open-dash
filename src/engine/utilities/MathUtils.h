#pragma once

#include "../core/types.h"
#include "../core/macros.h"
#include <vector>
namespace opendash::engine
{

inline float deg(float radians) {
    return radians * 57.29577951f; // Match CC_RADIANS_TO_DEGREES
}

inline float rad(float degrees) {
    return degrees * 0.01745329252f; // Match CC_DEGREES_TO_RADIANS
}

inline std::vector<Point> computeCirclePoints(u32 segments) {
    std::vector<Point> points{};

    const float radiansPerSegment = 2.0f * CC_PI / segments;
    
    for (u32 i = 0; i < segments; i++) {
        u32 idx = (i % 2 == 0)
            ? i / 2
            : segments - 1 - (i - 1) / 2;

        float radians = idx * radiansPerSegment;
        points.push_back({cosf(radians), sinf(radians)});
    }

    return points;
}

inline OutlineMesh computeOutlineCirclePoints(u32 segments) {
    OutlineMesh mesh{};

    const float radiansPerSegment = 2.0f * CC_PI / segments;

    for (u32 i = 0; i < segments; i++) {
        u32 next = (i + 1) % segments;

        float radians = i * radiansPerSegment;
        float nextRadians = next * radiansPerSegment;

        glm::vec2 segStart{cosf(radians), sinf(radians)};
        glm::vec2 segEnd{cosf(nextRadians), sinf(nextRadians)};

        u32 base = static_cast<u32>(mesh.vertices.size());

        // 4 corners: side in {-1, +1}, endpoint in {0, 1}
        mesh.vertices.push_back({segStart, segEnd, -1.0f, 0.0f}); // 0
        mesh.vertices.push_back({segStart, segEnd,  1.0f, 0.0f}); // 1
        mesh.vertices.push_back({segStart, segEnd, -1.0f, 1.0f}); // 2
        mesh.vertices.push_back({segStart, segEnd,  1.0f, 1.0f}); // 3

        // Two triangles covering the quad
        mesh.indices.push_back(base + 0);
        mesh.indices.push_back(base + 1);
        mesh.indices.push_back(base + 2);

        mesh.indices.push_back(base + 2);
        mesh.indices.push_back(base + 1);
        mesh.indices.push_back(base + 3);
    }

    return mesh;
}

// TODO slerp2D, squareDistance, etc...
    
}