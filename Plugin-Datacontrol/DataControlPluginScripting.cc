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




#include "DataControlPlugin.hh"

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>


//******************************************************************************

void DataControlPlugin::setDescriptions(){

  emit setSlotDescription("getObject(QString)",tr("Returns the id of an object with given name."),
                          QStringList(tr("Name")), QStringList(tr("Name of an object")));

  emit setSlotDescription("getObjectName(int)",tr("Returns the name of an object with given id."),
                          QStringList(tr("objectId")), QStringList(tr("ID of an object")));

  emit setSlotDescription("dataType(int)",tr("Returns the DataType of the object with the given id."),
                            QStringList(tr("objectId")), QStringList(tr("ID of an object")));

  emit setSlotDescription("hideObject(int)",tr("Hide object with the given id."),
                          QStringList(tr("objectId")), QStringList(tr("ID of an object")));

  emit setSlotDescription("showObject(int)",tr("Show object with the given id."),
                          QStringList(tr("objectId")), QStringList(tr("ID of an object")));

  emit setSlotDescription("setTarget(int,bool)",tr("Set given object as target."),
                          QString(tr("ObjectId,Target")).split(","),
                          QString(tr("id of the object, set object as target?")).split(","));

  emit setSlotDescription("setSource(int,bool)",tr("Set given object as source."),
                          QString(tr("ObjectId,Source")).split(","),
                          QString(tr("id of the object, set object as source?")).split(","));

  emit setSlotDescription("setObjectName(int,QString)",tr("Set name of given object."),
                          QString(tr("ObjectId,name")).split(","),
                          QString(tr("id of the object, the new name")).split(","));

  emit setSlotDescription("groupObjects(idList,QString)",tr("Group given Objects together."),
                          QString(tr("objectIds,groupName")).split(","),
                          QString(tr("List of objects that should be grouped., Name of the group.")).split(","));

  emit setSlotDescription("groupObjects(idList)",tr("Group given Objects together."),
                          QStringList(tr("objectIds")), QStringList(tr("List of objects that should be grouped.")));

  emit setSlotDescription("groupCount()",tr("Returns the number of group objects."),
                          QStringList(tr("")), QStringList(tr("")));
                          
  emit setSlotDescription("unGroupObject(int)",tr("Remove the given object from its group and append to root node."),
                          QStringList(tr("ObjectId")), QStringList(tr("Object to be removed from group.")));                          

  emit setSlotDescription("objectDelete(int)",tr("Delete an object"),
                          QStringList(tr("objectId")), QStringList(tr("Delete the given object.")));

  emit setSlotDescription("copyObject(int)",tr("Create a copy of an object"),
                          QStringList(tr("objectId")), QStringList(tr("Object to copy.")));

  emit setSlotDescription("setAllTarget()",tr("Set All objects as targets"),
                          QStringList(), QStringList());

  emit setSlotDescription("setAllSource()",tr("Set All objects as source"),
                          QStringList(), QStringList());

  emit setSlotDescription("clearAllTarget()",tr("Clear targets"),
                          QStringList(), QStringList());

  emit setSlotDescription("clearAllSource()",tr("Clear sources"),
                          QStringList(), QStringList());

  emit setSlotDescription("showAll()",tr("Show all objects"),
                          QStringList(), QStringList());

  emit setSlotDescription("hideAll()",tr("Hide all objects"),
                          QStringList(), QStringList());

  emit setSlotDescription("availableDataTypeNames()",tr("Returns a QStringList of all available DataType names."),
                              QStringList(tr("")), QStringList(tr("")));

  emit setSlotDescription("printObjectInfoToLog()",tr("Print info about all objects to log"),
                            QStringList(), QStringList());

  emit setSlotDescription("addObjectToGroup(int,int)",tr("Add an Object to an existing group"),
                            QStringList(tr("objectId,groupId").split(",")),
                            QStringList(tr("ID of an object.,ID of an group where the object has to be added.").split(",")));

}


