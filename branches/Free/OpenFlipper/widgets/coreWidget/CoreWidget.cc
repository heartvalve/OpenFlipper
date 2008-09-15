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

#define WIDGET_HEIGHT 800
#define WIDGET_WIDTH  800

//== IMPLEMENTATION ========================================================== 

/** \brief Constuctor for the Core Widget 
 * 
*/
CoreWidget::
CoreWidget( QVector<ViewMode*>& _viewModes,
            std::vector<PluginInfoT>& _plugins ) :
  QMainWindow(),
  viewModes_(_viewModes),
  dockViewMode_(0),
  viewModeButton_(0),
  viewModeMenu_(0),
  viewGroup_(0),
  examiner_widget_(0),
  splitter_(0),
  textedit_(0),
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
  textedit_ = new QTextEdit(splitter_);
  textedit_->setReadOnly(true);
  textedit_->setSizePolicy( QSizePolicy ( QSizePolicy::Preferred , QSizePolicy::Preferred ) );
  textedit_->resize( splitter_->width() ,120);
  textedit_->setLineWrapMode( QTextEdit::NoWrap );
  
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
  examiner_widget_ = new ACG::QtWidgets::QtExaminerViewer(stackedWidget_,  
                                                          "Examiner Widget", 
                                                          statusBar_ , 
                                                          &format);
  examiner_widget_->sceneGraph( PluginFunctions::getSceneGraphRootNode() );
  examiner_widget_->enablePopupMenu(false);
  stackedWidget_->addWidget(examiner_widget_);
  
  // ======================================================================
  // Setup dragging for examiner widget
  // ======================================================================
  examiner_widget_->setExternalDrag(true);
  connect( examiner_widget_, SIGNAL(startDragEvent( QMouseEvent*)),
           this, SLOT(startDrag(QMouseEvent* )));
  connect( examiner_widget_, SIGNAL(dragEnterEvent( QDragEnterEvent*)),
           this, SLOT(dragEnterEvent(QDragEnterEvent* )));
  connect( examiner_widget_, SIGNAL(dropEvent( QDropEvent*)),
           this, SLOT(dropEvent(QDropEvent* )));    
  
  // ======================================================================
  // Create main Toolbar
  // ======================================================================
  
  mainToolbar_ = new QToolBar("Main Toolbar");
  addToolBar(mainToolbar_);
  
  // ======================================================================
  // Get Toolbar from examiner and integrate it into main window
  // ======================================================================
  
  // Take control over the toolbar
  viewerToolbar_ = examiner_widget_->removeToolBar();
  addToolBar(Qt::TopToolBarArea,viewerToolbar_);
  viewerToolbar_->setParent(this);
  viewerToolbar_->setAllowedAreas(Qt::AllToolBarAreas);
  viewerToolbar_->setIconSize(QSize(20,20));
  
  // Set our own Icons
  QList<QAction *> actions = viewerToolbar_->actions();
  for ( int i = 0 ; i < actions.size(); ++i ) {
    if ( actions[i]->text() == "Move" ) {
      QIcon icon;
      icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"transform-move.png");
      
      QToolButton * button = qobject_cast<QToolButton *>( viewerToolbar_->widgetForAction ( actions[i] ) );
      button->setIcon(icon);
    } else if ( actions[i]->text() == "Home" ) {
      QIcon icon;
      icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"go-home.png");
      
      QToolButton * button = qobject_cast<QToolButton *>( viewerToolbar_->widgetForAction ( actions[i] ) );
      button->setIcon(icon);
    } else if ( actions[i]->text() == "Set Home" ) {
      QIcon icon;
      icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"set-home.png");
      
      QToolButton * button = qobject_cast<QToolButton *>( viewerToolbar_->widgetForAction ( actions[i] ) );
      button->setIcon(icon);
    } else if ( actions[i]->text() == "Light" ) {
      QIcon icon;
      icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"ktip.png");
      
      QToolButton * button = qobject_cast<QToolButton *>( viewerToolbar_->widgetForAction ( actions[i] ) );
      button->setIcon(icon);
    } else if ( actions[i]->text() == "Question" ) {
      QIcon icon;
      icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"help-about.png");
      
      QToolButton * button = qobject_cast<QToolButton *>( viewerToolbar_->widgetForAction ( actions[i] ) );
      button->setIcon(icon);
    } else if ( actions[i]->text() == "SceneGraph" ) {
      QIcon icon;
      icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"scenegraph.png");
      
      QToolButton * button = qobject_cast<QToolButton *>( viewerToolbar_->widgetForAction ( actions[i] ) );
      button->setIcon(icon);
    } else if ( actions[i]->text() == "Pick" ) {
//       QIcon icon;
//       icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"scenegraph.png");
//       
//       QToolButton * button = qobject_cast<QToolButton *>( viewerToolbar_->widgetForAction ( actions[i] ) );
//       button->setIcon(icon);
    } else if ( actions[i]->text() == "View All" ) {
//       QIcon icon;
//       icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"scenegraph.png");
//       
//       QToolButton * button = qobject_cast<QToolButton *>( viewerToolbar_->widgetForAction ( actions[i] ) );
//       button->setIcon(icon);
    } else if ( actions[i]->text() == "Projection" ) {
//       QIcon icon;
//       icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"scenegraph.png");
//       
//       QToolButton * button = qobject_cast<QToolButton *>( viewerToolbar_->widgetForAction ( actions[i] ) );
//       button->setIcon(icon);
    } else if ( actions[i]->text() == "Stereo" ) {
//       QIcon icon;
//       icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"scenegraph.png");
//       
//       QToolButton * button = qobject_cast<QToolButton *>( viewerToolbar_->widgetForAction ( actions[i] ) );
//       button->setIcon(icon);
    }
    
