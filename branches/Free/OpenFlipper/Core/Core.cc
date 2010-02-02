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
 *   $Revision$                                                       *
 *   $Author$                                                      *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/





//=============================================================================
//
//  CLASS Core - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================

// -------------------- mview
#include "Core.hh"
// -------------------- ACG
#include <ACG/Scenegraph/DrawModes.hh>
#include <ACG/Scenegraph/SceneGraph.hh>

#include <ACG/QtWidgets/QtFileDialog.hh>
// -------------------- Qt

#include <QKeyEvent>
#include <QSplitter>
#include <QMenuBar>
#include <QToolBox>
#include <QApplication>
#include <QStatusBar>
#include <QMessageBox>
#include <QFile>
#include <QSettings>

#include <QPluginLoader>
#include "OpenFlipper/BasePlugin/BaseInterface.hh"
#include "OpenFlipper/BasePlugin/KeyInterface.hh"
#include "OpenFlipper/BasePlugin/MouseInterface.hh"
#include "OpenFlipper/BasePlugin/PickingInterface.hh"
#include "OpenFlipper/BasePlugin/ToolboxInterface.hh"
#include "OpenFlipper/BasePlugin/TextureInterface.hh"
#include "OpenFlipper/BasePlugin/MenuInterface.hh"
#include "OpenFlipper/BasePlugin/INIInterface.hh"

#include "OpenFlipper/INIFile/INIFile.hh"

#include "OpenFlipper/common/GlobalOptions.hh"
#include <OpenFlipper/common/RecentFiles.hh>
#include <OpenFlipper/ACGHelper/DrawModeConverter.hh>

#include <QStringList>
#include <QtScript/QScriptValueIterator>

#include <ACG/Scenegraph/SeparatorNode.hh>

#include "OpenFlipper/BasePlugin/PluginFunctions.hh"
#include "OpenFlipper/BasePlugin/PluginFunctionsCore.hh"

#include "OpenFlipper/BasePlugin/RPCWrappers.hh"

#include <OpenMesh/Core/System/omstream.hh>

#include <OpenFlipper/common/BaseObjectCore.hh>
#include <OpenFlipper/common/TypesInternal.hh>

#define WIDGET_HEIGHT 800
#define WIDGET_WIDTH  800

//== IMPLEMENTATION ==========================================================

/** \brief Constuctor for the Core Widget ( This is stage 1 , call init for stage 2)
 *
 * Initialization is working the following way:\n
 * - Setup basic paths \n
 * - Get Options from Option files ( While skipping the OpenFiles Sections as Core is not running )\n
 * - Jump back ( Stage two is done by calling init from main ) so CALL init!!
 * - This Two stage system allows using commandline options which override Option Files
*/
Core::
Core() :
  QObject(),
  capture_(false),
  nextBackupId_(0),
  objectRoot_(0),
  coreWidget_(0)
{

  //init logFile
  logStream_ = 0;
  logFile_ = 0;

  //init nodes
  root_node_scenegraph_ = new ACG::SceneGraph::SeparatorNode(0, "SceneGraph Root Node");
  
  // This seperator will manage the cores nodes
  core_nodes_ = new ACG::SceneGraph::SeparatorNode(root_node_scenegraph_, "Core Nodes");
  
  // Coordsys rendering nodes
  coordsysMaterialNode_ = new ACG::SceneGraph::MaterialNode(core_nodes_,"Coordsys Material Node");
  coordsysNode_ = new ACG::SceneGraph::CoordsysNode(coordsysMaterialNode_,"Core Coordsys Node");  
  coordsysNode_->setTraverseMode (BaseNode::NodeFirst | BaseNode::SecondPass);
  
  // seperator handling the nodes for data
  dataSeparatorNode_ = new ACG::SceneGraph::SeparatorNode(root_node_scenegraph_, "Data Separator Root Node");
  
  // seperator handling the nodes for data
  dataRootNode_      = new ACG::SceneGraph::SeparatorNode(dataSeparatorNode_, "Data Root Node");
  
  
//   gridNode_ = new ACG::SceneGraph::GridNode(core_nodes_,"Grid Node");
//   gridNode_->hide();

   // Add ViewMode All
  ViewMode* vm = new ViewMode();
  vm->name = "All";
  vm->icon = "viewmode_all.png";
  vm->custom = false;
  vm->visibleToolboxes = QStringList();

  viewModes_.push_front(vm);

  //init ViewerProperties (always for 4 Viewers!)
  std::vector< Viewer::ViewerProperties* > viewerProperties;

  for (int i=0; i < 4; i++) {
    Viewer::ViewerProperties* viewerProperty = new Viewer::ViewerProperties();
    viewerProperty->snapshotBaseFileName("snap-Viewer-" + QString::number(i) + ".png");
    viewerProperties.push_back( viewerProperty );
  }

  PluginFunctions::setViewerProperties(viewerProperties);

  //set viewer defaults
  OpenFlipper::Options::defaultProjectionMode( 1, 0 );
  OpenFlipper::Options::defaultProjectionMode( 0, 1 );
  OpenFlipper::Options::defaultProjectionMode( 0, 2 );
  OpenFlipper::Options::defaultProjectionMode( 0, 3 );
  OpenFlipper::Options::defaultViewingDirection( PluginFunctions::VIEW_FREE,  0 );
  OpenFlipper::Options::defaultViewingDirection( PluginFunctions::VIEW_TOP,   1 );
  OpenFlipper::Options::defaultViewingDirection( PluginFunctions::VIEW_LEFT,  2 );
  OpenFlipper::Options::defaultViewingDirection( PluginFunctions::VIEW_FRONT, 3 );

  // Get all relevant Paths and Options from option files
  setupOptions();

  // set discriptions for scriptable slots
  setDescriptions();
  
  // Initialize the build in dataTypes
  initializeTypes();
}

/** \brief Second initialization stage
 *
 * This Stage does the following :\n
 * - Create the Core GUI Elements (Examiner, Toolbox,...)\n
 * - Create the MenuBar \n
 * - load the Plugins \n
 * - connect the Mouse slots \n
 * - Load all ini files (This includes the Global Option files) \n
 */
