/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2009 by Computer Graphics Group, RWTH Aachen      *
 *                           www.openflipper.org                             *
 *                                                                           *
 *---------------------------------------------------------------------------*
 *  This file is part of OpenFlipper.                                        *
 *                                                                           *
 *  OpenFlipper is free software: you can redistribute it and/or modify      *
 *  it under the terms of the GNU Lesser General Public License as           *
 *  published by the Free Software Foundation, either version 3 of           *
 *  the License, or (at your option) any later version with the              *
 *  following exceptions:                                                    *
 *                                                                           *
 *  If other files instantiate templates or use macros                       *
 *  or inline functions from this file, or you compile this file and         *
 *  link it with other files to produce an executable, this file does        *
 *  not by itself cause the resulting executable to be covered by the        *
 *  GNU Lesser General Public License. This exception does not however       *
 *  invalidate any other reasons why the executable file might be            *
 *  covered by the GNU Lesser General Public License.                        *
 *                                                                           *
 *  OpenFlipper is distributed in the hope that it will be useful,           *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU Lesser General Public License for more details.                      *
 *                                                                           *
 *  You should have received a copy of the GNU LesserGeneral Public          *
 *  License along with OpenFlipper. If not,                                  *
 *  see <http://www.gnu.org/licenses/>.                                      *
 *                                                                           *
\*===========================================================================*/

