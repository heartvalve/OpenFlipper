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
//  CLASS MViewWidget - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================

// -------------------- mview
#include "CoreWidget.hh"

// -------------------- OpenFlipper Includes
#include <OpenFlipper/common/GlobalOptions.hh>
#include <OpenFlipper/common/RecentFiles.hh>
#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include <OpenFlipper/BasePlugin/KeyInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>

#define WIDGET_HEIGHT 800
#define WIDGET_WIDTH  800

//== IMPLEMENTATION ==========================================================

/** \brief Constuctor for the Core Widget
 *
*/
CoreWidget::
CoreWidget( QVector<ViewMode*>& _viewModes,
            std::vector<PluginInfo>& _plugins,
            QList< SlotInfo >& _coreSlots ) :
  QMainWindow(),
  coreSlots_(_coreSlots),
  viewModes_(_viewModes),
  dockViewMode_(0),
  viewModeButton_(0),
  viewModeMenu_(0),
  viewGroup_(0),
  splitter_(0),
  logWidget_(0),
  recentFilesMenu_(0),
  pluginsMenu_(0),
  fileMenu_(0),
  viewMenu_(0),
  fileMenuEnd_(0),
  contextMenu_(0),
  contextSelectionMenu_(0),
  stackMenu_(0),
  stackedWidget_(0),
  helpBrowserDeveloper_(0),
  helpBrowserUser_(0),
  aboutWidget_(0),
  optionsWidget_(0),
  plugins_(_plugins)

