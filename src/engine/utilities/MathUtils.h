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

inline float randomMinus1And1() { // Match CCRANDOM_MINUS1_1
    return (2.0f * ( (float)rand() / RAND_MAX) ) - 1.0f;
}

inline float clampAngle(float angle) {
    if (angle > 360.0f)
        return angle - 360.0f;
    if (angle < 0.0f)
        return angle + 360.0f;
    return angle;
}

inline float getPerpendicularAngleCCW(float angle) {
    if (angle > 270.0f)
        return angle - 270.0f;
    else
        return angle + 90.0f;
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

inline float slerp2D(float fromAngle, float toAngle, float t)
{
    float halfFrom = fromAngle * 0.5f;
    float halfTo = toAngle   * 0.5f;

    float cosFrom = cosf(halfFrom);
    float sinFrom = sinf(halfFrom);
    float cosTo = cosf(halfTo);
    float sinTo = sinf(halfTo);

    float cosOmega = (sinTo * sinFrom) + (cosTo * cosFrom);

    if (cosOmega < 0.0f)
    {
        cosOmega = -cosOmega;
        sinTo = -sinTo;
        cosTo = -cosTo;
    }

    float coeff0 = 1.0f - t;
    float coeff1 = t;

    if ((1.0f - cosOmega) > 0.0001f)
    {
        float omega    = acosf(cosOmega);
        float sinOmega = sinf(omega);
        coeff0 = sinf((1.0f - t) * omega) / sinOmega;
        coeff1 = sinf(t * omega) / sinOmega;
    }

    double halfResult = atan2((sinFrom * coeff0) + (coeff1 * sinTo), (cosFrom * coeff0) + (coeff1 * cosTo));

    return (float)(halfResult + halfResult);
}

inline float squareDistance(float x1, float y1, float x2, float y2)
{
    return ((y2 - y1) * (y2 - y1)) + ((x2 - x1) * (x2 - x1));
}

inline void snapRotation360(float& rotation)
{
	if (rotation <= 180.0f) {
		if (rotation < -180.0f) {
			rotation += 360.0f;
		}
	}
	else {
		rotation -= 360.0f;
	}
}
    
}