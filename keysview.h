#ifndef KEYSVIEW_H
#define KEYSVIEW_H

#include <QWidget>
#include "pointhelpers.h"

class QrealKey;

class QrealAnimator;

class QrealPoint;

class Canvas;

class MainWindow;

class BoxesList;

class AnimationDockWidget;

class KeysView : public QWidget
{
    Q_OBJECT
public:
    explicit KeysView(BoxesList *boxesList, QWidget *parent = 0);

    void setAnimationDockWidget(AnimationDockWidget *graphControls);
    void setGraphViewed(bool bT);

    int getMinViewedFrame();
    int getMaxViewedFrame();
    qreal getPixelsPerFrame();

    void updatePixelsPerFrame();

    void addKeyToSelection(QrealKey *key);
    void removeKeyFromSelection(QrealKey *key);
    void clearKeySelection();
    void selectKeysInSelectionRect();

    // graph

    void graphPaint(QPainter *p);
    void graphWheelEvent(QWheelEvent *event);
    void graphMousePressEvent(QPoint eventPos, Qt::MouseButton eventButton);
    void graphMouseMoveEvent(QPoint eventPos, Qt::MouseButtons eventButtons);
    void graphMouseReleaseEvent(Qt::MouseButton eventButton);
    bool graphProcessFilteredKeyEvent(QKeyEvent *event);
    void graphResizeEvent(QResizeEvent *);
    void graphSetAnimator(QrealAnimator *animator);
    void graphIncScale(qreal inc);
    void graphSetScale(qreal scale);
    void graphUpdateDimensions();
    void graphIncMinShownVal(qreal inc);
    void graphSetMinShownVal(qreal newMinShownVal);
    void graphGetValueAndFrameFromPos(QPointF pos, qreal *value, qreal *frame);
    void graphMiddleMove(QPointF movePos);
    void graphMiddlePress(QPointF pressPos);
    void graphMouseRelease();
    void graphMousePress(QPointF pressPos);
    void graphMiddleRelease();
    void graphSetCtrlsModeForSelected(CtrlsMode mode);
    void graphDeletePressed();
    void graphClearKeysSelection();
    void graphAddKeyToSelection(QrealKey *key);
    void graphRemoveKeyFromSelection(QrealKey *key);
    void graphMouseMove(QPointF mousePos);
    void graphRepaint();
    void graphUpdateDrawPathIfNeeded();
    void graphSetDrawPathUpdateNeeded();
    void graphResetValueScaleAndMinShown();
    void scheduleGraphUpdateAfterKeysChanged();
    void graphUpdateAfterKeysChangedIfNeeded();
    bool processFilteredKeyEvent(QKeyEvent *event);
    void ifIsCurrentAnimatorSetNull(QrealAnimator *animator);
    void deleteSelectedKeys();
    void middleMove(QPointF movePos);
    void middlePress(QPointF pressPos);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *e);
protected:
    void resizeEvent(QResizeEvent *e);

    void wheelEvent(QWheelEvent *e);

    void mousePressEvent(QMouseEvent *e);

    void paintEvent(QPaintEvent *);
signals:
    void changedViewedFrames(int, int);
public slots:
    void graphUpdateAfterKeysChangedAndRepaint();

    void graphUpdateAfterKeysChanged();
    void graphMergeKeysIfNeeded();

    void setFramesRange(int startFrame, int endFrame);

    void graphSetSmoothCtrl();
    void graphSetSymmetricCtrl();
    void graphSetCornerCtrl();
    void graphSetTwoSideCtrlForSelected();
    void graphSetRightSideCtrlForSelected();
    void graphSetLeftSideCtrlForSelected();
    void graphSetNoSideCtrlForSelected();
private:
    AnimationDockWidget *mGraphControls = NULL;

    BoxesList *mBoxesList;
    QRectF mSelectionRect;
    bool mSelecting = false;
    bool mGraphViewed = false;
    qreal mPixelsPerFrame;
    QPointF mMiddlePressPos;
    QrealAnimator *mAnimator = NULL;

    bool mGraphUpdateAfterKeysChangedNeeded = false;

    QrealKey *mLastPressedKey = NULL;
    bool mFirstMove = false;
    int mMoveDFrame = 0;
    QPointF mLastPressPos;
    bool mMovingKeys = false;

    Canvas *mCanvas;
    MainWindow *mMainWindow;
    QList<QrealKey*> mSelectedKeys;

    int mMinViewedFrame = 0;
    int mMaxViewedFrame = 50;


    // graph

    qreal mMargin = 20.;
    qreal mPixelsPerValUnit;
    qreal mMinShownVal;
    QrealPoint *mCurrentPoint = NULL;
    qreal mMinVal;
    qreal mMaxVal;
    qreal mValueScale = 1.;
    qreal mMinMoveFrame;
    qreal mMaxMoveFrame;
    QPointF mPressFrameAndValue;
    qreal mSavedMinViewedFrame;
    qreal mSavedMaxViewedFrame;
    qreal mSavedMinShownValue;
    qreal mValueInc;
};

#endif // KEYSVIEW_H