/*===========================================================================*\
 *                                                                           *
 *   $Revision$                                                         *
 *   $Author$                                                      *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/




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
#include <OpenFlipper/BasePlugin/MenuInterface.hh>

//== IMPLEMENTATION ==========================================================



void CoreWidget::slotAddMenubarAction( QAction* _action , QString _name ) {

  if (!menus_.contains (_name))
    return;

  if (_name == FILEMENU)
  {
    fileMenu_->insertSeparator(fileMenuEnd_);
    fileMenu_->insertAction( fileMenuEnd_ , _action );
  }
  else
    menus_[_name]->addAction (_action);
}

//=============================================================================

void CoreWidget::slotGetMenubarMenu (QString _name, QMenu *& _menu, bool _create)
{
  if (menus_.contains (_name))
    _menu = menus_[_name];
  else if (_create)
  {
    _menu = new QMenu(_name);
    menus_[_name] = _menu;
    menuBar()->insertAction(helpMenu_->menuAction() ,_menu->menuAction ());
  } else
    _menu = NULL;
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
  fileMenu_ = new QMenu( FILEMENU );
  menuBar()->addMenu(fileMenu_ );
  menus_[tr("File")] = fileMenu_;

  //Clear all
  QAction* AC_clear_all = new QAction(tr("Clear All"), this);;
  AC_clear_all->setStatusTip(tr("Clear all Objects"));
  AC_clear_all->setWhatsThis(tr("Close all open Objects"));
  AC_clear_all->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"edit-clear.png"));
  connect(AC_clear_all, SIGNAL(triggered()), this, SIGNAL(clearAll()));
  fileMenu_->addAction(AC_clear_all);

  fileMenu_->addSeparator();

  //Load object
  QAction* AC_Load = new QAction(tr("Load Object"), this);
  AC_Load->setStatusTip(tr("Load an object"));
  AC_Load->setWhatsThis(tr("Load a new object"));
  AC_Load->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"document-open.png"));

  connect(AC_Load, SIGNAL(triggered()), this, SIGNAL(loadMenu()));
  fileMenu_->addAction(AC_Load);

  //Add empty object
  QAction* AC_AddEmpty = new QAction(tr("Add Empty Object"), this);
  AC_AddEmpty->setStatusTip(tr("Add an empty object"));
  AC_AddEmpty->setWhatsThis(tr("Creates a new empty object of a given type"));
  AC_AddEmpty->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"add-empty-object.png"));
  connect(AC_AddEmpty, SIGNAL(triggered()), this, SIGNAL(addEmptyObjectMenu()));
  fileMenu_->addAction(AC_AddEmpty);

  fileMenu_->addSeparator();

  //Save object
  QAction* AC_Save = new QAction(tr("Save Object"), this);
//   AC_Save->setShortcut (Qt::CTRL + Qt::Key_S);
  AC_Save->setStatusTip(tr("Save currently selected objects"));
  AC_Save->setWhatsThis(tr("Saves all currently selected objects"));
  AC_Save->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"document-save.png"));
  connect(AC_Save, SIGNAL(triggered()), this, SIGNAL(saveMenu()));
  fileMenu_->addAction(AC_Save);

  //Save object to
  QAction* AC_Save_to = new QAction(tr("Save Object To"), this);
  AC_Save_to->setStatusTip(tr("Save current Object(s) To"));
  AC_Save_to->setWhatsThis(tr("Saves all currently selected objects under a new name"));
  AC_Save_to->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"document-save-as.png"));
  connect(AC_Save_to, SIGNAL(triggered()), this, SIGNAL(saveToMenu()));
  fileMenu_->addAction(AC_Save_to);

  fileMenu_->addSeparator();

  //Load ini
  QAction* AC_load_ini = new QAction(tr("Load Settings"), this);
  AC_load_ini->setStatusTip(tr("Load Settings from INI file"));
  AC_load_ini->setWhatsThis(tr("Load a previous settings from file (objects,colors,...)"));
  AC_load_ini->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"load-settings.png"));
  connect(AC_load_ini, SIGNAL(triggered()), this, SIGNAL(loadIniMenu()));
  fileMenu_->addAction(AC_load_ini);

  //Save ini
  QAction* AC_save_ini = new QAction(tr("Save Settings"), this);
  AC_save_ini->setStatusTip(tr("Save current settings to INI file"));
  AC_save_ini->setWhatsThis(tr("Save settings to file (objects,colors,...)"));
  AC_save_ini->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"save-settings.png"));
  connect(AC_save_ini, SIGNAL(triggered()), this, SIGNAL(saveIniMenu()));
  fileMenu_->addAction(AC_save_ini);

  fileMenu_->addSeparator();

  //Options
  QAction* AC_Options = new QAction(tr("Options"), this);
  AC_Options->setStatusTip(tr("Edit OpenFlipper Options"));
  AC_Options->setWhatsThis(tr("Edit OpenFlipper Options"));
  AC_Options->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"options.png"));
  connect(AC_Options, SIGNAL(triggered()), this, SLOT(showOptionsWidget()));
  fileMenu_->addAction(AC_Options);

  //Remember entry of menu (required for adding File Menu entries from plugins)
  fileMenuEnd_ = fileMenu_->addSeparator();

  //Recent files
  recentFilesMenu_ = new QMenu(tr("Recent Files"));
  recentFilesMenu_->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"document-open-recent.png"));
  recentFilesMenu_->setWhatsThis(tr("Open recent files"));
  connect(recentFilesMenu_,SIGNAL(triggered(QAction*)),this,SIGNAL(recentOpen(QAction*)));
  fileMenu_->addMenu(recentFilesMenu_);

  fileMenu_->addSeparator();

  //Main Application exit menu entry
  QAction* AC_exit = new QAction(tr("Exit"), this);;
  AC_exit->setShortcut (Qt::CTRL + Qt::Key_Q);
  AC_exit->setStatusTip(tr("Exit Application"));
  recentFilesMenu_->setWhatsThis(tr("Close OpenFlipper"));
  AC_exit->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"application-exit.png"));
  connect(AC_exit, SIGNAL(triggered()), this, SIGNAL(exit()));
  fileMenu_->addAction(AC_exit);


  // ======================================================================
  // View Menu
  // ======================================================================
  viewMenu_ = new QMenu( VIEWMENU );
  menuBar()->addMenu(viewMenu_ );
  menus_[tr("View")] = viewMenu_;

  slotUpdateGlobalDrawMenu();
  viewMenu_->addMenu(globalDrawMenu_);

  //============================================================================================================
  // Rendering options Menu
  //============================================================================================================

  QMenu* renderingOptionsMenu = new QMenu(tr("Rendering Options"),viewMenu_);
  renderingOptionsMenu->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"renderingOptions.png"));
  viewMenu_->addMenu(renderingOptionsMenu);

  orthogonalProjectionAction_ = new QAction( tr("Switch Viewers to Orthogonal Projection"), renderingOptionsMenu );;
  orthogonalProjectionAction_->setIcon( QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"orthogonal.png") );
  orthogonalProjectionAction_->setCheckable( false );
  orthogonalProjectionAction_->setToolTip(   tr("Switch to orthogonal projection mode."));
  orthogonalProjectionAction_->setWhatsThis( tr("Switch projection mode<br><br>"
      "Switch to <b>orthogonal</b> projection mode."));
  connect( orthogonalProjectionAction_,SIGNAL( triggered() ), this, SLOT( slotGlobalOrthographicProjection() ) );
  renderingOptionsMenu->addAction( orthogonalProjectionAction_);

  // =====================
  
  globalAnimationAction_ = renderingOptionsMenu->addAction(tr("Animation"));
  globalAnimationAction_->setCheckable( true );
  globalAnimationAction_->setIcon( QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"animation.png") );
  connect(globalAnimationAction_, SIGNAL(triggered(bool)), this , SLOT( slotGlobalChangeAnimation(bool) ) );

  //======================

  globalBackfaceCullingAction_ = renderingOptionsMenu->addAction(tr("Backface Culling"));
  globalBackfaceCullingAction_->setCheckable( true );  
  globalBackfaceCullingAction_->setIcon( QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"backFaceCulling.png") );
  connect(globalBackfaceCullingAction_, SIGNAL(triggered(bool)), this , SLOT( slotGlobalChangeBackFaceCulling(bool) ) );

  //======================

  globalTwosidedLightingAction_ = renderingOptionsMenu->addAction(tr("Two-sided Lighting"));
  globalTwosidedLightingAction_->setCheckable( true );
  globalTwosidedLightingAction_->setIcon( QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"twosidedLighting.png") );
  connect(globalTwosidedLightingAction_, SIGNAL(triggered(bool)), this , SLOT( slotGlobalChangeTwoSidedLighting(bool) ) );
  
  //======================

  globalMultisamplingAction_ = renderingOptionsMenu->addAction(tr("Multisampling"));
  globalMultisamplingAction_->setCheckable( true );
// TODO:Icon for multisampling  
//   globalMultisamplingAction_->setIcon( QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"twosidedLighting.png") );
  connect(globalMultisamplingAction_, SIGNAL(triggered(bool)), this , SLOT( slotGlobalChangeMultisampling(bool)) );  

  //============================================================================================================
  // Other toplevel actions
  //============================================================================================================

  viewMenu_->addSeparator();

  connect( viewMenu_,SIGNAL( aboutToShow() ), this, SLOT( slotViewMenuAboutToShow() ) );

  QAction* homeAction = new QAction(tr("Restore Home View"),viewMenu_);
  homeAction->setIcon( QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"go-home.png") );
  homeAction->setCheckable( false );
  homeAction->setStatusTip( tr("Restore <b>home</b> view."));
  homeAction->setWhatsThis( tr("Restore home view<br><br>"
                               "Resets the view to the home view"));
  viewMenu_->addAction( homeAction );
  connect( homeAction,SIGNAL( triggered() ), this, SLOT( slotGlobalHomeView() ) );


  QAction* setHomeAction = new QAction( tr("Set Home View") , viewMenu_ );
  setHomeAction->setIcon( QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"set-home.png") );
  setHomeAction->setCheckable( false );
  setHomeAction->setStatusTip( tr("Set <b>home</b> view"));
  setHomeAction->setWhatsThis( tr("Store home view<br><br>"
                                  "Stores the current view as the home view"));
  viewMenu_->addAction( setHomeAction);
  connect( setHomeAction,SIGNAL( triggered() ), this, SLOT( slotGlobalSetHomeView() ) );

  QAction* viewAllAction = new QAction( tr("View all"), viewMenu_ );
  viewAllAction->setIcon( QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"viewall.png") );
  viewAllAction->setCheckable( false );
  viewAllAction->setStatusTip( tr("View all.") );
  viewAllAction->setWhatsThis( tr("View all<br><br>"
                                  "Move the objects in the scene so that"
                                  " the whole scene is visible."));
  connect( viewAllAction,SIGNAL( triggered() ), this, SLOT( slotGlobalViewAll() ) );
  viewMenu_->addAction( viewAllAction);

  viewMenu_->addSeparator();

  QAction* snapShotAction = new QAction( tr("Viewer Snapshot"), viewMenu_ );
  snapShotAction->setIcon( QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"viewerSnapshot.png") );
  snapShotAction->setCheckable( false );
  snapShotAction->setStatusTip( tr("Take a snapshot from all viewers."));
  snapShotAction->setWhatsThis( tr("Viewer Snapshot<br><br>"
                                   "Take a snapshot of all viewers at once."));
  connect( snapShotAction,SIGNAL( triggered() ), this, SLOT( viewerSnapshotDialog() ) );
  viewMenu_->addAction( snapShotAction);

  QAction* appSnapShotAction = new QAction( tr("Application Snapshot"), viewMenu_ );
  appSnapShotAction->setIcon( QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"snapshot.png") );
  appSnapShotAction->setCheckable( false );
  appSnapShotAction->setStatusTip( tr("Take a snapshot from OpenFlipper."));
  appSnapShotAction->setWhatsThis( tr("Snapshot<br><br>"
                                      "Take a snapshot from OpenFlipper."));
  connect( appSnapShotAction,SIGNAL( triggered() ), this, SLOT( applicationSnapshotDialog() ) );
  viewMenu_->addAction( appSnapShotAction);

  perspectiveProjectionAction_ = new QAction( tr("Switch Viewers to Perspective Projection"), viewMenu_ );;
  perspectiveProjectionAction_->setIcon( QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"perspective.png") );
  perspectiveProjectionAction_->setCheckable( false );
  perspectiveProjectionAction_->setStatusTip( tr("Switch to perspective projection mode."));
  perspectiveProjectionAction_->setWhatsThis( tr("Switch projection mode<br><br>"
                                                 "Switch to <b>perspective</b> projection mode."));
  connect( perspectiveProjectionAction_,SIGNAL( triggered() ), this, SLOT( slotGlobalPerspectiveProjection() ) );
  viewMenu_->addAction( perspectiveProjectionAction_);

  viewMenu_->addSeparator();

  QAction* wheelSwitchAction = new QAction( tr("Show / hide wheels"), viewMenu_ );
  wheelSwitchAction->setCheckable( true );
  wheelSwitchAction->setStatusTip( tr("Show / hide navigation wheels in viewer widget."));
  wheelSwitchAction->setWhatsThis( tr("Show / hide navigation wheels in viewer widget.<br><br>"
                                      " These wheels appear in the corners of the viewports. "
                                      " Use wheels to rotate and scale scene."));
  if(OpenFlipper::Options::showWheelsAtStartup()) {
	  wheelSwitchAction->setChecked(true);
  }
  connect( wheelSwitchAction,SIGNAL( toggled(bool) ), this, SLOT( slotSwitchWheels(bool) ) );
  viewMenu_->addAction( wheelSwitchAction);

  QAction* coordSys = viewMenu_->addAction(tr("Coordinate Systems"));
  coordSys->setCheckable(true);
  coordSys->setChecked(true);
  coordSys->setStatusTip(tr("Toggle visibility of the coordinate systems"));
  coordSys->setIcon( QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"coordsys.png") );
  connect(coordSys, SIGNAL(triggered(bool)), this, SLOT( slotCoordSysVisibility(bool) ) );

  viewMenu_->addSeparator();

  QAction* setGlobalBackgroundColor = new QAction(tr("Set Background Color"), this);;
  setGlobalBackgroundColor->setToolTip(tr("Set Background Color for all viewers"));
  setGlobalBackgroundColor->setStatusTip(tr("Set Background Color for all viewers"));
  setGlobalBackgroundColor->setWhatsThis(tr("Set Background Color for all viewers"));
  setGlobalBackgroundColor->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"BackgroundColor.png") );
  connect(setGlobalBackgroundColor, SIGNAL(triggered()), this, SLOT(slotSetGlobalBackgroundColor()));
  viewMenu_->addAction(setGlobalBackgroundColor);

  //===========================================================================================================================
  // Tools Menu
  //===========================================================================================================================

  toolsMenu_ = new QMenu( TOOLSMENU );
  menuBar()->addMenu(toolsMenu_ );
  menus_[tr("Tools")] = toolsMenu_;

  QAction* sceneGraphAction = new QAction( tr("Show SceneGraph ") ,toolsMenu_ );
  sceneGraphAction->setIcon( QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"scenegraph.png") );
  sceneGraphAction->setCheckable( false );
  sceneGraphAction->setToolTip( tr("Show scene graph viewer.") );
  sceneGraphAction->setWhatsThis( tr("Toggle scene graph viewer<br><br>"
                                     "The scene graph viewer enables you to examine the "
                                     "displayed scene graph and to modify certain nodes.<br><br>" ) );
  QObject::connect( sceneGraphAction, SIGNAL( triggered() ),
                    this,             SLOT( slotShowSceneGraphDialog() ) );
  toolsMenu_->addAction( sceneGraphAction);

  toolsMenu_->addSeparator();

  QAction* startVideoCaptureAction =  new QAction( tr("Start Video Capture ") ,toolsMenu_ );
  startVideoCaptureAction->setIcon( QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"recordVideo.png") );
  startVideoCaptureAction->setCheckable( false );
  startVideoCaptureAction->setToolTip( tr("Start video capturing.") );
  startVideoCaptureAction->setWhatsThis( tr("Start to capture a video sequence of the user actions")) ;
  toolsMenu_->addAction( startVideoCaptureAction );
  connect(startVideoCaptureAction, SIGNAL(triggered()), this, SLOT(startVideoCaptureDialog()) );

  QAction* stopVideoCaptureAction =  new QAction( tr("Stop Video Capture ") ,toolsMenu_ );
  stopVideoCaptureAction->setIcon( QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"stopVideoCapture.png") );
  stopVideoCaptureAction->setCheckable( false );
  stopVideoCaptureAction->setToolTip( tr("Stop video capturing." ));
  stopVideoCaptureAction->setWhatsThis( tr("Stop Video capturing" ));

  toolsMenu_->addAction( stopVideoCaptureAction);
  connect(stopVideoCaptureAction, SIGNAL(triggered()), this, SIGNAL(stopVideoCapture()) );

  // ======================================================================
  // help Menu
  // ======================================================================
  helpMenu_ = new QMenu(tr("Help"));
  menuBar()->addMenu(helpMenu_);
  menus_[tr("Help")] = helpMenu_;

  //Open Help Browser
  QAction* AC_HelpBrowser = new QAction(tr("Help"), this);
  AC_HelpBrowser->setStatusTip(tr("Open Help Browser with Documentation"));
  AC_HelpBrowser->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"help-browser.png"));
  AC_HelpBrowser->setWhatsThis(tr("Open the <b>Help Browser</b>"));
  connect(AC_HelpBrowser, SIGNAL(triggered()), this, SLOT(showHelpBrowser()));
  helpMenu_->addAction(AC_HelpBrowser);

  //Switch to whats this mode
  QAction* AC_Whats_this = QWhatsThis::createAction ( this );
  AC_Whats_this->setStatusTip(tr("Enter What's this Mode"));
  AC_Whats_this->setWhatsThis(tr("Get information about a specific Button/Widget/..."));
  helpMenu_->addAction(AC_Whats_this);

  helpMenu_->addSeparator();

  //show plugins
  QAction* AC_Plugins = new QAction(tr("Plugins"), this);
  AC_Plugins->setStatusTip(tr("Show loaded plugins"));
  AC_Plugins->setWhatsThis(tr("Show loaded plugins"));
  AC_Plugins->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"network-connect.png"));
  connect(AC_Plugins, SIGNAL(triggered()), this, SIGNAL(showPlugins()));
  helpMenu_->addAction(AC_Plugins);

  helpMenu_->addSeparator();

  //About Action
  QAction* AC_About = new QAction(tr("About"), this);
  AC_About->setStatusTip(tr("About OpenFlipper"));
  AC_About->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"help-about.png"));
  AC_About->setWhatsThis(tr("This entry shows information about <b>OpenFlipper</b>"));
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

  uint enabledCount  = 0;
  uint disabledCount = 0;
  
  for ( int i = 0 ; i< PluginFunctions::viewers(); ++i ) {
    if ( PluginFunctions::viewerProperties(i).animation() )
      enabledCount++;
    else
      disabledCount++;
  }

  if ( enabledCount != 0 && disabledCount != 0 ) {
    globalAnimationAction_->setChecked(Qt::PartiallyChecked);
    globalAnimationAction_->setToolTip(tr("Disable animation for all viewers"));  
    globalAnimationAction_->setStatusTip(tr("Disable animation for all viewers"));  
    globalAnimationAction_->setText(tr("Disable animation"));  
  } else if ( enabledCount == 4 ) {
    globalAnimationAction_->setChecked( Qt::Checked );
    globalAnimationAction_->setToolTip(tr("Disable animation for all viewers"));  
    globalAnimationAction_->setStatusTip(tr("Disable animation for all viewers"));  
    globalAnimationAction_->setText(tr("Disable animation"));  
  } else {
    globalAnimationAction_->setChecked( Qt::Unchecked );
    globalAnimationAction_->setToolTip(tr("Enable animation for all viewers"));  
    globalAnimationAction_->setStatusTip(tr("Enable animation for all viewers"));  
    globalAnimationAction_->setText(tr("Enable animation"));  
  }
  
  //=============================================================================================================================
  
  enabledCount  = 0;
  disabledCount = 0;  
  
  for ( int i = 0 ; i< PluginFunctions::viewers(); ++i ) {
    if ( PluginFunctions::viewerProperties(i).backFaceCulling() )
      enabledCount++;
    else
      disabledCount++;
  }
  
  if ( enabledCount != 0 && disabledCount != 0 ) {
    globalBackfaceCullingAction_->setChecked(Qt::PartiallyChecked);
    globalBackfaceCullingAction_->setToolTip(tr("Disable backface culling for all viewers"));  
    globalBackfaceCullingAction_->setStatusTip(tr("Disable backface culling for all viewers"));  
    globalBackfaceCullingAction_->setText(tr("Disable backface culling"));  
  } else if ( enabledCount == 4 ) {
    globalBackfaceCullingAction_->setChecked( Qt::Checked );
    globalBackfaceCullingAction_->setToolTip(tr("Disable backface culling for all viewers"));  
    globalBackfaceCullingAction_->setStatusTip(tr("Disable backface culling for all viewers"));  
    globalBackfaceCullingAction_->setText(tr("Disable backface culling"));  
  } else {
    globalBackfaceCullingAction_->setChecked( Qt::Unchecked );
    globalBackfaceCullingAction_->setToolTip(tr("Enable backface culling for all viewers"));  
    globalBackfaceCullingAction_->setStatusTip(tr("Enable backface culling for all viewers"));  
    globalBackfaceCullingAction_->setText(tr("Enable backface culling"));  
  }

  //=============================================================================================================================
  
  enabledCount  = 0;
  disabledCount = 0;
  
  for ( int i = 0 ; i< PluginFunctions::viewers(); ++i ) {
    if ( PluginFunctions::viewerProperties(i).twoSidedLighting() )
      enabledCount++;
    else
      disabledCount++;
  }

  if ( enabledCount != 0 && disabledCount != 0 ) {
    globalTwosidedLightingAction_->setChecked(Qt::PartiallyChecked);
    globalTwosidedLightingAction_->setToolTip(tr("Disable two-sided lighting for all viewers"));  
    globalTwosidedLightingAction_->setStatusTip(tr("Disable two-sided lighting for all viewers"));  
    globalTwosidedLightingAction_->setText(tr("Disable two-sided lighting"));  
  } else if ( enabledCount == 4 ) {
    globalTwosidedLightingAction_->setChecked( Qt::Checked );
    globalTwosidedLightingAction_->setToolTip(tr("Disable two-sided lighting for all viewers"));  
    globalTwosidedLightingAction_->setStatusTip(tr("Disable two-sided lighting for all viewers"));  
    globalTwosidedLightingAction_->setText(tr("Disable two-sided lighting"));  
  } else {
    globalTwosidedLightingAction_->setChecked( Qt::Unchecked );
    globalTwosidedLightingAction_->setToolTip(tr("Enable two-sided lighting for all viewers"));  
    globalTwosidedLightingAction_->setStatusTip(tr("Enable two-sided lighting for all viewers"));  
    globalTwosidedLightingAction_->setText(tr("Enable two-sided lighting"));      
  }

  //=============================================================================================================================
  
  enabledCount  = 0;
  disabledCount = 0;
  
  
  for ( int i = 0 ; i< PluginFunctions::viewers(); ++i ) {
    if ( PluginFunctions::viewerProperties(i).multisampling() )
      enabledCount++;
    else
      disabledCount++;
  }

  if ( enabledCount != 0 && disabledCount != 0 ) {
    globalMultisamplingAction_->setChecked(Qt::PartiallyChecked);
    globalMultisamplingAction_->setToolTip(tr("Disable Multisampling for all viewers"));  
    globalMultisamplingAction_->setStatusTip(tr("Disable Multisampling for all viewers"));  
    globalMultisamplingAction_->setText(tr("Disable Multisampling"));
  } else if ( enabledCount == 4 ) {
    globalMultisamplingAction_->setChecked( Qt::Checked );
    globalMultisamplingAction_->setToolTip(tr("Disable Multisampling for all viewers"));      
    globalMultisamplingAction_->setStatusTip(tr("Disable Multisampling for all viewers"));      
    globalMultisamplingAction_->setText(tr("Disable Multisampling"));
  } else {
    globalMultisamplingAction_->setChecked( Qt::Unchecked );
    globalMultisamplingAction_->setToolTip(tr("Enable Multisampling for all viewers"));  
    globalMultisamplingAction_->setStatusTip(tr("Enable Multisampling for all viewers"));  
    globalMultisamplingAction_->setText(tr("Enable Multisampling"));
  }
  
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
    globalDrawMenu_  = new QMenu(tr("Set Global DrawMode"));
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
