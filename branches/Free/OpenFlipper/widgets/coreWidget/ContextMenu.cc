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
#include "../../common/GlobalOptions.hh"
#include <OpenFlipper/common/RendererInfo.hh>

//== IMPLEMENTATION ==========================================================
#include <ACG/Scenegraph/CoordsysNode.hh>

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
    contextMenu_->popup( popupPosition );

}

void CoreWidget::slotHideContextMenu() {
  contextMenu_->hide();
}

/** \brief Update context Menu when an arbitrary node has been clicked on.
 *
 * This function is called when a node has been clicked on not belonging to an object.
 * This context menu will show all function relevant for the Node.
 *
 * @param _menu Pointer to the context Menu
 * @param _node node which has been clicked on.
 */
void CoreWidget::updatePopupMenuNode(QMenu* _menu , ACG::SceneGraph::BaseNode* _node) {

  QString nodeName = QString(_node->name().c_str());
  QAction* typeEntry = new QAction( nodeName ,_menu );
  _menu->addAction( typeEntry );

  _menu->addSeparator();

  emit updateContextMenuNode(_node->id());

  addContextMenus( _menu , CONTEXTNODEMENU, _node->id() ) ;
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
void CoreWidget::updatePopupMenuCoordsysNode(QMenu* _menu  , const int /*_part*/) {
    
  QString iconPath = OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator();
  
  QAction* typeEntry = new QAction(tr("Viewer Settings"),_menu);
  _menu->addAction( typeEntry );
  typeEntry->setDisabled(true);
  _menu->addSeparator();
  
  QAction* orthogonalCoordsys = 0;
  if ( getCoordsysProjection() == ACG::SceneGraph::CoordsysNode::PERSPECTIVE_PROJECTION ) {
    orthogonalCoordsys = new QAction( tr("Switch to Orthogonal coordinate system"), _menu );
    orthogonalCoordsys->setIcon( QIcon(iconPath+"orthogonal.png") );
  } else {
    orthogonalCoordsys = new QAction( tr("Switch to Perspective coordinate system"), _menu );
    orthogonalCoordsys->setIcon( QIcon(iconPath+"perspective.png") );
  }
  connect( orthogonalCoordsys,SIGNAL( triggered() ), this, SLOT( slotContextSwitchCoordsysProjection() ) );
  _menu->addAction(orthogonalCoordsys);

  //====================================================================================================
  // DrawModes
  //====================================================================================================
  slotUpdateViewerDrawMenu();
  if (! viewerDrawMenu_->isEmpty())
    _menu->addMenu( viewerDrawMenu_ );

  //====================================================================================================
  // RenderingOptions
  //====================================================================================================
  
  QMenu* renderingOptionsMenu = new QMenu(tr("Rendering Options"),_menu);
  renderingOptionsMenu->setIcon( QIcon(iconPath+"core_renderingOptions.png") );
  _menu->addMenu(renderingOptionsMenu);

  QAction* projectionAction = 0;
  if ( examiner_widgets_[PluginFunctions::activeExaminer() ]->projectionMode() == glViewer::PERSPECTIVE_PROJECTION ) {
    projectionAction = new QAction( tr("Switch to Orthogonal Projection"), renderingOptionsMenu );
    projectionAction->setIcon( QIcon(iconPath+"orthogonal.png") );
    projectionAction->setToolTip(   tr("Switch to perspective orthogonal mode."));
  } else {
    projectionAction = new QAction( tr("Switch to Perspective Projection"), renderingOptionsMenu );
    projectionAction->setIcon( QIcon(iconPath+"perspective.png") );
    projectionAction->setToolTip(   tr("Switch to perspective projection mode."));
  }

  projectionAction->setCheckable( false );
  projectionAction->setToolTip(   tr("Switch between <b>perspective</b> and "
      "<b>parrallel</b> projection mode."));
  projectionAction->setWhatsThis( tr("Switch projection modes<br><br>"
      "Switch between <b>perspective</b> and "
      "<b>parrallel</b> projection mode."));
  connect( projectionAction,SIGNAL( triggered() ), this, SLOT( slotContextSwitchProjection() ) );
  renderingOptionsMenu->addAction( projectionAction );


  QAction* animation = renderingOptionsMenu->addAction(tr("Animation"));

  animation->setToolTip(tr("Animate rotation of objects"));
  animation->setCheckable( true );
  animation->setIcon( QIcon(iconPath+"animation.png") );
  animation->setChecked( PluginFunctions::viewerProperties(PluginFunctions::activeExaminer()).animation() );
  connect(animation, SIGNAL(triggered(bool)), this , SLOT( slotLocalChangeAnimation(bool) ) );


  //====================================================================================================

  QAction* backfaceCulling = renderingOptionsMenu->addAction(tr("Backface Culling"));
  backfaceCulling->setToolTip(tr("Enable backface culling"));
  backfaceCulling->setCheckable( true );
  backfaceCulling->setIcon( QIcon(iconPath+"backFaceCulling.png") );
  backfaceCulling->setChecked( PluginFunctions::viewerProperties().backFaceCulling() );
  connect(backfaceCulling, SIGNAL(triggered(bool)), this , SLOT( slotLocalChangeBackFaceCulling(bool) ) );

  //====================================================================================================

  QAction* twoSidedLighting = renderingOptionsMenu->addAction(tr("Two-sided Lighting"));
  twoSidedLighting->setToolTip(tr("Enable two-sided lighting"));
  twoSidedLighting->setCheckable( true );
  twoSidedLighting->setIcon( QIcon(iconPath+"twosidedLighting.png") );
  twoSidedLighting->setChecked( PluginFunctions::viewerProperties().twoSidedLighting() );
  connect(twoSidedLighting, SIGNAL(triggered(bool)), this , SLOT( slotLocalChangeTwoSidedLighting(bool) ) );
  
  //====================================================================================================

  QAction* multisampling = renderingOptionsMenu->addAction(tr("Multisampling"));
  multisampling->setToolTip(tr("Enable Multisampling"));
  multisampling->setCheckable( true );
  multisampling->setIcon( QIcon(iconPath+"multiSampling.png") );
  multisampling->setChecked( PluginFunctions::viewerProperties().multisampling() );
  connect(multisampling, SIGNAL(triggered(bool)), this , SLOT( slotLocalChangeMultisampling(bool) ) );
  
  //====================================================================================================

  QAction* mipmapping = renderingOptionsMenu->addAction(tr("Mipmapping"));
  mipmapping->setToolTip(tr("Enable Mipmapping"));
  mipmapping->setCheckable( true );
  mipmapping->setIcon( QIcon(iconPath+"mipmapping.png") );
  mipmapping->setChecked( PluginFunctions::viewerProperties().mipmapping() );
  connect(mipmapping, SIGNAL(triggered(bool)), this , SLOT( slotLocalChangeMipmapping(bool) ) );

  //============================================================================================================
  // Renderer Menu
  //============================================================================================================

  if ( renderManager().available() > 1 ) {
    QMenu* rendererMenu = new QMenu(tr("Renderers"),_menu);
    rendererMenu->setIcon(QIcon(iconPath+"renderers.png"));

    _menu->addMenu(rendererMenu);

    // Recreate actionGroup
    QActionGroup* groupRenderer = new QActionGroup( this );
    groupRenderer->setExclusive( true );


    // Get the options action for the currently active renderer
    if( renderManager()[ renderManager().activeId(PluginFunctions::activeExaminer() )]->optionsAction != 0 ) {
      rendererMenu->addAction(renderManager()[ renderManager().activeId(PluginFunctions::activeExaminer() ) ]->optionsAction );
      rendererMenu->addSeparator();
    }

    for ( unsigned int i = 0 ; i < renderManager().available() ; ++i) {

      // Add a new Action with the renderer name
      QAction * action = new QAction( renderManager()[i]->name, groupRenderer );
      action->setCheckable( true );

      // Check if this processor is currently active
      if ( renderManager().activeId(PluginFunctions::activeExaminer() ) == i )
        action->setChecked(true);

      // Remember the id for the processor
      action->setData(QVariant(i));
    }

    // Add all new actions from the group to the menu
    rendererMenu->addActions( groupRenderer->actions() );

    // Connect signal of group to our managing slot
    connect( groupRenderer , SIGNAL( triggered( QAction * ) ),
        this               , SLOT( slotRenderMenu( QAction * ) ) );

  }

  //============================================================================================================
  // Post processor Menu
  //============================================================================================================

  if ( postProcessorManager().available() > 1 ) {
    QMenu* postProcessorMenu = new QMenu(tr("Post processors"),_menu);
    postProcessorMenu->setIcon(QIcon(iconPath+"postprocessors.png"));

    _menu->addMenu(postProcessorMenu);

    // Recreate actionGroup
    QActionGroup* groupPostProcessor = new QActionGroup( this );
    groupPostProcessor->setExclusive( true );

    // Get the options action for the currently active postprocessor
    if( postProcessorManager()[ postProcessorManager().activeId(PluginFunctions::activeExaminer() )]->optionsAction != 0 ) {
      postProcessorMenu->addAction(postProcessorManager()[ postProcessorManager().activeId(PluginFunctions::activeExaminer() ) ]->optionsAction );
      postProcessorMenu->addSeparator();
    }



    // Now add the processor chooser
    for ( unsigned int i = 0 ; i < postProcessorManager().available() ; ++i) {

      // Add a new Action with the postprocessors name
      QAction * action = new QAction( postProcessorManager()[i]->name, groupPostProcessor );
      action->setCheckable( true );

      // Check if this processor is currently active
      if ( postProcessorManager().activeId(PluginFunctions::activeExaminer() ) == i )
        action->setChecked(true);

      // Remember the id for the processor
      action->setData(QVariant(i));
    }

    // Add all new actions from the group to the menu
    postProcessorMenu->addActions( groupPostProcessor->actions() );

    // Connect signal of group to our managing slot
    connect( groupPostProcessor , SIGNAL( triggered( QAction * ) ),
             this               , SLOT( slotPostProcessorMenu( QAction * ) ) );

  }


  //============================================================================================================
  // Viewing Direction Menu
  //============================================================================================================

  QMenu* viewingDirectionMenu = new QMenu( tr("Viewing Direction"), _menu);
  viewingDirectionMenu->setIcon(QIcon(iconPath+"core_viewingDirection.png"));
  _menu->addMenu(viewingDirectionMenu);

  QActionGroup* dirGroup = new QActionGroup(this);

  QAction* viewAction;
  // freeView
  viewAction = new QAction( tr("Free View"), viewingDirectionMenu );
  viewAction->setIcon( QIcon(iconPath+"orthogonal.png") );
  viewAction->setCheckable( true );
  viewAction->setData( PluginFunctions::VIEW_FREE );
  viewAction->setChecked( PluginFunctions::viewerProperties().currentViewingDirection() == PluginFunctions::VIEW_FREE );
  viewingDirectionMenu->addAction( viewAction );
  dirGroup->addAction(viewAction);
  viewingDirectionMenu->addSeparator();
  // TOP
  viewAction = new QAction( tr("Top View"), viewingDirectionMenu );
  viewAction->setIcon( QIcon(iconPath+"viewcontrol_top.png") );
  viewAction->setCheckable( true );
  viewAction->setData( PluginFunctions::VIEW_TOP );
  viewAction->setChecked( PluginFunctions::viewerProperties().currentViewingDirection() == PluginFunctions::VIEW_TOP );
  viewingDirectionMenu->addAction( viewAction );
  dirGroup->addAction(viewAction);
  // BOTTOM
  viewAction = new QAction( tr("Bottom View"), viewingDirectionMenu );
  viewAction->setIcon( QIcon(iconPath+"viewcontrol_bottom.png") );
  viewAction->setCheckable( true );
  viewAction->setData( PluginFunctions::VIEW_BOTTOM );
  viewAction->setChecked( PluginFunctions::viewerProperties().currentViewingDirection() == PluginFunctions::VIEW_BOTTOM );
  viewingDirectionMenu->addAction( viewAction );
  dirGroup->addAction(viewAction);
  // LEFT
  viewAction = new QAction( tr("Left View"), viewingDirectionMenu );
  viewAction->setIcon( QIcon(iconPath+"viewcontrol_left.png") );
  viewAction->setCheckable( true );
  viewAction->setData( PluginFunctions::VIEW_LEFT );
  viewAction->setChecked( PluginFunctions::viewerProperties().currentViewingDirection() == PluginFunctions::VIEW_LEFT );
  viewingDirectionMenu->addAction( viewAction );
  dirGroup->addAction(viewAction);
  // RIGHT
  viewAction = new QAction( tr("Right View"), viewingDirectionMenu );
  viewAction->setIcon( QIcon(iconPath+"viewcontrol_right.png") );
  viewAction->setCheckable( true );
  viewAction->setData( PluginFunctions::VIEW_RIGHT );
  viewAction->setChecked( PluginFunctions::viewerProperties().currentViewingDirection() == PluginFunctions::VIEW_RIGHT );
  viewingDirectionMenu->addAction( viewAction );
  dirGroup->addAction(viewAction);
  // FRONT
  viewAction = new QAction( tr("Front View"), viewingDirectionMenu );
  viewAction->setIcon( QIcon(iconPath+"viewcontrol_front.png") );
  viewAction->setCheckable( true );
  viewAction->setData( PluginFunctions::VIEW_FRONT );
  viewAction->setChecked( PluginFunctions::viewerProperties().currentViewingDirection() == PluginFunctions::VIEW_FRONT );
  viewingDirectionMenu->addAction( viewAction );
  dirGroup->addAction(viewAction);
  // BACK
  viewAction = new QAction( tr("Back View"), viewingDirectionMenu );
  viewAction->setIcon( QIcon(iconPath+"viewcontrol_back.png") );
  viewAction->setCheckable( true );
  viewAction->setData( PluginFunctions::VIEW_BACK );
  viewAction->setChecked( PluginFunctions::viewerProperties().currentViewingDirection() == PluginFunctions::VIEW_BACK );
  viewingDirectionMenu->addAction( viewAction );
  dirGroup->addAction(viewAction);
  
  viewingDirectionMenu->addSeparator();

  connect( dirGroup, SIGNAL(triggered(QAction*)), this, SLOT(slotSetViewingDirection(QAction*) ) );
  
  //===========================================================================
  // Check box to determine whether rotation should be locked or not
  
  QAction* lockAction = viewingDirectionMenu->addAction("Lock rotation");
  lockAction->setCheckable( true );
  lockAction->setIcon( QIcon(iconPath+"lock_rotation.png") );
  lockAction->setToolTip(tr("Lock rotation in current examiner"));
  lockAction->setChecked( PluginFunctions::viewerProperties().rotationLocked() );
  viewingDirectionMenu->addAction( lockAction );
  
  connect( lockAction, SIGNAL(triggered(bool)), this, SLOT(slotLockRotation(bool) ) );
  
  //====================================================================================================
  // Other Toplevel Action
  //====================================================================================================

  _menu->addSeparator();

  //====================================================================================================

  QAction* homeAction = new QAction(tr("Restore home view"),_menu);
  homeAction->setIcon( QIcon(iconPath+"go-home.png") );
  homeAction->setCheckable( false );
  homeAction->setToolTip(tr("Restore <b>home</b> view."));
  homeAction->setWhatsThis( tr("Restore home view<br><br>"
                            "Resets the view to the home view"));
  _menu->addAction( homeAction );
  connect( homeAction,SIGNAL( triggered() ), this, SLOT( slotContextHomeView() ) );

  QAction* setHomeAction = new QAction( tr("Set Home View") , _menu );
  setHomeAction->setIcon( QIcon(iconPath+"set-home.png") );
  setHomeAction->setCheckable( false );
  setHomeAction->setToolTip(tr("Set <b>home</b> view"));
  setHomeAction->setWhatsThis( tr("Store home view<br><br>"
                               "Stores the current view as the home view"));
  _menu->addAction( setHomeAction);
  connect( setHomeAction,SIGNAL( triggered() ), this, SLOT( slotContextSetHomeView() ) );

  QAction* viewAllAction = new QAction( tr("View all"), _menu );
  viewAllAction->setIcon( QIcon(iconPath+"viewall.png") );
  viewAllAction->setCheckable( false );
  viewAllAction->setToolTip(tr("View all."));
  viewAllAction->setWhatsThis( tr("View all<br><br>"
                                  "Move the objects in the scene so that"
                                  " the whole scene is visible."));
  connect( viewAllAction,SIGNAL( triggered() ), this, SLOT( slotContextViewAll() ) );
  _menu->addAction( viewAllAction);


  _menu->addSeparator();

  //====================================================================================================

  QAction* copyView = _menu->addAction(tr("Copy View"));
  copyView->setToolTip(tr("Copy current view, window size and toolbar size to clipboard"));
  copyView->setIcon( QIcon(iconPath+"edit-copy.png") );
  connect(copyView, SIGNAL(triggered()), this, SLOT(slotCopyView()) );

  //====================================================================================================

  QAction* pasteView = _menu->addAction(tr("Paste View"));
  pasteView->setToolTip(tr("Paste current view from clipboard"));
  pasteView->setIcon( QIcon(iconPath+"edit-paste.png") );
  connect(pasteView, SIGNAL(triggered()), this , SLOT( slotPasteView( ) ) );

  //====================================================================================================

  QAction* pasteViewAndWindow = _menu->addAction(tr("Paste View and Window Size"));
  pasteViewAndWindow->setToolTip(tr("Paste current view, window size and the toolbox size from clipboard"));
  pasteViewAndWindow->setIcon( QIcon(iconPath+"edit-paste.png") );
  connect(pasteViewAndWindow, SIGNAL(triggered()), this , SLOT( slotPasteViewAndWindow( ) ) );

  //====================================================================================================

  QAction* snapshot_examiner = _menu->addAction(tr("Examiner Snapshot"));
  snapshot_examiner->setToolTip(tr("Take a snapshot of the current examiner"));
  snapshot_examiner->setIcon( QIcon(iconPath+"snapshot.png") );
  connect(snapshot_examiner, SIGNAL(triggered()), this, SLOT( slotExaminerSnapshot() ) );
  
  //====================================================================================================
  
  QAction* snapshot_viewer = _menu->addAction(tr("Viewer Snapshot"));
  snapshot_viewer->setToolTip(tr("Take a snapshot of the whole viewer"));
  snapshot_viewer->setIcon( QIcon(iconPath+"snapshot.png") );
  connect(snapshot_viewer, SIGNAL(triggered()), this, SLOT( viewerSnapshotDialog() ) );

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
void CoreWidget::updatePopupMenuBackground(QMenu* _menu , const QPoint& /*_point*/) {

  //====================================================================================================
  // DrawModes
  //====================================================================================================
  slotUpdateViewerDrawMenu();
  _menu->addMenu( viewerDrawMenu_ );

  _menu->addSeparator();
  
  QAction* action = _menu->addAction(tr("Set Background Color"));
  action->setToolTip(tr("Set the background color for the current viewer"));
  action->setStatusTip(tr("Set the background color for the current viewer"));
  action->setWhatsThis(tr("Set the background color for the current viewer"));
  action->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"BackgroundColor.png") );
  connect(action, SIGNAL(triggered()), this, SLOT(slotSetContextBackgroundColor()) );

  /*
   * Show coordsys context menu of coordsys if
   * invisible...
   */

  ACG::SceneGraph::BaseNode* root = PluginFunctions::getSceneGraphRootNode();
  ACG::SceneGraph::BaseNode* coordSys = root->find("Core Coordsys Node");

  if(!coordSys->visible()) {

	  if(!coordSysMenu_) {
		  coordSysMenu_ = new QMenu(tr("Viewer Settings"), _menu);
		  updatePopupMenuCoordsysNode(coordSysMenu_, 0);
	  }
	  _menu->addSeparator();
	  _menu->addMenu(coordSysMenu_);
  }

  // Tell Plugins to update their context Menu
  emit updateContextMenuBackground();

  addContextMenus( _menu , CONTEXTBACKGROUNDMENU ) ;

}

