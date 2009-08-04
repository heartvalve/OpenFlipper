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
//  CLASS Core - IMPLEMENTATION of Path and File Handling
//
//=============================================================================


//== INCLUDES =================================================================

// -------------------- mview
#include "Core.hh"
// -------------------- ACG
#include <ACG/Scenegraph/DrawModes.hh>

#include "OpenFlipper/INIFile/INIFile.hh"
#include "OpenFlipper/common/GlobalOptions.hh"
#include <OpenFlipper/BasePlugin/PluginFunctions.hh>

//== IMPLEMENTATION ==========================================================

void Core::applyOptions(){

  if ( OpenFlipper::Options::gui() ) {

    //Init ViewModes
    coreWidget_->initViewModes();

    //Set default Viewmode
    if (OpenFlipper::Options::defaultToolboxMode() != "")
      coreWidget_->slotChangeView(OpenFlipper::Options::defaultToolboxMode(), QStringList());
    //Set Fullscreen
    if ( OpenFlipper::Options::fullScreen() )
      coreWidget_->setWindowState( coreWidget_->windowState() | Qt::WindowFullScreen);
    else
		  coreWidget_->setWindowState( (coreWidget_->windowState() | Qt::WindowFullScreen) ^ Qt::WindowFullScreen);

    // Logger
    coreWidget_->showLogger( OpenFlipper::Options::loggerState() );

    // Prepare Picking Debugger Flag
    ACG::SceneGraph::PickTarget target;
    if ( OpenFlipper::Options::pickingRenderMode() == "PICK_ANYHING") {
      target = ACG::SceneGraph::PICK_ANYTHING;
    } else if ( OpenFlipper::Options::pickingRenderMode() == "PICK_VERTEX") {
      target = ACG::SceneGraph::PICK_VERTEX;
    } else if ( OpenFlipper::Options::pickingRenderMode() == "PICK_EDGE") {
      target = ACG::SceneGraph::PICK_EDGE;
    } else if ( OpenFlipper::Options::pickingRenderMode() == "PICK_FACE") {
      target = ACG::SceneGraph::PICK_FACE;
    } else if ( OpenFlipper::Options::pickingRenderMode() == "PICK_FRONT_VERTEX") {
      target = ACG::SceneGraph::PICK_FRONT_VERTEX;
    } else if ( OpenFlipper::Options::pickingRenderMode() == "PICK_FRONT_EDGE") {
      target = ACG::SceneGraph::PICK_FRONT_EDGE;
    } else {
      target = ACG::SceneGraph::PICK_ANYTHING;
    }

    //set viewer properties
    for (int i=0; i < PluginFunctions::viewers(); i++){

      PluginFunctions::setDrawMode( OpenFlipper::Options::defaultDrawMode(i), i );
      PluginFunctions::setFixedView(OpenFlipper::Options::defaultViewingDirection(i), i );

      //only switch projection here if an object is opened
      //this prevents problems when applying options on app start
      if ( PluginFunctions::objectCount() > 0 ){ 
	if ( OpenFlipper::Options::defaultProjectionMode(i) == 0 )
	  PluginFunctions::orthographicProjection(i);
	else
	  PluginFunctions::perspectiveProjection(i);
      }
    }

    //set defaultBackgroundColor
    QColor c = OpenFlipper::Options::defaultBackgroundColor() ;

    for ( unsigned int i = 0 ; i < OpenFlipper::Options::examinerWidgets() ; ++i ) {
      //wheel zoom factor
      PluginFunctions::viewerProperties(i).wheelZoomFactor(OpenFlipper::Options::wheelZoomFactor());
      PluginFunctions::viewerProperties(i).wheelZoomFactorShift(OpenFlipper::Options::wheelZoomFactorShift());

      // Picking Debugger
      PluginFunctions::viewerProperties(i).renderPicking(OpenFlipper::Options::renderPicking(), target );
    }

    //hideToolbox
    if (OpenFlipper::Options::hideToolbox()) {

      coreWidget_->showToolbox (false);
    }

    //setup logFile
    if (logFile_ != 0){

      if (OpenFlipper::Options::logFile() != logFile_->fileName() ){
        logFile_->close();
        delete logFile_;
        logFile_ = 0;
        if (logStream_ != 0){
          delete logStream_;
          logStream_ = 0;
        }
      }
    }

    updateView();

  }
}

