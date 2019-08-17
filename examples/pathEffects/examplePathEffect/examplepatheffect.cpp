#include "examplepatheffect.h"

qsptr<CustomPathEffect> eCreateNewestVersion() {
    // Use default, most up to date, version
    return enve::make_shared<ExamplePathEffect000>();
}

qsptr<CustomPathEffect> eCreate(
        const CustomIdentifier &identifier) {
    Q_UNUSED(identifier);
    // Choose version based on identifier
    // if(identifier.fVersion == CustomIdentifier::Version{0, 0, 0})
    return enve::make_shared<ExamplePathEffect000>();
}

// Returned value must be unique, lets enve distinguish effects
QString effectId() {
    return "waer9yv11r3gl10x1qtm";
}

// Name of your effect used in UI
QString eName() {
    return "Example";
}

// here specify your effect's most up to date version
CustomIdentifier::Version effectVersion() {
    return { 0, 0, 0 };
}

CustomIdentifier eIdentifier() {
    return { effectId(), eName(), effectVersion() };
}

bool eSupports(const CustomIdentifier &identifier) {
    if(identifier.fEffectId != effectId()) return false;
    if(identifier.fEffectName != eName()) return false;
    return identifier.fVersion == effectVersion();
}

#include "enveCore/Animators/qrealanimator.h"
ExamplePathEffect000::ExamplePathEffect000() :
    CustomPathEffect(eName().toLower()) {
    mInfluence = enve::make_shared<QrealAnimator>(0, 0, 1, 0.1, "influence");
    ca_addChild(mInfluence);
}

CustomIdentifier ExamplePathEffect000::getIdentifier() const {
    return { effectId(), eName(), { 0, 0, 0 } };
}

void ExamplePathEffect000::apply(const qreal relFrame,
                                 const SkPath &src,
                                 SkPath * const dst) {
    const float infl = toSkScalar(mInfluence->getEffectiveValue(relFrame));
    const float invInf = 1 - infl;
    dst->reset();

    SkPath::Iter iter(src, false);
    SkPoint pts[4];
    for(;;) {
        switch(iter.next(pts, true, true)) {
        case SkPath::kLine_Verb: {
            dst->lineTo(pts[1]);
        } break;
        case SkPath::kQuad_Verb: {
            dst->quadTo(pts[1]*invInf + (pts[0] + pts[2])*0.5f*infl, pts[2]);
        } break;
        case SkPath::kConic_Verb: {
            dst->conicTo(pts[1]*invInf + (pts[0] + pts[2])*0.5f*infl,
                         pts[2], iter.conicWeight());
        } break;
        case SkPath::kCubic_Verb: {
            dst->cubicTo(pts[0]*infl + pts[1]*invInf,
                         pts[3]*infl + pts[2]*invInf, pts[3]);
        } break;
        case SkPath::kClose_Verb: {
            dst->close();
        } break;
        case SkPath::kMove_Verb: {
            dst->moveTo(pts[0]);
        } break;
        case SkPath::kDone_Verb:
            return;
        }
    }
}