/** \brief Update popup Menu when an object has been clicked on.
 *
 * This function is called when an object has been clicked on.
 * This context menu will show all properties for the given object.
 *
 * @param _menu Pointer to the context Menu
 * @param _object Object that has been clicked on.
 */
void CoreWidget::updatePopupMenuObject(QMenu* _menu , BaseObjectData* _object ) {

  QAction* typeEntry = new QAction( typeName(_object->dataType()) ,_menu);
  typeEntry->setIcon(typeIcon(_object->dataType()));
  _menu->addAction( typeEntry );

  _menu->addSeparator( );

  // Tell Plugins to update their context Menu
  emit updateContextMenu(_object->id() );

  if ( addContextMenus( _menu , CONTEXTOBJECTMENU , _object->id() ) )
    _menu->addSeparator();

  // Add picking Menu
  if (pickMenu_ != 0 && pickMenu_->actions().size() > 0) {
    pickMenu_->setTitle(tr("&Picking"));
    contextMenu_->addMenu( pickMenu_ );
    pickMenu_->setTearOffEnabled(true);
  }
}

bool CoreWidget::addContextMenus( QMenu* _menu , ContextMenuType _type , int _id ) {

  bool added = false;

  QMap< QString , QAction* > menuMap; //QMap sorts by key
  QMap< QString , QAction* > actionMap;

  // Add context menus from plugins
  for ( uint i = 0 ; i < contextMenus_.size(); ++i ) {

    if ( contextMenus_[i].type != _type )
      continue;

    switch (contextMenus_[i].type) {
      case CONTEXTBACKGROUNDMENU:
        break;
      case CONTEXTOBJECTMENU:
        BaseObjectData* object;
        if ( !PluginFunctions::getObject(_id, object) ) {
          emit log(LOGERR,tr("Cant get object for objectContextMenu"));
          continue;
        }

        // Datatype does not match
        if ( ! object->dataType(  contextMenus_[i].contextType ) )
          continue;

        break;
      case CONTEXTNODEMENU:
        break;

    }

    QMenu* menu = contextMenus_[i].action->menu();

    if (menu == 0) //is it a menu
      actionMap[ contextMenus_[i].action->text() ] = contextMenus_[i].action;
    else
      menuMap[ contextMenus_[i].action->text() ] = contextMenus_[i].action;

    added = true;

    // Get all Actions in the menu and its submenus.
    // Set their data to the picked Object id


    QList< QAction *> allActions;
    if ( menu == 0) {
      allActions.push_back(contextMenus_[i].action);
    } else {
      allActions = menu->actions();
    }

    while ( !allActions.empty() ) {
      QList< QAction *> tmpList;

      // Set userdata of all actions to the picked Object Id
      for ( int j = 0 ; j < allActions.size(); ++j ) {
        allActions[j]->setData( QVariant( _id ) );
        if ( allActions[j]->menu() != 0 )
          tmpList << allActions[j]->menu()->actions();
      }

      allActions = tmpList;
    }

  }

  //find the currently selected view mode
  int id = -1;
  for (int i=0; i<viewModes_.size(); i++) {
    if (viewModes_[i]->name == OpenFlipper::Options::currentViewMode()) {
      id = i;
      break;
    }
  }

  // Default to mode all (0) if not found
  if ( id == -1 ) {
    emit log(LOGERR, tr("Unable to find view mode %1.").arg(OpenFlipper::Options::currentViewMode()) );
    id = 0;
  }
  
  /// \todo Sort the menu entries by the order given in visibleContextMenus
  
  //first add all menus
  QMapIterator<QString, QAction*> it(menuMap);
  
  QStringList visible = viewModes_[id]->visibleContextMenus;
  if (visible.contains("ALL_THAT_EXIST")) {
    //this plugin adds all context menus, no special configuration so far.
    visible = viewModes_[0]->visibleContextMenus;
  }

  // Remove Plugin Name from string
  visible.replaceInStrings(QRegExp(".*>"), "");

  // Remove accelerator specifications
  visible.replaceInStrings("&", "");

  while (it.hasNext()) {
    it.next();

	for ( int i = 0 ; i < visible.size(); ++i ) {
	  if ( it.key().contains(visible[i]) ) {
	    _menu->addAction( it.value() );
	  }
    }
  }

  _menu->addSeparator();

  //then all actions
  QMapIterator<QString, QAction*> it2(actionMap);

  while (it2.hasNext()) {
     it2.next();

	 for ( int i = 0 ; i < visible.size(); ++i ) {
		 if ( it2.key().contains(visible[i]) ) {
	       _menu->addAction( it2.value() );
		 }
	 }
  }

  return added;
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

  enum CONTEXTTYPE {
    COORDSYSCONTEXT ,BACKGROUNDCONTEXT ,OBJECTCONTEXT, NODECONTEXT
  } context = BACKGROUNDCONTEXT;

  // Do picking in the gl area to find an object
  unsigned int               node_idx, target_idx;
  ACG::Vec3d                 hit_point;
  BaseObjectData*            object = 0;
  ACG::SceneGraph::BaseNode* node   = 0;

  if (examiner_widgets_[PluginFunctions::activeExaminer()]->pick( ACG::SceneGraph::PICK_ANYTHING,_point,node_idx, target_idx, &hit_point ) ) {

    if ( PluginFunctions::getPickedObject(node_idx, object) ) {
      context  = OBJECTCONTEXT;
    } else {
      node = ACG::SceneGraph::find_node( PluginFunctions::getSceneGraphRootNode() , node_idx );
      if ( node != 0 && ( node->name() == "Core Coordsys Node") )
        context = COORDSYSCONTEXT;
      else
        context = NODECONTEXT;
    }
  }

  // =============================================================================
  // Depending on the context create the basic context menu.
  // =============================================================================

  QIcon icon;

  switch (context) {
    case BACKGROUNDCONTEXT:
      updatePopupMenuBackground(contextMenu_,_point);
      return;
      break;
    case OBJECTCONTEXT:
      updatePopupMenuObject(contextMenu_ , object );
      return;
      break;
    case COORDSYSCONTEXT:
      updatePopupMenuCoordsysNode(contextMenu_,target_idx);
      return;
      break;
    case NODECONTEXT:
      updatePopupMenuNode(contextMenu_,node);
      return;
      break;
  }

}


