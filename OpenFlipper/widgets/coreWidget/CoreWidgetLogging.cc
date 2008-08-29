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
//   $Revision$
//   $Author$
//   $Date$
//
//=============================================================================




//=============================================================================
//
//  CLASS MViewWidget - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================

// -------------------- mview
#include "CoreWidget.hh"
// -------------------- ACG
#include "OpenFlipper/common/GlobalOptions.hh"

// -------------------- Qt
#include <QScrollBar>
#include <QApplication>

//== IMPLEMENTATION ========================================================== 

/** \brief Slot writing everything to the Logger widget
 * 
 * This slot has to be called by all loggers. It is used to serialize
 * and color the Output.
 * 
 * @param _type Logtype (defines the color of the output)
 * @param _message The message for output
 **/
void 
CoreWidget::
slotLog(Logtype _type, QString _message) {
  switch (_type) {
    case LOGINFO:
      textedit_->setTextColor(QColor(0,160,0));
      break;
    case LOGOUT:
      textedit_->setTextColor(QColor(0,0,0));
      break;
    case LOGWARN:
      textedit_->setTextColor(QColor(160,160,0));
      break;
    case LOGERR:
      textedit_->setTextColor(QColor(250,0,0));
      break;
  }
  
  textedit_->append(_message);
  
  QScrollBar* bar = textedit_->verticalScrollBar();
  bar->setValue(bar->maximum());
  
  // Make shure, we see the message
  QApplication::processEvents();
}

//=============================================================================
