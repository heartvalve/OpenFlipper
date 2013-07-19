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

#include <OpenFlipper/common/RendererInfo.hh>

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
  processManager_(0),
  nextBackupId_(0),
  nextBackupGroupId_(0),
  objectRoot_(0),
  coreWidget_(0)
{

  //init logFile
  logStream_ = 0;
  logFile_ = 0;

  //init nodes
  root_node_scenegraph_ = new ACG::SceneGraph::SeparatorNode(0, "SceneGraph Root Node");
  
  // init global data node
  root_node_scenegraph_global_ = new ACG::SceneGraph::SeparatorNode(root_node_scenegraph_ , "SceneGraph Rendered Root Node");
  
  // This separator will manage the cores nodes
  core_nodes_ = new ACG::SceneGraph::SeparatorNode(root_node_scenegraph_global_, "Core Nodes");
  
  // Coordsys rendering nodes
  coordsysMaterialNode_ = new ACG::SceneGraph::MaterialNode(core_nodes_,"Coordsys Material Node");
  coordsysNode_ = new ACG::SceneGraph::CoordsysNode(coordsysMaterialNode_,"Core Coordsys Node");  
  coordsysNode_->setTraverseMode (BaseNode::NodeFirst | BaseNode::SecondPass);
  
  // Separator handling the nodes for data
  dataSeparatorNode_ = new ACG::SceneGraph::SeparatorNode(root_node_scenegraph_global_, "Data Separator Root Node");
  
  // Separator handling the nodes for data
  dataRootNode_      = new ACG::SceneGraph::SeparatorNode(dataSeparatorNode_, "Data Root Node");

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
    Viewer::ViewerProperties* viewerProperty = new Viewer::ViewerProperties(i);
    viewerProperty->snapshotBaseFileName("snap-Viewer-" + QString::number(i) + ".png");
    viewerProperties.push_back( viewerProperty );
  }

  PluginFunctions::setViewerProperties(viewerProperties);

  //set viewer defaults
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
  
  // Initialize the build in updateTypes
  initializeUpdateTypes();
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
  
  // Check library versions
  checkLibraryVersions();

  // Make root_node available to the plugins ( defined in PluginFunctions.hh)
  PluginFunctions::setDataSeparatorNodes( dataSeparatorNode_ );

  // Topmost node of the scenegraph
  PluginFunctions::setSceneGraphRootNode( root_node_scenegraph_ );
  
  // Node below the global status nodes. All nodes with global rendering
  // will be attached here.
  PluginFunctions::setSceneGraphRootNodeGlobal(root_node_scenegraph_global_);

  // Initialize the first object as the root Object for the object tree
  objectRoot_ =  dynamic_cast< BaseObject* > ( new GroupObject("ObjectRoot") );
  PluginFunctions::setDataRoot( objectRoot_ );
  
  // Bring up the object manager ( has to be done after the rootobject is created)
  connect(getObjectManager(),SIGNAL(newObject(int)), this ,SLOT(newObject(int)));
  
  connect(getObjectManager(),SIGNAL(deletedObject(int)), this ,SLOT(deletedObject(int)));

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

      splash_ = new QSplashScreen(splashPixmap, Qt::SplashScreen | Qt::WindowStaysOnTopHint);
      splash_->show();

      splash_->showMessage(tr("Initializing mainwindow") ,
                          Qt::AlignBottom | Qt::AlignLeft , Qt::white);
    }

    coreWidget_ = new CoreWidget(viewModes_ , plugins_, coreSlots_);

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
    
    connect( coreWidget_->logWidget_->openMeshFilterAction_,SIGNAL(toggled(bool)), this, SLOT(enableOpenMeshErrorLog(bool)) );

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

    #ifdef DEBUG
      coreWidget_->setWindowTitle( OpenFlipper::Options::windowTitle() + " [DEBUG]" );
    #else
      coreWidget_->setWindowTitle( OpenFlipper::Options::windowTitle() );
    #endif

    // Sanity check for OpenGL capabilities!
    checkOpenGLCapabilities();

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

  connect(&scriptEngine_, SIGNAL( signalHandlerException(const QScriptValue &) ), this, SLOT( slotScriptError(const QScriptValue &) ));

  // process Events every 500 msecs during script execution
  scriptEngine_.setProcessEventsInterval( 500 );

  // Register own print function :
  QScriptValue printFunction = scriptEngine_.newFunction(myPrintFunction);
  printFunction.setProperty("textedit",scriptEngine_.newQObject(this));
  scriptEngine_.globalObject().setProperty("print", printFunction);

  // Register print to file function :
  QScriptValue printToFileFunc = scriptEngine_.newFunction(printToFileFunction);
  scriptEngine_.globalObject().setProperty("printToFile", printToFileFunc);
  scriptingFunctions_.push_back( "-.printToFile(QString,QString)" );

  // Register help function :
  QScriptValue helpFunc = scriptEngine_.newFunction(helpFunction);
  helpFunc.setProperty("core",scriptEngine_.newQObject(this));
  scriptEngine_.globalObject().setProperty("help", helpFunc);
  scriptingFunctions_.push_back( "-.help(QString)" );

  // Register IdList Type to scripting Engine
  qScriptRegisterSequenceMetaType< IdList >(&scriptEngine_);
  
  // Register Vector of ints Type to scripting Engine
  qScriptRegisterSequenceMetaType< QVector< int > >(&scriptEngine_);
  
  //==========================================================================
  // Register the 3d Vector Type to the core ( is Vec3d )
  //==========================================================================
  qScriptRegisterMetaType(&scriptEngine_,
                          toScriptValueVector,
                          fromScriptValueVector,
                          scriptEngine_.newQObject(&vec3dPrototype_));
                          
  // set a constructor to allow creation via Vector(x,y,z)
  QScriptValue ctorVec3 = scriptEngine_.newFunction(createVector);
  scriptEngine_.globalObject().setProperty("Vector", ctorVec3);

  //==========================================================================
  // Register the 4d Vector Type to the core ( is Vec4d )
  //==========================================================================
  qScriptRegisterMetaType(&scriptEngine_,
                          toScriptValueVector4,
                          fromScriptValueVector4,
                          scriptEngine_.newQObject(&vec4dPrototype_));

  // set a constructor to allow creation via Vector(x,y,z)
  QScriptValue ctorVec4 = scriptEngine_.newFunction(createVector4);
  scriptEngine_.globalObject().setProperty("Vector4", ctorVec4);
                          
  //==========================================================================
  // Register the DataType Class to the core
  //==========================================================================

  // Register DataType in QScriptEngine
  qScriptRegisterMetaType<DataType>(&scriptEngine_,
                          toScriptValueDataType,
                          fromScriptValueDataType,
                          scriptEngine_.newQObject(&DataTypePrototype_));
                          
  // set a constructor to allow creation via DataType(uint)
  QScriptValue dataTypector = scriptEngine_.newFunction(createDataType);
  scriptEngine_.globalObject().setProperty("DataType", dataTypector);     
  
  //==========================================================================
  // Register the Matrix Class to the core
  //==========================================================================

  // Register Matrix Type to scripting Engine ( is ACG::Matrix4x4d )
  qScriptRegisterMetaType(&scriptEngine_,
                          toScriptValueMatrix4x4 ,
                          fromScriptValueMatrix4x4,
                          scriptEngine_.newQObject(&matrix4x4Prototype_));

  // set a constructor to allow creation via Matrix(x,y,z)
  QScriptValue matrix4x4ctor = scriptEngine_.newFunction(createMatrix4x4);
  scriptEngine_.globalObject().setProperty("Matrix4x4", matrix4x4ctor);

  //==========================================================================
  //  Collect Core scripting information
  //==========================================================================

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
    if ( checkSignal( this, it.name().toLatin1() ) )
      continue;

    scriptingFunctions_.push_back( "core." + it.name() );

  }

  loadPlugins();

  if ( OpenFlipper::Options::gui() ) {

    //register keyBinding for all scripting slots
    coreWidget_->slotRegisterSlotKeyBindings();

    //get keyAssignments from config files
    restoreKeyBindings();

    if ( OpenFlipper::Options::currentViewMode( ) != "" )
      coreWidget_->setViewMode( OpenFlipper::Options::currentViewMode() );
    else
      coreWidget_->setViewMode("All");

    // Set the renderer to the one stored in the settings or to default
    for ( unsigned int i = 0 ; i < OpenFlipper::Options::examinerWidgets(); ++i ) {
      connect( coreWidget_->examiner_widgets_[i], SIGNAL(signalMouseEvent(QMouseEvent*)),
              this                              , SLOT(slotMouseEvent(QMouseEvent*)));
      connect( coreWidget_->examiner_widgets_[i], SIGNAL(signalMouseEventIdentify(QMouseEvent*)),
              this                              , SLOT(slotMouseEventIdentify(QMouseEvent*)));
      connect( coreWidget_->examiner_widgets_[i], SIGNAL(signalMouseEventLight(QMouseEvent*)),
              this                              , SLOT(slotMouseEventLight(QMouseEvent*)));
      connect( coreWidget_->examiner_widgets_[i], SIGNAL(signalWheelEvent(QWheelEvent *, const std::string &)),
              this                              , SLOT(slotWheelEvent(QWheelEvent *, const std::string &)));

      connect( coreWidget_->examiner_widgets_[i], SIGNAL( viewUpdated() ),
               this, SLOT( viewUpdated()) ,Qt::DirectConnection);

      connect( coreWidget_->examiner_widgets_[i], SIGNAL( viewChanged() ),
               coreWidget_->examiner_widgets_[i], SLOT( updateGL() ) ,Qt::DirectConnection);

      // ====================================================
      // Set renderer
      // ====================================================

      QString defaultRendererKey  = "Viewer" + QString::number(i)+"/DefaultRenderer";
      QString defaultRendererName = OpenFlipperSettings().value(defaultRendererKey,"Default Classical Renderer Plugin").toString();

      // Check if the renderer is there
      int defaultRendererId = renderManager().getRendererId(defaultRendererName);

      if ( defaultRendererId == -1 ) {
        emit log(LOGERR,tr("Stored default renderer \"") + defaultRendererName + tr("\" is not available, trying Classical!"));

        // Check if the renderer is there
        defaultRendererId = renderManager().getRendererId("Default Classical Renderer Plugin");

        // Classical available?
        if ( defaultRendererId != -1 ) {
          renderManager().setActive(defaultRendererId,i);
        } else {
          emit log(LOGERR,tr("Default classical renderer is also not available. Trying to use any other renderer i can find!"));

          // debug information for this case, print renderer count and their names
          const unsigned int rendererCount = renderManager().available();
          emit log(LOGERR,tr("Currently ") + QString::number(rendererCount) + tr(" renderers are available:") );
          for (unsigned int i = 0 ; i < rendererCount ; ++i )
            emit log(LOGERR, tr("Renderer ") + QString::number(i) + ": " + renderManager()[i]->name );
        }

      } else {
        renderManager().setActive(defaultRendererId,i);
      }

    }

    // Warn the user in the log and via messagebox, that he is using the build in renderer only
    if ( renderManager().available() == 1 ) {
      emit log(LOGERR,tr("No external plugin renderers available!"));
      emit log(LOGERR,tr("The build in renderer is only a very limited one and is missing many features!"));
      emit log(LOGERR,tr("You should build and use the other renderers!"));

      QMessageBox msgBox;
      msgBox.setText("No external plugin renderers available!");
      msgBox.setInformativeText("The build in renderer is only a very limited one and is missing many features.\nYou should build and use the other free renderers shipped with OpenFlipper.");
      msgBox.setIcon(QMessageBox::Warning);
      msgBox.exec();
    }

  }

  // ===============================================================================================
  // Load Settings from configuration files
  // ===============================================================================================

  QStringList optionFiles = OpenFlipper::Options::optionFiles();
  for ( int i = 0 ; i < (int)optionFiles.size(); ++i) {

    if ( OpenFlipper::Options::gui() && OpenFlipperSettings().value("Core/Gui/splash",true).toBool() ) {
      splash_->showMessage(tr("Loading Configuration File %1/%2").arg(i+1).arg(optionFiles.size()),
                           Qt::AlignBottom | Qt::AlignLeft , Qt::white);
    }

    // Load global ini files. Use only plugin global options from these files as the
    // rest has been loaded at the beginning.
    if ( OpenFlipper::Options::gui() )
      coreWidget_->updateRecent();

    openIniFile( optionFiles[i] ,false,true,false);
  }

  if ( OpenFlipper::Options::gui() && OpenFlipperSettings().value("Core/Gui/splash",true).toBool() )
      splash_->clearMessage();

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

    QFile statesFile(OpenFlipper::Options::configDirStr()  + OpenFlipper::Options::dirSeparator() + "WindowStates.dat");

    if (statesFile.exists() ) {
      QSettings windowStates(OpenFlipper::Options::configDirStr()  + OpenFlipper::Options::dirSeparator() + "WindowStates.dat", QSettings::IniFormat);

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

    } else {

      coreWidget_->show();
      applyOptions();

    }

    if ( OpenFlipperSettings().value("Core/Gui/splash",true).toBool() ) {
        splash_->raise();
        splash_->showMessage(tr("Ready."), Qt::AlignBottom | Qt::AlignLeft , Qt::white);
        QTimer::singleShot(2000, this, SLOT(finishSplash()));
    }

    // start checking for scenegraph changes
    scenegraphCheckTimer_->setInterval (1000 / OpenFlipperSettings().value("Core/Gui/glViewer/maxFrameRate",35).toInt() );
    scenegraphCheckTimer_->start ();
  }

  // System is ready now.
  OpenFlipper::Options::finishedStartup();
  
  QTimer::singleShot(100, this, SLOT(slotExecuteAfterStartup()));
}


