/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2014 by Computer Graphics Group, RWTH Aachen       *
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

#include <OpenFlipper/INIFile/INIFile.hh>
#include <OpenFlipper/common/GlobalOptions.hh>
#include <OpenFlipper/common/RecentFiles.hh>

#include <ObjectTypes/Light/Light.hh>

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

        QString entryToolbars;
        QString entryToolboxes;
        QString entryContextMenus;
        QString entryIcon;

        QString keyToolbars     = "ViewModeToolbars"     + QString::number(i);
        QString keyToolboxes    = "ViewModeToolboxes"    + QString::number(i);
        QString keyContextMenus = "ViewModeContextMenus" + QString::number(i);
        QString keyIcon         = "ViewModeIcon"         + QString::number(i);

        // Read the entries
        if ( !_ini.get_entry( entryToolbars     , "Options" , keyToolbars ) )     continue;
        if ( !_ini.get_entry( entryToolboxes    , "Options" , keyToolboxes ) )    continue;
        if ( !_ini.get_entry( entryContextMenus , "Options" , keyContextMenus ) ) continue;
        if ( !_ini.get_entry( entryIcon         , "Options" , keyIcon ) )         continue;

        QStringList toolBars     = entryToolbars.split(";");
        QStringList toolBoxes    = entryToolboxes.split(";");
        QStringList contextMenus = entryContextMenus.split(";");

        // Get Mode name ( prepended to all toolbox/toolbar/context menu lists )
        QString mode = toolBoxes.first();

        // Remove leading Modes
        toolBoxes.removeFirst();
        toolBars.removeFirst();
        contextMenus.removeFirst();

        // Check if the mode already exists
        bool found = false;
        for (int i=0; i < viewModes_.size(); i++)
          if (viewModes_[i]->name == mode)
            found = true;

        if (!found){
          ViewMode* vm = new ViewMode();
          vm->name = mode;
          vm->custom = true;
          vm->visibleToolbars     = toolBars;
          vm->visibleToolboxes    = toolBoxes;
          vm->visibleContextMenus = contextMenus;
          vm->icon                = entryIcon;
          viewModes_.push_back(vm);
        }

      }

    //load default dataType
    QString type;
    if (_ini.get_entry(type, "Options" , "default_DataType" ))
      OpenFlipper::Options::lastDataType(type);

    //============================================================================
    // Load the startup dir for file dialogs
    //============================================================================
    QString startup_dir;
    if( _ini.get_entry(startup_dir, "Options", "StartupDir") )
      OpenFlipperSettings().setValue("Core/CurrentDir", startup_dir );

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
    bool random_default_color = false;
    if ( _ini.get_entry( random_default_color, "Options" , "RandomDefaultColor") )
      OpenFlipper::Options::randomDefaultColor( random_default_color );

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
      set[0] = 0.299f;
      set[3] = 0.587f;
      set[6] = 0.114f;
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
    // Load the setting for the object color option
    //============================================================================
    unsigned int defaultColor = 0;
    if ( _ini.get_entry( defaultColor, "Options" , "DefaultColor") )
      OpenFlipper::Options::defaultColor(QRgb(defaultColor));

    //============================================================================
    // Load the setting for the default Toolbox mode
    //============================================================================
    QString viewmode = "";
    if ( _ini.get_entry( viewmode, "Options" , "CurrentViewMode") )
      OpenFlipper::Options::currentViewMode(viewmode);

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

    for (int i=0; i < 4/*PluginFunctions::viewers()*/; i++ ){

      if( _ini.get_entry(draw_modes, "Options", "DefaultDrawModes" + QString::number(i) ) )
        OpenFlipper::Options::defaultDrawMode( listToDrawMode(draw_modes), i );

    }

    //============================================================================
    // Load slotDebugging state
    //============================================================================
    bool doSlotDebugging = false;
    if( _ini.get_entry(doSlotDebugging, "Options", "SlotDebugging") )
      OpenFlipper::Options::doSlotDebugging(doSlotDebugging);

    //============================================================================
    // ViewerProperties
    //============================================================================

    unsigned int viewerCount = 0;
    if( _ini.get_entry(viewerCount, "Options", "ViewerCount") ){
    }

    for ( unsigned int i = 0 ; i < viewerCount; ++i ) {

      if (OpenFlipper::Options::examinerWidgets() < i)
        break;

      QString entryHeader = "Viewer" + QString::number(i) + "/";

      // Load the animation setting
      PluginFunctions::viewerProperties(i).animation(OpenFlipperSettings().value(entryHeader+"Animation",false).toBool());

      // Load the twoSidedLighting setting
      PluginFunctions::viewerProperties(i).twoSidedLighting(OpenFlipperSettings().value(entryHeader+"TwoSidedLighting",false).toBool());

      // Load the backface culling setting
      PluginFunctions::viewerProperties(i).backFaceCulling(OpenFlipperSettings().value(entryHeader+"BackfaceCulling",false).toBool());

      // Load the setting for the background color option
      PluginFunctions::viewerProperties(i).backgroundColor(OpenFlipperSettings().value(entryHeader+"BackgroundColor",QColor(0,0,0)).value< QColor >());
    }

  }
}