void CoreWidget::slotSnapshotName() {
  std::cerr << "Todo : slotSnapShotName only sets name for current viewer" << std::endl;

  QString fname = PluginFunctions::viewerProperties().snapshotName();

  fname.replace('%', '$');
  fname = QFileDialog::getSaveFileName ( 0,
                                         tr("Save snapshot name"),
                                         OpenFlipperSettings().value("Core/CurrentDir").toString());

  if (!fname.isEmpty())
  {
    fname.replace('$', '%');


    // Get the chosen directory and remember it.
    QFileInfo fileInfo(fname);
    OpenFlipperSettings().setValue("Core/CurrentDir", fileInfo.absolutePath() );

    PluginFunctions::viewerProperties().snapshotBaseFileName(fname);
    QString msg=tr("next snapshot: ");
    statusBar()->showMessage(msg);
  }

}

void CoreWidget::slotAddContextItem(QAction* _entry, ContextMenuType _type) {
  MenuInfo info;
  info.action = _entry;
  info.type   = _type;

  contextMenus_.push_back(info); 
  slotAddContextItemToViewMode(_entry);
}

void CoreWidget::slotAddContextItem( QAction* _entry , DataType _dataType ,ContextMenuType _type ) {
  MenuInfo info;
  info.action      = _entry;
  info.contextType = _dataType;
  info.type        = _type;

  contextMenus_.push_back(info);
  slotAddContextItemToViewMode(_entry);
}

