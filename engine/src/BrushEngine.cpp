#include "engine/BrushEngine.h"
#include <algorithm>
#include <cmath>

namespace comicos {

BrushEngine::BrushEngine() = default;
BrushEngine::~BrushEngine() = default;

void BrushEngine::beginStroke(Layer* layer, const Stroke& strokeParams) {
    m_activeLayer = layer;
    m_currentStroke = strokeParams;
    m_dabPlacer.reset();
    m_affectedTiles.clear();
    m_beforeSnapshots.clear();
}

void BrushEngine::addPoint(const CanvasPoint& point) {
    if (!m_activeLayer) return;

    m_currentStroke.addPoint(point);

    // Generate dabs from the last two points
    if (m_currentStroke.pointCount() < 2) {
        // First point: place a single dab
        CanvasPoint p = point;
        auto dabs = m_dabPlacer.placeDabs(p, p,
                                           m_currentStroke.brushSize(),
                                           m_currentStroke.hardness(),
                                           m_currentStroke.color());
        for (auto& dab : dabs) {
            renderDab(dab);
        }
        return;
    }

    auto& points = m_currentStroke.points();
    auto& prev = points[points.size() - 2];
    auto& curr = points[points.size() - 1];

    // Here is where the brush pipeline goes:
    // Future: catmull-rom interpolation, stabilizer, smoothing
    auto dabs = m_dabPlacer.placeDabs(prev, curr,
                                       m_currentStroke.brushSize(),
                                       m_currentStroke.hardness(),
                                       m_currentStroke.color());

    for (auto& dab : dabs) {
        renderDab(dab);
    }
}

std::vector<TileCoord> BrushEngine::endStroke() {
    m_activeLayer = nullptr;
    auto result = std::move(m_affectedTiles);
    m_affectedTiles.clear();
    return result;
}

void BrushEngine::cancelStroke() {
    m_activeLayer = nullptr;
    m_affectedTiles.clear();
    m_beforeSnapshots.clear();
}

std::unordered_map<TileCoord, std::unique_ptr<Tile>> BrushEngine::takeBeforeSnapshots() {
    return std::move(m_beforeSnapshots);
}

void BrushEngine::renderDab(const BrushDab& dab) {
    if (!m_activeLayer) return;

    float r = dab.radius;
    if (r < 0.1f) r = 0.5f;

    int minX = static_cast<int>(std::floor(dab.x - r));
    int minY = static_cast<int>(std::floor(dab.y - r));
    int maxX = static_cast<int>(std::ceil(dab.x + r));
    int maxY = static_cast<int>(std::ceil(dab.y + r));

    // Snapshot tiles that the dab will touch, before any pixel modification
    TileCoord tcMin = pixelToTile(minX, minY);
    TileCoord tcMax = pixelToTile(maxX, maxY);
    for (int ty = tcMin.ty; ty <= tcMax.ty; ++ty) {
        for (int tx = tcMin.tx; tx <= tcMax.tx; ++tx) {
            TileCoord tc{tx, ty};
            if (m_beforeSnapshots.find(tc) == m_beforeSnapshots.end()) {
                const Tile* existing = m_activeLayer->tiles().tileAt(tc);
                if (existing && !existing->isEmpty()) {
                    m_beforeSnapshots[tc] = existing->clone();
                } else {
                    m_beforeSnapshots[tc] = nullptr;  // tile was empty/missing
                }
            }
        }
    }

    for (int py = minY; py <= maxY; ++py) {
        for (int px = minX; px <= maxX; ++px) {
            float dx = px + 0.5f - dab.x;
            float dy = py + 0.5f - dab.y;
            float dist = std::sqrt(dx * dx + dy * dy);

            if (dist > r) continue;

            // Hardness falloff
            float t = dist / r;
            float alpha;
            if (t <= dab.hardness) {
                alpha = 1.0f;
            } else {
                alpha = 1.0f - (t - dab.hardness) / (1.0f - dab.hardness + 0.001f);
            }
            alpha *= dab.opacity;

            TileCoord tc = pixelToTile(px, py);
            Tile* tile = m_activeLayer->tiles().getOrCreateTile(tc);

            int localX = px - tc.tx * TILE_SIZE;
            int localY = py - tc.ty * TILE_SIZE;
            blendPixel(tile, localX, localY, dab.color, alpha);

            // Track affected tiles
            if (std::find(m_affectedTiles.begin(), m_affectedTiles.end(), tc) ==
                m_affectedTiles.end()) {
                m_affectedTiles.push_back(tc);
            }
        }
    }
}

void BrushEngine::blendPixel(Tile* tile, int localX, int localY,
                              const QColor& color, float alpha) {
    tile->ensureAllocated();

    Pixel dst = tile->pixelAt(localX, localY);

    // Eraser: reduce destination alpha
    if (m_currentStroke.toolType() == ToolType::Eraser) {
        float newA = (dst.a / 255.0f) * (1.0f - alpha);
        Pixel result = dst;
        result.a = static_cast<uint8_t>(std::clamp(newA * 255.0f, 0.0f, 255.0f));
        tile->setPixelAt(localX, localY, result);
        return;
    }

    // Source-over alpha compositing
    float sa = alpha;
    float sr = color.redF() * sa;
    float sg = color.greenF() * sa;
    float sb = color.blueF() * sa;

    float da = dst.a / 255.0f;
    float dr = dst.r / 255.0f;
    float dg = dst.g / 255.0f;
    float db = dst.b / 255.0f;

    float outA = sa + da * (1.0f - sa);
    if (outA > 0.0f) {
        float outR = (sr + dr * (1.0f - sa)) / outA;
        float outG = (sg + dg * (1.0f - sa)) / outA;
        float outB = (sb + db * (1.0f - sa)) / outA;

        Pixel result;
        result.r = static_cast<uint8_t>(std::clamp(outR * 255.0f, 0.0f, 255.0f));
        result.g = static_cast<uint8_t>(std::clamp(outG * 255.0f, 0.0f, 255.0f));
        result.b = static_cast<uint8_t>(std::clamp(outB * 255.0f, 0.0f, 255.0f));
        result.a = static_cast<uint8_t>(std::clamp(outA * 255.0f, 0.0f, 255.0f));
        tile->setPixelAt(localX, localY, result);
    }
}

}  // namespace comicos