{
  setupStatusBar();

  splitter_ = new QSplitter(Qt::Vertical,this);
  setCentralWidget(splitter_);
  stackedWidget_ = new QStackedWidget(splitter_);

  // ======================================================================
  // Set up the logging window
  // ======================================================================
  logWidget_ = new LoggerWidget(splitter_);
  logWidget_->setReadOnly(true);
  logWidget_->setSizePolicy( QSizePolicy ( QSizePolicy::Preferred , QSizePolicy::Preferred ) );
  logWidget_->resize( splitter_->width() ,120);
  logWidget_->setLineWrapMode( QTextEdit::NoWrap );

  originalLoggerSize_ = 0;

  QList<int> wsizes( splitter_->sizes() );
  if (OpenFlipper::Options::hideLogger()) {
    wsizes[1] = 0;
    splitter_->setSizes(wsizes);
  }

  // ======================================================================
  // Create examiner
  // ======================================================================
  QGLFormat format;
  QGLFormat::setDefaultFormat(format);
  format.setStereo( OpenFlipper::Options::stereo() );

  QWidget* tmp = 0;

  if ( !OpenFlipper::Options::multiView() ) {

    QtExaminerViewer* examinerWidget = new QtExaminerViewer(stackedWidget_,
                                                            "Examiner Widget",
                                                            statusBar_ ,
                                                            &format);

    examiner_widgets_.push_back(examinerWidget);

    examinerWidget->disableKeyHandling(true);
    examinerWidget->sceneGraph( PluginFunctions::getSceneGraphRootNode() );
    examinerWidget->enablePopupMenu(false);

    stackedWidget_->addWidget(examinerWidget);

  } else {

    // Create collector widget which holds all viewers
    tmp = new QWidget(stackedWidget_);

    // Create master examiner widget
    QtExaminerViewer* examinerWidget = new QtExaminerViewer(tmp,
                                                            "Examiner Widget",
                                                            statusBar_ ,
                                                            &format,
                                                            0,
                                                            QtExaminerViewer::Nothing);
    examiner_widgets_.push_back(examinerWidget);


    // Create all other examiners using the same gl context as the others
    for ( unsigned int i = 1 ; i < OpenFlipper::Options::examinerWidgets() ; ++i ) {
      QtExaminerViewer* newWidget = new QtExaminerViewer(tmp,
                                                         "Examiner Widget",
                                                         statusBar_ ,
                                                         &format,
                                                         examinerWidget,
                                                         QtExaminerViewer::Nothing);
      examiner_widgets_.push_back(newWidget);
    }

    // Initialize all examiners
    for ( unsigned int i = 0 ; i < OpenFlipper::Options::examinerWidgets() ; ++i ) {
      examiner_widgets_[i]->sceneGraph( PluginFunctions::getSceneGraphRootNode() );
      examiner_widgets_[i]->enablePopupMenu(false);
      examiner_widgets_[i]->disableKeyHandling(true);
    }

    QGridLayout* layout = new QGridLayout(tmp);
    layout->addWidget(examiner_widgets_[0],0,0);
    layout->addWidget(examiner_widgets_[1],0,1);
    layout->addWidget(examiner_widgets_[2],1,0);
    layout->addWidget(examiner_widgets_[3],1,1);

    for ( unsigned int i = 1 ; i < OpenFlipper::Options::examinerWidgets() ; ++i )
      examiner_widgets_[i]->hide();

    stackedWidget_->addWidget(tmp);
  }




  // ======================================================================
  // Setup dragging for examiner widget
  // ======================================================================
  for ( uint i = 0 ; i < OpenFlipper::Options::examinerWidgets() ; ++i ) {
    examiner_widgets_[i]->setExternalDrag(true);

    connect( examiner_widgets_[i], SIGNAL(startDragEvent( QMouseEvent*)),
             this, SLOT(startDrag(QMouseEvent* )));
    connect( examiner_widgets_[i], SIGNAL(dragEnterEvent( QDragEnterEvent*)),
            this, SLOT(dragEnterEvent(QDragEnterEvent* )));
    connect( examiner_widgets_[i], SIGNAL(dropEvent( QDropEvent*)),
            this, SLOT(dropEvent(QDropEvent* )));
  }


  // ======================================================================
  // Create main Toolbar
  // ======================================================================

  mainToolbar_ = new QToolBar("Main Toolbar");
  mainToolbar_->setWindowTitle("Main Toolbar");
  mainToolbar_->setObjectName("MainToolbar");
  slotAddToolbar(mainToolbar_);

  // ======================================================================
  // Get Toolbar from examiner and integrate it into main window
  // ======================================================================


  // Take control over the toolbar
  viewerToolbar_ = examiner_widgets_[0]->removeToolBar();

  for ( unsigned int i = 1 ; i < OpenFlipper::Options::examinerWidgets() ; ++i ) {
    examiner_widgets_[i]->removeToolBar();
  }

  addToolBar(Qt::TopToolBarArea,viewerToolbar_);
  viewerToolbar_->setParent(this);
  viewerToolbar_->setAllowedAreas(Qt::AllToolBarAreas);
  viewerToolbar_->setIconSize(QSize(20,20));
  viewerToolbar_->setObjectName("ViewerToolbar");

  // Set our own Icons and connect to additional examiner widgets
  QList<QAction *> actions = viewerToolbar_->actions();
  for ( int i = 0 ; i < actions.size(); ++i ) {
    if ( actions[i]->text() == "Move" ) {
      QIcon icon;
      icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"transform-move.png");

      QToolButton * button = qobject_cast<QToolButton *>( viewerToolbar_->widgetForAction ( actions[i] ) );
      button->setIcon(icon);

      // Connect action to all other widgets ( First widget provides the toolbar so dont connect twice!)
      for ( unsigned int i = 1 ; i < OpenFlipper::Options::examinerWidgets() ; ++i )
        connect( button,SIGNAL( clicked() ), examiner_widgets_[i], SLOT( examineMode() ) );

    } else if ( actions[i]->text() == "Home" ) {
      QIcon icon;
      icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"go-home.png");

      QToolButton * button = qobject_cast<QToolButton *>( viewerToolbar_->widgetForAction ( actions[i] ) );
      button->setIcon(icon);

      // Connect action to all other widgets ( First widget provides the toolbar so dont connect twice!)
      for ( unsigned int i = 1 ; i < OpenFlipper::Options::examinerWidgets() ; ++i )
        connect( button,SIGNAL( clicked() ), examiner_widgets_[i], SLOT( home() ) );

    } else if ( actions[i]->text() == "Set Home" ) {
      QIcon icon;
      icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"set-home.png");

      QToolButton * button = qobject_cast<QToolButton *>( viewerToolbar_->widgetForAction ( actions[i] ) );
      button->setIcon(icon);

      // Connect action to all other widgets ( First widget provides the toolbar so dont connect twice!)
      for ( unsigned int i = 1 ; i < OpenFlipper::Options::examinerWidgets() ; ++i )
        connect( button,SIGNAL( clicked() ), examiner_widgets_[i], SLOT( setHome() ) );

    } else if ( actions[i]->text() == "Light" ) {
      QIcon icon;
      icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"light-mode.png");

      QToolButton * button = qobject_cast<QToolButton *>( viewerToolbar_->widgetForAction ( actions[i] ) );
      button->setIcon(icon);

      // Connect action to all other widgets ( First widget provides the toolbar so dont connect twice!)
      for ( unsigned int i = 1 ; i < OpenFlipper::Options::examinerWidgets() ; ++i )
        connect( button,SIGNAL( clicked() ), examiner_widgets_[i], SLOT( lightMode() ) );

    } else if ( actions[i]->text() == "Question" ) {
      QIcon icon;
      icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"help-about.png");

      QToolButton * button = qobject_cast<QToolButton *>( viewerToolbar_->widgetForAction ( actions[i] ) );
      button->setIcon(icon);

      // Connect action to all other widgets ( First widget provides the toolbar so dont connect twice!)
      for ( unsigned int i = 1 ; i < OpenFlipper::Options::examinerWidgets() ; ++i )
        connect( button,SIGNAL( clicked() ), examiner_widgets_[i], SLOT( questionMode() ) );

    } else if ( actions[i]->text() == "SceneGraph" ) {

      QIcon icon;
      icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"scenegraph.png");

      QToolButton * button = qobject_cast<QToolButton *>( viewerToolbar_->widgetForAction ( actions[i] ) );
      button->setIcon(icon);

      // Connect action to all other widgets ( First widget provides the toolbar so dont connect twice!)
      for ( unsigned int i = 1 ; i < OpenFlipper::Options::examinerWidgets() ; ++i )
        connect( button,SIGNAL( clicked() ), examiner_widgets_[i], SLOT( showSceneGraphDialog() ) );

    } else if ( actions[i]->text() == "Pick" ) {

      QToolButton * button = qobject_cast<QToolButton *>( viewerToolbar_->widgetForAction ( actions[i] ) );

      // Connect action to all other widgets ( First widget provides the toolbar so dont connect twice!)
      for ( unsigned int i = 1 ; i < OpenFlipper::Options::examinerWidgets() ; ++i )
        connect( button,SIGNAL( clicked() ), examiner_widgets_[i], SLOT( pickingMode() ) );

    } else if ( actions[i]->text() == "View All" ) {

      QToolButton * button = qobject_cast<QToolButton *>( viewerToolbar_->widgetForAction ( actions[i] ) );

      // Connect action to all other widgets ( First widget provides the toolbar so dont connect twice!)
      for ( unsigned int i = 1 ; i < OpenFlipper::Options::examinerWidgets() ; ++i )
        connect( button,SIGNAL( clicked() ), examiner_widgets_[i], SLOT( viewAll() ) );

    } else if ( actions[i]->text() == "Projection" ) {

      QToolButton * button = qobject_cast<QToolButton *>( viewerToolbar_->widgetForAction ( actions[i] ) );

      // Connect action to all other widgets ( First widget provides the toolbar so dont connect twice!)
      for ( unsigned int i = 1 ; i < OpenFlipper::Options::examinerWidgets() ; ++i )
        connect( button,SIGNAL( clicked() ), examiner_widgets_[i], SLOT( toggleProjectionMode() ) );

    } else if ( actions[i]->text() == "Stereo" ) {

      QToolButton * button = qobject_cast<QToolButton *>( viewerToolbar_->widgetForAction ( actions[i] ) );

      // Connect action to all other widgets ( First widget provides the toolbar so dont connect twice!)
      for ( unsigned int i = 1 ; i < OpenFlipper::Options::examinerWidgets() ; ++i )
        connect( button,SIGNAL( clicked() ), examiner_widgets_[i], SLOT( toggleStereoMode() ) );

    }

  }

  if ( !OpenFlipper::Options::multiView() ) {
    stackWidgetList_.push_back( StackWidgetInfo( false, "3D Examiner Widget", examiner_widgets_[0] ) );
  } else {
    stackWidgetList_.push_back( StackWidgetInfo( false, "3D Examiner Widget", tmp ) );
  }

  // Remember logger size
  wsizes = splitter_->sizes();
  originalLoggerSize_  = wsizes[1];

  // ======================================================================
  // Create Upper DockWidget for ToolWidget control
  // ======================================================================
  dockViewMode_ = new QDockWidget("ViewMode" , this );
  dockViewMode_->setObjectName("DockViewMode");
  QPushButton* button = new QPushButton("Change View Mode");
  dockViewMode_->setWidget(button);
  button->setParent(dockViewMode_);
  dockViewMode_->setFeatures( QDockWidget::NoDockWidgetFeatures );
  button->show();
  addDockWidget(Qt::RightDockWidgetArea,dockViewMode_);
  connect(button, SIGNAL(clicked()), this, SLOT(slotViewModeDialog()));
  dockViewMode_->setVisible(false);

  // ======================================================================
  // Context menu setup
  // ======================================================================

  for ( unsigned int i = 0 ; i < OpenFlipper::Options::examinerWidgets() ; ++i ) {
    connect( examiner_widgets_[i] , SIGNAL(signalCustomContextMenuRequested( const QPoint&) ) ,
             this                 , SLOT( slotCustomContextMenu( const QPoint&) ) );
  }

  contextMenu_ = new QMenu(this);
  contextSelectionMenu_ = new QMenu("Selection",0);

  setupMenuBar();

  updateRecent();

  statusBar_->showMessage("Ready", 5000);

  registerCoreKeys();

}


