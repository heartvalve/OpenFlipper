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

/// Slot for Remove action in ContextMenu
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

/// Slot for Ungroup action in ContextMenu
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

  emit updatedObject(-1);
}

/// Slot for Copy action in ContextMenu
void DataControlPlugin::slotCopy (  ) {
  QItemSelectionModel* selection = view_->selectionModel();

  if (selection == 0) return;

  // Get all selected rows
  QModelIndexList indexList = selection->selectedRows();

  for ( int i = 0 ; i < indexList.size() ; ++i) {

    BaseObject* copyItem = model_->getItem( indexList[i] );

    // remove the whole subtree below this item
    if ( PluginFunctions::copyObject(copyItem->id()) == -1 ) {
      emit log(LOGERR, "Unable to copy object" );
      continue;
    }

    emit updatedObject(copyItem->id());
  }

  emit updateView();
}

/// Slot for Group action in ContextMenu
void DataControlPlugin::slotGroup (  ) {
  QItemSelectionModel* selection = view_->selectionModel();

  // Get all selected rows
  QModelIndexList indexList = selection->selectedRows ( 0 );

  //check if all objects have the same parent
  //abort if the parents differ
  BaseObject* parent = (model_->getItem( indexList[0]))->parent();
  for ( int i = 1 ; i < indexList.size() ; ++i) {
    BaseObject* item = model_->getItem( indexList[i] );
    if (parent != item->parent()){
      emit log("Cannot group Objects with different parents");
      return;
    }
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

  emit updatedObject(-1);
}

/// ContextMenu requested - creates the contextMenu
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

void DataControlPlugin::slotHeaderCustomContextMenuRequested ( const QPoint & _pos ) {

  headerPopupType_ = viewHeader_->logicalIndexAt( _pos );

  QMenu menu(0);

  QIcon icon;



  switch (headerPopupType_) {
//     case 0 :
//       std::cerr << "0";
//       break;
    //Show / Hide
    case 1 :
      icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"drawModes.png");
      menu.addAction(icon,"Show all",this,SLOT ( showAll() ));
      menu.addAction("Hide all",this,SLOT ( hideAll() ));
      break;
    // Source
    case 2 :
      menu.addAction("Select all",this,SLOT ( setAllSource() ));
      menu.addAction("Deselect all",this,SLOT ( clearAllSource() ));
      break;
    // Target
    case 3 :
      menu.addAction("Select all",this,SLOT ( setAllTarget() ));
      menu.addAction("Deselect all",this,SLOT ( clearAllTarget() ));
      break;
    default :
//       std::cerr << "def";
      break;
  }

  menu.exec(viewHeader_->mapToGlobal( _pos ) );
}

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
    if (ok && !newName.isEmpty())
      item->setName(newName);
  }
}

void DataControlPlugin::slotMaterialProperties(){
  QItemSelectionModel* selection = view_->selectionModel();

  // Get all selected rows
  QModelIndexList indexList = selection->selectedRows ( 0 );
  int selectedRows = indexList.size();
  if (selectedRows == 1){
    BaseObject* item = model_->getItem( indexList[0]);

    BaseObjectData* itemData = dynamic_cast< BaseObjectData* > (item);

    ACG::QtWidgets::QtMaterialDialog* dialog = new ACG::QtWidgets::QtMaterialDialog( 0, itemData->materialNode() );

    dialog->setWindowFlags(dialog->windowFlags() | Qt::WindowStaysOnTopHint);

    connect(dialog, SIGNAL(signalNodeChanged(ACG::SceneGraph::BaseNode*)),
            this,   SLOT(slotNodeChanged(ACG::SceneGraph::BaseNode*)) );

    dialog->setWindowIcon( QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"datacontrol-material.png"));

    dialog->show();

  }
}

///Called when the material properties were changed inside the material dialog
void DataControlPlugin::slotNodeChanged( ACG::SceneGraph::BaseNode* /*_node*/ ){
  emit updateView();
}

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
