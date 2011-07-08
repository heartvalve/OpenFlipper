#include "CoreWidget.hh"

//========================================================

void CoreWidget::showInSceneLogger() {

    if(slidingLogger_ != 0) {
        slidingLogger_->show();
        updateInSceneLoggerGeometry(glScene_->sceneRect());
    }
}

//========================================================

void CoreWidget::hideInSceneLogger() {

    if(slidingLogger_ != 0) {
        slidingLogger_->hide();
    }
}

//========================================================

void CoreWidget::updateInSceneLoggerGeometry(const QRectF& _rect) {

   if(!slidingLogger_) return;

   slidingLogger_->setParentGeometry(_rect);

   slidingLogger_->updateGeometry();
}
