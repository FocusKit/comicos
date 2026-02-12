#pragma once

#include "core/History.h"
#include "core/LayerStack.h"
#include "core/Types.h"
#include <QSize>
#include <QString>

namespace comicos {

/// Root document model.
/// Contains all layers, history, and document-level settings.
/// This is the top-level data object that gets serialized to .cmc files.
class Document {
public:
    Document();
    explicit Document(const QSize& canvasSize);
    ~Document();

    // --- Canvas ---
    QSize canvasSize() const { return m_canvasSize; }
    void setCanvasSize(const QSize& size) { m_canvasSize = size; }
    int dpi() const { return m_dpi; }
    void setDpi(int dpi) { m_dpi = dpi; }

    // --- Layers ---
    LayerStack& layers() { return m_layers; }
    const LayerStack& layers() const { return m_layers; }

    // --- History ---
    History& history() { return m_history; }
    const History& history() const { return m_history; }

    // --- File ---
    const QString& filePath() const { return m_filePath; }
    void setFilePath(const QString& path) { m_filePath = path; }
    bool isDirty() const { return m_dirty; }
    void setDirty(bool dirty) { m_dirty = dirty; }

    // --- Serialization ---
    // Extension point: .cmc project format save/load
    // bool save(const QString& path);
    // static std::unique_ptr<Document> load(const QString& path);

    // Extension point: export to PNG/PSD/etc.
    // QImage exportFlattened() const;

private:
    QSize m_canvasSize{2480, 3508};  // A4 at 300dpi
    int m_dpi = 300;
    LayerStack m_layers;
    History m_history;
    QString m_filePath;
    bool m_dirty = false;
};

}  // namespace comicos
