#include "bridge/AppController.h"
#include <QGuiApplication>
#include <QStyleHints>

namespace comicos {

// --- StrokeCommand ---

StrokeCommand::StrokeCommand(
    LayerStack* layers, LayerId layerId,
    std::vector<TileCoord> affectedTiles,
    std::unordered_map<TileCoord, std::unique_ptr<Tile>> before)
    : m_layers(layers)
    , m_layerId(layerId)
    , m_coords(std::move(affectedTiles))
    , m_before(std::move(before))
    , m_firstRedo(true)
{
    // Capture "after" state — the stroke is already applied by BrushEngine
    Layer* layer = m_layers->layerById(m_layerId);
    if (layer) {
        for (const auto& tc : m_coords) {
            const Tile* tile = layer->tiles().tileAt(tc);
            if (tile && !tile->isEmpty()) {
                m_after[tc] = tile->clone();
            }
        }
    }
}

void StrokeCommand::undo() {
    Layer* layer = m_layers->layerById(m_layerId);
    if (!layer) return;  // Layer was deleted — nothing to undo

    for (const auto& tc : m_coords) {
        auto it = m_before.find(tc);
        if (it != m_before.end() && it->second) {
            Tile* tile = layer->tiles().getOrCreateTile(tc);
            *tile = *(it->second);
        } else {
            layer->tiles().removeTile(tc);
        }
    }
}

void StrokeCommand::redo() {
    if (m_firstRedo) {
        m_firstRedo = false;
        return;
    }

    Layer* layer = m_layers->layerById(m_layerId);
    if (!layer) return;  // Layer was deleted — nothing to redo

    for (const auto& tc : m_coords) {
        auto it = m_after.find(tc);
        if (it != m_after.end() && it->second) {
            Tile* tile = layer->tiles().getOrCreateTile(tc);
            *tile = *(it->second);
        } else {
            layer->tiles().removeTile(tc);
        }
    }
}

size_t StrokeCommand::memoryUsage() const {
    // Approximate: each tile snapshot is TILE_BYTES, we have before + after
    size_t count = 0;
    for (const auto& [tc, tile] : m_before) {
        if (tile) ++count;
    }
    for (const auto& [tc, tile] : m_after) {
        if (tile) ++count;
    }
    return count * TILE_BYTES;
}

// --- AppController ---

AppController::AppController(QObject* parent) : QObject(parent) {
    m_document = std::make_unique<Document>();

    // Create layer model and bind to document
    m_layerModel = new DocumentModel(this);
    m_layerModel->setDocument(m_document.get());

    // When layer visuals change, repaint canvas
    connect(m_layerModel, &DocumentModel::layerVisualChanged, this, [this]() {
        if (m_canvasItem) {
            m_canvasItem->invalidateCanvas();
        }
        m_document->setDirty(true);
        emit canvasNeedsUpdate();
        emit dirtyChanged();
    });

    // Detect system theme
    auto* hints = QGuiApplication::styleHints();
    if (hints) {
        connect(hints, &QStyleHints::colorSchemeChanged, this, [this]() {
            if (m_theme == "system") {
                emit themeChanged();
            }
        });
    }
}

AppController::~AppController() = default;

// --- Tool Getters ---

int AppController::currentTool() const {
    return static_cast<int>(m_currentTool);
}

QColor AppController::currentColor() const {
    return m_currentColor;
}

qreal AppController::brushSize() const {
    return m_brushSize;
}

qreal AppController::brushHardness() const {
    return m_brushHardness;
}

// --- Tool Setters ---

void AppController::setCurrentTool(int tool) {
    auto t = static_cast<ToolType>(tool);
    if (m_currentTool == t) return;
    m_currentTool = t;
    emit currentToolChanged();
}

void AppController::setCurrentColor(const QColor& color) {
    if (m_currentColor == color) return;
    m_currentColor = color;
    emit currentColorChanged();
}

void AppController::setBrushSize(qreal size) {
    size = qBound(1.0, size, 500.0);
    if (qFuzzyCompare(m_brushSize, size)) return;
    m_brushSize = size;
    emit brushSizeChanged();
}

void AppController::setBrushHardness(qreal hardness) {
    hardness = qBound(0.0, hardness, 1.0);
    if (qFuzzyCompare(m_brushHardness, hardness)) return;
    m_brushHardness = hardness;
    emit brushHardnessChanged();
}

// --- Theme ---

QString AppController::theme() const {
    return m_theme;
}

void AppController::setTheme(const QString& theme) {
    if (m_theme == theme) return;
    m_theme = theme;
    emit themeChanged();
}

bool AppController::isDarkTheme() const {
    if (m_theme == "dark") return true;
    if (m_theme == "light") return false;
    // System detection
    auto* hints = QGuiApplication::styleHints();
    return hints && hints->colorScheme() == Qt::ColorScheme::Dark;
}

// --- Layer Model ---

DocumentModel* AppController::layerModel() const {
    return m_layerModel;
}

// --- Document ---

bool AppController::canUndo() const {
    return m_document && m_document->history().canUndo();
}

bool AppController::canRedo() const {
    return m_document && m_document->history().canRedo();
}

bool AppController::isDirty() const {
    return m_document && m_document->isDirty();
}

QString AppController::filePath() const {
    return m_document ? m_document->filePath() : QString();
}

// --- Actions ---

void AppController::newDocument(int width, int height, int dpi) {
    // Cancel any in-progress stroke before replacing document
    if (m_brushEngine.isActive()) {
        m_brushEngine.cancelStroke();
        m_strokeLayer = nullptr;
    }

    m_document = std::make_unique<Document>(QSize(width, height));
    m_document->setDpi(dpi);

    m_layerModel->setDocument(m_document.get());

    if (m_canvasItem) {
        m_canvasItem->setDocument(m_document.get());
    }

    emit historyChanged();
    emit dirtyChanged();
    emit filePathChanged();
}

void AppController::undo() {
    if (!m_document) return;

    // Cancel any in-progress stroke before undo
    if (m_brushEngine.isActive()) {
        m_brushEngine.cancelStroke();
        m_strokeLayer = nullptr;
    }

    m_document->history().undo();
    emit historyChanged();

    if (m_canvasItem) {
        m_canvasItem->invalidateCanvas();
    }
}

void AppController::redo() {
    if (!m_document) return;

    // Cancel any in-progress stroke before redo
    if (m_brushEngine.isActive()) {
        m_brushEngine.cancelStroke();
        m_strokeLayer = nullptr;
    }

    m_document->history().redo();
    emit historyChanged();

    if (m_canvasItem) {
        m_canvasItem->invalidateCanvas();
    }
}

void AppController::setCanvasItem(CanvasItem* item) {
    m_canvasItem = item;
    if (m_canvasItem && m_document) {
        m_canvasItem->setDocument(m_document.get());
    }
}

// --- Stroke Handling ---

void AppController::onStrokeStarted(QPointF canvasPos, float pressure) {
    if (!m_document) return;

    // Only Pen and Eraser use the brush engine
    if (m_currentTool != ToolType::Pen && m_currentTool != ToolType::Eraser) return;

    Layer* layer = m_document->layers().activeLayer();
    if (!layer || layer->isLocked()) return;

    m_strokeLayer = layer;

    Stroke stroke;
    stroke.setToolType(m_currentTool);
    stroke.setColor(m_currentColor);
    stroke.setBrushSize(m_brushSize);
    stroke.setHardness(m_brushHardness);
    stroke.setTargetLayerId(layer->id());

    m_brushEngine.beginStroke(layer, stroke);

    CanvasPoint point;
    point.x = canvasPos.x();
    point.y = canvasPos.y();
    point.pressure = pressure;
    m_brushEngine.addPoint(point);

    emit canvasNeedsUpdate();
}

void AppController::onStrokeUpdated(QPointF canvasPos, float pressure) {
    if (!m_brushEngine.isActive()) return;

    CanvasPoint point;
    point.x = canvasPos.x();
    point.y = canvasPos.y();
    point.pressure = pressure;
    m_brushEngine.addPoint(point);

    emit canvasNeedsUpdate();
}

void AppController::onStrokeEnded() {
    if (!m_brushEngine.isActive()) return;

    auto affectedTiles = m_brushEngine.endStroke();
    auto beforeSnapshots = m_brushEngine.takeBeforeSnapshots();

    if (m_strokeLayer && !affectedTiles.empty()) {
        auto cmd = std::make_unique<StrokeCommand>(
            &m_document->layers(), m_strokeLayer->id(),
            affectedTiles, std::move(beforeSnapshots));
        m_document->history().push(std::move(cmd));
    }

    m_strokeLayer = nullptr;
    m_document->setDirty(true);
    emit historyChanged();
    emit dirtyChanged();
    emit canvasNeedsUpdate();
}

}  // namespace comicos
