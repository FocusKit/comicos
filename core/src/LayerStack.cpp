#include "core/LayerStack.h"
#include <algorithm>

namespace comicos {

LayerStack::LayerStack() = default;
LayerStack::~LayerStack() = default;

Layer* LayerStack::addLayer(const QString& name) {
    auto id = nextId();
    auto layer = std::make_unique<Layer>(id, name);
    auto* ptr = layer.get();
    m_layers.push_back(std::move(layer));
    m_activeLayerId = id;
    return ptr;
}

Layer* LayerStack::insertLayer(int index, std::unique_ptr<Layer> layer) {
    index = qBound(0, index, static_cast<int>(m_layers.size()));
    auto* ptr = layer.get();
    m_layers.insert(m_layers.begin() + index, std::move(layer));
    return ptr;
}

std::unique_ptr<Layer> LayerStack::removeLayer(LayerId id) {
    auto it = std::find_if(m_layers.begin(), m_layers.end(),
                           [id](const auto& l) { return l->id() == id; });
    if (it == m_layers.end()) return nullptr;

    auto removed = std::move(*it);
    m_layers.erase(it);

    // Update active layer if we removed it
    if (m_activeLayerId == id && !m_layers.empty()) {
        m_activeLayerId = m_layers.back()->id();
    }
    return removed;
}

void LayerStack::moveLayer(int fromIndex, int toIndex) {
    if (fromIndex < 0 || fromIndex >= count()) return;
    toIndex = qBound(0, toIndex, count() - 1);
    if (fromIndex == toIndex) return;

    auto layer = std::move(m_layers[fromIndex]);
    m_layers.erase(m_layers.begin() + fromIndex);
    m_layers.insert(m_layers.begin() + toIndex, std::move(layer));
}

Layer* LayerStack::duplicateLayer(LayerId id) {
    auto* source = layerById(id);
    if (!source) return nullptr;

    auto newId = nextId();
    auto copy = source->clone(newId);
    int idx = indexOf(id);
    return insertLayer(idx + 1, std::move(copy));
}

Layer* LayerStack::layerById(LayerId id) {
    for (auto& l : m_layers) {
        if (l->id() == id) return l.get();
    }
    return nullptr;
}

const Layer* LayerStack::layerById(LayerId id) const {
    for (auto& l : m_layers) {
        if (l->id() == id) return l.get();
    }
    return nullptr;
}

Layer* LayerStack::layerAt(int index) {
    if (index < 0 || index >= count()) return nullptr;
    return m_layers[index].get();
}

const Layer* LayerStack::layerAt(int index) const {
    if (index < 0 || index >= count()) return nullptr;
    return m_layers[index].get();
}

int LayerStack::indexOf(LayerId id) const {
    for (int i = 0; i < count(); ++i) {
        if (m_layers[i]->id() == id) return i;
    }
    return -1;
}

Layer* LayerStack::activeLayer() {
    return layerById(m_activeLayerId);
}

LayerId LayerStack::nextId() {
    return m_nextId++;
}

}  // namespace comicos
