#include "core/Layer.h"

namespace comicos {

Layer::Layer(LayerId id, const QString& name)
    : m_id(id), m_name(name.isEmpty() ? QStringLiteral("레이어 %1").arg(id) : name) {}

Layer::~Layer() = default;

void Layer::clear() {
    m_tiles.clear();
}

std::unique_ptr<Layer> Layer::clone(LayerId newId) const {
    auto copy = std::make_unique<Layer>(newId, m_name + " (복사)");
    copy->m_opacity = m_opacity;
    copy->m_visible = m_visible;
    copy->m_locked = m_locked;
    copy->m_blendMode = m_blendMode;

    // Deep copy tile data
    for (auto* tile : m_tiles.allTiles()) {
        if (!tile->isEmpty()) {
            auto* newTile = copy->m_tiles.getOrCreateTile(tile->coord());
            *newTile = *tile;
        }
    }
    return copy;
}

}  // namespace comicos
