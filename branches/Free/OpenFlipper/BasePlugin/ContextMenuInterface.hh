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
// C++ Interface: BasePlugin
//
// Description:
//
//
// Author: Jan Moebius <jan_moebius@web.de>, (C) 2007
//

#ifndef CONTEXTMENUINTERFACE_HH
#define CONTEXTMENUINTERFACE_HH

#include <QtGui>
#include <QMenuBar>
#include <OpenFlipper/common/Types.hh>

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
  * Using this interface you can create custom context menus for your plugin. You
  * can choose between context menus for objects nodes or the background.\n
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
    * This Action will be visible  when you rightclick in the viewer widget on an item
    * of the given context menu type. You can add a whole menu here by adding the action:
    * menu->menuAction() of your own menu.
    * @param _menu Pointer to the new Action
  */
  virtual void addContextMenuItem(QAction* /*_action*/ , ContextMenuType /*_type*/) {};

  /**  \brief Add an entry for a  context Menu
    *
    * Create an action (Can also be the action of a Menu) and register this action as a context menu entry to the core.
    * This Action will only be visible if the picked object is of the given datatype.
    * To support multiple object types with your menu, you can emit this signal multiple
    * times with the same action but different DataTypes. You can add a whole Menu here by adding the action:
    * menu->menuAction()
    *
    * @param _action Pointer to the new action
    * @param _objectType Type of the picked object
   *  @param _type Type of the context Menu ( See ContextMenuType )
  */
  virtual void addContextMenuItem(QAction* /*_action*/ ,DataType /*_objectType*/ , ContextMenuType /*_type*/ ) {};

private slots:

  /** When the main application requests a context menu, this slot is called before showing the window.
   * If an object is picked the id will be given in this call so you can change the contents of your menu
   * depending on the given object.
   * @param _objectId id of the object
   */
  virtual void slotUpdateContextMenu( int /*_objectId*/ ) {};

  /** When the main application requests a context menu, this slot is called before showing the window.
   * This slot will be called indicating that a scenegraph node not belonging to an object
   * has been picked.
   * The id of the node is given as a parameter
   * @param _node id of the picked node
   */
  virtual void slotUpdateContextMenuNode( int /*_nodeId*/ ) {};

  /** When the main application requests a context menu, this slot is called before showing the window.
   * This slot will be called indicating thatthe background has been picked.
   * The id of the node is given as a parameter
   * @param _node id of the picked node
   */
  virtual void slotUpdateContextMenuBackground( ) {};

};

Q_DECLARE_INTERFACE(ContextMenuInterface,"OpenFlipper.ContextMenuInterface/1.0")

#endif // CONTEXTMENUINTERFACE_HH
