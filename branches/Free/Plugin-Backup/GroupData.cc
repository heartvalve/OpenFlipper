#include "GroupData.hh"

#include "GroupBackup.hh"
#include <OpenFlipper/BasePlugin/PluginFunctions.hh>

//-----------------------------------------------------------------------------

GroupData::GroupData() : BackupData(0)
{
}

//-----------------------------------------------------------------------------

GroupData::~GroupData(){
}

//-----------------------------------------------------------------------------

/** \brief Undo operation on groups
 *
 * The undo on groups is different then on single objects
 * we cannot revert to the last undo state. We have to check which objects
 * are affected by the current state and revert to the last state for all of them
 *
 */
void GroupData::undo(){

  if ( undoStates_.empty() )
    return;

  GroupBackup* current = dynamic_cast< GroupBackup* >( currentState_ );
  
  if ( current == 0 )
    return;

  IdList ids = current->objectIDs();

  for (unsigned int i=0; i < ids.size(); i++ ){
    
    //get backup data and perform object undo
    BaseObjectData* object = 0;
    PluginFunctions::getObject(ids[i], object);

    if ( object != 0 ){

      if ( object->hasObjectData( OBJECT_BACKUPS ) ){

        //get backup object data
        BackupData* backupData = 0;

        backupData = dynamic_cast< BackupData* >(object->objectData(OBJECT_BACKUPS));
        backupData->undo();
      }
    }
  }

  //get backup
  BaseBackup* backup = undoStates_.back();

  // update current state
  undoStates_.pop_back();
  redoStates_.push_back( currentState_ );
  currentState_ = backup;
}

//-----------------------------------------------------------------------------

/** \brief Redo operation on groups
 *
 * The redo on groups is different then on single objects
 * we revert to the last redo state and perform a redo on every
 * involved object
 *
 */
void GroupData::redo(){

  if ( redoStates_.empty() )
    return;

  GroupBackup* redoState = dynamic_cast< GroupBackup* >( redoStates_.back() );
  
  if ( redoState == 0 )
    return;

  IdList ids = redoState->objectIDs();
  
  for (unsigned int i=0; i < ids.size(); i++ ){

    //get backup data and perform object undo
    BaseObjectData* object = 0;
    PluginFunctions::getObject(ids[i], object);

    if ( object != 0 ){

      if ( object->hasObjectData( OBJECT_BACKUPS ) ){

        //get backup object data
        BackupData* backupData = 0;

        backupData = dynamic_cast< BackupData* >(object->objectData(OBJECT_BACKUPS));
        backupData->redo();
      }
    }
  }

  //get backup
  BaseBackup* backup = redoStates_.back();
  
  // update current state
  redoStates_.pop_back();
  undoStates_.push_back( currentState_ );
  currentState_ = backup;
}

//-----------------------------------------------------------------------------

/** \brief Update GroupData structure after object undo/redo
 *
 * After and undo/redo on an object the global structure has to be updated
 * to stay in sync.
 *
 */
void GroupData::updateBackupData(int _objectid, bool _isUndo){
  
  GroupBackup* current = dynamic_cast< GroupBackup* >( currentState_ );
  
  if ( current == 0)
    return;
  
  //find last backup that involves this object in the group backupData
  if ( _isUndo ){
    //UNDO ACTION

    //first case the undo is the last one on the stack
    if ( current->contains(_objectid) ){
      //get backup
      BaseBackup* backup = undoStates_.back();

      // update current state
      undoStates_.pop_back();
      redoStates_.push_back( currentState_ );
      currentState_ = backup;

    } else {
      //in this case the global currentState is unaffected by the current action
      //we find the last action in the undo queue that affects the object and move it to the redoQueue
      GroupBackup* backup =  0;
      int backupIndex     = -1;

      for(int i=undoStates_.size()-1; i >= 0; --i){
        GroupBackup* back = dynamic_cast< GroupBackup* >( undoStates_[i] );
        if ( back->contains(_objectid) ){
          backup = back;
          backupIndex = i;
          break;
        }
      }

      if (backup != 0){
        if ( !backup->blocked() ){
          // we found the backup that was reverted
          // move it to the redo states
          undoStates_.erase( undoStates_.begin() + backupIndex );
          redoStates_.push_back( backup );
        } else 
          std::cerr << "Error: Cannot updateBackupData. Backup is blocked!" << std::endl;
      }
    }

  } else {
    //REDO ACTION
    GroupBackup* backup =  0;
    int backupIndex     = -1;

    for(int i=redoStates_.size()-1; i >= 0; --i){
      GroupBackup* back = dynamic_cast< GroupBackup* >( redoStates_[i] );
      if ( back->contains(_objectid) ){
        backup = back;
        backupIndex = i;
        break;
      }
    }

    if (backup != 0){
      if ( !backup->blocked() ){
        // we found the backup that was reverted
        // it becomes the new current state
        redoStates_.erase( redoStates_.begin() + backupIndex );
        undoStates_.push_back( currentState_ );
        currentState_ = backup;
      } else 
        std::cerr << "Error: Cannot updateBackupData. Backup is blocked!" << std::endl;
    }
  }
}

//-----------------------------------------------------------------------------

void GroupData::undo(int _objectid){

  //get backup data and perform object undo
  BaseObjectData* object = 0;
  PluginFunctions::getObject(_objectid, object);

  if ( object != 0 ){

    if ( object->hasObjectData( OBJECT_BACKUPS ) ){

      //get backup object data
      BackupData* backupData = dynamic_cast< BackupData* >(object->objectData(OBJECT_BACKUPS));

      if ( !backupData->undoBlocked() ){
        backupData->undo();
        updateBackupData(_objectid, true);
      }else
        std::cerr << "Cannot undo operation. This backup involves multiple objects!" << std::endl;
    }
  }
}

//-----------------------------------------------------------------------------

void GroupData::redo(int _objectid){

  //get backup data and perform object undo
  BaseObjectData* object = 0;
  PluginFunctions::getObject(_objectid, object);

  if ( object != 0 ){



    if ( object->hasObjectData( OBJECT_BACKUPS ) ){

      //get backup object data
      BackupData* backupData = dynamic_cast< BackupData* >(object->objectData(OBJECT_BACKUPS));

      if ( !backupData->redoBlocked() ){
        backupData->redo();
        updateBackupData(_objectid, false);
      }else
        std::cerr << "Cannot redo operation. This backup involves multiple objects!" << std::endl;
    }
  }
}

//-----------------------------------------------------------------------------
