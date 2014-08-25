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

#ifndef STATUSBARINTERFACE_HH
#define STATUSBARINTERFACE_HH

#include <OpenFlipper/common/Types.hh>

#if QT_VERSION >= 0x050000 
  #include <QtWidgets>
#else
  #include <QtGui>
#endif

#include <QStatusBar>


/** \file StatusbarInterface.hh
*
* Interface for controlling OpenFlippers statusbar .\ref statusbarInterfacePage
*/

/** Namespace for Application status enum
 *
 */
namespace ApplicationStatus {
  
  /** \brief Enum for the statusBar Status Icon
   *
   * This enum is used to controll the small activity icon of OpenFlipper.
   * See \ref statusbarInterfacePage
   */
  enum applicationStatus {
    READY,     //!< Status is ready (green light)
    PROCESSING,//!< Status is processing but system will allow interaction (yellow light)
    BLOCKED    //!< Status is processing and blocked system will not allow interaction (red light)
  };

}


 /** \brief Control OpenFlippers status bar
 *
 * \ref statusbarInterfacePage "Detailed description"
 * \n
 *
 * This interface can be used to send messages to OpenFlippers statusbar.
 * It is also possible to add your own widget to the bar.
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
      
      
      /** \brief Sets the status icon of the StatusBar
       *
       * OpenFlipper statusbar contains a small icon in the statusbar which can be used to indicate
       * the current status of the application. The valid values are defined in the enum ApplicationStatus::applicationStatus.
       * 
       * @param _status New status
       */
      void setStatus( ApplicationStatus::applicationStatus _status) {};
      
      /** \brief Removes the current message from the statusBar
       * 
       */
      void clearStatusMessage() {};
      
      /** \brief Add a widget on the right side of the statusBar
       * 
       * Use this function if you want to add an additional widget to the statusbar.
       * It will be added on the right side.
       *
       * @param _widget The widget that should be added to the statusbar
       */
      void addWidgetToStatusbar(QWidget* _widget) {};
      
};


/** \page statusbarInterfacePage Statusbar Interface
\image html StatusbarInterface.png
\n
The StatusbarInterface can be used by plugins to send messages to OpenFlippers
statusbar. The statusbar is located below the GL viewer.

Messages can be controlled via StatusbarInterface::setStatus(),StatusbarInterface::clearStatusMessage()

Additionally it is possible to add small widgets to it ( StatusbarInterface::addWidgetToStatusbar).

A small icon can be controlled to indicate OpenFlippers status via StatusbarInterface::setStatus().

To use the StatusbarInterface:
<ul>
<li> include StatusbarInterface.hh in your plugins header file
<li> derive your plugin from the class StatusbarInterface
<li> add Q_INTERFACES(StatusbarInterface) to your plugin class
<li> And add the signals or slots you want to use to your plugin class (You don't need to implement all of them)
</ul>

The following code shows a simple example to send a message to the statusbar.
\code
void ExamplePlugin::function()
{
  ...

  // Print the text "Your Status Message" for 4 seconds in the statusBar
  emit showStatusMessage( tr("Your Status Message"), 4000 );

  ...
}
\endcode

The following code shows a simple example to add a widget to the statusbar.
Usually you should implement the BaseInterface::pluginsInitialized() function from BaseInterface.
In this function you can setup your widget for the status bar.
\code
void ExamplePlugin::pluginsInitialized()
{

  // Create the widget
  QWidget* infoBar = new QWidget();

  // Setup buttons or anything else in your widget
  ...

  // Add the new widget to the statusBar
  emit addWidgetToStatusbar(infoBar);

}
\endcode

*/



Q_DECLARE_INTERFACE(StatusbarInterface,"OpenFlipper.StatusbarInterface/1.0")

#endif // STATUSBARINTERFACE_HH
