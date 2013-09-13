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

#include <OpenFlipper/widgets/videoCaptureDialog/VideoCaptureDialog.hh>

#include <OpenFlipper/widgets/glWidget/CursorPainter.hh>

#include <ACG/Scenegraph/SceneGraphAnalysis.hh>

// -------------------- Qt event Includes
#include <QEvent>
#include <QWhatsThisClickedEvent>

#define WIDGET_HEIGHT 800
#define WIDGET_WIDTH  800

//== IMPLEMENTATION ==========================================================

PickMode::PickMode(const std::string& _n, const bool _t, const bool _v, QCursor _c, QToolBar *_tb ) :
      name_(_n),
      tracking_(_t),
      visible_(_v),
      cursor_(_c),
      toolbar_(_tb)
{

}

void PickMode::visible(const bool _visible) {
  visible_ = _visible;
}

bool PickMode::visible() const  {
  return visible_;
}

std::string PickMode::name() const {
  return name_;
}

void PickMode::tracking(bool _tracking) {
  tracking_ = _tracking;
}

bool PickMode::tracking() const {
  return tracking_;
}

void PickMode::cursor(const QCursor _cursor) {
  cursor_ = _cursor;
}

QCursor PickMode::cursor() const{
  return cursor_;
}

void PickMode::toolbar(QToolBar* _toolbar) {
  toolbar_ = _toolbar;
}

QToolBar * PickMode::toolbar() const {
  return toolbar_;
}