void CoreWidget::slotAddContextItemToViewMode( QAction* _entry ) {
  int id = -1;
  // Find the plugin which added this Context Menu
  for ( uint i = 0 ; i < plugins_.size(); ++i ) {
    if ( plugins_[i].plugin == sender() ) {
      id = i;
      break;
    }
  }

  // Find the scripting plugin because we assign this context menu to it as we did not find the original sender
  if ( id == -1 ) {
    for ( uint i = 0 ; i < plugins_.size(); ++i ) {
      if ( plugins_[i].name == "Scripting" ) {
        id = i;
        break;
      }
    }


    if ( id == -1 ) {
      std::cerr << "Unknown sender plugin when adding Context Menu!" << std::endl;
      return;
    }
  }

  plugins_[id].contextMenus.push_back( std::pair< QString,QAction* >( plugins_[id].name + "->" + _entry->text(), _entry) );

  // add widget name to viewMode 'all'
  if ( !viewModes_[0]->visibleContextMenus.contains(plugins_[id].name + "->" + _entry->text()) ){
    viewModes_[0]->visibleContextMenus << plugins_[id].name + "->" + _entry->text();
    viewModes_[0]->visibleContextMenus.sort();
  }

  setViewMode( OpenFlipper::Options::currentViewMode() );
}

