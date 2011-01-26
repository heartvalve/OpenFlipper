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

#include <QtGui>
#include "BackupPlugin.hh"
#include <iostream>
#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include <OpenFlipper/common/GlobalOptions.hh>
// #include <boost/property_map/property_map.hpp>

#define MAX_BACKUP_GROUPS 20

void BackupPlugin::initializePlugin() {
  maxBackupsPerElement = 20;
  activeGroup_ = -1;
  menuDirty_ = true;
}

void BackupPlugin::pluginsInitialized() {

  // Create Backup menu
  emit getMenubarMenu(tr("&Backup"), backupMenu_, true );

  // update menu before showing it
  connect( backupMenu_, SIGNAL(aboutToShow()), this , SLOT(slotMenuAboutToShow()) );
  
  if ( OpenFlipper::Options::gui() ) {
    backupsEnabledAction_ = new QAction("Backups Enabled",0);
    backupsEnabledAction_->setCheckable(true);
  
    // Global Flag to enable or disable backups
    backupsEnabledAction_->setChecked( OpenFlipper::Options::backupEnabled() );
    
    connect (backupsEnabledAction_, SIGNAL(triggered(bool)), this, SLOT(slotEnableDisableBackups()) );
   

    // Add a backup Toolbar
    QToolBar* toolbar = new QToolBar("Backup Toolbar");
    
    //Undo
    undoAction_ = new QAction(tr("&Undo"), this);
    undoAction_->setEnabled(false);
    undoAction_->setStatusTip(tr("Undo the last action."));
    undoAction_->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"edit-undo.png") );
    
    connect(undoAction_, SIGNAL(triggered()), this, SLOT(slotUndo()) );
    toolbar->addAction(undoAction_);
    
    //Redo
    redoAction_ = new QAction(tr("&Redo"), this);
    redoAction_->setEnabled(false);
    redoAction_->setStatusTip(tr("Redo the last action"));
    redoAction_->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"edit-redo.png") );
    connect(redoAction_, SIGNAL(triggered()), this, SLOT(slotRedo()) );
    toolbar->addAction(redoAction_);
  
    emit addToolbar( toolbar );
    
    //the release event does not contain the modifier
    emit registerKey(Qt::Key_Z,  Qt::ControlModifier, tr("Undo Action"));
    emit registerKey(Qt::Key_Z, (Qt::ControlModifier | Qt::ShiftModifier),   tr("Redo Action"));
  }
}

void BackupPlugin::slotAllCleared(){
  
  backupGroups_.clear();
  groupNames_.clear();
  
  while( !groupUndoQueue_.empty() )
    groupUndoQueue_.pop();

  while( !groupRedoQueue_.empty() )
    groupRedoQueue_.pop();
  
  undoAction_->setEnabled(false);
  redoAction_->setEnabled(false);
  
  activeGroup_ = -1;
}

void BackupPlugin::slotMenuAboutToShow() {
  if ( menuDirty_ ) {
    update_menu();
  } 
}

void BackupPlugin::slotEnableDisableBackups() {
  OpenFlipper::Options::enableBackup(backupsEnabledAction_->isChecked());
}

void BackupPlugin::slotKeyEvent( QKeyEvent* _event ){
   switch (_event->key())
   {
      case Qt::Key_Z :
         if ( _event->modifiers() == Qt::ControlModifier )
           slotUndo();
         else if ( _event->modifiers() == (Qt::ControlModifier | Qt::ShiftModifier) )
           slotRedo();
         break;
      default:
         break;
  }
}

void BackupPlugin::objectDeleted(int _objectid) {
    removeDeletedObject (_objectid);
}