/** \brief Constructor for the Core Widget
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
  viewModeButton_(0),
  viewModeMenu_(0),
  splitter_(0),
  logWidget_(0),
  recentFilesMenu_(0),
  helpMenu_(0),
  windowMenu_(0),
  AC_ShowViewModeControls_(0),
  pickToolBarExternal_(0),
  cursorPainter_(0),
  sceneGraphDialog_(0),
  fileMenu_(0),
  viewMenu_(0),
  toolsMenu_(0),
  fileMenuEnd_(0),
  stereoButton_(0),
  moveButton_(0),
  pickButton_(0),
  questionButton_(0),
  globalDrawMenu_(0),
  rendererMenu_(0),
  postprocessorMenu_(0),
  drawGroup_(0),
  rendererGroup_(0),
  postProcessorGroup_(0),
  viewGroup_(0),
  perspectiveProjectionAction_(0),
  activeDrawModes_(0),
  availableGlobalDrawModes_(0),
  contextMenu_(0),
  contextSelectionMenu_(0),
  drawGroupViewer_(0),
  viewerDrawMenu_(0),
  coordSysMenu_(0),
  snapshotName_(""),
  snapshotCounter_(0),
  stackMenu_(0),
  helpWidget_(0),
  postProcessorDialog_(0),
  rendererDialog_(0),
  rendererObjectWidget_(0),
  stereoSettingsWidget_(0),
  aboutWidget_(0),
  optionsWidget_(0),
  plugins_(_plugins),
  stereoActive_(false),
  actionMode_(Viewer::PickingMode),
  lastActionMode_(Viewer::ExamineMode),
  pickMenu_(0),
  pick_mode_name_(""),
  pick_mode_idx_(-1)
{

  setupStatusBar();

  defaultIconSize_ = iconSize();

  toolSplitter_ = new QSplitter(Qt::Horizontal,this);

  setCentralWidget(toolSplitter_);

  splitter_ = new QSplitter(Qt::Vertical,toolSplitter_);
  stackedWidget_ = new QStackedWidget(splitter_);

  QGLFormat format = QGLFormat::defaultFormat();

  #ifdef ARCH_DARWIN
  format.setStereo(false);
  #else
  format.setStereo( OpenFlipper::Options::stereo() );
  #endif
  format.setAlpha(true);
  format.setStencil(true);
  format.setSampleBuffers(true);
  QGLFormat::setDefaultFormat(format);

  // Construct GL context & widget
  baseLayout_ = new QtMultiViewLayout;
  baseLayout_->setContentsMargins(0,0,0,0);

  // ===============================================================================
  // Test context capabilities ...
  // If we get stereo buffers, we use them .. which might disable multisampling
  // If we don't have stereo, we disable it to not interfere with multisampling
  // ===============================================================================
  QGLWidget* test = new QGLWidget(format);
  if ( ! test->format().stereo() ) {
    //     std::cerr << "No stereo ... disabling stereo for real context!" << std::endl;
    format.setStereo(false);
    QGLFormat::setDefaultFormat(format);
  }/* else {
    std::cerr << "Stereo found ok" << std::endl;
  }*/

  delete test;

  glWidget_ = new QGLWidget(format,0);
  PluginFunctions::shareGLWidget (glWidget_);

  glView_ = new QtGLGraphicsView(stackedWidget_);
  glScene_ = new QtGLGraphicsScene (&examiner_widgets_, baseLayout_);

  // is stereo possible, use it?
  OpenFlipper::Options::glStereo(glWidget_->format().stereo());

  glView_->setViewport(glWidget_);
  glView_->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
  glView_->setScene(glScene_);
  glView_->setFrameStyle(QFrame::NoFrame);

  // gl widget as parent to make sure that the CursorPainter will be deleted before
  cursorPainter_ = new CursorPainter (glWidget_);
  cursorPainter_->setForceNative (OpenFlipperSettings().value("Core/Gui/glViewer/nativeMouse",false).toBool() );
  cursorPainter_->setEnabled (stereoActive_);
  glScene_->setCursorPainter (cursorPainter_);

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

  slidingLogger_ = new QtSlideWindow (tr("Log Viewer"), centerWidget_);

  logWidget_ = new LoggerWidget(splitter_);
  logWidget_->setSizePolicy( QSizePolicy ( QSizePolicy::Preferred , QSizePolicy::Preferred ) );
  logWidget_->resize( splitter_->width() ,240);

  originalLoggerSize_ = 0;
  loggerState_ = OpenFlipper::Options::Normal;

  QList<int> wsizes(splitter_->sizes());
  // Set initial values to have a usable state
  wsizes[0] = 480;
  wsizes[1] = 240;
  splitter_->setSizes(wsizes);

  if (OpenFlipper::Options::loggerState() == OpenFlipper::Options::InScene) {

    slidingLogger_->attachWidget (logWidget_);
    showLoggerInSplitView(false);
    loggerState_ = OpenFlipper::Options::InScene;

  } else if (OpenFlipper::Options::loggerState() == OpenFlipper::Options::Hidden) {

    showLoggerInSplitView(false);
    loggerState_ = OpenFlipper::Options::Hidden;
  } else {

    showLoggerInSplitView(true);
  }


  // ======================================================================
  // Create examiner
  // ======================================================================

  // First we analyze the scenegraph
  unsigned int maxPases = 1;
  ACG::Vec3d bbmin,bbmax;
  ACG::SceneGraph::analyzeSceneGraph(PluginFunctions::getSceneGraphRootNode(),maxPases,bbmin,bbmax);

  if ( !OpenFlipperSettings().value("Core/Gui/glViewer/useMultipleViewers",true).toBool() ) {

    glViewer* examinerWidget = new glViewer(glScene_,
					                                  glWidget_,
					                                  PluginFunctions::viewerProperties(0),
					                                  centerWidget_);

    examiner_widgets_.push_back(examinerWidget);

    examinerWidget->sceneGraph( PluginFunctions::getSceneGraphRootNode(), maxPases,bbmin,bbmax );

    baseLayout_->addItem(examinerWidget, 0);

    cursorPainter_->registerViewer (examinerWidget);

  } else {


    // Create examiners
    for ( unsigned int i = 0 ; i < OpenFlipper::Options::examinerWidgets() ; ++i ) {
      glViewer* newWidget = new glViewer(glScene_,
					                               glWidget_,
                                         PluginFunctions::viewerProperties(i),
                                         centerWidget_);

      examiner_widgets_.push_back(newWidget);

      connect (&PluginFunctions::viewerProperties(i), SIGNAL( getPickMode(std::string&) ),
               this,                                   SLOT( getPickMode(std::string&) ),Qt::DirectConnection );
      connect (&PluginFunctions::viewerProperties(i), SIGNAL( setPickMode(const std::string) ),
               this,                                   SLOT( setPickMode(const std::string) ),Qt::DirectConnection );
      connect (&PluginFunctions::viewerProperties(i), SIGNAL( getActionMode(Viewer::ActionMode&) ),
               this,                                   SLOT( getActionMode(Viewer::ActionMode&) ),Qt::DirectConnection );
      connect (&PluginFunctions::viewerProperties(i), SIGNAL( setActionMode(const Viewer::ActionMode) ),
               this,                                   SLOT( setActionMode(const Viewer::ActionMode)), Qt::DirectConnection );

      connect (&PluginFunctions::viewerProperties(i), SIGNAL( drawModeChanged(int)), this, SIGNAL(drawModeChanged(int)));
    }

    // Initialize all examiners
    for ( unsigned int i = 0 ; i < OpenFlipper::Options::examinerWidgets() ; ++i ) {
      examiner_widgets_[i]->sceneGraph( PluginFunctions::getSceneGraphRootNode(), maxPases,bbmin,bbmax );
      cursorPainter_->registerViewer (examiner_widgets_[i]);
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
    connect (examiner_widgets_[i], SIGNAL(signalMakeActive ()),
             this, SLOT(slotActivateExaminer()));

    connect (examiner_widgets_[i], SIGNAL(statusMessage(QString, int)),
             statusBar_, SLOT(showMessage(const QString&, int)));
  }


  // ======================================================================
  // Create main Toolbar
  // ======================================================================

  mainToolbar_ = new QToolBar(tr("Main Toolbar"));
  mainToolbar_->setWindowTitle(tr("Main Toolbar"));
  mainToolbar_->setObjectName(tr("MainToolbar"));
  slotAddToolbar(mainToolbar_);

  // ======================================================================
  // Get Toolbar from examiner and integrate it into main window
  // ======================================================================


  // Create the toolbar
  viewerToolbar_ = new QToolBar( tr("Viewer Toolbar" ), this );
  viewerToolbar_->setOrientation(Qt::Vertical);
  viewerToolbar_->setAllowedAreas(Qt::AllToolBarAreas);
  viewerToolbar_->setIconSize(QSize(20,20));
  viewerToolbar_->setObjectName(tr("ViewerToolbar"));

  slotAddToolbar(viewerToolbar_);

  moveButton_ = new QToolButton( viewerToolbar_ );
  moveButton_->setIcon( QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"transform-move.png") );
  moveButton_->setMinimumSize( 16, 16 );
  moveButton_->setMaximumSize( 32, 32 );
  moveButton_->setCheckable(true);
  moveButton_->setToolTip( tr("Switch to <b>move</b> mode.") );
  moveButton_->setWhatsThis(tr(
                  "Switch to <b>move</b> mode.<br>"
                  "<ul><li><b>Rotate</b> using <b>left</b> mouse button.</li>"
                  "<li><b>Translate</b> using <b>middle</b> mouse button.</li>"
                  "<li><b>Zoom</b> using <b>left+middle</b> mouse buttons.</li></ul>" ));

  connect( moveButton_,SIGNAL( clicked() ), this, SLOT( setExamineMode() ) );
