#version 460

#include "common.h"

layout (location = 0) in vec2 a_position;
layout (location = 1) in vec4 a_color;

layout (location = 0) out vec4 v_color;

UNIFORM_BUFFER(UNIFORM_SLOT_VIEW_PROJECTION) ViewProjection {
    mat4 viewProjection;
};

UNIFORM_BUFFER(UNIFORM_SLOT_MESH_UBO) MeshUBO {
    mat4 positionTransform;
    vec4 color;
};

void main() {
    gl_Position = viewProjection * positionTransform * vec4(a_position, 0.0, 1.0);
    v_color = a_color * color;
}