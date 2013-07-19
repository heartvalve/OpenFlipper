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

#ifndef MENUBARINTERFACE_HH
#define MENUBARINTERFACE_HH


#if QT_VERSION >= 0x050000 
  #include <QtWidgets>
#else
  #include <QtGui>
#endif

#include <QMenuBar>
#include <OpenFlipper/common/Types.hh>

/// The Menu will be added inside the File Menu
#define FILEMENU tr("File")

/// The Menu will be added inside the View Menu
#define VIEWMENU tr("View")

/// The Menu will be added inside the Tools Menu
#define TOOLSMENU tr("Tools")

/// The Menu will be added inside the Algorithms Menu
#define ALGORITHMMENU tr("Algorithms")

/** \file MenuInterface.hh
*
* Interface for adding per plugin toolboxes to OpenFlippers UI.\ref menuInterfacePage
*/


/** \brief Interface for all plugins which provide entries to the main menubar
  *
  * \ref menuInterfacePage "Detailed description" 
  * \n
  * \n
  * To add custom menus or actions to the menubar, you have to use this interface class. Create
  * your own QMenu or QAction and emit addMenubarAction to add it to one of the menubar top level menus.
  * You can also get a pointer to one existing top level menu or create a new one with the getMenubarMenu
  * function. You can connect the signals and slots for your menu or action inside the plugin.
 */
class MenuInterface {

public :

  /// Destructor
  virtual ~MenuInterface() {};

signals:

   /**  \brief Get a existing top level menu pointer or create a new one
    *
    *  Checks if a top level menu is present and creates one if needed \n
    *
    * @param _name   Menu name (see FILEMENU/VIEWMENU/TOOLSMENU example defines or use other QStrings )
    * @param _menu   The returned top level menu
    * @param _create Should a new menu be created if id doesn't exist
  */
  virtual void getMenubarMenu (QString _name, QMenu *& _menu, bool _create) {};

  /**  \brief Adds an action to the menubar
    *
    *  Add an action to one of the menubar top level menus \n
    * \n
    *   Example : \n
    * \code
    *   QMenu *colorMenu = new QMenu(tr("&Colors")); 
    *   emit addMenubarAction( colorMenu->menuAction(), TOOLSMENU );
    * \endcode
    *
    * All actions or sub actions can be freely controlled by yourself. You have
    * to connect the required signals and slots to your plugin.
    *
    * @param _action Pointer to the new action
    * @param _name   Name of the menu
  */
  virtual void addMenubarAction(QAction* _action, QString _name ) {};

  /** \brief Add multiple actions to the menu bar.
   *
   * Does the same as multiple calls to addMenubarAction except
   * it doesn't insert a separator between the individual items.
   *
   * @param _actions Vector of pointers to the new actions.
   * @param _name Name of the menu.
   */
  virtual void addMenubarActions(std::vector<QAction*> &_actions, QString _name ) {};
};




/** \page menuInterfacePage Menu Interface
\image html MenuInterface.png
\n
The MenuInterface can be used by plugins to add menu entries to OpenFlippers
UI. The entries will be added to OpenFlippers menubar or submenus (See image). 

To use the MenuInterface:
<ul>
<li> include MenuInterface.hh in your plugins header file
<li> derive your plugin from the class MenuInterface
<li> add Q_INTERFACES(MenuInterface) to your plugin class 
<li> And add the signals or slots you want to use to your plugin class (You don't need to implement all of them)
</ul>

Usually you should implement the BaseInterface::pluginsInitialized() function from BaseInterface. In this function you can setup
your menus.

The following code shows a simple example to create a menu entry in the file menu.
\code 
void PrintPlugin::pluginsInitialized()
{
  // Create a submenu called printing
  QMenu *printMenu = new QMenu(tr("&Printing"));
  
  // Set an icon for this submenu
  printMenu->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"document-print.png"));
  
  // Add the new submenu to OpenFlippers file menu
  emit addMenubarAction(printMenu->menuAction(), FILEMENU );
  
  // Create an action to be added to the submenu
  QAction* AC_Print = new QAction(tr("&Print"), this);
  
  // Set status tip for this entry
  AC_Print->setStatusTip(tr("Print the current view"));
  
  // Set icon for the entry
  AC_Print->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"document-print.png"));
  
  // connect the actions triggered slot to a local slot in this plugin.
  connect(AC_Print, SIGNAL(triggered()), this, SLOT(printView()));
  
  // add the new action to the print submenu
  printMenu->addAction(AC_Print);
}
\endcode

If you want to create a toplevel menu you can use the following code snippet:
\code
  //Pointer to the new menu
  QMenu *topLevelMenu;

  // This function checks if a menu called "Toplevel" exists and returns it in the pointer.
  // If it does not exist, it is automatically created.
  emit getMenubarMenu(tr("Toplevel"), topLevelMenu, true );
\endcode

Signals and slots of your menus (e.g. from an action inside it) can be directly connected to signals and slots in
your plugin. Therefore the embedding of your menus into the OpenFlippers menu list is fully transparent.

*/

Q_DECLARE_INTERFACE(MenuInterface,"OpenFlipper.MenuInterface/1.0")

#endif // MENUBARINTERFACE_HH
