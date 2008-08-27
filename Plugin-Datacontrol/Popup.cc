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

#include <QInputDialog>

void DataControlPlugin::slotPopupRemove (  ) {
  QItemSelectionModel* selection = view_->selectionModel();
  
  // Get all selected rows
  QModelIndexList indexList = selection->selectedRows ( 0 ); 
  int selectedRows = indexList.size();
    
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
   
  emit update_view();
  emit updated_objects(-1);    
}

void DataControlPlugin::slotUngroup (  ) {
  QItemSelectionModel* selection = view_->selectionModel();
  
  // Get all selected rows
  QModelIndexList indexList = selection->selectedRows ( 0 ); 
  int selectedRows = indexList.size();

  //remove complete group if selected item was a group
  BaseObject* group = model_->getItem( indexList[0]);

  for (int i=0; i < group->childCount(); i++){
    group->child(i)->setParent(group->parent());
    group->parent()->appendChild( group->child(i) );
  }

  group->parent()->removeChild(group);

  delete group;

  emit updated_objects(-1);
}

void DataControlPlugin::slotGroup (  ) {
  QItemSelectionModel* selection = view_->selectionModel();
  
  // Get all selected rows
  QModelIndexList indexList = selection->selectedRows ( 0 ); 
  int selectedRows = indexList.size();

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
  
  emit updated_objects(-1);
}

void DataControlPlugin::slotCustomContextMenuRequested ( const QPoint & _pos ) {
  popupIndex_ = view_->indexAt(_pos);
  
  BaseObject* item = model_->getItem(popupIndex_);
  
  QItemSelectionModel* selection = view_->selectionModel();
  
  // Get all selected rows
  QModelIndexList indexList = selection->selectedRows ( 0 ); 
  int selectedRows = indexList.size();
  
  QMenu    menu(0);
  QAction* action;
  QIcon icon;
  if ( selectedRows > 1 ) {
    menu.addAction("Group",this,SLOT ( slotGroup() )); 
    menu.addSeparator();
    menu.addAction("Remove",this,SLOT ( slotPopupRemove() ));  
   }else
    // check if the item is a group item
    if ( item->isGroup() ) {
      menu.addAction("Ungroup",this,SLOT ( slotUngroup() )); 
      menu.addSeparator();
      action = menu.addAction("Rename",this,SLOT ( slotRename() )); 
      icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"edit-rename.png");
      action->setIcon(icon);
      menu.addSeparator();
      action = menu.addAction("Remove",this,SLOT ( slotPopupRemove() ));  
      icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"user-trash.png");
      action->setIcon(icon);
    } else {
      action = menu.addAction("Rename",this,SLOT ( slotRename() )); 
      icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"edit-rename.png");
      action->setIcon(icon);
      menu.addSeparator();
      action = menu.addAction("Remove",this,SLOT ( slotPopupRemove() ));  
      icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"user-trash.png");
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
