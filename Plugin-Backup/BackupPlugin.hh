/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2010 by Computer Graphics Group, RWTH Aachen       *
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

#include <QObject>
#include <QMenuBar>

#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/KeyInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>
#include <OpenFlipper/BasePlugin/MenuInterface.hh>
#include <OpenFlipper/BasePlugin/BackupInterface.hh>
#include <OpenFlipper/BasePlugin/ToolbarInterface.hh>
#include <OpenFlipper/BasePlugin/LoadSaveInterface.hh>
#include <OpenFlipper/common/Types.hh>
#include <ObjectIDPointerManagerT.hh>

#include <ObjectTypes/PolyMesh/PolyMesh.hh>
#include <ObjectTypes/TriangleMesh/TriangleMesh.hh>

#ifdef ENABLE_SKELETON_SUPPORT
#include <ObjectTypes/Skeleton/Skeleton.hh>
#endif

#include <QGroupBox>
#include <queue>

class BackupContainerBase {
  
  public:
    BackupContainerBase() :
      objectId(-1),
      backupName("NONE"),
      backupNumber(-1),
      persistent(false)
      {}
      
    virtual ~BackupContainerBase() {
    }
    
    void clear() {
      // Delete everything in our per Object Container
      QMapIterator<QString, PerObjectData* > i (perObjectDatas);
      while (i.hasNext()) {
        i.next();
        delete i.value();
      }
      
      perObjectDatas.clear();
    }
    
    // Id of the object backed up
    int objectId;
  
    // Human readable Name of the Backup
    QString backupName;
    
    // Unique number for the given Backup
    int backupNumber;
    
    // Persistence flag ( will not be automatically deleted
    bool persistent;
    
    // Stores the per Object data objects per Backup
    QMap< QString, PerObjectData* > perObjectDatas;
  
};

class BackupT: public BackupContainerBase{
  
  public:
    BackupT():
      BackupContainerBase(),
      mesh(0),
      polyMesh(0)
      #ifdef ENABLE_SKELETON_SUPPORT
      ,skeleton(0)
      #endif
    {
    }
    
    virtual ~BackupT()
    {
//       if (mesh != 0)
//         delete mesh;
//       
//       if (polyMesh != 0)
//         delete polyMesh;
    }
    
  public:
    TriMesh*    mesh;
    PolyMesh*   polyMesh;
    
    #ifdef ENABLE_SKELETON_SUPPORT
    Skeleton*  skeleton;
    #endif
};

class BackupPlugin : public QObject, BaseInterface , KeyInterface, MenuInterface, BackupInterface, LoggingInterface, ToolbarInterface, LoadSaveInterface
{
Q_OBJECT
Q_INTERFACES(BaseInterface)
Q_INTERFACES(KeyInterface)
Q_INTERFACES(MenuInterface)
Q_INTERFACES(BackupInterface)
Q_INTERFACES(LoggingInterface)
Q_INTERFACES(ToolbarInterface)
Q_INTERFACES(LoadSaveInterface)

signals:
  // BaseInterface
  void updateView();
  void updatedObject(int);
  
  //  LoggingInterface
  void log(Logtype _type, QString _message);
  void log(QString _message);

  // MenuInterface
  void getMenubarMenu (QString _name, QMenu *& _menu, bool _create);
  
  // BackupInterface
  // Tell Plugins about restore state:
  void aboutToRestore(int _objectid , int _internalId); 
  void restore(int _objectid , int _internalId);
  void restored( int _objectid , int _internalId);
  
  void createBackupGroup(QString _name, int& _groupId);
  
  // ToolBarInterface
  void addToolbar(QToolBar* _toolbar);
  void getToolBar( QString _name, QToolBar*& _toolbar);
  
  //KeyInterface
  void registerKey(int _key, Qt::KeyboardModifiers _modifiers, QString _description, bool _multiUse = false);
  
private slots:

  // BaseInterface
  void initializePlugin();
  void pluginsInitialized();
  
  void slotAllCleared();
  
  // KeyInterface
  void slotKeyEvent( QKeyEvent* _event );
  
  // LoadSaveInterface
  void objectDeleted (int _objectid);
   
  // Backup Interface
  /// Backup an object ( remember that this slot is called on all plugins.
  void slotBackup( int _objectid , QString _name , int _internalId, int _groupId = -1);
  
  // Called to prevent automatic removal of a backup.
  void slotMakeBackupPersistent(int _objectid, int _internalId);
  
  /// Restore an object
  void slotRestoreObject(int _objectid , int _internalId);
  
  // a backup group was added
  void slotBackupGroup( QString _name, int _groupId);

private slots:    
  
  /// Called before showing Backup Menu ( used for updates )
  void slotMenuAboutToShow();
   
  /// Backup Menu clicked
  void slotBackupMenu(QAction* _action);
  
  /// Backups enabled or disabled checkbox
  void slotEnableDisableBackups();

  
public slots:
  QString version() { return QString("0.1"); };  
  
public:
  ~BackupPlugin() {};
  
  QString name() { return (QString("Backup")); };
  QString description( ) { return (QString("Creates Backups of objects when supported by plugins")); };

private:
  
  /** Restore an object ( only for internal use! )
  * @param _objectId  id of the object to restore
  * @param _backupId id of the backup to restore or -1 for last backup
  */
  void backupPluginInternalRestore(int _objectId,  int _backupId );  
  
  
  /** \brief Get backup data from storage
  *
  * This function is used to get a backup from the storage
  * 
  */
  bool getBackupData(int _objectId, int _backupId, BackupT*& _container, BaseObjectData*& _object);

private slots:       

   /// Restore the last backup
   void slotUndo();

   /// Redo last operation
   void slotRedo();


private :

   /// map that associates a list of (object-id,backupIDs) to a groupID
   std::map< int, std::vector< std::pair<int,int> > > backupGroups_;
  
   /// map from groupID to group name
   std::map< int, QString > groupNames_;
   
   /// queue that stores the chronological order of all groupIDs
   /// first element is the oldest groupID
   std::queue< int > groupUndoQueue_;
   std::queue< int > groupRedoQueue_;

   /// the latest backup always represents the active state
   /// from which we can undo back to older states
   int activeGroup_;
   
   /// Update the backup menu
   void update_menu();

   /// If an object is deleted or not found, this function will clean the backup map
   void removeDeletedObject(int _id);

   /// Maximum number of backups per object
   int maxBackupsPerElement;

   /// The backup Menu
   QMenu*  backupMenu_;

   /// The backup archive
   /// Don't use the global perObjectData as this would be changed on restore and or backup!
   ObjectIDPointerManagerT< std::vector<BackupT>* > backups_;

   /** \brief Flag if the menu needs to be updated before showing it.
    *
    * This flag enables us to decouple updates from updated object
    */
   bool menuDirty_;
   
   QAction* backupsEnabledAction_;

   //toolbar
   QAction* undoAction_;
   QAction* redoAction_;
};

#endif //BACKUPPLUGIN_HH