/*
  connect( this, SIGNAL( actionModeChanged( Viewer::ActionMode ) ),
            this, SLOT(   slotActionModeChanged(Viewer::ActionMode) ) );*/

  viewerToolbar_->addWidget( moveButton_ )->setText(tr("Move"));
  moveButton_->setChecked(true);

  
  pickButton_ = new QToolButton( viewerToolbar_ );
  pickButton_->setIcon( QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"pick.png") );
  pickButton_->setMinimumSize( 16, 16 );
  pickButton_->setMaximumSize( 32, 32 );
  pickButton_->setCheckable(true);
  pickButton_->setToolTip(tr("Switch to <b>picking</b> mode."));
  pickButton_->setWhatsThis(tr(
                  "Switch to <b>picking</b> mode.<br>"
                  "Use picking functions like flipping edges.<br>"
                  "To change the mode use the right click<br>"
                  "context menu in the viewer."));
  connect( pickButton_,SIGNAL( clicked() ), this, SLOT( setPickingMode() ) );
  viewerToolbar_->addWidget( pickButton_)->setText(tr("Pick"));


  questionButton_ = new QToolButton( viewerToolbar_ );
  questionButton_->setIcon( QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"help-about.png") );
  questionButton_->setMinimumSize( 16, 16 );
  questionButton_->setMaximumSize( 32, 32 );
  questionButton_->setCheckable(true);
  questionButton_->setToolTip(tr("Switch to <b>identification</b> mode."));
  questionButton_->setWhatsThis(tr(
                  "Switch to <b>identification</b> mode.<br>"
                  "Use identification mode to get information "
                  "about objects. Click on an object and see "
                  "the log output for information about the "
                  "object."));
  connect( questionButton_,SIGNAL( clicked() ), this, SLOT( setQuestionMode() ) );
  viewerToolbar_->addWidget( questionButton_)->setText(tr("Question"));

  viewerLayoutBox_ = new QComboBox( viewerToolbar_ );
  viewerLayoutBox_->setMinimumSize( 32, 16 );
  viewerLayoutBox_->setMaximumSize( 64, 32 );
  viewerLayoutBox_->setToolTip(tr("Switch <b>viewer layout</b>."));
  viewerLayoutBox_->setWhatsThis(tr(
                  "Switch <b>viewer layout</b>.<br>"
                  "Select the desired viewer layout. "
                  "Possible layouts are: "
                  "<ul> "
				  "<li>Single viewer</li>"
          "<li>Double viewer</li>"
				  "<li>Multiple viewers (grid)</li>"
				  "<li>Multiple viewers (hsplit)</li>"
				  "</ul>"));

  viewerLayoutBox_->addItem(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"singleviewmode.png"), "");
  viewerLayoutBox_->addItem(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"doubleviewmode.png"), "");
  viewerLayoutBox_->addItem(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"multiviewmode1.png"), "");
  viewerLayoutBox_->addItem(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"multiviewmode2.png"), "");

  viewerLayoutBox_->setIconSize(QSize(22,22));

  connect( viewerLayoutBox_,SIGNAL( activated(int) ), this, SLOT( setViewerLayout(int) ) );

  extended_actions.push_back(viewerToolbar_->addWidget( viewerLayoutBox_ ));

  extended_actions.push_back(viewerToolbar_->addSeparator());

  if (OpenFlipper::Options::stereo())
  {
    stereoButton_ = new QToolButton( viewerToolbar_ );
    stereoButton_->setIcon(  QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"mono.png")  );
    stereoButton_->setMinimumSize( 16, 16 );
    stereoButton_->setMaximumSize( 32, 32 );
    stereoButton_->setCheckable( true );
    stereoButton_->setToolTip(tr( "Toggle stereo viewing"));
    // We want a custom context menu
    stereoButton_->setContextMenuPolicy(Qt::CustomContextMenu);
    stereoButton_->setWhatsThis(tr(
                  "Toggle stereo mode<br><br>"
                  "Use this button to switch between stereo "
                  "and mono view. To use this feature you need "
                  "a stereo capable graphics card and a stereo "
                  "display/projection system."));
    connect( stereoButton_, SIGNAL( clicked() ), this , SLOT( slotToggleStereoMode() ) );
    // Custom context menu
    connect( stereoButton_, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(stereoButtonContextMenu(const QPoint &)));
    QAction *stereoAction = viewerToolbar_->addWidget( stereoButton_ );
    stereoAction->setText( tr("Stereo"));
    extended_actions.push_back(stereoAction);
  }


  addToolBar(Qt::TopToolBarArea,viewerToolbar_);

  // Remember logger size
  wsizes = splitter_->sizes();

  if(wsizes.size() > 1) {
    originalLoggerSize_  = wsizes[1];
  } else {
    originalLoggerSize_ = 240;
  }

  // ======================================================================
  // Create ToolBox area
  // ======================================================================

  toolBoxArea_ = new QWidget (toolSplitter_);

  viewModeControlBox_ = new QGroupBox (tr("ViewMode"));

  QHBoxLayout *hLayout = new QHBoxLayout;

  vmChangeButton_ = new QPushButton(tr("Change View Mode"));
  QPushButton* vmEditButton   = new QPushButton(tr("Edit View Modes"));

  hLayout->addWidget(vmChangeButton_);
  hLayout->addWidget(vmEditButton);
  viewModeControlBox_->setLayout (hLayout);

  connect(vmChangeButton_, SIGNAL(clicked()), this, SLOT(slotViewChangeDialog()));
  connect(vmEditButton, SIGNAL(clicked()), this, SLOT(slotViewModeDialog()));

  toolBoxScroll_ = new QScrollArea ();
  toolBox_ = new SideArea ();
  toolBoxScroll_->setWidget (toolBox_);
  toolBoxScroll_->setWidgetResizable (true);
  toolBoxScroll_->setFrameStyle (QFrame::StyledPanel);

  QVBoxLayout *vLayout = new QVBoxLayout;
  vLayout->addWidget(viewModeControlBox_);
  vLayout->addWidget(toolBoxScroll_);
  
  if ( OpenFlipperSettings().value("Core/Gui/TaskSwitcher/Hide",false).toBool() ) {
    viewModeControlBox_->hide();
    if (viewModeButton_)
        viewModeButton_->setVisible(false);
  }

  toolBoxArea_->setLayout (vLayout);

  wsizes = toolSplitter_->sizes();

  // if the toolbox should be on the right, use the defaults. Otherwise, we have to reorder them.
  if ( OpenFlipperSettings().value("Core/Gui/ToolBoxes/ToolBoxOnTheRight", true).toBool() ) {

    // Set relative sizes of windows
    wsizes[0] = 480;
    wsizes[1] = 240;
    toolSplitter_->setSizes(wsizes);  
  } else {

    // Show tool box on the left side of the main window
    toolSplitter_->insertWidget(0, toolBoxArea_);
    toolSplitter_->insertWidget(1, splitter_);

    // Default sizes are swaped when toolbox is on the left.
    wsizes[0] = 240;
    wsizes[1] = 480;
    toolSplitter_->setSizes(wsizes);
  }
  
  // ======================================================================
  // Context menu setup
  // ======================================================================

  for ( unsigned int i = 0 ; i < OpenFlipper::Options::examinerWidgets() ; ++i ) {
    connect( examiner_widgets_[i] , SIGNAL(signalCustomContextMenuRequested( const QPoint&) ) ,
             this                 , SLOT( slotCustomContextMenu( const QPoint&) ) );
  }

  contextMenu_ = new QMenu(this);
  contextSelectionMenu_ = new QMenu(tr("Selection"),0);

  setupMenuBar();

  statusBar_->showMessage(tr("Ready"), 5000);

  registerCoreKeys();

  setExamineMode();

  setWindowIcon( OpenFlipper::Options::OpenFlipperIcon() );

  // Create stereo settings widget
  stereoSettingsWidget_ = new StereoSettingsWidget(this);
  // Make it look like a dialog
  stereoSettingsWidget_->setWindowFlags(Qt::Popup);
  // Connect combo boxes and sliders to local slots
  connect(stereoSettingsWidget_->stereoOpengl, SIGNAL(clicked()),
          this, SLOT(slotApplyStereoSettings()));
  connect(stereoSettingsWidget_->stereoAnaglyph, SIGNAL(clicked()),
          this, SLOT(slotApplyStereoSettings()));
  connect(stereoSettingsWidget_->stereoCustomAnaglyph, SIGNAL(clicked()),
          this, SLOT(slotApplyStereoSettings()));

  connect(stereoSettingsWidget_->focalDistance, SIGNAL(sliderReleased()),
          this, SLOT(slotApplyStereoSettings()));
  connect(stereoSettingsWidget_->eyeDistance, SIGNAL(editingFinished()),
          this, SLOT(slotApplyStereoSettings()));

  // Close button
  connect(stereoSettingsWidget_->closeButton, SIGNAL(clicked()),
          stereoSettingsWidget_, SLOT(hide()));


  // ======================================================================
  // Help Browser start up
  // ======================================================================

  helpWidget_ = new HelpWidget(this,homePage_,false);
  connect(this, SIGNAL(changeHelpSite(QUrl)), helpWidget_, SLOT(activateLink(QUrl)));
  helpWidget_->hide();


  // Add the core license information about used libraries
  addCoreLicenseInfo();

}


