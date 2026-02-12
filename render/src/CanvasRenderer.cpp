#include "render/CanvasRenderer.h"
#include <QtMath>

namespace comicos {

CanvasRenderer::CanvasRenderer() = default;
CanvasRenderer::~CanvasRenderer() = default;

void CanvasRenderer::setViewportSize(const QSize& size) {
    m_viewportSize = size;
}

void CanvasRenderer::setZoom(float zoom) {
    m_zoom = qBound(0.05f, zoom, 32.0f);
}

void CanvasRenderer::setPan(const QPointF& offset) {
    m_pan = offset;
}

void CanvasRenderer::setRotation(float degrees) {
    m_rotation = degrees;
}

QMatrix4x4 CanvasRenderer::viewMatrix() const {
    QMatrix4x4 mat;
    // Center the viewport
    mat.translate(m_viewportSize.width() * 0.5f, m_viewportSize.height() * 0.5f, 0);
    // Apply pan
    mat.translate(m_pan.x(), m_pan.y(), 0);
    // Apply zoom
    mat.scale(m_zoom, m_zoom, 1.0f);
    // Apply rotation
    mat.rotate(m_rotation, 0, 0, 1);
    return mat;
}

QPointF CanvasRenderer::screenToCanvas(const QPointF& screen) const {
    QMatrix4x4 inv = viewMatrix().inverted();
    QVector4D v = inv * QVector4D(screen.x(), screen.y(), 0, 1);
    return {v.x(), v.y()};
}

QPointF CanvasRenderer::canvasToScreen(const QPointF& canvas) const {
    QMatrix4x4 mat = viewMatrix();
    QVector4D v = mat * QVector4D(canvas.x(), canvas.y(), 0, 1);
    return {v.x(), v.y()};
}

std::vector<TileCoord> CanvasRenderer::visibleTiles(const QSize& canvasSize) const {
    std::vector<TileCoord> tiles;

    // Convert viewport corners to canvas space
    QPointF topLeft = screenToCanvas({0, 0});
    QPointF bottomRight = screenToCanvas(
        {static_cast<qreal>(m_viewportSize.width()),
         static_cast<qreal>(m_viewportSize.height())});

    // Clamp to canvas bounds
    int minTx = qMax(0, static_cast<int>(qFloor(topLeft.x())) / TILE_SIZE);
    int minTy = qMax(0, static_cast<int>(qFloor(topLeft.y())) / TILE_SIZE);
    int maxTx = qMin(
        (canvasSize.width() + TILE_SIZE - 1) / TILE_SIZE,
        static_cast<int>(qCeil(bottomRight.x())) / TILE_SIZE + 1);
    int maxTy = qMin(
        (canvasSize.height() + TILE_SIZE - 1) / TILE_SIZE,
        static_cast<int>(qCeil(bottomRight.y())) / TILE_SIZE + 1);

    for (int ty = minTy; ty < maxTy; ++ty) {
        for (int tx = minTx; tx < maxTx; ++tx) {
            tiles.push_back({tx, ty});
        }
    }

    return tiles;
}

void CanvasRenderer::invalidateTiles(const std::vector<TileCoord>& tiles) {
    for (auto& coord : tiles) {
        m_tileCache.invalidate(coord);
    }
}

}  // namespace comicos