//-----------------------------------------------------------------------------


CoreWidget::~CoreWidget() {

}

//-----------------------------------------------------------------------------

/** Set viewer to Fullscreen Mode and back
  */
void
CoreWidget::toggleFullscreen() {

  setWindowState( windowState() ^  Qt::WindowFullScreen);

  OpenFlipper::Options::fullScreen( bool( windowState() & Qt::WindowFullScreen) );

  show();
}

//-----------------------------------------------------------------------------

/** Enable or disable Fullscreen Mode
  */
void
CoreWidget::setFullscreen(bool _state ) {
  if ( _state )
    setWindowState( windowState() | Qt::WindowFullScreen);
  else {
    if ( windowState() & Qt::WindowFullScreen )
      setWindowState( windowState() ^  Qt::WindowFullScreen);
  }

  OpenFlipper::Options::fullScreen( bool( windowState() & Qt::WindowFullScreen) );

  show();
}

//-----------------------------------------------------------------------------

/** Hide or show logger
  */
void
CoreWidget::toggleLogger() {
  // toggle
  OpenFlipper::Options::hideLogger( !OpenFlipper::Options::hideLogger() );

  // Hide/Show Logger
  showLogger( !OpenFlipper::Options::hideLogger() );
}

/** Hide or show logger
  */
