#pragma once

#include "core/Types.h"
#include <QColor>
#include <vector>

namespace comicos {

/// A single brush "dab" - the atomic unit of brush rendering.
/// A stroke is composed of many dabs placed along the path.
struct BrushDab {
    float x = 0.0f;
    float y = 0.0f;
    float radius = 1.0f;
    float opacity = 1.0f;
    float hardness = 1.0f;
    QColor color = Qt::black;

    // Extension point: brush texture, rotation, aspect ratio
    // float rotation = 0.0f;
    // float aspect = 1.0f;
    // int textureId = -1;
};

/// Generates dab positions along a stroke path with spacing control.
class DabPlacer {
public:
    DabPlacer();
    ~DabPlacer();

    /// Set spacing as fraction of brush diameter (0.1 = 10% spacing).
    void setSpacing(float spacing) { m_spacing = spacing; }
    float spacing() const { return m_spacing; }

    /// Reset for a new stroke.
    void reset();

    /// Generate dabs between two consecutive input points.
    /// Returns the dabs to render.
    std::vector<BrushDab> placeDabs(
        const CanvasPoint& from, const CanvasPoint& to,
        float brushSize, float hardness, const QColor& color);

    // Extension point: pressure curves, tilt mapping
    // void setPressureCurve(const PressureCurve& curve);

private:
    float m_spacing = 0.15f;  // 15% of diameter
    float m_accumDistance = 0.0f;
};

}  // namespace comicos
