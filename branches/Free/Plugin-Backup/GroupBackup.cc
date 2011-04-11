#include "GroupBackup.hh"

#include <OpenFlipper/common/BackupData.hh>
#include <OpenFlipper/BasePlugin/PluginFunctions.hh>

//-----------------------------------------------------------------------------

GroupBackup::GroupBackup(IdList _objectIDs, QString _name)
  : BaseBackup(_name),
    objectIDs_(_objectIDs)
{
//   std::cerr << "Create GroupBackup with name:" << name_.toStdString() << "(id : " << id_ << ")" << std::endl;
  
  //store the backup ids of all objects
  for (unsigned int i=0; i < objectIDs_.size(); ++i ){

    BaseObjectData* object = 0;
    PluginFunctions::getObject(objectIDs_[i], object);
    int id = -1;

    if ( object != 0 ){

      //get backup object data
      BackupData* backupData = 0;

      if ( object->hasObjectData( OBJECT_BACKUPS ) ){
        backupData = dynamic_cast< BackupData* >(object->objectData(OBJECT_BACKUPS));
        id = backupData->currentID();

        //add links so that object backups know that they have to be applied together
        if ( objectIDs_.size() > 1 )
          backupData->setLinks(_objectIDs);
      }
    }
    backupIDs_.push_back(id);
  }
}

//-----------------------------------------------------------------------------

GroupBackup::~GroupBackup(){
//   std::cerr << "Delete GroupBackup with name:" << name_.toStdString() << "(id : " << id_ << ")" << std::endl;
}

//-----------------------------------------------------------------------------

void GroupBackup::apply(){

}

//-----------------------------------------------------------------------------

IdList GroupBackup::objectIDs(){
  return objectIDs_;
}

//-----------------------------------------------------------------------------

bool GroupBackup::contains( int _objectid ){
  bool found = false;

  for(IdList::iterator it = objectIDs_.begin(); it != objectIDs_.end(); ++it)
    if ( *it == _objectid ){
      found = true;
      break;
    }

  return found;
}

//-----------------------------------------------------------------------------