#pragma once

#include "core/Document.h"
#include "core/Types.h"
#include "engine/Compositor.h"
#include "engine/TileCache.h"
#include <QMatrix4x4>
#include <QPointF>
#include <QRectF>
#include <QSize>

namespace comicos {

/// High-level canvas renderer.
/// Manages the view transform (pan/zoom/rotate) and orchestrates
/// tile rendering for the viewport.
class CanvasRenderer {
public:
    CanvasRenderer();
    ~CanvasRenderer();

    // --- View Transform ---
    void setViewportSize(const QSize& size);
    QSize viewportSize() const { return m_viewportSize; }

    void setZoom(float zoom);
    float zoom() const { return m_zoom; }

    void setPan(const QPointF& offset);
    QPointF pan() const { return m_pan; }

    void setRotation(float degrees);
    float rotation() const { return m_rotation; }

    /// Full view matrix (pan + zoom + rotation).
    QMatrix4x4 viewMatrix() const;

    /// Convert screen coords to canvas coords.
    QPointF screenToCanvas(const QPointF& screen) const;

    /// Convert canvas coords to screen coords.
    QPointF canvasToScreen(const QPointF& canvas) const;

    // --- Rendering ---
    /// Determine which tiles are visible in the current viewport.
    std::vector<TileCoord> visibleTiles(const QSize& canvasSize) const;

    /// Mark tiles as needing re-render.
    void invalidateTiles(const std::vector<TileCoord>& tiles);

    // --- Components ---
    TileCache& tileCache() { return m_tileCache; }
    Compositor& compositor() { return m_compositor; }

    // Extension point: here is where tile rendering orchestration goes
    // Future: frustum culling, LOD for zoomed-out view, render thread

private:
    QSize m_viewportSize;
    float m_zoom = 1.0f;
    QPointF m_pan{0, 0};
    float m_rotation = 0.0f;

    TileCache m_tileCache;
    Compositor m_compositor;
};

}  // namespace comicos
