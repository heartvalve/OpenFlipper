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



#ifndef LOGGINGINTERFACE_HH 
#define LOGGINGINTERFACE_HH 

/** \file LoggingInterface.hh
*
* Interface for sending log messages to the log widget. \ref loggingInterfacePage
*/

/** \page loggingInterfacePage Logging Interface
\image html LoggingInterface.png
The Logging interface can be used by plugins to print messages in OpenFlippers log widget below 
the gl viewer (See image). The log widget can apply filters to visualize only the messages
of a certain importance. The widget has 3 different modes, which can be toggled by pressing Ctrl + L:
<ul>
<li> Docked automatic mode: In this mode the widget is at the bottom and will hide itself if not used.
<li> Docked mode: The widget is always visible and docked at the bottom
<li> Undocked: The widget is undocked into a separate window
</ul>

The message importance level can be specified by the Logtype enum. There are four different levels which 
describe the importance and are represented by different colors in the log widget.
<ul>
<li> Out:    Simple output messages in black
<li> Info:   Information messages printed in green
<li> Warn:   Warnings in yellow
<li> Errors: Error messages printed in red
</ul>

To use the LoggingInterface:
<ul>
<li> include LoggingInterface.hh in your plugins header file
<li> derive your plugin from the class LoggingInterface
<li> add Q_INTERFACES(LoggingInterface) to your plugin class 
<li> And add the signals or slots you want to use to your plugin class (You don't need to implement all of them)
</ul>

If you use the interface, all messages printed from that plugin will be prepended with the name of the plugin
to identify the origin of the message.\n
The following code sends a log message
\code 
  emit log(LOGERR,tr("Error message"));
\endcode

*/

/** \brief Log types for Message Window 
*
* Use this enum to specify the importance of log messages.
*/
enum Logtype {  LOGSTATUS , /*!< Status log messages. Will be printed in blue in the logwidget */
                LOGOUT    , /*!< Standard log messages. Will be printed in black in the logwidget */
                LOGINFO   , /*!< Info log messages. Will be printed in green in the logwidget */
                LOGWARN   , /*!< Warning messages. Will be printed in yellow in the logwidget */
                LOGERR      /*!< Error messages. Will be printed in red in the logwidget */
             };

/** \brief Interface for all Plugins which do logging to the logging window of the framework
  * 
  * \ref loggingInterfacePage "Detailed description" 
  * \n
  * By emitting the given signals you can log information to the main logger window of the core.
  * To simplify debugging, the core will prepend the plugin name to every log message. You dont
  * have to do this yourself!\n
  * The log message will either be black or will be colored depending on the Logtype you specified.
  *
  * A more detailed description of the LoggingInterface can be found \ref loggingInterfacePage "here" or
  * read our tutorials \ref ex2 and \ref ex3 for an example of how to use logging output.
 */ 
class LoggingInterface {
  
   signals :
      /** Send a log message to the mainwindow of the widget
       *
       * @param _type Message type (LOGINFO,LOGOUT,LOGWARN,LOGERR)
       * @param _message Message to be displayed
       */
      virtual void log(Logtype _type, QString _message) = 0;
      
      /** Send a log message to the mainwindow of the widget \n
       * defaults to LOGOUT message type
       *
       * @param _message Message to be displayed
       */
      virtual void log(QString _message) = 0;
      
   private slots:
     
      /** Through this slot you can receive all logging information emitted by OpenFlipper
       * or one of its plugins
       *
       * @param _type    Message type
       * @param _message Message
       */
      virtual void logOutput( Logtype _type , QString _message ) {};

   public:

      /// Destructor
      virtual ~LoggingInterface() {};
};

Q_DECLARE_INTERFACE(LoggingInterface,"OpenFlipper.LoggingInterface/1.0")
      
#endif // LOGGINGINTERFACE_HH
