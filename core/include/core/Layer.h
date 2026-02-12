#pragma once

#include "core/TileManager.h"
#include "core/Types.h"
#include <QString>
#include <memory>

namespace comicos {

/// A single layer in the document.
/// Each layer owns a TileManager that stores its pixel content.
class Layer {
public:
    explicit Layer(LayerId id, const QString& name = QString());
    ~Layer();

    // --- Properties ---
    LayerId id() const { return m_id; }

    const QString& name() const { return m_name; }
    void setName(const QString& name) { m_name = name; }

    float opacity() const { return m_opacity; }
    void setOpacity(float opacity) { m_opacity = qBound(0.0f, opacity, 1.0f); }

    bool isVisible() const { return m_visible; }
    void setVisible(bool visible) { m_visible = visible; }

    bool isLocked() const { return m_locked; }
    void setLocked(bool locked) { m_locked = locked; }

    BlendMode blendMode() const { return m_blendMode; }
    void setBlendMode(BlendMode mode) { m_blendMode = mode; }

    // --- Tile Access ---
    TileManager& tiles() { return m_tiles; }
    const TileManager& tiles() const { return m_tiles; }

    // --- Operations ---
    void clear();

    /// Deep clone this layer (new ID).
    std::unique_ptr<Layer> clone(LayerId newId) const;

    // Extension point: layer types (raster, vector, text, adjustment)
    // virtual LayerType type() const { return LayerType::Raster; }

    // Extension point: clipping mask, layer mask
    // Layer* mask() const;
    // void setMask(std::unique_ptr<Layer> mask);

private:
    LayerId m_id;
    QString m_name;
    float m_opacity = 1.0f;
    bool m_visible = true;
    bool m_locked = false;
    BlendMode m_blendMode = BlendMode::Normal;
    TileManager m_tiles;
};

}  // namespace comicos
