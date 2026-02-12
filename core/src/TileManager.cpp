#include "core/TileManager.h"
#include <algorithm>
#include <cmath>
#include <limits>

namespace comicos {

TileManager::TileManager() = default;
TileManager::~TileManager() = default;

const Tile* TileManager::tileAt(const TileCoord& coord) const {
    auto it = m_tiles.find(coord);
    return (it != m_tiles.end()) ? it->second.get() : nullptr;
}

Tile* TileManager::getOrCreateTile(const TileCoord& coord) {
    auto& ptr = m_tiles[coord];
    if (!ptr) {
        ptr = std::make_unique<Tile>(coord);
    }
    return ptr.get();
}

bool TileManager::hasTile(const TileCoord& coord) const {
    return m_tiles.count(coord) > 0;
}

void TileManager::removeTile(const TileCoord& coord) {
    m_tiles.erase(coord);
}

std::vector<const Tile*> TileManager::allTiles() const {
    std::vector<const Tile*> result;
    result.reserve(m_tiles.size());
    for (auto& [coord, tile] : m_tiles) {
        result.push_back(tile.get());
    }
    return result;
}

std::vector<Tile*> TileManager::allTilesMut() {
    std::vector<Tile*> result;
    result.reserve(m_tiles.size());
    for (auto& [coord, tile] : m_tiles) {
        result.push_back(tile.get());
    }
    return result;
}

std::vector<Tile*> TileManager::tilesInRect(const QRectF& pixelRect) {
    std::vector<Tile*> result;
    TileCoord topLeft = pixelToTile(
        static_cast<int>(std::floor(pixelRect.left())),
        static_cast<int>(std::floor(pixelRect.top())));
    TileCoord bottomRight = pixelToTile(
        static_cast<int>(std::ceil(pixelRect.right())) - 1,
        static_cast<int>(std::ceil(pixelRect.bottom())) - 1);

    for (int ty = topLeft.ty; ty <= bottomRight.ty; ++ty) {
        for (int tx = topLeft.tx; tx <= bottomRight.tx; ++tx) {
            auto it = m_tiles.find({tx, ty});
            if (it != m_tiles.end()) {
                result.push_back(it->second.get());
            }
        }
    }
    return result;
}

std::vector<Tile*> TileManager::dirtyTiles() {
    std::vector<Tile*> result;
    for (auto& [coord, tile] : m_tiles) {
        if (tile->isDirty()) {
            result.push_back(tile.get());
        }
    }
    return result;
}

void TileManager::clearDirtyFlags() {
    for (auto& [coord, tile] : m_tiles) {
        tile->setDirty(false);
    }
}

void TileManager::clear() {
    m_tiles.clear();
}

QRectF TileManager::boundingRect() const {
    if (m_tiles.empty()) return {};

    int minTx = std::numeric_limits<int>::max();
    int minTy = std::numeric_limits<int>::max();
    int maxTx = std::numeric_limits<int>::min();
    int maxTy = std::numeric_limits<int>::min();

    for (auto& [coord, tile] : m_tiles) {
        minTx = std::min(minTx, coord.tx);
        minTy = std::min(minTy, coord.ty);
        maxTx = std::max(maxTx, coord.tx);
        maxTy = std::max(maxTy, coord.ty);
    }

    return QRectF(
        minTx * TILE_SIZE, minTy * TILE_SIZE,
        (maxTx - minTx + 1) * TILE_SIZE,
        (maxTy - minTy + 1) * TILE_SIZE);
}

std::unordered_map<TileCoord, std::unique_ptr<Tile>> TileManager::snapshotDirtyTiles() {
    std::unordered_map<TileCoord, std::unique_ptr<Tile>> snapshot;
    for (auto& [coord, tile] : m_tiles) {
        if (tile->isDirty()) {
            snapshot[coord] = tile->clone();
        }
    }
    return snapshot;
}

void TileManager::restoreSnapshot(
    const std::unordered_map<TileCoord, std::unique_ptr<Tile>>& snapshot) {
    for (auto& [coord, tile] : snapshot) {
        m_tiles[coord] = tile->clone();
        m_tiles[coord]->setDirty(true);
    }
}

}  // namespace comicos
