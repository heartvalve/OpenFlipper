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
#include <OpenFlipper/common/GlobalOptions.hh>
#include <OpenFlipper/BasePlugin/PluginFunctions.hh>

//== IMPLEMENTATION ==========================================================



void CoreWidget::slotAddMenubarAction( QAction* _action , MenuActionType _type ) {

  switch (_type) {
    case TOPLEVELMENU :
      // Add it to the menubar as a top level Menu
      menuBar()->insertAction(helpMenu_->menuAction() ,_action);
      break;
    case FILEMENU :
      fileMenu_->insertSeparator(fileMenuEnd_);
      fileMenu_->insertAction( fileMenuEnd_ , _action );
      break;
    case VIEWMENU :
      viewMenu_->addAction( _action );
      break;
    case TOOLSMENU:
      toolsMenu_->addAction( _action );
  }

}


//=============================================================================
/*
bool CoreWidget::eventFilter(QObject *obj, QEvent *event)
 {
     if (obj == menuBar() ) {
       emit log(LOGERR,"Alt filter menubar " + QString::number(int(event->type())));
         if (event->type() == QEvent::ShortcutOverride) {
             QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
             if ( (keyEvent->key() == Qt::Key_Alt ||
                   keyEvent->key() == Qt::Key_Meta ) &&
                   keyEvent->modifiers() == Qt::AltModifier )
                   emit log(LOGERR,"Alt key press");
             return obj->eventFilter(obj, event);
         } else {
             return obj->eventFilter(obj, event);
         }
     } else {
         // pass the event on to the parent class
         return QMainWindow::eventFilter(obj, event);
     }
 }*/

void CoreWidget::setupMenuBar()
{

 // menuBar()->installEventFilter(this);

  // ======================================================================
  // File Menu
  // ======================================================================
  fileMenu_ = new QMenu(tr("&File"));
  menuBar()->addMenu(fileMenu_ );

  //Clear all
  QAction* AC_clear_all = new QAction(tr("&Clear All"), this);;
  AC_clear_all->setStatusTip(tr("Clear all Objects"));
  AC_clear_all->setWhatsThis("Close all open Objects");
  AC_clear_all->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"edit-clear.png"));
  connect(AC_clear_all, SIGNAL(triggered()), this, SIGNAL(clearAll()));
  fileMenu_->addAction(AC_clear_all);

  fileMenu_->addSeparator();

  //Load object
  QAction* AC_Load = new QAction(tr("&Load Object"), this);
//   AC_Load->setShortcut (Qt::CTRL + Qt::Key_O);
  AC_Load->setStatusTip(tr("Load an object"));
  AC_Load->setWhatsThis("Load a new object");
  AC_Load->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"document-open.png"));

  connect(AC_Load, SIGNAL(triggered()), this, SIGNAL(loadMenu()));
  fileMenu_->addAction(AC_Load);

  //Add empty object
  QAction* AC_AddEmpty = new QAction(tr("&Add Empty Object"), this);
  AC_AddEmpty->setStatusTip(tr("Add an empty object"));
  AC_AddEmpty->setWhatsThis("Creates a new empty object of a given type");
  AC_AddEmpty->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"add-empty-object.png"));
  connect(AC_AddEmpty, SIGNAL(triggered()), this, SIGNAL(addEmptyObjectMenu()));
  fileMenu_->addAction(AC_AddEmpty);

  fileMenu_->addSeparator();

  //Save object
  QAction* AC_Save = new QAction(tr("&Save Object"), this);
