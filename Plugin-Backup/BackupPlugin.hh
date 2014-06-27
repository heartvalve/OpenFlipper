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

#ifndef BACKUPPLUGIN_HH
#define BACKUPPLUGIN_HH

#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/KeyInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>
#include <OpenFlipper/BasePlugin/MenuInterface.hh>
#include <OpenFlipper/BasePlugin/BackupInterface.hh>
#include <OpenFlipper/BasePlugin/ToolbarInterface.hh>
#include <OpenFlipper/BasePlugin/LoadSaveInterface.hh>
#include <OpenFlipper/BasePlugin/ContextMenuInterface.hh>
#include <OpenFlipper/common/Types.hh>

#include <QObject>
#include <QMenuBar>

#include "GroupData.hh"

class BackupPlugin : public QObject, BaseInterface , KeyInterface, MenuInterface, BackupInterface, LoggingInterface, ToolbarInterface, LoadSaveInterface, ContextMenuInterface
{
Q_OBJECT
Q_INTERFACES(BaseInterface)
Q_INTERFACES(KeyInterface)
Q_INTERFACES(MenuInterface)
Q_INTERFACES(BackupInterface)
Q_INTERFACES(LoggingInterface)
Q_INTERFACES(ToolbarInterface)
Q_INTERFACES(LoadSaveInterface)
Q_INTERFACES(ContextMenuInterface)

#if QT_VERSION >= 0x050000
  Q_PLUGIN_METADATA(IID "org.OpenFlipper.Plugins.Plugin-Backup")
#endif

signals:
  // BaseInterface
  void updateView();
  void updatedObject(int _identifier, const UpdateType& _type);
  
  //  LoggingInterface
  void log(Logtype _type, QString _message);
  void log(QString _message);

  // MenuInterface
  void getMenubarMenu (QString _name, QMenu *& _menu, bool _create);
  
  // BackupInterface
  void undo(int _objectid);
  void undo();
  void redo(int _objectid);
  void redo();
  void aboutToRestore(int _objectid);
  void restored(int _objectid);
  void generateBackup( int _id, QString _name, UpdateType _type);
  
  // ToolBarInterface
  void addToolbar(QToolBar* _toolbar);
  void getToolBar( QString _name, QToolBar*& _toolbar);
  
  //KeyInterface
  void registerKey(int _key, Qt::KeyboardModifiers _modifiers, QString _description, bool _multiUse = false);

  //ContextMenuInterface
  void addContextMenuItem(QAction* _action , DataType _objectType, ContextMenuType _type);
  
private slots:

  // BaseInterface
  void pluginsInitialized();
  
  void slotAllCleared();
  
  // KeyInterface
  void slotKeyEvent( QKeyEvent* _event );
  
  // LoadSaveInterface
  void objectDeleted (int _objectid);
   
  // Backup Interface
  void slotCreateBackup( int _objectid, QString _name, UpdateType _type = UPDATE_ALL);
  void slotCreateBackup( IdList _objectids , QString _name, std::vector<UpdateType> _types);
  void slotUndo(int _objectid);
  void slotRedo(int _objectid);
  void slotUndo();
  void slotRedo();

  //ContextMenuInterface
  void slotUpdateContextMenu( int _objectId );

private slots:
  
  void updateButtons();
  
  /// Backups enabled or disabled checkbox
  void slotEnableDisableBackups();

  void slotObjectUndo();
  void slotObjectRedo();

public slots:
  QString version() { return QString("0.1"); };  
  
public:
  ~BackupPlugin() {};
  BackupPlugin();
  
  QString name() { return (QString("Backup")); };
  QString description( ) { return (QString("Creates Backups of objects when supported by plugins")); };

private :
  GroupData globalBackup_;

  /// The backup Menu
  QMenu*  backupMenu_;

  QAction* backupsEnabledAction_;

  QAction* undoMenuAction_;
  QAction* redoMenuAction_;

  QAction* undoToolAction_;
  QAction* redoToolAction_;
  
  QAction* undoContextAction_;
  QAction* redoContextAction_;
};

#endif //BACKUPPLUGIN_HH