//-----------------------------------------------------------------------------


CoreWidget::~CoreWidget() {

}

//-----------------------------------------------------------------------------

/** Set viewer to Fullscreen Mode and back
  */
void
CoreWidget::toggleFullscreen() {

  bool fullScreen = OpenFlipperSettings().value("Core/Gui/fullscreen", false ).toBool();

  setFullscreen( !fullScreen );
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

  OpenFlipperSettings().setValue("Core/Gui/fullscreen", bool( windowState() & Qt::WindowFullScreen) );

  show();

  emit fullScreenChanged( _state );
}

//-----------------------------------------------------------------------------

void 
CoreWidget::showViewModeControls(bool _show) {
  // Only change if the actual setting has changed!
  if ( OpenFlipper::Options::gui() && ( !_show != OpenFlipperSettings().value("Core/Gui/TaskSwitcher/Hide",false).toBool() ) ) {
  
    // Update setting in Conf storage
    OpenFlipperSettings().setValue("Core/Gui/TaskSwitcher/Hide",!_show);
    
    // Update The Checkbox in the Menu
    AC_ShowViewModeControls_->blockSignals(true);
    AC_ShowViewModeControls_->setChecked( _show  );
    AC_ShowViewModeControls_->blockSignals(false);
    
    if ( _show ) {
      viewModeControlBox_->show();
      viewModeButton_->setVisible(true);
    } else {
      viewModeControlBox_->hide();
      viewModeButton_->setVisible(false);
    }
    
  }
  
}

