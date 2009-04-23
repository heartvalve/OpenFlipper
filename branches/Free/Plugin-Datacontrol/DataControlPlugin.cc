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

//******************************************************************************

/** \brief Plugin initialization
 *
 */
void DataControlPlugin::pluginsInitialized() {

  //set the slot descriptions
  setDescriptions();

  QMenu* contextMenu = new QMenu("Object selection");

  //Target Objects
  QAction* hideAction = new QAction(tr("&Hide"), this);
  hideAction->setStatusTip(tr("Hide object"));
  connect(hideAction, SIGNAL(triggered()), this, SLOT(slotContextMenuHide()) );
  contextMenu->addAction(hideAction);

  //Target Objects
  targetAction_ = new QAction(tr("&target"), this);
  targetAction_->setCheckable(true);
  targetAction_->setStatusTip(tr("Set object as target"));
  connect(targetAction_, SIGNAL(triggered()), this, SLOT(slotContextMenuTarget()) );
  contextMenu->addAction(targetAction_);

  //Source Objects
  sourceAction_ = new QAction(tr("&source"), this);
  sourceAction_->setCheckable(true);
  sourceAction_->setStatusTip(tr("Set object as source"));
  connect(sourceAction_, SIGNAL(triggered()), this, SLOT(slotContextMenuSource()) );
  contextMenu->addAction(sourceAction_);

  emit addContextMenuItem(contextMenu->menuAction() , DATA_ALL , CONTEXTOBJECTMENU);

  QIcon icon = QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"datacontrol-material.png");
  QAction* material = new QAction(icon, "Material Properties", 0);
  connect (material, SIGNAL( triggered() ), this, SLOT ( slotMaterialProperties() ));

  emit addContextMenuItem(material , DATA_ALL , CONTEXTOBJECTMENU);
}


//******************************************************************************

/** \brief initialize the toolBox
 *
 * @param _widget a reference to the toolBox
 * @return returns if the toolbox was created successfully
 */
bool DataControlPlugin::initializeToolbox(QWidget*& _widget)
{
   locked = false;
   tool_ = new DatacontrolToolboxWidget();
   connect( tool_ , SIGNAL( keyEvent( QKeyEvent* ) ),
            this  , SLOT(slotKeyEvent ( QKeyEvent* ) ));
   _widget = tool_;
   QSize size(300, 300);
   tool_->resize(size);
   MeshDialogLayout_ = new QGridLayout( tool_);

   model_ = new TreeModel( );

   view_ = new QTreeView(0);
   view_->setModel(model_);

   view_->setMinimumHeight(400);

   view_->QTreeView::resizeColumnToContents(1);
   view_->QTreeView::resizeColumnToContents(2);
   view_->QTreeView::resizeColumnToContents(3);

   view_->setContextMenuPolicy(Qt::CustomContextMenu);

   view_->setDragEnabled(true);
   view_->setAcceptDrops(true);
   view_->setDropIndicatorShown(true);

   view_->setSelectionBehavior(QAbstractItemView::SelectRows);
   view_->setSelectionMode(QAbstractItemView::ExtendedSelection);


   connect( model_,SIGNAL(dataChangedInside(BaseObject*,int) ),
            this,  SLOT(  slotDataChanged(BaseObject*,int)) );

   connect( model_ , SIGNAL( modelAboutToBeReset() ),
            this , SLOT(slotModelAboutToReset() ) );

   connect( model_ , SIGNAL( modelReset() ),
            this , SLOT( slotModelResetComplete() ) );

   connect( view_,SIGNAL(customContextMenuRequested ( const QPoint &  )  ),
            this,SLOT(slotCustomContextMenuRequested ( const QPoint & ) ));


   MeshDialogLayout_->addWidget( view_ , 0,0  );

   viewHeader_ = view_->header();
   viewHeader_->setContextMenuPolicy(Qt::CustomContextMenu);

   // connect the slot for the context menu
   connect( viewHeader_, SIGNAL(customContextMenuRequested ( const QPoint &  )  ),
            this,        SLOT(slotHeaderCustomContextMenuRequested ( const QPoint & ) ));

   return true;
}


//******************************************************************************

/** \brief update drawing of objects when the active object changed
 *
 */
void DataControlPlugin::slotObjectSelectionChanged( int _identifier )
{

  BaseObjectData* obj = 0;

  if ( PluginFunctions::getObject( _identifier, obj) ){

    if ( obj->target() ) {
      obj->materialNode()->disable_blending();
      OpenMesh::Vec4f base_color = obj->materialNode()->base_color();
      base_color[3] = 1.0;
      obj->materialNode()->set_base_color(base_color);
      OpenMesh::Vec4f ambient_color = obj->materialNode()->ambient_color();
      ambient_color[3] = 1.0;
      obj->materialNode()->set_ambient_color(ambient_color);
      OpenMesh::Vec4f diffuse_color = obj->materialNode()->diffuse_color();
      diffuse_color[3] = 1.0;
      obj->materialNode()->set_diffuse_color(diffuse_color);
    }  else {
      obj->materialNode()->enable_blending();
      OpenMesh::Vec4f base_color = obj->materialNode()->base_color();
      base_color[3] = 0.4;
      obj->materialNode()->set_base_color(base_color);
      OpenMesh::Vec4f ambient_color = obj->materialNode()->ambient_color();
      ambient_color[3] = 0.4;
      obj->materialNode()->set_ambient_color(ambient_color);
      OpenMesh::Vec4f diffuse_color = obj->materialNode()->diffuse_color();
      diffuse_color[3] = 0.4;
      obj->materialNode()->set_diffuse_color(diffuse_color);
    }
  }

  model_->objectChanged( _identifier );

  emit updateView();
}


