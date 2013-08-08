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

#include "CoreWidget.hh"
#include <OpenFlipper/common/GlobalOptions.hh>
#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include <OpenFlipper/BasePlugin/MenuInterface.hh>
#include <OpenFlipper/common/RendererInfo.hh>

//== IMPLEMENTATION ==========================================================



void CoreWidget::slotAddMenubarAction( QAction* _action , QString _name ) {

  if (!menus_.contains(_name))
    return;

  if (_name == FILEMENU) {
    fileMenu_->insertSeparator(fileMenuEnd_);
    fileMenu_->insertAction(fileMenuEnd_, _action);
  } else if (_name == ALGORITHMMENU) {

    // We insert the algorithms menu if it is not available yet
    if ( menuBar()->actions().contains(helpMenu_->menuAction()) )
      menuBar()->insertMenu(helpMenu_->menuAction(), algorithmMenu_);

    menus_[_name]->addAction(_action);
  } else {
    menus_[_name]->addAction(_action);
  }

}

void CoreWidget::slotAddMenubarActions( std::vector<QAction*> _actions , QString _name ) {

  if (!menus_.contains(_name))
    return;

  if (_name == FILEMENU) {
    fileMenu_->insertSeparator(fileMenuEnd_);
    for (std::vector<QAction*>::iterator it = _actions.begin(); it != _actions.end(); ++it )
        fileMenu_->insertAction(fileMenuEnd_, *it);
  } else if (_name == ALGORITHMMENU) {

    // We insert the algorithms menu if it is not available yet
    if ( menuBar()->actions().contains(helpMenu_->menuAction()) )
      menuBar()->insertMenu(helpMenu_->menuAction(), algorithmMenu_);

    for (std::vector<QAction*>::iterator it = _actions.begin(); it != _actions.end(); ++it )
        menus_[_name]->addAction(*it);
  } else {
      for (std::vector<QAction*>::iterator it = _actions.begin(); it != _actions.end(); ++it )
          menus_[_name]->addAction(*it);
  }

}

//=============================================================================

void CoreWidget::slotGetMenubarMenu (QString _name, QMenu *& _menu, bool _create)
{
	//if menu already exists, return it
  if (menus_.contains (_name))
    _menu = menus_[_name];
  //otherwise create a new one
  else if (_create)
  {
    _menu = new QMenu(_name);
    menus_[_name] = _menu;
    //we have to install an event filter to get event information (e.g. what this)
    _menu->installEventFilter(this);
    //guarantee that helpMenu_ is always at the end of all menus
    menuBar()->insertAction(helpMenu_->menuAction() ,_menu->menuAction ());
  }
  //otherwise no menu was found
  else
    _menu = NULL;
}


//=============================================================================

bool CoreWidget::eventFilter(QObject *_obj, QEvent *_event)
{
	//WhatsThisClicked event for hyperlinks in 'whats this' boxes
		if( _event->type() == QEvent::WhatsThisClicked )
		{
			QWhatsThisClickedEvent *wtcEvent = static_cast<QWhatsThisClickedEvent*>(_event);
			QWhatsThis::hideText();
			this->showHelpBrowser(wtcEvent->href());
			return true;
		}

		return _obj->event(_event);
}

//=============================================================================

void CoreWidget::showReducedMenuBar(bool reduced) {
    for (std::vector<QAction*>::iterator it = extended_actions.begin(); it != extended_actions.end(); ++it) {
        (*it)->setVisible(!reduced);
    }
}

