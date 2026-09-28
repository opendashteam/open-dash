#version 460

#include "common.h"

layout (location = 0) in vec2 inputVertexPos;

layout (location = 0) out vec4 outputColor;

UNIFORM_BUFFER(UNIFORM_SLOT_VIEW_PROJECTION) ViewProjection {
    mat4 viewProjection;
};

UNIFORM_BUFFER(UNIFORM_SLOT_CIRCLE_UBO) CircleUBO {
    mat4 positionTransform;
    vec4 color;
};

void main() {
    gl_Position = viewProjection * positionTransform * vec4(inputVertexPos, 0.0, 1.0);
    outputColor = color;
}