void
Core::init() {

  // Make root_node available to the plugins ( defined in PluginFunctions.hh)
  PluginFunctions::setDataSeparatorNodes( dataSeparatorNode_ );

  PluginFunctions::setSceneGraphRootNode( root_node_scenegraph_ );

  // Initialize the first object as the root Object for the object tree
  objectRoot_ =  dynamic_cast< BaseObject* > ( new GroupObject("ObjectRoot") );
  PluginFunctions::setDataRoot( objectRoot_ );
  
  // Bring up the object manager ( has to be done after the rootobject is created)
  connect(getObjectManager(),SIGNAL(newObject(int)), this ,SLOT(newObject(int)));

  if ( OpenFlipper::Options::gui() ) {

    // Initialize redraw timer. Will be used to restrict the rendering framerate.
    redrawTimer_ = new QTimer();
    redrawTimer_->setSingleShot(true);
    connect(redrawTimer_, SIGNAL(timeout()), this, SLOT(updateView()),Qt::DirectConnection);

    // Initialice scenegraph check timer. Will be used to check for changes in the scenegraph
    scenegraphCheckTimer_ = new QTimer();
    scenegraphCheckTimer_->setSingleShot(false);
    connect(scenegraphCheckTimer_, SIGNAL(timeout()), this, SLOT(checkScenegraphDirty()),Qt::DirectConnection);

    // Will measure the time between redraws
    redrawTime_ = new QTime();
    redrawTime_->start ();

    if ( OpenFlipperSettings().value("Core/Gui/splash",true).toBool() ) {
      QPixmap splashPixmap(OpenFlipper::Options::iconDirStr() + OpenFlipper::Options::dirSeparator() + "splash.png");

      splash_ = new QSplashScreen(splashPixmap);
      splash_->show();

      splash_->showMessage(tr("Initializing mainwindow") ,
                          Qt::AlignBottom | Qt::AlignLeft , Qt::white);
      QApplication::processEvents();
    }

    coreWidget_ = new CoreWidget(viewModes_ , plugins, coreSlots_);

    connect(coreWidget_, SIGNAL(clearAll())           , this, SLOT(clearAll()));
    connect(coreWidget_, SIGNAL(loadMenu())           , this, SLOT(loadObject()));
    connect(coreWidget_, SIGNAL(addEmptyObjectMenu()) , this, SLOT(slotAddEmptyObjectMenu()));
    connect(coreWidget_, SIGNAL(saveMenu())           , this, SLOT(saveAllObjects()));
    connect(coreWidget_, SIGNAL(saveToMenu())         , this, SLOT(saveAllObjectsTo()));
    connect(coreWidget_, SIGNAL(loadIniMenu())        , this, SLOT(loadSettings()));
    connect(coreWidget_, SIGNAL(saveIniMenu())        , this, SLOT(saveSettings()));
    connect(coreWidget_, SIGNAL(applyOptions())       , this, SLOT(applyOptions()));
    connect(coreWidget_, SIGNAL(saveOptions())        , this, SLOT(saveOptions()));
    connect(coreWidget_, SIGNAL(recentOpen(QAction*)) , this, SLOT(slotRecentOpen(QAction*)));
    connect(coreWidget_, SIGNAL(exit())               , this, SLOT(slotExit()));


    connect( coreWidget_, SIGNAL( resizeViewers(int,int) ), this, SLOT( resizeViewers(int,int) ) );
    connect( coreWidget_, SIGNAL( resizeApplication(int,int) ), this, SLOT( resizeApplication(int,int) ) );
    connect( coreWidget_, SIGNAL( stopVideoCapture() ), this, SLOT( stopVideoCapture() ) );
    connect( coreWidget_, SIGNAL( startVideoCapture(QString,int,bool) ), this, SLOT( startVideoCapture(QString,int,bool) ) );
    connect( coreWidget_, SIGNAL( dragOpenFile(QString)), this, SLOT(loadObject(QString)));

    connect(coreWidget_, SIGNAL(showPlugins())       , this, SLOT(slotShowPlugins()));

    connect(coreWidget_, SIGNAL(call(QString,bool&)), this, SLOT(slotCall(QString,bool&)));

    QRect rect = QApplication::desktop()->screenGeometry();

    uint width = rect.width();
    if ( width > 1000 ) {
      width = 1000;
    }

    uint height = rect.height();
    if ( height > 1000 ) {
      height = 1000;
    }

#ifdef ARCH_DARWIN
    width = rect.width() - 300;
    height = rect.height() - 150;

//     coreWidget_->setMaximumSize( width, height  );
#endif

    coreWidget_->resize(width,height);

    coreWidget_->setWindowTitle( OpenFlipper::Options::windowTitle() );

  }

  // ======================================================================
  // Create intermediate logger class for Core which will mangle the output
  // ======================================================================
  PluginLogger* newlog = new PluginLogger("Core");

  loggers_.push_back(newlog);
  connect(this,SIGNAL(log(Logtype, QString )),newlog,SLOT(slotLog(Logtype, QString )),Qt::DirectConnection);
  connect(this,SIGNAL(log(QString )),newlog,SLOT(slotLog(QString )),Qt::DirectConnection);

  // Connect it to the Master logger
  if ( OpenFlipper::Options::gui() )
    connect(newlog,SIGNAL(log(Logtype, QString )),coreWidget_,SLOT(slotLog(Logtype, QString )),Qt::DirectConnection);

  connect(newlog,SIGNAL(log(Logtype, QString )),this,SLOT(slotLog(Logtype, QString )),Qt::DirectConnection);

  // connection to file logger
  connect(newlog,SIGNAL(log(Logtype, QString )),this,SLOT(slotLogToFile(Logtype, QString )),Qt::DirectConnection);

  // ======================================================================
  // Create a logger class for CoreWidget
  // ======================================================================

  if ( OpenFlipper::Options::gui() ){
    PluginLogger* widgetlog = new PluginLogger("CoreWidget");

    loggers_.push_back(widgetlog);
    connect(coreWidget_,SIGNAL(log(Logtype, QString )),widgetlog,SLOT(slotLog(Logtype, QString )),Qt::DirectConnection);
    connect(coreWidget_,SIGNAL(log(QString )),widgetlog,SLOT(slotLog(QString )),Qt::DirectConnection);

    // Connect it to the Master logger
    connect(widgetlog,SIGNAL(log(Logtype, QString )),coreWidget_,SLOT(slotLog(Logtype, QString )),Qt::DirectConnection);
    connect(widgetlog,SIGNAL(log(Logtype, QString )),this,SLOT(slotLog(Logtype, QString )),Qt::DirectConnection);
    // connection to file logger
    connect(widgetlog,SIGNAL(log(Logtype, QString )),this,SLOT(slotLogToFile(Logtype, QString )),Qt::DirectConnection);
  }

  // ======================================================================
  // Catch OpenMesh Error logs with an own Logger
  // ======================================================================
  newlog = new PluginLogger("Core ( OpenMesh )",LOGERR);
  omerr().connect(*newlog);
  omerr().disconnect(std::cerr);

  loggers_.push_back(newlog);

  // Connect it to the Master logger
  if ( OpenFlipper::Options::gui() )
    connect(newlog,SIGNAL(log(Logtype, QString )),coreWidget_,SLOT(slotLog(Logtype, QString )),Qt::DirectConnection);

  connect(newlog,SIGNAL(log(Logtype, QString )),this,SLOT(slotLog(Logtype, QString )),Qt::DirectConnection);
  // connection to file logger
  connect(newlog,SIGNAL(log(Logtype, QString )),this,SLOT(slotLogToFile(Logtype, QString )),Qt::DirectConnection);

  // ======================================================================
  // Catch OpenMesh omout logs with an own Logger
  // ======================================================================
  newlog = new PluginLogger("Core ( OpenMesh )",LOGINFO);
  omout().connect(*newlog);
  omout().disconnect(std::cout);

  loggers_.push_back(newlog);

  // Connect it to the Master logger
  if ( OpenFlipper::Options::gui() )
    connect(newlog,SIGNAL(log(Logtype, QString )),coreWidget_,SLOT(slotLog(Logtype, QString )),Qt::DirectConnection);

  connect(newlog,SIGNAL(log(Logtype, QString )),this,SLOT(slotLog(Logtype, QString )),Qt::DirectConnection);
  // connection to file logger
  connect(newlog,SIGNAL(log(Logtype, QString )),this,SLOT(slotLogToFile(Logtype, QString )),Qt::DirectConnection);

  // ======================================================================
  // Catch OpenMesh omlog logs with an own Logger
  // ======================================================================
  newlog = new PluginLogger("Core ( OpenMesh )",LOGOUT);
  omlog().connect(*newlog);

  loggers_.push_back(newlog);

  // Connect it to the Master logger
  if ( OpenFlipper::Options::gui() )
    connect(newlog,SIGNAL(log(Logtype, QString )),coreWidget_,SLOT(slotLog(Logtype, QString )),Qt::DirectConnection);

  connect(newlog,SIGNAL(log(Logtype, QString )),this,SLOT(slotLog(Logtype, QString )),Qt::DirectConnection);
  // connection to file logger
  connect(newlog,SIGNAL(log(Logtype, QString )),this,SLOT(slotLogToFile(Logtype, QString )),Qt::DirectConnection);

  // ======================================================================
  // Log Scripting stuff through a separate logger
  // ======================================================================
  newlog = new PluginLogger("Scripting",LOGOUT);

  loggers_.push_back(newlog);

  // Connect it to the Master logger
  if ( OpenFlipper::Options::gui() )
    connect(newlog,SIGNAL(log(Logtype, QString )),coreWidget_,SLOT(slotLog(Logtype, QString )),Qt::DirectConnection);

  connect(newlog,SIGNAL(log(Logtype, QString )),this,SLOT(slotLog(Logtype, QString )),Qt::DirectConnection);
  // connection to file logger
  connect(newlog,SIGNAL(log(Logtype, QString )),this,SLOT(slotLogToFile(Logtype, QString )),Qt::DirectConnection);

  // connect signal to logger
  connect(this,SIGNAL(scriptLog(QString )),newlog,SLOT(slotLog(QString )),Qt::DirectConnection);

  
  // ======================================================================
  // Set up QtScript Environment
  // ======================================================================
  
  // Set a reference to the scriptengine for simple rpc calls
  RPC::setScriptEngine(&scriptEngine_);

  // process Events every 500 msecs during script execution
  scriptEngine_.setProcessEventsInterval( 500 );

  // Register own print function :
  QScriptValue printFunction = scriptEngine_.newFunction(myPrintFunction);
  printFunction.setProperty("textedit",scriptEngine_.newQObject(this));
  scriptEngine_.globalObject().setProperty("print", printFunction);

  // Register Vector Type to ScriptEngine ( is Vec3d )
  qScriptRegisterMetaType(&scriptEngine_,
                          toScriptValueVector,
                          fromScriptValueVector,
                          scriptEngine_.newQObject(&vec3dPrototype_));

  // set a constructor to allow creation via Vector(x,y,z)
  QScriptValue ctor = scriptEngine_.newFunction(createVector);
  scriptEngine_.globalObject().setProperty("Vector", ctor);


//    // Register ObjectId Type to ScriptEngine ( is int )
//   qScriptRegisterMetaType(&scriptEngine_,
//                           toScriptValueObjectId,
//                           fromScriptValueObjectId);
//
//   // set a constructor to allow creation via Vector(x,y,z)
//   ctor = scriptEngine_.newFunction(createObjectId);
//   scriptEngine_.globalObject().setProperty("ObjectId", ctor);


  // Register IdList Type to scripting Engine
  qScriptRegisterSequenceMetaType< IdList >(&scriptEngine_);

  qScriptRegisterSequenceMetaType< QVector< int > >(&scriptEngine_);


  // Register Matrix Type to scripting Engine ( is ACG::Matrix4x4d )
  qScriptRegisterMetaType(&scriptEngine_,
                          toScriptValueMatrix4x4 ,
                          fromScriptValueMatrix4x4,
                          scriptEngine_.newQObject(&matrix4x4Prototype_));

  // set a constructor to allow creation via Matrix(x,y,z)
  QScriptValue matrix4x4ctor = scriptEngine_.newFunction(createMatrix4x4);
  scriptEngine_.globalObject().setProperty("Matrix4x4", matrix4x4ctor);

  // Collect Core scripting information
  QScriptValue scriptInstance = scriptEngine_.newQObject(this,
                                                         QScriptEngine::QtOwnership,
                                                         QScriptEngine::ExcludeChildObjects |
                                                         QScriptEngine::ExcludeSuperClassMethods |
                                                         QScriptEngine::ExcludeSuperClassProperties
                                                         );

  scriptEngine_.globalObject().setProperty("core", scriptInstance);

  QScriptValueIterator it(scriptInstance);
  while (it.hasNext()) {
    it.next();

    /// Skip all signals for function calls
    if ( checkSignal( this, it.name().toAscii() ) )
      continue;

    scriptingFunctions_.push_back( "core." + it.name() );

  }

  loadPlugins();

  if ( OpenFlipper::Options::gui() ) {

    //register keyBinding for all scripting slots
    coreWidget_->slotRegisterSlotKeyBindings();

    //get keyAssignments from config files
    restoreKeyBindings();

    if ( OpenFlipper::Options::defaultToolboxMode( ) != "" )
      coreWidget_->setViewMode( OpenFlipper::Options::defaultToolboxMode() );
    else
      coreWidget_->setViewMode("All");

    for ( uint i = 0 ; i < OpenFlipper::Options::examinerWidgets(); ++i ) {
      connect( coreWidget_->examiner_widgets_[i], SIGNAL(signalMouseEvent(QMouseEvent*)),
              this                              , SLOT(slotMouseEvent(QMouseEvent*)));
      connect( coreWidget_->examiner_widgets_[i], SIGNAL(signalMouseEventIdentify(QMouseEvent*)),
              this                              , SLOT(slotMouseEventIdentify(QMouseEvent*)));
      connect( coreWidget_->examiner_widgets_[i], SIGNAL(signalWheelEvent(QWheelEvent *, const std::string &)),
              this                              , SLOT(slotWheelEvent(QWheelEvent *, const std::string &)));

      connect( coreWidget_->examiner_widgets_[i], SIGNAL( viewUpdated() ),
               this, SLOT( viewUpdated()) ,Qt::DirectConnection);

      connect( coreWidget_->examiner_widgets_[i], SIGNAL( viewChanged() ),
               this, SIGNAL( pluginViewChanged() ) ,Qt::DirectConnection);
    }

  }

  // ===============================================================================================
  // Load Settings from configuration files
  // ===============================================================================================

  QStringList optionFiles = OpenFlipper::Options::optionFiles();
  for ( int i = 0 ; i < (int)optionFiles.size(); ++i) {

    if ( OpenFlipper::Options::gui() && OpenFlipperSettings().value("Core/Gui/splash",true).toBool() ) {
      splash_->showMessage(tr("Loading Configuration File ") + QString::number(i) + "/"  + QString::number(optionFiles.size()) ,
                           Qt::AlignBottom | Qt::AlignLeft , Qt::white);
      QApplication::processEvents();
    }

    // Load global ini files. Use only plugin global options from these files as the
    // rest has been loaded at the beginning.
    readRecentFiles( optionFiles[i] );
    if ( OpenFlipper::Options::gui() )
      coreWidget_->updateRecent();

    openIniFile( optionFiles[i] ,false,true,false);
  }

  // ===============================================================================================
  // Load Settings from configuration files
  // ===============================================================================================

  if ( OpenFlipper::Options::lang().contains("UTF") || OpenFlipper::Options::lang().contains("utf") ) {
    emit log(LOGWARN,tr("UTF8-Locale used!"));
//     emit log(LOGWARN,"Only OFF files are fully supported with UTF8. Others might fail.");
//     emit log(LOGWARN,"You can change your locale by :");
//     emit log(LOGWARN,"export LANG=C");
//     emit log(LOGWARN,"Work is in progress to resolve this issue.");
  }

  if ( OpenFlipper::Options::gui() ) {

    QSettings windowStates(QDir::home().absolutePath() + OpenFlipper::Options::dirSeparator() + ".OpenFlipper" +
                           OpenFlipper::Options::dirSeparator() +  "WindowStates.dat", QSettings::IniFormat);

    //try to restore the windowState
    coreWidget_->restoreState (windowStates.value("Core/Window/State").toByteArray ());
    //try to restore the geometry
    coreWidget_->restoreGeometry (windowStates.value("Core/Window/Geometry").toByteArray ());

    coreWidget_->toolSplitter_->restoreState (windowStates.value("Core/ToolSplitter").toByteArray ());
    coreWidget_->splitter_->restoreState (windowStates.value("Core/LogSplitter").toByteArray ());

    coreWidget_->show();

    applyOptions();

    windowStates.beginGroup ("Core");
    windowStates.beginGroup ("LogSlider");
    coreWidget_->slidingLogger_->restoreState (windowStates);
    windowStates.endGroup ();
    coreWidget_->toolBox_->restoreState (windowStates);
    windowStates.endGroup ();

    if ( OpenFlipperSettings().value("Core/Gui/splash",true).toBool() ) {
      splash_->finish(coreWidget_);
    }

    // start checking for scenegraph changes
    scenegraphCheckTimer_->setInterval (1000 / OpenFlipper::Options::maxFrameRate());
    scenegraphCheckTimer_->start ();
  }


  QTimer::singleShot(100, this, SLOT(slotExecuteAfterStartup()));
}