//-----------------------------------------------------------------------------

/** Hide or show toolbox
  */
void
CoreWidget::toggleToolbox() {

  //toggle
  showToolbox( OpenFlipperSettings().value("Core/Gui/ToolBoxes/hidden",false).toBool() );

}

//-----------------------------------------------------------------------------

/** Hide or show toolbox
  */
void
CoreWidget::showToolbox( bool _state ) {

  //toggle
  OpenFlipperSettings().setValue("Core/Gui/ToolBoxes/hidden",!_state);

  if ( OpenFlipperSettings().value("Core/Gui/ToolBoxes/hidden",false).toBool() ){

    //hide ViewMode Selection Widget
    toolBoxArea_->setVisible(false);

  }else{
    //show last view mode
    toolBoxArea_->setVisible(true);
  }
  emit toolBoxVisChanged(_state);
}
//-----------------------------------------------------------------------------
void CoreWidget::showMenuBar( bool _state )
{
  OpenFlipperSettings().setValue("Core/Gui/MenuBar/hidden",!_state);

  if ( _state ){

    //hide ViewMode Selection Widget
    menuBar()->show();

  }else{
    //show last view mode
    menuBar()->hide();
  }
  emit menuBarVisChanged(_state);
}
//-----------------------------------------------------------------------------
/// Show or hide toolbar
void CoreWidget::showToolBar( bool _state )
{
  OpenFlipperSettings().setValue("Core/Gui/Toolbar/hidden",!_state);
  if ( !_state )
  {
    //hide main toolbar
    if ( ! mainToolbar_->isFloating() )
      mainToolbar_->hide();

    //hide viewer toolbar
    if ( ! viewerToolbar_->isFloating() )
      viewerToolbar_->hide();

    for (uint p=0; p < plugins_.size(); p++)
      for ( uint j = 0 ; j < plugins_[p].toolbars.size(); ++j ) {
        if ( ! plugins_[p].toolbars[j].second->isFloating() )
          plugins_[p].toolbars[j].second->hide();
      }
  }
  else
  {
    //show toolbars
    setViewMode( OpenFlipper::Options::currentViewMode() );
  }
  emit toolBarVisChanged(_state);
}
//-----------------------------------------------------------------------------
/// Hide or show menubar
void CoreWidget::toggleMenuBar()
{
  bool hidden = OpenFlipperSettings().value("Core/Gui/MenuBar/hidden",false).toBool();
  showMenuBar( hidden );
}

