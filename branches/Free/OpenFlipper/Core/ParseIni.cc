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

#include <OpenFlipper/INIFile/INIFile.hh>
#include <OpenFlipper/common/GlobalOptions.hh>
#include <OpenFlipper/common/RecentFiles.hh>

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>

#include <QFile>

#include <OpenFlipper/ACGHelper/DrawModeConverter.hh>

//== IMPLEMENTATION ==========================================================


/** Load the standard options from the given Ini File
* @param _ini Inifile to use
*/
void Core::readApplicationOptions(INIFile& _ini) {
  // Parse standard options
  if ( _ini.section_exists("Options") ) {

    // Load maxRecent Setting
    int mrecent = 6;
    if (_ini.get_entry(mrecent,"Options","MaxRecent"))
      OpenFlipper::Options::maxRecent(mrecent);

    for ( int j = mrecent-1 ; j >= 0; --j) {
      QString file;
      QString key = "recent" + QString::number(j);

      if ( !_ini.get_entry( file , "Options" , key ) )
        continue;

      key = "type" + QString::number(j);
      QString type;

      if ( !_ini.get_entry( type , "Options" , key ) )
        continue;

      OpenFlipper::Options::addRecentFile(file, typeId(type) );
    }

    // load ViewModes
    int viewModeCount;
    if (_ini.get_entry(viewModeCount,"Options","ViewModeCount") )
      for (int i=0; i < viewModeCount; i++){
        QString entry;
        QString key = "ViewMode" + QString::number(i);

        if ( !_ini.get_entry( entry , "Options" , key ) ) continue;

        QStringList widgets = entry.split(" ");
        QString mode = widgets.first();
        widgets.removeFirst();

        bool found = false;
        for (int i=0; i < viewModes_.size(); i++)
          if (viewModes_[i]->name == mode)
            found = true;
        if (!found){
          ViewMode* vm = new ViewMode();
          vm->name = mode;
          vm->custom = true;
          vm->visibleWidgets = widgets;
          viewModes_.push_back(vm);
        }

      }

    //load default dataType
    QString type;
    if (_ini.get_entry(type, "Options" , "default_DataType" ))
      OpenFlipper::Options::lastDataType(type);

    //============================================================================
    // Splash Screen
    //============================================================================
    bool splash;
    if( _ini.get_entry(splash, "Options", "Splash") )
      OpenFlipper::Options::splash(splash);

    //============================================================================
    // Load the startup dir for file dialogs
    //============================================================================
    QString startup_dir;
    if( _ini.get_entry(startup_dir, "Options", "StartupDir") )
      OpenFlipper::Options::currentDir(startup_dir);

    //============================================================================
    // Load the default script directory
    //============================================================================
    QString script_dir;
    if( _ini.get_entry(script_dir, "Options", "ScriptDir") )
      OpenFlipper::Options::currentScriptDir(script_dir);

    //============================================================================
    // Load the random base color setting
    //============================================================================
    bool random_base_color = false;
    if ( _ini.get_entry( random_base_color, "Options" , "RandomBaseColor") )
      OpenFlipper::Options::randomBaseColor( random_base_color );

    //============================================================================
    // Load the backface culling setting
    //============================================================================
    bool backface_culling = false;
    if ( _ini.get_entry( backface_culling, "Options" , "BackfaceCulling") )
      OpenFlipper::Options::backfaceCulling(backface_culling);

    //============================================================================
    // Load the WheelZoomFactor setting
    //============================================================================
    double wheelZoomFactor = 1.0;
    if ( _ini.get_entry( wheelZoomFactor, "Options" , "WheelZoomFactor") )
      OpenFlipper::Options::wheelZoomFactor(wheelZoomFactor);

    //============================================================================
    // Load the WheelZoomFactorShift setting
    //============================================================================
    double wheelZoomFactorShift = 0.2;
    if ( _ini.get_entry( wheelZoomFactorShift, "Options" , "WheelZoomFactorShift") )
      OpenFlipper::Options::wheelZoomFactorShift(wheelZoomFactorShift);

    //============================================================================
    // Load the animation setting
    //============================================================================
    bool animation = false;
    if ( _ini.get_entry( animation, "Options" , "Animation") )
      OpenFlipper::Options::animation(animation);

    //============================================================================
    // Load the twoSidedLighting setting
    //============================================================================
    bool twoSidedLighting = false;
    if ( _ini.get_entry( twoSidedLighting, "Options" , "TwoSidedLighting") )
      OpenFlipper::Options::twoSidedLighting(twoSidedLighting);

    //============================================================================
    // Load the synchronization setting
    //============================================================================
    bool synchronization = false;
    if ( _ini.get_entry( synchronization, "Options" , "Synchronization") )
      OpenFlipper::Options::synchronization(synchronization);

    //============================================================================
    // Load the stereo mode setting
    //============================================================================
    bool stereo = false;
    if ( _ini.get_entry( stereo, "Options" , "Stereo") )
      OpenFlipper::Options::stereo(stereo);

    //============================================================================
    // Load the setting for the loger window
    //============================================================================
    bool hideLogger = false;
    if ( _ini.get_entry( hideLogger, "Options" , "HideLogger") )
      OpenFlipper::Options::hideLogger(hideLogger);

    //============================================================================
    // Load the setting for the toolbox window
    //============================================================================
    bool hideToolbox = false;
    if ( _ini.get_entry( hideToolbox, "Options" , "HideToolbox") )
      OpenFlipper::Options::hideToolbox(hideToolbox);

    //============================================================================
    // Load the setting for the fullscreen option
    //============================================================================
    bool fullScreen = false;
    if ( _ini.get_entry( fullScreen, "Options" , "FullScreen") )
      OpenFlipper::Options::fullScreen(fullScreen);

    //============================================================================
    // Load the setting for the background color option
    //============================================================================
    uint backgroundColor = 0;
    if ( _ini.get_entry( backgroundColor, "Options" , "DefaultBackgroundColor") )
      OpenFlipper::Options::defaultBackgroundColor(QRgb(backgroundColor));

    //============================================================================
    // Load the setting for the object color option
    //============================================================================
    uint baseColor = 0;
    if ( _ini.get_entry( baseColor, "Options" , "DefaultBaseColor") )
      OpenFlipper::Options::defaultBaseColor(QRgb(baseColor));

    //============================================================================
    // Load the setting for the default Toolbox mode
    //============================================================================
    QString toolboxMode = false;
    if ( _ini.get_entry( toolboxMode, "Options" , "DefaultToolboxMode") )
      OpenFlipper::Options::defaultToolboxMode(toolboxMode);

    //============================================================================
    // Load the drawmodes
    //============================================================================
    std::vector< QString > draw_modes;
    if( _ini.get_entry(draw_modes, "Options", "StandardDrawModes") )
      OpenFlipper::Options::standardDrawMode( ListToDrawMode(draw_modes) );

    //============================================================================
    // Load logFile status
    //============================================================================
    bool logFileEnabled = true;
    if( _ini.get_entry(logFileEnabled, "Options", "LogFileEnabled") )
      OpenFlipper::Options::logFileEnabled(logFileEnabled);

    //============================================================================
    // Load logFile location
    //============================================================================
    QString logFile = "";
    if( _ini.get_entry(logFile, "Options", "LogFileLocation") )
      OpenFlipper::Options::logFile(logFile);

    //============================================================================
    // Load restrictFrameRate
    //============================================================================
    bool restrictFrameRate = false;
    if( _ini.get_entry(restrictFrameRate, "Options", "RestrictFrameRate") )
      OpenFlipper::Options::restrictFrameRate(restrictFrameRate);

    //============================================================================
    // Load maxFrameRate
    //============================================================================
    int maxFrameRate = 35;
    if( _ini.get_entry(maxFrameRate, "Options", "MaxFrameRate") )
      OpenFlipper::Options::maxFrameRate(maxFrameRate);

    //============================================================================
    // Load slotDebugging state
    //============================================================================
    bool doSlotDebugging = false;
    if( _ini.get_entry(doSlotDebugging, "Options", "SlotDebugging") )
      OpenFlipper::Options::doSlotDebugging(doSlotDebugging);

    //============================================================================
    // Load Picking debugger state
    //============================================================================
    bool renderPicking = false;
    if( _ini.get_entry(renderPicking, "Options", "RenderPicking") )
      OpenFlipper::Options::renderPicking(renderPicking);

    QString pickingRenderMode = "";
    if( _ini.get_entry(renderPicking, "Options", "PickingRenderMode") )
      OpenFlipper::Options::pickingRenderMode(pickingRenderMode);

    //============================================================================
    // Update information
    //============================================================================
    QString updateUrl = "";
    if( _ini.get_entry(updateUrl, "Options", "UpdateURL") )
      OpenFlipper::Options::updateUrl(updateUrl);

    QString updateUsername = "";
    if( _ini.get_entry(updateUsername, "Options", "UpdateUsername") )
      OpenFlipper::Options::updateUsername(updateUsername);

    QString updatePassword = "";
    if( _ini.get_entry(updatePassword, "Options", "UpdatePassword") )
      OpenFlipper::Options::updatePassword(updatePassword);


  }
}