void BackupPlugin::slotBackup( int _objectid , QString _name , int _internalId, int _groupId) {

  if ( !OpenFlipper::Options::backupEnabled() )
    return;

  BaseObjectData* object;

  if ( PluginFunctions::getObject(_objectid,object) ) {
    
    //create a group if not already there
    int groupID = _groupId;
    
    if ( groupID == -1 )
      emit createBackupGroup(_name, groupID);
    
    backupGroups_[ groupID ].push_back( std::make_pair(_objectid, _internalId) );

    // Check if a backup list already exists for this object
    std::vector<BackupT>* archive;
    if ( !backups_.get_pointer(_objectid,archive) ) {
      archive = new std::vector<BackupT>;
      backups_.add_pointer(_objectid,archive);
    }

    // We count the backups which could be non persistant. 
    // If this is less than the maximum number of backups, we will not delete any backup.
    int remainingBackups = archive->size();
    
    // Delete oldest element if maximum number of elements per object reached
    if ( (int)archive->size() >= maxBackupsPerElement ) {
      std::vector<BackupT>::iterator item = archive->begin();
      
      while (item != archive->end() ) {
        
        // Use at minimum maxBackupsPerElement which are non persistent
        if ( remainingBackups < maxBackupsPerElement )
          break;
        
        if ( ! item->persistent ) {
          delete (item->mesh);
          
          // delete the perObjectDatas
          item->clear();
          archive->erase(item);
          break;
        }
        remainingBackups--;
      }

    }

    // Create backups of poly and triangle meshes
    BackupT new_backup;

    if ( object->dataType( DATA_TRIANGLE_MESH ) )
        new_backup.mesh = new TriMesh(*(PluginFunctions::triMesh(object)));

    if ( object->dataType( DATA_POLY_MESH ) )
      new_backup.polyMesh = new PolyMesh(*(PluginFunctions::polyMesh(object)));
    
    #ifdef ENABLE_SKELETON_SUPPORT
    if ( object->dataType( DATA_SKELETON ) )
      new_backup.skeleton = new Skeleton(*(PluginFunctions::skeleton(object)));
    #endif

    // Get the per Object Datas
    QMap<QString, PerObjectData*> dataMap = object->getPerObjectDataMap();
    
    // Iterate over all per Object datas and try to copy them into our backup storage
    QMap<QString, PerObjectData*>::const_iterator mapIter = dataMap.begin();  
    while ( mapIter != dataMap.end() ) {
      
      // Try to get a copy of the object.
      PerObjectData* copiedData = mapIter.value()->copyPerObjectData();
      
      if ( copiedData ) {
        new_backup.perObjectDatas.insert(mapIter.key(),copiedData);
      } else {
        std::cerr << "Failed to copy per Object Data: " << mapIter.key().toStdString() << std::endl;
      }
      
      mapIter++;
    }
    
    // Store information about the backup
    new_backup.objectId = object->id();
    QString backupNumber;
    backupNumber.setNum(_internalId);
    new_backup.backupName   = _name ;
    new_backup.backupNumber = _internalId;

    archive->push_back(new_backup);

    emit log(LOGOUT,"Created backup for " + QString::number(_objectid)+ " , Name : \"" + _name + "\", id " + QString::number(_internalId));

    menuDirty_ = true;
  } else {
    emit log(LOGWARN,"Unable to find backup target object for id " + QString::number(_objectid)+
                  " for requested Backup " + _name);
  }
}

// a backup group was added
void BackupPlugin::slotBackupGroup( QString _name, int _groupId) {
  
  //push last active group to undo queue
  if ( activeGroup_ != -1)
    groupUndoQueue_.push( activeGroup_ );
  
  //add new group as active group
  groupNames_[ _groupId ] = _name;
  activeGroup_ = _groupId;

  undoAction_->setEnabled( !groupUndoQueue_.empty() );
  redoAction_->setEnabled(false);
  
  //remove groups that are too old
  while ( groupUndoQueue_.size() > MAX_BACKUP_GROUPS ){
    
    int oldGroup = groupUndoQueue_.front();
    groupUndoQueue_.pop();
    
    groupNames_.erase( oldGroup );
    backupGroups_.erase( oldGroup );
  }

  //remove all redo steps because they are no longer applicable
  while ( groupRedoQueue_.size() > 0 ){
    
    int redoGroup = groupRedoQueue_.front();
    groupRedoQueue_.pop();
    
    groupNames_.erase( redoGroup );
    backupGroups_.erase( redoGroup );
  }
}

