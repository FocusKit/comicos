#include "core/CmcFormat.h"
#include "core/Layer.h"
#include "core/LayerStack.h"
#include "core/Tile.h"
#include "core/TileManager.h"
#include "core/Types.h"
#include <QByteArray>
#include <QDataStream>
#include <QFile>
#include <QIODevice>
#include <QSaveFile>

namespace comicos {

// --- Helpers ---

static void writeTag(QDataStream& out, const char tag[4]) {
    out.writeRawData(tag, 4);
}

static bool readTag(QDataStream& in, char tag[4]) {
    return in.readRawData(tag, 4) == 4;
}

static bool tagsEqual(const char a[4], const char b[4]) {
    return a[0] == b[0] && a[1] == b[1] && a[2] == b[2] && a[3] == b[3];
}

// --- Save ---

bool CmcFormat::save(const Document& doc, const QString& path) {
    QSaveFile file(path);
    if (!file.open(QIODevice::WriteOnly))
        return false;

    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_6_5);
    out.setByteOrder(QDataStream::LittleEndian);

    // Magic
    out.writeRawData(MAGIC, 4);

    // CANV chunk
    {
        QByteArray buf;
        QDataStream s(&buf, QIODevice::WriteOnly);
        s.setVersion(QDataStream::Qt_6_5);
        s.setByteOrder(QDataStream::LittleEndian);
        s << static_cast<quint32>(doc.canvasSize().width())
          << static_cast<quint32>(doc.canvasSize().height())
          << static_cast<quint32>(doc.dpi());

        writeTag(out, TAG_CANV);
        out << static_cast<quint32>(buf.size());
        out.writeRawData(buf.constData(), buf.size());
    }

    // LYRS chunk
    {
        const auto& stack = doc.layers();
        QByteArray buf;
        QDataStream s(&buf, QIODevice::WriteOnly);
        s.setVersion(QDataStream::Qt_6_5);
        s.setByteOrder(QDataStream::LittleEndian);

        s << static_cast<quint32>(stack.count())
          << static_cast<quint64>(stack.activeLayerId())
          << static_cast<quint64>(stack.peekNextId());

        for (const auto& layer : stack.layers()) {
            s << static_cast<quint64>(layer->id())
              << layer->name()
              << layer->opacity()
              << layer->isVisible()
              << layer->isLocked()
              << static_cast<quint8>(layer->blendMode());
        }

        writeTag(out, TAG_LYRS);
        out << static_cast<quint32>(buf.size());
        out.writeRawData(buf.constData(), buf.size());
    }

    // TILE chunks — one per non-empty tile
    {
        const auto& stack = doc.layers();
        for (const auto& layer : stack.layers()) {
            auto tiles = layer->tiles().allTiles();
            for (const Tile* tile : tiles) {
                if (tile->isEmpty()) continue;

                const uint8_t* raw = tile->constData();
                if (!raw) continue;

                QByteArray uncompressed(reinterpret_cast<const char*>(raw), TILE_BYTES);
                QByteArray compressed = qCompress(uncompressed);

                QByteArray buf;
                QDataStream s(&buf, QIODevice::WriteOnly);
                s.setVersion(QDataStream::Qt_6_5);
                s.setByteOrder(QDataStream::LittleEndian);
                s << static_cast<quint64>(layer->id())
                  << static_cast<qint32>(tile->coord().tx)
                  << static_cast<qint32>(tile->coord().ty)
                  << compressed;

                writeTag(out, TAG_TILE);
                out << static_cast<quint32>(buf.size());
                out.writeRawData(buf.constData(), buf.size());
            }
        }
    }

    // END chunk
    writeTag(out, TAG_END);
    out << static_cast<quint32>(0);

    return file.commit();
}

// --- Load ---

std::unique_ptr<Document> CmcFormat::load(const QString& path) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
        return nullptr;

    QDataStream in(&file);
    in.setVersion(QDataStream::Qt_6_5);
    in.setByteOrder(QDataStream::LittleEndian);

    // Verify magic
    char magic[4];
    if (in.readRawData(magic, 4) != 4) return nullptr;
    if (!tagsEqual(magic, MAGIC)) return nullptr;

    // Temporary storage for parsed data
    QSize canvasSize;
    int dpi = 300;
    bool hasCanv = false;

    struct LayerInfo {
        quint64 id;
        QString name;
        float opacity;
        bool visible;
        bool locked;
        quint8 blendMode;
    };
    std::vector<LayerInfo> layerInfos;
    quint64 activeLayerId = 0;
    quint64 nextLayerId = 1;

    struct TileInfo {
        quint64 layerId;
        qint32 tx, ty;
        QByteArray compressed;
    };
    std::vector<TileInfo> tileInfos;

    // Read chunks
    while (!in.atEnd()) {
        char tag[4];
        if (!readTag(in, tag)) break;

        quint32 size;
        in >> size;

        if (tagsEqual(tag, TAG_END)) {
            break;
        }

        // Read chunk data into buffer
        QByteArray chunkData(size, Qt::Uninitialized);
        if (in.readRawData(chunkData.data(), size) != static_cast<int>(size))
            return nullptr;

        QDataStream s(chunkData);
        s.setVersion(QDataStream::Qt_6_5);
        s.setByteOrder(QDataStream::LittleEndian);

        if (tagsEqual(tag, TAG_CANV)) {
            quint32 w, h, d;
            s >> w >> h >> d;
            canvasSize = QSize(w, h);
            dpi = d;
            hasCanv = true;
        } else if (tagsEqual(tag, TAG_LYRS)) {
            quint32 count;
            s >> count >> activeLayerId >> nextLayerId;

            layerInfos.reserve(count);
            for (quint32 i = 0; i < count; ++i) {
                LayerInfo li;
                s >> li.id >> li.name >> li.opacity
                  >> li.visible >> li.locked >> li.blendMode;
                layerInfos.push_back(std::move(li));
            }
        } else if (tagsEqual(tag, TAG_TILE)) {
            TileInfo ti;
            s >> ti.layerId >> ti.tx >> ti.ty >> ti.compressed;
            tileInfos.push_back(std::move(ti));
        }
        // Unknown chunks are silently skipped (forward compatibility)
    }

    if (!hasCanv) return nullptr;

    // Build document
    auto doc = std::make_unique<Document>(canvasSize);
    doc->setDpi(dpi);

    // Remove the default layer created by the constructor
    auto& stack = doc->layers();
    if (stack.count() > 0) {
        stack.removeLayer(stack.layerAt(0)->id());
    }

    // Recreate layers from file
    for (const auto& li : layerInfos) {
        auto layer = std::make_unique<Layer>(li.id, li.name);
        layer->setOpacity(li.opacity);
        layer->setVisible(li.visible);
        layer->setLocked(li.locked);
        layer->setBlendMode(static_cast<BlendMode>(li.blendMode));
        stack.insertLayer(stack.count(), std::move(layer));
    }
    stack.setActiveLayerId(activeLayerId);
    stack.setNextId(nextLayerId);

    // Restore tile data
    for (const auto& ti : tileInfos) {
        Layer* layer = stack.layerById(ti.layerId);
        if (!layer) continue;

        QByteArray raw = qUncompress(ti.compressed);
        if (raw.size() != TILE_BYTES) continue;

        TileCoord coord{ti.tx, ti.ty};
        Tile* tile = layer->tiles().getOrCreateTile(coord);
        tile->ensureAllocated();
        std::memcpy(tile->data(), raw.constData(), TILE_BYTES);
    }

    doc->setDirty(false);
    return doc;
}

}  // namespace comicos