//   AC_Save->setShortcut (Qt::CTRL + Qt::Key_S);
  AC_Save->setStatusTip(tr("Save currently selected objects"));
  AC_Save->setWhatsThis("Saves all currently selected objects");
  AC_Save->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"document-save.png"));
  connect(AC_Save, SIGNAL(triggered()), this, SIGNAL(saveMenu()));
  fileMenu_->addAction(AC_Save);

  //Save object to
  QAction* AC_Save_to = new QAction(tr("&Save Object To"), this);
  AC_Save_to->setStatusTip(tr("Save current Object(s) To"));
  AC_Save_to->setWhatsThis("Saves all currently selected objects under a new name");
  AC_Save_to->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"document-save-as.png"));
  connect(AC_Save_to, SIGNAL(triggered()), this, SIGNAL(saveToMenu()));
  fileMenu_->addAction(AC_Save_to);

  fileMenu_->addSeparator();

  //Load ini
  QAction* AC_load_ini = new QAction(tr("&Load Settings"), this);
  AC_load_ini->setStatusTip(tr("Load Settings from INI file"));
  AC_load_ini->setWhatsThis("Load a previous settings from file (objects,colors,...)");
  AC_load_ini->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"load-settings.png"));
  connect(AC_load_ini, SIGNAL(triggered()), this, SIGNAL(loadIniMenu()));
  fileMenu_->addAction(AC_load_ini);

  //Save ini
  QAction* AC_save_ini = new QAction(tr("&Save Settings"), this);
  AC_save_ini->setStatusTip(tr("Save current settings to INI file"));
  AC_save_ini->setWhatsThis("Save settings to file (objects,colors,...)");
  AC_save_ini->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"save-settings.png"));
  connect(AC_save_ini, SIGNAL(triggered()), this, SIGNAL(saveIniMenu()));
  fileMenu_->addAction(AC_save_ini);

  fileMenu_->addSeparator();

  //Options
  QAction* AC_Options = new QAction(tr("&Options"), this);
  AC_Options->setStatusTip(tr("Edit OpenFlipper Options"));
  AC_Options->setWhatsThis("Edit OpenFlipper Options");
  AC_Options->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"options.png"));
  connect(AC_Options, SIGNAL(triggered()), this, SLOT(showOptionsWidget()));
  fileMenu_->addAction(AC_Options);

  //Remember entry of menu (required for adding File Menu entries from plugins)
  fileMenuEnd_ = fileMenu_->addSeparator();

  //Recent files
  recentFilesMenu_ = new QMenu(tr("&Recent Files"));
  recentFilesMenu_->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"document-open-recent.png"));
  recentFilesMenu_->setWhatsThis("Open recent files");
  connect(recentFilesMenu_,SIGNAL(triggered(QAction*)),this,SIGNAL(recentOpen(QAction*)));
  fileMenu_->addMenu(recentFilesMenu_);

  fileMenu_->addSeparator();

  //Main Application exit menu entry
  QAction* AC_exit = new QAction(tr("&Exit"), this);;
  AC_exit->setShortcut (Qt::CTRL + Qt::Key_Q);
  AC_exit->setStatusTip(tr("Exit Application"));
  recentFilesMenu_->setWhatsThis("Close OpenFlipper");
  AC_exit->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"application-exit.png"));
  connect(AC_exit, SIGNAL(triggered()), this, SIGNAL(exit()));
  fileMenu_->addAction(AC_exit);


  // ======================================================================
  // View Menu
  // ======================================================================
  viewMenu_ = new QMenu(tr("&View"));
  menuBar()->addMenu(viewMenu_ );

  slotUpdateGlobalDrawMenu();
  viewMenu_->addMenu(globalDrawMenu_);

  //============================================================================================================
  // Rendering options Menu
  //============================================================================================================

  QMenu* renderingOptionsMenu = new QMenu("Rendering Options",viewMenu_);
  renderingOptionsMenu->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"renderingOptions.png"));
  viewMenu_->addMenu(renderingOptionsMenu);

  orthogonalProjectionAction_ = new QAction( "Switch Viewers to Orthogonal Projection", renderingOptionsMenu );;
  orthogonalProjectionAction_->setIcon( QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"orthogonal.png") );
  orthogonalProjectionAction_->setCheckable( false );
  orthogonalProjectionAction_->setToolTip(   "Switch to orthogonal projection mode.");
  orthogonalProjectionAction_->setWhatsThis( "Switch projection mode<br><br>"
      "Switch to <b>orthogonal</b> projection mode.");
  connect( orthogonalProjectionAction_,SIGNAL( triggered() ), this, SLOT( slotGlobalOrthographicProjection() ) );
  renderingOptionsMenu->addAction( orthogonalProjectionAction_);

  QAction* animation = renderingOptionsMenu->addAction("Animation");

  uint enabledCount = 0;
  uint disabledCount = 0;
  for ( int i = 0 ; i< PluginFunctions::viewers(); ++i ) {
    if ( PluginFunctions::viewerProperties(i).animation() )
      enabledCount++;
    else
      disabledCount++;
  }

  if ( enabledCount != 0 && disabledCount != 0 )
    animation->setChecked(Qt::PartiallyChecked);
  else if ( enabledCount == 4 )
    animation->setChecked( Qt::Checked );
  else
    animation->setChecked( Qt::Unchecked );

  animation->setToolTip("Animate rotation of objects");
  animation->setCheckable( true );
  animation->setIcon( QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"animation.png") );
  connect(animation, SIGNAL(triggered(bool)), this , SLOT( slotGlobalChangeAnimation(bool) ) );


  //======================

  QAction* backfaceCulling = renderingOptionsMenu->addAction("Backface Culling");

  enabledCount  = 0;
  disabledCount = 0;
  for ( int i = 0 ; i< PluginFunctions::viewers(); ++i ) {
    if ( PluginFunctions::viewerProperties(i).backFaceCulling() )
      enabledCount++;
    else
      disabledCount++;
  }

  if ( enabledCount != 0 && disabledCount != 0 )
    backfaceCulling->setChecked(Qt::PartiallyChecked);
  else if ( enabledCount == 4 )
    backfaceCulling->setChecked( Qt::Checked );
  else
    backfaceCulling->setChecked( Qt::Unchecked );

  backfaceCulling->setToolTip("Enable backface culling");
  backfaceCulling->setCheckable( true );
  backfaceCulling->setIcon( QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"backFaceCulling.png") );
  connect(backfaceCulling, SIGNAL(triggered(bool)), this , SLOT( slotGlobalChangeBackFaceCulling(bool) ) );

  //======================

  QAction* twoSidedLighting = renderingOptionsMenu->addAction("Two-sided Lighting");

  enabledCount  = 0;
  disabledCount = 0;
  for ( int i = 0 ; i< PluginFunctions::viewers(); ++i ) {
    if ( PluginFunctions::viewerProperties(i).twoSidedLighting() )
      enabledCount++;
    else
      disabledCount++;
  }

  if ( enabledCount != 0 && disabledCount != 0 )
    twoSidedLighting->setChecked(Qt::PartiallyChecked);
  else if ( enabledCount == 4 )
    twoSidedLighting->setChecked( Qt::Checked );
  else
    twoSidedLighting->setChecked( Qt::Unchecked );

  twoSidedLighting->setToolTip("Enable two-sided lighting");
  twoSidedLighting->setCheckable( true );
  twoSidedLighting->setIcon( QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"twosidedLighting.png") );
  twoSidedLighting->setChecked( PluginFunctions::viewerProperties().twoSidedLighting() );
  connect(twoSidedLighting, SIGNAL(triggered(bool)), this , SLOT( slotGlobalChangeTwoSidedLighting(bool) ) );

  //============================================================================================================
  // Other toplevel actions
  //============================================================================================================

  viewMenu_->addSeparator();

  connect( viewMenu_,SIGNAL( aboutToShow() ), this, SLOT( slotViewMenuAboutToShow() ) );

  QAction* homeAction = new QAction("Restore Home View",viewMenu_);
  homeAction->setIcon( QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"go-home.png") );
  homeAction->setCheckable( false );
  homeAction->setStatusTip("Restore <b>home</b> view.");
  homeAction->setWhatsThis( "Restore home view<br><br>"
                            "Resets the view to the home view");
  viewMenu_->addAction( homeAction );
  connect( homeAction,SIGNAL( triggered() ), this, SLOT( slotGlobalHomeView() ) );


  QAction* setHomeAction = new QAction( "Set Home View" , viewMenu_ );
  setHomeAction->setIcon( QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"set-home.png") );
  setHomeAction->setCheckable( false );
  setHomeAction->setStatusTip("Set <b>home</b> view");
  setHomeAction->setWhatsThis( "Store home view<br><br>"
                               "Stores the current view as the home view");
  viewMenu_->addAction( setHomeAction);
  connect( setHomeAction,SIGNAL( triggered() ), this, SLOT( slotGlobalSetHomeView() ) );

  QAction* viewAllAction = new QAction( "View all", viewMenu_ );
  viewAllAction->setIcon( QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"viewall.png") );
  viewAllAction->setCheckable( false );
  viewAllAction->setStatusTip("View all.");
  viewAllAction->setWhatsThis( "View all<br><br>"
                               "Move the objects in the scene so that"
                               " the whole scene is visible.");
  connect( viewAllAction,SIGNAL( triggered() ), this, SLOT( slotGlobalViewAll() ) );
  viewMenu_->addAction( viewAllAction);

  viewMenu_->addSeparator();

  QAction* snapShotAction = new QAction( "Viewer Snapshot", viewMenu_ );
  snapShotAction->setIcon( QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"viewerSnapshot.png") );
  snapShotAction->setCheckable( false );
  snapShotAction->setStatusTip("Take a snapshot from all viewers.");
  snapShotAction->setWhatsThis( "Viewer Snapshot<br><br>"
                               "Take a snapshot of all viewers at once.");
  connect( snapShotAction,SIGNAL( triggered() ), this, SLOT( viewerSnapshotDialog() ) );
  viewMenu_->addAction( snapShotAction);

  QAction* appSnapShotAction = new QAction( "Application Snapshot", viewMenu_ );
  appSnapShotAction->setIcon( QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"snapshot.png") );
  appSnapShotAction->setCheckable( false );
  appSnapShotAction->setStatusTip("Take a snapshot from OpenFlipper.");
  appSnapShotAction->setWhatsThis( "Snapshot<br><br>"
                               "Take a snapshot from OpenFlipper.");
  connect( appSnapShotAction,SIGNAL( triggered() ), this, SLOT( applicationSnapshotDialog() ) );
  viewMenu_->addAction( appSnapShotAction);

  perspectiveProjectionAction_ = new QAction( "Switch Viewers to Perspective Projection", viewMenu_ );;
  perspectiveProjectionAction_->setIcon( QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"perspective.png") );
  perspectiveProjectionAction_->setCheckable( false );
  perspectiveProjectionAction_->setStatusTip(   "Switch to perspective projection mode.");
  perspectiveProjectionAction_->setWhatsThis( "Switch projection mode<br><br>"
                                              "Switch to <b>perspective</b> projection mode.");
  connect( perspectiveProjectionAction_,SIGNAL( triggered() ), this, SLOT( slotGlobalPerspectiveProjection() ) );
  viewMenu_->addAction( perspectiveProjectionAction_);

  viewMenu_->addSeparator();

  QAction* wheelSwitchAction = new QAction( "Show / hide wheels", viewMenu_ );
  wheelSwitchAction->setCheckable( true );
  wheelSwitchAction->setStatusTip("Show / hide navigation wheels in viewer widget.");
  wheelSwitchAction->setWhatsThis( "Show / hide navigation wheels in viewer widget.<br><br>"
                               " These wheels appear in the corners of the viewports. "
							   " Use wheels to rotate and scale scene.");
  if(OpenFlipper::Options::showWheelsAtStartup()) {
	  wheelSwitchAction->setChecked(true);
  }
  connect( wheelSwitchAction,SIGNAL( toggled(bool) ), this, SLOT( slotSwitchWheels(bool) ) );
  viewMenu_->addAction( wheelSwitchAction);

  QAction* coordSys = viewMenu_->addAction("Coordinate Systems");
  coordSys->setCheckable(true);
  coordSys->setChecked(true);
  coordSys->setStatusTip("Toggle visibility of the coordinate systems");