void BackupPlugin::update_menu( ) {
  backupMenu_->clear();

  // Global Flag to enable or disable backups
  backupMenu_->addAction(backupsEnabledAction_);
  backupsEnabledAction_->setChecked( OpenFlipper::Options::backupEnabled() );
  
  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS,DATA_TRIANGLE_MESH|DATA_TRIANGLE_MESH) ; o_it != PluginFunctions::objectsEnd(); ++o_it) {

    std::vector<BackupT>* archive;
    if ( !backups_.get_pointer(o_it->id(),archive) ) {
      continue;
    }
    
    //Add a Menu for every Object that has a backup
    QMenu* menu = new QMenu( o_it->name() );
    backupMenu_->addMenu(menu);

    // grouping =
    // 0 element does not belong to a group
    // 1 element is first of group
    // 2 element belongs to group but is not first nor last
    // 3 element is last in a group
    short grouping = 0;
    uint numEntries = archive->size();
    QActionGroup* tmp_group = 0;
    QMenu* backupMenu = 0;
    for ( uint i = 0 ; i < numEntries; ++i ) {
        
        // Reset grouping if last element of group has been passed
        if(grouping == 3) grouping = 0;
        
        if(((i < numEntries - 1) && ((*archive)[i].backupName == (*archive)[i+1].backupName))) {
               // Element is either first or some element between the first and the last one in group
               
               if(((i > 0) && ((*archive)[i].backupName != (*archive)[i-1].backupName)) || (i == 0)) {
                   // Element is first of group
                   grouping = 1;
               } else if ((i > 0) && ((*archive)[i].backupName == (*archive)[i-1].backupName)) {
                   // Element belongs to group but is not the first one
                   grouping = 2;
               }
        } else if((grouping > 0 && grouping < 3 && ((i < numEntries - 1) && ((*archive)[i].backupName != (*archive)[i+1].backupName))) ||
            ((i == numEntries-1 && grouping > 0 && grouping < 3))) {
            
            grouping = 3; // Last element of a group
            
        } else {
            grouping = 0;
        }
        
        if(grouping == 0) {
            // Element does not belong to a group
            
            // Associate ObjectId and backupId with Object
            QString identifier = QString::number(o_it->id()) + ":" + QString::number((*archive)[i].backupNumber);
            
            QAction* new_backup = new QAction((*archive)[i].backupName, 0);
        
            new_backup->setData(identifier);
            new_backup->setText((*archive)[i].backupName);
            
            menu->addAction(new_backup);
            
        } else {
            // Element belongs to a group
            
            // Associate ObjectId and backupId with Object
            QString identifier = QString::number(o_it->id()) + ":" + QString::number((*archive)[i].backupNumber);
            
            if(grouping == 1) {
                // Start new group
                tmp_group = new QActionGroup(0);
                backupMenu = new QMenu((*archive)[i].backupName);
            }
            
            QAction* new_backup = new QAction((*archive)[i].backupName, tmp_group);
            
            new_backup->setData(identifier);
            new_backup->setText((*archive)[i].backupName);
            
            if(grouping == 3) {
                // Last element of group
                if(backupMenu != 0) {
                    backupMenu->addActions(tmp_group->actions());
                    menu->addMenu(backupMenu);
                    
                    // Connect triggering of actions to local slot
                    connect( backupMenu, SIGNAL( triggered( QAction * ) ), this, SLOT( slotBackupMenu( QAction * ) ) );
                }
            }
        }
        
    }

    // Connect triggering of actions to local slot
    connect( menu,       SIGNAL( triggered( QAction * ) ), this, SLOT( slotBackupMenu( QAction * ) ) );

   }

  if (groupNames_.size() > 0) {
      
      //Add a Menu for backup groups if there are any
      QMenu* menu = new QMenu( "Backup Groups" );
      backupMenu_->addSeparator();
      backupMenu_->addMenu(menu);

      std::queue<int> tmpQueue = groupUndoQueue_;

      while ( tmpQueue.size() > 0 ) {

        int groupID = tmpQueue.front();
        tmpQueue.pop();

        QAction* new_backup = new QAction( "Undo:" + groupNames_[groupID],0);
        new_backup->setData(groupID);

        menu->addAction(new_backup);
      }

      menu->addSeparator();
      QAction* new_backup = new QAction( "Active:" + groupNames_[activeGroup_],0);
      new_backup->setData(activeGroup_);
      menu->addAction(new_backup);
      menu->addSeparator();

      tmpQueue = groupRedoQueue_;
      std::vector< QAction* > redoActions;
      while ( tmpQueue.size() > 0 ) {

        int groupID = tmpQueue.front();
        tmpQueue.pop();

        QAction* new_backup = new QAction( "Redo:" + groupNames_[groupID],0);
        new_backup->setData(groupID);

        redoActions.push_back( new_backup );
      }

      //add redo steps in reverse order
      for (std::vector<QAction*>::reverse_iterator it = redoActions.rbegin(); it != redoActions.rend(); ++it)
        menu->addAction( *it );
  }
  
  menuDirty_ = false;
}