void CoreWidget::setupMenuBar()
{

  // ======================================================================
  // File Menu
  // ======================================================================
  fileMenu_ = new QMenu( FILEMENU );
  menuBar()->addMenu( fileMenu_ );
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
  extended_actions.push_back(AC_AddEmpty);
  extended_actions.push_back(fileMenu_->addSeparator());

  //Save object
  QAction* AC_Save = new QAction(tr("Save Objects"), this);
//   AC_Save->setShortcut (Qt::CTRL + Qt::Key_S);
  AC_Save->setStatusTip(tr("Save current objects"));
  AC_Save->setWhatsThis(tr("Save current objects"));
  AC_Save->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"document-save.png"));
  connect(AC_Save, SIGNAL(triggered()), this, SIGNAL(saveMenu()));
  fileMenu_->addAction(AC_Save);
  extended_actions.push_back(AC_Save);

  //Save object to
  QAction* AC_Save_to = new QAction(tr("Save Objects to"), this);
  AC_Save_to->setStatusTip(tr("Save current Object(s) to"));
  AC_Save_to->setWhatsThis(tr("Save current Object(s) under a new name"));
  AC_Save_to->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"document-save-as.png"));
  connect(AC_Save_to, SIGNAL(triggered()), this, SIGNAL(saveToMenu()));
  fileMenu_->addAction(AC_Save_to);

  extended_actions.push_back(fileMenu_->addSeparator());

  //Load ini
  QAction* AC_load_ini = new QAction(tr("Load Settings"), this);
  AC_load_ini->setStatusTip(tr("Load Settings from INI file"));
  AC_load_ini->setWhatsThis(tr("Load a previous settings from file (objects,colors,...)"));
  AC_load_ini->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"load-settings.png"));
  connect(AC_load_ini, SIGNAL(triggered()), this, SIGNAL(loadIniMenu()));
  fileMenu_->addAction(AC_load_ini);
  extended_actions.push_back(AC_load_ini);

  //Save ini
  QAction* AC_save_ini = new QAction(tr("Save Settings"), this);
  AC_save_ini->setStatusTip(tr("Save current settings to INI file"));
  AC_save_ini->setWhatsThis(tr("Save settings to file (objects,colors,...)"));
  AC_save_ini->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"save-settings.png"));
  connect(AC_save_ini, SIGNAL(triggered()), this, SIGNAL(saveIniMenu()));
  fileMenu_->addAction(AC_save_ini);
  extended_actions.push_back(AC_save_ini);

  extended_actions.push_back(fileMenu_->addSeparator());

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
  extended_actions.push_back(
          viewMenu_->addMenu(globalDrawMenu_));

  //============================================================================================================
  // Rendering options Menu
  //============================================================================================================

  QMenu* renderingOptionsMenu = new QMenu(tr("Global Rendering Options"),viewMenu_);
  renderingOptionsMenu->setIcon( QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"core_renderingOptions.png") );
  extended_actions.push_back(
          viewMenu_->addMenu(renderingOptionsMenu));

  orthogonalProjectionAction_ = new QAction( tr("Switch Viewers to Orthogonal Projection"), renderingOptionsMenu );;
  orthogonalProjectionAction_->setIcon( QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"orthogonal.png") );
  orthogonalProjectionAction_->setCheckable( false );
  orthogonalProjectionAction_->setToolTip(   tr("Switch to orthogonal projection mode."));
  orthogonalProjectionAction_->setWhatsThis( tr("Switch projection mode<br><br>"
      "Switch to <b>orthogonal</b> projection mode."));
  connect( orthogonalProjectionAction_,SIGNAL( triggered() ), this, SLOT( slotGlobalOrthographicProjection() ) );
  renderingOptionsMenu->addAction( orthogonalProjectionAction_);

  perspectiveProjectionAction_ = new QAction( tr("Switch Viewers to Perspective Projection"), viewMenu_ );;
  perspectiveProjectionAction_->setIcon( QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"perspective.png") );
  perspectiveProjectionAction_->setCheckable( false );
  perspectiveProjectionAction_->setStatusTip( tr("Switch to perspective projection mode."));
  perspectiveProjectionAction_->setWhatsThis( tr("Switch projection mode<br><br>"
                                                 "Switch to <b>perspective</b> projection mode."));
  connect( perspectiveProjectionAction_,SIGNAL( triggered() ), this, SLOT( slotGlobalPerspectiveProjection() ) );
  renderingOptionsMenu->addAction( perspectiveProjectionAction_);

  // =====================

  globalAnimationAction_ = renderingOptionsMenu->addAction(tr("Global Animation"));
  globalAnimationAction_->setIcon( QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"animation.png") );
  connect(globalAnimationAction_, SIGNAL(triggered(bool)), this , SLOT( slotGlobalToggleAnimation() ) );

  //======================

  globalBackfaceCullingAction_ = renderingOptionsMenu->addAction(tr("Global Backface Culling"));
  globalBackfaceCullingAction_->setIcon( QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"backFaceCulling.png") );
  connect(globalBackfaceCullingAction_, SIGNAL(triggered(bool)), this , SLOT( slotGlobalToggleBackFaceCulling() ) );

  //======================

  globalTwosidedLightingAction_ = renderingOptionsMenu->addAction(tr("Global Two-sided Lighting"));
  globalTwosidedLightingAction_->setIcon( QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"twosidedLighting.png") );
  connect(globalTwosidedLightingAction_, SIGNAL(triggered(bool)), this , SLOT( slotGlobalToggleTwoSidedLighting() ) );

  //======================

  globalMultisamplingAction_ = renderingOptionsMenu->addAction(tr("Global Multisampling"));
  globalMultisamplingAction_->setIcon( QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"multisampling.png") );
  connect(globalMultisamplingAction_, SIGNAL(triggered(bool)), this , SLOT( slotGlobalToggleMultisampling()) );
  //======================

  globalMipmappingAction_ = renderingOptionsMenu->addAction(tr("Global Mipmapping"));
  globalMipmappingAction_->setIcon( QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"mipmapping.png") );
  connect(globalMipmappingAction_, SIGNAL(triggered(bool)), this , SLOT( slotGlobalToggleMipmapping()) );
  

  //============================================================================================================
  // Global renderer menu
  //============================================================================================================

  slotUpdateRendererMenu();

  //============================================================================================================
  // Other toplevel actions
  //============================================================================================================

  viewMenu_->addSeparator();

  //============================================================================================================
  // Post processor Manager
  //============================================================================================================

  QString iconPath = OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator();
  QAction* showPostProcessorDialog = new QAction(tr("Show post processor manager"),this);
  showPostProcessorDialog->setIcon(QIcon(iconPath+"postprocessors.png"));
  connect(showPostProcessorDialog,SIGNAL(triggered()),this,SLOT(slotShowPostProcessorManager()));
  viewMenu_->addAction(showPostProcessorDialog);

  viewMenu_->addSeparator();

  //====================================================================================================


  QAction* navigationSwitchAction = new QAction( tr("First-person Navigation"), viewMenu_ );
  navigationSwitchAction->setCheckable( true );
  navigationSwitchAction->setIcon( QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"core_firstPersonMode.png") );
  navigationSwitchAction->setStatusTip( tr("Switch between normal and first-person navigation mode."));
  navigationSwitchAction->setWhatsThis( tr("Switch between normal and first-person navigation mode."));
  navigationSwitchAction->setChecked( false );

  connect( navigationSwitchAction, SIGNAL( toggled(bool) ), this, SLOT( slotSwitchNavigation(bool) ) );
  viewMenu_->addAction( navigationSwitchAction);
  extended_actions.push_back(navigationSwitchAction);

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

  viewMenu_->addSeparator();

  QAction* wheelSwitchAction = new QAction( tr("Show / hide wheels"), viewMenu_ );
  wheelSwitchAction->setIcon( QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"core_wheels.png") );
  wheelSwitchAction->setCheckable( true );
  wheelSwitchAction->setStatusTip( tr("Show / hide navigation wheels in viewer widget."));
  wheelSwitchAction->setWhatsThis( tr("Show / hide navigation wheels in viewer widget.<br><br>"
                                      " These wheels appear in the corners of the viewports. "
                                      " Use wheels to rotate and scale scene."));
                                      
  if(OpenFlipperSettings().value("Core/Gui/glViewer/showControlWheels").toBool() ) 
    wheelSwitchAction->setChecked(true);

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
  extended_actions.push_back(sceneGraphAction);

  toolsMenu_->addSeparator();

  QAction* startVideoCaptureAction =  new QAction( tr("Start Video Capture ") ,toolsMenu_ );
  startVideoCaptureAction->setIcon( QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"video-start.png") );
  startVideoCaptureAction->setCheckable( false );
  startVideoCaptureAction->setToolTip( tr("Start video capturing.") );
  startVideoCaptureAction->setWhatsThis( tr("Start to capture a video sequence of the user actions")) ;
  toolsMenu_->addAction( startVideoCaptureAction );
  connect(startVideoCaptureAction, SIGNAL(triggered()), this, SLOT(startVideoCaptureDialog()) );

  QAction* stopVideoCaptureAction =  new QAction( tr("Stop Video Capture ") ,toolsMenu_ );
  stopVideoCaptureAction->setIcon( QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"video-stop.png") );
  stopVideoCaptureAction->setCheckable( false );
  stopVideoCaptureAction->setToolTip( tr("Stop video capturing." ));
  stopVideoCaptureAction->setWhatsThis( tr("Stop Video capturing" ));

  toolsMenu_->addAction( stopVideoCaptureAction);
  connect(stopVideoCaptureAction, SIGNAL(triggered()), this, SIGNAL(stopVideoCapture()) );
  
  extended_actions.push_back(startVideoCaptureAction);
  extended_actions.push_back(stopVideoCaptureAction);

  toolsMenu_->addSeparator();
  
  //show plugins
  QAction* AC_Plugins = new QAction(tr("Plugins"), this);
  AC_Plugins->setStatusTip(tr("Show loaded plugins"));
  AC_Plugins->setWhatsThis(tr("Show loaded plugins"));
  AC_Plugins->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"network-connect.png"));
  connect(AC_Plugins, SIGNAL(triggered()), this, SIGNAL(showPlugins()));
  toolsMenu_->addAction(AC_Plugins);
  
  // ======================================================================
  // Window Menu
  // ======================================================================
  windowMenu_ = new QMenu(tr("Windows"));
  menuBar()->addMenu(windowMenu_);
  menus_[tr("Windows")] = windowMenu_;
  
  
  // Show or Hide the View Mode Controls
  AC_ShowViewModeControls_ = new QAction(tr("Show View Mode Controls"), this);
  AC_ShowViewModeControls_->setStatusTip(tr("Show or Hide View Mode Control Widget"));
  //   AC_HelpBrowser->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"help-browser.png"));
  AC_ShowViewModeControls_->setWhatsThis(tr("Show or Hide View Mode Control Widget"));
  AC_ShowViewModeControls_->setCheckable(true);
  AC_ShowViewModeControls_->setChecked( ! OpenFlipperSettings().value("Core/Gui/TaskSwitcher/Hide",false).toBool()  );
  connect(AC_ShowViewModeControls_, SIGNAL(toggled( bool )), this, SLOT(showViewModeControls(bool)));
  windowMenu_->addAction(AC_ShowViewModeControls_);
  extended_actions.push_back(AC_ShowViewModeControls_);
  
  // Show or Hide the View Mode Controls
  QAction* AC_ShowToolbox = new QAction(tr("Show Toolboxes"), this);
  AC_ShowToolbox->setStatusTip(tr("Show or Hide the Toolbox Widget"));
  //   AC_HelpBrowser->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"help-browser.png"));
  AC_ShowToolbox->setWhatsThis(tr("Show or Hide the Toolbox Widget"));
  AC_ShowToolbox->setCheckable(true);
  AC_ShowToolbox->setChecked( ! OpenFlipperSettings().value("Core/Gui/ToolBoxes/hidden",false).toBool()  );
  connect(AC_ShowToolbox, SIGNAL( triggered()), this, SLOT(toggleToolbox()));
  connect(this,SIGNAL(toolBoxVisChanged(bool)),AC_ShowToolbox,SLOT(setChecked(bool)));
  windowMenu_->addAction(AC_ShowToolbox);

  // Show or Hide the Status bar
  QAction* AC_ShowStatusBar = new QAction(tr("Show Statusbar"), this);
  AC_ShowStatusBar->setStatusTip(tr("Show or Hide the Statusbar"));
  AC_ShowStatusBar->setWhatsThis(tr("Show or Hide the Statusbar"));
  AC_ShowStatusBar->setCheckable(true);
  AC_ShowStatusBar->setChecked( !OpenFlipperSettings().value("Core/Gui/StatusBar/hidden",false).toBool());
  connect(AC_ShowStatusBar,SIGNAL(triggered()),this,SLOT(toggleStatusBar()));
  connect(this,SIGNAL(statusBarVisChanged(bool)),AC_ShowStatusBar,SLOT(setChecked(bool)));
  windowMenu_->addAction(AC_ShowStatusBar);

  // Show or Hide the Menu bar
  QAction* AC_ShowMenuBar = new QAction(tr("Show Menubar"), this);
  AC_ShowMenuBar->setStatusTip(tr("Show or Hide the Menubar"));
  AC_ShowMenuBar->setWhatsThis(tr("Show or Hide the Menubar"));
  AC_ShowMenuBar->setCheckable(true);
  AC_ShowMenuBar->setChecked( !OpenFlipperSettings().value("Core/Gui/MenuBar/hidden",false).toBool());
  connect(AC_ShowMenuBar,SIGNAL(triggered()),this,SLOT(toggleMenuBar()));
  connect(this,SIGNAL(menuBarVisChanged(bool)),AC_ShowMenuBar,SLOT(setChecked(bool)));
  windowMenu_->addAction(AC_ShowMenuBar);
  extended_actions.push_back(AC_ShowMenuBar);

  // Show or Hide the Tool bar
  QAction* AC_ShowToolBar = new QAction(tr("Show Toolbar"), this);
  AC_ShowToolBar->setStatusTip(tr("Show or Hide the Toolbar"));
  AC_ShowToolBar->setWhatsThis(tr("Show or Hide the Toolbar"));
  AC_ShowToolBar->setCheckable(true);
  AC_ShowToolBar->setChecked( !OpenFlipperSettings().value("Core/Gui/ToolBar/hidden",false).toBool());
  connect(AC_ShowToolBar,SIGNAL(triggered()),this,SLOT(toggleToolBar()));
  connect(this,SIGNAL(toolBarVisChanged(bool)),AC_ShowToolBar,SLOT(setChecked(bool)));
  windowMenu_->addAction(AC_ShowToolBar);

  // Enable or Disable Fullscreen Mode
  QAction* AC_Fullscreen = new QAction(tr("Fullscreen"), this);
  AC_Fullscreen->setStatusTip(tr("Enable or Disable the Fullscreen"));
  AC_Fullscreen->setWhatsThis(tr("Enable or Disable the Fullscreen"));
  AC_Fullscreen->setCheckable(true);
  AC_Fullscreen->setChecked( OpenFlipperSettings().value("Core/Gui/fullscreen", false ).toBool() );
  connect(AC_Fullscreen,SIGNAL(triggered()),this,SLOT(toggleFullscreen()));
  connect(this,SIGNAL(fullScreenChanged(bool)),AC_Fullscreen,SLOT(setChecked(bool)));
  windowMenu_->addAction(AC_Fullscreen);

  // ======================================================================
  // Algorithms Menu
  // ======================================================================
  algorithmMenu_ = new QMenu( ALGORITHMMENU );
  menus_[tr("Algorithms")] = algorithmMenu_;

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


  // install event filters for what is this event
  // todo: why doesn't go any event through CoreWidget::event from menus? i don't get it
  fileMenu_->installEventFilter(this);
  viewMenu_->installEventFilter(this);
  toolsMenu_->installEventFilter(this);
  windowMenu_->installEventFilter(this);
  algorithmMenu_->installEventFilter(this);
  helpMenu_->installEventFilter(this);
}