//-----------------------------------------------------------------------------

Core::~Core()
{
   for ( uint i = 0 ; i < plugins_.size() ; ++i ){
     BaseInterface* basePlugin = qobject_cast< BaseInterface * >(plugins_[i].plugin);

     // Dont call exit if we cannot get the Plugin
     if ( basePlugin )
       if ( checkSlot( plugins_[i].plugin , "exit()" ) )
          QMetaObject::invokeMethod(plugins_[i].plugin, "exit",  Qt::DirectConnection);
  }

  // Delete the objectRoot if it was constructed
  if ( objectRoot_ != 0 ) {
    objectRoot_->deleteSubtree();
    delete objectRoot_;
  }

  // Clean up loggers
  for ( uint i = 0 ; i < loggers_.size(); ++i )
    delete loggers_[i];

  delete coreWidget_;
}

//-----------------------------------------------------------------------------

void
Core::slotMouseEventIdentify( QMouseEvent* _event )
{
//   // Dont do anything as a context Menu will popup on right button click
//   if ( _event->button() == Qt::RightButton )
//     return;

  // Only catch left-button clicks
  if(_event->button() != Qt::LeftButton) return;

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

  // Do picking
  unsigned int   node_idx, target_idx;
  ACG::Vec3d     hit_point;

  if(PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_ANYTHING, _event->pos(), node_idx, target_idx, &hit_point)) {

    BaseObjectData* object = 0;

    if(PluginFunctions::getPickedObject(node_idx, object)) {
      // Request type information widget
      InformationInterface* infoPlugin = 0;
      infoPlugin = getInfoPlugin(object->dataType());
      if(infoPlugin != 0)
          infoPlugin->slotInformationRequested(_event->pos(), object->dataType());
    }
  }
}

