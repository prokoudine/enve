#include "boxscrollwidget.h"
#include "BoxesList/OptimalScrollArea/scrollarea.h"
#include "Boxes/boundingbox.h"

BoxScrollWidget::BoxScrollWidget(ScrollArea *parent) :
    ScrollWidget(parent) {
    createVisiblePartWidget();
}

//void BoxScrollWidget::updateAbstraction() {
//    if(mMainTarget == NULL) {
//        mMainAbstraction = NULL;
//    } else {
//        mMainAbstraction = mMainTarget->
//                SWT_getAbstractionForWidget(mVisiblePartWidget);
//    }
//    mVisiblePartWidget->setMainAbstraction(mMainAbstraction);
//    updateHeight();
//}

BoxScrollWidgetVisiblePart *BoxScrollWidget::getVisiblePartWidget() {
    return (BoxScrollWidgetVisiblePart*)mVisiblePartWidget;
}

void BoxScrollWidget::createVisiblePartWidget() {
    mVisiblePartWidget = new BoxScrollWidgetVisiblePart(this);
}