//-----------------------------------------------------------------------------
/// Hide or show current toolbar
void CoreWidget::toggleToolBar()
{
  bool hidden = OpenFlipperSettings().value("Core/Gui/Toolbar/hidden",false).toBool();
  showToolBar( hidden );
}


//=============================================================================

void CoreWidget::setToolBoxOrientationOnTheRight(bool _toolBoxRight) {
  QList<int> wsizes;
  
  // Is toolbox currently on the right hand side?
  bool currentstate = toolSplitter_->widget(1) == toolBoxArea_;
  
  if ( _toolBoxRight != currentstate ) {
    
    if(_toolBoxRight ) {
      
      // Show tool box on the right side of the main window
      toolSplitter_->insertWidget(0, splitter_);
      toolSplitter_->insertWidget(1, toolBoxArea_);
    } else {
      
      // Show tool box on the left side of the main window
      toolSplitter_->insertWidget(0, toolBoxArea_);
      toolSplitter_->insertWidget(1, splitter_);
    }
    
    // Store new setting
    OpenFlipperSettings().setValue("Core/Gui/ToolBoxes/ToolBoxOnTheRight",_toolBoxRight);
    
    // remove the windowstates definition for the toolboxes, as it changed anyway.
    QSettings windowStates(QDir::home().absolutePath() + OpenFlipper::Options::dirSeparator() + ".OpenFlipper" +
                            OpenFlipper::Options::dirSeparator() +  "WindowStates.dat", QSettings::IniFormat);
    
    windowStates.value("Core/ToolSplitter");
    
  }
  

    
  toolSplitter_->refresh();
}
      

