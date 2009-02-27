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
#include <OpenFlipper/BasePlugin/PluginFunctionsCore.hh>
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
  shiftPressed_(false),
  viewModes_(_viewModes),
  dockViewMode_(0),
  viewModeButton_(0),
  viewModeMenu_(0),
  viewGroup_(0),
  splitter_(0),
  logWidget_(0),
  recentFilesMenu_(0),
  helpMenu_(0),
  sceneGraphDialog_(0),
  fileMenu_(0),
  viewMenu_(0),
  fileMenuEnd_(0),
  stereoButton_(0),
  projectionButton_(0),
  moveButton_(0),
  lightButton_(0),
  pickButton_(0),
  questionButton_(0),
  contextMenu_(0),
  functionMenu_(0),
  contextSelectionMenu_(0),
  stackMenu_(0),
  helpBrowserDeveloper_(0),
  helpBrowserUser_(0),
  aboutWidget_(0),
  optionsWidget_(0),
  plugins_(_plugins),
  stereoActive_(false)
{
  setupStatusBar();

  splitter_ = new QSplitter(Qt::Vertical,this);
  setCentralWidget(splitter_);
  stackedWidget_ = new QStackedWidget(splitter_);

  QGLFormat format;
  QGLFormat::setDefaultFormat(format);
  format.setStereo( OpenFlipper::Options::stereo() );
  format.setAlpha(true);

  // Construct GL context & widget

  baseLayout_ = new QtMultiViewLayout;
  baseLayout_->setContentsMargins(0,0,0,0);

  glWidget_ = new QGLWidget(format);
  glView_ = new QtGLGraphicsView(stackedWidget_);
  glScene_ = new QtGLGraphicsScene (&examiner_widgets_, baseLayout_);

  // is stereo possible, use it?
  if (format.stereo()) {
    std::cerr << "Stereo buffer requested: " << (glWidget_->format().stereo() ? "ok\n" : "failed\n");
    OpenFlipper::Options::stereo(glWidget_->format().stereo());
  }


  glView_->setViewport(glWidget_);
  glView_->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
  glView_->setScene(glScene_);


  centerWidget_ = new QGraphicsWidget;
  glScene_->addItem(centerWidget_);
  centerWidget_->setGeometry (glScene_->sceneRect ());

  connect ( glView_, SIGNAL( sceneRectChanged( const QRectF & ) ),
            this, SLOT( sceneRectChanged( const QRectF & ) ) );

  stackedWidget_->addWidget(glView_);
  stackWidgetList_.push_back( StackWidgetInfo( false, "3D Examiner Widget", glView_ ) );

  // ======================================================================
  // Set up the logging window
  // ======================================================================

  slidingLogger_ = new QtSlideWindow ("Log Viewer", centerWidget_);
  tempLogWidget = new QWidget;

  logWidget_ = new LoggerWidget(splitter_);
  logWidget_->setReadOnly(true);
  logWidget_->setSizePolicy( QSizePolicy ( QSizePolicy::Preferred , QSizePolicy::Preferred ) );
  logWidget_->resize( splitter_->width() ,240);
  logWidget_->setLineWrapMode( QTextEdit::NoWrap );

  originalLoggerSize_ = 0;
  loggerState_ = OpenFlipper::Options::Normal;

  QList<int> wsizes( splitter_->sizes() );
  
  if (OpenFlipper::Options::loggerState() == OpenFlipper::Options::InScene) {
    slidingLogger_->attachWidget (logWidget_);
    splitter_->insertWidget (1, tempLogWidget);
    wsizes[0] = 1;
    wsizes[1] = 0;
    splitter_->setSizes(wsizes);
    loggerState_ = OpenFlipper::Options::InScene;
  } else if (OpenFlipper::Options::loggerState() == OpenFlipper::Options::Hidden) {
    splitter_->insertWidget (1, tempLogWidget);
    wsizes[0] = 1;
    wsizes[1] = 0;
    splitter_->setSizes(wsizes);
    loggerState_ = OpenFlipper::Options::Hidden;
  } else {
    // Set initial values to have a usable state
    wsizes[0] = 480;
    wsizes[1] = 240;
    splitter_->setSizes(wsizes);
  }

  // ======================================================================
  // Create examiner
  // ======================================================================

  if ( !OpenFlipper::Options::multiView() ) {

    glViewer* examinerWidget = new glViewer(glScene_,
					    glWidget_,
              PluginFunctions::viewerProperties(0),
					    centerWidget_,
                                            "Examiner Widget",
                                            statusBar_);

    examiner_widgets_.push_back(examinerWidget);

    examinerWidget->sceneGraph( PluginFunctions::getSceneGraphRootNode() );

    baseLayout_->addItem(examinerWidget, 0);

  } else {


    // Create examiners
    for ( unsigned int i = 0 ; i < OpenFlipper::Options::examinerWidgets() ; ++i ) {
      glViewer* newWidget = new glViewer(glScene_,
					 glWidget_,
           PluginFunctions::viewerProperties(i),
					 centerWidget_,
					 "Examiner Widget",
           statusBar_);

      examiner_widgets_.push_back(newWidget);
    }

    // Initialize all examiners
    for ( unsigned int i = 0 ; i < OpenFlipper::Options::examinerWidgets() ; ++i ) {
      examiner_widgets_[i]->sceneGraph( PluginFunctions::getSceneGraphRootNode() );
    }

    baseLayout_->addItem(examiner_widgets_[0],0);
    baseLayout_->addItem(examiner_widgets_[1],1);
    baseLayout_->addItem(examiner_widgets_[2],2);
    baseLayout_->addItem(examiner_widgets_[3],3);
  }

  // Make examiner available to the plugins ( defined in PluginFunctions.hh)
  PluginFunctions::setViewers( examiner_widgets_ );

  centerWidget_->setLayout(baseLayout_);


  // ======================================================================
  // Setup dragging for examiner widget
  // ======================================================================
  for ( uint i = 0 ; i < OpenFlipper::Options::examinerWidgets() ; ++i ) {

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


  // Create the toolbar
  viewerToolbar_ = new QToolBar( "Viewer Toolbar", this );
  viewerToolbar_->setOrientation(Qt::Vertical);
  viewerToolbar_->setAllowedAreas(Qt::AllToolBarAreas);
  viewerToolbar_->setIconSize(QSize(20,20));
  viewerToolbar_->setObjectName("ViewerToolbar");

  moveButton_ = new QToolButton( viewerToolbar_ );
  moveButton_->setIcon( QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"transform-move.png") );
  moveButton_->setMinimumSize( 16, 16 );
  moveButton_->setMaximumSize( 32, 32 );
  moveButton_->setToolTip( "Switch to <b>move</b> mode." );
  moveButton_->setWhatsThis(
                  "Switch to <b>move</b> mode.<br>"
                  "<ul><li><b>Rotate</b> using <b>left</b> mouse button.</li>"
                  "<li><b>Translate</b> using <b>middle</b> mouse button.</li>"
                  "<li><b>Zoom</b> using <b>left+middle</b> mouse buttons.</li></ul>" );
  for ( unsigned int i = 0 ; i < OpenFlipper::Options::examinerWidgets() ; ++i ) {
    connect( moveButton_,SIGNAL( clicked() ), &PluginFunctions::viewerProperties(i), SLOT( setExamineMode() ) );
    connect( &PluginFunctions::viewerProperties(i) , SIGNAL( actionModeChanged( Viewer::ActionMode ) ),
             this                                  , SLOT(   slotActionModeChanged(Viewer::ActionMode) ) );
  }

  viewerToolbar_->addWidget( moveButton_ )->setText("Move");
  moveButton_->setDown(true);


  lightButton_ = new QToolButton( viewerToolbar_ );
  lightButton_->setIcon( QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"light-mode.png") );
  lightButton_->setMinimumSize( 16, 16 );
  lightButton_->setMaximumSize( 32, 32 );
  lightButton_->setToolTip("Switch to <b>light</b> mode.");
  lightButton_->setWhatsThis(
                  "Switch to <b>light</b> mode.<br>"
                  "Rotate lights using left mouse button.");
  for ( unsigned int i = 0 ; i < OpenFlipper::Options::examinerWidgets() ; ++i )
        connect( lightButton_,SIGNAL( clicked() ), &PluginFunctions::viewerProperties(i), SLOT( setLightMode() ) );
  viewerToolbar_->addWidget( lightButton_)->setText("Light");


  pickButton_ = new QToolButton( viewerToolbar_ );
  pickButton_->setIcon( QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"pick.png") );
  pickButton_->setMinimumSize( 16, 16 );
  pickButton_->setMaximumSize( 32, 32 );
  pickButton_->setToolTip("Switch to <b>picking</b> mode.");
  pickButton_->setWhatsThis(
                  "Switch to <b>picking</b> mode.<br>"
                  "Use picking functions like flipping edges.<br>"
                  "To change the mode use the right click<br>"
                  "context menu in the viewer.");
  for ( unsigned int i = 0 ; i < OpenFlipper::Options::examinerWidgets() ; ++i )
    connect( pickButton_,SIGNAL( clicked() ), &PluginFunctions::viewerProperties(i), SLOT( setPickingMode() ) );
  viewerToolbar_->addWidget( pickButton_)->setText("Pick");


  questionButton_ = new QToolButton( viewerToolbar_ );
  questionButton_->setIcon( QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"help-about.png") );
  questionButton_->setMinimumSize( 16, 16 );
  questionButton_->setMaximumSize( 32, 32 );
  questionButton_->setToolTip("Switch to <b>identification</b> mode.");
  questionButton_->setWhatsThis(
                  "Switch to <b>identification</b> mode.<br>"
                  "Use identification mode to get information "
                  "about objects. Click on an object and see "
                  "the log output for information about the "
                  "object.");
  for ( unsigned int i = 0 ; i < OpenFlipper::Options::examinerWidgets() ; ++i )
    connect( questionButton_,SIGNAL( clicked() ), &PluginFunctions::viewerProperties(i), SLOT( setQuestionMode() ) );
  viewerToolbar_->addWidget( questionButton_)->setText("Question");

  viewerToolbar_->addSeparator();

  QToolButton* homeButton = new QToolButton( viewerToolbar_ );
  homeButton->setIcon( QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"go-home.png") );
  homeButton->setMinimumSize( 16, 16 );
  homeButton->setMaximumSize( 32, 32 );
  homeButton->setCheckable( false );
  homeButton->setToolTip("Restore <b>home</b> view.");
  homeButton->setWhatsThis(
                  "Restore home view<br><br>"
                  "Resets the view to the home view");
  for ( unsigned int i = 0 ; i < OpenFlipper::Options::examinerWidgets() ; ++i )
    connect( homeButton,SIGNAL( clicked() ), examiner_widgets_[i], SLOT( home() ) );
  viewerToolbar_->addWidget( homeButton)->setText("Home");


  QToolButton* setHomeButton = new QToolButton( viewerToolbar_ );
  setHomeButton->setIcon( QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"set-home.png") );
  setHomeButton->setMinimumSize( 16, 16 );
  setHomeButton->setMaximumSize( 32, 32 );
  setHomeButton->setCheckable( false );
  setHomeButton->setToolTip("Set <b>home</b> view");
  setHomeButton->setWhatsThis(
                  "Store home view<br><br>"
                  "Stores the current view as the home view");
  for ( unsigned int i = 0 ; i < OpenFlipper::Options::examinerWidgets() ; ++i )
    connect( setHomeButton,SIGNAL( clicked() ), examiner_widgets_[i], SLOT( setHome() ) );
  viewerToolbar_->addWidget( setHomeButton)->setText("Set Home");


  QToolButton* viewAllButton = new QToolButton( viewerToolbar_ );
  viewAllButton->setIcon( QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"viewall.png") );
  viewAllButton->setMinimumSize( 16, 16 );
  viewAllButton->setMaximumSize( 32, 32 );
  viewAllButton->setCheckable( false );
  viewAllButton->setToolTip("View all.");
  viewAllButton->setWhatsThis(
                  "View all<br><br>"
                  "Move the objects in the scene so that"
                  " the whole scene is visible.");
  for ( unsigned int i = 0 ; i < OpenFlipper::Options::examinerWidgets() ; ++i )
    connect( viewAllButton,SIGNAL( clicked() ), examiner_widgets_[i], SLOT( viewAll() ) );
  viewerToolbar_->addWidget( viewAllButton)->setText("View all");


  projectionButton_ = new QToolButton( viewerToolbar_ );
  projectionButton_->setIcon( QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"perspective.png") );
  projectionButton_->setMinimumSize( 16, 16 );
  projectionButton_->setMaximumSize( 32, 32 );
  projectionButton_->setCheckable( false );
  projectionButton_->setToolTip(
                "Switch between <b>perspective</b> and "
                "<b>parrallel</b> projection mode.");
  projectionButton_->setWhatsThis(
                "Switch projection modes<br><br>"
                "Switch between <b>perspective</b> and "
                "<b>parrallel</b> projection mode.");
  for ( unsigned int i = 0 ; i < OpenFlipper::Options::examinerWidgets() ; ++i ) {
    connect( projectionButton_,SIGNAL( clicked() ), examiner_widgets_[i], SLOT( toggleProjectionMode() ) );
    connect( examiner_widgets_[i],SIGNAL( projectionModeChanged( bool ) ), this , SLOT( slotProjectionModeChanged( bool ) ) );
  }


  viewerToolbar_->addWidget( projectionButton_)->setText( "Projection" );


  viewerToolbar_->addSeparator();

  QToolButton* sceneGraphButton = new QToolButton( viewerToolbar_ );
  sceneGraphButton->setIcon( QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"scenegraph.png") );
  sceneGraphButton->setMinimumSize( 16, 16 );
  sceneGraphButton->setMaximumSize( 32, 32 );
  sceneGraphButton->setCheckable( false );
  sceneGraphButton->setToolTip("Toggle scene graph viewer.");
  sceneGraphButton->setWhatsThis(
                  "Toggle scene graph viewer<br><br>"
                  "The scene graph viewer enables you to examine the "
                  "displayed scene graph and to modify certain nodes.<br><br>"
                  "There are three modi for the scene graph viewer:"
                  "<ul><li><b>hidden</b></li>"
                  "<li><b>split</b>: share space</li>"
                  "<li><b>dialog</b>: own dialog window</li></ul>"
                  "This button toggles between these modi.");
  QObject::connect( sceneGraphButton, SIGNAL( clicked() ),
                    this, SLOT( slotShowSceneGraphDialog() ) );
  viewerToolbar_->addWidget( sceneGraphButton)->setText( "SceneGraph" );


  if (OpenFlipper::Options::stereo())
  {
    stereoButton_ = new QToolButton( viewerToolbar_ );
    stereoButton_->setIcon(  QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"mono.png")  );
    stereoButton_->setMinimumSize( 16, 16 );
    stereoButton_->setMaximumSize( 32, 32 );
    stereoButton_->setCheckable( true );
    stereoButton_->setToolTip( "Toggle stereo viewing");
    stereoButton_->setWhatsThis(
                  "Toggle stereo mode<br><br>"
                  "Use this button to switch between stereo "
                  "and mono view. To use this feature you need "
                  "a stereo capable graphics card and a stereo "
                  "display/projection system.");
    connect( stereoButton_,SIGNAL( clicked() ), this , SLOT( slotToggleStereoMode() ) );
    viewerToolbar_->addWidget( stereoButton_ )->setText( "Stereo");
  }


  addToolBar(Qt::TopToolBarArea,viewerToolbar_);

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


  setWindowIcon( OpenFlipper::Options::OpenFlipperIcon() );

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

  switch (OpenFlipper::Options::loggerState ())
  {
    case OpenFlipper::Options::InScene:
      OpenFlipper::Options::loggerState(OpenFlipper::Options::Normal);
      break;
    case OpenFlipper::Options::Normal:
      OpenFlipper::Options::loggerState(OpenFlipper::Options::Hidden);
      break;
    case OpenFlipper::Options::Hidden:
      OpenFlipper::Options::loggerState(OpenFlipper::Options::InScene);
      break;
  }

  // Hide/Show Logger
  showLogger( OpenFlipper::Options::loggerState() );
}

