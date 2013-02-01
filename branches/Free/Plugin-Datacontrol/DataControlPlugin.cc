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

#include "DataControlPlugin.hh"

#include <QLayout>
#include <QGridLayout>
#include <QItemSelectionModel>

#include <iostream>
#include <ACG/GL/GLState.hh>
#include <QStringList>
#include <ACG/Scenegraph/BaseNode.hh>
#include <ACG/QtWidgets/QtMaterialDialog.hh>
#include <QModelIndexList>

#include <queue>

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include <OpenFlipper/common/GlobalOptions.hh>

#include <ObjectTypes/Light/Light.hh>

//******************************************************************************

const ACG::Vec4f base_color (0.0,0.0,0.5,1.0);
const ACG::Vec4f source_color (0.5,0.0,0.0,1.0);
const ACG::Vec4f target_color (0.0,0.5,0.2,1.0);

//******************************************************************************



DataControlPlugin::DataControlPlugin() :
        tool_(0),
        toolIcon_(0),
        MeshDialogLayout_(0),
        objectList_(0),
        locked(false),
        model_(0),
        view_(0),
        viewHeader_(0),
        onlyDown_(0),
        onlyUp_(0),
        headerPopupType_(0),
        targetAction_(0),
        sourceAction_(0),
        removeAction_(0)
{

}


/** \brief Plugin initialization
 *
 */
void DataControlPlugin::pluginsInitialized() {

  //set the slot descriptions
  setDescriptions();

  if ( ! OpenFlipper::Options::gui())
    return;

  QMenu* contextMenu = new QMenu("Object");

  //Target Objects
  QIcon icon = QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"datacontrol-hide-object.png");
  QAction* hideAction = new QAction(icon, tr("&Hide"), this);
  hideAction->setStatusTip(tr("Hide object"));
  connect(hideAction, SIGNAL(triggered()), this, SLOT(slotContextMenuHide()) );
  contextMenu->addAction(hideAction);

  //Target Objects
  icon = QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"datacontrol-target-object.png");
  targetAction_ = new QAction(icon, tr("&Target"), this);
  targetAction_->setCheckable(true);
  targetAction_->setStatusTip(tr("Set object as target"));
  connect(targetAction_, SIGNAL(triggered()), this, SLOT(slotContextMenuTarget()) );
  contextMenu->addAction(targetAction_);

  //Source Objects
  icon = QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"datacontrol-source-object.png");
  sourceAction_ = new QAction(icon, tr("&Source"), this);
  sourceAction_->setCheckable(true);
  sourceAction_->setStatusTip(tr("Set object as source"));
  connect(sourceAction_, SIGNAL(triggered()), this, SLOT(slotContextMenuSource()) );
  contextMenu->addAction(sourceAction_);

  contextMenu->addSeparator();
  
  //Remove Objects
  icon = QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"datacontrol-delete-item.png");
  removeAction_ = new QAction(icon, tr("&Remove"), this);
  removeAction_->setCheckable(false);
  removeAction_->setStatusTip(tr("Remove object"));
  connect(removeAction_, SIGNAL(triggered()), this, SLOT(slotContextMenuRemove()) );
  contextMenu->addAction(removeAction_);
  
  emit addContextMenuItem(contextMenu->menuAction() , DATA_ALL , CONTEXTOBJECTMENU);

  icon = QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"datacontrol-material.png");
  QAction* material = new QAction(icon, tr("Material Properties"), 0);
  connect (material, SIGNAL( triggered() ), this, SLOT ( slotMaterialProperties() ));
  emit addContextMenuItem(material , DATA_ALL , CONTEXTOBJECTMENU);

  QAction* copyMaterial = new QAction(tr("Copy Material Properties to Targeted Objects"), 0);
  connect (copyMaterial, SIGNAL( triggered() ), this, SLOT ( slotCopyMaterialToTargeted() ));
  emit addContextMenuItem(copyMaterial , DATA_ALL , CONTEXTOBJECTMENU);

  PluginFunctions::setDefaultViewObjectMarker (&objectMarker);
  PluginFunctions::setViewObjectMarker (&objectMarker);
  
  connect(tool_->lightSources, SIGNAL(stateChanged(int)), this, SLOT(slotShowLightSources(int)));
  //update light visibility, if layout was changed
  connect(model_, SIGNAL(layoutChanged ()), this, SLOT(slotShowLightSources()) );
}


//******************************************************************************

