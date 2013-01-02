/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2011 by Computer Graphics Group, RWTH Aachen       *
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
*   $LastChangedBy$                                                *
*   $Date$                     *
*                                                                            *
\*===========================================================================*/



//=============================================================================
//
//  CLASS CoreWidget - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================

// -------------------- mview
#include "CoreWidget.hh"
#include <OpenFlipper/common/GlobalOptions.hh>

//== IMPLEMENTATION ========================================================== 



void CoreWidget::statusMessage(QString _message, int _timeout) {
  statusBar_->showMessage(_message,_timeout);
}


//=============================================================================


void CoreWidget::setupStatusBar()
{
  statusBar_ = new ColorStatusBar();

  setStatusBar( statusBar_ );

  QPixmap pix;
  pix.load(OpenFlipper::Options::iconDirStr() +
          OpenFlipper::Options::dirSeparator() + 
          "status_green.png");
  
  
  statusIcon_ = new QLabel();
  statusIcon_->setPixmap(pix.scaled(12,12,Qt::KeepAspectRatio,Qt::SmoothTransformation));
  
  statusBar_->addPermanentWidget(statusIcon_);

  if ( ! OpenFlipperSettings().value("Core/Gui/StatusBar/hidden",false).toBool() )
    statusBar()->show();
  else
    statusBar()->hide();
}

//=============================================================================

void CoreWidget::clearStatusMessage()
{
  statusBar_->clearMessage();
}

//=============================================================================

void CoreWidget::setStatus( ApplicationStatus::applicationStatus _status)
{
  QPixmap pix;

  switch (_status) {
    case ApplicationStatus::READY :
      pix.load(OpenFlipper::Options::iconDirStr() +
          OpenFlipper::Options::dirSeparator() + 
          "status_green.png");
      break;
    case ApplicationStatus::PROCESSING :
      pix.load(OpenFlipper::Options::iconDirStr() +
          OpenFlipper::Options::dirSeparator() + 
          "status_yellow.png");
      break;
    case ApplicationStatus::BLOCKED :
      pix.load(OpenFlipper::Options::iconDirStr() +
          OpenFlipper::Options::dirSeparator() + 
          "status_red.png");
      break;
  }
  
  statusIcon_->setPixmap(pix.scaled(12,12,Qt::KeepAspectRatio,Qt::SmoothTransformation));
  
}

void CoreWidget::addWidgetToStatusbar(QWidget* _widget){

  statusBar_->addPermanentWidget(_widget);

  statusBar_->removeWidget(statusIcon_);
  statusBar_->addPermanentWidget(statusIcon_);
  statusIcon_->show();
  
  
}

//-----------------------------------------------------------------------------

/** Hide or show Status bar
  */
void
CoreWidget::toggleStatusBar() {

  //toggle
  showStatusBar( OpenFlipperSettings().value("Core/Gui/StatusBar/hidden",false).toBool() );
}

//-----------------------------------------------------------------------------

/** Hide or show  Status bar
  */
void
CoreWidget::showStatusBar( bool _state ) {

  //toggle
  OpenFlipperSettings().setValue("Core/Gui/StatusBar/hidden",!_state);

  if ( OpenFlipperSettings().value("Core/Gui/StatusBar/hidden",false).toBool() ){
    statusBar_->setVisible(false);
  }else{
    statusBar_->setVisible(true);
  }
}

//=============================================================================
