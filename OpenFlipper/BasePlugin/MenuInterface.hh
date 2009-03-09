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
// C++ Interface: BasePlugin
//
// Description:
//
//
// Author: Jan Moebius <jan_moebius@web.de>, (C) 2007
//

#ifndef MENUBARINTERFACE_HH
#define MENUBARINTERFACE_HH

#include <QtGui>
#include <QMenuBar>
#include <OpenFlipper/common/Types.hh>

enum MenuActionType {
    /// The Menu will be created directly inside the Menubar.
    TOPLEVELMENU,

    /// The Menu will be added inside the File Menu
    FILEMENU,

    /// The Menu will be added inside the View Menu
    VIEWMENU,

    /// The Menu will be added inside the Tools Menu
    TOOLSMENU

};

/** \brief Interface for all plugins which provide entries to the main menubar
  *
  * To add custom menus or action to the menubar and its submenus, you have to use this interface class. Create
  * your own QMenu or QAction and emit addMenubarAction to add it to the menubar. You can connect the
  * signals and slots for your menu or action inside the plugin.\n
  * The placement of your menu in the global menubar is controlled by the MenuActionType. See MenuActionType for details.
 */
class MenuInterface {

public :

  /// Destructor
  virtual ~MenuInterface() {};

signals:
  /**  \brief Adds an action to the menubar
    *
    *  Add an action to the menubar or one of its sub menus \n
    * \n
    *   Example : \n
    *   QMenu *colorMenu = new QMenu(tr("&Colors")); \n
    *   emit addMenubarAction( colorMenu->menuAction(), TOOLSMENU )
    * \n
    * All actions or sub actions can be freely controlled by yourself. You have
    * to connect the required signals and slots to your plugin.
    *
    * @param _action Pointer to the new action
    * @param _type Type of the Action ( See MenuActionType for Details )
  */
  virtual void addMenubarAction(QAction* /*_action*/, MenuActionType /*_type*/ ) {};

};

Q_DECLARE_INTERFACE(MenuInterface,"OpenFlipper.MenuInterface/1.0")

#endif // MENUBARINTERFACE_HH
