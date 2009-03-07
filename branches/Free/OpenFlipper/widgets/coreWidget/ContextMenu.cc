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

  QPoint   popupPosition;
  QPoint   scenePos;

  // Calculate popup position. Use the position from the viewer which was clicked on.
  popupPosition =  examiner_widgets_[PluginFunctions::activeExaminer()]->glMapToGlobal(_point);
  QPointF f = examiner_widgets_[PluginFunctions::activeExaminer()]->mapToScene(QPointF(_point.x(), _point.y()));
  scenePos = QPoint (f.x(), f.y());
  
  // Call function to adapt the menu to the currently used contex.
  updatePopupMenu(scenePos);

  // If the menu is not correctly initialized, dont try to show it.
  if ( !contextMenu_->isEmpty () )
    contextMenu_->exec( popupPosition );

}

/** \brief Update context Menu when Coordsys node has been clicked on.
 * 
 * This function is called when the coordinate system in a viewer has been clicked on.
 * This context menu will show all function relevant for the current view like the
 * projection/viewing direction...
 * 
 * @param _menu Pointer to the context Menu
 * @param _part id of the coordsys part which has been clicked on.
 */
void CoreWidget::updatePopupMenuCoordsysNode(QMenu* _menu  , const int _part) {
  std::cerr << "Coordsys part was : " << _part << std::endl;
  QAction* typeEntry = new QAction("Viewer Settings",_menu);
  _menu->addAction( typeEntry );
  
  //====================================================================================================
  
  
}

/** \brief Update context Menu when background has been clicked on.
 * 
 * This function is called when the background in a viewer has been clicked on.
 * This context menu will show functions which are related to the background of the 
 * viewer.
 * 
 * @param _menu Pointer to the context Menu
 * @param _point position in the viewer where the user clicked.
 */
void CoreWidget::updatePopupMenuBackground(QMenu* _menu , const QPoint& _point) {
  
  QAction* action = _menu->addAction("Set Background Color");
  action->setToolTip("Set the background color for the current viewer");
  action->setStatusTip(tr("Set the background color for the current viewer"));
  action->setWhatsThis(tr("Set the background color for the current viewer"));  
  connect(action, SIGNAL(triggered()), this, SLOT(slotSetLocalBackgroundColor()) );
  
  //====================================================================================================
  
}

/** \brief Update context Menu an object has been clicked on.
 * 
 * This function is called when an object has been clicked on.
 * This context menu will show all properties for the given object.
 * 
 * @param _menu Pointer to the context Menu
 * @param _objectId Id of the object that has been clicked on.
 */
void CoreWidget::updatePopupMenuObject(QMenu* _menu , const int _objectId ) {
  std::cerr << "Object Context Menu for id: " << _objectId << std::endl;
}



/** \brief check current context and initialize context menu according to this context. 
 * 
 * This function is called whenever a context menu for the corewidget is requested. 
 * It will decide about the current context, collect all menus for plugins and
 * construct the final context menu.
 */
