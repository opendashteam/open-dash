#version 460

layout (location = 0) in vec2 a_position;

layout (location = 0) out vec2 v_uv;
layout (location = 1) out vec4 v_color;

layout (set = 1, binding = 0) uniform TransformUBO {
    mat4 mvp;
    vec4 color;
} ubo;

void main() {
    gl_Position = ubo.mvp * vec4(a_position, 0.0, 1.0);
    v_uv = a_position + vec2(0.5, 0.5);
    v_color = ubo.color;
}