/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2014 by Computer Graphics Group, RWTH Aachen       *
*                           www.openflipper.org                              *
*                                                                            *
*--------------------------------------------------------------------------- *
*  This file is part of OpenFlipper.                                         *
*                                                                            *
*  OpenFlipper is free software: you can redistribute it and/or modify       *
*  it under the terms of the GNU Lesser General Public License as            *
*  published by the Free Software Foundation, either version 3 of            *
*  the License, or (at your option) any later version with the               *
*  following exceptions:                                                     *
*                                                                            *
*  If other files instantiate templates or use macros                        *
*  or inline functions from this file, or you compile this file and          *
*  link it with other files to produce an executable, this file does         *
*  not by itself cause the resulting executable to be covered by the         *
*  GNU Lesser General Public License. This exception does not however        *
*  invalidate any other reasons why the executable file might be             *
*  covered by the GNU Lesser General Public License.                         *
*                                                                            *
*  OpenFlipper is distributed in the hope that it will be useful,            *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
*  GNU Lesser General Public License for more details.                       *
*                                                                            *
*  You should have received a copy of the GNU LesserGeneral Public           *
*  License along with OpenFlipper. If not,                                   *
*  see <http://www.gnu.org/licenses/>.                                       *
*                                                                            *
\*===========================================================================*/

/*===========================================================================*\
*                                                                            *
*   $Revision$                                                       *
*   $LastChangedBy$                                                 *
*   $Date$                    *
*                                                                            *
\*===========================================================================*/

#include <OpenFlipper/widgets/coreWidget/CoreWidget.hh>

/** Hide or show logger
 */
void CoreWidget::toggleLogger() {

    switch (OpenFlipper::Options::loggerState()) {
    case OpenFlipper::Options::InScene:
        OpenFlipper::Options::loggerState(OpenFlipper::Options::Normal);
        break;
    case OpenFlipper::Options::Normal:
        OpenFlipper::Options::loggerState(OpenFlipper::Options::Hidden);
        break;
    case OpenFlipper::Options::Hidden:
        OpenFlipper::Options::loggerState(OpenFlipper::Options::InScene);
        break;
    }

    // Hide/Show Logger
    showLogger(OpenFlipper::Options::loggerState());
}

//-----------------------------------------------------------------------------

/** Hide or show logger
 */
void CoreWidget::showLogger(OpenFlipper::Options::LoggerState _state) {

    //Hide Logger
    if (_state == loggerState_)
        return;

    switch (_state) {
    case OpenFlipper::Options::InScene: {

        showLoggerInSplitView(false);
        logWidget_->resize(logWidget_->width(), originalLoggerSize_);

        slidingLogger_->attachWidget(logWidget_);

        break;
    }
    case OpenFlipper::Options::Normal: {

        slidingLogger_->detachWidget();

        showLoggerInSplitView(true);

        logWidget_->show();

        break;
    }
    case OpenFlipper::Options::Hidden: {

        if (loggerState_ == OpenFlipper::Options::InScene)
            slidingLogger_->detachWidget();

        showLoggerInSplitView(false);

        break;
    }
    } // End switch

    loggerState_ = _state;
}

//-----------------------------------------------------------------------------

void CoreWidget::showLoggerInSplitView(bool _show) {

    if(!_show) {

        QList<int> wsizes = splitter_->sizes();

        // Remember old size
        if (loggerState_ == OpenFlipper::Options::Normal) {
            if(wsizes.size() > 1) {
                originalLoggerSize_ = wsizes[1];
            } else {
                originalLoggerSize_ = 240;
            }
        }

        if (originalLoggerSize_ == 0)
            originalLoggerSize_ = 240;

        logWidget_->setParent(0);
        wsizes[0] = 1;
        splitter_->setSizes(wsizes);
    } else {

        if (loggerState_ == OpenFlipper::Options::InScene)
            originalLoggerSize_ = logWidget_->height();

        if (originalLoggerSize_ == 0)
            originalLoggerSize_ = 240;

        logWidget_->setParent(splitter_);
        splitter_->insertWidget(1, logWidget_);
        QList<int> wsizes = splitter_->sizes();
        int totalHeight = wsizes[0] + wsizes[1];
        wsizes[1] = originalLoggerSize_;
        wsizes[0] = totalHeight - originalLoggerSize_;
        splitter_->setSizes(wsizes);
    }
}

//-----------------------------------------------------------------------------

void CoreWidget::updateInSceneLoggerGeometry() {

  if(loggerState_ == OpenFlipper::Options::InScene) {
      slidingLogger_->updateGeometry();
  }
}
