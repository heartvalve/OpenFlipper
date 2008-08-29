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




//
// C++ Interface: LoggingInterface
//
// Description: 
//
//
// Author: Jan Moebius <jan_moebius@web.de>, (C) 2007
//

#ifndef LOGGINGINTERFACE_HH 
#define LOGGINGINTERFACE_HH 

/// Log types for Message Window 
enum Logtype { LOGOUT, LOGINFO , LOGWARN , LOGERR };

/** \brief Interface for all Plugins which do logging to the logging window of the framework
  * 
  * By emitting the given signals you can log information to the main logger window of the core.
  * To simplify debugging the core will prepend the plugin name to every log message. You dont
  * have to do this yourself!\n
  * The log message will either be black or will be colored depending on the Logtype you specified. 
 */ 
class LoggingInterface {
  
   signals :
      /** Send a log message to the mainwindow of the widget \n
       * @param _type Message type (LOGINFO,LOGOUT,LOGWARN,LOGERR)
       * @param _message Message to be displayed
       */
      virtual void log(Logtype _type, QString _message) = 0;
      
      /** Send a log message to the mainwindow of the widget \n
       * defaults to LOGOUT message type
       * @param _message Message to be displayed
       */
      virtual void log(QString _message) = 0;
      
   private slots:
     
      /** Through this slot you can receive all logging information emitted by OpenFlipper
       * or one of its plugins */
      virtual void logOutput( Logtype _type , QString _message ) {};
      
};

Q_DECLARE_INTERFACE(LoggingInterface,"OpenFlipper.LoggingInterface/1.0")
      
#endif // LOGGINGINTERFACE_HH
