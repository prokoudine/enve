#include "ctrlpoint.h"
#include "pathpoint.h"
#include "vectorpath.h"
#include "boxesgroup.h"

CtrlPoint::CtrlPoint(PathPoint *parentPoint, bool isStartCtrlPt) :
    MovablePoint(parentPoint->getParentPath(),
                 MovablePointType::TYPE_CTRL_POINT)
{
    mIsStartCtrlPt = isStartCtrlPt;
    mParentPoint = parentPoint;
    mRelPos.setTraceKeyOnCurrentFrame(true);
}

void CtrlPoint::moveToWithoutUpdatingTheOther(QPointF absPos)
{
    MovablePoint::moveToAbs(absPos);
}

void CtrlPoint::moveToAbs(QPointF absPos)
{
    MovablePoint::moveToAbs(absPos);
    if(mOtherCtrlPt->isSelected()) {
        return;
    }
    mParentPoint->ctrlPointPosChanged(mIsStartCtrlPt);
}

void CtrlPoint::moveBy(QPointF absTranslation)
{
    MovablePoint::moveBy(absTranslation);
    if(mOtherCtrlPt->isSelected()) {
        return;
    }
    mParentPoint->ctrlPointPosChanged(mIsStartCtrlPt);
}

void CtrlPoint::startTransform()
{
    MovablePoint::startTransform();
    mParentPoint->MovablePoint::startTransform();
    mOtherCtrlPt->MovablePoint::startTransform();
}

void CtrlPoint::finishTransform()
{
    startNewUndoRedoSet();
    mParentPoint->MovablePoint::finishTransform();
    MovablePoint::finishTransform();
    mOtherCtrlPt->MovablePoint::finishTransform();
    finishUndoRedoSet();
}

void CtrlPoint::setOtherCtrlPt(CtrlPoint *ctrlPt) {
    mOtherCtrlPt = ctrlPt;
}

void CtrlPoint::remove()
{
    mParentPoint->setCtrlPtEnabled(false, mIsStartCtrlPt);
}

bool CtrlPoint::isHidden()
{
    return MovablePoint::isHidden() ||
           (!mParentPoint->isNeighbourSelected() && !BoxesGroup::getCtrlsAlwaysVisible() );
}
