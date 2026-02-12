#pragma once

#include "core/Layer.h"
#include "core/Stroke.h"
#include "core/Types.h"
#include "engine/BrushDab.h"
#include <QColor>
#include <memory>
#include <unordered_map>

namespace comicos {

/// Core brush rendering engine.
/// Converts input strokes into pixel modifications on tile data.
/// This is the performance-critical hot path of the application.
class BrushEngine {
public:
    BrushEngine();
    ~BrushEngine();

    // --- Stroke Lifecycle ---
    /// Begin a new stroke on the given layer.
    void beginStroke(Layer* layer, const Stroke& strokeParams);

    /// Add a point to the current stroke (called per tablet/mouse input).
    void addPoint(const CanvasPoint& point);

    /// End the current stroke. Returns affected tile coordinates.
    std::vector<TileCoord> endStroke();

    /// Cancel the current stroke (discard).
    void cancelStroke();

    bool isActive() const { return m_activeLayer != nullptr; }

    /// Take ownership of before-snapshots captured during the stroke.
    /// Call after endStroke() to get tile data for undo.
    std::unordered_map<TileCoord, std::unique_ptr<Tile>> takeBeforeSnapshots();

    // --- Dab Rendering ---
    // Extension point: here is where the brush pipeline goes
    // Currently renders simple circular dabs.
    // Future: texture stamps, scatter, dynamics, wet brushes.

private:
    /// Render a single dab onto the layer's tiles.
    void renderDab(const BrushDab& dab);

    /// Blend a single pixel (src over dst).
    void blendPixel(Tile* tile, int localX, int localY,
                    const QColor& color, float alpha);

    Layer* m_activeLayer = nullptr;
    Stroke m_currentStroke;
    DabPlacer m_dabPlacer;
    std::vector<TileCoord> m_affectedTiles;
    std::unordered_map<TileCoord, std::unique_ptr<Tile>> m_beforeSnapshots;
};

}  // namespace comicos
