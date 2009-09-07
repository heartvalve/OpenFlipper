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
 *   $Revision$                                                         *
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

    // load ViewModes
    int viewModeCount;
    if (_ini.get_entry(viewModeCount,"Options","ViewModeCount") )
      for (int i=0; i < viewModeCount; i++){
        QString entry;
        QString key = "ViewMode" + QString::number(i);

        if ( !_ini.get_entry( entry , "Options" , key ) ) continue;

        QStringList widgets = entry.split(";");
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
    if( _ini.get_entry(script_dir, "Options", "CurrentScriptDir") )
      OpenFlipper::Options::currentScriptDir(script_dir);

    //============================================================================
    // Load the default texture directory
    //============================================================================
    QString current_texture_dir;
    if( _ini.get_entry(current_texture_dir, "Options", "CurrentTextureDir") )
      OpenFlipper::Options::currentTextureDir(current_texture_dir);

    //============================================================================
    // Load the random base color setting
    //============================================================================
    bool random_base_color = false;
    if ( _ini.get_entry( random_base_color, "Options" , "RandomBaseColor") )
      OpenFlipper::Options::randomBaseColor( random_base_color );

    //============================================================================
    // Load the synchronization setting
    //============================================================================
    bool synchronization = false;
    if ( _ini.get_entry( synchronization, "Options" , "Synchronization") )
      OpenFlipper::Options::synchronization(synchronization);

    //============================================================================
    // Load the stereo mode setting
    //============================================================================
    //bool stereo = false;
    //if ( _ini.get_entry( stereo, "Options" , "Stereo") )
    //  OpenFlipper::Options::stereo(stereo);

    //============================================================================
    // Load the stereo mode setting
    //============================================================================
    int stereoMode = 0;
    if ( _ini.get_entry( stereoMode, "Options" , "StereoMode") )
      OpenFlipper::Options::stereoMode(static_cast<OpenFlipper::Options::StereoMode> (stereoMode));

    //============================================================================
    // Load stereo view settings
    //============================================================================

    float val;
    if ( _ini.get_entry( val, "Options" , "EyeDistance") )
      OpenFlipper::Options::eyeDistance(val);
    if ( _ini.get_entry( val, "Options" , "FocalDistance") )
      OpenFlipper::Options::focalDistance(val);

    //============================================================================
    // Load the custom anaglyph stereo mode color matrices
    //============================================================================
    std::vector<float> mat;
    if ( _ini.get_entry( mat, "Options" , "CustomAnaglyphLeftEye") && mat.size () == 9)
    {
      OpenFlipper::Options::anaglyphLeftEyeColorMatrix(mat);
    }
    else
    {
      std::vector<float> set (9,0.0);
      set[0] = 0.299;
      set[3] = 0.587;
      set[6] = 0.114;
      OpenFlipper::Options::anaglyphLeftEyeColorMatrix(set);
    }

    if ( _ini.get_entry( mat, "Options" , "CustomAnaglyphRightEye") && mat.size () == 9)
    {
      OpenFlipper::Options::anaglyphRightEyeColorMatrix(mat);
    }
    else
    {
      std::vector<float> set (9,0.0);
      set[4] = 1.0;
      set[8] = 1.0;
      OpenFlipper::Options::anaglyphRightEyeColorMatrix(set);
    }

    //============================================================================
    // Load stereo mouse picking behavior
    //============================================================================
    bool stereoMousePick = false;
    if ( _ini.get_entry( stereoMousePick, "Options" , "StereoMousePick") )
      OpenFlipper::Options::stereoMousePick(stereoMousePick);

    //============================================================================
    // Load gl mouse painting behavior
    //============================================================================
    bool glMouse = false;
    if ( _ini.get_entry( glMouse, "Options" , "GlMouse") )
      OpenFlipper::Options::glMouse(glMouse);

    //============================================================================
    // Load the setting for the loger window
    //============================================================================
    int loggerState = 0;
    if ( _ini.get_entry( loggerState, "Options" , "LoggerState") )
      OpenFlipper::Options::loggerState(static_cast<OpenFlipper::Options::LoggerState> (loggerState));

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
    // Load the setting for the translation language
    //============================================================================
    QString translation = false;
    if ( _ini.get_entry( translation, "Options" , "Translation") )
      OpenFlipper::Options::translation(translation);

    //============================================================================
    // Load the setting for the gridVisibility
    //============================================================================
    bool visible = false;
    if ( _ini.get_entry( visible, "Options" , "GridVisible") )
      OpenFlipper::Options::gridVisible(visible);

    //============================================================================
    // Load the setting for the viewer layout
    //============================================================================
    int viewerLayout = 0;
    if ( _ini.get_entry( viewerLayout, "Options" , "DefaultViewerLayout") )
      OpenFlipper::Options::defaultViewerLayout(viewerLayout);

    //============================================================================
    // Load the viewer settings
    //============================================================================
    std::vector< QString > draw_modes;
    int mode;

    for (int i=0; i < 4/*PluginFunctions::viewers()*/; i++ ){

      if( _ini.get_entry(draw_modes, "Options", "DefaultDrawModes" + QString::number(i) ) )
        OpenFlipper::Options::defaultDrawMode( ListToDrawMode(draw_modes), i );

      if( _ini.get_entry(mode, "Options", "DefaultProjectionMode" + QString::number(i) ) )
        OpenFlipper::Options::defaultProjectionMode( mode, i );

      if( _ini.get_entry(mode, "Options", "DefaultViewingDirection" + QString::number(i) ) )
        OpenFlipper::Options::defaultViewingDirection( mode, i );
    }

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
    // Load the WheelZoomFactor setting
    //============================================================================
    double wheelZoomFactor = 1.0;
    if ( _ini.get_entry( wheelZoomFactor, "Options" , "WheelZoomFactor") )
      OpenFlipper::Options::wheelZoomFactor(wheelZoomFactor);

    //============================================================================
    // Load the WheelZoomFactorShift setting
    //============================================================================
    double wheelZoomFactorShift = 0.2;
    if ( _ini.get_entry( wheelZoomFactorShift, "Options" , "WheelZoomFactorShift" ) )
      OpenFlipper::Options::wheelZoomFactorShift(wheelZoomFactorShift);

    //============================================================================
    // Load restrictFrameRate
    //============================================================================
    bool restrictFrameRate = false;
    if( _ini.get_entry(restrictFrameRate, "Options", "RestrictFrameRate") )
      OpenFlipper::Options::restrictFrameRate(restrictFrameRate);

    //============================================================================
    // Load showWheelsAtStartup
    //============================================================================
    bool showWheelsAtStartup = false;
    if( _ini.get_entry(showWheelsAtStartup, "Options", "ShowWheelsAtStartup") )
      OpenFlipper::Options::showWheelsAtStartup(showWheelsAtStartup);

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
    // ViewerProperties
    //============================================================================

    bool multiView = false;
    if( _ini.get_entry(multiView, "Options", "MultiView") )
      OpenFlipper::Options::multiView(multiView);

    uint viewerCount = 0;
    if( _ini.get_entry(viewerCount, "Options", "ViewerCount") ){
    }

    if ( _ini.section_exists("ViewerProperties") ){
      for ( unsigned int i = 0 ; i < viewerCount; ++i ) {

        if (OpenFlipper::Options::examinerWidgets() < i)
          break;

        // Load the animation setting
        bool animation = false;
        if ( _ini.get_entry( animation, "ViewerProperties" , "Animation" + QString::number(i) ) )
          PluginFunctions::viewerProperties(i).animation(animation);

        // Load the twoSidedLighting setting
        bool twoSidedLighting = false;
        if ( _ini.get_entry( twoSidedLighting, "ViewerProperties" , "TwoSidedLighting" + QString::number(i)) )
          PluginFunctions::viewerProperties(i).twoSidedLighting(twoSidedLighting);

        // Load the backface culling setting
        bool backface_culling = false;
        if ( _ini.get_entry( backface_culling, "ViewerProperties" , "BackfaceCulling" + QString::number(i)) )
          PluginFunctions::viewerProperties(i).backFaceCulling(backface_culling);

        // Load the setting for the background color option
        uint viewerBackground = 0;
        if ( _ini.get_entry( viewerBackground, "ViewerProperties" , "BackgroundColor" + QString::number(i)) )
          PluginFunctions::viewerProperties(i).backgroundColor( QRgb(viewerBackground) );
      }
    }

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

void Core::readRecentFiles(QString _filename){

  INIFile ini;

  if ( ! ini.connect(_filename,false) ) {
    emit log(LOGERR,"Failed to connect to ini file" + _filename);
    return;
  }

  OpenFlipper::Options::loadingSettings(true);

  // Load maxRecent Setting
  int mrecent = 6;
  if (ini.get_entry(mrecent,"Options","MaxRecent"))
    OpenFlipper::Options::maxRecent(mrecent);

  for ( int j = mrecent-1 ; j >= 0; --j) {
    QString file;
    QString key = "recent" + QString::number(j);

    if ( !ini.get_entry( file , "Options" , key ) )
      continue;

    key = "type" + QString::number(j);
    QString type;

    if ( !ini.get_entry( type , "Options" , key ) )
      continue;

    OpenFlipper::Options::addRecentFile(file, typeId(type) );
  }

  ini.disconnect();

  OpenFlipper::Options::loadingSettings(false);

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
          entry += ";" + coreWidget_->viewModes_[i]->visibleWidgets[j];

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
  _ini.add_entry("Options","WheelZoomFactor", OpenFlipper::Options::wheelZoomFactor() );
  _ini.add_entry("Options","WheelZoomFactorShift", OpenFlipper::Options::wheelZoomFactorShift() );
  // restrict Framerate
  _ini.add_entry("Options","RestrictFrameRate",OpenFlipper::Options::restrictFrameRate() );
  // Show wheels on startup
  _ini.add_entry("Options","ShowWheelsAtStartup",OpenFlipper::Options::showWheelsAtStartup() );
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
  _ini.add_entry("Options","CurrentScriptDir",scriptDir);

  QString current_texture_dir = OpenFlipper::Options::currentTextureDirStr().toUtf8();
  _ini.add_entry("Options","CurrentTextureDir",current_texture_dir);

  _ini.add_entry("Options","RandomBaseColor", OpenFlipper::Options::randomBaseColor() );

  if ( OpenFlipper::Options::gui() ) {

    _ini.add_entry("Options","DefaultViewerLayout", OpenFlipper::Options::defaultViewerLayout() );
    _ini.add_entry("Options","Translation", OpenFlipper::Options::translation() );
    _ini.add_entry("Options","GridVisible", OpenFlipper::Options::gridVisible() );

    _ini.add_entry("Options","MultiView", OpenFlipper::Options::multiView() );
    _ini.add_entry("Options","ViewerCount", OpenFlipper::Options::examinerWidgets() );

    if ( !_ini.section_exists("ViewerProperties") )
      _ini.add_section("ViewerProperties");

    for ( unsigned int i = 0 ; i < OpenFlipper::Options::examinerWidgets(); ++i ) {

      _ini.add_entry("ViewerProperties","Animation" + QString::number(i),       PluginFunctions::viewerProperties(i).animation());
      _ini.add_entry("ViewerProperties","BackfaceCulling" + QString::number(i), PluginFunctions::viewerProperties(i).backFaceCulling());
      _ini.add_entry("ViewerProperties","TwoSidedLighting" + QString::number(i),PluginFunctions::viewerProperties(i).twoSidedLighting());
      _ini.add_entry("ViewerProperties","BackgroundColor" + QString::number(i),
                     (uint)PluginFunctions::viewerProperties(i).backgroundColorRgb() );
    }
    //============================================================================
    // Save the current viewer properties
    //============================================================================
    std::vector< QString > draw_modes;
    int mode;

    for (int i=0; i < PluginFunctions::viewers(); i++ ){

      draw_modes = drawModeToList( OpenFlipper::Options::defaultDrawMode(i) );
      _ini.add_entry("Options","DefaultDrawModes" + QString::number(i), draw_modes);

      _ini.add_entry("Options","DefaultProjectionMode" + QString::number(i), OpenFlipper::Options::defaultProjectionMode(i) );
      _ini.add_entry("Options","DefaultViewingDirection" + QString::number(i), OpenFlipper::Options::defaultViewingDirection(i) );
    }
    

    _ini.add_entry("Options","LoggerState", OpenFlipper::Options::loggerState() );
    _ini.add_entry("Options","HideToolbox", OpenFlipper::Options::hideToolbox() );

    // check if we are in fullscreen mode:
    _ini.add_entry("Options","FullScreen", OpenFlipper::Options::fullScreen() );

    _ini.add_entry("Options","DefaultBackgroundColor", (uint)OpenFlipper::Options::defaultBackgroundColor().rgba ()  );
    _ini.add_entry("Options","DefaultBaseColor", (uint)OpenFlipper::Options::defaultBaseColor().rgba ()  );

    _ini.add_entry("Options","StereoMode",OpenFlipper::Options::stereoMode() );

    _ini.add_entry("Options" , "EyeDistance", OpenFlipper::Options::eyeDistance());
    _ini.add_entry("Options" , "FocalDistance", OpenFlipper::Options::focalDistance());

    _ini.add_entry("Options" , "CustomAnaglyphLeftEye", OpenFlipper::Options::anaglyphLeftEyeColorMatrix() );
    _ini.add_entry("Options" , "CustomAnaglyphRightEye", OpenFlipper::Options::anaglyphRightEyeColorMatrix() );

    _ini.add_entry("Options", "StereoMousePick", OpenFlipper::Options::stereoMousePick() );

    _ini.add_entry("Options", "GlMouse", OpenFlipper::Options::glMouse() );
  }

  emit saveOnExit(_ini);

  // _ini.add_entry("Options","Stereo",OpenFlipper::Options::stereo() );
}

void Core::openIniFile( QString _filename,
                        bool    _coreSettings,
                        bool    _perPluginSettings,
                        bool    _loadObjects ){
  INIFile ini;

  if ( ! ini.connect(_filename,false) ) {
    emit log(LOGERR,tr("Failed to connect to ini file") + _filename);
    return;
  }

  if ( OpenFlipper::Options::gui() ) {
    coreWidget_->statusMessage( tr("Loading ini File ") + _filename + " ...");
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
          emit log(LOGWARN, tr("This ini file uses old int style ObjectType fields!") );
          emit log(LOGWARN, tr("Please convert it to new format! ( ... just save it )") );
        } else {

          QString typeName="";

          if ( ini.get_entry( typeName, sectionName , "type"  )) {
            type = typeId(typeName);
          } else
            emit log(LOGWARN, tr("Unable to get DataType for object ") +  sectionName + tr(" assuming Triangle Mesh") );
        }

        int newObjectId = loadObject(type, path);

        BaseObject* object = objectRoot_->childExists( newObjectId );
        if ( object == 0 )  {
          emit log(LOGERR,tr("Unable to open Object ") + path);
          continue;
        }

        bool flag;
        if ( ini.get_entry( flag, sectionName , "target" ) )
          object->target(flag);

        if ( ini.get_entry( flag, sectionName , "source" ) )
          object->source(flag);

        emit iniLoad( ini,object->id() );

        emit objectSelectionChanged( object->id() );
      }
    }

  }

  // Tell Plugins that all objects are
  if ( _perPluginSettings )
    emit iniLoadOptionsLast( ini );

  ini.disconnect();

  OpenFlipper::Options::loadingSettings(false);

  resetScenegraph();

  if ( OpenFlipper::Options::gui() ){
    for ( unsigned int i = 0 ; i < OpenFlipper::Options::examinerWidgets() ; ++i ) {
      PluginFunctions::viewerProperties(i).drawMode( OpenFlipper::Options::defaultDrawMode(i) );
      coreWidget_->examiner_widgets_[i]->viewAll();
    }

    coreWidget_->statusMessage( tr("Loading ini File ") + _filename + tr(" ... Done"), 4000);
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
    emit log(LOGERR,tr("Failed to connect to _ini file") + _filename);
      return;
  }

  if ( OpenFlipper::Options::gui() ) {
    coreWidget_->statusMessage( tr("Saving ini File ") + _filename + " ...");
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
                                          o_it != PluginFunctions::objectsEnd(); ++o_it) {
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
                                          o_it != PluginFunctions::objectsEnd(); ++o_it)
      emit iniSave(  ini , o_it->id() );
  }



  ini.disconnect();

  if ( OpenFlipper::Options::gui() ) {
    coreWidget_->statusMessage( tr("Saving ini File ") + _filename + tr(" ... Done"), 4000);
    coreWidget_->setStatus(ApplicationStatus::READY );
  }
}


//=============================================================================
