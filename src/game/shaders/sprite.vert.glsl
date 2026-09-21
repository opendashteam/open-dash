#version 460

layout (location = 0) in vec2 a_position;
layout (location = 1) in vec2 a_uv;
layout (location = 0) out vec2 v_uv;

layout (set = 1, binding = 0) uniform TransformUBO {
    mat4 mvp;
} ubo;

void main() {
    gl_Position = ubo.mvp * vec4(a_position, 0.0, 1.0);
    v_uv = a_uv;
}