//     else 
//       std::cerr << actions[i]->text().toStdString() << std::endl; 
  }
    
  stackWidgetList_.push_back( StackWidgetInfo( false, "3D Examiner Widget", examiner_widget_ ) );
  
  // Remember logger size
  wsizes = splitter_->sizes();
  originalLoggerSize_  = wsizes[1];
  
  // ======================================================================
  // Create Upper DockWidget for ToolWidget control
  // ======================================================================  
  dockViewMode_ = new QDockWidget("ViewMode" , this );
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
  
  connect( examiner_widget_ , SIGNAL(signalCustomContextMenuRequested( const QPoint&) ) , 
           this             , SLOT( slotCustomContextMenu( const QPoint&) ) );
  
  contextMenu_ = new QMenu(0);
  contextSelectionMenu_ = new QMenu("Selection",0);
  
  setupMenuBar();
  
  updateRecent();
  
  statusBar_->showMessage("Ready", 5000);
}


//-----------------------------------------------------------------------------


CoreWidget::~CoreWidget() {
  
}

//-----------------------------------------------------------------------------

/** Set viewer to Fullscreen Mode and back
  */
void
CoreWidget::toggleFullscreen() {
  setWindowFlags( windowFlags () ^ Qt::FramelessWindowHint) ;
  OpenFlipper::Options::fullScreen( bool( windowFlags () | Qt::FramelessWindowHint) );
  resize(QApplication::desktop()->size());
  show();
}

//-----------------------------------------------------------------------------

/** Hide or show logger
  */
void
CoreWidget::toggleLogger() {
  // toggle
  OpenFlipper::Options::hideLogger( !OpenFlipper::Options::hideLogger() );
      
  //Hide Logger
  if ( OpenFlipper::Options::hideLogger() ) {
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
  OpenFlipper::Options::hideToolbox(!OpenFlipper::Options::hideToolbox());

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
  
//-----------------------------------------------------------------------------
 
/** Handles keypress events or passes them to plugins
  */
void
CoreWidget::keyPressEvent(QKeyEvent* _e)
{
  if (_e->modifiers() == Qt::ControlModifier ) {
    switch (_e->key()) 
    {
        case Qt::Key_F :
            toggleFullscreen();
          return;
          
        case Qt::Key_L :
            toggleLogger();
          return;
          
        case Qt::Key_T :
            toggleToolbox();
          return;
          
        // Send remaining events to plugins
        default:  
           emit PluginKeyEvent(_e);
        return;
    }  
  }
  
   switch (_e->key()) 
   {
      // Send remaining events to plugins
      default:
          emit PluginKeyEvent(_e);
      break;
  }
}

//-----------------------------------------------------------------------------

/** Handle Key Release Events */
void 
CoreWidget::keyReleaseEvent(QKeyEvent* _e) {
   emit PluginKeyReleaseEvent(_e);
}

//=============================================================================

void 
CoreWidget::addRecent(QString _filename, DataType _type) 
{
  //dont add objects within INI files to recentMenu
  if ( OpenFlipper::Options::openingIni() ) return; 
  
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
    switch(recentFiles[i].type){
      case DATA_TRIANGLE_MESH: 
        path += "TriangleType.png"; break;
      case DATA_POLY_MESH: 
        path += "PolyType.png"; break;
      case DATA_POLY_LINE: 
        path += "PolyLineType.png"; break;
      case DATA_BSPLINE_CURVE: 
        path += "BSplineCurveType.png"; break;
      default: path = "";
    }
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
    optionsWidget_ = new OptionsWidget(0);
    connect(optionsWidget_,SIGNAL(applyOptions()),this,SIGNAL(applyOptions()));
    connect(optionsWidget_,SIGNAL(saveOptions()),this,SIGNAL(saveOptions()));
  }

  //show the optionsWidget centered  
  QPoint center;
  center.setX( x() + width() / 2 ); 
  center.setY( y() + height() / 2 );

  optionsWidget_->setGeometry(center.x() - optionsWidget_->width() / 2, center.y() - optionsWidget_->height() / 2, optionsWidget_->width(), optionsWidget_->height());

  optionsWidget_->show();
    
}
 

//=============================================================================
