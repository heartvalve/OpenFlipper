/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2009 by Computer Graphics Group, RWTH Aachen      *
 *                           www.openflipper.org                             *
 *                                                                           *
 *---------------------------------------------------------------------------*
 *  This file is part of OpenFlipper.                                        *
 *                                                                           *
 *  OpenFlipper is free software: you can redistribute it and/or modify      *
 *  it under the terms of the GNU Lesser General Public License as           *
 *  published by the Free Software Foundation, either version 3 of           *
 *  the License, or (at your option) any later version with the              *
 *  following exceptions:                                                    *
 *                                                                           *
 *  If other files instantiate templates or use macros                       *
 *  or inline functions from this file, or you compile this file and         *
 *  link it with other files to produce an executable, this file does        *
 *  not by itself cause the resulting executable to be covered by the        *
 *  GNU Lesser General Public License. This exception does not however       *
 *  invalidate any other reasons why the executable file might be            *
 *  covered by the GNU Lesser General Public License.                        *
 *                                                                           *
 *  OpenFlipper is distributed in the hope that it will be useful,           *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU Lesser General Public License for more details.                      *
 *                                                                           *
 *  You should have received a copy of the GNU LesserGeneral Public          *
 *  License along with OpenFlipper. If not,                                  *
 *  see <http://www.gnu.org/licenses/>.                                      *
 *                                                                           *
\*===========================================================================*/

/*===========================================================================*\
 *                                                                           *
 *   $Revision$                                                       *
 *   $Author$                                                      *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/




#include "DataControlPlugin.hh"

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>


//******************************************************************************

void DataControlPlugin::setDescriptions(){

  emit setSlotDescription("getObject(QString)",tr("Returns the id of an object with given name."),
                          QStringList(tr("Name")), QStringList(tr("Name of an object")));

  emit setSlotDescription("getObjectName(int)",tr("Returns the name of an object with given id."),
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

/** \brief Hide object with the given id
 *
 * @param objectId id of an object
 */
void DataControlPlugin::hideObject( int objectId ) {

  BaseObjectData* object;
  if ( PluginFunctions::getObject(objectId,object) ){
    object->hide();

    emit visibilityChanged( object->id() );
  }

  emit updateView();
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

    emit objectSelectionChanged( object->id() );
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

    emit objectSelectionChanged( object->id() );
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

    emit objectPropertiesChanged( object->id() );
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

  BaseObjectData* object;
  if ( PluginFunctions::getObject(objectId,object) ){
    object->show();

    emit visibilityChanged( object->id() );
  }

  emit updateView();
}


//******************************************************************************

/** \brief Group given Objects together
 *
 * @param _objectIDs list of object ids
 * @param _groupName the name of the new group
 */
void DataControlPlugin::groupObjects(idList _objectIDs, QString _groupName) {
  QVector< BaseObject* > objs;

  for (uint i=0; i < _objectIDs.size(); i++){
    BaseObject* obj;
    if ( PluginFunctions::getObject(_objectIDs[i],obj) )
      objs.push_back(obj);
  }

  if (objs.size() == 0){
    emit log(tr("No objects to group."));
    return;
  }

  //check if all objects have the same parent
  //abort if the parents differ
  bool visible = (objs[0])->visible();
  bool target  = (objs[0])->target();
  bool source  = (objs[0])->source();

  BaseObject* parent = (objs[0])->parent();
  for ( int i = 1 ; i < objs.size() ; ++i){
    if ( parent != (objs[i])->parent() ){
      emit log(tr("Cannot group Objects with different parents"));
      return;
    }

    visible |= (objs[i])->visible();
    target  |= (objs[i])->target();
    source  |= (objs[i])->source();
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
  parent->appendChild( dynamic_cast< BaseObject* >( groupItem ) );
  groupItem->setParent( parent );

  emit emptyObjectAdded( groupItem->id() );

  //append new children to group
  for ( int i = 0 ; i < objs.size() ; ++i) {
    (objs[i])->parent()->removeChild( objs[i] );
    (objs[i])->setParent( dynamic_cast< BaseObject* >( groupItem )  );
    groupItem->appendChild( objs[i] );

    //inform everyone that the parent changed
    emit objectPropertiesChanged( (objs[i])->id() );
  }

  //update target/source state
  groupItem->visible(visible);
  groupItem->target(target);
  groupItem->source(source);

  emit objectPropertiesChanged( groupItem->id() );
  emit visibilityChanged ( groupItem->id() );
  emit objectSelectionChanged ( groupItem->id() );
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

    emit objectSelectionChanged( o_it->id() );
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

    emit objectSelectionChanged( o_it->id() );
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

    emit objectSelectionChanged( o_it->id() );
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

    emit objectSelectionChanged( o_it->id() );
  }
}


//******************************************************************************

/** \brief Hide all objects
 *
 */
void DataControlPlugin::hideAll() {

  DataType type = DATA_ALL;

  //try to find dataType restriction if called by contextMenu
  QAction* action = dynamic_cast< QAction* > ( sender() );

  if ( action != 0 && action->data().isValid() )
    type = (DataType) action->data().toUInt();

  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS,type) ;
                                    o_it != PluginFunctions::objectsEnd(); ++o_it){
    o_it->hide();

    emit visibilityChanged( o_it->id() );
  }

  emit updateView();
}


//******************************************************************************

/** \brief Show all objects
 *
 */
void DataControlPlugin::showAll() {

  DataType type = DATA_ALL;

  //try to find dataType restriction if called by contextMenu
  QAction* action = dynamic_cast< QAction* > ( sender() );

  if ( action != 0 && action->data().isValid() )
    type = (DataType) action->data().toUInt();

  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS, type); o_it != PluginFunctions::objectsEnd(); ++o_it){
    o_it->show();

    emit visibilityChanged( o_it->id() );
  }

  emit updateView();
}