/** Hide or show logger
  */
void
CoreWidget::showLogger(OpenFlipper::Options::LoggerState _state) {
  //Hide Logger
  if (_state == loggerState_)
    return;

  switch (_state)
  {
    case OpenFlipper::Options::InScene:
      {
        QList<int> wsizes( splitter_->sizes() );

        // Remember old size
        if (loggerState_ == OpenFlipper::Options::Normal)
          originalLoggerSize_  = wsizes[1];

	if ( originalLoggerSize_ == 0)
          originalLoggerSize_ = 240;

        splitter_->insertWidget (1, tempLogWidget);
        wsizes[0] = wsizes[0]+wsizes[1];
        wsizes[1] = 0;
        splitter_->setSizes(wsizes);
        logWidget_->resize (logWidget_->width (), originalLoggerSize_);
        slidingLogger_->attachWidget (logWidget_);
      }
      break;
    case OpenFlipper::Options::Normal:
      {
        if ( originalLoggerSize_ == 0)
          originalLoggerSize_ = 240;

        QList<int> wsizes( splitter_->sizes() );

        if (loggerState_ == OpenFlipper::Options::InScene)
          originalLoggerSize_ = logWidget_->height ();

        slidingLogger_->detachWidget ();
        splitter_->insertWidget (1, logWidget_);

        wsizes[0] = wsizes[0]+wsizes[1] - originalLoggerSize_;
        wsizes[1] = originalLoggerSize_;
        splitter_->setSizes(wsizes);
      }
      break;
    case OpenFlipper::Options::Hidden:
      {
        QList<int> wsizes( splitter_->sizes() );

        // Remember old size
        if (loggerState_ == OpenFlipper::Options::Normal)
          originalLoggerSize_  = wsizes[1];

	if (loggerState_ == OpenFlipper::Options::InScene)
	{
	  slidingLogger_->detachWidget ();
	  originalLoggerSize_ = logWidget_->height ();
	}

        splitter_->insertWidget (1, tempLogWidget);
        wsizes[0] = wsizes[0]+wsizes[1];
        wsizes[1] = 0;
        splitter_->setSizes(wsizes);
      }
      break;
  }
  loggerState_ = _state;

/*
  if ( !_state ) {
    QList<int> wsizes( splitter_->sizes() );

    // Remember old size
    originalLoggerSize_  = wsizes[1];

    int height = logWidget_->height ();

    splitter_->insertWidget (1, tempLogWidget);
    wsizes[0] = wsizes[0]+wsizes[1];
    wsizes[1] = 0;
    splitter_->setSizes(wsizes);
    logWidget_->resize (logWidget_->width (), height);
    slidingLogger_->attachWidget (logWidget_);

  } else if (splitter_->widget (1) == logWidget_) {

    if ( originalLoggerSize_ == 0)
        originalLoggerSize_ = 240;

    QList<int> wsizes( splitter_->sizes() );

    if (wsizes[0] == 0)
      wsizes[0] = height();

    wsizes[0] = wsizes[0]+wsizes[1] - originalLoggerSize_;
    wsizes[1] = originalLoggerSize_;
    splitter_->setSizes(wsizes);
  } else {

    QList<int> wsizes( splitter_->sizes() );

    int height = logWidget_->height ();

    slidingLogger_->detachWidget ();
    splitter_->insertWidget (1, logWidget_);

    wsizes[0] = wsizes[0]+wsizes[1] - height;
    wsizes[1] = height;
    splitter_->setSizes(wsizes);
  }
  */
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

    optionsWidget_->setWindowIcon( OpenFlipper::Options::OpenFlipperIcon() );
  }

  //show the optionsWidget centered
  QPoint center;
  center.setX( x() + width() / 2 );
  center.setY( y() + height() / 2 );

  optionsWidget_->setGeometry(center.x() - optionsWidget_->width() / 2,
                              center.y() - optionsWidget_->height()/ 2, optionsWidget_->width(), optionsWidget_->height());

  optionsWidget_->show();

}


void
CoreWidget::slotShowSceneGraphDialog()
{
  if ( PluginFunctions::getSceneGraphRootNode() )
  {
    if (!sceneGraphDialog_)
    {
      sceneGraphDialog_ = new ACG::QtWidgets::QtSceneGraphDialog( this, PluginFunctions::getSceneGraphRootNode() );

      for ( unsigned int i = 0 ; i < OpenFlipper::Options::examinerWidgets() ; ++i ) {
        connect(examiner_widgets_[i], SIGNAL(signalSceneGraphChanged(ACG::SceneGraph::BaseNode*)),
                sceneGraphDialog_,    SLOT(update(ACG::SceneGraph::BaseNode*)));

        connect(sceneGraphDialog_, SIGNAL(signalNodeChanged(ACG::SceneGraph::BaseNode*)),
                examiner_widgets_[i], SLOT(updateGL()));
      }

    }

    sceneGraphDialog_->show();
  }
}

//-----------------------------------------------------------------------------

void
CoreWidget::sceneRectChanged(const QRectF &rect)
{
  centerWidget_->setGeometry (rect);
  slidingLogger_->updateGeometry ();
}




//=============================================================================
