#pragma once

#include "core/Tile.h"
#include "core/Types.h"
#include <QRectF>
#include <memory>
#include <unordered_map>
#include <vector>

namespace comicos {

/// Manages a sparse grid of tiles for a single layer.
/// Only allocates tiles where actual content exists (sparse storage).
/// This is the key data structure for large canvas support.
class TileManager {
public:
    TileManager();
    ~TileManager();

    // --- Tile Access ---
    /// Get tile at coordinate (returns nullptr if not allocated).
    const Tile* tileAt(const TileCoord& coord) const;

    /// Get or create tile at coordinate (allocates on demand).
    Tile* getOrCreateTile(const TileCoord& coord);

    /// Check if tile exists at coordinate.
    bool hasTile(const TileCoord& coord) const;

    /// Remove tile (free memory for empty tiles).
    void removeTile(const TileCoord& coord);

    // --- Iteration ---
    /// All allocated tiles.
    std::vector<const Tile*> allTiles() const;
    std::vector<Tile*> allTilesMut();

    /// Tiles that intersect a given pixel rect.
    std::vector<Tile*> tilesInRect(const QRectF& pixelRect);

    /// Tiles marked as dirty.
    std::vector<Tile*> dirtyTiles();

    /// Clear all dirty flags.
    void clearDirtyFlags();

    // --- Bulk Operations ---
    /// Clear all tiles.
    void clear();

    /// Number of allocated tiles.
    size_t tileCount() const { return m_tiles.size(); }

    /// Bounding rect of all allocated tiles (in pixel coordinates).
    QRectF boundingRect() const;

    // --- Snapshot for Undo ---
    /// Take a snapshot of dirty tiles (for undo).
    /// Returns a map of coord -> tile data copy.
    std::unordered_map<TileCoord, std::unique_ptr<Tile>> snapshotDirtyTiles();

    /// Restore tiles from a snapshot.
    void restoreSnapshot(const std::unordered_map<TileCoord, std::unique_ptr<Tile>>& snapshot);

    // Extension point: tile streaming / disk cache for very large canvases
    // void enableDiskCache(const QString& cachePath);

private:
    std::unordered_map<TileCoord, std::unique_ptr<Tile>> m_tiles;
};

}  // namespace comicos
