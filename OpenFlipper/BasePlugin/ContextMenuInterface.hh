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

#ifndef CONTEXTMENUINTERFACE_HH 
#define CONTEXTMENUINTERFACE_HH 

#include <QtGui>
#include <QMenuBar>
#include <ACG/QtWidgets/QtExaminerViewer.hh>
#include <OpenFlipper/common/Types.hh>
 
enum ContextMenuType {
    /// The Menu will be created directly inside the Menu.
    CONTEXTTOPLEVELMENU, 
    /// The Menu will be added inside the Selection part of the Context Menu
    CONTEXTSELECTIONMENU
};
 
 /** \brief Interface class for creating custom context menus
  * 
  * Using this interface you can create custom context menus for your plugin. You
  * can choose between a persistent context menu which will be visible every time
  * you rightclick in the viewerwindow or an object based context menu. This type 
  * of menu will only show up when the user clicked on a specific type defined by 
  * your plugin.\n
  * You have to create a QMenu. The signals and slots of your menu have to be connected
  * to your plugin. Just connect them as usual. Only visibility of the menu is handled
  * by the core.
 */
class ContextMenuInterface {
  
public : 
      
  /// Destructor
  virtual ~ContextMenuInterface() {};
    
signals:
  
  /**  \brief Add an entry for a persistent context Menu
    * 
    * Create an QMenu and register this menu as a persistent context menu to the core.
    * This Menu will be always visible  when you rightclick in the viewer widget.
    * @param _menu Pointer to the new Menu
  */
  virtual void addContextMenu(QMenu* _menu , ContextMenuType _type) {};
  
  /**  \brief Add an entry for a  context Menu
    * 
    * Create an QMenu and register this menu as a context menu to the core.
    * This Menu will only be visible if the picked object is of the given datatype.
    * To support multiple object types with your menu, you can emit this signal multiple
    * times with the same menu but different DataTypes
    * 
    * @param _menu Pointer to the new Menu
    * @param _objectType Type of the picked object
   *  @param _type Type of the context Menu ( See ContextMenuType )
  */
  virtual void addContextMenu(QMenu* _menu ,DataType _objectType , ContextMenuType _type ) {};
  
private slots:
  
  /** When the main application requests a context menu, this slot is called before showing the window.
   * If an object is picked the id will be given in this call so you can change the contents of your menu
   * depending on the given object,
   * @param _objectId id of the object or -1 if nothing picked
   */
  virtual void slotUpdateContextMenu( int _objectId ) {};

};

Q_DECLARE_INTERFACE(ContextMenuInterface,"OpenFlipper.ContextMenuInterface/0.3")
      
#endif // CONTEXTMENUINTERFACE_HH