void CoreWidget::updatePopupMenu(const QPoint& _point) {

  // Clear the complete context menu.
  contextMenu_->clear();
  
  // Clear the selection context menu part.
  contextSelectionMenu_->clear();

  // =============================================================================
  // First do a picking on the current position to check which context we are in.
  // =============================================================================
  int objectId = -1;
  
  enum CONTEXTTYPE {
    COORDSYSCONTEXT ,BACKGROUNDCONTEXT ,OBJECTCONTEXT
  } context = BACKGROUNDCONTEXT;

  // Do picking in the gl area to find an object
  unsigned int    node_idx, target_idx;
  ACG::Vec3d      hit_point;
  BaseObjectData* object;
  
  if (examiner_widgets_[PluginFunctions::activeExaminer()]->pick( ACG::SceneGraph::PICK_ANYTHING,_point,node_idx, target_idx, &hit_point ) ) {

    if ( PluginFunctions::getPickedObject(node_idx, object) ) {
      objectId = object->id();
      context  = OBJECTCONTEXT;
    } else {
      ACG::SceneGraph::BaseNode* node = ACG::SceneGraph::find_node( PluginFunctions::getSceneGraphRootNode() , node_idx );
      if ( node != 0 && ( node->name() == "Core Coordsys Node") ) 
        context = COORDSYSCONTEXT;
    }
  } 
  
  // =============================================================================
  // Depending on the context create the basic context menu.
  // =============================================================================
  
  QIcon icon;
  QAction* typeEntry = 0;      
  switch (context) {
    case BACKGROUNDCONTEXT:
      updatePopupMenuBackground(contextMenu_,_point);
      break;
    case OBJECTCONTEXT:
      typeEntry = new QAction("Object",contextMenu_);
      contextMenu_->addAction( typeEntry );
      break;
    case COORDSYSCONTEXT:
      updatePopupMenuCoordsysNode(contextMenu_,target_idx);
      break;
  }
  
  // Add the global entry to the context menu.
  QAction* entrySeparator = contextMenu_->addSeparator( );

  QAction* contextSelectionAction = contextMenu_->addMenu( contextSelectionMenu_ );

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
    if ( examiner_widgets_[0]->getPickMenu()->actions().size() > 0 ) {
      examiner_widgets_[0]->getPickMenu()->setTitle("&Picking");
      contextMenu_->addMenu(examiner_widgets_[0]->getPickMenu() );
      examiner_widgets_[0]->getPickMenu()->setTearOffEnabled(true);
    }
  }

  // Add a functions menu
  QAction* action;
  if ( functionMenu_ == 0 ){

    functionMenu_ = new QMenu("&Functions",contextMenu_);

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

    action = functionMenu_->addAction("Animation");
    action->setToolTip("Animate rotation of objects");
    action->setCheckable( true );
    action->setChecked( PluginFunctions::viewerProperties().animation() );
    connect(action, SIGNAL(triggered(bool)), this , SLOT( slotChangeAnimation(bool) ) );

    //====================================================================================================

    action = functionMenu_->addAction("Backface Culling");
    action->setToolTip("Enable backface culling");
    action->setCheckable( true );
    action->setChecked( PluginFunctions::viewerProperties().backFaceCulling() );
    connect(action, SIGNAL(triggered(bool)), this , SLOT( slotChangeBackFaceCulling(bool) ) );

    //====================================================================================================

    action = functionMenu_->addAction("Two-sided Lighting");
    action->setToolTip("Enable two-sided lighting");
    action->setCheckable( true );
    action->setChecked( PluginFunctions::viewerProperties().twoSidedLighting() );
    connect(action, SIGNAL(triggered(bool)), this , SLOT( slotChangeTwoSidedLighting(bool) ) );

    functionMenu_->setTearOffEnabled(true);
  }

  contextMenu_->addMenu(functionMenu_ );

  if ( ( examiner_widgets_[PluginFunctions::activeExaminer()]->getDrawMenu() != NULL ) && OpenFlipper::Options::drawModesInContextMenu() ) {

    examiner_widgets_[PluginFunctions::activeExaminer()]->getDrawMenu()->setTitle("&DrawModes");
    QAction* drawMenuAction = contextMenu_->addMenu(examiner_widgets_[PluginFunctions::activeExaminer()]->getDrawMenu() );

    QIcon icon;
    icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"drawModes.png");
    drawMenuAction->setIcon(icon);

    examiner_widgets_[PluginFunctions::activeExaminer()]->getDrawMenu()->setTearOffEnabled(true);
  }
}

void CoreWidget::slotSnapshotName() {
  std::cerr << "Todo : slotSnapShotName only sets name for current viewer" << std::endl;

  QString fname = PluginFunctions::viewerProperties().snapshotName();

  fname.replace('%', '$');
  fname = QFileDialog::getSaveFileName ( 0, "Save snapshot name" );
  if (!fname.isEmpty())
  {
    fname.replace('$', '%');

    PluginFunctions::viewerProperties().snapshotBaseFileName(fname);
    QString msg="next snapshot: ";
    statusBar()->showMessage(msg);
  }

}

void CoreWidget::slotChangeAnimation(bool _animation){
  if ( shiftPressed_ ){
    for ( uint i = 0 ; i < OpenFlipper::Options::examinerWidgets() ; ++i )
      PluginFunctions::viewerProperties(i).animation(_animation);
    }else
      PluginFunctions::viewerProperties().animation(_animation);
}

void CoreWidget::slotChangeBackFaceCulling(bool _backFaceCulling){
  if ( shiftPressed_ ){
    for ( uint i = 0 ; i < OpenFlipper::Options::examinerWidgets() ; ++i )
      PluginFunctions::viewerProperties(i).backFaceCulling(_backFaceCulling);
    }else
      PluginFunctions::viewerProperties().backFaceCulling(_backFaceCulling);
}

void CoreWidget::slotChangeTwoSidedLighting(bool _lighting){
  if ( shiftPressed_ ){
    for ( uint i = 0 ; i < OpenFlipper::Options::examinerWidgets() ; ++i )
      PluginFunctions::viewerProperties(i).twoSidedLighting(_lighting);
    }else
      PluginFunctions::viewerProperties().twoSidedLighting(_lighting);
}

void CoreWidget::slotSnapshot() {
  examiner_widgets_[PluginFunctions::activeExaminer()]->snapshot();
}

void CoreWidget::slotPasteView( ) {
  examiner_widgets_[PluginFunctions::activeExaminer()]->actionPasteView();
}

void CoreWidget::slotCopyView( ) {
  examiner_widgets_[PluginFunctions::activeExaminer()]->actionCopyView();
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
