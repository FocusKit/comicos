#include "engine/TileCache.h"
#include <algorithm>

namespace comicos {

TileCache::TileCache(size_t maxTextures) : m_maxTextures(maxTextures) {}
TileCache::~TileCache() = default;

TileCache::TextureHandle TileCache::getOrUpload(const Tile* tile) {
    if (!tile) return InvalidHandle;

    auto coord = tile->coord();
    auto it = m_cache.find(coord);

    if (it != m_cache.end()) {
        // Move to front of LRU
        m_lruOrder.remove(coord);
        m_lruOrder.push_front(coord);

        if (it->second.dirty) {
            // Here is where GPU texture re-upload goes
            // Future: QRhiTexture::upload() with tile pixel data
            it->second.dirty = false;
        }
        return it->second.handle;
    }

    // Evict if at capacity
    while (m_cache.size() >= m_maxTextures) {
        evictLRU();
    }

    // Here is where GPU texture creation goes
    // Future: create QRhiTexture, upload pixel data
    TextureHandle handle = m_nextHandle++;
    m_cache[coord] = {handle, false};
    m_lruOrder.push_front(coord);

    return handle;
}

void TileCache::invalidate(const TileCoord& coord) {
    auto it = m_cache.find(coord);
    if (it != m_cache.end()) {
        it->second.dirty = true;
    }
}

void TileCache::invalidateAll() {
    for (auto& [coord, entry] : m_cache) {
        entry.dirty = true;
    }
}

void TileCache::evictLRU() {
    if (m_lruOrder.empty()) return;

    auto coord = m_lruOrder.back();
    m_lruOrder.pop_back();

    // Here is where GPU texture destruction goes
    // Future: destroy QRhiTexture
    m_cache.erase(coord);
}

}  // namespace comicos