void DataControlPlugin::initializePlugin()
{
  if ( ! OpenFlipper::Options::gui())
    return;

  tool_ = new DatacontrolToolboxWidget();
  connect( tool_ , SIGNAL( keyEvent( QKeyEvent* ) ),
      this  , SLOT(slotKeyEvent ( QKeyEvent* ) ));
  QSize size(300, 300);
  tool_->resize(size);

  model_ = new TreeModel( );

  view_ = tool_->treeView;

  tool_->treeView->setModel(model_);

  view_->QTreeView::resizeColumnToContents(1);
  view_->QTreeView::resizeColumnToContents(2);
  view_->QTreeView::resizeColumnToContents(3);


  connect( model_,SIGNAL(dataChangedInside(int,int,const QVariant&) ),
      this,  SLOT(    slotDataChanged(int,int,const QVariant&)) );

  connect( model_,SIGNAL(   moveBaseObject(int,int) ),
      this,  SLOT( slotMoveBaseObject(int,int) ) );

  connect( view_,SIGNAL(customContextMenuRequested ( const QPoint &  )  ),
      this,SLOT(slotCustomContextMenuRequested ( const QPoint & ) ));

  connect( tool_->notSelected, SIGNAL(stateChanged ( int ) ),
      this, SLOT (slotBoundingBoxChange ( ) ));
  connect( tool_->sourceSelected, SIGNAL(stateChanged ( int ) ),
      this, SLOT (slotBoundingBoxChange ( ) ));
  connect( tool_->targetSelected, SIGNAL(stateChanged ( int ) ),
      this, SLOT (slotBoundingBoxChange ( ) ));


  viewHeader_ = tool_->treeView->header();
  viewHeader_->setContextMenuPolicy(Qt::CustomContextMenu);

  // connect the slot for the context menu
  connect( viewHeader_, SIGNAL(customContextMenuRequested ( const QPoint &  )  ),
      this,        SLOT(slotHeaderCustomContextMenuRequested ( const QPoint & ) ));

  toolIcon_ = new QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"datacontrol-toolbox.png");

  emit addToolbox("Data Control", tool_, toolIcon_);

  QIcon icon = QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"datacontrol-boundingBox.png");
  tool_->boundingBoxBtn->setIcon( icon );
  icon = QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"datacontrol-hide-object.png");
  tool_->visibleDataBtn->setIcon( icon );

  //hide additional boxes
  tool_->visibleDataBtn->setChecked(false);
  tool_->boundingBoxBtn->setChecked(false);
}


//******************************************************************************

/** \brief update drawing of objects when the active object changed
 *
 */
void DataControlPlugin::slotObjectSelectionChanged( int _identifier )
{
  if ( ! OpenFlipper::Options::gui())
    return;

  BaseObjectData* obj = 0;

  if ( PluginFunctions::getObject( _identifier, obj) )
    updateBoundingBox (obj);

  model_->objectChanged( _identifier );

  
  //check for changes in the tree
  BaseObject* object = 0;

  if ( PluginFunctions::getObject( _identifier, object) ){

    // if we are allowed to propagate down
    if ( onlyUp_ == 0 ){

      onlyDown_++;

      if ( object->isGroup() )
        propagateDownwards(object, 2); // 2 = source-target

      onlyDown_--;
    }

    // if we are allowed to propagate up
    if ( onlyDown_ == 0 ){

      onlyUp_++;

      propagateUpwards(object->parent(), 2); // 2 = source-target

      onlyUp_--;
    }

      
  }
}


//******************************************************************************

/** \brief Update the model if the visibility of an object changed
 *
 * @param _identifier id of an object
 */
void DataControlPlugin::slotVisibilityChanged( int _identifier ){

  if ( ! OpenFlipper::Options::gui())
    return;

  // if onlyUp_ > 0 --> _identifier is a group and the selection
  // does not have to be applied
  if (onlyUp_ == 0){
    //inform the model
    model_->objectChanged( _identifier );
  }

  //check for changes in the tree
  BaseObject* obj = 0;

  if ( PluginFunctions::getObject( _identifier, obj) ){

    // if we are allowed to propagate up
    if ( onlyDown_ == 0 ){
    
      onlyUp_++;

      propagateUpwards(obj->parent(), 1); // 1 = visibilty

      onlyUp_--;  

    }

    // if we are allowed to propagate down
    if ( onlyUp_ == 0 ){

      onlyDown_++;
      
      if ( obj->isGroup() )
        propagateDownwards(obj, 1); // 1 = visibilty

      onlyDown_--;
    }
  }

  BaseObjectData* object = 0;

  if ( PluginFunctions::getObject( _identifier, object) )
    updateBoundingBox (object);

}


