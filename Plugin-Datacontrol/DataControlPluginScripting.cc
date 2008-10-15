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

  emit setSlotDescription("groupObjects(idList,QString)","Group given Objects together.",
                          QString("objectIds,groupName").split(","),
                          QString("List of objects that should be grouped., Name of the group.").split(","));

  emit setSlotDescription("groupObjects(idList)","Group given Objects together.",
                          QStringList("objectIds"), QStringList("List of objects that should be grouped."));

}


/// Returns the id of an object with given name
int DataControlPlugin::getObject( QString _name ) {
  
  BaseObject* object = PluginFunctions::objectRoot()->childExists(_name); 
  
  if ( object == 0)
    return -1;
  
  return object->id();
}

/// Returns the name of an object with given id
QString DataControlPlugin::getObjectName( int objectId ) {
  
  BaseObjectData* object;
  if ( ! PluginFunctions::get_object(objectId,object) ) {
    emit log(LOGERR,"getObjectName : unable to get object" ); 
    return QString("Unknown Object");
  } else 
    return  object->name() ;
  
}

/// Hide object with the given id
void DataControlPlugin::hideObject( int objectId ) {
  
  BaseObjectData* object;
  if ( ! PluginFunctions::get_object(objectId,object) ) 
    return;
  
  if ( object == 0)
    return;
  
  object->hide();
}

/// Show object with the given id
void DataControlPlugin::showObject( int objectId ) {
  
  BaseObjectData* object;
  if ( ! PluginFunctions::get_object(objectId,object) ) 
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
    if ( PluginFunctions::get_object(_objectIDs[i],obj) )
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
  
  emit updated_objects(-1);
}

