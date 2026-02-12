#pragma once

#include "core/Types.h"
#include <QMatrix4x4>
#include <QSGNode>
#include <QSGSimpleTextureNode>
#include <QSGTexture>
#include <QQuickWindow>
#include <unordered_map>
#include <memory>

class QSGRectangleNode;

namespace comicos {

class LayerStack;
class Compositor;

/// Manages SceneGraph nodes for visible tiles.
/// Creates and updates QSGSimpleTextureNode instances for each
/// visible tile in the canvas viewport.
class TileRenderer {
public:
    TileRenderer();
    ~TileRenderer();

    /// Set the layer stack to read tile data from.
    void setLayerStack(const LayerStack* layers);

    /// Set the compositor for compositing layer tiles.
    void setCompositor(const Compositor* compositor);

    /// Update the SceneGraph for visible tiles.
    /// Called from QQuickItem::updatePaintNode on the render thread.
    QSGNode* updateSceneGraph(
        QSGNode* oldNode,
        QQuickWindow* window,
        const std::vector<TileCoord>& visibleTiles,
        const QMatrix4x4& viewMatrix,
        const QSize& canvasSize);

    /// Invalidate cached textures for given tiles.
    void invalidate(const std::vector<TileCoord>& coords);

    /// Clear all cached state. Next updateSceneGraph rebuilds everything.
    void clearAll();

private:
    struct TileNode {
        QSGSimpleTextureNode* sgNode = nullptr;
    };

    const LayerStack* m_layers = nullptr;
    const Compositor* m_compositor = nullptr;

    std::unordered_map<TileCoord, TileNode> m_nodes;
    QSGRectangleNode* m_paperNode = nullptr;
    bool m_cleared = false;
};

}  // namespace comicos