void CoreWidget::slotUpdateViewerDrawMenu() {
  if ( drawGroupViewer_ ) {

    disconnect( drawGroupViewer_ , SIGNAL( triggered( QAction * ) ),
                this             , SLOT( slotViewerDrawMenu( QAction * ) ) );
    delete( drawGroupViewer_ );
    drawGroupViewer_ = 0;

  }

  // Recreate drawGroup
  drawGroupViewer_ = new QActionGroup( this );
  drawGroupViewer_->setExclusive( false );

  connect( drawGroupViewer_ , SIGNAL( triggered( QAction * ) ),
           this       , SLOT( slotViewerDrawMenu( QAction * ) ) );

  if ( !viewerDrawMenu_ ) {

    QIcon icon;
    icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"drawModes.png");
    viewerDrawMenu_  = new QMenu(tr("Set Draw Mode"));
    viewerDrawMenu_->setTearOffEnabled(true);
    viewerDrawMenu_->setIcon(icon);

    connect(viewerDrawMenu_,SIGNAL(aboutToShow () ) , this, SLOT(slotUpdateGlobalDrawMenu() ) );
  }

  // Collect available draw modes
  // Single pass action, draw modes independent from multipass rendering
  ACG::SceneGraph::CollectDrawModesAction actionAvailable;
  ACG::SceneGraph::traverse( PluginFunctions::getRootNode() , actionAvailable);
  availableGlobalDrawModes_ = actionAvailable.drawModes();

  // Get currently active drawModes (first viewer only )
  // TODO: create combination from all viewers!
  ACG::SceneGraph::DrawModes::DrawMode activeDrawModes = PluginFunctions::drawMode();

  // Convert to ids
  std::vector< ACG::SceneGraph::DrawModes::DrawMode > availDrawModeIds;
  availDrawModeIds = availableGlobalDrawModes_.getAtomicDrawModes() ;

  viewerDrawMenu_->clear();

  for ( unsigned int i = 0; i < availDrawModeIds.size(); ++i )
  {
    ACG::SceneGraph::DrawModes::DrawMode id    = availDrawModeIds[i];
    std::string  descr =  id.description();

    QCheckBox *checkBox = new QCheckBox(QString(descr.c_str()), viewerDrawMenu_);
    checkBox->setChecked(activeDrawModes.containsAtomicDrawMode(id));
    QWidgetAction *checkableAction = new QWidgetAction(drawGroupViewer_);
    checkableAction->setText(descr.c_str());
    checkableAction->setDefaultWidget(checkBox);
    connect(checkBox, SIGNAL(toggled(bool) ), checkableAction, SLOT(trigger() ) );
  }

  viewerDrawMenu_->addActions( drawGroupViewer_->actions() );

}

