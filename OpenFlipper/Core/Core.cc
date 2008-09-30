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
//  CLASS Core - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================

// -------------------- mview
#include "Core.hh"
// -------------------- ACG
#include <ACG/Scenegraph/DrawModes.hh>

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

#include <QPluginLoader>
#include "OpenFlipper/BasePlugin/BaseInterface.hh"
#include "OpenFlipper/BasePlugin/KeyInterface.hh"
#include "OpenFlipper/BasePlugin/MouseInterface.hh"
#include "OpenFlipper/BasePlugin/PickingInterface.hh"
#include "OpenFlipper/BasePlugin/ToolboxInterface.hh"
#include "OpenFlipper/BasePlugin/TextureInterface.hh"
#include "OpenFlipper/BasePlugin/MenuInterface.hh"
#include "OpenFlipper/BasePlugin/INIInterface.hh"
#include "OpenFlipper/BasePlugin/GlobalAccessInterface.hh"

#include "OpenFlipper/INIFile/INIFile.hh"

#include "OpenFlipper/common/GlobalOptions.hh"
#include <OpenFlipper/common/RecentFiles.hh>
#include <OpenFlipper/ACGHelper/DrawModeConverter.hh>

#include <QStringList>
#include <QtScript/QScriptValueIterator>

#include <ACG/Scenegraph/SeparatorNode.hh>

#include "OpenFlipper/BasePlugin/PluginFunctions.hh"

