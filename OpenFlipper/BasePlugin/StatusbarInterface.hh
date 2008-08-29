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
//   $Revision: 1909 $
//   $Author: wilden $
//   $Date: 2008-06-03 18:45:21 +0200 (Tue, 03 Jun 2008) $
//
//=============================================================================

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
      virtual void showStatusMessage(QString _message, int _timeout = 0) {};
      
      
      /** \brief Sets the icon of the StatusBar
       * 
       */
      void setStatus( ApplicationStatus::applicationStatus _status) {};
      
      /** \brief Removes the current message from the statusBar
       * 
       */
      void clearStatusMessage() {};
      
};

Q_DECLARE_INTERFACE(StatusbarInterface,"OpenFlipper.StatusbarInterface/1.0")

#endif // STATUSBARINTERFACE_HH