//   coordSys->setIcon( QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"snapshot.png") );
  connect(coordSys, SIGNAL(triggered(bool)), this, SLOT( slotCoordSysVisibility(bool) ) );

  viewMenu_->addSeparator();

  QAction* setGlobalBackgroundColor = new QAction(tr("&Set Background Color"), this);;
  setGlobalBackgroundColor->setToolTip(tr("Set Background Color for all viewers"));
  setGlobalBackgroundColor->setStatusTip(tr("Set Background Color for all viewers"));
  setGlobalBackgroundColor->setWhatsThis("Set Background Color for all viewers");
  setGlobalBackgroundColor->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"BackgroundColor.png") );
  connect(setGlobalBackgroundColor, SIGNAL(triggered()), this, SLOT(slotSetGlobalBackgroundColor()));
  viewMenu_->addAction(setGlobalBackgroundColor);

  //===========================================================================================================================
  // Tools Menu
  //===========================================================================================================================

  toolsMenu_ = new QMenu(tr("&Tools"));
  menuBar()->addMenu(toolsMenu_ );

  QAction* sceneGraphAction = new QAction( "Show SceneGraph " ,toolsMenu_ );
  sceneGraphAction->setIcon( QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"scenegraph.png") );
  sceneGraphAction->setCheckable( false );
  sceneGraphAction->setToolTip("Show scene graph viewer.");
  sceneGraphAction->setWhatsThis( "Toggle scene graph viewer<br><br>"
                                  "The scene graph viewer enables you to examine the "
                                  "displayed scene graph and to modify certain nodes.<br><br>" );
  QObject::connect( sceneGraphAction, SIGNAL( triggered() ),
                    this,             SLOT( slotShowSceneGraphDialog() ) );
  toolsMenu_->addAction( sceneGraphAction);

  toolsMenu_->addSeparator();

  QAction* startVideoCaptureAction =  new QAction( "Start Video Capture " ,toolsMenu_ );
  startVideoCaptureAction->setIcon( QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"recordVideo.png") );
  startVideoCaptureAction->setCheckable( false );
  startVideoCaptureAction->setToolTip( "Start video capturing." );
  startVideoCaptureAction->setWhatsThis( "Start to capture a video sequence of the user actions");
  toolsMenu_->addAction( startVideoCaptureAction );
  connect(startVideoCaptureAction, SIGNAL(triggered()), this, SLOT(startVideoCaptureDialog()) );

  QAction* stopVideoCaptureAction =  new QAction( "Stop Video Capture " ,toolsMenu_ );
  stopVideoCaptureAction->setIcon( QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"stopVideoCapture.png") );
  stopVideoCaptureAction->setCheckable( false );
  stopVideoCaptureAction->setToolTip( "Stop video capturing." );
  stopVideoCaptureAction->setWhatsThis( "Stop Video capturing");

  toolsMenu_->addAction( stopVideoCaptureAction);
  connect(stopVideoCaptureAction, SIGNAL(triggered()), this, SIGNAL(stopVideoCapture()) );

  // ======================================================================
  // help Menu
  // ======================================================================
  helpMenu_ = new QMenu(tr("&Help"));
  menuBar()->addMenu(helpMenu_);

  //Open Help Browser
  QAction* AC_HelpBrowser = new QAction(tr("&Help"), this);
  AC_HelpBrowser->setStatusTip(tr("Open Help Browser with Documentation"));
  AC_HelpBrowser->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"help-browser.png"));
  AC_HelpBrowser->setWhatsThis("Open the <b>Help Browser</b>");
  connect(AC_HelpBrowser, SIGNAL(triggered()), this, SLOT(showHelpBrowser()));
  helpMenu_->addAction(AC_HelpBrowser);

  //Switch to whats this mode
  QAction* AC_Whats_this = QWhatsThis::createAction ( this );
  AC_Whats_this->setStatusTip(tr("Enter What's this Mode"));
  AC_Whats_this->setWhatsThis("Get information about a specific Button/Widget/...");
  helpMenu_->addAction(AC_Whats_this);

  helpMenu_->addSeparator();

  //show plugins
  QAction* AC_Plugins = new QAction(tr("&Plugins"), this);
  AC_Plugins->setStatusTip(tr("Show loaded plugins"));
  AC_Plugins->setWhatsThis("Show loaded plugins");
  AC_Plugins->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"network-connect.png"));
  connect(AC_Plugins, SIGNAL(triggered()), this, SIGNAL(showPlugins()));
  helpMenu_->addAction(AC_Plugins);

  helpMenu_->addSeparator();

  //About Action
  QAction* AC_About = new QAction(tr("&About"), this);
  AC_About->setStatusTip(tr("About OpenFlipper"));
  AC_About->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"help-about.png"));
  AC_About->setWhatsThis("This entry shows information about <b>OpenFlipper</b>");
  connect(AC_About, SIGNAL(triggered()), this, SLOT(showAboutWidget()));
  helpMenu_->addAction(AC_About);

  // Add Menu entries to the main Toolbar
  mainToolbar_->addAction(AC_Load);
  mainToolbar_->addAction(AC_AddEmpty);
  mainToolbar_->addSeparator();
  mainToolbar_->addAction(AC_Save);
  mainToolbar_->addAction(AC_Save_to);
  mainToolbar_->addSeparator();
  mainToolbar_->addAction(AC_load_ini);
  mainToolbar_->addAction(AC_save_ini);

}

