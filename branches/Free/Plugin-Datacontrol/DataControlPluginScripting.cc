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




#include "DataControlPlugin.hh"

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>


void DataControlPlugin::setDescriptions(){

  emit setSlotDescription("getObject(QString)","Returns the id of an object with given name.",
                          QStringList("Name"), QStringList("Name of an object"));

  emit setSlotDescription("getObjectName(int)","Returns the name of an object with given id.",
                          QStringList("objectId"), QStringList("ID of an object"));

  emit setSlotDescription("hideObject(int)","Hide object with the given id.",
                          QStringList("objectId"), QStringList("ID of an object"));

  emit setSlotDescription("showObject(int)","Show object with the given id.",
                          QStringList("objectId"), QStringList("ID of an object"));

  emit setSlotDescription("setTarget(int,bool)","Set given object as target.",
                          QString("ObjectId,Target").split(","),
                          QString("id of the object, set object as target?").split(","));

  emit setSlotDescription("setSource(int,bool)","Set given object as source.",
                          QString("ObjectId,Source").split(","),
                          QString("id of the object, set object as source?").split(","));

  emit setSlotDescription("setObjectName(int,QString)","Set name of given object.",
                          QString("ObjectId,name").split(","),
                          QString("id of the object, the new name").split(","));

  emit setSlotDescription("groupObjects(idList,QString)","Group given Objects together.",
                          QString("objectIds,groupName").split(","),
                          QString("List of objects that should be grouped., Name of the group.").split(","));

  emit setSlotDescription("groupObjects(idList)","Group given Objects together.",
                          QStringList("objectIds"), QStringList("List of objects that should be grouped."));

  emit setSlotDescription("deleteObject(int)","Delete an object",
                          QStringList("objectId"), QStringList("Delete the given object."));

  emit setSlotDescription("copyObject(int)","Create a copy of an object",
                          QStringList("objectId"), QStringList("Object to copy."));

  emit setSlotDescription("setAllTarget()","Set All objects as targets",
                          QStringList(), QStringList());

  emit setSlotDescription("setAllSource()","Set All objects as source",
                          QStringList(), QStringList());

  emit setSlotDescription("clearAllTarget()","Clear targets",
                          QStringList(), QStringList());

  emit setSlotDescription("clearAllSource()","Clear sources",
                          QStringList(), QStringList());

  emit setSlotDescription("showAll()","Show all objects",
                          QStringList(), QStringList());

  emit setSlotDescription("hideAll()","Hide all objects",
                          QStringList(), QStringList());
}


/// Returns the id of an object with given name
int DataControlPlugin::getObject( QString _name ) {

  BaseObject* object = PluginFunctions::objectRoot()->childExists(_name);

  if ( object == 0)
    return -1;

  return object->id();
}

int DataControlPlugin::copyObject( int objectId ) {

  int newObject = PluginFunctions::copyObject( objectId );

  if ( newObject == -1 )
    std::cerr << "Unable to copy object" << std::endl;
  else
    emit updatedObject(newObject);

  return newObject;
}


/// Returns the name of an object with given id
QString DataControlPlugin::getObjectName( int objectId ) {

  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(objectId,object) ) {
    emit log(LOGERR,"getObjectName : unable to get object" );
    return QString("Unknown Object");
  } else
    return  object->name() ;

}

/// Hide object with the given id
void DataControlPlugin::hideObject( int objectId ) {

  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(objectId,object) )
    return;

  if ( object == 0)
    return;

  object->hide();
}

/// set the given Object as target
void DataControlPlugin::setTarget( int objectId, bool _target ) {

  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(objectId,object) )
    return;

  if ( object == 0)
    return;

  object->target( _target );
}

/// set the given Object as source
void DataControlPlugin::setSource( int objectId, bool _source ) {

  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(objectId,object) )
    return;

  if ( object == 0)
    return;

  object->source( _source );
}

/// set the name of the given Object as source
void DataControlPlugin::setObjectName( int objectId, QString _name ) {

  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(objectId,object) )
    return;

  if ( object == 0)
    return;

  object->setName( _name );
}

void DataControlPlugin::deleteObject( int objectId ) {

  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(objectId,object) )
    return;

  if ( object == 0)
    return;

  PluginFunctions::deleteObject(objectId);
}

/// Show object with the given id
void DataControlPlugin::showObject( int objectId ) {

  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(objectId,object) )
    return;

  if ( object == 0)
    return;

  object->show();
}

/// Group given Objects together
void DataControlPlugin::groupObjects(idList _objectIDs, QString _groupName) {

  QVector< BaseObject* > objs;

  for (uint i=0; i < _objectIDs.size(); i++){
    BaseObject* obj;
    if ( PluginFunctions::getObject(_objectIDs[i],obj) )
      objs.push_back(obj);
  }

  if (objs.size() == 0){
    emit log("No objects to group.");
    return;
  }

  //check if all objects have the same parent
  //abort if the parents differ
  BaseObject* parent = (objs[0])->parent();
  for ( int i = 1 ; i < objs.size() ; ++i)
    if ( parent != (objs[i])->parent() ){
      emit log("Cannot group Objects with different parents");
      return;
    }

  //create new group
  if (parent == 0)
    parent = PluginFunctions::objectRoot();
  GroupObject* groupItem = new GroupObject( "newGroup", dynamic_cast< GroupObject* >(parent));
  //set groupName
  if (_groupName == "")
    groupItem->setName("newGroup " + QString::number(groupItem->id()));
  else
    groupItem->setName( _groupName );
  parent->appendChild( dynamic_cast< BaseObject* >( groupItem ) );
  groupItem->setParent( parent );

  //append new children to group
  for ( int i = 0 ; i < objs.size() ; ++i) {
    (objs[i])->parent()->removeChild( objs[i] );
    (objs[i])->setParent( dynamic_cast< BaseObject* >( groupItem )  );
    groupItem->appendChild( objs[i] );
  }

  emit updatedObject(-1);
}

void DataControlPlugin::setAllTarget() {
  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS) ;
                                    o_it != PluginFunctions::objectsEnd(); ++o_it)
  o_it->target(true);
  emit activeObjectChanged();
  emit updatedObject(-1);
}

void DataControlPlugin::setAllSource() {
  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS) ;
                                    o_it != PluginFunctions::objectsEnd(); ++o_it)
  o_it->source(true);
  emit updatedObject(-1);
}

void DataControlPlugin::clearAllTarget() {
  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS) ;
                                    o_it != PluginFunctions::objectsEnd(); ++o_it)
  o_it->target(false);
  emit activeObjectChanged();
  emit updatedObject(-1);
}

void DataControlPlugin::clearAllSource() {
  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS) ;
                                    o_it != PluginFunctions::objectsEnd(); ++o_it)
  o_it->source(false);
  emit updatedObject(-1);
}

void DataControlPlugin::hideAll() {
  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS) ;
                                    o_it != PluginFunctions::objectsEnd(); ++o_it)
    o_it->hide();
  emit updateView();
}

void DataControlPlugin::showAll() {
  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS) ;
                                    o_it != PluginFunctions::objectsEnd(); ++o_it)
    o_it->show();
  emit updateView();
}
