//=============================================================================
//
//                               OpenFlipper
//        Copyright (C) 2008 by Computer Graphics Group, RWTH Aachen
//                           www.openflipper.org
//
//-----------------------------------------------------------------------------
//
//                                License
//
//  OpenFlipper is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
// 
//  OpenFlipper is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
// 
//  You should have received a copy of the GNU Lesser General Public License
//  along with OpenFlipper.  If not, see <http://www.gnu.org/licenses/>.
//
//-----------------------------------------------------------------------------
//
//   $Revision: 2063 $
//   $Author: moebius $
//   $Date: 2008-06-22 17:32:25 +0200 (So, 22 Jun 2008) $
//
//=============================================================================




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
  statusBar_ = statusBar();
  
  QPixmap pix;
  pix.load(OpenFlipper::Options::iconDirStr() +
          OpenFlipper::Options::dirSeparator() + 
          "status_green.png");
  
  
  statusIcon_ = new QLabel();
  statusIcon_->setPixmap(pix.scaled(12,12,Qt::KeepAspectRatio,Qt::SmoothTransformation));
  
  statusBar_->addPermanentWidget(statusIcon_);
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
  
//   QApplication::processEvents();
  
}

//=============================================================================