void CoreWidget::slotViewMenuAboutToShow() {

  uint enabledCount  = 0;

  for ( int i = 0 ; i< PluginFunctions::viewers(); ++i ) {
    if ( PluginFunctions::viewerProperties(i).animation() )
      enabledCount++;
  }

  if ( enabledCount != 0 ) {
    globalAnimationAction_->setToolTip(tr("Disable animation for all viewers"));
    globalAnimationAction_->setStatusTip(tr("Disable animation for all viewers"));
    globalAnimationAction_->setText(tr("Disable animation"));
  } else {
    globalAnimationAction_->setToolTip(tr("Enable animation for all viewers"));
    globalAnimationAction_->setStatusTip(tr("Enable animation for all viewers"));
    globalAnimationAction_->setText(tr("Enable animation"));
  }

  //=============================================================================================================================

  enabledCount  = 0;

  for ( int i = 0 ; i< PluginFunctions::viewers(); ++i ) {
    if ( PluginFunctions::viewerProperties(i).backFaceCulling() )
      enabledCount++;
  }

  if ( enabledCount != 0 ) {
    globalBackfaceCullingAction_->setToolTip(tr("Disable backface culling for all viewers"));
    globalBackfaceCullingAction_->setStatusTip(tr("Disable backface culling for all viewers"));
    globalBackfaceCullingAction_->setText(tr("Disable backface culling"));
  } else {
    globalBackfaceCullingAction_->setToolTip(tr("Enable backface culling for all viewers"));
    globalBackfaceCullingAction_->setStatusTip(tr("Enable backface culling for all viewers"));
    globalBackfaceCullingAction_->setText(tr("Enable backface culling"));
  }

  //=============================================================================================================================

  enabledCount  = 0;

  for ( int i = 0 ; i< PluginFunctions::viewers(); ++i ) {
    if ( PluginFunctions::viewerProperties(i).twoSidedLighting() )
      enabledCount++;
  }

  if ( enabledCount != 0 ) {
    globalTwosidedLightingAction_->setToolTip(tr("Disable two-sided lighting for all viewers"));
    globalTwosidedLightingAction_->setStatusTip(tr("Disable two-sided lighting for all viewers"));
    globalTwosidedLightingAction_->setText(tr("Disable two-sided lighting"));
  } else {
    globalTwosidedLightingAction_->setToolTip(tr("Enable two-sided lighting for all viewers"));
    globalTwosidedLightingAction_->setStatusTip(tr("Enable two-sided lighting for all viewers"));
    globalTwosidedLightingAction_->setText(tr("Enable two-sided lighting"));
  }

  //=============================================================================================================================

  enabledCount  = 0;

  for ( int i = 0 ; i< PluginFunctions::viewers(); ++i ) {
    if ( PluginFunctions::viewerProperties(i).multisampling() )
      enabledCount++;
  }

  if ( enabledCount != 0 ) {
    globalMultisamplingAction_->setToolTip(tr("Disable multisampling for all viewers"));
    globalMultisamplingAction_->setStatusTip(tr("Disable multisampling for all viewers"));
    globalMultisamplingAction_->setText(tr("Disable multisampling"));
  } else {
    globalMultisamplingAction_->setToolTip(tr("Enable multisampling for all viewers"));
    globalMultisamplingAction_->setStatusTip(tr("Enable multisampling for all viewers"));
    globalMultisamplingAction_->setText(tr("Enable multisampling"));
  }
  
  //=============================================================================================================================

  enabledCount  = 0;

  for ( int i = 0 ; i< PluginFunctions::viewers(); ++i ) {
    if ( PluginFunctions::viewerProperties(i).mipmapping() )
      enabledCount++;
  }

  if ( enabledCount != 0 ) {
    globalMipmappingAction_->setToolTip(tr("Disable mipmapping for all viewers"));
    globalMipmappingAction_->setStatusTip(tr("Disable mipmapping for all viewers"));
    globalMipmappingAction_->setText(tr("Disable mipmapping"));
  } else {
    globalMipmappingAction_->setToolTip(tr("Enable mipmapping for all viewers"));
    globalMipmappingAction_->setStatusTip(tr("Enable mipmapping for all viewers"));
    globalMipmappingAction_->setText(tr("Enable mipmapping"));
  }

  //=============================================================================================================================
    
  int perspectiveCount = 0;
  int orthogonalCount = 0;

  for ( int i = 0 ; i < PluginFunctions::viewers() ; ++i ) {
    if ( examiner_widgets_[ i ]->projectionMode() == glViewer::PERSPECTIVE_PROJECTION )
      perspectiveCount++;
    else
      orthogonalCount++;
  }
  
  if ( perspectiveCount == PluginFunctions::viewers() )
    perspectiveProjectionAction_->setVisible(false);
  else
    perspectiveProjectionAction_->setVisible(true);

  if ( orthogonalCount == PluginFunctions::viewers() )
    orthogonalProjectionAction_->setVisible(false);
  else
    orthogonalProjectionAction_->setVisible(true);

}

