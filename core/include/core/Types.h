#pragma once

#include <QColor>
#include <QPointF>
#include <QRectF>
#include <QSize>
#include <cstdint>
#include <vector>

namespace comicos {

// --- Tile Constants ---
constexpr int TILE_SIZE = 256;  // 256x256 pixels per tile (industry standard)
constexpr int TILE_PIXELS = TILE_SIZE * TILE_SIZE;
constexpr int TILE_BYTES = TILE_PIXELS * 4;  // RGBA8

// --- Pixel Format ---
struct Pixel {
    uint8_t r = 0, g = 0, b = 0, a = 0;
};
static_assert(sizeof(Pixel) == 4);

// --- Tile Coordinate ---
struct TileCoord {
    int tx = 0;
    int ty = 0;

    bool operator==(const TileCoord& o) const { return tx == o.tx && ty == o.ty; }
    bool operator<(const TileCoord& o) const {
        return ty < o.ty || (ty == o.ty && tx < o.tx);
    }
};

// --- Canvas Point (with pressure) ---
struct CanvasPoint {
    float x = 0.0f;
    float y = 0.0f;
    float pressure = 1.0f;
    float tiltX = 0.0f;
    float tiltY = 0.0f;
    double timestamp = 0.0;
};

// --- Blend Mode ---
enum class BlendMode : uint8_t {
    Normal,
    Multiply,
    Screen,
    Overlay,
    // TODO: Add more blend modes as needed
};

// --- Tool Type ---
enum class ToolType : uint8_t {
    Pen,
    Eraser,
    Fill,
    Select,
    Move,
    // Extension point: add new tools here
};

// --- Layer ID ---
using LayerId = uint64_t;

// --- Utility ---
inline TileCoord pixelToTile(int px, int py) {
    // Floor division for negative coordinates
    int tx = (px >= 0) ? px / TILE_SIZE : (px - TILE_SIZE + 1) / TILE_SIZE;
    int ty = (py >= 0) ? py / TILE_SIZE : (py - TILE_SIZE + 1) / TILE_SIZE;
    return {tx, ty};
}

inline QPointF tileToPixel(const TileCoord& tc) {
    return {static_cast<qreal>(tc.tx * TILE_SIZE),
            static_cast<qreal>(tc.ty * TILE_SIZE)};
}

}  // namespace comicos

// Hash for TileCoord (for unordered containers)
template <>
struct std::hash<comicos::TileCoord> {
    size_t operator()(const comicos::TileCoord& c) const noexcept {
        return std::hash<int64_t>{}(
            (static_cast<int64_t>(c.tx) << 32) | static_cast<int64_t>(c.ty));
    }
};
