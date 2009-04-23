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
#include <OpenFlipper/common/GlobalOptions.hh>

#include <ACG/QtWidgets/QtMaterialDialog.hh>

#include <QInputDialog>
#include <QMessageBox>
#include "Menu.hh"


//******************************************************************************

/** \brief Slot for Remove action in ContextMenu
 * 
 */
void DataControlPlugin::slotPopupRemove (  ) {
  QItemSelectionModel* selection = view_->selectionModel();

  if (selection == 0) return;

  // Get all selected rows
  QModelIndexList indexList = selection->selectedRows();


  QMessageBox msgBox;
  msgBox.setText("Do you really want to remove the selected objects?");
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
  msgBox.setDefaultButton(QMessageBox::No);

  QString text;

  for ( int i = 0 ; i < indexList.size() ; ++i) {
    BaseObject* deleteItem = model_->getItem( indexList[i] );
    text += deleteItem->name() + "\n";
  }
  msgBox.setDetailedText(text);

  if ( msgBox.exec() == QMessageBox::No)
    return;

  for ( int i = 0 ; i < indexList.size() ; ++i) {

    BaseObject* deleteItem = model_->getItem( indexList[i] );

    // Skip the root item
    if ( model_->isRoot( deleteItem ) )
      continue;

    // remove the whole subtree below this item
    deleteItem->deleteSubtree();

    // remove the item itself from the parent
    deleteItem->parent()->removeChild(deleteItem);

    // delete it
    delete deleteItem;
  }

  emit updateView();
  emit updatedObject(-1);
}


//******************************************************************************

/** \brief Slot for Ungroup action in ContextMenu
 * 
 */
void DataControlPlugin::slotUngroup (  ) {
  QItemSelectionModel* selection = view_->selectionModel();

  // Get all selected rows
  QModelIndexList indexList = selection->selectedRows ( 0 );

  //remove complete group if selected item was a group
  BaseObject* group = model_->getItem( indexList[0]);

  for (int i=0; i < group->childCount(); i++){
    group->child(i)->setParent(group->parent());
    group->parent()->appendChild( group->child(i) );
  }

  group->parent()->removeChild(group);

  delete group;

  //because the parent of all items in the group changed
  emit objectPropertiesChanged( -1 );
}


//******************************************************************************

/** \brief Slot for Copy action in ContextMenu
 * 
 */
void DataControlPlugin::slotCopy() {
  QItemSelectionModel* selection = view_->selectionModel();

  if (selection == 0) return;

  // Get all selected rows
  QModelIndexList indexList = selection->selectedRows();

  for ( int i = 0 ; i < indexList.size() ; ++i) {

    BaseObject* copyItem = model_->getItem( indexList[i] );

    int newObject = PluginFunctions::copyObject(copyItem->id());

    if ( newObject == -1 ) {
      emit log(LOGERR, "Unable to copy object" );
      continue;
    } else {
      emit emptyObjectAdded( newObject );
      emit updatedObject( newObject );
    }
  }

  emit updateView();
}


//******************************************************************************

/** \brief Slot for Group action in ContextMenu
 * 
 */