/** Write the standard options to the given Ini File
* @param _ini Inifile to use
*/
void Core::writeApplicationOptions(INIFile& _ini) {

  // Write maximum recent file count to ini
  _ini.add_entry("Options","MaxRecent",OpenFlipper::Options::maxRecent());

  // Write list of recent files to ini
  QVector< OpenFlipper::Options::RecentFile > recentFiles = OpenFlipper::Options::recentFiles();
  for ( int j = 0 ; j < recentFiles.size(); ++j) {
    // Save filename
    QString key = "recent" + QString::number(j);
    QString filename = recentFiles[j].filename;
    _ini.add_entry( "Options" , key , filename );
    // Save DataType
    key = "type" + QString::number(j);
    QString type = typeName( recentFiles[j].type );
    _ini.add_entry( "Options" , key , type );
  }

  // save ViewModes
  QVector< QString > entries;
  if ( OpenFlipper::Options::gui() )
    for (int i=0; i < coreWidget_->viewModes_.size(); i++)
      if (coreWidget_->viewModes_[i]->custom){

        //store name
        QString entry = coreWidget_->viewModes_[i]->name;

        //store widgets
        for (int j=0; j < coreWidget_->viewModes_[i]->visibleWidgets.size(); j++)
          entry += " " + coreWidget_->viewModes_[i]->visibleWidgets[j];

        entries.push_back(entry);
      }

  //save viewmodes to ini
  _ini.add_entry("Options","ViewModeCount" ,entries.size());
  for (int i=0; i < entries.size(); i++)
    _ini.add_entry("Options","ViewMode" + QString::number(i) ,entries[i]);

  //save KeyBindings
  if ( OpenFlipper::Options::gui() )
    coreWidget_->saveKeyBindings(_ini);

  //============================================================================
  // Splash Screen
  //============================================================================
  _ini.add_entry( "Options" , "Splash" , OpenFlipper::Options::splash() );

  //write default dataType to INI
  _ini.add_entry( "Options" , "default_DataType" , OpenFlipper::Options::lastDataType() );

  //write default ToolboxMode
  _ini.add_entry("Options","DefaultToolboxMode",OpenFlipper::Options::defaultToolboxMode() );

  //============================================================================
  // logfile
  //============================================================================
  //write logfile status
  _ini.add_entry("Options","LogFileEnabled",OpenFlipper::Options::logFileEnabled() );
  //write logfile location
  _ini.add_entry("Options","LogFileLocation",OpenFlipper::Options::logFile() );

  //============================================================================
  // other
  //============================================================================
  // restrict Framerate
  _ini.add_entry("Options","RestrictFrameRate",OpenFlipper::Options::restrictFrameRate() );
  // max Framerate
  _ini.add_entry("Options","MaxFrameRate",OpenFlipper::Options::maxFrameRate() );

  //============================================================================
  // Debugging
  //============================================================================
  _ini.add_entry("Options","SlotDebugging",OpenFlipper::Options::doSlotDebugging() );

  //============================================================================
  // Write Picking debugger state
  //============================================================================
  _ini.add_entry("Options","RenderPicking",OpenFlipper::Options::renderPicking() );
  _ini.add_entry("Options","PickingRenderMode",OpenFlipper::Options::pickingRenderMode() );

  //============================================================================
  // Update information
  //============================================================================
  _ini.add_entry("Options","UpdateURL",OpenFlipper::Options::updateUrl() );
  _ini.add_entry("Options","UpdateUsername",OpenFlipper::Options::updateUsername() );
  _ini.add_entry("Options","UpdatePassword",OpenFlipper::Options::updatePassword() );

  //============================================================================
  //dontLoad Plugins
  //============================================================================
  QString dontLoad;
  for (int i=0; i < dontLoadPlugins_.size(); i++)
    if (!dontLoad.contains(dontLoadPlugins_[i].trimmed() + ";"))
      dontLoad += dontLoadPlugins_[i].trimmed() + ";";

  if ( !_ini.section_exists("Plugins") )
    _ini.add_section("Plugins");

  //write dontLoad Plugins
  _ini.add_entry("Plugins","DontLoad", dontLoad);



  if ( !_ini.section_exists("Options") )
    _ini.add_section("Options");

  QString dir = OpenFlipper::Options::currentDirStr().toUtf8();
  _ini.add_entry("Options","StartupDir",dir);

  QString scriptDir = OpenFlipper::Options::currentScriptDirStr().toUtf8();
  _ini.add_entry("Options","ScriptDir",scriptDir);

  _ini.add_entry("Options","RandomBaseColor", OpenFlipper::Options::randomBaseColor() );

  if ( OpenFlipper::Options::gui() ) {

    if ( OpenFlipper::Options::multiView() ) {
      std::cerr << "Todo : Save application options for multiple views" << std::endl;
    }

    _ini.add_entry("Options","BackfaceCulling",PluginFunctions::viewerProperties().backFaceCulling());
    _ini.add_entry("Options","Animation",PluginFunctions::viewerProperties().animation());
    _ini.add_entry("Options","twoSidedLighting",PluginFunctions::viewerProperties().twoSidedLighting());

    _ini.add_entry("Options","WheelZoomFactor",PluginFunctions::viewerProperties().wheelZoomFactor());
    _ini.add_entry("Options","WheelZoomFactorShift",PluginFunctions::viewerProperties().wheelZoomFactorShift());

    //============================================================================
    // Save the current draw modes
    //============================================================================
    std::vector< QString > draw_modes;
    draw_modes = drawModeToList( OpenFlipper::Options::standardDrawMode() );
    _ini.add_entry("Options","StandardDrawModes",draw_modes);

    _ini.add_entry("Options","HideLogger", OpenFlipper::Options::hideLogger() );
    _ini.add_entry("Options","HideToolbox", OpenFlipper::Options::hideToolbox() );

    // check if we are in fullscreen mode:
    _ini.add_entry("Options","FullScreen", OpenFlipper::Options::fullScreen() );

    _ini.add_entry("Options","DefaultBackgroundColor", (uint)OpenFlipper::Options::defaultBackgroundColor().rgba ()  );
    _ini.add_entry("Options","DefaultBaseColor", (uint)OpenFlipper::Options::defaultBaseColor().rgba ()  );
  }

  _ini.add_entry("Options","Stereo",OpenFlipper::Options::stereo() );
}

