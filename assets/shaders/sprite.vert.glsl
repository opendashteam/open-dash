#version 460

layout (location = 0) in vec2 a_position;

layout (location = 0) out vec2 v_uv;
layout (location = 1) out vec4 v_color;

layout (std140, set = 1, binding = 0) uniform TransformUBO {
    mat4 positionTransform;
    mat3 textureTransform;
    vec4 color;
} ubo;

void main() {
    gl_Position = ubo.positionTransform * vec4(a_position, 0.0, 1.0);
    v_uv = (ubo.textureTransform * vec3(a_position, 1.0)).xy;
    v_color = ubo.color;
}