//******************************************************************************

/** \brief Returns the id of an object with given name
 *
 * @param _name name of an object
 * @return the id
 */
int DataControlPlugin::getObject( QString _name ) {

  BaseObject* object = PluginFunctions::objectRoot()->childExists(_name);

  if ( object == 0)
    return -1;

  return object->id();
}


//******************************************************************************

/** \brief Generate a copy of an object
 *
 * @param objectId id of an object
 * @return id of the generated object
 */
int DataControlPlugin::copyObject( int objectId ) {

  int newObject;

  emit copyObject( objectId, newObject );

  return newObject;
}


//******************************************************************************

/** \brief Returns the name of an object with given id
 *
 * @param objectId id of an object
 * @return name of the given object
 */
QString DataControlPlugin::getObjectName( int objectId ) {

  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(objectId,object) ) {
    emit log(LOGERR,tr("getObjectName : unable to get object") );
    return QString(tr("Unknown Object"));
  } else
    return  object->name() ;

}

//******************************************************************************

/// Get the DataType of a given object
DataType DataControlPlugin::dataType( int _objectId ) {
  BaseObject* object = 0;
  PluginFunctions::getObject(_objectId,object);

  if ( object )
    return object->dataType();
  else
    return DATA_UNKNOWN;
}


//******************************************************************************

/** \brief Hide object with the given id
 *
 * @param objectId id of an object
 */
void DataControlPlugin::hideObject( int objectId ) {

  if ( ! OpenFlipper::Options::gui())
    return;

  BaseObjectData* object;
  if ( PluginFunctions::getObject(objectId,object) ){
    object->hide();
  }

}


//******************************************************************************

/** \brief set the given Object as target
 *
 * @param objectId id of an object
 * @param _target set object as target?
 */
void DataControlPlugin::setTarget( int objectId, bool _target ) {

  BaseObjectData* object;
  if ( PluginFunctions::getObject(objectId,object) ){
    object->target( _target );
  }
}


//******************************************************************************

/** \brief set the given Object as source
 *
 * @param objectId id of an object
 * @param _source set object as source?
 */
void DataControlPlugin::setSource( int objectId, bool _source ) {

  BaseObjectData* object;
  if ( PluginFunctions::getObject(objectId,object) ){
    object->source( _source );
  }

}


//******************************************************************************

/** \brief set the name of the given Object
 *
 * @param objectId id of an object
 * @param _name new name
 */
void DataControlPlugin::setObjectName( int objectId, QString _name ) {

  BaseObjectData* object;
  if ( PluginFunctions::getObject(objectId,object) ){
    object->setName( _name );
  }
}


//******************************************************************************

/** \brief Delete an object
 *
 * @param objectId id of the object that should be deleted
 */
void DataControlPlugin::objectDelete( int objectId ) {

  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(objectId,object) )
    return;

  if ( object == 0)
    return;

  emit deleteObject(objectId);
}


//******************************************************************************

/** \brief Show object with the given id
 *
 * @param objectId id of an object
 */
void DataControlPlugin::showObject( int objectId ) {

  if ( ! OpenFlipper::Options::gui())
      return;

  BaseObjectData* object;
  if ( PluginFunctions::getObject(objectId,object) ){
    object->show();
  }

}


//******************************************************************************

/** \brief Group given Objects together
 *
 * @param _objectIDs list of object ids
 * @param _groupName the name of the new group
 */