void
CoreWidget::showLogger(bool _state) {
  //Hide Logger
  if ( !_state ) {
    QList<int> wsizes( splitter_->sizes() );

    // Remember old size
    originalLoggerSize_  = wsizes[1];

    wsizes[0] = wsizes[0]+wsizes[1];
    wsizes[1] = 0;
    splitter_->setSizes(wsizes);
  } else {

    if ( originalLoggerSize_ == 0)
        originalLoggerSize_ = 240;

    QList<int> wsizes( splitter_->sizes() );

    if (wsizes[0] == 0)
      wsizes[0] = height();

    wsizes[0] = wsizes[0]+wsizes[1] - originalLoggerSize_;
    wsizes[1] = originalLoggerSize_;
    splitter_->setSizes(wsizes);
  }
}

//-----------------------------------------------------------------------------

/** Hide or show toolbox
  */
void
CoreWidget::toggleToolbox() {

  //toggle
  showToolbox( OpenFlipper::Options::hideToolbox() );
}

//-----------------------------------------------------------------------------

/** Hide or show toolbox
  */
void
CoreWidget::showToolbox( bool _state ) {

  //toggle
  OpenFlipper::Options::hideToolbox( !_state );

  if ( OpenFlipper::Options::hideToolbox() ){
    //hide all toolWidgets
    for (uint p=0; p < plugins_.size(); p++)
      if (plugins_[p].widget)
        plugins_[p].widget->setVisible(false);

    //hide ViewMode Selection Widget
    dockViewMode_->setVisible(false);

  }else{
    //reset last ViewMode
    if (OpenFlipper::Options::defaultToolboxMode().trimmed() == "")
      setViewMode("All");
    else
      setViewMode( OpenFlipper::Options::defaultToolboxMode() );
  }
}