//-----------------------------------------------------------------------------

Core::~Core()
{
   for ( uint i = 0 ; i < plugins.size() ; ++i ){
     BaseInterface* basePlugin = qobject_cast< BaseInterface * >(plugins[i].plugin);

     // Dont call exit if we cannot get the Plugin
     if ( basePlugin )
       if ( checkSlot( plugins[i].plugin , "exit()" ) )
          QMetaObject::invokeMethod(plugins[i].plugin, "exit",  Qt::DirectConnection);
  }

  // Delete the objectRoot if it was constructed
  if ( objectRoot_ != 0 ) {
    objectRoot_->deleteSubtree();
    delete objectRoot_;
  }

  // Clean up loggers
  for ( uint i = 0 ; i < loggers_.size(); ++i )
    delete loggers_[i];

}

//-----------------------------------------------------------------------------

void
Core::slotMouseEventIdentify( QMouseEvent* _event )
{
//   // Dont do anything as a context Menu will popup on right button click
//   if ( _event->button() == Qt::RightButton )
//     return;

  const QObject* senderPointer = sender();
  unsigned int examinerId = 0;

  if ( senderPointer == 0 ) {
    std::cerr << "Error : slotMouseEventIdentify directly called! This should only be called by an examiner" << std::endl;
  } else {
    for ( unsigned int i = 0 ; i < OpenFlipper::Options::examinerWidgets(); ++i ) {
      if ( senderPointer == coreWidget_->examiner_widgets_[i] ) {
        examinerId = i;
        if ( OpenFlipper::Options::doSlotDebugging() )
              emit log(LOGINFO,tr("slotMouseEventIdentify from examiner ") + QString::number(i) );
        break;
      }
    }

  }

  PluginFunctions::setActiveExaminer( examinerId );

  emit PluginMouseEventIdentify( _event );


}