void Core::saveOptions(){
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
}

void Core::setupOptions() {

  // Get the Main config dir in the home directory and possibly create it
  QDir configDir = QDir::home();

  if ( ! configDir.cd(".OpenFlipper") ) {
    emit log(LOGOUT,"Creating config Dir ~/.OpenFlipper");
    configDir.mkdir(".OpenFlipper");
    if ( ! configDir.cd(".OpenFlipper") ) {
      emit log(LOGERR,"Unable to create config dir ~/.OpenFlipper");
      return;
    }
  }
  OpenFlipper::Options::configDir(configDir);

  // Remember the main application directory (assumed to be one above executable Path)
  QDir tempDir = QDir(qApp->applicationDirPath());
  #ifdef OPENFLIPPER_APPDIR
    tempDir.cd(OPENFLIPPER_APPDIR);
  #else
    tempDir.cd(".." + OpenFlipper::Options::dirSeparator() );
  #endif
  OpenFlipper::Options::applicationDir(tempDir.absolutePath());

  // Set the standard path to the plugins
  #ifdef OPENFLIPPER_PLUGINDIR
    tempDir.cd(OPENFLIPPER_PLUGINDIR);
  #else
    tempDir.cd("Plugins");

    #if defined(WIN32)
      tempDir.cd("Windows");
    #elif defined(ARCH_DARWIN)
      tempDir.cd("Darwin");
    #else
      tempDir.cd("Linux");
    #endif

    if ( OpenFlipper::Options::is64bit() )
      tempDir.cd("64");
    else
      tempDir.cd("32");

    #ifdef WIN32
        #ifndef NDEBUG
          #define DEBUG
        #endif
    #endif

    #ifdef DEBUG
            tempDir.cd("Debug");
    #else
            tempDir.cd("Release");
    #endif
  #endif

  OpenFlipper::Options::pluginDir(tempDir.absolutePath());

  // Set the Path to the shaders
  tempDir = QDir(OpenFlipper::Options::applicationDir());
  #ifdef OPENFLIPPER_DATADIR
    tempDir.cd(OPENFLIPPER_DATADIR);
  #endif
  tempDir.cd("Shaders");
  OpenFlipper::Options::shaderDir(tempDir.absolutePath());


  // Set the Path to the textures
  tempDir = QDir(OpenFlipper::Options::applicationDir());
  #ifdef OPENFLIPPER_DATADIR
    tempDir.cd(OPENFLIPPER_DATADIR);
  #endif
  tempDir.cd("Textures");
  OpenFlipper::Options::textureDir(tempDir.absolutePath());

  // Set the Path to the Scripts
  tempDir = QDir(OpenFlipper::Options::applicationDir());
  #ifdef OPENFLIPPER_DATADIR
    tempDir.cd(OPENFLIPPER_DATADIR);
  #endif
  tempDir.cd("Scripts");
  OpenFlipper::Options::scriptDir(tempDir.absolutePath());

  // Set the Path to the Icons
  tempDir = QDir(OpenFlipper::Options::applicationDir());
  #ifdef OPENFLIPPER_DATADIR
    tempDir.cd(OPENFLIPPER_DATADIR);
  #endif
  tempDir.cd("Icons");
  OpenFlipper::Options::iconDir(tempDir.absolutePath());
  
  // Set the Path to the translations
  tempDir = QDir(OpenFlipper::Options::applicationDir());
  #ifdef OPENFLIPPER_DATADIR
    tempDir.cd(OPENFLIPPER_DATADIR);
  #endif
  tempDir.cd("Translations");
  OpenFlipper::Options::translationsDir(tempDir.absolutePath());  

  // Set the Path to the Fonts
  tempDir = QDir(OpenFlipper::Options::applicationDir());
  #ifdef OPENFLIPPER_DATADIR
    tempDir.cd(OPENFLIPPER_DATADIR);
  #endif
  tempDir.cd("Fonts");
  OpenFlipper::Options::fontsDir(tempDir.absolutePath());

  // Set the Path to the License files
  tempDir = QDir(OpenFlipper::Options::applicationDir());
  #ifdef OPENFLIPPER_DATADIR
    tempDir.cd(OPENFLIPPER_DATADIR);
  #endif
  tempDir.cd("Licenses");
  OpenFlipper::Options::licenseDir(tempDir.absolutePath());

  // Set the Path to the Help
  tempDir = QDir(OpenFlipper::Options::applicationDir());
  #ifdef OPENFLIPPER_DATADIR
    tempDir.cd(OPENFLIPPER_DATADIR);
  #endif
  tempDir.cd("Help");
  OpenFlipper::Options::helpDir(tempDir.absolutePath());

  // Set the Path to the main data directory
  tempDir = QDir(OpenFlipper::Options::applicationDir());
  #ifdef OPENFLIPPER_DATADIR
    tempDir.cd(OPENFLIPPER_DATADIR);
  #endif
  OpenFlipper::Options::dataDir(tempDir.absolutePath());

  QStringList optionFiles;

  // ==============================================================
  // Global ini file in the application directory
  // ==============================================================
  QFile globalIni(OpenFlipper::Options::applicationDir().absolutePath() + OpenFlipper::Options::dirSeparator() + "OpenFlipper.ini");
  if ( globalIni.exists() )
    optionFiles.push_back(OpenFlipper::Options::applicationDir().absolutePath() + OpenFlipper::Options::dirSeparator() + "OpenFlipper.ini");

  // ==============================================================
  // Local ini file in the users home directory
  // ==============================================================
  QFile localIni(OpenFlipper::Options::configDirStr() + OpenFlipper::Options::dirSeparator() + "OpenFlipper.ini");
  if ( localIni.exists() )
    optionFiles.push_back(OpenFlipper::Options::configDirStr() + OpenFlipper::Options::dirSeparator() + "OpenFlipper.ini");

  // Set the previously generated Optionfiles
  OpenFlipper::Options::optionFiles(optionFiles);

  // ==============================================================
  // Default File open directories
  // ==============================================================
  // Default to OpenFlippers Texture dir
  OpenFlipper::Options::currentTextureDir(OpenFlipper::Options::textureDirStr());

  // Default to home directory
  OpenFlipper::Options::currentDir( QDir::home() );


  // ==============================================================
  // Load Application options from all files available
  // ==============================================================

  for ( int i = 0 ; i < (int)optionFiles.size(); ++i) {

    INIFile _ini;

    if ( ! _ini.connect(optionFiles[i],false) ) {
      emit log(LOGERR,"Failed to connect to _ini file" + optionFiles[i]);
      continue;
    }

    readApplicationOptions(_ini);

    _ini.disconnect();
  }

}

