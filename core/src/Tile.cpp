#include "core/Tile.h"
#include <algorithm>
#include <cstring>

namespace comicos {

Tile::Tile() = default;

Tile::Tile(const TileCoord& coord) : m_coord(coord) {}

Tile::~Tile() = default;

Tile::Tile(const Tile& other)
    : m_coord(other.m_coord), m_dirty(other.m_dirty) {
    if (other.m_data) {
        m_data = std::make_unique<uint8_t[]>(TILE_BYTES);
        std::memcpy(m_data.get(), other.m_data.get(), TILE_BYTES);
    }
}

Tile& Tile::operator=(const Tile& other) {
    if (this != &other) {
        m_coord = other.m_coord;
        m_dirty = other.m_dirty;
        if (other.m_data) {
            if (!m_data) m_data = std::make_unique<uint8_t[]>(TILE_BYTES);
            std::memcpy(m_data.get(), other.m_data.get(), TILE_BYTES);
        } else {
            m_data.reset();
        }
    }
    return *this;
}

Tile::Tile(Tile&& other) noexcept = default;
Tile& Tile::operator=(Tile&& other) noexcept = default;

void Tile::ensureAllocated() {
    if (!m_data) {
        m_data = std::make_unique<uint8_t[]>(TILE_BYTES);
        std::memset(m_data.get(), 0, TILE_BYTES);
    }
}

const uint8_t* Tile::constData() const {
    return m_data.get();
}

uint8_t* Tile::data() {
    return m_data.get();
}

Pixel Tile::pixelAt(int localX, int localY) const {
    if (!m_data || localX < 0 || localX >= TILE_SIZE || localY < 0 || localY >= TILE_SIZE) {
        return {};
    }
    int offset = (localY * TILE_SIZE + localX) * 4;
    return {m_data[offset], m_data[offset + 1], m_data[offset + 2], m_data[offset + 3]};
}

void Tile::setPixelAt(int localX, int localY, const Pixel& pixel) {
    if (localX < 0 || localX >= TILE_SIZE || localY < 0 || localY >= TILE_SIZE) return;
    ensureAllocated();
    int offset = (localY * TILE_SIZE + localX) * 4;
    m_data[offset] = pixel.r;
    m_data[offset + 1] = pixel.g;
    m_data[offset + 2] = pixel.b;
    m_data[offset + 3] = pixel.a;
    m_dirty = true;
}

void Tile::clear() {
    if (m_data) {
        std::memset(m_data.get(), 0, TILE_BYTES);
        m_dirty = true;
    }
}

std::unique_ptr<Tile> Tile::clone() const {
    auto copy = std::make_unique<Tile>(*this);
    return copy;
}

QImage Tile::toImage() const {
    if (!m_data) {
        return QImage(TILE_SIZE, TILE_SIZE, QImage::Format_RGBA8888);
    }
    // Create image referencing our data (no copy until modified)
    return QImage(m_data.get(), TILE_SIZE, TILE_SIZE, TILE_SIZE * 4,
                  QImage::Format_RGBA8888)
        .copy();  // Deep copy to decouple from internal buffer
}

}  // namespace comicos
