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

  QObject* senderPointer = sender();
  unsigned int examinerId = 0;
  QPoint   popupPosition;

  if ( senderPointer == 0 ) {
    std::cerr << "Error : slotCustomContextMenu directly called! This should only be called by an examiner" << std::endl;
  } else {
    for ( unsigned int i = 0 ; i < OpenFlipper::Options::examinerWidgets(); ++i ) {
      if ( senderPointer == examiner_widgets_[i] ) {
        popupPosition =  examiner_widgets_[i]->glMapToGlobal(_point);
        examinerId = i;
        break;
      }
    }

  }

  PluginFunctions::setActiveExaminer( examinerId );

  updatePopupMenu(_point,examinerId);

  // If not initialized, dont show it!!
  if ( !contextMenu_->isEmpty () )
    contextMenu_->exec( popupPosition );

}

void CoreWidget::updatePopupMenu(const QPoint& _point, unsigned int _examinerId) {

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
  if (examiner_widgets_[_examinerId]->pick( ACG::SceneGraph::PICK_ANYTHING,_point,node_idx, target_idx, &hit_point ) ) {
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

  if (examiner_widgets_[0]->getPickMenu() != NULL) {
    examiner_widgets_[0]->getPickMenu()->setTitle("&Picking");
    contextMenu_->addMenu(examiner_widgets_[0]->getPickMenu() );
    examiner_widgets_[0]->getPickMenu()->setTearOffEnabled(true);
  }

  // Add a functions menu
  QAction* action;
  if ( functionMenu_ == 0 ) {
    functionMenu_ = new QMenu("&Functions",contextMenu_);

    //====================================================================================================

    action = functionMenu_->addAction("Set Background Color");
    action->setToolTip("Set the background color for the viewer");
    connect(action, SIGNAL(triggered()), this, SLOT(changeBackgroundColor()) );

    //====================================================================================================

    functionMenu_->addSeparator();

    //====================================================================================================

    action = functionMenu_->addAction("Snapshot");
    action->setToolTip("Make a snapshot");
    connect(action, SIGNAL(triggered()), this, SLOT( slotSnapshot() ) );

    //====================================================================================================

    action = functionMenu_->addAction("Set Snapshot Name");
    action->setToolTip("Set a name for snapshots");
    connect(action, SIGNAL(triggered()), this, SLOT(slotSnapshotName()) );

    //====================================================================================================

    functionMenu_->addSeparator();

    //====================================================================================================

    action = functionMenu_->addAction("Copy View");
    action->setToolTip("Copy current view to clipboard");
    connect(action, SIGNAL(triggered()), this, SLOT(slotCopyView()) );

    //====================================================================================================

    action = functionMenu_->addAction("Paste View");
    action->setToolTip("Paste current view from clipboard");
    connect(action, SIGNAL(triggered()), this , SLOT( slotPasteView( ) ) );

    //====================================================================================================

    functionMenu_->addSeparator();

    //====================================================================================================

    action = functionMenu_->addAction("Synchronization");
    action->setToolTip("Synchronize two different viewers");
    action->setCheckable( true );
    action->setChecked( OpenFlipper::Options::synchronization() );
    for ( uint i = 0 ; i < OpenFlipper::Options::examinerWidgets() ; ++i )
      connect(action, SIGNAL(triggered(bool)), examiner_widgets_[i], SLOT(setSynchronization(bool)) );
    connect(action, SIGNAL(triggered(bool)), this, SLOT(updateGlobalOptions(bool)) );

    //====================================================================================================

    action = functionMenu_->addAction("Animation");
    action->setToolTip("Animate rotation of objects");
    action->setCheckable( true );
    action->setChecked( OpenFlipper::Options::animation() );
    for ( uint i = 0 ; i < OpenFlipper::Options::examinerWidgets() ; ++i )
      connect(action, SIGNAL(triggered(bool)), examiner_widgets_[i], SLOT(animation(bool)) );
    connect(action, SIGNAL(triggered(bool)), this, SLOT(updateGlobalOptions(bool)) );

    //====================================================================================================

    action = functionMenu_->addAction("Backface Culling");
    action->setToolTip("Enable backface culling");
    action->setCheckable( true );
    action->setChecked( OpenFlipper::Options::backfaceCulling() );
    for ( uint i = 0 ; i < OpenFlipper::Options::examinerWidgets() ; ++i )
      connect(action, SIGNAL(triggered(bool)), examiner_widgets_[i], SLOT(backFaceCulling(bool)) );
    connect(action, SIGNAL(triggered(bool)), this, SLOT(updateGlobalOptions(bool)) );

    //====================================================================================================

    action = functionMenu_->addAction("Two-sided Lighting");
    action->setToolTip("Enable two-sided lighting");
    action->setCheckable( true );
    action->setChecked( OpenFlipper::Options::twoSidedLighting() );
    for ( uint i = 0 ; i < OpenFlipper::Options::examinerWidgets() ; ++i )
      connect(action, SIGNAL(triggered(bool)), examiner_widgets_[i], SLOT(twoSidedLighting(bool)) );
    connect(action, SIGNAL(triggered(bool)), this, SLOT(updateGlobalOptions(bool)) );

    functionMenu_->setTearOffEnabled(true);
  }

  contextMenu_->addMenu(functionMenu_ );

  if ( ( examiner_widgets_[0]->getDrawMenu() != NULL ) && OpenFlipper::Options::drawModesInContextMenu() ) {

    examiner_widgets_[0]->getDrawMenu()->setTitle("&DrawModes");
    QAction* drawMenuAction = contextMenu_->addMenu(examiner_widgets_[0]->getDrawMenu() );

    QIcon icon;
    icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"drawModes.png");
    drawMenuAction->setIcon(icon);

    examiner_widgets_[0]->getDrawMenu()->setTearOffEnabled(true);
  }
}

