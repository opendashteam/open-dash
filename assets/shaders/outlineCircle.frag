#version 460

layout(location = 0) in vec2 v_segStart;
layout(location = 1) in vec2 v_segEnd;
layout(location = 2) in vec2 v_fragWorldPos;
layout(location = 3) in vec4 v_color;
layout(location = 4) in float v_lineWidth;

layout (location = 0) out vec4 v_fragColor;

void main() {
    vec2 delta = v_segEnd - v_segStart;
    float halfWidth = v_lineWidth * 0.5;
    bool shallowLine = abs(delta.y) < abs(delta.x);

    // Deliberately recreate the inconsistencies and bugs with the
    // legacy `glLineWidth`. It extrudes line width in the dominant
    // axis (x or y) instead of perpendicular to the segment.
    if (shallowLine) {
        float t = (v_fragWorldPos.x - v_segStart.x) / delta.x;
        float lineY = mix(v_segStart.y, v_segEnd.y, t);
        float diff = v_fragWorldPos.y - lineY;
        if (t < 0.0 || t > 1.0 || diff < -halfWidth || diff >= halfWidth)
            discard;
    } else {
        float t = (v_fragWorldPos.y - v_segStart.y) / delta.y;
        float lineX = mix(v_segStart.x, v_segEnd.x, t);
        float diff = v_fragWorldPos.x - lineX;
        if (t < 0.0 || t > 1.0 || diff < -halfWidth || diff >= halfWidth)
            discard;
    }

    v_fragColor = v_color;
}