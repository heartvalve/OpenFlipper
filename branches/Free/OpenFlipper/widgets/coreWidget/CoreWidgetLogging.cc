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
*   $LastChangedBy$                                                *
*   $Date$                     *
*                                                                            *
\*===========================================================================*/




//=============================================================================
//
//  CLASS MViewWidget - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================

#include "CoreWidget.hh"

// -------------------- ACG
#include "OpenFlipper/common/GlobalOptions.hh"

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

  QColor textColor;

  switch (_type) {
    case LOGINFO:
      textColor = QColor(0,160,0);
      break;
    case LOGOUT:
      textColor = QColor(0,0,0);
      break;
    case LOGWARN:
      textColor = QColor(160,160,0);
      break;
    case LOGERR:
      textColor = QColor(250,0,0);
      break;
    case LOGSTATUS:
      textColor = QColor(0,0,250);
      break;
  }

  logWidget_->append(_message, _type);

  if (_type == LOGERR)
    statusBar_->showMessage(_message,textColor, 4000);

}

//=============================================================================