void DataControlPlugin::slotGroup() {
  QItemSelectionModel* selection = view_->selectionModel();

  // Get all selected rows
  QModelIndexList indexList = selection->selectedRows ( 0 );

  //check if all objects have the same parent
  //abort if the parents differ
  bool visible = (model_->getItem( indexList[0]))->visible();
  bool target  = (model_->getItem( indexList[0]))->target();
  bool source  = (model_->getItem( indexList[0]))->source();

  BaseObject* parent = (model_->getItem( indexList[0]))->parent();
  for ( int i = 1 ; i < indexList.size() ; ++i) {
    BaseObject* item = model_->getItem( indexList[i] );
    if (parent != item->parent()){
      emit log("Cannot group Objects with different parents");
      return;
    }

    //remember if at least on child was target/source
    visible |= (model_->getItem( indexList[i]))->visible();
    target  |= (model_->getItem( indexList[i]))->target();
    source  |= (model_->getItem( indexList[i]))->source();
  }

  //create new group
  if (parent == 0)
    parent = PluginFunctions::objectRoot();
  GroupObject* groupItem = new GroupObject( "newGroup", dynamic_cast< GroupObject* >(parent));
  groupItem->setName("newGroup " + QString::number(groupItem->id()));
  parent->appendChild( dynamic_cast< BaseObject* >( groupItem ) );
  groupItem->setParent( parent );

  //append new children to group
  for ( int i = 0 ; i < indexList.size() ; ++i) {
    BaseObject* item = model_->getItem( indexList[i] );
    item->parent()->removeChild(item);
    item->setParent( dynamic_cast< BaseObject* >( groupItem )  );
    groupItem->appendChild(item);
  }

  //update target/source state
  groupItem->visible(visible);
  groupItem->target(target);
  groupItem->source(source);

  emit visibilityChanged( groupItem->id() );
  emit objectPropertiesChanged( groupItem->id() );
  emit objectSelectionChanged ( groupItem->id() );

  //because the parent of all items in the group changed
  emit objectPropertiesChanged( -1 );
}


//******************************************************************************

/** \brief ContextMenu requested - creates the contextMenu
 * 
 * @param _pos Position where it was requested
 */
void DataControlPlugin::slotCustomContextMenuRequested ( const QPoint & _pos ) {
  popupIndex_ = view_->indexAt(_pos);

  if (!popupIndex_.isValid())
    return;

  BaseObject* item = model_->getItem(popupIndex_);


  QItemSelectionModel* selection = view_->selectionModel();

  // Get all selected rows
  QModelIndexList indexList = selection->selectedRows ( 0 );
  int selectedRows = indexList.size();

  Menu    menu(0);
  QAction* action;
  QIcon icon;
  if ( selectedRows > 1 ) {
    action = menu.addAction("Copy",this,SLOT ( slotCopy() ));
    icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"edit-copy.png");
    action->setIcon(icon);
    menu.addAction("Group",this,SLOT ( slotGroup() ));
    menu.addSeparator();
    menu.addAction("Remove",this,SLOT ( slotPopupRemove() ));
   }else
    // check if the item is a group item
    if ( item->isGroup() ) {
      action = menu.addAction("Zoom to objects",this,SLOT ( slotZoomTo() ));
      icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"zoom-in.png");
      action->setIcon(icon);
      menu.addAction("Ungroup",this,SLOT ( slotUngroup() ));
      menu.addSeparator();
      action = menu.addAction("Rename",this,SLOT ( slotRename() ));
      icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"edit-rename.png");
      action->setIcon(icon);
      menu.addSeparator();
      action = menu.addAction("Remove",this,SLOT ( slotPopupRemove() ));
      icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"datacontrol-delete-item.png");
      action->setIcon(icon);
    } else {
      action = menu.addAction("Zoom to object",this,SLOT ( slotZoomTo() ));
      icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"zoom-in.png");
      action->setIcon(icon);
      action = menu.addAction("Copy",this,SLOT ( slotCopy() ));
      icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"edit-copy.png");
      action->setIcon(icon);
      action = menu.addAction("Rename",this,SLOT ( slotRename() ));
      icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"edit-rename.png");
      action->setIcon(icon);
      action = menu.addAction("Material Properties",this,SLOT ( slotMaterialProperties() ));
      icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"datacontrol-material.png");
      action->setIcon(icon);
      menu.addAction("Group",this,SLOT ( slotGroup() ));
      menu.addSeparator();
      action = menu.addAction("Remove",this,SLOT ( slotPopupRemove() ));
      icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"datacontrol-delete-item.png");
      action->setIcon(icon);
    }



  menu.exec(view_->mapToGlobal( _pos) );

}


//******************************************************************************

/** \brief Generate a ContextMenu for the header of the TreeView
 * 
 * @param _pos Position where the Menu has to be created
 */
