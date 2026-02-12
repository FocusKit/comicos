#include "render/RenderBackend.h"

namespace comicos {

std::unique_ptr<RenderBackend> RenderBackend::create() {
    return std::make_unique<RhiRenderBackend>();
}

// --- RhiRenderBackend ---

RhiRenderBackend::RhiRenderBackend() = default;
RhiRenderBackend::~RhiRenderBackend() = default;

bool RhiRenderBackend::initialize() {
    // Extension point: initialize QRhi
    // Platform branching:
    // #if defined(COMICOS_PLATFORM_windows)
    //     m_rhi = QRhi::create(QRhi::D3D12, ...);
    // #elif defined(COMICOS_PLATFORM_macos) || defined(COMICOS_PLATFORM_ios)
    //     m_rhi = QRhi::create(QRhi::Metal, ...);
    // #else
    //     m_rhi = QRhi::create(QRhi::Vulkan, ...);
    // #endif
    return true;
}

uint64_t RhiRenderBackend::createTileTexture() {
    // Extension point: create QRhiTexture(RGBA8, TILE_SIZE, TILE_SIZE)
    return m_nextTextureId++;
}

void RhiRenderBackend::uploadTileData(uint64_t /*textureId*/, const uint8_t* /*data*/) {
    // Extension point: QRhiResourceUpdateBatch -> uploadTexture
}

void RhiRenderBackend::destroyTileTexture(uint64_t /*textureId*/) {
    // Extension point: destroy QRhiTexture
}

void RhiRenderBackend::beginFrame() {
    // Extension point: QRhi::beginFrame
}

void RhiRenderBackend::drawTile(uint64_t /*textureId*/, float /*x*/, float /*y*/, float /*scale*/) {
    // Extension point: set uniforms, bind texture, draw quad
}

void RhiRenderBackend::endFrame() {
    // Extension point: QRhi::endFrame
}

}  // namespace comicos
