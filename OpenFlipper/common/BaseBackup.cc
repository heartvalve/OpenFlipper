#include "BaseBackup.hh"
#include <OpenFlipper/common/BackupData.hh> 

//-----------------------------------------------------------------------------

static int maxBackupId_ = 0;

//-----------------------------------------------------------------------------

BaseBackup::BaseBackup(QString _name) : object_(0), name_(_name), id_(maxBackupId_++)
{
}

//-----------------------------------------------------------------------------

BaseBackup::BaseBackup(BaseObjectData* _object, QString _name, UpdateType _type) : object_(_object), name_(_name), id_(maxBackupId_++)
{
//   std::cerr << "Create BaseBackup with name:" << name_.toStdString() << "(id : " << id_ << ")" << std::endl;

  // Get the per Object Datas
  QMap<QString, PerObjectData*> dataMap = object_->getPerObjectDataMap();
  
  // Iterate over all per Object datas and try to copy them into our backup storage
  QMap<QString, PerObjectData*>::const_iterator mapIter;
  
  for ( mapIter = dataMap.constBegin(); mapIter != dataMap.constEnd(); ++mapIter){
    //don't copy myself
    if ( mapIter.key() == OBJECT_BACKUPS ) continue;

    // Try to get a copy of the objectData
    PerObjectData* copiedData = mapIter.value()->copyPerObjectData();

    if ( copiedData )
      objectDatas_.push_back( std::make_pair( mapIter.key(),copiedData ) );
    else
      std::cerr << "Failed to copy per Object Data: " << mapIter.key().toStdString() << std::endl;
  }
}

//-----------------------------------------------------------------------------

BaseBackup::~BaseBackup(){

//   std::cerr << "Delete BaseBackup with name:" << name_.toStdString() << "(id : " << id_ << ")" << std::endl;

  for (uint i=0; i < objectDatas_.size(); i++)
    delete objectDatas_[i].second;
}

//-----------------------------------------------------------------------------

void BaseBackup::apply(){

//   std::cerr << "Apply BaseBackup with name:" << name_.toStdString() << "(id : " << id_ << ")" << std::endl;

  if (object_ == 0)
    return;

  PerObjectData* backupData = 0;

  // Get the per Object Data container in the object
  QMap<QString, PerObjectData*>& dataMap = object_->getPerObjectDataMap();

  QMapIterator<QString, PerObjectData* > i(dataMap);
  while (i.hasNext()) {
      i.next();
      if ( i.key() == OBJECT_BACKUPS )
        backupData = i.value();
      else
        delete i.value();
  }

  dataMap.clear();

  if (backupData == 0){
    std::cerr << "Cannot apply backup. BackupData not found!!" << std::endl;
    return;
  }

  // insert backup Data
  dataMap.insert( OBJECT_BACKUPS, backupData);
  
  // now insert copies of the perObjectData from the backup
  for (unsigned int i=0; i < objectDatas_.size(); i++ ){
    // Try to get a copy of the objectData
    PerObjectData* copiedData = objectDatas_[i].second->copyPerObjectData();

    if ( copiedData )
      dataMap.insert( objectDatas_[i].first, copiedData );
  }
}

//-----------------------------------------------------------------------------

QString BaseBackup::name(){
  return name_;
}

//-----------------------------------------------------------------------------

int BaseBackup::id(){
  return id_;
}

//-----------------------------------------------------------------------------

bool BaseBackup::blocked(){
  return !links_.empty();
}

//-----------------------------------------------------------------------------

void BaseBackup::setLinks(IdList _objectIDs){
  
  //remove myself from links
  for(int i=_objectIDs.size()-1; i >= 0; --i )
    if ( i == id_ )
      _objectIDs.erase( _objectIDs.begin() + i );

  //store links
  links_ = _objectIDs;
}

//-----------------------------------------------------------------------------