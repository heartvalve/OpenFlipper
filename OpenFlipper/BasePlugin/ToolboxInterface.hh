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

#ifndef TOOLBOXINTERFACE_HH
#define TOOLBOXINTERFACE_HH

#include <OpenFlipper/common/Types.hh>
#if QT_VERSION >= 0x050000 
  #include <QtWidgets>
#else
  #include <QtGui>
#endif

#include <QMenuBar>
#include <QStringList>


/** \file ToolboxInterface.hh
*
* Interface for adding per plugin toolboxes to OpenFlippers UI.\ref toolboxInterfacePage
*/


 /**
  * \brief Plugins can add its own toolbox to the main widget's toolbox area by using this interface.
  *
  * \ref toolboxInterfacePage "Detailed description" 
  * \n
  *
  * This Interface can be used by plugins which will provide a toolbox widget.
  * Each Plugin can create own Widgets in the toolbox area. Just create your widget and then add it with \n
  * emit addToolbox( QString  _name  , QWidget* _widget ) \n
  * to the user interface. You can create extra signals and slots in your Plugin.
  * These can be connected between your toolbox and your plugin.\n
  *
  * See our tutorials \ref ex2 and \ref ex3 for an example of to add custom toolboxes to
  * OpenFlipper.
  */
class ToolboxInterface {

   private slots :

   public :

      /// Destructor
      virtual ~ToolboxInterface() {};

    signals :

      /** \brief Add a toolbox widget to the gui with the given name
       *
       * This signal adds a toolbox widget to the toolbox area on the right.
       *
       * @param _name   Visible name of the toolbox
       * @param _widget Pointer to the toolbox widget
       */
      virtual void addToolbox( QString  _name  , QWidget* _widget ) {};

      /** \brief Add a toolbox widget to the gui with the given name and an icon
       *
       * This signal adds a toolbox widget to the toolbox area on the right. And sets an icon for it
       *
       * @param _name   Visible name of the toolbox
       * @param _widget Pointer to the toolbox widget
       * @param _icon   Icon for the toolbox
       */
      virtual void addToolbox( QString  _name  , QWidget* _widget, QIcon* _icon) {};
};



/** \page toolboxInterfacePage Toolbox Interface
\image html ToolboxInterface.png

The ToolboxInterface can be used by plugins to add widgets to the list of toolboxes in OpenFlippers
UI. The toolboxes are located left or right of the GL viewer (See image).

The list can be hidden by pressing Ctrl + t.

To use the ToolboxInterface:
<ul>
<li> include ToolboxInterface.hh in your plugins header file
<li> derive your plugin from the class ToolboxInterface
<li> add Q_INTERFACES(ToolboxInterface) to your plugin class 
<li> And add the signals or slots you want to use to your plugin class (You don't need to implement all of them)
</ul>

Usually you should implement the BaseInterface::pluginsInitialized() function from BaseInterface. In this function you can setup
your toolbox ( some kind of QWidget, can also be any widget created with QT-designer). Optionally you can also
add an icon to the toolbox. 

The following code shows a simple example to create an empty toolbox.
\code 
void ExamplePlugin::pluginsInitialized()
{
  // Create the Toolbox Widget
  QWidget* toolBox = new QWidget();
  
  // Create an icon which is shown along with the toolbox
  QIcon* toolIcon = new QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"exampleIcon.png");
  
  // Tell the core to include the new toolbox widget with the name Simple Smoother, a pointer to the widget and a pointer to the icon
  emit addToolbox( tr("Example Plugin Toolbox") , toolBox, toolIcon );
}
\endcode

Signals and slots of your toolbox (e.g. from a QPushButton inside it) can be directly connected to signals and slots in
your plugin. Therefore the embedding of the widget into the toolbox list is fully transparent.

*/

Q_DECLARE_INTERFACE(ToolboxInterface,"OpenFlipper.ToolboxInterface/1.1")

#endif // TOOLBOXINTERFACE_HH
