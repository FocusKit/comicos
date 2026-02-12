#include "render/CanvasItem.h"
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QtMath>

namespace comicos {

CanvasItem::CanvasItem(QQuickItem* parent) : QQuickItem(parent) {
    setFlag(ItemHasContents, true);
    setAcceptedMouseButtons(Qt::AllButtons);
    setAcceptHoverEvents(true);
    setAcceptTouchEvents(true);
}

CanvasItem::~CanvasItem() = default;

// --- Properties ---

float CanvasItem::zoom() const {
    return m_renderer.zoom();
}

void CanvasItem::setZoom(float zoom) {
    if (qFuzzyCompare(m_renderer.zoom(), zoom)) return;
    m_renderer.setZoom(zoom);
    emit zoomChanged();
    update();
}

QPointF CanvasItem::pan() const {
    return m_renderer.pan();
}

void CanvasItem::setPan(const QPointF& pan) {
    if (m_renderer.pan() == pan) return;
    m_renderer.setPan(pan);
    emit panChanged();
    update();
}

float CanvasItem::canvasRotation() const {
    return m_renderer.rotation();
}

void CanvasItem::setCanvasRotation(float degrees) {
    if (qFuzzyCompare(m_renderer.rotation(), degrees)) return;
    m_renderer.setRotation(degrees);
    emit rotationChanged();
    update();
}

void CanvasItem::setDocument(Document* doc) {
    m_document = doc;
    if (m_document) {
        fitCanvasInView();
    }
    invalidateCanvas();
}

void CanvasItem::invalidateCanvas() {
    m_tileRenderer.clearAll();
    update();
}

QPointF CanvasItem::screenToCanvas(const QPointF& screen) const {
    return m_renderer.screenToCanvas(screen);
}

void CanvasItem::invalidateTiles(const std::vector<TileCoord>& tiles) {
    m_tileRenderer.invalidate(tiles);
    update();
}

void CanvasItem::fitCanvasInView() {
    if (!m_document) return;
    float vw = static_cast<float>(width());
    float vh = static_cast<float>(height());
    if (vw <= 0 || vh <= 0) {
        // Viewport not ready yet; use a reasonable default
        vw = 1039; vh = 728;
    }
    float cw = m_document->canvasSize().width();
    float ch = m_document->canvasSize().height();

    // Start at 50% zoom for usable brush visibility
    float fitZoom = std::min(vw / cw, vh / ch);
    fitZoom = std::max(fitZoom, 0.5f);  // at least 50%
    m_renderer.setZoom(fitZoom);

    // Center: pan so canvas center maps to viewport center
    // screen = canvasPoint * zoom + pan + viewport/2
    // For canvas center (cw/2, ch/2) → screen center (vw/2, vh/2):
    //   pan = -(cw/2 * zoom, ch/2 * zoom)
    m_renderer.setPan(QPointF(-(cw / 2.0f) * fitZoom, -(ch / 2.0f) * fitZoom));

    emit zoomChanged();
    emit panChanged();
}

// --- SceneGraph Rendering ---

QSGNode* CanvasItem::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData*) {
    m_renderer.setViewportSize(QSize(
        static_cast<int>(width()), static_cast<int>(height())));

    if (!m_document) {
        m_tileRenderer.clearAll();
        delete oldNode;
        return nullptr;
    }

    // Pass compositor and layer stack to tile renderer
    m_tileRenderer.setLayerStack(&m_document->layers());
    m_tileRenderer.setCompositor(&m_renderer.compositor());

    // Get visible tiles
    auto visibleTiles = m_renderer.visibleTiles(m_document->canvasSize());

    return m_tileRenderer.updateSceneGraph(
        oldNode, window(), visibleTiles,
        m_renderer.viewMatrix(),
        m_document->canvasSize());
}

// --- Input Handling ---

void CanvasItem::mousePressEvent(QMouseEvent* event) {
    // Regain focus when canvas is clicked (toolbar clicks steal it)
    forceActiveFocus();

    if (event->button() == Qt::LeftButton && m_spaceHeld) {
        // Pan mode
        m_isPanning = true;
        setCursor(Qt::ClosedHandCursor);
        m_lastMousePos = event->position();
        event->accept();
        return;
    }

    if (event->button() == Qt::LeftButton) {
        QPointF canvasPos = m_renderer.screenToCanvas(event->position());
        emit strokeStarted(canvasPos, 1.0f);
        m_lastMousePos = event->position();
        event->accept();
    }
}

void CanvasItem::mouseMoveEvent(QMouseEvent* event) {
    if (m_isPanning) {
        QPointF delta = event->position() - m_lastMousePos;
        setPan(pan() + delta);
        m_lastMousePos = event->position();
        event->accept();
        return;
    }

    if (event->buttons() & Qt::LeftButton) {
        QPointF canvasPos = m_renderer.screenToCanvas(event->position());
        emit strokeUpdated(canvasPos, 1.0f);
        m_lastMousePos = event->position();
        event->accept();
    }
}

void CanvasItem::mouseReleaseEvent(QMouseEvent* event) {
    if (m_isPanning) {
        m_isPanning = false;
        // Restore cursor: open hand if space still held, default otherwise
        if (m_spaceHeld) {
            setCursor(Qt::OpenHandCursor);
        } else {
            unsetCursor();
        }
        event->accept();
        return;
    }

    if (event->button() == Qt::LeftButton) {
        emit strokeEnded();
        event->accept();
    }
}

void CanvasItem::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Space && !event->isAutoRepeat()) {
        m_spaceHeld = true;
        if (!m_isPanning) {
            setCursor(Qt::OpenHandCursor);
        }
        event->accept();
        return;
    }
    QQuickItem::keyPressEvent(event);
}

void CanvasItem::keyReleaseEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Space && !event->isAutoRepeat()) {
        m_spaceHeld = false;
        if (!m_isPanning) {
            unsetCursor();
        }
        event->accept();
        return;
    }
    QQuickItem::keyReleaseEvent(event);
}

void CanvasItem::wheelEvent(QWheelEvent* event) {
    float delta = event->angleDelta().y() / 120.0f;
    float factor = qPow(1.15f, delta);
    float newZoom = zoom() * factor;

    // Zoom toward cursor position
    QPointF cursorCanvas = m_renderer.screenToCanvas(event->position());
    setZoom(newZoom);
    QPointF newCursorScreen = m_renderer.canvasToScreen(cursorCanvas);
    QPointF correction = event->position() - newCursorScreen;
    setPan(pan() + correction);

    event->accept();
}

void CanvasItem::geometryChange(const QRectF& newGeometry, const QRectF& oldGeometry) {
    QQuickItem::geometryChange(newGeometry, oldGeometry);
    if (newGeometry.size() != oldGeometry.size()) {
        update();
    }
}

}  // namespace comicos
