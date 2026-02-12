#pragma once

#include "core/Tile.h"
#include "core/Types.h"
#include <QImage>
#include <list>
#include <unordered_map>

namespace comicos {

/// GPU-side tile texture cache.
/// Manages upload of tile pixel data to GPU textures for rendering.
/// Uses LRU eviction to limit GPU memory usage.
class TileCache {
public:
    explicit TileCache(size_t maxTextures = 512);
    ~TileCache();

    /// An opaque handle to a cached GPU texture.
    using TextureHandle = uint64_t;
    static constexpr TextureHandle InvalidHandle = 0;

    /// Get or upload a tile to the cache. Returns a texture handle.
    /// The handle is valid until the tile is evicted.
    TextureHandle getOrUpload(const Tile* tile);

    /// Mark a tile as needing re-upload (content changed).
    void invalidate(const TileCoord& coord);

    /// Invalidate all cached textures.
    void invalidateAll();

    /// Number of cached textures.
    size_t size() const { return m_cache.size(); }

    // Extension point: actual GPU texture management via RHI
    // This stub uses texture handles as placeholders.
    // Real implementation will allocate QRhiTexture objects.

private:
    void evictLRU();

    struct CacheEntry {
        TextureHandle handle;
        bool dirty;
    };

    size_t m_maxTextures;
    std::unordered_map<TileCoord, CacheEntry> m_cache;
    std::list<TileCoord> m_lruOrder;  // front = most recently used
    TextureHandle m_nextHandle = 1;
};

}  // namespace comicos