//******************************************************************************

/** \brief Update the model if properties of an object changed
 *
 * @param _identifier id of an object
 */
void DataControlPlugin::slotObjectPropertiesChanged( int _identifier ){

  if ( ! OpenFlipper::Options::gui())
    return;

  model_->objectChanged( _identifier );
}


//******************************************************************************

/** \brief Update the model if a file has been opened
 *
 * @param _id id of an object
 */
void DataControlPlugin::fileOpened(int _id){

  if ( ! OpenFlipper::Options::gui())
    return;

  BaseObject* obj = 0;

  if ( PluginFunctions::getObject(_id, obj) )
    model_->objectAdded(obj);

  // Only if the added object was a light source, we will traverse the objects!
  if ( obj->dataType() == DATA_LIGHT)
    slotShowLightSources(tool_->lightSources->checkState());

  view_->resizeColumnToContents(0);
}


//******************************************************************************

/** \brief Update the model if an empty object has been added
 *
 * @param _id id of an object
 */
void DataControlPlugin::addedEmptyObject(int _id){
  fileOpened(_id);
}

//******************************************************************************

/** \brief an object was deleted. delete it internally
 *
 * @param _id id of the object
 */
void DataControlPlugin::objectDeleted(int _id){

  if ( ! OpenFlipper::Options::gui())
    return;

  model_->objectDeleted(_id);
}

//******************************************************************************

/** \brief a key event occurred
 *
 * @param _event the event that occurred
 */
void DataControlPlugin::slotKeyEvent( QKeyEvent* _event )
{

  if ( _event->modifiers() == Qt::ControlModifier ) {
    switch (_event->key()) {
      case Qt::Key_A :
          setAllTarget();
        return;
      default:
        return;
    }
  }

  switch (_event->key()) {
    case Qt::Key_Delete :
        slotPopupRemove();
      return;
    default:
      return;
  }

}


//******************************************************************************

/** \brief emit the right updates when the model changed
 *
 * @param _id     Object id that was changed
 * @param _column Which column changed
 * @param _value  What is the new value?
 */
void DataControlPlugin::slotDataChanged ( int _id, int _column, const QVariant& _value)
{

  //get the corresponding baseObject
  BaseObject* obj = 0;
  if ( !PluginFunctions::getObject( _id, obj) )
    return;
  
  switch ( _column ) {
    // Name
    case 0:
      obj->setName( _value.toString() );
      break;

    // show/hide
    case 1:
      obj->visible( _value.toBool() );
      break;

    // source
    case 2:
      obj->source( _value.toBool() );
      break;

    // target
    case 3:
      obj->target( _value.toBool() );
      break;

    default:
      break;
  }
}


//******************************************************************************

/** \brief Gets called when an object was moved via drag n drop
 *
 * @param _id          id of the object
 * @param _newParentId id of the new parent
 */
void DataControlPlugin::slotMoveBaseObject(int _id, int _newParentId){

  BaseObject* obj = 0;

  if ( !PluginFunctions::getObject(_id, obj) )
    return;

  BaseObject* parent = 0;

  if ( !PluginFunctions::getObject(_newParentId, parent) )
    return;

  BaseObject* oldParent = obj->parent();

  //set new parent
  obj->setParent( parent );

  //if oldParent is an empty group -> delete it
  if ( oldParent != PluginFunctions::objectRoot() && oldParent->childCount() == 0 )
    emit deleteObject( oldParent->id() );
}


//******************************************************************************

void DataControlPlugin::slotShowLightSources( int _state ) {

    int rows = model_->rowCount();

    for(int i = 0; i < rows; ++i) {
        TreeItem* item = model_->getItem(model_->index(i,0));
        if(item->dataType() == DATA_LIGHT) {
          view_->setRowHidden(i, model_->parent(model_->index(i,0)), !(_state == Qt::Checked));
        }else{
          //always show, if it is not a light
          view_->setRowHidden(i, model_->parent(model_->index(i,0)), false);
        }
    }
}


void DataControlPlugin::slotShowLightSources()
{
  slotShowLightSources( tool_->lightSources->checkState() );
}
//******************************************************************************

/** \brief Load Groups from ini file
 *
 * @param _ini an ini file
 */
