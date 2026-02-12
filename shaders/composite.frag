// Layer compositing fragment shader.
// Blends layers together using various blend modes.
//
// Extension point: here is where GPU-accelerated compositing goes.
// Each blend mode is a different mixing function.
// Future: compute shader version for better parallelism.

#version 440

layout(location = 0) in vec2 v_texCoord;
layout(location = 0) out vec4 fragColor;

layout(binding = 1) uniform sampler2D bottomLayer;
layout(binding = 2) uniform sampler2D topLayer;

layout(std140, binding = 0) uniform CompositeParams {
    int blendMode;      // 0=Normal, 1=Multiply, 2=Screen, 3=Overlay
    float layerOpacity; // Top layer opacity
    vec2 padding;
};

vec3 blendNormal(vec3 base, vec3 blend) {
    return blend;
}

vec3 blendMultiply(vec3 base, vec3 blend) {
    return base * blend;
}

vec3 blendScreen(vec3 base, vec3 blend) {
    return 1.0 - (1.0 - base) * (1.0 - blend);
}

vec3 blendOverlay(vec3 base, vec3 blend) {
    return mix(
        2.0 * base * blend,
        1.0 - 2.0 * (1.0 - base) * (1.0 - blend),
        step(0.5, base)
    );
}

void main() {
    vec4 bottom = texture(bottomLayer, v_texCoord);
    vec4 top = texture(topLayer, v_texCoord);

    top.a *= layerOpacity;

    vec3 blended;
    if (blendMode == 0) blended = blendNormal(bottom.rgb, top.rgb);
    else if (blendMode == 1) blended = blendMultiply(bottom.rgb, top.rgb);
    else if (blendMode == 2) blended = blendScreen(bottom.rgb, top.rgb);
    else if (blendMode == 3) blended = blendOverlay(bottom.rgb, top.rgb);
    else blended = blendNormal(bottom.rgb, top.rgb);

    // Alpha compositing (src over)
    float outA = top.a + bottom.a * (1.0 - top.a);
    vec3 outRGB = (blended * top.a + bottom.rgb * bottom.a * (1.0 - top.a));
    if (outA > 0.0) outRGB /= outA;

    fragColor = vec4(outRGB, outA);
}