//-----------------------------------------------------------------------------


void
Core::slotMouseEvent( QMouseEvent* _event )
{
//   // Dont do anything as a context Menu will popup on right button click
//   if ( _event->button() == Qt::RightButton )
//     return;

  const QObject* senderPointer = sender();
  unsigned int examinerId = 0;

  if ( senderPointer == 0 ) {
    std::cerr << "Error : slotMouseEvent directly called! This should only be called by an examiner" << std::endl;
  } else {
    for ( unsigned int i = 0 ; i < OpenFlipper::Options::examinerWidgets(); ++i ) {
      if ( senderPointer == coreWidget_->examiner_widgets_[i] ) {
        examinerId = i;
        if ( OpenFlipper::Options::doSlotDebugging() ) {
          QString message = tr("slotMouseEvent from examiner ") + QString::number(i) + " with ";

          if ( _event->type() == QEvent::MouseButtonRelease )
            message += " MouseButtonRelease";
          else if ( _event->type() == QEvent::MouseButtonPress )
            message += " MouseButtonRelease";
          else if ( _event->type() == QEvent::MouseButtonDblClick )
            message += " MouseButtonDblClick";
          else if ( _event->type() == QEvent::MouseMove )
            message += " MouseMove";
          else
            message += tr("unknown event type");

          emit log(LOGINFO,message );
        }

        break;

      }
    }

  }

  PluginFunctions::setActiveExaminer( examinerId );

  emit PluginMouseEvent(_event );
}

