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



void CoreWidget::slotAddMenu( QMenu* _menu , MenuType _type ) {

  switch (_type) {
    case TOPLEVELMENU :
      // Add it to the menubar as a top level Menu
      menuBar()->insertMenu(helpMenu_->menuAction() ,_menu);
      break;
    case FILEMENU :
      fileMenu_->insertSeparator(fileMenuEnd_);
      fileMenu_->insertMenu( fileMenuEnd_ , _menu );
      break;
    case VIEWMENU :
      viewMenu_->addMenu( _menu );
      viewMenu_->addSeparator( );
      break;

  }

}


//=============================================================================


void CoreWidget::setupMenuBar()
{
  // ======================================================================
  // File Menu
  // ======================================================================
  fileMenu_ = new QMenu(tr("&File"));
  menuBar()->addMenu(fileMenu_ );

  viewMenu_ = new QMenu(tr("&View"));
  menuBar()->addMenu(viewMenu_ );
  
  QIcon icon;
  QAction* setGlobalBackgroundColor = new QAction(tr("&Background Color"), this);;
  setGlobalBackgroundColor->setToolTip(tr("Set Background Color for all viewers"));
  setGlobalBackgroundColor->setStatusTip(tr("Set Background Color for all viewers"));
  setGlobalBackgroundColor->setWhatsThis("Set Background Color for all viewers");
  icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"bgColor.png");
  setGlobalBackgroundColor->setIcon(icon);
  connect(setGlobalBackgroundColor, SIGNAL(triggered()), this, SLOT(slotSetGlobalBackgroundColor()));
  viewMenu_->addAction(setGlobalBackgroundColor);
  
  slotUpdateGlobalDrawMenu();
  
  //Clear all
  QAction* AC_clear_all = new QAction(tr("&Clear All"), this);;
  AC_clear_all->setStatusTip(tr("Clear all Objects"));
  AC_clear_all->setWhatsThis("Close all open Objects");
  icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"edit-clear.png");
  AC_clear_all->setIcon(icon);
  connect(AC_clear_all, SIGNAL(triggered()), this, SIGNAL(clearAll()));
  fileMenu_->addAction(AC_clear_all);

  fileMenu_->addSeparator();

  //Load object
  QAction* AC_Load = new QAction(tr("&Load Object"), this);
//   AC_Load->setShortcut (Qt::CTRL + Qt::Key_O);
  AC_Load->setStatusTip(tr("Load an object"));
  AC_Load->setWhatsThis("Load a new object");
  icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"document-open.png");
  AC_Load->setIcon(icon);

  connect(AC_Load, SIGNAL(triggered()), this, SIGNAL(loadMenu()));
  fileMenu_->addAction(AC_Load);

  //Add empty object
  QAction* AC_AddEmpty = new QAction(tr("&Add Empty Object"), this);
  AC_AddEmpty->setStatusTip(tr("Add an empty object"));
  AC_AddEmpty->setWhatsThis("Creates a new empty object of a given type");
  icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"add-empty-object.png");
  AC_AddEmpty->setIcon(icon);
  connect(AC_AddEmpty, SIGNAL(triggered()), this, SIGNAL(addEmptyObjectMenu()));
  fileMenu_->addAction(AC_AddEmpty);

  fileMenu_->addSeparator();

  //Save object
  QAction* AC_Save = new QAction(tr("&Save Object"), this);
