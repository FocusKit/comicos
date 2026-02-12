#pragma once

#include "core/Types.h"
#include <QSize>
#include <memory>

namespace comicos {

/// Abstract render backend for platform-specific GPU operations.
/// This provides an abstraction over Qt's RHI so we can potentially
/// support different rendering strategies per platform.
class RenderBackend {
public:
    virtual ~RenderBackend() = default;

    /// Initialize the backend with window/context info.
    virtual bool initialize() = 0;

    /// Create a GPU texture for a tile.
    virtual uint64_t createTileTexture() = 0;

    /// Upload pixel data to a tile texture.
    virtual void uploadTileData(uint64_t textureId, const uint8_t* data) = 0;

    /// Destroy a tile texture.
    virtual void destroyTileTexture(uint64_t textureId) = 0;

    /// Begin a frame.
    virtual void beginFrame() = 0;

    /// Draw a tile at the given position.
    virtual void drawTile(uint64_t textureId, float x, float y, float scale) = 0;

    /// End a frame.
    virtual void endFrame() = 0;

    // Extension point: platform-specific backends
    // - Windows: D3D12 via RHI
    // - macOS/iOS: Metal via RHI
    // - Linux: Vulkan via RHI
    static std::unique_ptr<RenderBackend> create();
};

/// Default RHI-based implementation.
/// Uses Qt's QRhi abstraction to work across all graphics APIs.
class RhiRenderBackend : public RenderBackend {
public:
    RhiRenderBackend();
    ~RhiRenderBackend() override;

    bool initialize() override;
    uint64_t createTileTexture() override;
    void uploadTileData(uint64_t textureId, const uint8_t* data) override;
    void destroyTileTexture(uint64_t textureId) override;
    void beginFrame() override;
    void drawTile(uint64_t textureId, float x, float y, float scale) override;
    void endFrame() override;

private:
    // Extension point: QRhi resources
    // QRhi* m_rhi = nullptr;
    // QRhiRenderPassDescriptor* m_rpDesc = nullptr;
    // QRhiGraphicsPipeline* m_pipeline = nullptr;
    // QRhiShaderResourceBindings* m_srb = nullptr;
    // QRhiBuffer* m_vertexBuffer = nullptr;
    uint64_t m_nextTextureId = 1;
};

}  // namespace comicos