//-----------------------------------------------------------------------------

void
Core::slotWheelEvent( QWheelEvent * _event, const std::string & _mode)
{
  const QObject* senderPointer = sender();
  unsigned int examinerId = 0;

  if ( senderPointer == 0 ) {
    std::cerr << "Error : slotWheelEvent directly called! This should only be called by an examiner" << std::endl;
  } else {
    for ( unsigned int i = 0 ; i < OpenFlipper::Options::examinerWidgets(); ++i ) {
      if ( senderPointer == coreWidget_->examiner_widgets_[i] ) {
        examinerId = i;
        break;
      }
    }

  }

  PluginFunctions::setActiveExaminer( examinerId );

  emit PluginWheelEvent(_event , _mode );
}

//-----------------------------------------------------------------------------

void
Core::slotAddPickMode( const std::string _mode ) {
  if ( OpenFlipper::Options::gui() )
    coreWidget_->addPickMode(_mode);

}

//-----------------------------------------------------------------------------

void
Core::slotAddHiddenPickMode( const std::string _mode ) {

  if ( OpenFlipper::Options::gui() )
    coreWidget_->addPickMode(_mode,false,1000,false);

}

//-----------------------------------------------------------------------------

 /** Update the view in the examiner widget
  */
void Core::updateView() {

  if ( OpenFlipper::Options::doSlotDebugging() ) {
    if ( sender() != 0 ) {
      if ( sender()->metaObject() != 0 ) {
        emit log(LOGINFO,tr("updateView() called by ") + QString( sender()->metaObject()->className() ) );
      }
    }
  }

  if ( !OpenFlipper::Options::gui() )
    return;

  if ( OpenFlipper::Options::restrictFrameRate() ) {

    int elapsed = redrawTime_->elapsed ();

    if ( elapsed < 1000 / OpenFlipper::Options::maxFrameRate() )
    {
      // redraw time not reached ... waiting for timer event for next redraw
      if ( redrawTimer_->isActive() ) {
        if ( OpenFlipper::Options::doSlotDebugging() )
          emit log(LOGINFO,tr("Too early for redraw! Delaying request from ") +
                           QString( sender()->metaObject()->className() ) );
        return;
      }

      // Start the timer
      redrawTimer_->start( (1000 / OpenFlipper::Options::maxFrameRate()) - elapsed);
      return;
    }
    else if ( redrawTimer_->isActive() )
	redrawTimer_->stop ();

  }

  redrawTime_->restart ();

  if ( !OpenFlipper::Options::loadingSettings() && !OpenFlipper::Options::redrawDisabled() ) {

    for ( unsigned int i = 0 ; i < OpenFlipper::Options::examinerWidgets() ; ++i )
      coreWidget_->examiner_widgets_[i]->updateGL();
  }
}



//-----------------------------------------------------------------------------

 /** Check if scenegraph is dirty and initiate redraw
  */
void Core::checkScenegraphDirty() {
  if ( true )
  {
    // This is a single pass traversal as we only need to check if there is still one node dirty in the graph
    ACG::SceneGraph::CheckDirtyAction action;
    ACG::SceneGraph::traverse( root_node_scenegraph_, action );
    
    // If the scenegraph is dirty, we have to redraw
    if ( action.isDirty () )
      emit updateView ();
  }
}

//-----------------------------------------------------------------------------

void Core::restrictFrameRate( bool _enable ) {
  OpenFlipper::Options::restrictFrameRate( _enable );
}

//-----------------------------------------------------------------------------

void Core::setMaxFrameRate( int _rate ) {
  OpenFlipper::Options::maxFrameRate( _rate );
  OpenFlipper::Options::restrictFrameRate( true );

  // update Timer to new framerate
  scenegraphCheckTimer_->setInterval (1000 / OpenFlipper::Options::maxFrameRate());
}

//-----------------------------------------------------------------------------

void
Core::clearAll()
{

  slotDeleteAllObjects();

  emit allCleared();

  slotScriptInfo( "core" , "clearAll()"  );
}

//-----------------------------------------------------------------------------

void
Core::exitApplication()
{
  QTimer* timer = new QTimer();
  connect(timer, SIGNAL(timeout()), this, SLOT(slotExit()));
  timer->start(100);

  QApplication::quit();
}

//-----------------------------------------------------------------------------

void Core::fullscreen( bool _state ) {
  if ( OpenFlipper::Options::gui() )
    coreWidget_->setFullscreen(_state);
}

//-----------------------------------------------------------------------------

