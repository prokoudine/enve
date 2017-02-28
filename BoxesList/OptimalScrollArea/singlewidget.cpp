#include "singlewidget.h"
#include "singlewidgetabstraction.h"
#include "scrollwidgetvisiblepart.h"

SingleWidget::SingleWidget(ScrollWidgetVisiblePart *parent) :
    QWidget(parent) {
    mParent = parent;
    setFixedHeight(20);
}

void SingleWidget::setTargetAbstraction(SingleWidgetAbstraction *abs) {
    mTarget = abs;
    if(abs == NULL) {
        hide();
    } else {
        if(isHidden()) show();
    }
}