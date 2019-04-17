#include "boxpathpoint.h"
#include "pointhelpers.h"
#include "Animators/transformanimator.h"

BoxPathPoint::BoxPathPoint(QPointFAnimator * const associatedAnimator,
                           BoxTransformAnimator * const boxTrans) :
    AnimatedPoint(associatedAnimator, boxTrans, TYPE_PIVOT_POINT, 7),
    mBoxTransform_cv(boxTrans) {}

void BoxPathPoint::setRelativePos(const QPointF &relPos) {
    mBoxTransform_cv->setPivotAutoAdjust(false);
    mBoxTransform_cv->setPivotFixedTransform(relPos);
}

void BoxPathPoint::startTransform() {
    MovablePoint::startTransform();
    mBoxTransform_cv->startPivotTransform();
}

void BoxPathPoint::finishTransform() {
    mBoxTransform_cv->finishPivotTransform();
}

void BoxPathPoint::drawSk(SkCanvas * const canvas,
                          const SkScalar &invScale) {
    if(isHidden()) return;
    const SkPoint absPos = toSkPoint(getAbsolutePos());
    const SkColor fillCol = isSelected() ?
                SkColorSetRGB(255, 255, 0) :
                SkColorSetRGB(255, 255, 125);
    drawOnAbsPosSk(canvas, absPos, invScale, fillCol);

    canvas->save();
    canvas->translate(absPos.x(), absPos.y());
    SkPaint paint;
    paint.setStyle(SkPaint::kStroke_Style);
    paint.setColor(SK_ColorBLACK);
    const SkScalar scaledHalfRadius = toSkScalar(getRadius()*0.5)*invScale;
    canvas->drawLine(-scaledHalfRadius, 0, scaledHalfRadius, 0, paint);
    canvas->drawLine(0, -scaledHalfRadius, 0, scaledHalfRadius, paint);
    canvas->restore();
}