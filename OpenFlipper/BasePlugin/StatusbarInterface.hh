/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2009 by Computer Graphics Group, RWTH Aachen      *
 *                           www.openflipper.org                             *
 *                                                                           *
 *---------------------------------------------------------------------------*
 *  This file is part of OpenFlipper.                                        *
 *                                                                           *
 *  OpenFlipper is free software: you can redistribute it and/or modify      *
 *  it under the terms of the GNU Lesser General Public License as           *
 *  published by the Free Software Foundation, either version 3 of           *
 *  the License, or (at your option) any later version with the              *
 *  following exceptions:                                                    *
 *                                                                           *
 *  If other files instantiate templates or use macros                       *
 *  or inline functions from this file, or you compile this file and         *
 *  link it with other files to produce an executable, this file does        *
 *  not by itself cause the resulting executable to be covered by the        *
 *  GNU Lesser General Public License. This exception does not however       *
 *  invalidate any other reasons why the executable file might be            *
 *  covered by the GNU Lesser General Public License.                        *
 *                                                                           *
 *  OpenFlipper is distributed in the hope that it will be useful,           *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU Lesser General Public License for more details.                      *
 *                                                                           *
 *  You should have received a copy of the GNU LesserGeneral Public          *
 *  License along with OpenFlipper. If not,                                  *
 *  see <http://www.gnu.org/licenses/>.                                      *
 *                                                                           *
\*===========================================================================*/

/*===========================================================================*\
 *                                                                           *
 *   $Revision$                                                         *
 *   $Author$                                                      *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/

//
// C++ Interface: StatusBarInterface
//
// Description:
//
//
// Author: Jan Moebius <jan_moebius@web.de>, (C) 2007
//

#ifndef STATUSBARINTERFACE_HH
#define STATUSBARINTERFACE_HH

 #include <QtGui>
 #include <OpenFlipper/common/Types.hh>
 #include <QStatusBar>

namespace ApplicationStatus {
  
/** Enum for the statusBar Status Icon
 */
enum applicationStatus { READY , PROCESSING, BLOCKED};

}


 /**
 */
class StatusbarInterface {

   private slots :

   public :

      /// Destructor
     virtual ~StatusbarInterface() {};

    signals :

      /** \brief Show a message in the status bar
       * 
       * You can specify a timeout if the message should be removed after
       * _timeout milliseconds
       */
      virtual void showStatusMessage(QString /*_message*/, int /*_timeout*/ = 0) {};
      
      
      /** \brief Sets the icon of the StatusBar
       * 
       */
      void setStatus( ApplicationStatus::applicationStatus /*_status*/) {};
      
      /** \brief Removes the current message from the statusBar
       * 
       */
      void clearStatusMessage() {};
      
      /** \brief Add a widget on the right side of the statusBar
       * 
       */
      void addWidgetToStatusbar(QWidget* _widget) {};
      
};

Q_DECLARE_INTERFACE(StatusbarInterface,"OpenFlipper.StatusbarInterface/1.0")

#endif // STATUSBARINTERFACE_HH
