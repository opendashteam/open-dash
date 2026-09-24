#version 460

#include "common.h"

layout (location = 0) in vec2 a_position;

layout (location = 0) out vec2 v_uv;
layout (location = 1) out vec4 v_color;

UNIFORM_BUFFER(UNIFORM_SLOT_VIEW_PROJECTION) ViewProjection {
    mat4 viewProjection;
};

UNIFORM_BUFFER(UNIFORM_SLOT_SPRITE_PROPERTIES) SpriteProperties {
    mat4 positionTransform;
    mat3 textureTransform;
    vec4 color;
};

void main() {
    gl_Position = viewProjection * positionTransform * vec4(a_position, 0.0, 1.0);
    v_uv = (textureTransform * vec3(a_position, 1.0)).xy;
    v_color = color;
}