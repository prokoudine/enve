#ifndef GRADIENTPOINTS_H
#define GRADIENTPOINTS_H
#include "complexanimator.h"
#include "gradientpoint.h"

class MovablePoint;

class GradientPoints : public ComplexAnimator {
public:
    GradientPoints();
    ~GradientPoints();

    void initialize(PathBox *parentT);

    void enable();

    void disable();

    void drawGradientPoints(QPainter *p);

    MovablePoint *getPointAt(QPointF absPos);

    QPointF getStartPoint();

    QPointF getEndPoint();

    void setColors(QColor startColor, QColor endColor);

    bool enabled;
    GradientPoint *startPoint;
    GradientPoint *endPoint;
    PathBox *parent;

    void setPositions(QPointF startPos, QPointF endPos, bool saveUndoRedo = true);
    void loadFromSql(int fillGradientStartId, int fillGradientEndId);
};

#endif // GRADIENTPOINTS_H