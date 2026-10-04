#version 460

layout (location = 0) in vec4 v_color;
layout (location = 1) in vec2 v_texCoord;

layout (location = 0) out vec4 v_fragColor;

layout (set = 2, binding = 0) uniform sampler2D meshTexture;

void main() {
    v_fragColor = texture(meshTexture, v_texCoord) * v_color;
}