void DataControlPlugin::slotHeaderCustomContextMenuRequested ( const QPoint & _pos ) {

  headerPopupType_ = viewHeader_->logicalIndexAt( _pos );

  QMenu menu(0);

  QIcon icon;

  //get all used types
  QVector<DataType> types;

  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS) ;
                                    o_it != PluginFunctions::objectsEnd(); ++o_it)
    if ( !types.contains( o_it->dataType() ) )
      types.push_back( o_it->dataType() );

  QAction* action;

  switch (headerPopupType_) {
//     case 0 :
//       std::cerr << "0";
//       break;
    //Show / Hide
    case 1 :
      icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"drawModes.png");
      action = menu.addAction(icon,"Show all",this,SLOT ( showAll() ));
      action->setData( QVariant() );

      if (types.count() > 1)
      for (int i=0; i < types.count(); i++){
        action = menu.addAction(icon, "Show all of type '" + typeName(types[i]) + "'",this,SLOT ( showAll() ));
        action->setData( QVariant( types[i] ) );
      }

      menu.addSeparator();
      action = menu.addAction("Hide all",this,SLOT ( hideAll() ));
      action->setData( QVariant() );

      if (types.count() > 1)
      for (int i=0; i < types.count(); i++){
        action = menu.addAction("Hide all of type '" + typeName(types[i]) + "'",this,SLOT ( hideAll() ));
        action->setData( QVariant( types[i] ) );
      }

      break;
    // Source
    case 2 :
      action = menu.addAction("Select all",this,SLOT ( setAllSource() ));
      action->setData( QVariant() );

      if (types.count() > 1)
      for (int i=0; i < types.count(); i++){
        action = menu.addAction(icon, "Select all of type '" + typeName(types[i]) + "'",this,SLOT ( setAllSource() ));
        action->setData( QVariant( types[i] ) );
      }

      menu.addSeparator();
      action = menu.addAction("Deselect all",this,SLOT ( clearAllSource() ));
      action->setData( QVariant() );

      if (types.count() > 1)
      for (int i=0; i < types.count(); i++){
        action = menu.addAction(icon, "Deselect all of type '" + typeName(types[i]) + "'",this,SLOT ( clearAllSource() ));
        action->setData( QVariant( types[i] ) );
      }

      break;
    // Target
    case 3 :
      action = menu.addAction("Select all",this,SLOT ( setAllTarget() ));
      action->setData( QVariant() );

      if (types.count() > 1)
      for (int i=0; i < types.count(); i++){
        action = menu.addAction(icon, "Select all of type '" + typeName(types[i]) + "'",this,SLOT ( setAllTarget() ));
        action->setData( QVariant( types[i] ) );
      }

      menu.addSeparator();
      action = menu.addAction("Deselect all",this,SLOT ( clearAllTarget() ));
      action->setData( QVariant() );

      if (types.count() > 1)
      for (int i=0; i < types.count(); i++){
        action = menu.addAction(icon, "Deselect all of type '" + typeName(types[i]) + "'",this,SLOT ( clearAllTarget() ));
        action->setData( QVariant( types[i] ) );
      }

      break;
    default :
//       std::cerr << "def";
      break;
  }

  menu.exec(viewHeader_->mapToGlobal( _pos ) );
}


//******************************************************************************

/** \brief show a rename dialog for object names
 * 
 */
void DataControlPlugin::slotRename(){
  QItemSelectionModel* selection = view_->selectionModel();

  // Get all selected rows
  QModelIndexList indexList = selection->selectedRows ( 0 );
  int selectedRows = indexList.size();
  if (selectedRows == 1){
    BaseObject* item = model_->getItem( indexList[0]);
    bool ok;
    QString newName = QInputDialog::getText(0, tr("Rename"),
                                            tr("Enter a new name:"), QLineEdit::Normal,
                                            item->name(), &ok);
    if (ok && !newName.isEmpty()){
      item->setName(newName);
      emit objectPropertiesChanged( item->id() );
    }
  }
}


//******************************************************************************

/** \brief show the material properties dialog
 * 
 */
