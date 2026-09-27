#version 460

layout (location = 0) in vec4 colorFromVertexShader;

layout (location = 0) out vec4 outputFragColor;

void main() {
    outputFragColor = colorFromVertexShader;
}