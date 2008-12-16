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




//=============================================================================
//
//  CLASS CoreWidget - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================

// -------------------- mview
#include "CoreWidget.hh"

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include <OpenFlipper/common/GlobalOptions.hh>

//== IMPLEMENTATION ==========================================================

void CoreWidget::slotCustomContextMenu( const QPoint& _point ) {

  updatePopupMenu(_point);

  // If not initialized, dont show it!!
  if ( !contextMenu_->isEmpty () )
    contextMenu_->exec( examiner_widget_->mapToGlobal(_point) );
}

void CoreWidget::updatePopupMenu(const QPoint& _point) {
  contextMenu_->clear();
  contextSelectionMenu_->clear();

  QIcon icon;
  QAction* typeEntry = new QAction("No type",contextMenu_);
  contextMenu_->addAction( typeEntry );

  QAction* entrySeparator = contextMenu_->addSeparator( );


  QAction* contextSelectionAction = contextMenu_->addMenu( contextSelectionMenu_ );

  // -1 if no object id found for the current picking position
  // otherwise the id of the object
  int objectId = -1;

  // Do picking in the gl area to find an object
  unsigned int    node_idx, target_idx;
  ACG::Vec3d      hit_point;
  BaseObjectData* object;
  if (PluginFunctions::scenegraph_pick(ACG::SceneGraph::PICK_ANYTHING, _point,node_idx, target_idx, &hit_point)) {
    if ( PluginFunctions::get_picked_object(node_idx, object) )
      objectId = object->id();
  }

  int topLevelAdded  = 0;

  if ( objectId != -1) {

    emit updateContextMenu(objectId);

    // Add an empty Menu defining the current Type
    typeEntry->setText( typeName(object->dataType()) );
    icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+ typeIcon(object->dataType()) );
    typeEntry->setIcon(icon);


    // Add real context Menus first
    for ( uint i = 0 ; i < contextMenus_.size(); ++i ) {

      // check if the dataType of the object matches the context type
      if ( object->dataType(  contextMenus_[i].contextType ) ) {
        if ( contextMenus_[i].position == CONTEXTTOPLEVELMENU ) {
          contextMenu_->addMenu( contextMenus_[i].menu );
          topLevelAdded++;
        } else if ( contextMenus_[i].position == CONTEXTSELECTIONMENU ) {
          contextSelectionMenu_->addMenu(contextMenus_[i].menu);
        }

        // Get all Actions in the menu and its submenus.
        // Set their data to the picked Object id
        QList< QAction *> allActions = contextMenus_[i].menu->actions();
        while ( !allActions.empty() ) {
          QList< QAction *> tmpList;

          // Set userdata of all actions to the picked Object Id
          for ( int j = 0 ; j < allActions.size(); ++j ) {
            allActions[j]->setData( QVariant( objectId ) );
            if ( allActions[j]->menu() != 0 )
              tmpList << allActions[j]->menu()->actions();
          }

          allActions = tmpList;
        }
      }
    }
  } else {

    emit updateContextMenu(-1);

    // Add real context Menus first
    for ( uint i = 0 ; i < contextMenus_.size(); ++i ) {
      if ( contextMenus_[i].contextType == DATA_NONE ) {
        contextMenu_->addMenu( contextMenus_[i].menu );
        topLevelAdded++;
      }
    }

    if ( topLevelAdded == 0 ) {
      contextMenu_->removeAction(typeEntry);
      contextMenu_->removeAction(entrySeparator);
    }
  }

  if ( contextSelectionMenu_->isEmpty()  )
    contextMenu_->removeAction( contextSelectionAction );

  if ( topLevelAdded > 0 )
    contextMenu_->addSeparator();

  // Add persistent context Menus as second part
  for ( uint i = 0 ; i < persistentContextMenus_.size(); ++i ) {
    contextMenu_->addMenu( persistentContextMenus_[i].menu );

    // Get all Actions in the menu and its submenus.
    // Set their data to the picked Object id
    QList< QAction *> allActions = persistentContextMenus_[i].menu->actions();
    while ( !allActions.empty() ) {
      QList< QAction *> tmpList;

      // Set userdata of all actions to the picked Object Id
      for ( int j = 0 ; j < allActions.size(); ++j ) {
        allActions[j]->setData( QVariant( objectId ) );
        if ( allActions[j]->menu() != 0 )
          tmpList << allActions[j]->menu()->actions();
      }

      allActions = tmpList;
    }

  }

  // Only add Separator if we had plugin context menus
  if ( persistentContextMenus_.size() > 0 )
    contextMenu_->addSeparator();

  if (examiner_widget_->getPickMenu() != NULL) {
    examiner_widget_->getPickMenu()->setTitle("&Picking");
    contextMenu_->addMenu(examiner_widget_->getPickMenu() );
    examiner_widget_->getPickMenu()->setTearOffEnabled(true);
  }

  // Add a functions menu
  QAction* action;
  QMenu* functionMenu = new QMenu("&Functions",contextMenu_);
  action = functionMenu->addAction("Set Background Color");
  action->setToolTip("Set the background color for the viewer");
  connect(action, SIGNAL(triggered()), this, SLOT(changeBackgroundColor()) );
  functionMenu->addSeparator();
  action = functionMenu->addAction("Snapshot");
  action->setToolTip("Make a snapshot");
  connect(action, SIGNAL(triggered()), examiner_widget_, SLOT(actionSnapshot()) );
  action = functionMenu->addAction("Set Snapshot Name");
  action->setToolTip("Set a name for snapshots");
  connect(action, SIGNAL(triggered()), examiner_widget_, SLOT(actionSnapshotName()) );
  functionMenu->addSeparator();
  action = functionMenu->addAction("Copy View");
  action->setToolTip("Copy current view to clipboard");
  connect(action, SIGNAL(triggered()), examiner_widget_, SLOT(actionCopyView()) );
  action = functionMenu->addAction("Paste View");
  action->setToolTip("Paste current view from clipboard");
  connect(action, SIGNAL(triggered()), examiner_widget_, SLOT(actionPasteView()) );
  functionMenu->addSeparator();
  action = functionMenu->addAction("Synchronization");
  action->setToolTip("Synchronize two different viewers");
  action->setCheckable( true );
  action->setChecked( OpenFlipper::Options::synchronization() );
  connect(action, SIGNAL(triggered(bool)), examiner_widget_, SLOT(actionSynchronize(bool)) );
  connect(action, SIGNAL(triggered(bool)), this, SLOT(updateGlobalOptions(bool)) );
  action = functionMenu->addAction("Animation");
  action->setToolTip("Animate rotation of objects");
  action->setCheckable( true );
  action->setChecked( OpenFlipper::Options::animation() );
  connect(action, SIGNAL(triggered(bool)), examiner_widget_, SLOT(actionAnimation(bool)) );
  connect(action, SIGNAL(triggered(bool)), this, SLOT(updateGlobalOptions(bool)) );
  action = functionMenu->addAction("Backface Culling");
  action->setToolTip("Enable backface culling");
  action->setCheckable( true );
  action->setChecked( OpenFlipper::Options::backfaceCulling() );
  connect(action, SIGNAL(triggered(bool)), examiner_widget_, SLOT(actionBackfaceCulling(bool)) );
  connect(action, SIGNAL(triggered(bool)), this, SLOT(updateGlobalOptions(bool)) );
  action = functionMenu->addAction("Two-sided Lighting");
  action->setToolTip("Enable two-sided lighting");
  action->setCheckable( true );
  action->setChecked( OpenFlipper::Options::twoSidedLighting() );
  connect(action, SIGNAL(triggered(bool)), examiner_widget_, SLOT(actionTwoSidedLighting(bool)) );
  connect(action, SIGNAL(triggered(bool)), this, SLOT(updateGlobalOptions(bool)) );

  functionMenu->setTearOffEnabled(true);
  contextMenu_->addMenu(functionMenu );

  if ( ( examiner_widget_->getDrawMenu() != NULL ) && OpenFlipper::Options::drawModesInContextMenu() ) {

    examiner_widget_->getDrawMenu()->setTitle("&DrawModes");
    QAction* drawMenuAction = contextMenu_->addMenu(examiner_widget_->getDrawMenu() );

    QIcon icon;
    icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"drawModes.png");
    drawMenuAction->setIcon(icon);

    examiner_widget_->getDrawMenu()->setTearOffEnabled(true);
  }
}

