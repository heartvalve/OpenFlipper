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

    // Add an empty Menu defining the current Type
    if ( object->dataType() == DATA_POLY_MESH ){
      typeEntry->setText("Poly Mesh");
      icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"PolyType.png");
      typeEntry->setIcon(icon);
    } else if ( object->dataType() == DATA_TRIANGLE_MESH ) {
      typeEntry->setText("Triangle Mesh");
      icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"TriangleType.png");
      typeEntry->setIcon(icon);
    } else if ( object->dataType() == DATA_POLY_LINE ) {
      typeEntry->setText("Poly Line");
      icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"PolyLineType.png");
      typeEntry->setIcon(icon);
    } else if ( object->dataType() == DATA_BSPLINE_CURVE ) {
      typeEntry->setText("BSpline Curve");
      icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"BSplineCurveType.png");
      typeEntry->setIcon(icon);
    }


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
    contextMenu_->removeAction(typeEntry);
    contextMenu_->removeAction(entrySeparator);
  }

  if ( contextSelectionMenu_->isEmpty()  )
    contextMenu_->removeAction( contextSelectionAction );

  if ( topLevelAdded > 0 )
    contextMenu_->addSeparator();

  emit updateContextMenu(objectId);

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

  if (examiner_widget_->getFuncMenu() != NULL) {
    examiner_widget_->getFuncMenu()->setTitle("&Functions");
    contextMenu_->addMenu(examiner_widget_->getFuncMenu() );
    examiner_widget_->getFuncMenu()->setTearOffEnabled(true);
  }

  if (examiner_widget_->getDrawMenu() != NULL) {

    examiner_widget_->getDrawMenu()->setTitle("&DrawModes");
    QAction* drawMenuAction = contextMenu_->addMenu(examiner_widget_->getDrawMenu() );

    QIcon icon;
    icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"drawModes.png");
    drawMenuAction->setIcon(icon);

    examiner_widget_->getDrawMenu()->setTearOffEnabled(true);
  }
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