void DataControlPlugin::loadIniFileOptionsLast( INIFile& _ini ) {

  if ( _ini.section_exists( "BoundingBox" ) && OpenFlipper::Options::gui() )
  {
    bool value;
    if (_ini.get_entry(value, "BoundingBox","notSelected"))
      tool_->notSelected->setChecked (value);
    if (_ini.get_entry(value, "BoundingBox","sourceSelected"))
      tool_->sourceSelected->setChecked (value);
    if (_ini.get_entry(value, "BoundingBox","targetSelected"))
      tool_->targetSelected->setChecked (value);
  }

  if ( !_ini.section_exists( "Groups" ) )
    return;

  // Names of all groups
  QStringList groupNames;

  // names of the primary groups
  QStringList rootGroup;

  // Get the list of group names to the file
  _ini.get_entry(groupNames,"Groups","groups");

  // Get the primary group names to the file
  _ini.get_entry(rootGroup,"Groups","rootGroup");

  //list of groups
  QVector< BaseObject* > groups;

  // Go over one level of the groups
  while ( rootGroup.size() > 0 ) {
    QString current = rootGroup[0];
    rootGroup.removeFirst();

    QStringList groupChildren;
    QStringList elementChildren;

    _ini.get_entry(elementChildren ,current,"children");
    _ini.get_entry(groupChildren ,current,"subgroups");

    // if we get a parent item, scan the tree for it or use the root node otherwise
    BaseObject* parentItem;
    QString parentName;
    if ( _ini.get_entry(parentName,current,"parent") ) {
      parentItem = PluginFunctions::objectRoot()->childExists(parentName);
      if ( parentItem == 0 )
        parentItem = PluginFunctions::objectRoot();
    } else
      parentItem = PluginFunctions::objectRoot();

    rootGroup << groupChildren;

    // check if this group already exists
    BaseObject* group =  PluginFunctions::objectRoot()->childExists( current );

    // group does not exist
    if ( !group ) {

      group = dynamic_cast< BaseObject* >( new GroupObject( current, dynamic_cast< GroupObject* >(parentItem ) ) );

      emit emptyObjectAdded( group->id() );

      // in the groups vector we only need the lowest groups
      // because they are used recursively
      int p = groups.indexOf( group->parent() );
      if ( p > -1 )
        groups.remove( p );

      groups.push_back( group );
    }

    // process children
    for ( int i = 0 ; i < elementChildren.size() ; ++i ) {
      BaseObject* childItem =  PluginFunctions::objectRoot()->childExists( elementChildren[i] );
      if ( childItem ) {
        childItem->setParent(group);
      }
    }
  }
}


//******************************************************************************

/** \brief Save groups to ini file
 *
 * @param _ini an ini file
 */
void DataControlPlugin::saveIniFileOptions( INIFile& _ini ) {

  std::queue< BaseObject* > children;
  children.push( PluginFunctions::objectRoot() );

  std::vector< BaseObject* > groups;

  // Get all groups from the tree
  while ( ! children.empty() ) {
    BaseObject* item = children.front();
    children.pop();

    for ( int i = 0 ; i < item->childCount(); ++i )
      if ( item->child(i)->dataType(DATA_GROUP))
        children.push( item->child(i) );

    if ( item->dataType(DATA_GROUP) && (item != PluginFunctions::objectRoot() ) )
      groups.push_back(item);
  }

  // Names of all groups
  QStringList groupNames;

  // names of the primary groups
  QStringList rootGroup;

  for ( uint i = 0 ; i < groups.size() ; ++i ) {
    groupNames.push_back( groups[i]->name() );

    _ini.add_entry(groups[i]->name(),"groupname",groups[i]->name());

    // write the name of the parent
    if ( ( groups[i]->parent() != 0 ) && ( groups[i]->parent() != PluginFunctions::objectRoot() ) )
      _ini.add_entry(groups[i]->name(),"parent",groups[i]->parent()->name());

    if ( groups[i]->parent() == PluginFunctions::objectRoot() )
      rootGroup.push_back( groups[i]->name() );

    // Write a list of this groups children
    QStringList groupchildren;
    QStringList elementchildren;
    for ( int j = 0 ; j < groups[i]->childCount(); ++j ) {
      if  ( groups[i]->child(j)->dataType(DATA_GROUP) )
        groupchildren.push_back( groups[i]->child(j)->name() );
      else
        elementchildren.push_back( groups[i]->child(j)->name() );
    }

    _ini.add_entry(groups[i]->name(),"subgroups",groupchildren);
    _ini.add_entry(groups[i]->name(),"children",elementchildren);
  }

  // Write the list of group names to the file
  _ini.add_entry("Groups","groups",groupNames);

  // Write the primary group names to the file
  _ini.add_entry("Groups","rootGroup",rootGroup);

  if ( OpenFlipper::Options::gui() ) {

    _ini.add_entry("BoundingBox","notSelected",tool_->notSelected->isChecked ());
    _ini.add_entry("BoundingBox","sourceSelected",tool_->sourceSelected->isChecked ());
    _ini.add_entry("BoundingBox","targetSelected",tool_->targetSelected->isChecked ());
  }

}


