#include "Boxes/circle.h"
#include "canvas.h"
#include "MovablePoints/movablepoint.h"
#include "Animators/gradientpoints.h"
#include "PropertyUpdaters/nodepointupdater.h"
#include "Animators/transformanimator.h"
#include "MovablePoints/pointshandler.h"
#include "PathEffects/patheffectanimators.h"

Circle::Circle() : PathBox(TYPE_CIRCLE) {
    prp_setName("Circle");

    setPointsHandler(enve::make_shared<PointsHandler>());

    mCenterAnimator = enve::make_shared<QPointFAnimator>("center");
    mCenterPoint = enve::make_shared<AnimatedPoint>(mCenterAnimator.get(),
                                             mTransformAnimator.get(),
                                             TYPE_PATH_POINT);
    mPointsHandler->appendPt(mCenterPoint);

    mCenterPoint->disableSelection();
    mCenterPoint->setRelativePos(QPointF(0, 0));
    mCenterAnimator->prp_setInheritedUpdater(enve::make_shared<NodePointUpdater>(this));
    ca_prependChildAnimator(mPathEffectsAnimators.data(),
                            mCenterAnimator);

    mHorizontalRadiusAnimator =
            enve::make_shared<QPointFAnimator>("horizontal radius");
    mHorizontalRadiusPoint = enve::make_shared<CircleRadiusPoint>(
                mHorizontalRadiusAnimator.get(), mTransformAnimator.get(),
                mCenterPoint.get(), TYPE_PATH_POINT, false);
    mPointsHandler->appendPt(mHorizontalRadiusPoint);
    mHorizontalRadiusPoint->setRelativePos(QPointF(10, 0));
    const auto hXAnimator = mHorizontalRadiusAnimator->getXAnimator();
    ca_prependChildAnimator(mPathEffectsAnimators.data(),
                            hXAnimator->ref<QrealAnimator>());
    hXAnimator->prp_setName("horizontal radius");

    mVerticalRadiusAnimator =
            enve::make_shared<QPointFAnimator>("vertical radius");
    mVerticalRadiusPoint = enve::make_shared<CircleRadiusPoint>(
                mVerticalRadiusAnimator.get(), mTransformAnimator.get(),
                mCenterPoint.get(), TYPE_PATH_POINT, true);
    mPointsHandler->appendPt(mVerticalRadiusPoint);
    mVerticalRadiusPoint->setRelativePos(QPointF(0, 10));
    const auto vYAnimator = mVerticalRadiusAnimator->getYAnimator();
    ca_prependChildAnimator(mPathEffectsAnimators.data(),
                            vYAnimator->ref<QrealAnimator>());
    vYAnimator->prp_setName("vertical radius");
    prp_setInheritedUpdater(enve::make_shared<NodePointUpdater>(this));
}

void Circle::moveRadiusesByAbs(const QPointF &absTrans) {
    mVerticalRadiusPoint->moveByAbs(absTrans);
    mHorizontalRadiusPoint->moveByAbs(absTrans);
}

void Circle::setVerticalRadius(const qreal verticalRadius) {
    const QPointF centerPos = mCenterPoint->getRelativePos();
    mVerticalRadiusPoint->setRelativePos(
                centerPos + QPointF(0, verticalRadius));
}

void Circle::setHorizontalRadius(const qreal horizontalRadius) {
    const QPointF centerPos = mCenterPoint->getRelativePos();
    mHorizontalRadiusPoint->setRelativePos(
                centerPos + QPointF(horizontalRadius, 0));
}

void Circle::setRadius(const qreal radius) {
    setHorizontalRadius(radius);
    setVerticalRadius(radius);
}

bool Circle::SWT_isCircle() const { return true; }

SkPath Circle::getPathAtRelFrameF(const qreal relFrame) {
    const float xRadius = static_cast<float>(
                mHorizontalRadiusAnimator->getEffectiveXValue(relFrame));
    const float yRadius = static_cast<float>(
                mVerticalRadiusAnimator->getEffectiveYValue(relFrame));
    SkRect rect = SkRect::MakeXYWH(-xRadius, -yRadius, 2*xRadius, 2*yRadius);
    const QPointF center = mCenterAnimator->getEffectiveValue();
    rect.offset(toSkPoint(center));
    SkPath path;
    path.addOval(rect);
    return path;
}

qreal Circle::getCurrentXRadius() {
    return mHorizontalRadiusAnimator->getEffectiveXValue();
}

qreal Circle::getCurrentYRadius() {
    return mVerticalRadiusAnimator->getEffectiveYValue();
}

void Circle::getMotionBlurProperties(QList<Property*> &list) const {
    PathBox::getMotionBlurProperties(list);
    list.append(mHorizontalRadiusAnimator.get());
    list.append(mVerticalRadiusAnimator.get());
}

bool Circle::differenceInEditPathBetweenFrames(
        const int frame1, const int frame2) const {
    if(mCenterAnimator->prp_differencesBetweenRelFrames(frame1, frame2)) return true;
    if(mHorizontalRadiusAnimator->prp_differencesBetweenRelFrames(frame1, frame2)) return true;
    return mVerticalRadiusAnimator->prp_differencesBetweenRelFrames(frame1, frame2);
}

CircleRadiusPoint::CircleRadiusPoint(QPointFAnimator * const associatedAnimator,
                                     BasicTransformAnimator * const parent,
                                     AnimatedPoint * const centerPoint,
                                     const MovablePointType &type,
                                     const bool blockX) :
    AnimatedPoint(associatedAnimator, type),
    mXBlocked(blockX), mCenterPoint(centerPoint) {
    setTransform(parent);
    disableSelection();
}

QPointF CircleRadiusPoint::getRelativePos() const {
    const QPointF centerPos = mCenterPoint->getRelativePos();
    return AnimatedPoint::getRelativePos() + centerPos;
}

void CircleRadiusPoint::setRelativePos(const QPointF &relPos) {
    const QPointF centerPos = mCenterPoint->getRelativePos();
    if(mXBlocked) {
        setValue(QPointF(0, relPos.y() - centerPos.y()));
    } else {
        setValue(QPointF(relPos.x() - centerPos.x(), 0));
    }
}