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
//  CLASS Core - IMPLEMENTATION of Path and File Handling
//
//=============================================================================


//== INCLUDES =================================================================

// -------------------- mview
#include "Core.hh"
// -------------------- ACG
#include <ACG/Scenegraph/DrawModes.hh>

#include <OpenFlipper/INIFile/INIFile.hh>
#include <OpenFlipper/common/GlobalOptions.hh>
#include <OpenFlipper/BasePlugin/PluginFunctions.hh>

#include <QCoreApplication>


//== IMPLEMENTATION ==========================================================

void Core::applyOptions(){

  if ( OpenFlipper::Options::gui() ) {

    //Init ViewModes
    coreWidget_->initViewModes();

    //Set default Viewmode (note: This always resets all toolboxes and should be skipped
    // here. Setting the initial view mode is instead done when initializing the core
    // and reading the ini-files.)
    //if (OpenFlipper::Options::currentViewMode() != "")
    //  coreWidget_->slotChangeView(OpenFlipper::Options::currentViewMode(), QStringList(), QStringList(), QStringList());
      
    //Set Fullscreen
    if ( OpenFlipperSettings().value("Core/Gui/fullscreen",false).toBool() )
      coreWidget_->setWindowState( coreWidget_->windowState() | Qt::WindowFullScreen);
    else
      coreWidget_->setWindowState( (coreWidget_->windowState() | Qt::WindowFullScreen) ^ Qt::WindowFullScreen);

    // Logger
    coreWidget_->showLogger( OpenFlipper::Options::loggerState() );

    // Toolbar Size
    switch ( OpenFlipperSettings().value("Core/Toolbar/iconSize",0).toInt() ){
      case 1  : coreWidget_->setIconSize( QSize(16,16) );break;
      case 2  : coreWidget_->setIconSize( QSize(32,32) );break;
      default : coreWidget_->setIconSize( coreWidget_->defaultIconSize() );
    }

    // gl mouse cursor
    coreWidget_->setForceNativeCursor( OpenFlipperSettings().value("Core/Gui/glViewer/nativeMouse",false).toBool() );
    
    //set viewer properties
    for (int i=0; i < PluginFunctions::viewers(); i++){

      // PluginFunctions::setDrawMode( OpenFlipper::Options::defaultDrawMode(i), i );
      PluginFunctions::setFixedView(OpenFlipper::Options::defaultViewingDirection(i), i );
      PluginFunctions::allowRotation(!OpenFlipper::Options::defaultLockRotation(i),i);

      //only switch projection here if an object is opened
      //this prevents problems when applying options on app start
      if ( PluginFunctions::objectCount() > 0 ){
        if ( OpenFlipper::Options::defaultPerspectiveProjectionMode(i)  )
          PluginFunctions::perspectiveProjection(i);
        else
          PluginFunctions::orthographicProjection(i);
      }
    }

    if ( OpenFlipperSettings().value("Core/Gui/glViewer/useMultipleViewers",true).toBool() )
      coreWidget_->setViewerLayout( OpenFlipper::Options::defaultViewerLayout() );


    // toolbox orientation
    coreWidget_->setToolBoxOrientationOnTheRight(OpenFlipperSettings().value("Core/Gui/ToolBoxes/ToolBoxOnTheRight",true).toBool());

    //set defaultBackgroundColor
    QColor c =  OpenFlipperSettings().value("Core/Gui/glViewer/defaultBackgroundColor").value<QColor>();

    for ( unsigned int i = 0 ; i < OpenFlipper::Options::examinerWidgets() ; ++i ) {
      //wheel zoom factor
      PluginFunctions::viewerProperties(i).wheelZoomFactor( OpenFlipperSettings().value("Core/Mouse/Wheel/ZoomFactor",1).toDouble() );
      PluginFunctions::viewerProperties(i).wheelZoomFactorShift( OpenFlipperSettings().value("Core/Mouse/Wheel/ZoomFactorShift",0.2).toDouble() );
      PluginFunctions::viewerProperties(i).wheelInvert( OpenFlipperSettings().value("Core/Mouse/Wheel/Invert",false).toBool() );
    }

    //hideToolbox
    if ( OpenFlipperSettings().value("Core/Gui/ToolBoxes/hidden",false).toBool() ) 
      coreWidget_->showToolbox (false);

    //setup logFile
    if (logFile_ != 0){

      if (  OpenFlipperSettings().value("Core/Log/logFile").toString() != logFile_->fileName() ){
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
  QString inifile = OpenFlipper::Options::configDirStr() +  "OpenFlipper.ini";

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
}

void Core::setupOptions() {

  // initialize the Settings:
  OpenFlipper::Options::initializeSettings();

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

  // Set the previously generated option files
  OpenFlipper::Options::optionFiles(optionFiles);

  // ==============================================================
  // Default File open directories
  // ==============================================================
  // Default to OpenFlippers Texture dir
  OpenFlipper::Options::currentTextureDir(OpenFlipper::Options::textureDirStr());

  // Default to home directory
  OpenFlipperSettings().setValue("Core/CurrentDir", QDir::homePath() );

  // ==============================================================
  // Load Application options from all files available
  // ==============================================================

  for ( int i = 0 ; i < (int)optionFiles.size(); ++i) {

    INIFile _ini;

    if ( ! _ini.connect(optionFiles[i],false) ) {
      emit log(LOGERR,tr("Failed to connect to _ini file") + optionFiles[i]);
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
      emit log(LOGERR,tr("Failed to connect to _ini file") + optionFiles[i]);
      continue;
    }

    if ( OpenFlipper::Options::gui() )
      coreWidget_->loadKeyBindings(_ini);

    _ini.disconnect();
  }
}

//=============================================================================
