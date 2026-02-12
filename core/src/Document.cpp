#include "core/Document.h"
#include "core/CmcFormat.h"

namespace comicos {

Document::Document() {
    // Create default layer
    m_layers.addLayer(QStringLiteral("레이어 1"));
}

Document::Document(const QSize& canvasSize) : m_canvasSize(canvasSize) {
    m_layers.addLayer(QStringLiteral("레이어 1"));
}

Document::~Document() = default;

bool Document::save(const QString& path) {
    if (!CmcFormat::save(*this, path))
        return false;
    m_filePath = path;
    m_dirty = false;
    return true;
}

std::unique_ptr<Document> Document::load(const QString& path) {
    auto doc = CmcFormat::load(path);
    if (doc)
        doc->setFilePath(path);
    return doc;
}

}  // namespace comicos