void Core::openIniFile( QString _filename,
                        bool    _coreSettings,
                        bool    _perPluginSettings,
                        bool    _loadObjects ){
  INIFile ini;

  if ( ! ini.connect(_filename,false) ) {
    emit log(LOGERR,"Failed to connect to ini file" + _filename);
    return;
  }

  if ( OpenFlipper::Options::gui() ) {
    coreWidget_->statusMessage( "Loading ini File " + _filename + " ...");
    coreWidget_->setStatus(ApplicationStatus::BLOCKED );
  }

  OpenFlipper::Options::loadingSettings(true);

  // Load Core settings only if requested
  if ( _coreSettings )
    readApplicationOptions(ini);

  // if requested load per Plugin settings from the settings file
  if ( _perPluginSettings )
    emit iniLoadOptions( ini );

  if ( _loadObjects ) {

    QStringList openFiles;

    // Parse File section for files to open
    if ( ini.section_exists("OpenFiles") && ini.get_entry(openFiles,"OpenFiles","open") ) {

      bool newActiveObject = false;

      for ( int i = 0 ; i < openFiles.size(); ++i ) {

        QString sectionName = openFiles[i];

        QString path;
        ini.get_entry( path, sectionName , "path" );

        //check if path is relative
        if (path.startsWith( "." + OpenFlipper::Options::dirSeparator() )){
          //check if _filename contains a path
          if (_filename.section(OpenFlipper::Options::dirSeparator(), 0, -2) != ""){
            path.remove(0,1); // remove .
            path = _filename.section(OpenFlipper::Options::dirSeparator(), 0, -2) + path;
          }
        }

        int tmpType;
        DataType type = DATA_TRIANGLE_MESH;

        if ( ini.get_entry( tmpType, sectionName , "type"  )) {
          type = DataType(tmpType);
          emit log(LOGWARN, "This ini file uses old int style ObjectType fields!" );
          emit log(LOGWARN, "Please convert it to new format! ( ... just save it )" );
        } else {

          QString typeName="";

          if ( ini.get_entry( typeName, sectionName , "type"  )) {
            type = typeId(typeName);
          } else
            emit log(LOGWARN, "Unable to get DataType for object " +  sectionName + " assuming Triangle Mesh" );
        }

        int newObjectId = loadObject(type, path);

        BaseObject* object = objectRoot_->childExists( newObjectId );
        if ( object == 0 )  {
          emit log(LOGERR,"Unable to open Object " + path);
          continue;
        }

        bool flag;
        if ( ini.get_entry( flag, sectionName , "target" ) ) {
          object->target(flag);
          newActiveObject = true;
        }

        if ( ini.get_entry( flag, sectionName , "source" ) )
          object->source(flag);

        emit iniLoad( ini,object->id() );
      }

      if ( newActiveObject )
        emit activeObjectChanged();

    }

  }

  // Tell Plugins that all objects are
  if ( _perPluginSettings )
    emit iniLoadOptionsLast( ini );

  ini.disconnect();

  OpenFlipper::Options::loadingSettings(false);

  resetScenegraph();

  if ( OpenFlipper::Options::gui() ) {
    coreWidget_->statusMessage( "Loading ini File " + _filename + " ... Done", 4000);
    coreWidget_->setStatus(ApplicationStatus::READY );
  }

}

