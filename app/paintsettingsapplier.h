#ifndef PAINTSETTINGSAPPLIER_H
#define PAINTSETTINGSAPPLIER_H
#include "smartPointers/sharedpointerdefs.h"
#include "colorsetting.h"
#include "Segments/qcubicsegment1d.h"
#include "GUI/BrushWidgets/brushwidget.h"
#include <QPainter>
#include "Boxes/pathbox.h"
#include "Animators/gradient.h"
class ColorAnimator;
class Gradient;
class PathBox;
enum PaintType : short;
class PaintSettings;

class PaintSetting {
public:
    enum Target { FILL, OUTLINE };
    virtual ~PaintSetting() {}

    void apply(PathBox * const target) const {
        applyToPS(targetPaintSettings(target));
    }
protected:
    PaintSetting(const Target& target) : mTarget(target) {}
    virtual void applyToPS(PaintSettings * const target) const = 0;
private:
    PaintSettings * targetPaintSettings(PathBox * const target) const {
        return mTarget == FILL ? target->getFillSettings() :
                                 target->getStrokeSettings();
    }
    const Target mTarget;
};

class ColorPaintSetting : public PaintSetting {
public:
    ColorPaintSetting(const Target& target,
                      const ColorSetting& colorSettings) :
        PaintSetting(target), mColorSetting(colorSettings) {}
protected:
    void applyToPS(PaintSettings * const target) const {
        mColorSetting.apply(target->getColorAnimator());
    }
private:
    const ColorSetting mColorSetting;
};

class GradientPaintSetting : public PaintSetting {
public:
    GradientPaintSetting(const Target& target,
                         Gradient * const gradient) :
        PaintSetting(target), mGradient(gradient) {}
protected:
    void applyToPS(PaintSettings * const target) const {
        target->setGradient(mGradient);
    }
private:
    Gradient * const mGradient;
};

class GradientTypePaintSetting : public PaintSetting {
public:
    GradientTypePaintSetting(const Target& target,
                             const Gradient::Type& type) :
        PaintSetting(target), mGradientType(type) {}
protected:
    void applyToPS(PaintSettings * const target) const {
        target->setGradientType(mGradientType);
    }
private:
    const Gradient::Type mGradientType;
};

class PaintTypePaintSetting : public PaintSetting {
public:
    PaintTypePaintSetting(const Target& target,
                          const PaintType& type) :
        PaintSetting(target), mPaintType(type) {}
protected:
    void applyToPS(PaintSettings * const target) const {
        target->setPaintType(mPaintType);
    }
private:
    const PaintType mPaintType;
};

class OutlineWidthPaintSetting : public PaintSetting {
public:
    OutlineWidthPaintSetting(const qreal& width) :
        PaintSetting(OUTLINE), mWidth(width) {}
protected:
    void applyToPS(PaintSettings * const target) const {
        static_cast<StrokeSettings*>(target)->setCurrentStrokeWidth(mWidth);
    }
private:
    const qreal mWidth;
};

class StrokeBrushPaintSetting : public PaintSetting {
public:
    StrokeBrushPaintSetting(SimpleBrushWrapper * const brush) :
        PaintSetting(OUTLINE), mBrush(brush) {}
protected:
    void applyToPS(PaintSettings * const target) const {
        static_cast<StrokeSettings*>(target)->setStrokeBrush(mBrush);
    }
private:
    SimpleBrushWrapper * const mBrush;
};

class StrokeWidthCurvePaintSetting : public PaintSetting {
public:
    StrokeWidthCurvePaintSetting(const qCubicSegment1D& widthCurve) :
        PaintSetting(OUTLINE), mWidthCurve(widthCurve) {}
protected:
    void applyToPS(PaintSettings * const target) const {
        static_cast<StrokeSettings*>(target)->setStrokeBrushWidthCurve(mWidthCurve);
    }
private:
    const qCubicSegment1D mWidthCurve;
};

class StrokePressureCurvePaintSetting : public PaintSetting {
public:
    StrokePressureCurvePaintSetting(const qCubicSegment1D& pressureCurve) :
        PaintSetting(OUTLINE), mPressureCurve(pressureCurve) {}
protected:
    void applyToPS(PaintSettings * const target) const {
        static_cast<StrokeSettings*>(target)->setStrokeBrushPressureCurve(mPressureCurve);
    }
private:
    const qCubicSegment1D mPressureCurve;
};

class StrokeTimeCurvePaintSetting : public PaintSetting {
public:
    StrokeTimeCurvePaintSetting(const qCubicSegment1D& timeCurve) :
        PaintSetting(OUTLINE), mTimeCurve(timeCurve) {}
protected:
    void applyToPS(PaintSettings * const target) const {
        static_cast<StrokeSettings*>(target)->setStrokeBrushTimeCurve(mTimeCurve);
    }
private:
    const qCubicSegment1D mTimeCurve;
};

class PaintSettingsApplier {
public:
    inline PaintSettingsApplier &operator<< (const stdsptr<PaintSetting> &t)
    { mSettings << t; return *this; }

    void apply(PathBox * const target) const {
        for(const auto& setting : mSettings)
            setting->apply(target);
    }
private:
    QList<stdsptr<PaintSetting>> mSettings;
};

#endif // PAINTSETTINGSAPPLIER_H