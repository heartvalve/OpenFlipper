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

#ifndef MENUBARINTERFACE_HH
#define MENUBARINTERFACE_HH

#include <QtGui>
#include <QMenuBar>
#include <OpenFlipper/common/Types.hh>

// typedef to support old interface
typedef QString MenuActionType;

/// The Menu will be added inside the File Menu
#define FILEMENU tr("File")

/// The Menu will be added inside the View Menu
#define VIEWMENU tr("View")

/// The Menu will be added inside the Tools Menu
#define TOOLSMENU tr("Tools")


/** \brief Interface for all plugins which provide entries to the main menubar
  *
  * To add custom menus or actions to the menubar, you have to use this interface class. Create
  * your own QMenu or QAction and emit addMenubarAction to add it to one of the menubar toplevel menus.
  * You can also get a pointer to one existing toplevel menus or create a new one with the getMenubarMenu
  * function. You can connect the signals and slots for your menu or action inside the plugin.
 */
class MenuInterface {

public :

  /// Destructor
  virtual ~MenuInterface() {};

signals:

   /**  \brief Get a existing toplevel menu pointer or create a new one
    *
    *  Checks if a toplevel menu is present and creates one if needed \n
    *
    * @param _name Menu name (see FILEMENU/VIEWMENU/TOOLSMENU example defines)
    * @param _menu The returned toplevel menu
    * @param _create Should a new menu be created if id doesn't exist
  */
  virtual void getMenubarMenu (QString /*_name*/, QMenu *& /*_menu*/, bool /*_create*/) {};

  /**  \brief Adds an action to the menubar
    *
    *  Add an action to one of the menubar toplevel menus \n
    * \n
    *   Example : \n
    *   QMenu *colorMenu = new QMenu(tr("&Colors")); \n
    *   emit addMenubarAction( colorMenu->menuAction(), TOOLSMENU )
    * \n
    * All actions or sub actions can be freely controlled by yourself. You have
    * to connect the required signals and slots to your plugin.
    *
    * @param _action Pointer to the new action
    * @param _name Name of the menu
  */
  virtual void addMenubarAction(QAction* /*_action*/, QString /*_name*/ ) {};
};

Q_DECLARE_INTERFACE(MenuInterface,"OpenFlipper.MenuInterface/1.0")

#endif // MENUBARINTERFACE_HH