void CoreWidget::slotUpdateRendererMenu() {

  // Add the menu if it does not exist yet
  if ( rendererMenu_ == 0 ) {

    QString iconPath = OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator();

    rendererMenu_ = new QMenu(tr("Global Renderer"),viewMenu_);
    rendererMenu_->setIcon(QIcon(iconPath+"renderers.png"));
    extended_actions.push_back(viewMenu_->addMenu(rendererMenu_));


    connect(rendererMenu_,SIGNAL(aboutToShow () ) , this, SLOT(slotUpdateRendererMenu() ) );
  }

  // delete the old renerer group if it exists
  if ( rendererGroup_ ) {

    disconnect( rendererGroup_ , SIGNAL( triggered( QAction * ) ),
        this           , SLOT( slotGlobalRendererMenu( QAction * ) ) );

    delete( rendererGroup_ );
    rendererGroup_ = 0;

  }

  // Recreate actionGroup
  rendererGroup_ = new QActionGroup( this );
  rendererGroup_->setExclusive( true );

//  // Add the options for all active renderers
//  for ( int i = 0 ; i < PluginFunctions::viewers() ; ++i) {
//
//    //Get the options action for the currently active renderer
//    if( renderManager()[ renderManager().activeId( i )]->optionsAction != 0 ) {
//      rendererMenu_->addAction(renderManager()[ renderManager().activeId(i) ]->optionsAction );
//    }
//  }

//  rendererMenu_->addSeparator();

  // Add the renderers
  for ( unsigned int i = 0 ; i < renderManager().available() ; ++i) {

    // Add a new Action with the renderer name
    QAction * action = new QAction( renderManager()[i]->name, rendererGroup_ );
    action->setCheckable( true );

    // Check if this processor is currently active
    if ( renderManager().activeId(PluginFunctions::activeExaminer() ) == i )
      action->setChecked(true);

    // Remember the id for the processor
    action->setData(QVariant(i));
  }

  // Remove old data
  rendererMenu_->clear();

  // Add all new actions from the group to the menu
  rendererMenu_->addActions( rendererGroup_->actions() );

  // Connect signal of group to our managing slot
  connect( rendererGroup_ , SIGNAL( triggered( QAction * ) ),
      this          , SLOT( slotGlobalRendererMenu( QAction * ) ) );


}