//=============================================================================

void
CoreWidget::addRecent(QString _filename, DataType _type)
{
  //dont add objects to recentMenu while loadind Settings
  if ( OpenFlipper::Options::loadingSettings() ) return;

  OpenFlipper::Options::addRecentFile(_filename, _type);

  updateRecent();

}

//=============================================================================

void
CoreWidget::updateRecent()
{
  if ( recentFilesMenu_ == NULL)
    return;

  recentFilesMenu_->clear();

  QVector< OpenFlipper::Options::RecentFile > recentFiles = OpenFlipper::Options::recentFiles();
  for (int i = 0 ; i < recentFiles.size() ; ++i ) {
    QString path = OpenFlipper::Options::iconDirStr() + OpenFlipper::Options::dirSeparator();
    path += typeIcon( recentFiles[i].type );

    recentFilesMenu_->addAction(QIcon(path), recentFiles[i].filename);
  }

}

void
CoreWidget::closeEvent ( QCloseEvent * /* event */ ) {
  emit exit();
}

void CoreWidget::showOptionsWidget() {

  if ( OpenFlipper::Options::nogui() )
    return;

  if ( optionsWidget_ == 0 ) {
    optionsWidget_ = new OptionsWidget(plugins_, coreKeys_, invKeys_, 0);
    connect(optionsWidget_,SIGNAL(applyOptions()),this,SIGNAL(applyOptions()));
    connect(optionsWidget_,SIGNAL(saveOptions()),this,SIGNAL(saveOptions()));
    connect(optionsWidget_,SIGNAL(addKeyMapping(int,Qt::KeyboardModifiers,QObject*,int)),
            this,          SLOT(slotAddKeyMapping(int,Qt::KeyboardModifiers,QObject*,int)));
  }

  //show the optionsWidget centered
  QPoint center;
  center.setX( x() + width() / 2 );
  center.setY( y() + height() / 2 );

  optionsWidget_->setGeometry(center.x() - optionsWidget_->width() / 2,
                              center.y() - optionsWidget_->height()/ 2, optionsWidget_->width(), optionsWidget_->height());

  optionsWidget_->show();

}


//=============================================================================
