#version 460

#include "common.h"

layout(location = 0) in vec2 a_segStart;
layout(location = 1) in vec2 a_segEnd;
layout(location = 2) in float a_side; // -1 or +1
layout(location = 3) in float a_endpoint; // 0 or 1

layout(location = 0) out vec2 v_segStart;
layout(location = 1) out vec2 v_segEnd;
layout(location = 2) out vec2 v_fragWorldPos;
layout(location = 3) out vec4 v_color;
layout(location = 4) out float v_lineWidth;

UNIFORM_BUFFER(UNIFORM_SLOT_VIEW_PROJECTION) ViewProjection {
    mat4 viewProjection;
};

UNIFORM_BUFFER(UNIFORM_SLOT_OUTLINE_CIRCLE_UBO) OutlineCircleUBO {
    mat4 positionTransform;
    vec4 color;
    float lineWidth;
};

void main() {
    vec2 worldStart = (positionTransform * vec4(a_segStart, 0.0, 1.0)).xy;
    vec2 worldEnd   = (positionTransform * vec4(a_segEnd,   0.0, 1.0)).xy;

    vec2 delta = worldEnd - worldStart;
    bool shallow = abs(delta.y) < abs(delta.x);
    float halfWidth = lineWidth * 0.5;
    vec2 offset = shallow
                    ? vec2(0.0, a_side * halfWidth)
                    : vec2(a_side * halfWidth, 0.0);

    vec2 worldBase = mix(worldStart, worldEnd, a_endpoint) + offset;

    v_segStart = worldStart;
    v_segEnd = worldEnd;
    v_fragWorldPos = worldBase;
    v_color = color;
    v_lineWidth = lineWidth;

    gl_Position = viewProjection * vec4(worldBase, 0.0, 1.0);
}