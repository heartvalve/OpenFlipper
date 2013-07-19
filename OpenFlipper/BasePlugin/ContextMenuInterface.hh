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


#ifndef CONTEXTMENUINTERFACE_HH
#define CONTEXTMENUINTERFACE_HH


#if QT_VERSION >= 0x050000 
  #include <QtWidgets>
#else
  #include <QtGui>
#endif

#include <QMenuBar>
#include <OpenFlipper/common/Types.hh>


/** \file ContextMenuInterface.hh
*
* Interface for adding context menus to OpenFlippers UI from a plugin.\ref contextMenuInterfacePage
*/


/** Enum to specify the type of context menus
*/
enum ContextMenuType {
  
  /// The Menu will be shown when an object was picked
  CONTEXTOBJECTMENU,   
  
  /// The Menu will be shown when a node was picked
  CONTEXTNODEMENU,      
  
  /// The Menu will be shown when the background was picked
  CONTEXTBACKGROUNDMENU 
};




/** \brief Interface class for creating custom context menus
  *
  * \ref contextMenuInterfacePage "Detailed description"
  * \n
  * \n
  * Using this interface you can create custom context menus for your plugin. You
  * can choose between context menus for objects, nodes or the background.\n
  * Before a menu of the requested type is shown, an update function for the specific type
  * will be invoked by the core.
  * You have to create a QAction. The signals and slots of your actions have to be connected
  * to your plugin. Just connect them as usual. Only visibility of the menu is handled
  * by the core. You can also add submenus to the context menus. Just add the action for
  * the menu ( menu->menuAction() )
*/
class ContextMenuInterface {

public :

  /// Destructor
  virtual ~ContextMenuInterface() {};

signals:

  /**  \brief Add an entry for a context Menu
    *
    * Create an Action (Can also be the action of a Menu) and register this menu as a context menu to the core.
    * This Action will be visible  when you right click in the viewer widget on an item
    * of the given context menu type. You can add a whole menu here by adding the action:
    * menu->menuAction() of your own menu.
    *
    * @param _action Pointer to the new Action
    * @param _type   Type context menu type
  */
  virtual void addContextMenuItem(QAction* _action , ContextMenuType _type) {};

  /**  \brief Add an entry for a  context Menu
    *
    * Create an action (Can also be the action of a Menu) and register this action as a context menu entry to the core.
    * This Action will only be visible if the picked object is of the given DataType.
    * To support multiple object types with your menu, you can emit this signal multiple
    * times with the same action but different DataTypes. You can add a whole Menu here by adding the action:
    * menu->menuAction()
    *
    * @param _action Pointer to the new action
    * @param _objectType Type of the picked object
   *  @param _type Type of the context Menu ( See ContextMenuType )
  */
  virtual void addContextMenuItem(QAction* _action ,DataType _objectType , ContextMenuType _type ) {};

  /// hide the main context menu
  virtual void hideContextMenu() {};

private slots:

  /** When the main application requests a context menu, this slot is called before showing the window.
   * If an object is picked the id will be given in this call so you can change the contents of your menu
   * depending on the given object.
   *
   * @param _objectId id of the object
   */
  virtual void slotUpdateContextMenu( int _objectId ) {};

  /** When the main application requests a context menu, this slot is called before showing the window.
   * This slot will be called indicating that a scenegraph node not belonging to an object
   * has been picked.
   *
   * @param _nodeId id of the picked node
   */
  virtual void slotUpdateContextMenuNode( int _nodeId ) {};

  /** When the main application requests a context menu, this slot is called before showing the window.
   * This slot will be called indicating that the background has been picked.
   *
   */
  virtual void slotUpdateContextMenuBackground( ) {};

};


/** \page contextMenuInterfacePage Context Menu Interface
\image html ContextMenuInterface.png
\n
The ContextMenuInterface can be used by plugins to add menu entries to OpenFlippers
UI. The entries will be added to OpenFlippers contextMenus in the glView (see image). 
You can choose between context menus for objects, nodes or the background.\n

The given context menu entries will be shown when the user right clicks on the corresponding
primitive (node,object) in the GL viewer.

To use the ContextMenuInterface:
<ul>
<li> include ContextMenuInterface.hh in your plugins header file
<li> derive your plugin from the class ContextMenuInterface
<li> add Q_INTERFACES(ContextMenuInterface) to your plugin class 
<li> And add the signals or slots you want to use to your plugin class (You don't need to implement all of them)
</ul>


Usually you should implement the BaseInterface::pluginsInitialized() function from BaseInterface. In this function you can setup
your menus.\n

You have to create a QAction. The signals and slots of your actions have to be connected
to your plugin. Just connect them as usual. Only visibility of the menu is handled
by the core. You can also add submenus to the context menus. Just add the action for
the menu ( menu->menuAction() )

Before a menu of the requested type is shown, an update function for the specific type
will be invoked by the core depending on your type of context menu 
( ContextMenuInterface::slotUpdateContextMenu(), ContextMenuInterface::slotUpdateContextMenuNode(), ContextMenuInterface::slotUpdateContextMenuBackground() ). 
In this function you can update entries based on the object that was clicked on. The id of the node
or the object is provided by these update functions.


The following code shows a simple example to create a menu entry in the context menu.
\code 

// Setup the menus in initialize plugins
void ExamplePlugin::pluginsInitialized()
{
  // create a global QMenu in the plugin that contains our menu
  // in the header is: 
  // QMenu* contextMenu_
  contextMenu_ = new QMenu(tr("Select"));
  
  // Create a menu action called all in the Selection context menu
  // in the header is:   
  // QAction* menuAction_
  menuAction_ = contextMenu_->addAction( tr("All Mesh vertices") );
  menuAction->setToolTip(tr("Select all"));
  
  // Add the new menu to OpenFlippers context menu fot objects
  // Show the context menu for triangle meshes
  emit addContextMenuItem(contextMenu_->menuAction() , DATA_TRIANGLE_MESH , CONTEXTOBJECTMENU );
  
  // Show the context menu for poly meshes meshes
  emit addContextMenuItem(contextMenu_->menuAction() , DATA_POLY_MESH     , CONTEXTOBJECTMENU );
  }
  
  
// Example function to update the context menu based on the given object id
void ExamplePlugin::slotUpdateContextMenu( int _objectId ){
  
  // Get the corresponding object of the given id or return
  BaseObjectData* object = 0;
  if ( !PluginFunctions::getObject( _objectId, object ) ) {
    return;
  }
  
  // If its a triangle mesh, rename the to triangle mesh selection
  if (object->dataType( DATA_TRIANGLE_MESH ) ) 
    menuAction_->setText("All Triangle mesh vertices");
  
  // If its a triangle mesh, rename the to poly mesh selection
  if ( object->dataType( DATA_POLY_MESH ) )  
    menuAction_->setText("All Triangle mesh vertices");    
  
}


}
\endcode


Signals and slots of your menus (e.g. from an action inside it) can be directly connected to signals and slots in
your plugin. Therefore the embedding of your menus into the OpenFlippers context menu list is fully transparent.

*/

Q_DECLARE_INTERFACE(ContextMenuInterface,"OpenFlipper.ContextMenuInterface/1.0")

#endif // CONTEXTMENUINTERFACE_HH