#include <OpenMesh/Core/System/omstream.hh>

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
  nextBackupId_(0),
  set_random_base_color_(true),
  coreWidget_(0)
{
  //init logFile
  logStream_ = 0;
  logFile_ = 0;
  OpenFlipper::Options::logFileEnabled(true);

  //init nodes
  root_node_scenegraph_ = new ACG::SceneGraph::SeparatorNode(0, "SceneGraph Root Node");
  root_node_ = new ACG::SceneGraph::SeparatorNode(root_node_scenegraph_, "Data Root Node");

   // Add ViewMode All
  ViewMode* vm = new ViewMode();
  vm->name = "All";
  vm->custom = false;
  vm->visibleWidgets = QStringList();

  viewModes_.push_front(vm);

  // Get all relevant Paths and Options from option files
  setupOptions();
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
  PluginFunctions::set_rootNode( root_node_ );

  PluginFunctions::set_sceneGraphRootNode( root_node_scenegraph_ );

  // Initialize the first object as the root Object for the object tree
  objectRoot_ =  dynamic_cast< BaseObject* > ( new GroupObject("ObjectRoot") );
  PluginFunctions::setDataRoot( objectRoot_ );


  if ( OpenFlipper::Options::gui() ) {

    redrawTimer_ = new QTimer();
    redrawTimer_->setSingleShot(true);
    connect(redrawTimer_, SIGNAL(timeout()), this, SLOT(updateView()),Qt::DirectConnection);


    if ( OpenFlipper::Options::splash() ) {
      QPixmap splashPixmap(OpenFlipper::Options::iconDirStr() + OpenFlipper::Options::dirSeparator() + "splash.png");

      splash_ = new QSplashScreen(splashPixmap);
      splash_->show();

      splash_->showMessage("Initializing mainwindow" ,
                          Qt::AlignBottom | Qt::AlignLeft , Qt::white);
      QApplication::processEvents();
    }

    coreWidget_ = new CoreWidget(viewModes_ , plugins);

    connect(coreWidget_, SIGNAL(clearAll())           , this, SLOT(clearAll()));
    connect(coreWidget_, SIGNAL(loadMenu())           , this, SLOT(slotLoadMenu()));
    connect(coreWidget_, SIGNAL(addEmptyObjectMenu()) , this, SLOT(slotAddEmptyObjectMenu()));
    connect(coreWidget_, SIGNAL(saveMenu())           , this, SLOT(slotSaveMenu()));
    connect(coreWidget_, SIGNAL(saveToMenu())         , this, SLOT(slotSaveToMenu()));
    connect(coreWidget_, SIGNAL(loadIniMenu())        , this, SLOT(slotLoadIniMenu()));
    connect(coreWidget_, SIGNAL(saveIniMenu())        , this, SLOT(slotSaveIniMenu()));
    connect(coreWidget_, SIGNAL(applyOptions())       , this, SLOT(applyOptions()));
    connect(coreWidget_, SIGNAL(saveOptions())        , this, SLOT(saveOptions()));
    connect(coreWidget_, SIGNAL(recentOpen(QAction*)) , this, SLOT(slotRecentOpen(QAction*)));
    connect(coreWidget_, SIGNAL(exit())               , this, SLOT(slotExit()));

    connect(coreWidget_, SIGNAL(loadPlugin())         , this, SLOT(slotLoadPlugin()));
    connect(coreWidget_, SIGNAL(unloadPlugin())       , this, SLOT(slotUnloadPlugin()));

    coreWidget_->resize(1000,1000);

    coreWidget_->setWindowTitle( OpenFlipper::Options::windowTitle() );

    // Make examiner available to the plugins ( defined in PluginFunctions.hh)
    PluginFunctions::set_examiner( coreWidget_->examiner_widget_ );


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
  // This connection will tell the plugins, when their Toolbox is active
  // ======================================================================
  ///@todo reimplement
//   connect(module_list,SIGNAL(currentChanged(int)),this,SLOT(slotToolboxSwitched(int)));

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


  // Register idList Type to scripting Engine
  qScriptRegisterSequenceMetaType< idList >(&scriptEngine_);

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
  emit log(LOGOUT,"Core Scripting initialized with Name : core  ");

  emit log(LOGOUT,"Available scripting functions :");

  QScriptValueIterator it(scriptInstance);
  while (it.hasNext()) {
    it.next();

    /// Skip all signals for function calls
    if ( checkSignal( this, it.name().toAscii() ) )
      continue;

    scriptingFunctions_.push_back( "core." + it.name() );

    emit log(LOGOUT,"\t" + it.name());
  }

  emit log(LOGOUT,"=============================================================================================");

  loadPlugins();

  if ( OpenFlipper::Options::gui() ) {

    if ( OpenFlipper::Options::defaultToolboxMode( ) != "" )
      coreWidget_->setViewMode( OpenFlipper::Options::defaultToolboxMode() );
    else
      coreWidget_->setViewMode("All");

    connect( coreWidget_->examiner_widget_, SIGNAL(signalMouseEvent(QMouseEvent*)),
            this,SLOT(slotMouseEvent(QMouseEvent*)));
    connect( coreWidget_->examiner_widget_, SIGNAL(signalMouseEventIdentify(QMouseEvent*)),
            this,SLOT(slotMouseEventIdentify(QMouseEvent*)));
    connect( coreWidget_->examiner_widget_, SIGNAL(signalWheelEvent(QWheelEvent *, const std::string &)),
            this,                           SLOT(slotWheelEvent(QWheelEvent *, const std::string &)));

  }

  QStringList optionFiles = OpenFlipper::Options::optionFiles();
  for ( int i = 0 ; i < (int)optionFiles.size(); ++i) {

    if ( OpenFlipper::Options::gui() && OpenFlipper::Options::splash() ) {
      splash_->showMessage("Loading Configuration File " + QString::number(i) + "/"  + QString::number(optionFiles.size()) ,
                           Qt::AlignBottom | Qt::AlignLeft , Qt::white);
      QApplication::processEvents();
    }

    openIniFile( optionFiles[i] );
  }


  if ( OpenFlipper::Options::lang().contains("UTF") || OpenFlipper::Options::lang().contains("utf") ) {
    emit log(LOGWARN,"Warning, OpenFlipper detected that you are using an utf-8 locale!");
    emit log(LOGWARN,"Only OFF files are fully supported with UTF8. Others might fail.");
    emit log(LOGWARN,"You can change your locale by :");
    emit log(LOGWARN,"export LANG=C");
    emit log(LOGWARN,"Work is in progress to resolve this issue.");
  }

  applyOptions();

  if ( OpenFlipper::Options::gui() ) {

    //try to restore the windowState
    QFile file(QDir::home().absolutePath() + OpenFlipper::Options::dirSeparator() + ".OpenFlipper" +
                                                  OpenFlipper::Options::dirSeparator() +  "windowState.dat");
    if (file.open(QIODevice::ReadOnly)){
      QByteArray bytes = file.readAll();

      coreWidget_->restoreState( bytes );

      file.close();
    }

    //try to restore the geometry
    QFile file2(QDir::home().absolutePath() + OpenFlipper::Options::dirSeparator() + ".OpenFlipper" +
                                                  OpenFlipper::Options::dirSeparator() +  "geometry.dat");
    if (file2.open(QIODevice::ReadOnly)){
      QByteArray bytes = file2.readAll();

      coreWidget_->restoreGeometry( bytes );

      file2.close();
    }


    coreWidget_->show();

    if ( OpenFlipper::Options::splash() ) {
      splash_->finish(coreWidget_);
    }

  }

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

  objectRoot_->deleteSubtree();
  delete objectRoot_;

  // Clean up loggers
  for ( uint i = 0 ; i < loggers_.size(); ++i )
    delete loggers_[i];

}

//-----------------------------------------------------------------------------

void
Core::slotMouseEventIdentify( QMouseEvent* _event )
{
  // Dont do anything as a context Menu will popup on right button click
  if ( _event->button() == Qt::RightButton )
    return;

  emit PluginMouseEventIdentify( _event );
}

//-----------------------------------------------------------------------------


void
Core::slotMouseEvent( QMouseEvent* _event )
{
  // Dont do anything as a context Menu will popup on right button click
  if ( _event->button() == Qt::RightButton )
    return;

  emit PluginMouseEvent(_event );
}

//-----------------------------------------------------------------------------

void
Core::slotWheelEvent( QWheelEvent * _event, const std::string & _mode)
{
  emit PluginWheelEvent(_event , _mode );
}

//-----------------------------------------------------------------------------

void
Core::slotAddPickMode( const std::string _mode ) {
  if ( OpenFlipper::Options::gui() )
    coreWidget_->examiner_widget_->addPickMode(_mode);
}

//-----------------------------------------------------------------------------

void
Core::slotAddHiddenPickMode( const std::string _mode ) {
  if ( OpenFlipper::Options::gui() )
    coreWidget_->examiner_widget_->addPickMode(_mode,false,1000,false);
}

//-----------------------------------------------------------------------------

void
Core::slotAddPickMode( const std::string _mode , QCursor _cursor) {
  if ( OpenFlipper::Options::gui() )
    coreWidget_->examiner_widget_->addPickMode(_mode,false,1000,true,_cursor);
}

//-----------------------------------------------------------------------------

void
Core::slotAddHiddenPickMode( const std::string _mode , QCursor _cursor) {
  if ( OpenFlipper::Options::gui() )
    coreWidget_->examiner_widget_->addPickMode(_mode,false,1000,false, _cursor);
}



//-----------------------------------------------------------------------------

 /** Update the view in the examiner widget
  */
void Core::updateView() {

  if ( OpenFlipper::Options::doSlotDebugging() ) {
    if ( sender() != 0 ) {
      if ( sender()->metaObject() != 0 ) {
        emit log(LOGINFO,"updateView() called by " + QString( sender()->metaObject()->className() ) );
      }
    }
  }

  if ( OpenFlipper::Options::restrictFrameRate() ) {

    // redraw time not reached ... waiting for timer event for next redraw
    if ( redrawTimer_->isActive() ) {
      if ( OpenFlipper::Options::doSlotDebugging() )
        emit log(LOGINFO,"Too early for redraw! Delaying request from " + QString( sender()->metaObject()->className() ) );
      return;
    }

//     std::cerr << "Redraw" << std::endl;

    // Start the timer if we are not called by the timer
    if ( sender() != redrawTimer_ ) {
      redrawTimer_->start( 1000 / OpenFlipper::Options::maxFrameRate() );
    }

  }


  if ( OpenFlipper::Options::gui() && !OpenFlipper::Options::openingIni() && !OpenFlipper::Options::redrawDisabled() ) {
    coreWidget_->examiner_widget_->sceneGraph(root_node_scenegraph_);
    coreWidget_->examiner_widget_->updateGL();
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
}

//-----------------------------------------------------------------------------

void
Core::clearAll()
{
  objectRoot_->deleteSubtree();
  emit allCleared();
  emit ObjectListUpdated(-1);

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

void
Core::setDrawMode(QString _mode)
{

  QStringList list = _mode.split(';');

  std::vector< QString > drawModeList;

  for ( int i = 0 ; i < list.size() ; ++i )
    drawModeList.push_back(list[i]);

  unsigned int mode = ListToDrawMode(drawModeList);

  PluginFunctions::setDrawMode( mode );
  emit updateView();
}


//-----------------------------------------------------------------------------

void Core::translate( Vector _vec ) {
  PluginFunctions::translate( _vec );
}

//-----------------------------------------------------------------------------

void Core::rotate( Vector _axis, double _angle, Vector _center ) {
  PluginFunctions::rotate( _axis, _angle, _center );
}

//-----------------------------------------------------------------------------

void Core::setViewingDirection( Vector _direction, Vector _upvector ) {
  PluginFunctions::viewingDirection(_direction, _upvector);
}

//-----------------------------------------------------------------------------

void Core::fullscreen() {
  if ( OpenFlipper::Options::gui() )
    coreWidget_->toggleFullscreen();
}

//-----------------------------------------------------------------------------

void Core::logger() {
  if ( OpenFlipper::Options::gui() )
    coreWidget_->toggleLogger();
}

//-----------------------------------------------------------------------------

void Core::toolbox() {
  if ( OpenFlipper::Options::gui() )
    coreWidget_->toggleToolbox();
}


//-----------------------------------------------------------------------------

void
Core::slotRecentOpen(QAction* _action)
{
  QVector< OpenFlipper::Options::RecentFile > recentFiles = OpenFlipper::Options::recentFiles();
  for (int i = 0 ; i < recentFiles.size() ; ++i )
    if ( recentFiles[i].filename == _action->text() ){
      loadObject(recentFiles[i].type, recentFiles[i].filename);
      break;
    }
}


void
Core::writeOnExit() {
  QString inifile = QDir::home().absolutePath() + OpenFlipper::Options::dirSeparator() + ".OpenFlipper" +
                                                  OpenFlipper::Options::dirSeparator() +  "OpenFlipper.ini";

  INIFile ini;
  if ( ! ini.connect( inifile ,false) ) {
    emit log(LOGERR,"Failed to connect to users ini file");

    if ( ! ini.connect( inifile,true) ) {
      emit log(LOGERR,"Can not create user ini file");
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
    QFile file(QDir::home().absolutePath() + OpenFlipper::Options::dirSeparator() + ".OpenFlipper" +
                                                  OpenFlipper::Options::dirSeparator() +  "windowState.dat");
    if (file.open(QIODevice::WriteOnly))
    {
      file.write( coreWidget_->saveState() );
      file.close();
    }
    QFile file2(QDir::home().absolutePath() + OpenFlipper::Options::dirSeparator() + ".OpenFlipper" +
                                                  OpenFlipper::Options::dirSeparator() +  "geometry.dat");
    if (file2.open(QIODevice::WriteOnly))
    {
      file2.write( coreWidget_->saveGeometry() );
      file2.close();
    }
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

  qApp->quit();
}

/// Synchronise two viewers
bool Core::add_sync_host(const QString& _name)
{
  if ( OpenFlipper::Options::gui() ) {
    emit log(LOGINFO,"Adding SyncHost");
    bool ok = coreWidget_->examiner_widget_->add_sync_host(_name);
    if (ok)
      coreWidget_->examiner_widget_->setSynchronization(true);
    else
      emit log(LOGERR,"Sync failed! ");
    return ok;
  }
  return false;
}

/// log to file
void Core::slotLogToFile(Logtype _type, QString _message){

  if (!OpenFlipper::Options::logFileEnabled())
    return;

  if (logStream_ == 0){
    //check if a logfile has been specified
    if (OpenFlipper::Options::logFile() == "")
        OpenFlipper::Options::logFile(QDir::home().absolutePath() + OpenFlipper::Options::dirSeparator() + ".OpenFlipper" +
                                                  OpenFlipper::Options::dirSeparator() +  "OpenFlipper.log");

    logFile_ = new QFile( OpenFlipper::Options::logFile() );
    if ( logFile_->open(QFile::WriteOnly) ) {
        logStream_ = new QTextStream (logFile_);
    }else{
      emit log(LOGERR, "Unable to open logfile!");
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