/// restore key Assignments that were saved in config files
void Core::restoreKeyBindings(){

  QStringList optionFiles;

  // ==============================================================
  // Global ini file in the application directory
  // ==============================================================
  QFile globalIni(OpenFlipper::Options::applicationDir().absolutePath() + OpenFlipper::Options::dirSeparator() + "OpenFlipper.ini");
  if ( globalIni.exists() )
    optionFiles.push_back(OpenFlipper::Options::applicationDir().absolutePath() + OpenFlipper::Options::dirSeparator() + "OpenFlipper.ini");

  // ==============================================================
  // Local ini file in the users home directory
  // ==============================================================
  QFile localIni(OpenFlipper::Options::configDirStr() + OpenFlipper::Options::dirSeparator() + "OpenFlipper.ini");
  if ( localIni.exists() )
    optionFiles.push_back(OpenFlipper::Options::configDirStr() + OpenFlipper::Options::dirSeparator() + "OpenFlipper.ini");

  // Set the previously generated Optionfiles
  OpenFlipper::Options::optionFiles(optionFiles);


  // ==============================================================
  // Load Application options from all files available
  // ==============================================================

  for ( int i = 0 ; i < (int)optionFiles.size(); ++i) {

    INIFile _ini;

    if ( ! _ini.connect(optionFiles[i],false) ) {
      emit log(LOGERR,"Failed to connect to _ini file" + optionFiles[i]);
      continue;
    }

    if ( OpenFlipper::Options::gui() )
      coreWidget_->loadKeyBindings(_ini);

    _ini.disconnect();
  }
}

//=============================================================================