void DataControlPlugin::slotMaterialProperties(){

  BaseObject* item = 0;

  //check if it was called from object contextMenu or from the toolBox
  QAction* action = dynamic_cast< QAction* > ( sender() );

  if ( action ){
    bool ok = false;

    int id = action->data().toInt(&ok);

    if ( ok && id > 0 )
      PluginFunctions::getObject(id,item);
  }

  if ( item == 0 ){
    // the slot was called from toolbox
    QItemSelectionModel* selection = view_->selectionModel();

    // Get all selected rows
    QModelIndexList indexList = selection->selectedRows ( 0 );
    int selectedRows = indexList.size();
    if (selectedRows == 1)
      item = model_->getItem( indexList[0] );
  }

  if ( item != 0 ){

    BaseObjectData* itemData = dynamic_cast< BaseObjectData* > (item);

    ACG::QtWidgets::QtMaterialDialog* dialog = new ACG::QtWidgets::QtMaterialDialog( 0, itemData->materialNode() );

    dialog->setWindowFlags(dialog->windowFlags() | Qt::WindowStaysOnTopHint);

    connect(dialog, SIGNAL(signalNodeChanged(ACG::SceneGraph::BaseNode*)),
            this,   SLOT(slotNodeChanged(ACG::SceneGraph::BaseNode*)) );

    dialog->setWindowIcon( QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"datacontrol-material.png"));

    dialog->show();

  }
}


//******************************************************************************

/** \brief Called when the material properties were changed inside the material dialog
 * 
 * @param  unused
 */
void DataControlPlugin::slotNodeChanged( ACG::SceneGraph::BaseNode* /*_node*/ ){
  emit updateView();
}

//******************************************************************************

/** \brief Zoom to an object
 * 
 */
void DataControlPlugin::slotZoomTo(){
  QItemSelectionModel* selection = view_->selectionModel();

  // Get all selected rows
  QModelIndexList indexList = selection->selectedRows ( 0 );
  int selectedRows = indexList.size();
  if (selectedRows == 1){
    BaseObject* item = model_->getItem( indexList[0]);

    if ( item->isGroup() ) {
      //zoom to all objects in this group
      QList< BaseObject* > children = item->getLeafs();

      //compute boundingBox
      bool firstRound = true;
      ACG::Vec3d bbmin;
      ACG::Vec3d bbmax;

      for (int i=0; i < children.size(); i++){
        BaseObjectData* child = dynamic_cast< BaseObjectData* > (children[i]);
        if (child){
          ACG::Vec3d cur_min;
          ACG::Vec3d cur_max;

          child->getBoundingBox(cur_min, cur_max);

          if (firstRound){
            bbmin = cur_min;
            bbmax = cur_max;
            firstRound = false;
          }else{
            bbmin[0] = std::min( bbmin[0], cur_min[0]);
            bbmin[1] = std::min( bbmin[1], cur_min[1]);
            bbmin[2] = std::min( bbmin[2], cur_min[2]);
            bbmax[0] = std::max( bbmax[0], cur_max[0]);
            bbmax[1] = std::max( bbmax[0], cur_max[1]);
            bbmax[2] = std::max( bbmax[0], cur_max[2]);
          }

        }
      }
      //zoom to objects
      ACG::Vec3d bbcenter = (bbmax + bbmin) * 0.5;

      double bbradius = (bbmax - bbmin).norm();

      ACG::Vec3d eye = bbcenter + (PluginFunctions::eyePos() - bbcenter).normalize() * bbradius ;

      PluginFunctions::flyTo(eye, bbcenter );

    }else{
      //zoom to object
      BaseObjectData* obj = dynamic_cast< BaseObjectData* >(item);

      if (obj){

        ACG::Vec3d bbmin;
        ACG::Vec3d bbmax;

        obj->getBoundingBox(bbmin, bbmax);

        if ((bbmin[0] > bbmax[0]) || (bbmin[1] > bbmax[1]) || (bbmin[2] > bbmax[2]))
          std::cerr << "Error while computing bounding box!";

        ACG::Vec3d bbcenter = (bbmax + bbmin) * 0.5;

        double bbradius = (bbmax - bbmin).norm();

        ACG::Vec3d eye = bbcenter + (PluginFunctions::eyePos() - bbcenter).normalize() * bbradius ;

        PluginFunctions::flyTo(eye, bbcenter );
      }
    }
  }
}
