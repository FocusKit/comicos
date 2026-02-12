#pragma once

#include "core/LayerStack.h"
#include "core/Types.h"
#include <QImage>
#include <QRectF>
#include <vector>

namespace comicos {

/// Layer compositing pipeline.
/// Composites all visible layers into a final image for display or export.
/// Supports partial updates (only re-composites dirty regions).
class Compositor {
public:
    Compositor();
    ~Compositor();

    /// Composite all visible layers for the given tile coordinate.
    /// Returns the composited RGBA8 tile data.
    std::vector<uint8_t> compositeTile(const LayerStack& layers,
                                        const TileCoord& coord) const;

    /// Composite all visible layers in a region and return as QImage.
    /// Used for export and preview.
    QImage compositeRegion(const LayerStack& layers,
                           const QRectF& region) const;

    /// Flatten the entire canvas to a single QImage.
    QImage flatten(const LayerStack& layers, const QSize& canvasSize) const;

    // Extension point: here is where the compositing pipeline goes
    // Future features:
    // - GPU-accelerated compositing via compute shaders
    // - Blend mode implementations (multiply, screen, overlay, etc.)
    // - Clipping masks
    // - Adjustment layers
    // - Alpha lock

private:
    /// Blend two RGBA8 pixels using the given blend mode.
    static Pixel blendPixels(const Pixel& dst, const Pixel& src,
                             BlendMode mode, float layerOpacity);

    /// Alpha composite (src over dst).
    static Pixel alphaComposite(const Pixel& dst, const Pixel& src,
                                float opacity);
};

}  // namespace comicos
