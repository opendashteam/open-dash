#version 460

#include "common.h"

layout (location = 0) in vec2 a_position;
layout (location = 1) in vec2 a_texCoord;
layout (location = 2) in vec4 a_color;

layout (location = 0) out vec2 v_uv;
layout (location = 1) out vec4 v_color;

UNIFORM_BUFFER(UNIFORM_SLOT_VIEW_PROJECTION) SpriteProperties {
    mat4 viewProjection;
};

UNIFORM_BUFFER(UNIFORM_SLOT_SPRITE_BATCH_PROPERTIES) SpriteBatchProperties {
    mat4 modelMatrix;
};

void main() {
    gl_Position = viewProjection * modelMatrix * vec4(a_position, 0.0, 1.0);
    v_uv = a_texCoord;
    v_color = a_color;
}