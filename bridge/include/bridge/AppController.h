#pragma once

#include "core/Document.h"
#include "core/History.h"
#include "core/Types.h"
#include "engine/BrushEngine.h"
#include "render/CanvasItem.h"
#include <QObject>
#include <QQmlEngine>

namespace comicos {

/// Undoable command for a completed brush stroke.
/// Stores before/after tile snapshots for the affected tiles.
class StrokeCommand : public HistoryCommand {
public:
    StrokeCommand(Layer* layer,
                  std::vector<TileCoord> affectedTiles,
                  std::unordered_map<TileCoord, std::unique_ptr<Tile>> before);

    void undo() override;
    void redo() override;
    size_t memoryUsage() const override;

private:
    Layer* m_layer;
    std::vector<TileCoord> m_coords;
    std::unordered_map<TileCoord, std::unique_ptr<Tile>> m_before;
    std::unordered_map<TileCoord, std::unique_ptr<Tile>> m_after;
    bool m_firstRedo = true;
};

/// Main application controller exposed to QML.
/// Bridges QML UI actions to C++ core logic.
class AppController : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    // --- Tool State ---
    Q_PROPERTY(int currentTool READ currentTool WRITE setCurrentTool NOTIFY currentToolChanged)
    Q_PROPERTY(QColor currentColor READ currentColor WRITE setCurrentColor NOTIFY currentColorChanged)
    Q_PROPERTY(qreal brushSize READ brushSize WRITE setBrushSize NOTIFY brushSizeChanged)
    Q_PROPERTY(qreal brushHardness READ brushHardness WRITE setBrushHardness NOTIFY brushHardnessChanged)

    // --- Theme ---
    Q_PROPERTY(QString theme READ theme WRITE setTheme NOTIFY themeChanged)
    Q_PROPERTY(bool isDarkTheme READ isDarkTheme NOTIFY themeChanged)

    // --- Document State ---
    Q_PROPERTY(bool canUndo READ canUndo NOTIFY historyChanged)
    Q_PROPERTY(bool canRedo READ canRedo NOTIFY historyChanged)
    Q_PROPERTY(bool isDirty READ isDirty NOTIFY dirtyChanged)
    Q_PROPERTY(QString filePath READ filePath NOTIFY filePathChanged)

public:
    explicit AppController(QObject* parent = nullptr);
    ~AppController() override;

    // --- Tool Getters ---
    int currentTool() const;
    QColor currentColor() const;
    qreal brushSize() const;
    qreal brushHardness() const;

    // --- Tool Setters ---
    void setCurrentTool(int tool);
    void setCurrentColor(const QColor& color);
    void setBrushSize(qreal size);
    void setBrushHardness(qreal hardness);

    // --- Theme ---
    QString theme() const;
    void setTheme(const QString& theme);
    bool isDarkTheme() const;

    // --- Document ---
    bool canUndo() const;
    bool canRedo() const;
    bool isDirty() const;
    QString filePath() const;

    // --- Actions (invocable from QML) ---
    Q_INVOKABLE void newDocument(int width, int height, int dpi = 300);
    Q_INVOKABLE void undo();
    Q_INVOKABLE void redo();

    // --- Canvas Integration ---
    Q_INVOKABLE void setCanvasItem(CanvasItem* item);
    Document* document() { return m_document.get(); }

    // --- Stroke Handling ---
    Q_INVOKABLE void onStrokeStarted(QPointF canvasPos, float pressure);
    Q_INVOKABLE void onStrokeUpdated(QPointF canvasPos, float pressure);
    Q_INVOKABLE void onStrokeEnded();

signals:
    void currentToolChanged();
    void currentColorChanged();
    void brushSizeChanged();
    void brushHardnessChanged();
    void themeChanged();
    void historyChanged();
    void dirtyChanged();
    void filePathChanged();
    void canvasNeedsUpdate();

private:
    std::unique_ptr<Document> m_document;
    BrushEngine m_brushEngine;
    CanvasItem* m_canvasItem = nullptr;

    ToolType m_currentTool = ToolType::Pen;
    QColor m_currentColor = Qt::black;
    qreal m_brushSize = 10.0;
    qreal m_brushHardness = 1.0;
    QString m_theme = QStringLiteral("system");

    Layer* m_strokeLayer = nullptr;
};

}  // namespace comicos