void BackupPlugin::removeDeletedObject(int _id) {
  // Try to find the corresponding backup vector
  std::vector<BackupT>* archive;
  if ( !backups_.get_pointer(_id,archive) ) {
    return;
  }

  
  for ( uint i = 0 ; i < archive->size() ; ++i ) {
    // remove backup copies of the meshes
    if ((*archive)[i].mesh != NULL)
      delete (*archive)[i].mesh;
    if ((*archive)[i].polyMesh != NULL)
      delete (*archive)[i].polyMesh;
    
    // clear the perObjectDatas
    (*archive)[i].clear();
  }

  // delete vector
  delete archive;

  // remove pointer
  backups_.delete_object(_id);

  // Force update of menu
  menuDirty_ = true;
}

void BackupPlugin::slotBackupMenu(QAction* _action) {
  
  // Get the object identification which is attached to the action
  QStringList identifier = (_action->data().toString()).split(":");
                                                     
  // First entry is the object id
  int objectId = identifier[0].toInt();
  
  // Second entry is the backup id
  int backupId = identifier[1].toInt();
  
  backupPluginInternalRestore(objectId,backupId);
}

bool BackupPlugin::getBackupData(int _objectId, int _backupId, BackupT*& _container, BaseObjectData*& _object) {
  
  // get the right object or fail
  if ( ! PluginFunctions::getObject(_objectId,_object) ) {
    emit log(LOGERR,"Unable to find object with id " + QString::number(_objectId) + " for restore.");
    return false;
  }
  
  // Get the backup vector of this object
  std::vector<BackupT>* archive;
  if ( !backups_.get_pointer(_objectId,archive) ) {
    emit log(LOGERR,"Unable to find backups for object with id " + QString::number(_objectId));
    return false;
  }
  
  int vectorIndex = -1;
  
  // if negative, use current-x backup set
  if ( _backupId < 0 ) {
    vectorIndex = archive->size() + _backupId;
  } else {
    // otherwise use the given backup id  
    for ( uint i = 0 ; i < archive->size() ; ++i ) {
      if ( (*archive)[i].backupNumber == _backupId  ) {
        vectorIndex = i;
        break;
      }
    }
  }
  
  if ( !( (vectorIndex >= 0) && ( vectorIndex < (int)archive->size() ) )) {
    emit log(LOGERR,"Vector Index out of range when getting for backup ( Backup not found! ), ObjectId : " + 
                    QString::number(_objectId) + " , BackupId: " + QString::number(_backupId));
    return false ;
  }
  
  _container = &((*archive)[ vectorIndex ]);
  
  return true;
}

/// Restore an object
void BackupPlugin::slotRestoreObject(int _objectid , int _internalId){  
  backupPluginInternalRestore(_objectid,_internalId);
}

void BackupPlugin::backupPluginInternalRestore(int _objectId, int _backupId ) {

  BaseObjectData* object;
  BackupT*        restoreContainer;
  
  if ( !getBackupData(_objectId,_backupId,restoreContainer,object) ) {
    emit log(LOGERR, "Unable to find backup!");
    return;
  }
      
  emit log(LOGOUT,"Restoring " + restoreContainer->backupName + " from object " + QString::number(restoreContainer->objectId) );

  // Tell other plugins that we are going to do an restore
  emit aboutToRestore(_objectId,restoreContainer->backupNumber);

  // Restore object datas first. this will delete old perObject datas on the current mesh
  // rather than on the restored one, which could lack some properties
  // Delete current data
  object->deleteData();
  
  // Get the per Object Data container in the object
  QMap<QString, PerObjectData*>& dataMap = object->getPerObjectDataMap();
  
  // Iterate over all per Object datas in the restore set and try to copy them into our backup storage
  QMap<QString, PerObjectData*>::const_iterator mapIter = restoreContainer->perObjectDatas.begin();  
  while ( mapIter != restoreContainer->perObjectDatas.end() ) {
    
    // Try to get a copy of the object.
    PerObjectData* copiedData = mapIter.value()->copyPerObjectData();
    
    if ( copiedData ) {
      dataMap.insert(mapIter.key(),copiedData);
    } else {
      std::cerr << "Failed to copy per Object Data: " << mapIter.key().toStdString() << std::endl;
    }
    
    mapIter++;
  }


  // Restore the meshes
  if ( object->dataType( DATA_TRIANGLE_MESH ) && restoreContainer->mesh != NULL ) {
    *(PluginFunctions::triMesh(object)) = *(restoreContainer->mesh);
//     emit log(LOGINFO,"Restored Triangle Mesh for object " + QString::number(object->id()) );
  } else if ( object->dataType( DATA_POLY_MESH ) && restoreContainer->polyMesh != NULL ) {
    *(PluginFunctions::polyMesh(object)) = *(restoreContainer->polyMesh);
//     emit log(LOGINFO,"Restored Poly Mesh for object " + QString::number(object->id()) );
  #ifdef ENABLE_SKELETON_SUPPORT
  } else if ( object->dataType( DATA_SKELETON ) && restoreContainer->skeleton != NULL ) {
    *(PluginFunctions::skeleton(object)) = *(restoreContainer->skeleton);
//     emit log(LOGINFO,"Restored Skeleton for object " + QString::number(object->id()) );
  #endif
  } else
      emit log(LOGINFO,"Backup plugin does not support this type of object " + QString::number(object->id()) );
  
  
  // Tell other plugins that they should do a restore
  emit restore(_objectId,restoreContainer->backupNumber);
  
  // Tell other plugins that the restore is complete
  emit restored(_objectId,restoreContainer->backupNumber);
  
  // Tell all plugins that the object is updated
  emit updatedObject(_objectId);

  // Force update of menu
  menuDirty_ = true;
}