void Core::writeIniFile(QString _filename,
                        bool _relativePaths,
                        bool _targetOnly,
                        bool _saveSystemSettings,
                        bool _savePluginSettings ,
                        bool _saveObjectInfo ) {

  INIFile ini;

  if ( ! ini.connect(_filename,true) ) {
    emit log(LOGERR,"Failed to connect to _ini file" + _filename);
      return;
  }

  if ( OpenFlipper::Options::gui() ) {
    coreWidget_->statusMessage( "Saving ini File " + _filename + " ...");
    coreWidget_->setStatus(ApplicationStatus::BLOCKED );
  }

  // Only save application settings when requested
  if ( _saveSystemSettings )
    writeApplicationOptions(ini);

  if ( _savePluginSettings )
    emit iniSaveOptions( ini );

  if ( _saveObjectInfo ) {
    // This vector will hold the file sections to open
    QStringList openFiles;


    PluginFunctions::IteratorRestriction restriction;
    if ( _targetOnly )
      restriction = PluginFunctions::TARGET_OBJECTS;
    else
      restriction = PluginFunctions::ALL_OBJECTS;

    QString keyName;
    QString sectionName;
    for ( PluginFunctions::ObjectIterator o_it(restriction) ;
                                          o_it != PluginFunctions::objects_end(); ++o_it) {
      QString file = o_it->path() + OpenFlipper::Options::dirSeparator() + o_it->name();
      if (QFile(file).exists()){
        // Add a section for this object
        sectionName = o_it->name();
        ini.add_section( sectionName );
        openFiles.push_back( sectionName );

        //modify filename if relativePaths are wanted
        if (_relativePaths){
          int prefixLen = _filename.section(OpenFlipper::Options::dirSeparator(),0,-2).length();
          file.remove(0, prefixLen);
          file = "." + file;
        }
        // Add the path of this object to the section
        ini.add_entry( sectionName , "path" , file );
        ini.add_entry( sectionName , "type" , typeName(o_it->dataType() ) );
        ini.add_entry( sectionName , "target" , o_it->target() );
        ini.add_entry( sectionName , "source" , o_it->source() );

      }
    }

    ini.add_entry("OpenFiles","open",openFiles);

    // Tell plugins to save their information for the given object
    for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS) ;
                                          o_it != PluginFunctions::objects_end(); ++o_it)
      emit iniSave(  ini , o_it->id() );
  }



  ini.disconnect();

  if ( OpenFlipper::Options::gui() ) {
    coreWidget_->statusMessage( "Saving ini File " + _filename + " ... Done", 4000);
    coreWidget_->setStatus(ApplicationStatus::READY );
  }
}


//=============================================================================
