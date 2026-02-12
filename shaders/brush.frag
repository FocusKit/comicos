// Brush dab fragment shader.
// Renders individual brush dabs with hardness falloff and texture.
//
// Extension point: here is where the brush pipeline shader goes.
// Future features:
// - Brush texture stamps (procedural or image-based)
// - Wet paint simulation
// - Scatter / jitter
// - Dual brush
// - Color dynamics

#version 440

layout(location = 0) in vec2 v_texCoord;
layout(location = 0) out vec4 fragColor;

layout(std140, binding = 0) uniform BrushParams {
    vec4 brushColor;    // RGBA brush color
    float radius;       // Brush radius in pixels
    float hardness;     // 0.0 = soft, 1.0 = hard
    float opacity;      // Dab opacity (from pressure)
    float rotation;     // Dab rotation in radians
};

// layout(binding = 1) uniform sampler2D brushTexture;  // Extension: texture stamp

void main() {
    // Distance from center (0,0 = center, 1,1 = corner)
    vec2 center = v_texCoord * 2.0 - 1.0;
    float dist = length(center);

    if (dist > 1.0) discard;

    // Hardness falloff
    float alpha;
    if (dist <= hardness) {
        alpha = 1.0;
    } else {
        alpha = 1.0 - (dist - hardness) / (1.0 - hardness + 0.001);
    }

    alpha *= opacity;
    fragColor = vec4(brushColor.rgb, brushColor.a * alpha);
}