void CoreWidget::slotUpdatePostProcessorMenu() {


  // Add the menu if it does not exist yet
  if ( postprocessorMenu_ == 0 ) {

    QString iconPath = OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator();

    postprocessorMenu_ = new QMenu(tr("Global Post Processor"),viewMenu_);
    postprocessorMenu_->setIcon(QIcon(iconPath+"postprocessors.png"));
    extended_actions.push_back(viewMenu_->addMenu(postprocessorMenu_));

    connect(postprocessorMenu_,SIGNAL(aboutToShow () ) , this, SLOT(slotUpdatePostProcessorMenu() ) );
  }

  // delete the old post processor group if it exists
  if ( postProcessorGroup_ ) {

    disconnect( postProcessorGroup_ , SIGNAL( triggered( QAction * ) ),
        this           , SLOT( slotGlobalPostProcessorMenu( QAction * ) ) );

    delete( postProcessorGroup_ );
    postProcessorGroup_ = 0;

  }

  // Recreate actionGroup
  postProcessorGroup_ = new QActionGroup( this );
  postProcessorGroup_->setExclusive( true );

//  // Get the options action for the currently active postprocessor
//  for ( int i = 0 ; i < PluginFunctions::viewers() ; ++i)
//    if( postProcessorManager()[ postProcessorManager().activeId( i )]->optionsAction != 0 ) {
//      postprocessorMenu_->addAction(postProcessorManager()[ postProcessorManager().activeId( i ) ]->optionsAction );
//    }

//  postprocessorMenu_->addSeparator();


  // Now add the post processors
  for ( unsigned int i = 0 ; i < postProcessorManager().available() ; ++i) {

    // Add a new Action with the postprocessors name
    QAction * action = new QAction( postProcessorManager()[i]->name, postProcessorGroup_ );
    action->setCheckable( true );

    // Check if this processor is currently active
    if ( postProcessorManager().activeId(PluginFunctions::activeExaminer() ) == i )
      action->setChecked(true);

    // Remember the id for the processor
    action->setData(QVariant(i));
  }

  // Remove old data
  postprocessorMenu_->clear();

  // Add all new actions from the group to the menu
  postprocessorMenu_->addActions( postProcessorGroup_->actions() );

  // Connect signal of group to our managing slot
  connect( postProcessorGroup_ , SIGNAL( triggered( QAction * ) ),
      this          , SLOT( slotGlobalPostProcessorMenu( QAction * ) ) );

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
  // Single pass action, draw modes independent from multipass rendering
  ACG::SceneGraph::CollectDrawModesAction actionAvailable;
  ACG::SceneGraph::traverse( PluginFunctions::getRootNode() , actionAvailable);
  availableGlobalDrawModes_ = actionAvailable.drawModes();

  // Get currently active drawModes
  activeDrawModes_ = PluginFunctions::drawMode(0);
  for ( int i = 1 ; i < PluginFunctions::viewers(); ++i )
    activeDrawModes_ &= PluginFunctions::drawMode(i);

  // Convert to ids
  std::vector< ACG::SceneGraph::DrawModes::DrawMode > availDrawModeIds;
  availDrawModeIds = availableGlobalDrawModes_.getAtomicDrawModes() ;

  globalDrawMenu_->clear();

  for ( unsigned int i = 0; i < availDrawModeIds.size(); ++i )
  {
    ACG::SceneGraph::DrawModes::DrawMode id    = availDrawModeIds[i];
    std::string  descr =  id.description();

    QAction * action = new QAction( descr.c_str(), drawGroup_ );
    action->setCheckable( true );
    action->setChecked( activeDrawModes_.containsAtomicDrawMode(id) );
  }

  globalDrawMenu_->addActions( drawGroup_->actions() );

}

void CoreWidget::slotGlobalRendererMenu(QAction * _action) {

  unsigned int mode = _action->data().toUInt();

  // Set renderer for all viewers
  for ( int i = 0 ; i < PluginFunctions::viewers() ; ++i)
    renderManager().setActive(mode,i);

}

void CoreWidget::slotGlobalPostProcessorMenu(QAction * _action) {

  unsigned int mode = _action->data().toUInt();

  // Set postprocessor for all viewers
  for ( int i = 0 ; i < PluginFunctions::viewers() ; ++i)
    postProcessorManager().setActive(mode,i);

}

void CoreWidget::slotGlobalDrawMenu(QAction * _action) {

  //======================================================================================
  // Get the mode toggled
  //======================================================================================
  ACG::SceneGraph::DrawModes::DrawMode mode(ACG::SceneGraph::DrawModes::NONE);
  std::vector< ACG::SceneGraph::DrawModes::DrawMode > availDrawModeIds;
  availDrawModeIds = availableGlobalDrawModes_.getAtomicDrawModes();
  for ( unsigned int i = 0; i < availDrawModeIds.size(); ++i )
  {
    QString descr = QString( ( availDrawModeIds[i].description() ).c_str() );

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
