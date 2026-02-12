// Canvas tile fragment shader.
// Samples the tile texture and outputs the pixel color.
//
// Extension point: here is where tile rendering effects go.
// - Checkerboard transparency pattern
// - Color management / ICC profiles
// - Soft proofing

#version 440

layout(location = 0) in vec2 v_texCoord;
layout(location = 0) out vec4 fragColor;

layout(binding = 1) uniform sampler2D tileTexture;

void main() {
    fragColor = texture(tileTexture, v_texCoord);
}