int DataControlPlugin::groupObjects(IdList _objectIDs, QString _groupName) {
  QVector< BaseObject* > objs;

  // Try to get all objects given in list
  for (uint i=0; i < _objectIDs.size(); i++){
    BaseObject* obj;
    if ( PluginFunctions::getObject(_objectIDs[i],obj) )
      objs.push_back(obj);
  }

  // If all of them fail, stop here.
  if (objs.size() == 0){
    emit log(tr("No objects to group."));
    return -1;
  }

  // Check if all objects have the same parent
  // Abort if the parents differ
  BaseObject* parent = (objs[0])->parent();
  for ( int i = 1 ; i < objs.size() ; ++i){
    if ( parent != (objs[i])->parent() ){
      emit log(tr("Cannot group Objects with different parents"));
      return -1;
    }

  }

  //create new group
  if (parent == 0)
    parent = PluginFunctions::objectRoot();

  GroupObject* groupItem = new GroupObject( "newGroup", dynamic_cast< GroupObject* >(parent));

  //set groupName
  if (_groupName == "")
    groupItem->setName(tr("newGroup ") + QString::number(groupItem->id()));
  else
    groupItem->setName( _groupName );

  // Tell core that a new group has been added
  emit emptyObjectAdded( groupItem->id() );

  //append new children to group
  for ( int i = 0 ; i < objs.size() ; ++i) {
    (objs[i])->setParent( dynamic_cast< BaseObject* >( groupItem )  );
  }
  
  return groupItem->id();
}

bool DataControlPlugin::unGroupObject(int _id) {
  BaseObject* group = 0;
  
  PluginFunctions::getObject(_id,group);
 
  if ( group && group->isGroup())
  {
    //iterate over children
    for (int i=group->childCount()-1; i >= 0; --i){
      BaseObject* child = group->child(i);

      // then change the parent
      child->setParent(group->parent());
    }
    //delete the group
    emit deleteObject( group->id() );

    return true;
  } else {
    if (group)
      emit log( LOGERR, tr("Cannot Ungroup. Object with id %1 is not a group").arg(_id));
    else
      emit log( LOGERR, tr( "Unable to get Object with id %1 for ungrouping").arg(_id) );
    return false;
  }
}


//******************************************************************************

/** \brief Set Target Selection for all objects
 *
 */
void DataControlPlugin::setAllTarget() {

  DataType type = DATA_ALL;

  //try to find dataType restriction if called by contextMenu
  QAction* action = dynamic_cast< QAction* > ( sender() );

  if ( action != 0 && action->data().isValid() )
    type = (DataType) action->data().toUInt();

  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS, type) ;
                                    o_it != PluginFunctions::objectsEnd(); ++o_it){
    o_it->target(true);
  }
}


//******************************************************************************

/** \brief Set Source Selection for all objects
 *
 */
void DataControlPlugin::setAllSource() {

  DataType type = DATA_ALL;

  //try to find dataType restriction if called by contextMenu
  QAction* action = dynamic_cast< QAction* > ( sender() );

  if ( action != 0 && action->data().isValid() )
    type = (DataType) action->data().toUInt();

  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS, type) ;
                                    o_it != PluginFunctions::objectsEnd(); ++o_it){
    o_it->source(true);
  }
}


//******************************************************************************

/** \brief Clear Target Selection for all objects
 *
 */
void DataControlPlugin::clearAllTarget() {

  DataType type = DATA_ALL;

  //try to find dataType restriction if called by contextMenu
  QAction* action = dynamic_cast< QAction* > ( sender() );

  if ( action != 0 && action->data().isValid() )
    type = (DataType) action->data().toUInt();

  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS, type) ;
                                    o_it != PluginFunctions::objectsEnd(); ++o_it){
    o_it->target(false);
  }
}


//******************************************************************************

/** \brief Clear Source Selection for all objects
 *
 */
void DataControlPlugin::clearAllSource() {

  DataType type = DATA_ALL;

  //try to find dataType restriction if called by contextMenu
  QAction* action = dynamic_cast< QAction* > ( sender() );

  if ( action != 0 && action->data().isValid() )
    type = (DataType) action->data().toUInt();

  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS, type) ;
                                    o_it != PluginFunctions::objectsEnd(); ++o_it){
    o_it->source(false);
  }
}


//******************************************************************************

