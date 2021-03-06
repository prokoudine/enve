#include "blureffect.h"
#include "Animators/qrealanimator.h"

class BlurEffectCaller : public RasterEffectCaller {
public:
    BlurEffectCaller(const HardwareSupport hwSupport,
                     const qreal radius);

    void processGpu(QGL33 * const gl,
                    GpuRenderTools& renderTools,
                    GpuRenderData& data);
    void processCpu(CpuRenderTools& renderTools,
                    const CpuRenderData &data);
private:
    const float mRadius;
};

BlurEffect::BlurEffect() :
    RasterEffect("blur", HardwareSupport::gpuPreffered,
                 false, RasterEffectType::BLUR) {
    mRadius = enve::make_shared<QrealAnimator>(10, 0, 999.999, 1, "radius");
    ca_addChild(mRadius);
    connect(mRadius.get(), &QrealAnimator::valueChangedSignal,
            this, &RasterEffect::forcedMarginChanged);
}

stdsptr<RasterEffectCaller> BlurEffect::getEffectCaller(
        const qreal relFrame, const qreal resolution) const {
    const qreal radius = mRadius->getEffectiveValue(relFrame)*resolution;
    if(isZero4Dec(radius)) return nullptr;
    return enve::make_shared<BlurEffectCaller>(instanceHwSupport(), radius);
}

QMargins radiusToMargin(const qreal radius) {
    return QMargins() + qCeil(radius);
}

QMargins BlurEffect::getMargin() const {
    return radiusToMargin(mRadius->getEffectiveValue());
}

BlurEffectCaller::BlurEffectCaller(const HardwareSupport hwSupport,
                                   const qreal radius) :
    RasterEffectCaller(hwSupport, true, radiusToMargin(radius)),
    mRadius(static_cast<float>(radius)) {}


void BlurEffectCaller::processGpu(QGL33 * const gl,
                                  GpuRenderTools &renderTools,
                                  GpuRenderData &data) {
    Q_UNUSED(gl)
    Q_UNUSED(data)

    const float sigma = mRadius*0.3333333f;
    const auto filter = SkBlurImageFilter::Make(sigma, sigma, nullptr);

    SkPaint paint;
    paint.setImageFilter(filter);

    renderTools.switchToSkia();
    const auto canvas = renderTools.requestTargetCanvas();
    canvas->clear(SK_ColorTRANSPARENT);
    const auto srcTex = renderTools.requestSrcTextureImageWrapper();
    canvas->drawImage(srcTex, 0, 0, &paint);
    canvas->flush();

    renderTools.swapTextures();
}

void BlurEffectCaller::processCpu(CpuRenderTools &renderTools,
                                  const CpuRenderData &data) {
    Q_UNUSED(data)

    const float sigma = mRadius*0.3333333f;
    const auto filter = SkBlurImageFilter::Make(sigma, sigma, nullptr);

    SkPaint paint;
    paint.setImageFilter(filter);

    SkBitmap tile;
    renderTools.requestBackupBitmap().extractSubset(&tile, data.fTexTile);
    SkCanvas canvas(tile);
    canvas.clear(SK_ColorTRANSPARENT);

    const int radCeil = static_cast<int>(ceil(mRadius));
    const auto& srcBtmp = renderTools.fSrcDst;
    const auto& texTile = data.fTexTile;
    auto srcRect = texTile.makeOutset(radCeil, radCeil);
    if(srcRect.intersect(srcRect, srcBtmp.bounds())) {
        SkBitmap tileSrc;
        srcBtmp.extractSubset(&tileSrc, srcRect);
        canvas.drawBitmap(tileSrc,
                          srcRect.left() - texTile.left(),
                          srcRect.top() - texTile.top(), &paint);
    }

    renderTools.swap();
}