void CoreWidget::slotSnapshotName() {
  std::cerr << "Todo : slotSnapShotName only sets name for current viewer" << std::endl;
  examiner_widgets_[PluginFunctions::activeExaminer()]->actionSnapshotName();
}

void CoreWidget::changeBackgroundColor(){

  ACG::Vec4f bc = examiner_widgets_[PluginFunctions::activeExaminer()]->backgroundColor() * 255.0;

//   for ( unsigned int i = 0 ; i < OpenFlipper::Options::examinerWidgets() ; ++i )
//         std::cerr << "Color before : " << i << " : " <<  examiner_widgets_[i]->backgroundColor() << std::endl;

  QColor backCol((int)bc[0], (int)bc[1], (int)bc[2], (int)bc[3]);
  QColor c = QColorDialog::getColor(backCol,this);
  if (c != backCol && c.isValid()){
    std::cerr << "Examiner widgets : " << OpenFlipper::Options::examinerWidgets() << std::endl;
    for ( uint i = 0 ; i < OpenFlipper::Options::examinerWidgets(); ++i ) {
      examiner_widgets_[i]->backgroundColor(ACG::Vec4f(((double) c.redF())   ,
                                                       ((double) c.greenF()) ,
                                                       ((double) c.blueF())  ,
                                                         1.0));
      std::cerr << "i is  " << i << std::endl;
    }

    OpenFlipper::Options::defaultBackgroundColor( c );
  }

  for ( unsigned int i = 0 ; i < OpenFlipper::Options::examinerWidgets() ; ++i )
        std::cerr << "Color after : " << i << " : " <<  examiner_widgets_[i]->backgroundColor() << std::endl;

}

void CoreWidget::slotSnapshot() {
  examiner_widgets_[PluginFunctions::activeExaminer()]->actionSnapshot();
}

void CoreWidget::slotPasteView( ) {
  examiner_widgets_[PluginFunctions::activeExaminer()]->actionPasteView();
}

void CoreWidget::slotCopyView( ) {
  examiner_widgets_[PluginFunctions::activeExaminer()]->actionCopyView();
}

void CoreWidget::updateGlobalOptions(bool /*_enable*/){
  OpenFlipper::Options::synchronization( examiner_widgets_[0]->synchronization() );
  OpenFlipper::Options::animation( examiner_widgets_[0]->animation() );
  OpenFlipper::Options::backfaceCulling( examiner_widgets_[0]->backFaceCulling() );
  OpenFlipper::Options::twoSidedLighting( examiner_widgets_[0]->twoSidedLighting() );
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