//-----------------------------------------------------------------------------

void
Core::slotMouseEventLight( QMouseEvent* _event )
{
  const QObject* senderPointer = sender();
  unsigned int examinerId = 0;

  if ( senderPointer == 0 ) {
    std::cerr << "Error : slotMouseEventLight directly called! This should only be called by an examiner" << std::endl;
  } else {
    for ( unsigned int i = 0 ; i < OpenFlipper::Options::examinerWidgets(); ++i ) {
      if ( senderPointer == coreWidget_->examiner_widgets_[i] ) {
        examinerId = i;
        if ( OpenFlipper::Options::doSlotDebugging() )
              emit log(LOGINFO,tr("slotMouseEventLight from examiner ") + QString::number(i) );
        break;
      }
    }

  }

  PluginFunctions::setActiveExaminer( examinerId );

  emit PluginMouseEventLight( _event );


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
Core::slotAddPickMode( const std::string& _mode ) {
  if ( OpenFlipper::Options::gui() )
    coreWidget_->addPickMode(_mode);

}

//-----------------------------------------------------------------------------

void
Core::slotAddHiddenPickMode( const std::string& _mode ) {

  if ( OpenFlipper::Options::gui() )
    coreWidget_->addPickMode(_mode,false,1000,false);

}

//-----------------------------------------------------------------------------

 /** Update the view in the examiner widget
  */
void Core::updateView()
{

  if (OpenFlipper::Options::doSlotDebugging()) {
    if (sender() != 0) {
      if (sender()->metaObject() != 0) {
        emit log(LOGINFO, tr("updateView() called by ") + QString(sender()->metaObject()->className()));
      }
    }
  }

  if (!OpenFlipper::Options::gui())
    return;

  if (OpenFlipperSettings().value("Core/Gui/glViewer/restrictFrameRate", false).toBool()) {

    int elapsed = redrawTime_->elapsed();

    if (elapsed < 1000 / OpenFlipperSettings().value("Core/Gui/glViewer/maxFrameRate", 35).toInt()) {
      // redraw time not reached ... waiting for timer event for next redraw
      if (redrawTimer_->isActive()) {
        if (OpenFlipper::Options::doSlotDebugging())
          emit log(LOGINFO,
              tr("Too early for redraw! Delaying request from ") + QString(sender()->metaObject()->className()));
        return;
      }

      // Start the timer
      redrawTimer_->start((1000 / OpenFlipperSettings().value("Core/Gui/glViewer/maxFrameRate", 35).toInt()) - elapsed);
      return;
    } else if (redrawTimer_->isActive())
      redrawTimer_->stop();

  }

  redrawTime_->restart();

  if (!OpenFlipper::Options::sceneGraphUpdatesBlocked() && !OpenFlipper::Options::redrawDisabled()) {

    for (unsigned int i = 0; i < OpenFlipper::Options::examinerWidgets(); ++i)
      coreWidget_->examiner_widgets_[i]->updateGL();

    // Inform plugins of the scene update
    emit pluginSceneDrawn();
  }


}

//-----------------------------------------------------------------------------

void Core::updateUI() {
  QApplication::processEvents();
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
  OpenFlipperSettings().setValue("Core/Gui/glViewer/restrictFrameRate",_enable); 
}

//-----------------------------------------------------------------------------

void Core::setMaxFrameRate( int _rate ) {
  OpenFlipperSettings().setValue("Core/Gui/glViewer/maxFrameRate",_rate);
  OpenFlipperSettings().setValue("Core/Gui/glViewer/restrictFrameRate",true); 

  // update Timer to new framerate
  scenegraphCheckTimer_->setInterval (1000 / OpenFlipperSettings().value("Core/Gui/glViewer/maxFrameRate",35).toInt() );
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

void Core::showViewModeControls( bool _show ) {
  if ( OpenFlipper::Options::gui() ) {
    coreWidget_->showViewModeControls(_show);
  }
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

void Core::enableOpenMeshErrorLog(bool _state) {
  std::cerr << "Script" << std::endl;
  
  // Set the state on openmesh stream
  if ( _state ) {
    omerr().enable();
  } else {
    omerr().disable();
  }
  
  if ( OpenFlipper::Options::gui() ) {
    // store in application settings
    OpenFlipperSettings().setValue("Core/Gui/LogWindow/OpenMeshErrors",_state);
    
    coreWidget_->logWidget_->openMeshFilterAction_->blockSignals(true);
    coreWidget_->logWidget_->openMeshFilterAction_->setChecked(_state);
    coreWidget_->logWidget_->openMeshFilterAction_->blockSignals(false);
  }
  
}

//-----------------------------------------------------------------------------

void Core::showToolbox( bool _state ) {
  if ( OpenFlipper::Options::gui() )
    coreWidget_->showToolbox(_state);
}

//-----------------------------------------------------------------------------

void Core::showStatusBar( bool _state ) {
  if ( OpenFlipper::Options::gui() )
    coreWidget_->showStatusBar(_state);
}

//-----------------------------------------------------------------------------

void Core::multiViewMode( int _mode ) {
  if ( !OpenFlipper::Options::gui() || !OpenFlipperSettings().value("Core/Gui/glViewer/useMultipleViewers",true).toBool() )
    return;

  coreWidget_->setViewerLayout(_mode);

}




//-----------------------------------------------------------------------------

void
Core::slotRecentOpen(QAction* _action)
{
  // Get the recent files lists and datatypes
  QStringList recentFiles = OpenFlipperSettings().value("Core/File/RecentFiles", QStringList()).toStringList();
  QStringList recentTypes = OpenFlipperSettings().value("Core/File/RecentTypes", QStringList()).toStringList();

  // The type of the file to open is attached to the action as a string.
  // the name is the actions text
  QString actionTypeName = _action->data().toString();

  // Iterate over all recent files
  for (int i = 0 ; i < recentFiles.size() ; ++i )

    // If the name matches and also the type, we open it.
    if ( (recentFiles[i] == _action->text()) && ( actionTypeName ==  recentTypes[i] ) ){

        OpenFlipper::Options::loadingRecentFile(true);
        loadObject(typeId(recentTypes[i]), recentFiles[i]);
        coreWidget_->addRecent(recentFiles[i],typeId(recentTypes[i]) );
        OpenFlipper::Options::loadingRecentFile(false);

        return;
    }

  emit log(LOGERR, tr("Unable to open recent. Unable to find %1 with datatype %2 in recent files list.").arg(_action->text()).arg(actionTypeName) );

}

//-----------------------------------------------------------------------------

void
Core::writeOnExit() {
  QString inifile = OpenFlipper::Options::configDirStr()  + OpenFlipper::Options::dirSeparator() + "OpenFlipper.ini";

  INIFile ini;

  if ( ! ini.connect( inifile,true) ) {
    emit log(LOGERR,tr("Can not create user ini file"));
  } else {
    writeApplicationOptions(ini);
    ini.disconnect();
  }


  //store the windowState
  if ( OpenFlipper::Options::gui() ) {

    QSettings windowStates(OpenFlipper::Options::configDirStr()  + OpenFlipper::Options::dirSeparator() + "WindowStates.dat", QSettings::IniFormat);

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
   for (uint i = 0 ; i < plugins_.size() ; ++i) {
      BaseInterface* basePlugin = qobject_cast< BaseInterface * >(plugins_[i].plugin);
      if ( basePlugin )
          if ( checkSlot( plugins_[i].plugin , "exit()" ) )
            QMetaObject::invokeMethod(plugins_[i].plugin, "exit",  Qt::DirectConnection);
   }
}

//-----------------------------------------------------------------------------

void Core::slotExit() {
  // Write all information on application exit
  writeOnExit();
  
  // Call clearAll() before closing application
  // in order to call all object's destructors...
  clearAll();
  
  // close the log file to ensure everything is writeen correctly
  if (logFile_)
    logFile_->close();

  // Close the settings file
  OpenFlipper::Options::closeSettings();

  // Test if ini-file should be cleaned
  // If so, delete it...
  if(OpenFlipper::Options::deleteIniFile()) {
      bool success = true;
    
      // Iterate over all ini files and clear them
      QStringList optionFiles = OpenFlipper::Options::optionFiles();
      for ( int i = 0 ; i < (int)optionFiles.size(); ++i) {
          success &= QFile::remove(optionFiles[i]);
      }
      
      if(!success) {
          QMessageBox::warning(0, tr("Warning"),
                               tr("One or more files could not be removed.\nDelete files manually."),
                               QMessageBox::Ok,
                               QMessageBox::Ok);
      }
  }
  
  // Cleanup the widgets here
  delete coreWidget_;
  
  qApp->quit();
}

//-----------------------------------------------------------------------------

void Core::exitFailure() {

  // Call clearAll() before closing application
  // in order to call all object's destructors...
  clearAll();

  // Cleanup the widgets here
  delete coreWidget_;


  //stop qt event loop
  //this function does return to the caller
  qApp->exit(EXIT_FAILURE);

  // Kill application with an error
  // No need to clean up here anyway
  exit(EXIT_FAILURE);

}

//-----------------------------------------------------------------------------

/// log to file
void Core::slotLogToFile(Logtype _type, QString _message){

  if (!OpenFlipperSettings().value("Core/Log/logFileEnabled",true).toBool() )
    return;

  if (logStream_ == 0){
    //check if a logfile has been specified and if the path is valid

    QString fileName = OpenFlipperSettings().value("Core/Log/logFile","").toString();
    QFileInfo fi( fileName );

    if ( fileName == "" || !fi.dir().exists() ) {
      OpenFlipperSettings().setValue("Core/Log/logFile", OpenFlipper::Options::configDirStr()  + OpenFlipper::Options::dirSeparator() +  "OpenFlipper.log");
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

  for (uint i=0; i < plugins_.size(); i++)
    if (plugins_[i].plugin == sender())
      pluginInfo = &plugins_[i];

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

/// set descriptions for a scriptable slot
void Core::slotSetSlotDescriptionGlobalFunction(QString      _functionName,   QString _slotDescription,
                                                QStringList _parameters,      QStringList _descriptions)
{
  SlotInfo info;
  info.slotName        = _functionName;
  info.slotDescription = _slotDescription;
  info.parameters      = _parameters;
  info.descriptions    = _descriptions;

  globalFunctions_.push_back( info );
}

/// get available Descriptions for a scriptable slot
void Core::slotGetDescription(QString      _function,   QString&     _fnDescription,
                              QStringList& _parameters, QStringList& _descriptions )
{
  QString pluginName = _function.section(".", 0, 0);
  QString slotName   = _function.section(".", 1, 1);

  // Global function
  if ( !_function.contains(".") ) {

    // Only one section, so we have to swap
    slotName = pluginName;

    for (int i=0; i < globalFunctions_.count(); i++) {

      if (globalFunctions_[i].slotName == slotName){
        _fnDescription = globalFunctions_[i].slotDescription;
        _parameters    = globalFunctions_[i].parameters;
        _descriptions  = globalFunctions_[i].descriptions;
        return;
      }
    }
    return;
  }

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

  for (uint i=0; i < plugins_.size(); i++)
    if (plugins_[i].rpcName == pluginName)
      pluginInfo = &plugins_[i];

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

void Core::snapshotBaseFileName(QString _fname, unsigned int _viewerId ){

  if ( OpenFlipper::Options::gui() ) {
    if ( _viewerId >= OpenFlipper::Options::examinerWidgets() ) {
      emit log(LOGERR,tr("Unable to snapshotBaseFileName for viewer ") + QString::number(_viewerId) );
      return;
    }

    PluginFunctions::viewerProperties(_viewerId).snapshotBaseFileName( _fname );
  }

}

void Core::snapshotFileType(QString _type, unsigned int _viewerId ){
  
  if ( OpenFlipper::Options::gui() ) {
    if ( _viewerId >= OpenFlipper::Options::examinerWidgets() ) {
      emit log(LOGERR,tr("Unable to snapshotFileType for viewer ") + QString::number(_viewerId) );
      return;
    }
    
    PluginFunctions::viewerProperties(_viewerId).snapshotFileType( _type );
  }
  
}


void Core::snapshotCounterStart(const int _counter, unsigned int _viewerId ){

  if ( OpenFlipper::Options::gui() ) {
    if ( _viewerId >= OpenFlipper::Options::examinerWidgets() ) {
      emit log(LOGERR,tr("Unable to snapshotFileType for viewer ") + QString::number(_viewerId) );
      return;
    }
    
    PluginFunctions::viewerProperties(_viewerId).snapshotCounter( _counter );
  }
  
}

void Core::snapshot( unsigned int _viewerId, int _width, int _height, bool _alpha, bool _hideCoordsys, int _numSamples ){


  if ( OpenFlipper::Options::gui() ) {
    if ( _viewerId >= OpenFlipper::Options::examinerWidgets() ) {
      emit log(LOGERR,tr("Unable to create snapshot for viewer ") + QString::number(_viewerId) );
      return;
    }
    coreWidget_->examiner_widgets_[_viewerId]->snapshot(_width, _height, _alpha, _hideCoordsys, _numSamples);
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

    // offset +6, +6: Verified: resizing with current sizes leads to no effect
    coreWidget_->glView_->resize(_width+6, _height+6);
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
  if ( OpenFlipper::Options::isWindows() )
    ini.add_entry( "Core" , "VersionWindows" , OpenFlipper::Options::coreVersion() );
  else
    ini.add_entry( "Core" , "VersionLinux"   , OpenFlipper::Options::coreVersion() );

  //add pluginVersions
  for (uint i=0; i < plugins_.size(); i++){

    if ( OpenFlipper::Options::isWindows() )
      ini.add_entry( plugins_[i].name , "VersionWindows" , plugins_[i].version );
    else
      ini.add_entry( plugins_[i].name , "VersionLinux"   , plugins_[i].version );
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

  emit slotSetSlotDescriptionGlobalFunction("printToFile(QString,QString)", tr("Print a message to a file"), QStringList(QString("Filename;Values").split(";")), QStringList(QString("Filename to print into;Arbitrary number of arguments").split(";")));
  emit slotSetSlotDescriptionGlobalFunction("help(QString)", tr("Print help about something"), QStringList("Help Entry"), QStringList("help about what?"));


  emit setSlotDescription("deleteObject(int)", tr("Delete an object from the scene."), QStringList("ObjectId"), QStringList(tr("Id of the object to delete")));
  emit setSlotDescription("updateView()", tr("Redraw the contents of the viewer."), QStringList(), QStringList());
  emit setSlotDescription("clearAll()", tr("Clear all data objects."), QStringList(), QStringList());
  emit setSlotDescription("exitApplication()", tr("Quit OpenFlipper"), QStringList(), QStringList());
  emit setSlotDescription("fullscreen(bool)", tr("Enable or disable fullscreen mode"),
                           QStringList(tr("enabled")) ,
                           QStringList(tr("Enable or disable fullscreen mode")));
  emit setSlotDescription("showViewModeControls(bool)", tr("Show or hide the view mode control box"),
                           QStringList(tr("Show?")) ,
                           QStringList());                           
  emit setSlotDescription("loggerState(int)", tr("Change the logger window state"), QStringList(tr("Change the logger window state")), QStringList());
  emit setSlotDescription("enableOpenMeshErrorLog(bool)", tr("Enable or disable OpenMesh error logging"), QStringList(tr("OpenMesh error logging enabled")), QStringList());
  emit setSlotDescription("showToolbox(bool)", tr("Show or hide toolbox"), QStringList(tr("Show or hide the toolbox")), QStringList());
  emit setSlotDescription("showStatusBar(bool)", tr("Show or hide status bar"), QStringList(tr("Show or hide the status bar")), QStringList());
  emit setSlotDescription("multiViewMode(int)", tr("Switch MultiView Mode"),
                          QStringList(tr("Mode")), QStringList(tr("0: One Viewer\n1: Double Viewer\n2: Grid \n3: Horizontal split ")));

  emit setSlotDescription("restrictFrameRate(bool)", tr("Restrict FrameRate to MaxFrameRate"),
                        QStringList(tr("enabled")), QStringList(tr("restriction switch")));
  emit setSlotDescription("setMaxFrameRate(int)", tr("set the maximal framerate (automatically enables framerate restriction)"),
                        QStringList(tr("frameRate")), QStringList(tr("Maximum frameRate")));
  emit setSlotDescription("snapshotBaseFileName(QString)", tr("Set a filename for storing snapshots.")
                          , QStringList(), QStringList());
  emit setSlotDescription("snapshotFileType(QString)", tr("Set a filetype for storing snapshots.")
                          , QStringList(), QStringList());
  emit setSlotDescription("snapshotCounterStart(int)", tr("Set the starting number for the snapshot counter.")
                          , QStringList("StartValue"), QStringList("Starting number for the counter"));
  emit setSlotDescription("snapshot()", tr("Make a snapshot of the viewer. If no filename"
                          " was set using snapshotBaseFileName() the snapshot is stored"
                          " in snap.png in the current directory. The captured image will have "
                          " the same dimensions as the current viewport. "
                          "For every snapshot a counter is added to the filename."), QStringList(), QStringList());
  emit setSlotDescription("snapshot(uint)", tr("Make a snapshot of the viewer with id viewerId. If no filename"
                          " was set using snapshotBaseFileName() the snapshot is stored"
                          " in snap.png in the current directory. For every snapshot"
                          " a counter is added to the filename."), QStringList("viewerId"), QStringList("Id of viewer to be captured (default is 0)"));
  emit setSlotDescription("snapshot(uint,int,int)", tr("Make a snapshot of the viewer with id viewerId."
                          " Pass 0 as viewerId parameter to capture the current viewer. "
                          " The captured image will have the specified dimensions. "
                          " If 0 is passed as either width or height parameter, the value will "
                          " automatically be set to hold the right aspect ratio, respectively. "
                          " If no filename was set using snapshotBaseFileName() the snapshot is stored"
                          " in snap.png in the current directory. For every snapshot"
                          " a counter is added to the filename."), QStringList(QString("viewerId;width;height").split(";")),
                          QStringList(QString("Id of viewer (default is 0);Width of image;Height of image").split(";")));
  emit setSlotDescription("snapshot(uint,int,int,bool)", tr("Make a snapshot of the viewer with id viewerId."
                          " Pass 0 as viewerId parameter to capture the current viewer. "
                          " The captured image will have the specified dimensions. "
                          " If 0 is passed as either width or height parameter, the value will "
                          " automatically be set to hold the right aspect ratio, respectively. "
                          " If 0 is passed for both width and height values, the viewport's current "
                          " dimension is used. Set alpha to true if you want the background to be transparent. "
                          " If no filename was set using snapshotBaseFileName() the snapshot is stored"
                          " in snap.png in the current directory. For every snapshot"
                          " a counter is added to the filename."), QStringList(QString("viewerId;width;height;alpha").split(";")),
                          QStringList(QString("Id of viewer (default is 0);Width of image;Height of image;Transparent background").split(";")));
  emit setSlotDescription("snapshot(uint,int,int,bool,bool)", tr("Make a snapshot of the viewer with id viewerId."
                          " Pass 0 as viewerId parameter to capture the current viewer. "
                          " The captured image will have the specified dimensions. "
                          " If 0 is passed as either width or height parameter, the value will "
                          " automatically be set to hold the right aspect ratio, respectively. "
                          " If 0 is passed for both width and height values, the viewport's current "
                          " dimension is used. Set alpha to true if you want the background to be transparent. "
                          " The fifth parameter is used to hide the coordinate system in the upper right corner of the screen. "
                          " If no filename was set using snapshotBaseFileName() the snapshot is stored"
                          " in snap.png in the current directory. For every snapshot"
                          " a counter is added to the filename."), QStringList(QString("viewerId;width;height;alpha;hideCoordsys").split(";")),
                          QStringList(QString("Id of viewer (default is 0);Width of image;Height of image;Transparent background;Hide coordsys node").split(";")));
  emit setSlotDescription("snapshot(uint,int,int,bool,bool,int)", tr("Make a snapshot of the viewer with id viewerId."
                          " Pass 0 as viewerId parameter to capture the current viewer. "
                          " The captured image will have the specified dimensions. "
                          " If 0 is passed as either width or height parameter, the value will "
                          " automatically be set to hold the right aspect ratio, respectively. "
                          " If 0 is passed for both width and height values, the viewport's current "
                          " dimension is used. Set alpha to true if you want the background to be transparent. "
                          " The fifth parameter is used to hide the coordinate system in the upper right corner of the screen. "
                          " If no filename was set using snapshotBaseFileName() the snapshot is stored"
                          " in snap.png in the current directory. For every snapshot"
                          " a counter is added to the filename."), QStringList(QString("viewerId;width;height;alpha;hideCoordsys;numSamples").split(";")),
                          QStringList(QString("Id of viewer (default is 0);Width of image;Height of image;Transparent background;Hide coordsys node;Number of samples per pixel").split(";")));
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
  emit setSlotDescription("saveSettings(QString, bool, bool, bool, bool, bool, bool)", tr("Save the current setting to the supplied file."),
                          QStringList(tr("filePath,is_saveObjectInfo,is_targetOnly,is_saveAll,is_askOverwrite,is_saveProgramSettings,is_savePluginSettings").split(",")),
                          QStringList(tr("Path of the file to save the settings to.;Save objects in current setting.;Restrict to targeted objects.;<no idea what this parameter does>;Prompt before overwriting files that already exist (gui mode only).;Save "TOSTRING(PRODUCT_NAME)" program settings.;Save plugin settings.").split(";")));
  //load slots
  emit setSlotDescription("loadObject()", tr("Show the dialog to load an object. (only works if GUI is available)"),QStringList(), QStringList());
  emit setSlotDescription("loadObject(QString)", tr("Load an object specified in file filename. This automatically determines which file plugin to use."),
                          QStringList(tr("filename")), QStringList(tr("Filename")));
  emit setSlotDescription("getObjectId(QString)", tr("Return identifier of object with specified name. Returns -1 if object was not found."),QStringList(), QStringList());
  emit setSlotDescription("loadSettings()", tr("Show the dialog to load settings. (only works if GUI is available)"),QStringList(), QStringList());
  emit setSlotDescription("loadSettings(QString)", tr("load settings from file."),QStringList(), QStringList());

  emit setSlotDescription("createWidget(QString,QString)", tr("Create a widget from an ui file"),
                          QString(tr("Object name,ui file")).split(","),
                          QString(tr("Name of the new widget in script,ui file to load")).split(","));

  emit setSlotDescription("addToolbox(QString,QWidget*)", tr("Add a widget as a toolbox"),
                          QString(tr("Toolbox Entry name,Widget")).split(","),
                          QString(tr("Name of the new widget in the toolbox,Pointer to the new widget")).split(","));

  emit setSlotDescription("addToolbox(QString,QWidget*,QIcon*)", tr("Add a widget as a toolbox"),
                          QString(tr("Toolbox Entry name,Widget,Icon")).split(","),
                          QString(tr("Name of the new widget in the toolbox,Pointer to the new widget,Pointer to icon")).split(","));

  emit setSlotDescription("addViewModeToolboxes(QString,QString)", tr("Set toolboxes for a viewmode (This automatically adds the view mode if it does not exist)"),
                          QString(tr("Name,Toolbox List")).split(","),
                          QString(tr("Name of the Viewmode,seperated list of toolboxes visible in this viewmode")).split(","));

  emit setSlotDescription("addViewModeToolbars(QString,QString)", tr("Set toolbars for a viewmode (This automatically adds the view mode if it does not exist)"),
                          QString(tr("Name,Toolbar List")).split(","),
                          QString(tr("Name of the Viewmode,seperated list of toolbars visible in this viewmode")).split(","));
                          
  emit setSlotDescription("addViewModeContextMenus(QString,QString)", tr("Set context Menus for a viewmode (This automatically adds the view mode if it does not exist)"),
                          QString(tr("Name,Context Menu List")).split(","),
                          QString(tr("Name of the Viewmode,seperated list of Context Menus visible in this viewmode")).split(","));    
                          
  emit setSlotDescription("addViewModeIcon(QString,QString)", tr("Set Icon for a viewmode (This automatically adds the view mode if it does not exist)"),
                          QString(tr("Name,Icon filename")).split(","),
                          QString(tr("Name of the Viewmode,filename of the icon (will be taken from OpenFlippers icon directory)")).split(","));                                                    

  emit setSlotDescription("objectList(QString,QStringList)", tr("Returns object list"),
                          QString(tr("Selection type,Object types")).split(","),
                          QString(tr("Type of object selection (all,source,target),Object type (All,PolyMesh,TriangleMesh,...)")).split(";"));

  emit setSlotDescription("setToolBoxSide(QString)", tr("Determine whether the toolboxes should be displayed on the right or on the left side."),
                          QStringList(tr("side")), QStringList(tr("The desired side of the toolboxes (either 'left' or 'right')")));

  emit setSlotDescription("getToolbox(QString,QString)", tr("Returns a pointer to the requested toolbox widget if it was found, nullptr, otherwise."),
                          tr("Plugin Name\rToolbox Name").split("\r"),
                          tr("The plugin which the requested toolbox belongs to.\rThe name of the requested toolbox.").split("\r"));

  emit setSlotDescription("blockSceneGraphUpdates()", tr("Disable Scenegraph Updates (e.g. before loading or adding a large number of files)"),QStringList(), QStringList());
  emit setSlotDescription("unblockSceneGraphUpdates()", tr("Enable Scenegraph Updates (e.g. before loading or adding a large number of files)"),QStringList(), QStringList());
  emit setSlotDescription("setView", tr("Set the encoded view for the primary viewport."), QStringList(tr("view")), QStringList(tr("The encoded view. (You can obtain one through \"Copy View\" in the context menu of the coordinates.)")));

}

void Core::deleteObject( int _id ){

  if ( _id == -1 )
    return;

  // get the node
  BaseObject* object = objectRoot_->childExists(_id);

  if ( !object || object == objectRoot_ ) {
    std::cerr << "Error while deleting object, does not exist!!" << std::endl;
    return;
  }
  
  emit objectDeleted(_id);

  // remove the whole subtree below this item
  object->deleteSubtree();

  // remove the item itself from the parent
  object->parent()->removeChild(object);

  // delete it
  delete object;

}


void Core::setObjectComment(int _id, QString key, QString comment) {
    if ( _id == -1 ) return;

    BaseObject* object = objectRoot_->childExists(_id);

    if (!object || object == objectRoot_) {
        std::cerr << "No such object." << std::endl;
        return;
    }

    object->getCommentByKey(key) = comment;
}

void Core::clearObjectComment(int _id, QString key) {
    if ( _id == -1 ) return;

    BaseObject* object = objectRoot_->childExists(_id);

    if (!object || object == objectRoot_) {
        std::cerr << "No such object." << std::endl;
        return;
    }

    object->clearComment(key);
}

void Core::clearAllComments(int _id) {
    if ( _id == -1 ) return;

    BaseObject* object = objectRoot_->childExists(_id);

    if (!object || object == objectRoot_) {
        std::cerr << "No such object." << std::endl;
        return;
    }

    object->clearAllComments();
}


void Core::slotDeleteAllObjects( ){

  // Remember ids
  std::vector< int > ids;

  BaseObject* current = objectRoot_->next();

  while( current != objectRoot_ ){
    ids.push_back( current->id() );
    current = current->next();
  }
  
  for ( uint i = 0 ; i < ids.size(); ++i ) {
    emit objectDeleted(ids[i]);
  }

  // remove the whole subtree below the root
  objectRoot_->deleteSubtree();

  slotObjectUpdated(-1);

  emit allCleared();
}

//-----------------------------------------------------------------------------

bool Core::checkLibraryVersions()  {
  
  bool ok   = true;
  bool warn = false;
  
  QString messages;
  
  QString qtCompiledVersion = QString( QT_VERSION_STR );
  QString qtCurrentVersion = qVersion();
  
  if ( qtCompiledVersion != qtCurrentVersion ) {
    messages += tr("QT Library Version mismatch!\n");
    
    messages += tr("Currently used QT Version:\t") + qVersion() + "\n";
    messages += tr("Link time QT Version:\t\t") + QString( QT_VERSION_STR ) + "\n";
    messages += tr("This inconsistency may lead to an unstable behavior of OpenFlipper!");
    
    warn = true;
  }
  
  if ( !ok ) {
    QString message = tr("Error! Library tests failed!\n");
    message += tr("The following problems have been found:\n\n");
    
    message +=  messages;
    
    std::cerr << message.toStdString() << std::endl;
    
    if ( OpenFlipper::Options::gui() ) {
      QMessageBox::critical ( 0, tr( "Library incompatibilities found!"),message );
    }
    
    // Unsafe operation, so quit the application
    exitFailure();
    
  } else if ( warn ) {
    
    QString message = tr("Warning! The OpenGL capabilities of your current machine/driver could be insufficient!\n\n");
    message += tr("The following checks failed:\n\n");
    message += messages;
    
    std::cerr << message.toStdString() << std::endl;
    
    if ( OpenFlipper::Options::gui() ) {
      QMessageBox::warning ( 0, tr( "Library incompatibilities found!"),message );
    }
    
  }
  #ifndef NDEBUG
  else {
    std::cerr << "Library Check succeeded" << std::endl;
    return true;
  }
  #endif
  
  return true;
}

//-----------------------------------------------------------------------------


bool Core::checkOpenGLCapabilities()  {
  
  // No gui->no OpenGL
  if ( OpenFlipper::Options::nogui() )
    return true;
  
  // Status ok?
  bool ok   = true;
  bool warn = false;
  
  QString missing;
  
  // We need at least version 2.0 or higher 
  QGLFormat::OpenGLVersionFlags flags = QGLFormat::openGLVersionFlags();
  
  if ( QGLFormat::hasOpenGL() ) {
    if ( flags.testFlag(QGLFormat::OpenGL_Version_None) ) {
      missing += tr("OpenGL Version Unknown to QT!\n");
      missing += tr("OpenGL reports version: ") + QString((const char*)glGetString( GL_VERSION  )) ;
      warn = true;
    } else {
      if ( !( flags.testFlag(QGLFormat::OpenGL_Version_3_0) | 
              flags.testFlag(QGLFormat::OpenGL_Version_2_1) | 
              flags.testFlag(QGLFormat::OpenGL_Version_2_0) ) ) {
        ok = false; 
        missing += tr("OpenGL Version less then 2.0!\n");
      } 
    }
  
  } else {
   ok = false;
   missing += tr("No OpenGL support found!\n");
  }
  
  //Get OpenGL extensions
  QString glExtensions = QString((const char*)glGetString(GL_EXTENSIONS));
  
  // Vertex buffer objects used heavily in mesh node and almost all other nodes
  if ( !glExtensions.contains("GL_ARB_vertex_buffer_object") ) {
    ok = false; 
    missing += tr("Your graphics card does not support the GL_ARB_vertex_buffer_object extension!\n");
  }
  

  if ( !ok ) {
    QString message = tr("Error! \nThe OpenGL capabilities of your current machine/driver are not sufficient!\n\n");
    message += tr("The following checks failed:\n\n");
    message += missing;
    message += tr("\n\nPlease update your driver or graphics card.\n");
    #ifdef APPLE
      message += tr("If you have more than one GPU (e.g. MacBook) don't use the internal one!\n");
    #endif
    
    std::cerr << message.toStdString() << std::endl;
    
    
    QMessageBox::StandardButton button = QMessageBox::critical ( 0, tr( "Insufficient OpenGL Capabilities!"),message,QMessageBox::Abort|QMessageBox::Ignore , QMessageBox::Abort);
    
    // Unsafe operation, so quit the application
    if ( button == QMessageBox::Abort )
      exitFailure();
    else 
      QMessageBox::warning(0,tr( "Insufficient OpenGL Capabilities!"),tr("Ignoring OpenGL capabilities might lead to unstable Operation! Do it at your own risk!"));
    
    
    
  } else if ( warn ) {
    QString message = tr("Warning! Automatic system environment checks discovered some possible problems!\n\n");
    message += tr("The following checks failed:\n\n");
    message += missing;
        
    std::cerr << message.toStdString() << std::endl;
    
    QMessageBox::warning ( 0, tr( "Detected possible problems!"),message );
    
  }
  #ifndef NDEBUG
  else {
    std::cerr << "OpenGL Version Check succeeded" << std::endl;
  }
  #endif
  
  return ok;
}

void Core::showReducedMenuBar(bool reduced) {
    coreWidget_->showReducedMenuBar(reduced);
}

void Core::finishSplash() {
    splash_->finish(coreWidget_);
}


//=============================================================================