/** \brief Hide all objects
 *
 */
void DataControlPlugin::hideAll() {

  if ( ! OpenFlipper::Options::gui())
      return;

  DataType type = DATA_ALL;

  //try to find dataType restriction if called by contextMenu
  QAction* action = dynamic_cast< QAction* > ( sender() );

  if ( action != 0 && action->data().isValid() )
    type = (DataType) action->data().toUInt();

  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS,type) ;
                                    o_it != PluginFunctions::objectsEnd(); ++o_it){
    o_it->hide();
  }

}


//******************************************************************************

/** \brief Show all objects
 *
 */
void DataControlPlugin::showAll() {

  if ( ! OpenFlipper::Options::gui())
      return;

  DataType type = DATA_ALL;

  //try to find dataType restriction if called by contextMenu
  QAction* action = dynamic_cast< QAction* > ( sender() );

  if ( action != 0 && action->data().isValid() )
    type = (DataType) action->data().toUInt();

  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS, type); o_it != PluginFunctions::objectsEnd(); ++o_it){
    o_it->show();
  }

}

//******************************************************************************

/** \brief Get all target objects of given type
 *
 */
IdList DataControlPlugin::getTargetObjects(DataType _type) {

  IdList list;
  
  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS, _type); o_it != PluginFunctions::objectsEnd(); ++o_it)
    list.push_back( o_it->id() );
    
  return list;
}

//******************************************************************************

/** \brief Get all source objects of given type
 *
 */
IdList DataControlPlugin::getSourceObjects(DataType _type) {
  
  IdList list;
  
  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::SOURCE_OBJECTS, _type); o_it != PluginFunctions::objectsEnd(); ++o_it)
    list.push_back( o_it->id() );
    
  return list;  
}

//******************************************************************************

/** \brief Prints information about all open objects to the Log
 *
 */
void DataControlPlugin::printObjectInfoToLog() {

  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS); o_it != PluginFunctions::objectsEnd(); ++o_it)
    emit log(LOGINFO, tr("Object \"%1\" with ID %2 of type %3 ").arg(o_it->name()).arg(o_it->id()).arg(o_it->dataType().name()));
}

//******************************************************************************

/** \brief get number of groups
 *
 * Returns the number of groups that are in the scene
 *
 * @return Number of groups
 */
unsigned int DataControlPlugin::groupCount() const {
  unsigned int count = 0;
  for ( PluginFunctions::BaseObjectIterator o_it(PluginFunctions::ALL_OBJECTS,DATA_GROUP); o_it != PluginFunctions::objectsEnd(); ++o_it)
    ++count;

  return count;
}

//******************************************************************************

/** \brief Return available dataType
 *
 * Returns a space separated list of all available DataTypes
 *
 * @return List of available data types
 */
QStringList DataControlPlugin::availableDataTypeNames() const {

  QStringList list;

  for ( std::vector< TypeInfo >::const_iterator dataType = typesBegin(); dataType != typesEnd(); ++dataType )
    list.push_back(dataType->name);

  return list;
}

//******************************************************************************
/** \brief add an object to an existing group
 *
 * add the object with _objectId to the group with _groupId
 *
 * @return success
*/
bool DataControlPlugin::addObjectToGroup(int _objectId, int _groupId)
{
  BaseObject* obj = 0;
  BaseObject* group = 0;

  PluginFunctions::getObject(_objectId,obj);
  PluginFunctions::getObject(_groupId,group);

   if ( obj && group)
   {
     if (group->isGroup())
     {
       obj->setParent(group);
       return true;
     }
     else
       emit log( LOGERR, tr(" Object with id %1 is not a group").arg(_groupId));
   } else {
       if (!obj)
         emit log( LOGERR, tr( "Unable to get Object with id %1").arg(_objectId) );
       if (!_groupId)
         emit log( LOGERR, tr( "Unable to get Group with id %1").arg(_groupId) );
   }
   return false;
}