/** Write the standard options to the given Ini File
* @param _ini Inifile to use
*/
void Core::writeApplicationOptions(INIFile& _ini) {

  // save ViewModes
  QVector< QString > toolboxes;
  QVector< QString > toolbars;
  QVector< QString > contextmenus;
  QVector< QString > icons;

  if ( OpenFlipper::Options::gui() )
    for (int i=0; i < coreWidget_->viewModes_.size(); i++)
      if (coreWidget_->viewModes_[i]->custom){

        //store name
        QString entryToolboxes = coreWidget_->viewModes_[i]->name;

        //store widgets
        for (int j=0; j < coreWidget_->viewModes_[i]->visibleToolboxes.size(); j++)
          entryToolboxes += ";" + coreWidget_->viewModes_[i]->visibleToolboxes[j];

        toolboxes.push_back(entryToolboxes);

        //store name
        QString entryToolbars = coreWidget_->viewModes_[i]->name;

        //store widgets
        for (int j=0; j < coreWidget_->viewModes_[i]->visibleToolbars.size(); j++)
          entryToolbars += ";" + coreWidget_->viewModes_[i]->visibleToolbars[j];

        toolbars.push_back(entryToolbars);
	
	QString entryContextMenus = coreWidget_->viewModes_[i]->name;

        //store widgets
        for (int j=0; j < coreWidget_->viewModes_[i]->visibleContextMenus.size(); j++)
          entryContextMenus += ";" + coreWidget_->viewModes_[i]->visibleContextMenus[j];

        contextmenus.push_back(entryContextMenus);

        icons.push_back(coreWidget_->viewModes_[i]->icon);
      }

  //save viewmodes to ini
  _ini.add_entry("Options","ViewModeCount" ,toolboxes.size());
  for (int i=0; i < toolboxes.size(); i++) {
    _ini.add_entry("Options","ViewModeToolboxes"     + QString::number(i) ,toolboxes[i]);
    _ini.add_entry("Options","ViewModeToolbars"      + QString::number(i) ,toolbars[i] );
    _ini.add_entry("Options","ViewModeContextMenus"  + QString::number(i) ,contextmenus[i] );
    _ini.add_entry("Options","ViewModeIcon"          + QString::number(i) ,icons[i] );
  }

  //save KeyBindings
  if ( OpenFlipper::Options::gui() )
    coreWidget_->saveKeyBindings(_ini);

  //write default dataType to INI
  _ini.add_entry( "Options" , "default_DataType" , OpenFlipper::Options::lastDataType() );

  //write current ViewMode
  _ini.add_entry("Options","CurrentViewMode",OpenFlipper::Options::currentViewMode() );

  //============================================================================
  // Debugging
  //============================================================================
  _ini.add_entry("Options","SlotDebugging",OpenFlipper::Options::doSlotDebugging() );

  QString dir = OpenFlipperSettings().value("Core/CurrentDir").toString().toUtf8();
  _ini.add_entry("Options","StartupDir",dir);

  QString scriptDir = OpenFlipper::Options::currentScriptDirStr().toUtf8();
  _ini.add_entry("Options","CurrentScriptDir",scriptDir);

  QString current_texture_dir = OpenFlipper::Options::currentTextureDirStr().toUtf8();
  _ini.add_entry("Options","CurrentTextureDir",current_texture_dir);

  _ini.add_entry("Options","RandomDefaultColor", OpenFlipper::Options::randomDefaultColor() );

  if ( OpenFlipper::Options::gui() ) {

    _ini.add_entry("Options","DefaultViewerLayout", OpenFlipper::Options::defaultViewerLayout() );

    _ini.add_entry("Options","ViewerCount", OpenFlipper::Options::examinerWidgets() );

    for ( unsigned int i = 0 ; i < OpenFlipper::Options::examinerWidgets(); ++i ) {
      QString entryHead = "Viewer" + QString::number(i) + "/";
      OpenFlipperSettings().setValue(entryHead + "Animation", PluginFunctions::viewerProperties(i).animation());
      OpenFlipperSettings().setValue(entryHead + "BackfaceCulling", PluginFunctions::viewerProperties(i).backFaceCulling());
      OpenFlipperSettings().setValue(entryHead + "TwoSidedLighting", PluginFunctions::viewerProperties(i).twoSidedLighting());
      OpenFlipperSettings().setValue(entryHead + "BackgroundColor", PluginFunctions::viewerProperties(i).backgroundQColor());
    }

    //============================================================================
    // Save the current viewer properties
    //============================================================================
    std::vector< QString > draw_modes;

    for (int i=0; i < PluginFunctions::viewers(); i++ ){

      draw_modes = drawModeToList( OpenFlipper::Options::defaultDrawMode(i) );
      _ini.add_entry("Options","DefaultDrawModes" + QString::number(i), draw_modes);

    }

    _ini.add_entry("Options","DefaultColor", (uint)OpenFlipper::Options::defaultColor().rgba ()  );

    _ini.add_entry("Options", "StereoMode",OpenFlipper::Options::stereoMode() );

    _ini.add_entry("Options" , "CustomAnaglyphLeftEye", OpenFlipper::Options::anaglyphLeftEyeColorMatrix() );
    _ini.add_entry("Options" , "CustomAnaglyphRightEye", OpenFlipper::Options::anaglyphRightEyeColorMatrix() );
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

  // Tell plugins that we are currently reading an ini file
  OpenFlipper::Options::blockSceneGraphUpdates();

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

      for ( int i = 0 ; i < openFiles.size(); ++i ) {

        QString sectionName = openFiles[i];

        // Check if the string read is empty (e.g. colon at the end of the line ...)
        // So skip trying to read files without a filename.
        if ( sectionName.isEmpty() ) {
          emit log(LOGWARN,tr("Warning from ini file parser: OpenFiles list contains empty string.") );
          continue;
        }

        // Check if the specified section exists
        if ( !ini.section_exists(sectionName) ) {
          emit log(LOGERR,tr("Error parsing ini file. OpenFiles section %1 not found in File!").arg(sectionName));
          continue;
        }

        // Get the path for the file which should be opened
        QString path;
        if ( !ini.get_entry( path, sectionName , "path" ) ) {
          emit log(LOGERR,tr("Error parsing ini file. Section %1 contains no path description!").arg(sectionName));
          continue;
        }

        // Check if path is relative ( The path is considered to be relative if the first character is a ".")
        if (path.startsWith( "." + OpenFlipper::Options::dirSeparator() )){

          // check if _filename contains a path by testing if it contains a directory separator
          if (_filename.section(OpenFlipper::Options::dirSeparator(), 0, -2) != ""){
            path.remove(0,1); // remove .
            path = _filename.section(OpenFlipper::Options::dirSeparator(), 0, -2) + path;
          }

        }

        int tmpType;
        DataType type = typeId("TriangleMesh");

        // First check for old datatype style (Only numbers .. therefore not consistent for runtime added types)
        if ( ini.get_entry( tmpType, sectionName , "type"  )) {
          type = DataType(tmpType);
          emit log(LOGWARN, tr("This ini file uses old int style ObjectType fields!") );
          emit log(LOGWARN, tr("Please convert it to new format! ( ... just save it )") );
        } else {

          // Read new style type. The type is represented by its name (as a QString)
          QString typeName="";
          if ( ini.get_entry( typeName, sectionName , "type"  )) {
            type = typeId(typeName);
          } else
            emit log(LOGWARN, tr("Unable to get DataType for object %1 assuming Triangle Mesh!").arg(sectionName) );
        }

        // Now the object gets loaded based on the given datatype
        int newObjectId = loadObject(type, path);

        // get the new object from the object tree ( If that fails, the object was not loaded correctly)
        BaseObject* object = objectRoot_->childExists( newObjectId );
        if ( object == 0 )  {
          emit log(LOGERR,tr("Unable to open Object ") + path);
          continue;
        }

        // Read the target flag setting
        bool flag;
        if ( ini.get_entry( flag, sectionName , "target" ) )
          object->target(flag);

        // Read the source flag setting
        if ( ini.get_entry( flag, sectionName , "source" ) )
          object->source(flag);

        // Tell plugins to load their per object settings
        emit iniLoad( ini,object->id() );

      }
    }

  }

  // Tell Plugins that all objects are loaded and they should read the remaining parts if necessary
  if ( _perPluginSettings )
    emit iniLoadOptionsLast( ini );

  // close ini file
  ini.disconnect();

  // As the reading has been completed, tell plugins that we do not read an ini file anymore.
  OpenFlipper::Options::unblockSceneGraphUpdates();

  // Reset scenegraph and reset trackball center
  // This will also recompute the bounding boxes as well as the near and far plane
  resetScenegraph(true);

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
                        bool _saveObjectInfo,
                        std::map<int,QString>& _fileMapping) {

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

      QString file;
      std::map<int,QString>::iterator f = _fileMapping.find(o_it->id());
      if(f == _fileMapping.end()) {
          file = o_it->path() + OpenFlipper::Options::dirSeparator() + o_it->name();
      } else {
          file = f->second;
      }
    
      // Don't save default light source objects
      LightObject* light = 0;
      PluginFunctions::getObject( o_it->id(), light );
      if(light != 0) {
          if(light->defaultLight()) continue;
      }
    
      if (QFile(file).exists()){
        // Add a section for this object
        sectionName = o_it->name();
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
                                          o_it != PluginFunctions::objectsEnd(); ++o_it) {
      // Don't save default light source objects
      LightObject* light = 0;
      PluginFunctions::getObject( o_it->id(), light );
      if(light != 0) {
        if(light->defaultLight()) continue;
      }
        
      emit iniSave(  ini , o_it->id() );
    }
  }



  ini.disconnect();

  if ( OpenFlipper::Options::gui() ) {
    coreWidget_->statusMessage( tr("Saving ini File ") + _filename + tr(" ... Done"), 4000);
    coreWidget_->setStatus(ApplicationStatus::READY );
  }
}


//=============================================================================