void Core::loggerState(int _state) {
  OpenFlipper::Options::LoggerState state = static_cast<OpenFlipper::Options::LoggerState> (_state);
  if ( OpenFlipper::Options::gui() &&
       (state == OpenFlipper::Options::Hidden ||
        state == OpenFlipper::Options::InScene ||
        state == OpenFlipper::Options::Normal))
    coreWidget_->showLogger(state);
}

//-----------------------------------------------------------------------------

void Core::showToolbox( bool _state ) {
  if ( OpenFlipper::Options::gui() )
    coreWidget_->showToolbox(_state);
}

void Core::multiViewMode( int _mode ) {
  if ( !OpenFlipper::Options::gui() || !OpenFlipper::Options::multiView() )
    return;

  switch (_mode)
  {
    case 0:
      coreWidget_->baseLayout_->setMode (QtMultiViewLayout::SingleView);
      PluginFunctions::setActiveExaminer(0);
      break;
    case 1:
      coreWidget_->baseLayout_->setMode (QtMultiViewLayout::Grid);
      break;
    case 2:
      coreWidget_->baseLayout_->setMode (QtMultiViewLayout::HSplit);
      break;

    default:
      emit log(LOGERR,tr("Requested illegal multiview mode!"));
  }

}




//-----------------------------------------------------------------------------

void
Core::slotRecentOpen(QAction* _action)
{
  QStringList recentFiles = OpenFlipperSettings().value("Core/File/RecentFiles").toStringList();
  QStringList recentTypes = OpenFlipperSettings().value("Core/File/RecentTypes").toStringList();
  
  for (int i = 0 ; i < recentFiles.size() ; ++i )
    if ( recentFiles[i] == _action->text() ){
        OpenFlipper::Options::loadingRecentFile(true);
        loadObject(typeId(recentTypes[i]), recentFiles[i]);
        coreWidget_->addRecent(recentFiles[i],typeId(recentTypes[i]) );
        OpenFlipper::Options::loadingRecentFile(false);
        return;
    }
}


void
Core::writeOnExit() {
  QString inifile = QDir::home().absolutePath() + OpenFlipper::Options::dirSeparator() + ".OpenFlipper" +
                                                  OpenFlipper::Options::dirSeparator() +  "OpenFlipper.ini";

  INIFile ini;
  if ( ! ini.connect( inifile ,false) ) {
    emit log(LOGERR,tr("Failed to connect to users ini file"));

    if ( ! ini.connect( inifile,true) ) {
      emit log(LOGERR,tr("Can not create user ini file"));
    } else {
      writeApplicationOptions(ini);
      ini.disconnect();
    }
  } else {
    writeApplicationOptions(ini);
    ini.disconnect();
  }

  //store the windowState
  if ( OpenFlipper::Options::gui() ) {

    QSettings windowStates(QDir::home().absolutePath() + OpenFlipper::Options::dirSeparator() + ".OpenFlipper" +
                           OpenFlipper::Options::dirSeparator() +  "WindowStates.dat", QSettings::IniFormat);

    windowStates.setValue("Core/Window/State", coreWidget_->saveState ());
    windowStates.setValue("Core/Window/Geometry", coreWidget_->saveGeometry ());

    windowStates.setValue ("Core/ToolSplitter", coreWidget_->toolSplitter_->saveState ());
    windowStates.setValue ("Core/LogSplitter", coreWidget_->splitter_->saveState ());

    windowStates.beginGroup ("Core");
    windowStates.beginGroup ("LogSlider");
    coreWidget_->slidingLogger_->saveState (windowStates);
    windowStates.endGroup ();
    coreWidget_->toolBox_->saveState (windowStates);
    windowStates.endGroup ();
  }

  // Call exit for all plugins
   for (uint i = 0 ; i < plugins.size() ; ++i) {
      BaseInterface* basePlugin = qobject_cast< BaseInterface * >(plugins[i].plugin);
      if ( basePlugin )
          if ( checkSlot( plugins[i].plugin , "exit()" ) )
            QMetaObject::invokeMethod(plugins[i].plugin, "exit",  Qt::DirectConnection);
   }
}

void Core::slotExit() {
  writeOnExit();

  if (logFile_)
    logFile_->close();

  OpenFlipper::Options::closeSettings();
  qApp->quit();
}

/// log to file
void Core::slotLogToFile(Logtype _type, QString _message){

  if (!OpenFlipperSettings().value("Core/Log/logFileEnabled",true).toBool() )
    return;

  if (logStream_ == 0){
    //check if a logfile has been specified and if the path is valid

    QString fileName = OpenFlipperSettings().value("Core/Log/logFile","").toString();
    QFileInfo fi( fileName );

    if ( fileName == "" || !fi.dir().exists() ) {
      OpenFlipperSettings().setValue("Core/Log/logFile", QDir::home().absolutePath() + OpenFlipper::Options::dirSeparator() + ".OpenFlipper" +
                                                         OpenFlipper::Options::dirSeparator() +  "OpenFlipper.log");
    }

    logFile_ = new QFile( OpenFlipperSettings().value("Core/Log/logFile").toString() );
    if ( logFile_->open(QFile::WriteOnly) ) {
        logStream_ = new QTextStream (logFile_);
    } else {
      emit log(LOGERR, tr("Unable to open logfile!"));
      return;
    }
  }

  switch (_type) {
    case LOGINFO:
      (*logStream_) << "INFO:"; break;
    case LOGOUT:
      (*logStream_) << "OUT :"; break;
    case LOGWARN:
      (*logStream_) << "WARN:"; break;
    case LOGERR:
      (*logStream_) << "ERR :"; break;
  }

  (*logStream_) << _message << "\n" << flush;

}

/// set descriptions for a scriptable slot
void Core::slotSetSlotDescription(QString      _slotName,   QString _slotDescription,
                              QStringList _parameters, QStringList _descriptions)
{
  //handle core slots
  if (sender() == this){
    SlotInfo info;
    info.slotName = _slotName;
    info.slotDescription = _slotDescription;
    info.parameters = _parameters;
    info.descriptions = _descriptions;

    coreSlots_.push_back( info );
    return;
  }

  //handle plugin slots

  //find plugin
 PluginInfo* pluginInfo = 0;

  for (uint i=0; i < plugins.size(); i++)
    if (plugins[i].plugin == sender())
      pluginInfo = &plugins[i];

    if (pluginInfo == 0){
      emit log(LOGERR, tr("Unable to set slot-description. Plugin not found!"));
    return;
  }

  SlotInfo info;
  info.slotName = _slotName;
  info.slotDescription = _slotDescription;
  info.parameters = _parameters;
  info.descriptions = _descriptions;

  pluginInfo->slotInfos.append( info );
}