//******************************************************************************

/** \brief Recursively update a column up to the root of the tree
 *
 * @param _obj    Object to start with
 * @param _column Column to propagate
 */
void DataControlPlugin::propagateUpwards(BaseObject* _obj, int _column ){

  
  if ( _obj == PluginFunctions::objectRoot() || (!_obj->isGroup()) )
    return;

  QList< BaseObject* > children = _obj->getLeafs();
  bool changed = false;
  bool value    = false;
  bool value2   = false;


  switch ( _column ){

    case 1: //VISIBILTY

      for (int i=0; i < children.size(); i++)
        value |= children[i]->visible();

      _obj->visible( value );

      changed = true;

      break;

    case 2: //SOURCE-TARGET

      for (int i=0; i < children.size(); i++){
        value  |= children[i]->source();
        value2 |= children[i]->target();
      }

      if (_obj->source() != value){
        _obj->source( value );
        changed = true;
      }

      if (_obj->target() != value2){
        _obj->target( value2 );
        changed = true;
      }

      break;

    default:
      break;
  }

  if ( changed )
    propagateUpwards( _obj->parent(), _column );
}

//******************************************************************************

/** \brief Recursively update a column up to the root of the tree
 *
 * @param _obj    object to start with
 * @param _column Column to be propagated
 */
void DataControlPlugin::propagateDownwards(BaseObject* _obj, int _column ){

  for (int i=0; i < _obj->childCount(); i++){

    BaseObject* current = _obj->child(i);

    bool changed = false;

    switch ( _column ){

      case 1: //VISIBILTY

        if ( current->visible() != _obj->visible() ){

          current->visible( _obj->visible() );

          changed = true;
        }
        break;

      case 2: //SOURCE-TARGET

        if ( current->source() != _obj->source() ){
          current->source( _obj->source() );
          changed = true;
        }

        if ( current->target() != _obj->target() ){
          current->target( _obj->target() );
          changed = true;
        }

        break;

      default:
        break;
    }

    if ( changed || current->isGroup() ){
      propagateDownwards(current, _column);

    }
  }
}

//******************************************************************************

/** \brief Bounding box visibility selection changed
 */
void DataControlPlugin::slotBoundingBoxChange( )
{
  for (PluginFunctions::ObjectIterator o_it; o_it != PluginFunctions::objectsEnd(); ++o_it)  {
    updateBoundingBox (o_it);
  }

  emit updateView();
}

//******************************************************************************

/** \brief Update state of bounding box node
 *
 * @param _obj object
 */
void DataControlPlugin::updateBoundingBox(BaseObjectData* _obj)
{
  if (tool_->notSelected->isChecked () ||
      (_obj->source () && tool_->sourceSelected->isChecked ()) ||
      (_obj->target () && tool_->targetSelected->isChecked ()))
  {
    _obj->boundingBoxNode()->set_status( ACG::SceneGraph::BaseNode::Active );

    ACG::Vec4f color = base_color;

    if (_obj->source () && tool_->sourceSelected->isChecked ())
      color += source_color;

    if (_obj->target () && tool_->targetSelected->isChecked ())
      color += target_color;

    _obj->boundingBoxNode()->set_base_color (color);
  }
  else
    _obj->boundingBoxNode()->set_status( ACG::SceneGraph::TranslationManipulatorNode::HideNode );

}

//******************************************************************************
/** \brief Save settings before application is closed
 *
 * @param _ini reference to ini file
 */
void DataControlPlugin::saveOnExit(INIFile& _ini){
  _ini.add_entry("BoundingBox","notSelected",tool_->notSelected->isChecked ());
  _ini.add_entry("BoundingBox","sourceSelected",tool_->sourceSelected->isChecked ());
  _ini.add_entry("BoundingBox","targetSelected",tool_->targetSelected->isChecked ());
}


Q_EXPORT_PLUGIN2( datacontrolplugin , DataControlPlugin );