// Called to prevent automatic removal of a backup.
void BackupPlugin::slotMakeBackupPersistent(int _objectid, int _internalId) {
  
  BaseObjectData* object;
  BackupT*        restoreContainer;
  
  if ( !getBackupData(_objectid,_internalId,restoreContainer,object) ) {
    emit log(LOGERR, "Unable to find backup!");
    return;
  }
  
  restoreContainer->persistent = true;

}

void BackupPlugin::slotRedo() {

  if ( groupRedoQueue_.size() == 0)
    return;
  
  std::queue<int> tmpQueue = groupRedoQueue_;
  std::queue<int> newQueue; //queue without latest element
  
  //we need the newest(last) item in the queue
  int groupID = -1;
  
  while( !tmpQueue.empty() ){
    
    if (groupID != -1)
      newQueue.push( groupID );
    
    groupID = tmpQueue.front();
    tmpQueue.pop();
  }

  emit log(LOGINFO,"Redo '" + groupNames_[groupID] + "'" );

  //undo the group
  std::vector< std::pair<int,int> > internalIDs = backupGroups_[ groupID ];
  
  std::vector< std::pair<int,int> >::iterator it;
  
  for (it = internalIDs.begin(); it != internalIDs.end(); ++it)
    backupPluginInternalRestore( it->first, it->second );
  
  //and update the queues
  groupRedoQueue_ = newQueue;
  groupUndoQueue_.push(activeGroup_);

  //now the group is active
  activeGroup_ = groupID;
  
  undoAction_->setEnabled(true);
  redoAction_->setEnabled( !groupRedoQueue_.empty() );
}

void BackupPlugin::slotUndo() {
  
  if ( groupUndoQueue_.size() == 0)
    return;
  
  std::queue<int> tmpQueue = groupUndoQueue_;
  std::queue<int> newQueue; //queue without latest element
  
  //we need the newest(last) item in the queue
  int groupID = -1;
  
  while( !tmpQueue.empty() ){
    
    if (groupID != -1)
      newQueue.push( groupID );
    
    groupID = tmpQueue.front();
    tmpQueue.pop();
  }

  emit log(LOGINFO,"Undo '" + groupNames_[groupID] + "'" );

  //undo the group
  std::vector< std::pair<int,int> > internalIDs = backupGroups_[ groupID ];
  
  std::vector< std::pair<int,int> >::iterator it;
  
  for (it = internalIDs.begin(); it != internalIDs.end(); ++it)
    backupPluginInternalRestore( it->first, it->second );
    
  //and update the queues
  groupUndoQueue_ = newQueue;
  groupRedoQueue_.push(activeGroup_);

  //now the group is active
  activeGroup_ = groupID;
  
  undoAction_->setEnabled( !groupUndoQueue_.empty() );
  redoAction_->setEnabled(true);
}

Q_EXPORT_PLUGIN2( backupplugin , BackupPlugin );

