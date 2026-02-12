#pragma once

#include "core/Document.h"
#include "render/CanvasRenderer.h"
#include "render/TileRenderer.h"
#include <QQuickItem>
#include <QSGNode>

namespace comicos {

/// QQuickItem that renders the canvas in QML.
/// This is the bridge between the C++ rendering pipeline and the QML UI.
/// It handles:
/// - SceneGraph rendering via updatePaintNode
/// - Input events (mouse, tablet, touch) for drawing
/// - View transforms (pan, zoom, rotate)
class CanvasItem : public QQuickItem {
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(float zoom READ zoom WRITE setZoom NOTIFY zoomChanged)
    Q_PROPERTY(QPointF pan READ pan WRITE setPan NOTIFY panChanged)
    Q_PROPERTY(float rotation READ canvasRotation WRITE setCanvasRotation NOTIFY rotationChanged)

public:
    explicit CanvasItem(QQuickItem* parent = nullptr);
    ~CanvasItem() override;

    // --- Properties ---
    float zoom() const;
    void setZoom(float zoom);

    QPointF pan() const;
    void setPan(const QPointF& pan);

    float canvasRotation() const;
    void setCanvasRotation(float degrees);

    // --- Document ---
    void setDocument(Document* doc);
    Document* document() const { return m_document; }

    // --- Rendering ---
    void invalidateCanvas();
    void invalidateTiles(const std::vector<TileCoord>& tiles);
    void fitCanvasInView();

    // --- Coordinate conversion ---
    Q_INVOKABLE QPointF screenToCanvas(const QPointF& screen) const;

signals:
    void zoomChanged();
    void panChanged();
    void rotationChanged();

    /// Emitted when a stroke begins.
    void strokeStarted(QPointF canvasPos, float pressure);
    /// Emitted when a stroke point is added.
    void strokeUpdated(QPointF canvasPos, float pressure);
    /// Emitted when a stroke ends.
    void strokeEnded();

protected:
    // SceneGraph rendering (render thread)
    QSGNode* updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData*) override;

    // Input handling
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

    // Extension point: tablet and touch input
    // void tabletEvent(QTabletEvent* event) override;
    // void touchEvent(QTouchEvent* event) override;

    void wheelEvent(QWheelEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
    void geometryChange(const QRectF& newGeometry, const QRectF& oldGeometry) override;

private:
    Document* m_document = nullptr;
    CanvasRenderer m_renderer;
    TileRenderer m_tileRenderer;

    // Interaction state
    bool m_isPanning = false;
    bool m_spaceHeld = false;
    QPointF m_lastMousePos;

    // Platform branching point: input handling differs per platform
    // Windows: WM_POINTER / WinTab for tablet
    // macOS: NSEvent tablet proximity
    // iOS: UITouch with Apple Pencil
};

}  // namespace comicos
