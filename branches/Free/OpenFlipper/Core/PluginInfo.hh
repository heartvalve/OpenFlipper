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


#ifndef PLUGININFO_HH
#define PLUGININFO_HH

#include <QString>
#include <QObject>
#include <QStringList>
#include <QList>
#include <QDockWidget>

struct KeyBinding{
  int                   key;
  Qt::KeyboardModifiers modifiers;
  QString               description;
  bool                  multiUse;
  bool                  slot;
};

struct SlotInfo{
  QString slotName;
  QString slotDescription;
  QStringList parameters;
  QStringList descriptions;
};

/** Type defining a currently loaded Plugin */
class PluginInfo{

  public :

  PluginInfo() {
     plugin = 0;
     name = "";
     description = "";
     version = "";
     path = "";
     rpcName = "";
     rpcFunctions.clear();
     slotInfos.clear();
     keys.clear();
     toolboxWidgets.clear();
     toolboxIcons.clear();
     toolbars.clear();
     contextMenus.clear();
     optionsWidget = 0;
     buildIn = true;
     status = FAILED;
  }



  PluginInfo( const PluginInfo& _i) {
     plugin = _i.plugin;
     name = _i.name;
     description = _i.description;
     version = _i.version;
     path = _i.path;
     rpcName = _i.rpcName;
     rpcFunctions = _i.rpcFunctions;
     slotInfos = _i.slotInfos;
     keys = _i.keys;
     toolboxWidgets = _i.toolboxWidgets;
     toolboxIcons = _i.toolboxIcons;
     toolbars = _i.toolbars;
     contextMenus = _i.contextMenus;
     optionsWidget = _i.optionsWidget;
     buildIn = _i.buildIn;
     status = _i.status;
  }

  /// Pointer to the loaded plugin (Already casted when loading it)
  QObject*    plugin;

  /// Name of the plugin ( requested from the plugin on load)
  QString     name;

  /// Description of the plugin ( requested from the plugin on load)
  QString     description;

  /// Version of the plugin
  QString     version;

  /// Path to the plugin ( set on load )
  QString     path;

  /// Clean rpc name of the plugin
  QString     rpcName;

  /// List of exported rpc slots
  QStringList rpcFunctions;

  /// This list contains Descriptions about public slots if available
  QList< SlotInfo > slotInfos;

  /// List of registered keys with description
  QList< KeyBinding > keys;

  /// Pointer to plugins toolbox widget (if available)
  std::vector< std::pair< QString , QWidget* > > toolboxWidgets;

  /// Pointer to plugins toolbox widget icons (if available)
  std::vector< QIcon* > toolboxIcons;
  
  /// Pointer to plugins toolbars (if available)
  std::vector< std::pair< QString , QToolBar* > > toolbars;
  
  /// Pointer to plugins context menus (if available)
  std::vector< std::pair< QString , QAction* > > contextMenus;

  /// Pointer to plugins options widget (if available)
  QWidget* optionsWidget;

  /// Indicates, if the plugin is a built in Plugin (in Plugin directory)
  bool buildIn;

  /// current status of the plugin
  enum StatusType
  {
    LOADED = 0,
    BLOCKED,
    UNLOADED,
    FAILED
  }status;


};

#endif //PLUGININFO_HH
