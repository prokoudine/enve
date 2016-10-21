#ifndef VECTORPATH_H
#define VECTORPATH_H
#include <QPainterPath>
#include "pathpoint.h"
#include <QLinearGradient>
#include "pathanimator.h"
#include "pathbox.h"

class BoxesGroup;

class MainWindow;

class PathPivot;

enum CanvasMode : short;

class Edge;

class VectorPath : public PathBox
{
public:
    VectorPath(BoxesGroup *group);
    static VectorPath *createPathFromSql(int boundingBoxId, BoxesGroup *parent);

    ~VectorPath();

    void drawSelected(QPainter *p, CanvasMode currentCanvasMode);

    PathPoint *addPointAbsPos(QPointF absPtPos, PathPoint *toPoint = NULL);
    PathPoint *addPointRelPos(QPointF relPtPos, PathPoint *toPoint = NULL);

    MovablePoint *getPointAt(QPointF absPtPos, CanvasMode currentCanvasMode);

    void selectAndAddContainedPointsToList(QRectF absRect, QList<MovablePoint*> *list);

    PathPoint *addPoint(PathPoint *pointToAdd, PathPoint *toPoint);

    void removePoint(PathPoint *point);
    void replaceSeparatePathPoint(PathPoint *pointBeingReplaced, PathPoint *newPoint);
    void addPointToSeparatePaths(PathPoint *pointToAdd, bool saveUndoRedo = true);
    void removePointFromSeparatePaths(PathPoint *pointToRemove, bool saveUndoRedo = true);
    void appendToPointsList(PathPoint *point, bool saveUndoRedo = true);
    void removeFromPointsList(PathPoint *point, bool saveUndoRedo = true);

    PathPoint *addPointRelPos(QPointF relPos,
                              QPointF startRelPos, QPointF endRelPos,
                              PathPoint *toPoint = NULL);
    int saveToSql(int parentId);

    void clearAll();

    PathPoint *createNewPointOnLineNear(QPointF absPos, bool adjust);
    qreal percentAtPoint(QPointF absPos, qreal distTolerance,
                         qreal maxPercent, qreal minPercent,
                         bool *found = NULL, QPointF *posInPath = NULL);
    PathPoint *findPointNearestToPercent(qreal percent, qreal *foundAtPercent);

    void updateAfterFrameChanged(int currentFrame);

    void startAllPointsTransform();
    void finishAllPointsTransform();

    Edge *getEgde(QPointF absPos);

    void setInfluenceEnabled(bool bT) {
        if(bT) {
            enableInfluence();
        } else {
            disableInfluence();
        }
    }

    void disableInfluence() {
        mInfluenceEnabled = false;

        foreach(PathPoint *point, mPoints) {
            point->disableInfluenceAnimators();
        }
    }

    void enableInfluence() {
        mInfluenceEnabled = true;
        schedulePathUpdate();

        foreach(PathPoint *point, mPoints) {
            point->enableInfluenceAnimators();
        }
    }

    void showContextMenu(QPoint globalPos);

    PathPoint *findPointNearestToPercentEditPath(qreal percent, qreal *foundAtPercent);
    qreal findPercentForPointEditPath(QPointF point, qreal minPercent = 0., qreal maxPercent = 1.);
    Edge *getEdgeFromMappedEditPath(QPointF absPos);
    Edge *getEdgeFromMappedPath(QPointF absPos);
    void deletePointAndApproximate(PathPoint *pointToRemove);
    virtual void loadFromSql(int boundingBoxId);
protected:
    bool mInfluenceEnabled = false;
    void updatePath();
    void updateMappedPath();
    void updatePathPointIds();
    PathAnimator mPathAnimator;

    void loadPointsFromSql(int vectorPathId);

    qreal findPercentForPoint(QPointF point, qreal minPercent = 0.,
                              qreal maxPercent = 1.);

    bool mClosedPath = false;

    QList<PathPoint*> mSeparatePaths;
    QList<PathPoint*> mPoints;
    QPainterPath mEditPath;
    void centerPivotPosition();
};

#endif // VECTORPATH_H
