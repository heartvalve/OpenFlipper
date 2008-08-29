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
#include <ACG/QtWidgets/QtExaminerViewer.hh>
#include <OpenFlipper/common/Types.hh>
 
enum MenuType {
    /// The Menu will be created directly inside the Menubar.
    TOPLEVELMENU, 
    
    /// The Menu will be added inside the File Menu
    FILEMENU,
    
    /// The Menu will be added inside the View Menu
    VIEWMENU
        
};
 
/** \brief Interface for all plugins which provide entries to the main menubar
  * 
  * To add custom menus to the menubar, you have to use this interface class. Create
  * your own QMenu and emit initializeMenu to add it to the menubar. You can connect the 
  * signals and slots for your menu inside the plugin.\n
  * The placement of your menu is controlled by the MenuType. See MenuType for details.
 */ 
class MenuInterface {
  
public : 
      
  /// Destructor
  virtual ~MenuInterface() {};
    
signals:
  /**  \brief Initialize the Menubar
    * 
    *  Initialize a Menu (create a new menu in the main menubar) \n
    * \n
    *   Example : \n
    *   QMenu *colorMenu = new QMenu(tr("&Colors")); \n
    *   _menu = colorMenu;\n
    * \n
    * In the menu you can add anything you want.
    * @param _menu Pointer to the new Menu
    * @param _type Type of the Menu ( See MenuType for Details )
  */
  virtual void initializeMenu(QMenu* _menu, MenuType _type ) {};

};

Q_DECLARE_INTERFACE(MenuInterface,"OpenFlipper.MenuInterface/0.3")
      
#endif // MENUBARINTERFACE_HH