void CoreWidget::slotViewerDrawMenu(QAction * _action) {

  //======================================================================================
  // Get the mode toggled
  //======================================================================================
  ACG::SceneGraph::DrawModes::DrawMode mode = 0;
  std::vector< ACG::SceneGraph::DrawModes::DrawMode > availDrawModeIds;
  availDrawModeIds = availableGlobalDrawModes_.getAtomicDrawModes();
  for ( unsigned int i = 0; i < availDrawModeIds.size(); ++i )
  {
    QString descr = QString(  availDrawModeIds[i].description().c_str() );

    if ( descr == _action->text() ) {
      mode = availDrawModeIds[i];
      break;
    }
  }

  if ( qApp->keyboardModifiers() & Qt::ShiftModifier )
    PluginFunctions::viewerProperties().drawMode(  PluginFunctions::viewerProperties().drawMode() ^ mode );
  else
  {
    contextMenu_->hide();
    PluginFunctions::viewerProperties().drawMode(mode );
  }

}

void CoreWidget::slotPostProcessorMenu( QAction * _action)  {
  unsigned int mode = _action->data().toUInt();
  postProcessorManager().setActive(mode,PluginFunctions::activeExaminer());
}

void CoreWidget::slotRenderMenu( QAction * _action)  {
  unsigned int mode = _action->data().toUInt();
  renderManager().setActive(mode,PluginFunctions::activeExaminer());
}

//=============================================================================
