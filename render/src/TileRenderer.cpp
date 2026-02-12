#include "render/TileRenderer.h"
#include "core/LayerStack.h"
#include "engine/Compositor.h"
#include <QSGRectangleNode>
#include <QSGSimpleTextureNode>
#include <QSGTransformNode>
#include <QImage>
#include <unordered_set>

namespace comicos {

TileRenderer::TileRenderer() = default;

TileRenderer::~TileRenderer() {
    // SceneGraph nodes are owned by the scene graph tree.
    // We just clear our tracking state.
    m_nodes.clear();
    m_paperNode = nullptr;
}

void TileRenderer::setLayerStack(const LayerStack* layers) {
    m_layers = layers;
}

void TileRenderer::setCompositor(const Compositor* compositor) {
    m_compositor = compositor;
}

QSGNode* TileRenderer::updateSceneGraph(
    QSGNode* oldNode,
    QQuickWindow* window,
    const std::vector<TileCoord>& visibleTiles,
    const QMatrix4x4& viewMatrix,
    const QSize& canvasSize) {

    if (!window || !m_layers || !m_compositor) {
        delete oldNode;
        m_nodes.clear();
        m_paperNode = nullptr;
        return nullptr;
    }

    auto* rootNode = static_cast<QSGTransformNode*>(oldNode);
    if (!rootNode) {
        rootNode = new QSGTransformNode();
    }

    // If cleared, remove all children from existing root (keeps same pointer for Qt)
    if (m_cleared) {
        while (rootNode->childCount() > 0) {
            QSGNode* child = rootNode->firstChild();
            rootNode->removeChildNode(child);
            delete child;
        }
        m_nodes.clear();
        m_paperNode = nullptr;
        m_cleared = false;
    }

    // Apply view transform
    rootNode->setMatrix(viewMatrix);

    // Paper background (first child)
    if (!m_paperNode) {
        m_paperNode = window->createRectangleNode();
        m_paperNode->setColor(Qt::white);
        rootNode->prependChildNode(m_paperNode);
    }
    m_paperNode->setRect(QRectF(0, 0, canvasSize.width(), canvasSize.height()));

    // Build set of visible tile coords for quick lookup
    std::unordered_set<TileCoord> visibleSet(visibleTiles.begin(), visibleTiles.end());

    // Update or create nodes for each visible tile
    for (const auto& tc : visibleTiles) {
        // Quick check: does any visible layer have data at this tile?
        bool hasData = false;
        for (const auto& layerPtr : m_layers->layers()) {
            if (layerPtr->isVisible() && layerPtr->tiles().hasTile(tc)) {
                hasData = true;
                break;
            }
        }

        if (!hasData) {
            // Remove existing node if it exists
            auto it = m_nodes.find(tc);
            if (it != m_nodes.end()) {
                if (it->second.sgNode) {
                    rootNode->removeChildNode(it->second.sgNode);
                    delete it->second.sgNode;
                }
                m_nodes.erase(it);
            }
            continue;
        }

        // Composite tile data from all layers
        auto data = m_compositor->compositeTile(*m_layers, tc);

        // Create QImage from composited RGBA8 data
        QImage image(data.data(), TILE_SIZE, TILE_SIZE, TILE_SIZE * 4,
                     QImage::Format_RGBA8888);
        QImage imageCopy = image.copy();  // deep copy, data vector is temporary

        // Create GPU texture
        QSGTexture* texture = window->createTextureFromImage(
            imageCopy, QQuickWindow::TextureHasAlphaChannel);

        auto& tileNode = m_nodes[tc];

        // Create SceneGraph node if needed
        if (!tileNode.sgNode) {
            tileNode.sgNode = new QSGSimpleTextureNode();
            tileNode.sgNode->setOwnsTexture(true);
            rootNode->appendChildNode(tileNode.sgNode);
        } else {
            // setOwnsTexture(true) means old texture is deleted when we set new one
        }

        tileNode.sgNode->setTexture(texture);
        tileNode.sgNode->setRect(tc.tx * TILE_SIZE, tc.ty * TILE_SIZE,
                                  TILE_SIZE, TILE_SIZE);
        tileNode.sgNode->setFiltering(QSGTexture::Nearest);
    }

    // Remove nodes for tiles no longer visible
    std::vector<TileCoord> toRemove;
    for (auto& [coord, node] : m_nodes) {
        if (visibleSet.find(coord) == visibleSet.end()) {
            if (node.sgNode) {
                rootNode->removeChildNode(node.sgNode);
                delete node.sgNode;  // also deletes owned texture
            }
            toRemove.push_back(coord);
        }
    }
    for (auto& coord : toRemove) {
        m_nodes.erase(coord);
    }

    return rootNode;
}

void TileRenderer::invalidate(const std::vector<TileCoord>& /*coords*/) {
    // MVP: we re-composite all visible tiles every frame.
    // The coords parameter is reserved for future dirty-tile optimization.
}

void TileRenderer::clearAll() {
    m_nodes.clear();
    m_paperNode = nullptr;
    m_cleared = true;
}

}  // namespace comicos
