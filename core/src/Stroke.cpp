#include "core/Stroke.h"
#include <algorithm>
#include <cmath>
#include <limits>

namespace comicos {

Stroke::Stroke() = default;
Stroke::~Stroke() = default;

void Stroke::addPoint(const CanvasPoint& point) {
    m_points.push_back(point);
}

QRectF Stroke::boundingRect() const {
    if (m_points.empty()) return {};

    float minX = std::numeric_limits<float>::max();
    float minY = std::numeric_limits<float>::max();
    float maxX = std::numeric_limits<float>::lowest();
    float maxY = std::numeric_limits<float>::lowest();

    for (auto& p : m_points) {
        minX = std::min(minX, p.x);
        minY = std::min(minY, p.y);
        maxX = std::max(maxX, p.x);
        maxY = std::max(maxY, p.y);
    }

    // Expand by brush radius
    float r = m_brushSize * 0.5f;
    return QRectF(minX - r, minY - r, (maxX - minX) + m_brushSize, (maxY - minY) + m_brushSize);
}

}  // namespace comicos
