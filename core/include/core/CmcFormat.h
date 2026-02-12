#pragma once

#include "core/Document.h"
#include <QString>
#include <memory>

namespace comicos {

/// Chunk-based binary format (.cmc) for saving/loading documents.
///
/// File layout:
///   [Magic: "CMC\x01"]  — 4 bytes, magic + format version
///   [Tag: 4B] [Size: uint32] [Data]  — repeated chunks
///   ["END\0"] [Size: 0]             — terminator
///
/// Unknown chunks are skipped by size, enabling forward compatibility.
class CmcFormat {
public:
    static bool save(const Document& doc, const QString& path);
    static std::unique_ptr<Document> load(const QString& path);

private:
    static constexpr char MAGIC[4] = {'C', 'M', 'C', '\x01'};

    static constexpr char TAG_CANV[4] = {'C', 'A', 'N', 'V'};
    static constexpr char TAG_LYRS[4] = {'L', 'Y', 'R', 'S'};
    static constexpr char TAG_TILE[4] = {'T', 'I', 'L', 'E'};
    static constexpr char TAG_END[4]  = {'E', 'N', 'D', '\0'};
};

}  // namespace comicos
