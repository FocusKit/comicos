#include "core/Document.h"

namespace comicos {

Document::Document() {
    // Create default layer
    m_layers.addLayer(QStringLiteral("레이어 1"));
}

Document::Document(const QSize& canvasSize) : m_canvasSize(canvasSize) {
    m_layers.addLayer(QStringLiteral("레이어 1"));
}

Document::~Document() = default;

}  // namespace comicos
