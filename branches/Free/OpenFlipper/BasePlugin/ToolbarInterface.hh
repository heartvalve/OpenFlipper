/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2011 by Computer Graphics Group, RWTH Aachen       *
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

#ifndef TOOLBARINTERFACE_HH
#define TOOLBARINTERFACE_HH

 
#if QT_VERSION >= 0x050000 
  #include <QtWidgets>
#else
  #include <QtGui>
#endif

 #include <QMenuBar>
 #include <QStringList>
 #include <OpenFlipper/common/Types.hh>
 #include <QToolBar>

/** \file ToolbarInterface.hh
*
* Interface for adding per plugin toolbars to OpenFlippers UI.\ref toolbarInterfacePage
*/


/** \brief Add a toolbox to OpenFlipper
 *
 * \ref toolbarInterfacePage "Detailed description"
 * \n
 *
 * This Interface should be used by plugins which will provide a toolbar.
 * Each Plugin can create own toolbars.
*/
class ToolbarInterface {

   private slots :

   public :

      /// Destructor
      virtual ~ToolbarInterface() {};

    signals :

      /** \brief Adds a Toolbar to the main widget
      *
      * To create your own Toolbar, emit this signal with a pointer to your Toolbar.
      * @param _toolbar Your Toolbar
      */
      virtual void addToolbar(QToolBar* _toolbar) {};


      /** \brief Remove a Toolbar from the main widget
      *
      * @param _toolbar Toolbar to be removed
      */
      virtual void removeToolbar(QToolBar* _toolbar) {};


      /** \brief Get a pointer to a Toolbar of the given name or 0 if it does not exist.
      *
      * @param _name Name of the Toolbar
      * @param _toolbar requested Toolbar or 0
      */
      virtual void getToolBar( QString _name, QToolBar*& _toolbar ) {};


};



/** \page toolbarInterfacePage Toolbar Interface
\image html ToolbarInterface.png
\n
The ToolbarInterface can be used by plugins to add toolbars to OpenFlippers
UI. The toolbars are located above the GL viewer (See image).


To use the ToolbarInterface:
<ul>
<li> include ToolbarInterface in your plugins header file
<li> derive your plugin from the class ToolbarInterface
<li> add Q_INTERFACES(ToolbarInterface) to your plugin class
<li> And add the signals or slots you want to use to your plugin class (You don't need to implement all of them)
</ul>

Usually you should implement the BaseInterface::pluginsInitialized() function from BaseInterface. In this function you can setup
your toolbars.

The following code shows a simple example to create a simple toolbar.
\code
void ExamplePlugin::pluginsInitialized()
{

  // Create your toolbar
  QToolBar* toolbar = new QToolBar(tr("Example Toolbar"));

  // Create an action for the toolbar
  QAction* exampleAction = new QAction(tr("&Toolbar Button"), this);

  // Create an icon which is shown for the action
  exampleAction->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"exampleIcon.png"));

  // Add the action to the toolbar
  toolbar->addAction(exampleAction);

  // Integrate the new toolbar into OpenFlipper
  emit addToolbar( toolbar );
}
\endcode

Signals and slots of your toolbar (e.g. from an action inside it) can be directly connected to signals and slots in
your plugin. Therefore the embedding of the toolbar is fully transparent.

*/



Q_DECLARE_INTERFACE(ToolbarInterface,"OpenFlipper.ToolbarInterface/1.1")

#endif // TOOLBARINTERFACE_HH