//******************************************************************************

/** \brief Update the model if the visibility of an object changed
 * 
 * @param _identifier id of an object
 */
void DataControlPlugin::slotVisibilityChanged( int _identifier ){
  model_->objectChanged( _identifier );
}


//******************************************************************************

/** \brief Update the model if properties of an object changed
 * 
 * @param _identifier id of an object
 */
void DataControlPlugin::slotObjectPropertiesChanged( int _identifier ){
  model_->objectChanged( _identifier );
}


//******************************************************************************

/** \brief Update the model if an object was deleted
 * 
 * @param _identifier id of an object
 */
void DataControlPlugin::slotObjectUpdated( int _identifier ){

  if (_identifier == -1)
    model_->objectChanged( _identifier );
}


//******************************************************************************

/** \brief Update the model if a file has been opened
 * 
 * @param _id id of an object
 */
void DataControlPlugin::fileOpened(int){
  model_->objectChanged(-1);
}


//******************************************************************************

/** \brief Update the model if an empty object has been added
 * 
 * @param _id id of an object
 */
void DataControlPlugin::addedEmptyObject(int){
  model_->objectChanged(-1);
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
 * @param topLeft index in the model
 * @param
 */
void DataControlPlugin::slotDataChanged ( BaseObject* _obj, int _column)
{

  if (_obj != 0){
// std::cerr << "slotDataChanged obj " << _obj->id() << " column " << _column << std::endl;
    switch ( _column ) {
      // Name
      case 0:
        emit objectPropertiesChanged( _obj->id() );
        view_->expandToDepth(0);
        break;

      // show/hide
      case 1:
        emit visibilityChanged( _obj->id() );
        emit updateView();
        break;

      // source
      case 2:
        emit objectSelectionChanged( _obj->id() );
        break;

      // target
      case 3:
        emit objectSelectionChanged( _obj->id() );
        break;

      default:
        break;
    }
  }
}


//******************************************************************************

/** \brief Store the expanded status of all objects when the model wants to reset
 *
 */
void DataControlPlugin::slotModelAboutToReset(){

  isExpanded_.clear();

  QVector< BaseObject* > stack;

  stack.push_back( PluginFunctions::objectRoot() );

  BaseObject* item;


  do{ // Store the expanded state of all objects

    item = stack.front();

    stack.pop_front();

    for(int i=0; i < item->childCount(); i++)
      stack.push_back( item->child(i) );

    isExpanded_[ item ] = view_->isExpanded( model_->getModelIndex(item, 0 ) );

  } while ( !stack.isEmpty() );

}


//******************************************************************************

/** \brief restore the expanded status of all objects after reset
 *
 */
void DataControlPlugin::slotModelResetComplete(){

  // first expandAll so that alle ModelIndices in the TreeModel are recreated
  view_->expandAll();

  // and then restore the expanded state
  std::map< BaseObject*, bool>::iterator it;

  for ( it=isExpanded_.begin() ; it != isExpanded_.end(); it++ ){
    QModelIndex index = model_->getModelIndex( (*it).first, 0 );

    if (index.isValid())
      view_->setExpanded( index, (*it).second);
  }
}


//******************************************************************************

/** \brief Load Groups from ini file
 *
 * @param _ini an ini file
 */
void DataControlPlugin::loadIniFileOptionsLast( INIFile& _ini ) {
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

      parentItem->appendChild(group);
      group->setParent(parentItem);
    }

    // process children
    for ( int i = 0 ; i < elementChildren.size() ; ++i ) {
      BaseObject* childItem =  PluginFunctions::objectRoot()->childExists( elementChildren[i] );
      if ( childItem ) {
        childItem->parent()->removeChild(childItem);
        childItem->setParent(group);
        group->appendChild(childItem);
      }
    }
  }

  emit updatedObject(-1);
}


//******************************************************************************

/** \brief Save groups to ini file
 *
 * @param _ini an ini file
 */
void DataControlPlugin::saveIniFileOptions( INIFile& _ini ) {
  if ( !_ini.section_exists( "Groups" ) )
    _ini.add_section("Groups");

  std::queue< BaseObject* > children;
  children.push( PluginFunctions::objectRoot() );

  std::vector< BaseObject* > groups;

  // Get all groups from the tree
  while ( children.size() > 0 ) {
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

    if ( !_ini.section_exists( groups[i]->name() ) )
      _ini.add_section(groups[i]->name());

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
}


Q_EXPORT_PLUGIN2( datacontrolplugin , DataControlPlugin );

