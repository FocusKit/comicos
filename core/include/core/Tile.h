#pragma once

#include "core/Types.h"
#include <QImage>
#include <memory>
#include <vector>

namespace comicos {

/// A single tile of pixel data (TILE_SIZE x TILE_SIZE, RGBA8).
/// Tiles are the fundamental unit of the canvas. The entire canvas
/// is divided into a grid of tiles to enable efficient rendering,
/// memory management, and undo/redo operations.
class Tile {
public:
    Tile();
    explicit Tile(const TileCoord& coord);
    ~Tile();

    Tile(const Tile& other);
    Tile& operator=(const Tile& other);
    Tile(Tile&& other) noexcept;
    Tile& operator=(Tile&& other) noexcept;

    // --- Accessors ---
    const TileCoord& coord() const { return m_coord; }
    bool isEmpty() const { return !m_data; }
    bool isDirty() const { return m_dirty; }
    void setDirty(bool dirty) { m_dirty = dirty; }

    // --- Pixel Access ---
    /// Ensures pixel data is allocated (lazy allocation for empty tiles).
    void ensureAllocated();

    /// Raw pixel data pointer (may be null if not allocated).
    const uint8_t* constData() const;
    uint8_t* data();

    /// Get/set individual pixel (bounds-checked within tile).
    Pixel pixelAt(int localX, int localY) const;
    void setPixelAt(int localX, int localY, const Pixel& pixel);

    // --- Operations ---
    /// Clear all pixels to transparent.
    void clear();

    /// Create a deep copy of this tile.
    std::unique_ptr<Tile> clone() const;

    /// Convert to QImage for display/export.
    QImage toImage() const;

    // Extension point: tile compression for undo snapshots
    // std::vector<uint8_t> compress() const;
    // static Tile decompress(const TileCoord& coord, const std::vector<uint8_t>& data);

private:
    TileCoord m_coord;
    std::unique_ptr<uint8_t[]> m_data;  // RGBA8, TILE_SIZE*TILE_SIZE*4 bytes
    bool m_dirty = false;
};

}  // namespace comicos
