#ifndef COLORANIMATOR_H
#define COLORANIMATOR_H

#include "Animators/complexanimator.h"
#include "colorhelpers.h"
#include "qrealanimator.h"

class ColorAnimator : public ComplexAnimator {
    Q_OBJECT
    friend class SelfRef;
public:
    void qra_setCurrentValue(const QColor &qcolorValue);

    QColor getCurrentColor() const;
    QColor getColorAtRelFrame(const int &relFrame);
    QColor getColorAtRelFrameF(const qreal &relFrame);
    void setColorMode(const ColorMode &colorMode);

    void startVal1Transform();
    void startVal2Transform();
    void startVal3Transform();
    void startAlphaTransform();

    void setCurrentVal1Value(const qreal &val1);
    void setCurrentVal2Value(const qreal &val2);
    void setCurrentVal3Value(const qreal &val3);
    void setCurrentAlphaValue(const qreal &alpha);

    void prp_openContextMenu(const QPoint &pos);

    void duplicateVal1AnimatorFrom(QrealAnimator *source);
    void duplicateVal2AnimatorFrom(QrealAnimator *source);
    void duplicateVal3AnimatorFrom(QrealAnimator *source);
    void duplicateAlphaAnimatorFrom(QrealAnimator *source);

    ColorMode getColorMode() {
        return mColorMode;
    }

    QrealAnimator *getVal1Animator() {
        return mVal1Animator.get();
    }

    QrealAnimator *getVal2Animator() {
        return mVal2Animator.get();
    }

    QrealAnimator *getVal3Animator() {
        return mVal3Animator.get();
    }

    QrealAnimator *getAlphaAnimator() {
        return mAlphaAnimator.get();
    }

    bool SWT_isColorAnimator() const { return true; }

    void writeProperty(QIODevice * const target) const;
    void readProperty(QIODevice *target);
protected:
    ColorAnimator(const QString& name = "color");
private:
    ColorMode mColorMode = RGBMODE;

    qsptr<QrealAnimator> mVal1Animator =
            QrealAnimator::create0to1Animator("");
    qsptr<QrealAnimator> mVal2Animator =
            QrealAnimator::create0to1Animator("");
    qsptr<QrealAnimator> mVal3Animator =
            QrealAnimator::create0to1Animator("");
    qsptr<QrealAnimator> mAlphaAnimator =
            QrealAnimator::create0to1Animator("alpha");
signals:
    void colorModeChanged(ColorMode);
};

#endif // COLORANIMATOR_H
