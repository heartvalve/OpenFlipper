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
    coreWidget_->hideLogger( OpenFlipper::Options::hideLogger() );
    
    //animation
    coreWidget_->examiner_widget_->animation(OpenFlipper::Options::animation());
  
    //Backface Culling
    coreWidget_->examiner_widget_->backFaceCulling(OpenFlipper::Options::backfaceCulling());

    //wheel zoom factor
    coreWidget_->examiner_widget_->setWheelZoomFactor(OpenFlipper::Options::wheelZoomFactor());
    coreWidget_->examiner_widget_->setWheelZoomFactorShift(OpenFlipper::Options::wheelZoomFactorShift());
    
    //hideToolbox
    if (OpenFlipper::Options::hideToolbox()) {
      //hide all toolWidgets
      for (uint p=0; p < plugins.size(); p++)
        if (plugins[p].widget)
          plugins[p].widget->setVisible(false);
  
      //hide ViewMode Selection Widget
      coreWidget_->dockViewMode_->setVisible(false);  
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
  tempDir.cd(".." + OpenFlipper::Options::dirSeparator() );
  OpenFlipper::Options::applicationDir(tempDir.absolutePath());
  
  // Set the standard path to the plugins
  tempDir.cd("Plugins");

  ///@todo do not check WIN32 Only 
  if ( OpenFlipper::Options::is64bit() )
    tempDir.cd("64");
    
  if ( OpenFlipper::Options::is32bit() )
      #ifdef WIN32
         tempDir.cd("win32");
      #else
	      tempDir.cd("32");
      #endif
  

   #ifdef WIN32
      #ifdef NDEBUG
              tempDir.cd("Release");
      #else
              tempDir.cd("Debug");
      #endif
   #else
      #ifdef DEBUG
              tempDir.cd("dbg");
      #else
              tempDir.cd("max");
      #endif		
   #endif    
    
  OpenFlipper::Options::pluginDir(tempDir.absolutePath());
  
  // Set the Path to the shaders
  tempDir = QDir(OpenFlipper::Options::applicationDir());
  tempDir.cd("Shaders");
  OpenFlipper::Options::shaderDir(tempDir.absolutePath());
  
  
  // Set the Path to the textures
  tempDir = QDir(OpenFlipper::Options::applicationDir());
  tempDir.cd("Textures");
  OpenFlipper::Options::textureDir(tempDir.absolutePath());
  
  // Set the Path to the Icons
  tempDir = QDir(OpenFlipper::Options::applicationDir());
  tempDir.cd("Icons");
  OpenFlipper::Options::iconDir(tempDir.absolutePath());
  
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
    
    readApplicationOptions(_ini);
    
    _ini.disconnect();
  }
  
}

//=============================================================================
