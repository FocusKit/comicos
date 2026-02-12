#pragma once

#include "core/Layer.h"
#include "core/Types.h"
#include <memory>
#include <vector>

namespace comicos {

/// Ordered collection of layers that compose the final image.
/// Bottom layer is index 0, top layer is last.
class LayerStack {
public:
    LayerStack();
    ~LayerStack();

    // --- Layer Management ---
    /// Add a new empty layer on top. Returns the new layer.
    Layer* addLayer(const QString& name = QString());

    /// Insert a layer at the given index.
    Layer* insertLayer(int index, std::unique_ptr<Layer> layer);

    /// Remove layer by ID. Returns the removed layer (for undo).
    std::unique_ptr<Layer> removeLayer(LayerId id);

    /// Move layer from one index to another.
    void moveLayer(int fromIndex, int toIndex);

    /// Duplicate a layer.
    Layer* duplicateLayer(LayerId id);

    // --- Access ---
    Layer* layerById(LayerId id);
    const Layer* layerById(LayerId id) const;

    Layer* layerAt(int index);
    const Layer* layerAt(int index) const;

    int indexOf(LayerId id) const;
    int count() const { return static_cast<int>(m_layers.size()); }
    bool isEmpty() const { return m_layers.empty(); }

    // --- Iteration ---
    /// Bottom-to-top order (rendering order).
    const std::vector<std::unique_ptr<Layer>>& layers() const { return m_layers; }

    // --- Active Layer ---
    LayerId activeLayerId() const { return m_activeLayerId; }
    void setActiveLayerId(LayerId id) { m_activeLayerId = id; }
    Layer* activeLayer();

    // Extension point: layer groups / folders
    // Layer* createGroup(const QString& name);

private:
    LayerId nextId();

    std::vector<std::unique_ptr<Layer>> m_layers;
    LayerId m_activeLayerId = 0;
    LayerId m_nextId = 1;
};

}  // namespace comicos
