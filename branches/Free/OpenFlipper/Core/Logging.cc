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
//  CLASS Core - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================

// -------------------- mview
#include "Core.hh"
// -------------------- ACG
#include "OpenFlipper/common/GlobalOptions.hh"

// -------------------- Qt
#include <QScrollBar>
#include <QApplication>

//== IMPLEMENTATION ========================================================== 

// 0m - all attibutes off
// 1m - intensity
// 4m - underscore
// 5m - blinking
// 7m - reverse

// 30m - black
// 31m - red
// 32m - green
// 33m - yellow
// 34m - blue
// 35m - pink
// 36m - cyan
// 37m - white
// 
// 
// to get background colours change the 3's to 4's - eg 43m is yellow background, 33m is yellow text.

/** \brief Slot writing everything to the Logger widget
 * 
 * This slot has to be called by all loggers. It is used to serialize
 * and color the Output.
 * 
 * @param _type Logtype (defines the color of the output)
 * @param _message The message for output
 **/
void 
Core::
slotLog(Logtype _type, QString _message) {
  emit externalLog( _type , _message );
  if ( OpenFlipper::Options::logToConsole()  ) {
    std::string output( _message.toLatin1() ) ;
    
    switch (_type) {
      case LOGINFO:
        std::clog << "\33[32m" << output << std::endl;
        break;
      case LOGOUT:
        std::cout << "\33[0m"  << output << std::endl;
        break;
      case LOGWARN:
        std::cout << "\33[33m" << output << std::endl;
        break;
      case LOGERR:
        std::cerr << "\33[31m" << output << std::endl;
        break;
      case LOGSTATUS:
        std::cerr << "\33[34m" << output << std::endl;
        break;
    }
  }
  
}

//=============================================================================
