#include "engine/BrushDab.h"
#include <cmath>

namespace comicos {

DabPlacer::DabPlacer() = default;
DabPlacer::~DabPlacer() = default;

void DabPlacer::reset() {
    m_accumDistance = 0.0f;
}

std::vector<BrushDab> DabPlacer::placeDabs(
    const CanvasPoint& from, const CanvasPoint& to,
    float brushSize, float hardness, const QColor& color) {

    std::vector<BrushDab> dabs;
    float radius = brushSize * 0.5f;
    float step = std::max(brushSize * m_spacing, 0.5f);

    float dx = to.x - from.x;
    float dy = to.y - from.y;
    float dist = std::sqrt(dx * dx + dy * dy);

    if (dist < 0.001f) {
        // Single dab at current position
        if (m_accumDistance <= 0.0f) {
            BrushDab dab;
            dab.x = to.x;
            dab.y = to.y;
            dab.radius = radius * to.pressure;
            dab.opacity = to.pressure;
            dab.hardness = hardness;
            dab.color = color;
            dabs.push_back(dab);
            m_accumDistance = step;
        }
        return dabs;
    }

    float remaining = step - m_accumDistance;
    float t = remaining / dist;

    while (t <= 1.0f) {
        float x = from.x + dx * t;
        float y = from.y + dy * t;
        float pressure = from.pressure + (to.pressure - from.pressure) * t;

        BrushDab dab;
        dab.x = x;
        dab.y = y;
        dab.radius = radius * pressure;
        dab.opacity = pressure;
        dab.hardness = hardness;
        dab.color = color;
        dabs.push_back(dab);

        t += step / dist;
    }

    m_accumDistance = dist * (1.0f - (t - step / dist));
    return dabs;
}

}  // namespace comicos
