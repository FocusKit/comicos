#pragma once

#include "core/Types.h"
#include <QColor>
#include <vector>

namespace comicos {

/// Represents a single brush stroke as a sequence of input points.
/// Used both for real-time rendering and for undo/redo.
class Stroke {
public:
    Stroke();
    ~Stroke();

    // --- Configuration ---
    ToolType toolType() const { return m_toolType; }
    void setToolType(ToolType type) { m_toolType = type; }

    QColor color() const { return m_color; }
    void setColor(const QColor& color) { m_color = color; }

    float brushSize() const { return m_brushSize; }
    void setBrushSize(float size) { m_brushSize = size; }

    float hardness() const { return m_hardness; }
    void setHardness(float hardness) { m_hardness = hardness; }

    LayerId targetLayerId() const { return m_targetLayerId; }
    void setTargetLayerId(LayerId id) { m_targetLayerId = id; }

    // --- Point Data ---
    void addPoint(const CanvasPoint& point);
    const std::vector<CanvasPoint>& points() const { return m_points; }
    int pointCount() const { return static_cast<int>(m_points.size()); }

    /// Bounding rect of the stroke (in canvas pixel coordinates).
    QRectF boundingRect() const;

    // Extension point: brush texture, spacing, dynamics
    // BrushPreset* brushPreset() const;
    // void setBrushPreset(BrushPreset* preset);

private:
    ToolType m_toolType = ToolType::Pen;
    QColor m_color = Qt::black;
    float m_brushSize = 3.0f;
    float m_hardness = 1.0f;
    LayerId m_targetLayerId = 0;
    std::vector<CanvasPoint> m_points;
};

}  // namespace comicos