void CoreWidget::changeBackgroundColor(){
  ACG::Vec4f bc = examiner_widget_->backgroundColor();

  QColor backCol((int)bc[0], (int)bc[1], (int)bc[2]);
  QColor c = QColorDialog::getColor(backCol,this);
  if (c != backCol && c.isValid()){
    examiner_widget_->backgroundColor(ACG::Vec4f(((double) c.red())   / 255.0,
                                                  ((double) c.green()) / 255.0,
                                                  ((double) c.blue())  / 255.0,
                                                            1.0));

    OpenFlipper::Options::defaultBackgroundColor( c.rgb() );
  }
}

void CoreWidget::updateGlobalOptions(bool /*_enable*/){
  OpenFlipper::Options::synchronization( examiner_widget_->synchronization() );
  OpenFlipper::Options::animation( examiner_widget_->animation() );
  OpenFlipper::Options::backfaceCulling( examiner_widget_->backFaceCulling() );
  OpenFlipper::Options::twoSidedLighting( examiner_widget_->twoSidedLighting() );
}

void CoreWidget::slotAddContextMenu(QMenu* _menu) {
  MenuInfo info;
  info.menu = _menu;

  persistentContextMenus_.push_back(info);
}

void CoreWidget::slotAddContextMenu( QMenu* _menu , DataType _dataType ,ContextMenuType _type ) {
  MenuInfo info;
  info.menu        = _menu;
  info.contextType = _dataType;
  info.position    = _type;

  contextMenus_.push_back(info);
}

//=============================================================================