//   AC_Save->setShortcut (Qt::CTRL + Qt::Key_S);
  AC_Save->setStatusTip(tr("Save currently selected objects"));
  AC_Save->setWhatsThis("Saves all currently selected objects");
  icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"document-save.png");
  AC_Save->setIcon(icon);
  connect(AC_Save, SIGNAL(triggered()), this, SIGNAL(saveMenu()));
  fileMenu_->addAction(AC_Save);

  //Save object to
  QAction* AC_Save_to = new QAction(tr("&Save Object To"), this);
  AC_Save_to->setStatusTip(tr("Save current Object(s) To"));
  AC_Save_to->setWhatsThis("Saves all currently selected objects under a new name");
  icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"document-save-as.png");
  AC_Save_to->setIcon(icon);
  connect(AC_Save_to, SIGNAL(triggered()), this, SIGNAL(saveToMenu()));
  fileMenu_->addAction(AC_Save_to);

  fileMenu_->addSeparator();

  //Load ini
  QAction* AC_load_ini = new QAction(tr("&Load Settings"), this);
  AC_load_ini->setStatusTip(tr("Load Settings from INI file"));
  AC_load_ini->setWhatsThis("Load a previous settings from file (objects,colors,...)");
  icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"load-settings.png");
  AC_load_ini->setIcon(icon);
  connect(AC_load_ini, SIGNAL(triggered()), this, SIGNAL(loadIniMenu()));
  fileMenu_->addAction(AC_load_ini);

  //Save ini
  QAction* AC_save_ini = new QAction(tr("&Save Settings"), this);
  AC_save_ini->setStatusTip(tr("Save current settings to INI file"));
  AC_save_ini->setWhatsThis("Save settings to file (objects,colors,...)");
  icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"save-settings.png");
  AC_save_ini->setIcon(icon);
  connect(AC_save_ini, SIGNAL(triggered()), this, SIGNAL(saveIniMenu()));
  fileMenu_->addAction(AC_save_ini);

  fileMenu_->addSeparator();

  //Options
  QAction* AC_Options = new QAction(tr("&Options"), this);
  AC_Options->setStatusTip(tr("Edit OpenFlipper Options"));
  AC_Options->setWhatsThis("Edit OpenFlipper Options");
  icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"options.png");
  AC_Options->setIcon(icon);
  connect(AC_Options, SIGNAL(triggered()), this, SLOT(showOptionsWidget()));
  fileMenu_->addAction(AC_Options);

  //Remember entry of menu (required for adding File Menu entries from plugins)
  fileMenuEnd_ = fileMenu_->addSeparator();

  //Recent files
  recentFilesMenu_ = new QMenu(tr("&Recent Files"));
  icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"document-open-recent.png");
  recentFilesMenu_->setIcon(icon);
  recentFilesMenu_->setWhatsThis("Open recent files");
  connect(recentFilesMenu_,SIGNAL(triggered(QAction*)),this,SIGNAL(recentOpen(QAction*)));
  fileMenu_->addMenu(recentFilesMenu_);

  fileMenu_->addSeparator();

  //Main Application exit menu entry
  QAction* AC_exit = new QAction(tr("&Exit"), this);;
  AC_exit->setShortcut (Qt::CTRL + Qt::Key_Q);
  AC_exit->setStatusTip(tr("Exit Application"));
  recentFilesMenu_->setWhatsThis("Close OpenFlipper");
  icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"application-exit.png");
  AC_exit->setIcon(icon);
  connect(AC_exit, SIGNAL(triggered()), this, SIGNAL(exit()));
  fileMenu_->addAction(AC_exit);

  // ======================================================================
  // help Menu
  // ======================================================================
  helpMenu_ = new QMenu(tr("&Help"));
  menuBar()->addMenu(helpMenu_);

  //Open Help Browser
  QAction* AC_HelpBrowser = new QAction(tr("&User Help"), this);
  AC_HelpBrowser->setStatusTip(tr("Open Help Browser with User Documentation"));
  icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"help-browser.png");
  AC_HelpBrowser->setIcon(icon);
  AC_HelpBrowser->setWhatsThis("Open the <b>Help Browser</b>");
  connect(AC_HelpBrowser, SIGNAL(triggered()), this, SLOT(showHelpBrowserUser()));
  helpMenu_->addAction(AC_HelpBrowser);

  //Open Help Browser
  AC_HelpBrowser = new QAction(tr("&Developer Help"), this);
  AC_HelpBrowser->setStatusTip(tr("Open Help Browser with Developer Documentation"));
  icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"help-browser.png");
  AC_HelpBrowser->setIcon(icon);
  AC_HelpBrowser->setWhatsThis("Open the <b>Help Browser</b>");
  connect(AC_HelpBrowser, SIGNAL(triggered()), this, SLOT(showHelpBrowserDeveloper()));
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
  icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"network-connect.png");
  AC_Plugins->setIcon(icon);
  connect(AC_Plugins, SIGNAL(triggered()), this, SIGNAL(showPlugins()));
  helpMenu_->addAction(AC_Plugins);

  helpMenu_->addSeparator();

  //About Action
  QAction* AC_About = new QAction(tr("&About"), this);
  AC_About->setStatusTip(tr("About OpenFlipper"));
  icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"help-about.png");
  AC_About->setIcon(icon);
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
    
    viewMenu_->addMenu(globalDrawMenu_);
  }
  
  // Collect available draw Modes 
  ACG::SceneGraph::CollectDrawModesAction actionAvailable;
  ACG::SceneGraph::traverse( PluginFunctions::getRootNode() , actionAvailable);
  availableDrawModes_ = actionAvailable.drawModes();
  
  // Get currently active drawModes (first viewer only )
  // TODO: create combination from all viewers!
  activeDrawModes_ = INT_MAX;
  for ( int i = 0 ; i < PluginFunctions::viewers(); ++i )
    activeDrawModes_ &= PluginFunctions::drawMode(i);
  
  // Convert to ids
  std::vector< unsigned int > availDrawModeIds;
  availDrawModeIds = ACG::SceneGraph::DrawModes::getDrawModeIDs( availableDrawModes_ );
  
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
  availDrawModeIds = ACG::SceneGraph::DrawModes::getDrawModeIDs( availableDrawModes_ );
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