void CoreWidget::slotViewMenuAboutToShow() {

  uint perspectiveCount = 0;
  uint orthogonalCount = 0;

  for ( int i = 0 ; i < PluginFunctions::viewers() ; ++i ) {
    if ( examiner_widgets_[ i ]->projectionMode() == glViewer::PERSPECTIVE_PROJECTION )
      perspectiveCount++;
    else
      orthogonalCount++;
  }

  if ( perspectiveCount == 4 )
    perspectiveProjectionAction_->setVisible(false);
  else
    perspectiveProjectionAction_->setVisible(true);

  if ( orthogonalCount == 4 )
    orthogonalProjectionAction_->setVisible(false);
  else
    orthogonalProjectionAction_->setVisible(true);

}

void CoreWidget::slotUpdateGlobalDrawMenu() {
  if ( drawGroup_ ) {

    disconnect( drawGroup_ , SIGNAL( triggered( QAction * ) ),
                this       , SLOT( slotGlobalDrawMenu( QAction * ) ) );
    delete( drawGroup_ );
    drawGroup_ = 0;

  }

  // Recreate drawGroup
  drawGroup_ = new QActionGroup( this );
  drawGroup_->setExclusive( false );

  connect( drawGroup_ , SIGNAL( triggered( QAction * ) ),
           this       , SLOT( slotGlobalDrawMenu( QAction * ) ) );

  if ( !globalDrawMenu_ ) {

    QIcon icon;
    icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"drawModes.png");
    globalDrawMenu_  = new QMenu("Set Global DrawMode");
    globalDrawMenu_->setTearOffEnabled(true);
    globalDrawMenu_->setIcon(icon);

    connect(globalDrawMenu_,SIGNAL(aboutToShow () ) , this, SLOT(slotUpdateGlobalDrawMenu() ) );
  }

  // Collect available draw Modes
  ACG::SceneGraph::CollectDrawModesAction actionAvailable;
  ACG::SceneGraph::traverse( PluginFunctions::getRootNode() , actionAvailable);
  availableGlobalDrawModes_ = actionAvailable.drawModes();

  // Get currently active drawModes (first viewer only )
  // TODO: create combination from all viewers!
  activeDrawModes_ = INT_MAX;
  for ( int i = 0 ; i < PluginFunctions::viewers(); ++i )
    activeDrawModes_ &= PluginFunctions::drawMode(i);

  // Convert to ids
  std::vector< unsigned int > availDrawModeIds;
  availDrawModeIds = ACG::SceneGraph::DrawModes::getDrawModeIDs( availableGlobalDrawModes_ );

  globalDrawMenu_->clear();

  for ( unsigned int i = 0; i < availDrawModeIds.size(); ++i )
  {
    unsigned int id    = availDrawModeIds[i];
    std::string  descr = ACG::SceneGraph::DrawModes::description( id );

    QAction * action = new QAction( descr.c_str(), drawGroup_ );
    action->setCheckable( true );
    action->setChecked( ACG::SceneGraph::DrawModes::containsId( activeDrawModes_, id ) );
  }

  globalDrawMenu_->addActions( drawGroup_->actions() );

}

void CoreWidget::slotGlobalDrawMenu(QAction * _action) {

  //======================================================================================
  // Get the mode toggled
  //======================================================================================
  unsigned int mode = 0;
  std::vector< unsigned int > availDrawModeIds;
  availDrawModeIds = ACG::SceneGraph::DrawModes::getDrawModeIDs( availableGlobalDrawModes_ );
  for ( unsigned int i = 0; i < availDrawModeIds.size(); ++i )
  {
    QString descr = QString( ACG::SceneGraph::DrawModes::description( availDrawModeIds[i] ).c_str() );

    if ( descr == _action->text() ) {
      mode = availDrawModeIds[i];
      break;
    }
  }

  if ( qApp->keyboardModifiers() & Qt::ShiftModifier )
    activeDrawModes_ = ( activeDrawModes_ ^ mode);
  else
    activeDrawModes_ = mode ;

  PluginFunctions::setDrawMode( activeDrawModes_ );
  slotUpdateGlobalDrawMenu();
}


//=============================================================================