//=============================================================================

void
CoreWidget::addRecent(QString _filename, DataType _type)
{
  //dont add objects to recentMenu while loadind Settings
  if ( OpenFlipper::Options::sceneGraphUpdatesBlocked() ) return;

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

  QStringList recentFiles = OpenFlipperSettings().value("Core/File/RecentFiles", QStringList()).toStringList();
  QStringList recentTypes = OpenFlipperSettings().value("Core/File/RecentTypes", QStringList()).toStringList();

  for (int i = 0 ; i < recentFiles.size() ; ++i ) {

    QFileInfo fi(recentFiles[i]);

    QAction* newAction = 0;

    if (fi.suffix() == "ini") {
      newAction = recentFilesMenu_->addAction(QIcon(OpenFlipper::Options::iconDirStr() + OpenFlipper::Options::dirSeparator()+"Settings-Icon.png"), recentFiles[i]);
      newAction->setData( QVariant( QString("Unknown") ) );
    } else {
      newAction = recentFilesMenu_->addAction(typeIcon( typeId(recentTypes[i]) ), recentFiles[i]);
      newAction->setData( QVariant( recentTypes[i]) );
    }

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

void CoreWidget::nextViewerLayout() {

  if ( OpenFlipperSettings().value("Core/Gui/glViewer/useMultipleViewers",true).toBool() ) {

    switch (baseLayout_->mode()) {
      case QtMultiViewLayout::SingleView:
              baseLayout_->setPrimary (PluginFunctions::activeExaminer ());
              baseLayout_->setMode(QtMultiViewLayout::DoubleView);

              // Update combo box in the toolbar
              viewerLayoutBox_->setCurrentIndex(1);
              break;
      case QtMultiViewLayout::DoubleView:
              baseLayout_->setPrimary (PluginFunctions::activeExaminer ());
              baseLayout_->setMode(QtMultiViewLayout::Grid);

              // Update combo box in the toolbar
              viewerLayoutBox_->setCurrentIndex(2);
              break;
      case QtMultiViewLayout::Grid:
              baseLayout_->setPrimary (PluginFunctions::activeExaminer ());
              baseLayout_->setMode(QtMultiViewLayout::HSplit);

              // Update combo box in the toolbar
              viewerLayoutBox_->setCurrentIndex(3);
              break;
      case QtMultiViewLayout::HSplit:
              baseLayout_->setPrimary (PluginFunctions::activeExaminer ());
              baseLayout_->setMode(QtMultiViewLayout::SingleView);

              // Update combo box in the toolbar
              viewerLayoutBox_->setCurrentIndex(0);
              break;
      }
  }
}


void
CoreWidget::setViewerLayout(int _idx) {

  if ( OpenFlipperSettings().value("Core/Gui/glViewer/useMultipleViewers",true).toBool() ) {

    switch (_idx) {
      case 0:
        baseLayout_->setPrimary (PluginFunctions::activeExaminer ());
        baseLayout_->setMode(QtMultiViewLayout::SingleView);
        break;
      case 1:
        baseLayout_->setPrimary (PluginFunctions::activeExaminer ());
        baseLayout_->setMode(QtMultiViewLayout::DoubleView);
        break;
      case 2:
        baseLayout_->setPrimary (PluginFunctions::activeExaminer ());
        baseLayout_->setMode(QtMultiViewLayout::Grid);
        break;
      case 3:
        baseLayout_->setPrimary (PluginFunctions::activeExaminer ());
        baseLayout_->setMode(QtMultiViewLayout::HSplit);
        break;
      default:
        emit log(LOGERR,tr("Requested illegal multiview mode!"));
    }

    viewerLayoutBox_->setCurrentIndex(_idx);
  }

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

    sceneGraphDialog_->setGeneratorMap(PluginFunctions::getSceneGraphGeneratorList() );
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

//-----------------------------------------------------------------------------

void
CoreWidget::startVideoCaptureDialog(){

  VideoCaptureDialog* dialog = new VideoCaptureDialog();
  dialog->setModal(false);

  connect(dialog, SIGNAL(startVideoCapture(QString,int,bool)), this, SIGNAL(startVideoCapture(QString,int,bool)) );
  connect(dialog, SIGNAL(resizeViewers(int,int)), this, SIGNAL(resizeViewers(int,int)) );
  connect(dialog, SIGNAL(resizeApplication(int,int)), this, SIGNAL(resizeApplication(int,int)) );

  dialog->show();
}

//-----------------------------------------------------------------------------

void CoreWidget::slotActivateExaminer()
{
  glViewer* examiner = dynamic_cast<glViewer*>(QObject::sender());

  if (!examiner)
    return;

  for ( uint i = 0 ; i < OpenFlipper::Options::examinerWidgets() ; ++i ) {

    if (examiner == examiner_widgets_[i])
    {
      PluginFunctions::setActiveExaminer (i);
      if (postProcessorDialog_)
        postProcessorDialog_->refresh();
      break;
    }
  }
}

//-----------------------------------------------------------------------------

/// Use native or gl painted cursor
void CoreWidget::setForceNativeCursor ( bool _state )
{
  cursorPainter_->setForceNative (_state);
}

//-----------------------------------------------------------------------------

bool CoreWidget::event( QEvent *_event )
{

	//WhatsThisClicked event for hyperlinks in 'whats this' boxes
	if( _event->type() == QEvent::WhatsThisClicked )
	{
		QWhatsThisClickedEvent *wtcEvent = static_cast<QWhatsThisClickedEvent*>(_event);
		QWhatsThis::hideText();
		this->showHelpBrowser(wtcEvent->href());
		return true;
	}

	return QMainWindow::event(_event);
}

//=============================================================================

