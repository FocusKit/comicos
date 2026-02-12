#include "engine/Compositor.h"
#include <algorithm>
#include <cmath>
#include <cstring>

namespace comicos {

Compositor::Compositor() = default;
Compositor::~Compositor() = default;

std::vector<uint8_t> Compositor::compositeTile(const LayerStack& layers,
                                                const TileCoord& coord) const {
    std::vector<uint8_t> result(TILE_BYTES, 0);

    // Here is where the compositing pipeline goes:
    // Bottom-to-top layer compositing with blend modes.
    // Future: GPU compute shader compositing for real-time performance.

    for (auto& layerPtr : layers.layers()) {
        const Layer* layer = layerPtr.get();
        if (!layer->isVisible() || layer->opacity() <= 0.0f) continue;

        const Tile* tile = layer->tiles().tileAt(coord);
        if (!tile || tile->isEmpty()) continue;

        const uint8_t* src = tile->constData();
        if (!src) continue;

        float layerOpacity = layer->opacity();
        BlendMode mode = layer->blendMode();

        for (int i = 0; i < TILE_PIXELS; ++i) {
            int offset = i * 4;
            Pixel dst = {result[offset], result[offset + 1],
                         result[offset + 2], result[offset + 3]};
            Pixel srcPx = {src[offset], src[offset + 1],
                           src[offset + 2], src[offset + 3]};

            Pixel out = blendPixels(dst, srcPx, mode, layerOpacity);
            result[offset] = out.r;
            result[offset + 1] = out.g;
            result[offset + 2] = out.b;
            result[offset + 3] = out.a;
        }
    }

    return result;
}

QImage Compositor::compositeRegion(const LayerStack& /*layers*/,
                                    const QRectF& region) const {
    int w = static_cast<int>(std::ceil(region.width()));
    int h = static_cast<int>(std::ceil(region.height()));
    QImage image(w, h, QImage::Format_RGBA8888);
    image.fill(Qt::transparent);

    // Extension point: optimize by only compositing visible tiles
    // For now, delegate to compositeTile per visible tile in region

    return image;
}

QImage Compositor::flatten(const LayerStack& layers, const QSize& canvasSize) const {
    return compositeRegion(layers, QRectF(QPointF(0, 0), canvasSize));
}

Pixel Compositor::blendPixels(const Pixel& dst, const Pixel& src,
                               BlendMode mode, float layerOpacity) {
    // Apply layer opacity to source
    Pixel adjusted = src;
    adjusted.a = static_cast<uint8_t>(src.a * layerOpacity);

    switch (mode) {
    case BlendMode::Normal:
        return alphaComposite(dst, adjusted, 1.0f);

    case BlendMode::Multiply:
        // Extension point: multiply blend mode
        // For now, fall through to normal
        return alphaComposite(dst, adjusted, 1.0f);

    case BlendMode::Screen:
        // Extension point: screen blend mode
        return alphaComposite(dst, adjusted, 1.0f);

    case BlendMode::Overlay:
        // Extension point: overlay blend mode
        return alphaComposite(dst, adjusted, 1.0f);
    }

    return alphaComposite(dst, adjusted, 1.0f);
}

Pixel Compositor::alphaComposite(const Pixel& dst, const Pixel& src, float opacity) {
    float sa = (src.a / 255.0f) * opacity;
    float da = dst.a / 255.0f;
    float outA = sa + da * (1.0f - sa);

    if (outA <= 0.0f) return {};

    auto blend = [&](uint8_t s, uint8_t d) -> uint8_t {
        float sf = s / 255.0f;
        float df = d / 255.0f;
        float result = (sf * sa + df * da * (1.0f - sa)) / outA;
        return static_cast<uint8_t>(std::clamp(result * 255.0f, 0.0f, 255.0f));
    };

    return {
        blend(src.r, dst.r),
        blend(src.g, dst.g),
        blend(src.b, dst.b),
        static_cast<uint8_t>(std::clamp(outA * 255.0f, 0.0f, 255.0f))};
}

}  // namespace comicos