/// get available Descriptions for a scriptable slot
void Core::slotGetDescription(QString      _function,   QString&     _fnDescription,
                              QStringList& _parameters, QStringList& _descriptions )
{
  QString pluginName = _function.section(".", 0, 0);
  QString slotName   = _function.section(".", 1, 1);

  //handle core slots
  if (pluginName == "core"){

    _fnDescription = "";
    _parameters.clear();
    _descriptions.clear();

    for (int i=0; i < coreSlots_.count(); i++)
      if (coreSlots_[i].slotName == slotName){
      _fnDescription = coreSlots_[i].slotDescription;
      _parameters    = coreSlots_[i].parameters;
      _descriptions  = coreSlots_[i].descriptions;
      return;
      }
    return;
  }

  //handle plugin slots

  //find plugin
  PluginInfo* pluginInfo = 0;

  for (uint i=0; i < plugins.size(); i++)
    if (plugins[i].rpcName == pluginName)
      pluginInfo = &plugins[i];

  if (pluginInfo == 0){
    emit log(LOGERR, tr("Unable to get slot-description. Plugin not found!"));
    return;
  }

  _fnDescription = "";
  _parameters.clear();
  _descriptions.clear();

  //find slot
  for (int i=0; i < pluginInfo->slotInfos.count(); i++)
    if (pluginInfo->slotInfos[i].slotName == slotName){
      _fnDescription = pluginInfo->slotInfos[i].slotDescription;
      _parameters    = pluginInfo->slotInfos[i].parameters;
      _descriptions  = pluginInfo->slotInfos[i].descriptions;
      return;
    }
}

void Core::snapshotBaseFileName(const QString& _fname, unsigned int _viewerId ){

  if ( OpenFlipper::Options::gui() ) {
    if ( _viewerId >= OpenFlipper::Options::examinerWidgets() ) {
      emit log(LOGERR,tr("Unable to snapshotBaseFileName for viewer ") + QString::number(_viewerId) );
      return;
    }

    PluginFunctions::viewerProperties(_viewerId).snapshotBaseFileName( _fname );
  }

}

void Core::snapshot( unsigned int _viewerId ){


  if ( OpenFlipper::Options::gui() ) {
    if ( _viewerId >= OpenFlipper::Options::examinerWidgets() ) {
      emit log(LOGERR,tr("Unable to create snapshot for viewer ") + QString::number(_viewerId) );
      return;
    }
    coreWidget_->examiner_widgets_[_viewerId]->snapshot();
  }

}

void Core::applicationSnapshot(){

  if ( OpenFlipper::Options::gui() )
    coreWidget_->applicationSnapshot();
}

void Core::applicationSnapshotName(QString _name){

  if ( OpenFlipper::Options::gui() )
    coreWidget_->applicationSnapshotName(_name);
}

void Core::viewerSnapshot(){

  if ( OpenFlipper::Options::gui() )
    coreWidget_->viewerSnapshot();
}

void Core::resizeViewers(int _width, int _height ){
  if ( OpenFlipper::Options::gui() ){

    lastWidth_  = coreWidget_->glView_->width();
    lastHeight_ = coreWidget_->glView_->height();


    //+4,+20 to to get the correct dimension when using snapshotFunction
    coreWidget_->glView_->resize(_width+4, _height+20);
  }
}

void Core::resizeApplication(int _width, int _height ){
 if ( OpenFlipper::Options::gui() ){

    lastWidth_  = coreWidget_->glView_->width();
    lastHeight_ = coreWidget_->glView_->height();

    coreWidget_->resize(_width, _height);
 }
}

void Core::writeVersionNumbers(QString _filename){

INIFile ini;

  if ( ! ini.connect(_filename,true) ) {
    emit log(LOGERR,tr("Failed to connect to _ini file") + _filename);
      return;
  }

  //add coreVersion
  ini.add_section( "Core" );
  if ( OpenFlipper::Options::isWindows() )
    ini.add_entry( "Core" , "VersionWindows" , OpenFlipper::Options::coreVersion() );
  else
    ini.add_entry( "Core" , "VersionLinux"   , OpenFlipper::Options::coreVersion() );

  //add pluginVersions
  for (uint i=0; i < plugins.size(); i++){
    ini.add_section( plugins[i].name );

    if ( OpenFlipper::Options::isWindows() )
      ini.add_entry( plugins[i].name , "VersionWindows" , plugins[i].version );
    else
      ini.add_entry( plugins[i].name , "VersionLinux"   , plugins[i].version );
  }

  ini.disconnect();
}

QList<int> Core::objectList (QString _selection, QStringList _types)
{
  QList<int> rv;
  DataType ids = 0;
  PluginFunctions::IteratorRestriction selection = PluginFunctions::ALL_OBJECTS;

  foreach (QString s, _types)
    if (!s.isEmpty ())
      ids = typeId (s);
  if (_selection == "source")
    selection = PluginFunctions::SOURCE_OBJECTS;
  else if (_selection == "target")
    selection = PluginFunctions::TARGET_OBJECTS;

  for ( PluginFunctions::ObjectIterator o_it(selection, ids) ;
                                        o_it != PluginFunctions::objectsEnd(); ++o_it)
    rv.append (o_it->id ());
  return rv;
}

