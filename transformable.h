#ifndef TRANSFORMABLE_H
#define TRANSFORMABLE_H
#include "connectedtomainwindow.h"
#include <QPointF>


class Transformable : public ConnectedToMainWindow
{
public:
    Transformable();

    virtual void startTransform() {}
    virtual void finishTransform() {}

    virtual void moveBy(QPointF absTranslatione) { Q_UNUSED(absTranslatione); }

    virtual void scale(qreal scaleXBy, qreal scaleYBy) {
        Q_UNUSED(scaleXBy); Q_UNUSED(scaleYBy);
    }
    virtual void scale(qreal scaleBy) {
        scale(scaleBy, scaleBy);
    }

    virtual void rotateBy(qreal rot) {
        Q_UNUSED(rot);
    }

    bool isSelected() { return mSelected; }

    virtual bool isBeingTransformed() { return mSelected; }

    virtual void saveTransformPivot(QPointF absPivot) { Q_UNUSED(absPivot); }

    virtual void cancelTransform() {}
protected:
    QPointF mSavedTransformPivot;
    bool mSelected = false;
};

#endif // TRANSFORMABLE_H
