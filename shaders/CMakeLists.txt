# Shader compilation via Qt Shader Tools.
# .qsb files are generated at build time from GLSL sources.
# Qt's shader pipeline compiles GLSL -> SPIR-V -> platform shaders:
#   - HLSL (Windows/D3D12)
#   - Metal Shading Language (macOS/iOS)
#   - GLSL ES (fallback)
#   - SPIR-V (Vulkan)

# Extension point: add shaders as they are implemented
# qt_add_shaders(comicos "comicos_shaders"
#     BATCHABLE
#     PRECOMPILE
#     OPTIMIZED
#     PREFIX "/shaders"
#     FILES
#         canvas.vert
#         canvas.frag
#         brush.frag
#         composite.frag
# )