/// set the descriptions for scriptable slots of the core
void Core::setDescriptions(){

  connect(this, SIGNAL(setSlotDescription(QString,QString,QStringList,QStringList)),
          this,   SLOT(slotSetSlotDescription(QString,QString,QStringList,QStringList)) );

  emit setSlotDescription("updateView()", tr("Redraw the contents of the viewer."), QStringList(), QStringList());
  emit setSlotDescription("clearAll()", tr("Clear all data objects."), QStringList(), QStringList());
  emit setSlotDescription("exitApplication()", tr("Quit OpenFlipper"), QStringList(), QStringList());
  emit setSlotDescription("fullscreen(bool)", tr("Enable or disable fullscreen mode"),
                           QStringList(tr("enabled")) ,
                           QStringList(tr("Enable or disable fullscreen mode")));
  emit setSlotDescription("loggerState(int)", tr("Change the logger window state"), QStringList(tr("Change the logger window state")), QStringList());
  emit setSlotDescription("showToolbox(bool)", tr("Show or hide toolbox"), QStringList(tr("Show or hide the toolbox")), QStringList());
  emit setSlotDescription("multiViewMode(int)", tr("Switch MultiView Mode"),
                          QStringList(tr("Mode")), QStringList(tr("0: One Viewer\n 1: Grid \n 2: Horizontal split")));

  emit setSlotDescription("restrictFrameRate(bool)", tr("Restrict FrameRate to MaxFrameRate"),
                        QStringList(tr("enabled")), QStringList(tr("restriction switch")));
  emit setSlotDescription("setMaxFrameRate(int)", tr("set the maximal framerate (automatically enables framerate restriction)"),
                        QStringList(tr("frameRate")), QStringList(tr("Maximum frameRate")));
  emit setSlotDescription("snapshotBaseFileName(QString&)", tr("Set a filename for storing snapshots.")
                          , QStringList(), QStringList());
  emit setSlotDescription("snapshot()", tr("Make a snapshot of the viewer. If no filename"
                          " was set using snapshotBaseFileName() the snapshot is stored"
                          " in snap.png in the current directory. For every snapshot"
                          " a counter is added to the filename."), QStringList(), QStringList());
  emit setSlotDescription("resizeViewer(int,int)", tr("Resize the viewer"),
                           QString(tr("width,height")).split(","),
                           QString(tr("new width for the viewer,new height for the viewer")).split(","));
  emit setSlotDescription("writeVersionNumbers(QString)", tr("write the current versions of all plugins to INI file"),
                           QStringList(tr("filename")),
                           QStringList(tr("fullpath to a file where the versions should be written to.")));
  //save slots
  emit setSlotDescription("saveObject(int,QString)", tr("Save object to file. If the file exists it will be overwritten."),
                           QString(tr("object-id,filename")).split(","),
                           QString(tr("id of the object, complete path and filename")).split(","));
  emit setSlotDescription("saveObjectTo(int,QString)", tr("Save object to file. The location can be chosen in a dialog. "
                          "(only works if GUI is available)"),
                           QString(tr("object-id,filename")).split(","),
                           QString(tr("id of the object, initial filename for the dialog")).split(","));
  emit setSlotDescription("saveAllObjects()", tr("Saves all target objects. "
                          "If no filename is available a dialog is shown. (only works if GUI is available)"),QStringList(), QStringList());
  emit setSlotDescription("saveAllObjectsTo()", tr("Saves all target objects. The locations can be chosen in dialogs. "
                          "(only works if GUI is available)"),QStringList(), QStringList());
  emit setSlotDescription("saveSettings()", tr("Show the dialog to save the current setting. (only works if GUI is available)"),QStringList(), QStringList());
  //load slots
  emit setSlotDescription("loadObject()", tr("Show the dialog to load an object. (only works if GUI is available)"),QStringList(), QStringList());
  emit setSlotDescription("getObjectId(QString)", tr("Return identifier of object with specified name. Returns -1 if object was not found."),QStringList(), QStringList());
  emit setSlotDescription("loadSettings()", tr("Show the dialog to load settings. (only works if GUI is available)"),QStringList(), QStringList());
  emit setSlotDescription("loadSettings(QString)", tr("load settings from file."),QStringList(), QStringList());

  emit setSlotDescription("createWidget(QString,QString)", tr("Create a widget from an ui file"),
                          QString(tr("Object name,ui file")).split(","),
                          QString(tr("Name of the new widget in script,ui file to load")).split(","));

  emit setSlotDescription("addToolbox(QString,QWidget*)", tr("Add a widget as a toolbox"),
                          QString(tr("Toolbox Entry name,Widget")).split(","),
                          QString(tr("Name of the new widget in the toolbox,Pointer to the new widget")).split(","));

  emit setSlotDescription("addViewModeToolboxes(QString,QString)", tr("Set toolboxes for a viewmode (This automatically adds the view mode if it does not exist)"),
                          QString(tr("Name,Toolbox List")).split(","),
                          QString(tr("Name of the Viewmode, ; seperated list of toolboxes visible in this viewmode")).split(","));

  emit setSlotDescription("addViewModeToolbars(QString,QString)", tr("Set toolbars for a viewmode (This automatically adds the view mode if it does not exist)"),
                          QString(tr("Name,Toolbar List")).split(","),
                          QString(tr("Name of the Viewmode, ; seperated list of toolbars visible in this viewmode")).split(","));

  emit setSlotDescription("objectList(QString,QStringList)", tr("Returns object list"),
                          QString(tr("Selection type,Object types")).split(","),
                          QString(tr("Type of object selection (all,source,target),Object type (All,PolyMesh,TriangleMesh,...)")).split(";"));
}

void Core::slotDeleteObject( int _id ){

  if ( _id == -1 )
    return;

  // get the node
  BaseObject* object = objectRoot_->childExists(_id);

  if ( !object || object == objectRoot_ ) {
    std::cerr << "Error while deleting object, does not exist!!" << std::endl;
    return;
  }

  // remove the whole subtree below this item
  object->deleteSubtree();

  // remove the item itself from the parent
  object->parent()->removeChild(object);

  // delete it
  delete object;

  emit objectDeleted(_id);

  slotObjectUpdated(-1);
}

void Core::slotDeleteAllObjects( ){

  // Remember ids
  std::vector< int > ids;

  BaseObject* current = objectRoot_->next();

  while( current != objectRoot_ ){
    ids.push_back( current->id() );
    current = current->next();
  }

  // remove the whole subtree below the root
  objectRoot_->deleteSubtree();

  for ( uint i = 0 ; i < ids.size(); ++i ) {
    emit objectDeleted(ids[i]);
  }

  slotObjectUpdated(-1);
}

// //-----------------------------------------------------------------------------
//
// void Core::slotGetPlugin(QString _name, QObject* & _plugin ){
//   for (uint i=0; i < plugins.size(); i++)
//     if (plugins[i].name == _name){
//       _plugin = plugins[i].plugin;
//       return;
//     }
//
//   _plugin = 0;
//   return;
// }

//=============================================================================
