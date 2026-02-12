// Canvas tile vertex shader.
// Transforms tile quads from canvas space to screen space
// using the view matrix (pan/zoom/rotation).
//
// Extension point: here is where tile rendering vertex transform goes.
// This shader will be compiled to all platforms via Qt Shader Tools.

#version 440

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texCoord;

layout(location = 0) out vec2 v_texCoord;

layout(std140, binding = 0) uniform buf {
    mat4 mvp;           // Model-View-Projection matrix
    vec2 tileOffset;    // Tile position in canvas space
    float tileSize;     // TILE_SIZE (256.0)
    float padding;
};

void main() {
    v_texCoord = texCoord;
    vec4 worldPos = vec4(position.xy * tileSize + tileOffset, 0.0, 1.0);
    gl_Position = mvp * worldPos;
}
