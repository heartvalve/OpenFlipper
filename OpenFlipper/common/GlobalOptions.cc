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
//  Options used throughout the System
//
//=============================================================================

#include <QDir>

#include "GlobalOptions.hh"
#include <cstdlib>
#include <iostream>
#include <ACG/Scenegraph/DrawModes.hh>
#include <ACG/GL/ShaderGenerator.hh>
#include <OpenFlipper/BasePlugin/PluginFunctionsViewControls.hh>
#include <QDir>
#include <QCoreApplication>

namespace OpenFlipper {
namespace Options {

/// This Variable will hold special compiler information
static QString compilerInfo_ = "";
  
/// Pointer to the internal settings object storing OpenFlippers program options ( and the pplugins Options)  
static OpenFlipperQSettings* settings_ = 0;  

/// Stores the base Path of the application
static QDir applicationDir_;

/// Stores the base Path of the application
static QDir configDir_;

/// Stores the Path to the Standard Plugins
static QDir pluginDir_;

/// Stores the Path to the shaders
static QDir shaderDir_;

/// Stores the Path to the Textures
static QDir textureDir_;

/// Stores the Path to the Licenses
static QDir licenseDir_;

/// Stores the Path to the Scripts
static QDir scriptDir_;

/// Stores the Path to the Icons
static QDir iconDir_;

/// Stores the OpenFlipper Icon
static QIcon* OpenFlipperIcon_ = 0;

/// Stores the Path to the translation files
static QDir translationsDir_;

/// Stores the Path to the Fonts
static QDir fontsDir_;

/// Stores the Path to the Help
static QDir helpDir_;

/// Stores the Path to the data directory
static QDir dataDir_;

/// Stores all available option files in the Order in which they should be used
static QStringList optionFiles_;

/// Currentdirectory of the application
static QDir currentDir_;

/// Current script directory of the application
static QDir currentScriptDir_;

/// Current texture directory of the application
static QDir currentTextureDir_;

/// Is system in startup phase
static bool startup_ = false;

/// Should ini-files be deleted on exit?
static bool deleteIniFile_ = false;

/// Show gui or not?
static bool nogui_ = false;

/// Store the stereo support
static bool stereo_ = true;

/// Store the opengl stereo support
static bool glStereo_ = true;

/// Stereo mode
static StereoMode stereoMode_ = OpenGL;

/// vector containing left/right color matrices for custom anaglyph mode
static std::vector<float> anaglyphLeftEyeColors_ = std::vector<float> (9, 0.0);
static std::vector<float> anaglyphRightEyeColors_ = std::vector<float> (9, 0.0);

/// Store the synchronization mode
static bool synchronization_ = false;

/// Store the defaultDrawMode_ mode
static std::vector<ACG::SceneGraph::DrawModes::DrawMode> defaultDrawMode_ = std::vector<ACG::SceneGraph::DrawModes::DrawMode> (4, ACG::SceneGraph::DrawModes::SOLID_SMOOTH_SHADED);

/// Store the defaultViewer layout
static int defaultViewerLayout_ = 0;

/// Store the disableRedraw_ mode
static int redrawDisabled_ = 0;

/// Set if a drawMode Menu should always be in the context menu
static bool drawModesInContextMenu_ = true;

/// Current view mode
static QString currentViewMode_ = "";

static QString title_ = TOSTRING(PRODUCT_NAME)" v?";

/// default base color
static QColor defaultColor_ = QColor("white");

/// use random base color
static bool randomDefaultColor_ = false;

/// Scripting status
static bool scripting_ = false;

/// Log to logger and console
static bool logToConsole_ = false;

/// Extra debug info
static bool debug_ = false;

/// Slot Debugging
static bool doSlotDebugging_ = false;

/// last used DataType
static QString lastDataType_ = "Triangle Mesh";

/// Should backups be enabled
static bool enableBackup_ = true;

/// Updates currently blocked?
static int sceneGraphUpdatesBlocked_ = 0;

/// Currently loading recent file?
static bool loadingRecentFile_ = false;

/// Currently saving Settings?
static bool savingSettings_ = false;

static int* argc_;

static char *** argv_;

static bool remoteControl_ = false;

static int remoteControlPort_ = 2000;

# if __WORDSIZE == 64
  const bool is64 = true;
#else
  const bool is64 = false;
#endif

#if defined(WIN32)
  const bool linux_ = false;
  const bool windows_ = true;
  const bool darwin_ = false;
#elif defined(ARCH_DARWIN)
  const bool darwin_ = true;
  const bool linux_ = false;
  const bool windows_ = false;
#else
  const bool darwin_ = false;
  const bool linux_ = true;
  const bool windows_ = false;
#endif

bool is64bit() { return is64; };
bool is32bit() { return !is64; };

bool isLinux() {
  return linux_;
}

bool isWindows() {
  return windows_;
}

bool isDarwin() {
 return darwin_;
}


QString lang() {
  QString lang = getenv( "LANG" ) ;
  return lang;
}

QString dirSeparator() {
//   if (windows_)
//     return QString("\\");
//   if (linux_)
//     return QString("/");

  return QString("/");
}

QDir applicationDir()   { return applicationDir_; }
QDir configDir()        { return configDir_;  }
QDir pluginDir()        { return pluginDir_;  }
QDir shaderDir()        { return shaderDir_;  }
QDir textureDir()       { return textureDir_; }
QDir licenseDir()       { return licenseDir_; }
QDir scriptDir()        { return scriptDir_; }
QDir iconDir()          { return iconDir_;    }
QDir translationsDir()  { return translationsDir_;   }
QDir fontsDir()         { return fontsDir_;   }
QDir helpDir()         	{ return helpDir_;    }
QDir dataDir()          { return dataDir_;    }
QDir currentDir()       { return currentDir_; }
QDir currentScriptDir() { return currentScriptDir_; }
QDir currentTextureDir(){ return currentTextureDir_; }

QString applicationDirStr()    { return applicationDir_.absolutePath();    }
QString configDirStr()         { return configDir_.absolutePath();         }
QString pluginDirStr()         { return pluginDir_.absolutePath();         }
QString shaderDirStr()         { return shaderDir_.absolutePath();         }
QString textureDirStr()        { return textureDir_.absolutePath();        }
QString licenseDirStr()        { return licenseDir_.absolutePath();        }
QString scriptDirStr()         { return scriptDir_.absolutePath();         }
QString iconDirStr()           { return iconDir_.absolutePath();           }
QString translationsDirStr()   { return translationsDir_.absolutePath();   }
QString fontsDirStr()          { return fontsDir_.absolutePath();          }
QString helpDirStr()           { return helpDir_.absolutePath();           }
QString dataDirStr()           { return dataDir_.absolutePath();           }
QString currentDirStr()        { return currentDir_.absolutePath();        }
QString currentScriptDirStr()  { return currentScriptDir_.absolutePath();  }
QString currentTextureDirStr() { return currentTextureDir_.absolutePath(); }

QStringList optionFiles()   { return optionFiles_; }

void optionFiles(QStringList _list) { optionFiles_ = _list; }

bool startup() { return startup_; };

void finishedStartup() { startup_ = false;};

QIcon& OpenFlipperIcon() {

  if (OpenFlipperIcon_ == 0){
    OpenFlipperIcon_ = new QIcon( iconDirStr() + dirSeparator() + "OpenFlipper_Icon_128x128x32.png");
    OpenFlipperIcon_->addFile( iconDirStr() + dirSeparator() + "OpenFlipper_Icon_48x48x32.png", QSize(48,48) );
    OpenFlipperIcon_->addFile( iconDirStr() + dirSeparator() + "OpenFlipper_Icon_32x32x32.png", QSize(32,32) );
    OpenFlipperIcon_->addFile( iconDirStr() + dirSeparator() + "OpenFlipper_Icon_16x16x32.png", QSize(16,16) );
  }

  return *OpenFlipperIcon_;
}

void currentDir(QDir      _dir) {
  currentDir_ = _dir;
}

bool currentDir(QString      _dir) {
  QDir tmp(_dir);
  if (tmp.exists()) {
    currentDir_     = tmp;
    return true;
  }
  return false;
}


bool currentScriptDir(QString _dir) {
  QDir tmp(_dir);
  if (tmp.exists()) {
    currentScriptDir_     = tmp;
    return true;
  }
  return false;
}

bool currentTextureDir(QString _dir) {
  QDir tmp(_dir);
  if (tmp.exists()) {
    currentTextureDir_ = tmp;
    return true;
  }
  return false;
}

void deleteIniFile(bool _delete) {
    deleteIniFile_ = _delete;
}

bool deleteIniFile() {
    return deleteIniFile_;
}

/// Store if a gui should be visible or not
void nogui( bool _nogui ) {
  nogui_ = _nogui;
}

/// get if a gui should be visible or not
bool nogui( ) {
  return nogui_;
}

/// get if a gui should be visible or not
bool gui( ) {
  return !nogui_;
}

/// Store stereo setting
void stereo( bool _stereo ) {
  stereo_ = _stereo;
}

/// get current stereo setting
bool stereo( ) {
  return stereo_;
}

/// Store opengl stereo mode setting
void glStereo( bool _glStereo ) {
  glStereo_ = _glStereo;
}

/// get current opengl stereo setting
bool glStereo( ) {
  return glStereo_;
}

/// Store stereo mode setting
void stereoMode( StereoMode _mode ) {
  stereoMode_ = _mode;
}

/// get current stereo mode setting
StereoMode stereoMode( ) {
  return stereoMode_;
}

/// get stereo eye distance (default = 7cm)
void eyeDistance( double _eye ) {
  settings_->setValue("Core/Stereo/EyeDistance",_eye);
}

/// get stereo eye distance
double eyeDistance( )
{
  return settings_->value("Core/Stereo/EyeDistance",0.07f).toDouble();
}

/// Store stereo focal distance relative to scene near plane
void focalDistance( float _focal )
{
  settings_->setValue("Core/Stereo/FocalDistance",_focal);
}

/// get stereo focal distance relative to scene near plane
float focalDistance( )
{
  return settings_->value("Core/Stereo/FocalDistance",0.5f).toDouble();
}

/// Store the 3x3 left eye color matrix values for custom anaglyph stereo mode
void anaglyphLeftEyeColorMatrix( std::vector<float> _mat )
{
  for (int i = 0; i < 9; i++)
    anaglyphLeftEyeColors_[i] = _mat[i];
}

/// get the 3x3 left eye color matrix values for custom anaglyph stereo mode
std::vector<float> anaglyphLeftEyeColorMatrix( )
{
  return anaglyphLeftEyeColors_;
}

/// Store the 3x3 right eye color matrix values for custom anaglyph stereo mode
void anaglyphRightEyeColorMatrix( std::vector<float> _mat )
{
  for (int i = 0; i < 9; i++)
    anaglyphRightEyeColors_[i] = _mat[i];
}

/// get the 3x3 right eye color matrix values for custom anaglyph stereo mode
std::vector<float> anaglyphRightEyeColorMatrix( )
{
  return anaglyphRightEyeColors_;
}

/// Store synchronization mode setting
void synchronization( bool _synchronization ) {
  synchronization_ = _synchronization;
}

/// get current synchronization setting
bool synchronization( ) {
  return synchronization_;
}

void defaultDrawMode( ACG::SceneGraph::DrawModes::DrawMode _mode, int _viewer){
  defaultDrawMode_[_viewer] = _mode;
}

ACG::SceneGraph::DrawModes::DrawMode defaultDrawMode(int _viewer){
  return defaultDrawMode_[_viewer];
}

void defaultPerspectiveProjectionMode( bool _mode, int _viewer){

  // Core/Gui/glViewer<viewer> Orthogonal = false,Perspective = true (bool)\n

  QString entry = "Viewer" + QString::number(_viewer) + "/perspectiveProjection";
  OpenFlipperSettings().setValue(entry,_mode);

}

bool defaultPerspectiveProjectionMode(int _viewer){

  QString entry = "Viewer" + QString::number(_viewer) + "/perspectiveProjection";

  switch (_viewer) {
    case 0:
      return OpenFlipperSettings().value(entry,true).toBool();
    case 1:
      return OpenFlipperSettings().value(entry,false).toBool();
    case 2:
      return OpenFlipperSettings().value(entry,false).toBool();
    case 3:
      return OpenFlipperSettings().value(entry,false).toBool();
    default:
      std::cerr << "defaultProjectionMode: illegal viewer id: " << _viewer << std::endl;
      break;
  }

  return true;
}

void defaultViewingDirection( int _mode, int _viewer){
  QString entry = "Viewer" + QString::number(_viewer) + "/defaultViewingDirection";
  OpenFlipperSettings().setValue(entry,_mode);
}

int defaultViewingDirection(int _viewer){

  QString entry = "Viewer" + QString::number(_viewer) + "/defaultViewingDirection";

  switch (_viewer) {
    case 0:
      return OpenFlipperSettings().value(entry,PluginFunctions::VIEW_FREE).toInt();
    case 1:
      return OpenFlipperSettings().value(entry,PluginFunctions::VIEW_FRONT).toInt();
    case 2:
      return OpenFlipperSettings().value(entry,PluginFunctions::VIEW_RIGHT).toInt();
    case 3:
      return OpenFlipperSettings().value(entry,PluginFunctions::VIEW_TOP).toInt();
    default:
      std::cerr << "defaultViewingDirection: illegal viewer id: " << _viewer << std::endl;
      break;
  }

  return PluginFunctions::VIEW_FREE;
}

void defaultLockRotation( bool _lock, int _viewer ) {
  QString entry = "Viewer" + QString::number(_viewer) + "/defaultLockRotation";
  OpenFlipperSettings().setValue(entry,_lock);
}

bool defaultLockRotation( int _viewer ) {

  QString entry = "Viewer" + QString::number(_viewer) + "/defaultLockRotation";

  switch (_viewer) {
    case 0:
      return OpenFlipperSettings().value(entry,false).toBool();
    case 1:
      return OpenFlipperSettings().value(entry,true).toBool();
    case 2:
      return OpenFlipperSettings().value(entry,true).toBool();
    case 3:
      return OpenFlipperSettings().value(entry,true).toBool();
    default:
      std::cerr << "defaultViewingDirection: illegal viewer id: " << _viewer << std::endl;
      break;
  }

  return false;
}


void defaultViewerLayout( int _layout ){
  defaultViewerLayout_ = _layout;
}

int defaultViewerLayout(){
  return defaultViewerLayout_;
}

void redrawDisabled( bool disable ){
  if ( disable )
    redrawDisabled_++;
  else
    if ( redrawDisabled_ == 0 )
      std::cerr << "Mismatch of redraw enable/disable!!" << std::endl;
    else
      redrawDisabled_--;
}

bool redrawDisabled( ){
  return (redrawDisabled_ > 0);
}

bool drawModesInContextMenu() {
  return drawModesInContextMenu_;
}

void drawModesInContextMenu(bool _show) {
  drawModesInContextMenu_ = _show;
}

unsigned int examinerWidgets() {
  if ( OpenFlipperSettings().value("Core/Gui/glViewer/useMultipleViewers",true).toBool() )
    return 4;
  else
    return 1;
}

/// Set to current
void loggerState( LoggerState _state) {
  OpenFlipperSettings().setValue("Core/Gui/LogWindow/LogWindowMode",_state);
}

/// Current state of the logging window?
LoggerState loggerState( ) {
  return static_cast<OpenFlipper::Options::LoggerState> (OpenFlipperSettings().value("Core/Gui/LogWindow/LogWindowMode",0).toInt() );
}

/// Which mode should is currently selected?
QString currentViewMode( ) {
  return currentViewMode_;
}

/// Which view mode is currently selected?
void currentViewMode( QString _mode ) {
  currentViewMode_ = _mode;
}


QString windowTitle( ) {
  return title_;
}

void windowTitle( QString _titel ) {
  title_ = _titel;
}

void defaultColor( QColor _color ) {
  defaultColor_ = _color;
}

QColor defaultColor() {
  return defaultColor_;
}

void randomDefaultColor(bool _random) {
  randomDefaultColor_ = _random;
}

bool randomDefaultColor() {
  return randomDefaultColor_;
}

/// return last used dataType
QString lastDataType(){
  return lastDataType_;
}

/// set the last used dataType
void lastDataType(QString _type){
   lastDataType_ = _type;
}

bool scripting( ) {
  return scripting_;
}

void scripting(bool _status ) {
  scripting_ = _status;
}

bool logToConsole( ) {
  return logToConsole_;
}

void logToConsole(bool _logToConsole ) {
  logToConsole_ = _logToConsole;
}

bool debug() {
  return debug_;
}

void debug(bool _debug ) {
  debug_ = _debug;
}

bool backupEnabled( ) {
  return enableBackup_;
}

void enableBackup(bool _enableBackup ) {
  enableBackup_ = _enableBackup;
}

void blockSceneGraphUpdates( ) {
  ++sceneGraphUpdatesBlocked_;
}

void unblockSceneGraphUpdates() {
  if ( sceneGraphUpdatesBlocked_ <= 0)
      std::cerr << "Error: More unblocks than blocks! Ignoring this request" << std::endl;
  else
    --sceneGraphUpdatesBlocked_;
}

bool sceneGraphUpdatesBlocked( ) {
  if ( sceneGraphUpdatesBlocked_ < 0)
    std::cerr << "Error: updates blocked < 0 ... more unblocks than blocks" << std::endl;

  return (sceneGraphUpdatesBlocked_ > 0);
}

bool savingSettings( ) {
  return savingSettings_;
}

void savingSettings(bool _savingSettings ) {
  savingSettings_ = _savingSettings;
}

bool loadingRecentFile( ) {
  return loadingRecentFile_;
}

void loadingRecentFile(bool _loadingRecentFile ) {
  loadingRecentFile_ = _loadingRecentFile;
}

int* argc() {
  return argc_;
}

char *** argv() {
  return argv_;
}

void  argc( int* _argc ) {
  argc_ = _argc;
}

void argv( char*** _argv) {
  argv_ = _argv;
}

bool remoteControl(){
  return remoteControl_;
}

void remoteControl(bool _remote){
  remoteControl_ = _remote;
}

int remoteControlPort(){
  return remoteControlPort_;
}

void remoteControlPort(int _remotePort){
  remoteControlPort_ = _remotePort;
}

void doSlotDebugging( bool _debugging ) {
  doSlotDebugging_ = _debugging;
}

bool doSlotDebugging( ) {
  return doSlotDebugging_;
}

QString coreVersion() {
  #ifndef CORE_VERSION
	#define CORE_VERSION "1.0.0RC2"
  #endif
  return QString(CORE_VERSION);
}

static const char * const CONFIG_DIR = "." TOSTRING(PRODUCT_STRING);

bool initializeSettings() {
  
  #if defined(__INTEL_COMPILER)
    compilerInfo_ = "Intel: " + QString(  TOSTRING(__INTEL_COMPILER) ) + "." + QString(  TOSTRING(__GNUC_MINOR__) ) ;
    #if defined(__INTEL_COMPILER_BUILD_DATE)
      compilerInfo_ += " BuildDate " + QString(  TOSTRING(__INTEL_COMPILER_BUILD_DATE) );
    #endif
  #elif defined(__GNUC__)
    compilerInfo_ = "Gnu CC: " + QString(  TOSTRING(__GNUC__) ) + "." + QString(  TOSTRING(__GNUC_MINOR__) ) ;
    #if defined(__GNUC_PATCHLEVEL__)
      compilerInfo_ += "." + QString(  TOSTRING(__GNUC_PATCHLEVEL__) );
    #endif
  #elif defined (_MSC_FULL_VER)
    compilerInfo_ = "MSVC: " + QString(  TOSTRING(_MSC_FULL_VER) );
  #else
    compilerInfo_ = "Unknown Compiler";
  #endif
  
  
  //==================================================================================================
  // Get the Main config dir in the home directory and possibly create it
  //==================================================================================================
  #if  defined(ARCH_DARWIN)
    configDir_ = QDir::home();

    if ( ! configDir_.cd("Library") )
      configDir_.mkdir("Library");

    if ( ! configDir_.cd("Application Support") )
      configDir_.mkdir("Application Support");

    if ( ! configDir_.cd("de.rwth-aachen.graphics.openflipper") ) {
      configDir_.mkdir("de.rwth-aachen.graphics.openflipper");
      if ( ! configDir_.cd("de.rwth-aachen.graphics.openflipper") ) {
        std::cerr << "Unable to create config dir ~/Library/de.rwth-aachen.graphics.openflipper" << std::endl;
        return false;
      }
    }
  #else
    configDir_ = QDir::home();
    if ( ! configDir_.cd(CONFIG_DIR) ) {
      std::cerr << "Creating config Dir ~/" << CONFIG_DIR << std::endl;;
      configDir_.mkdir(CONFIG_DIR);
      if ( ! configDir_.cd(CONFIG_DIR) ) {
        std::cerr << "Unable to create config dir ~/" << CONFIG_DIR << std::endl;
        return false;
      }
    }

  #endif

  
  //==================================================================================================
  // Setup settings. 
  //==================================================================================================
  // This has to be done as early as possible to set the program options right
  
  // Force ini format on all platforms
  QSettings::setDefaultFormat ( QSettings::IniFormat );
  
  // Force settings to be stored in the OpenFlipper config directory
  QSettings::setPath( QSettings::IniFormat, QSettings::UserScope , configDir_.absolutePath() );

  // Finally attach the settings object.
  settings_ = new OpenFlipperQSettings(QSettings::IniFormat, QSettings::UserScope, "ACG", "OpenFlipper");

  //==================================================================================================
  // Now create special directories in th OpenFlipper config dir
  //==================================================================================================
  
  // Create a personal Icon cache dir to save for example user added icons
  if ( ! configDir_.exists("Icons") ){
    configDir_.mkdir("Icons");
    std::cerr << "Creating Icon Cache Dir ~/" << CONFIG_DIR << "/Icons" << std::endl;
  }
  
  //==================================================================================================
  // Setup main application dir
  //==================================================================================================

  // Remember the main application directory (assumed to be one above executable Path)
  applicationDir_ =  QCoreApplication::applicationDirPath();

  // When using cmake, we get the absolute path to the Application directory via a define
  applicationDir_.cd(OPENFLIPPER_APPDIR);
  
  //==================================================================================================
  // Setup directory containing plugins
  //==================================================================================================
   
  // start at application directory 
  pluginDir_ = applicationDir_;
  
  // cmake style: Path is directly given from define!
  pluginDir_.cd(OPENFLIPPER_PLUGINDIR);
  
  dataDir_ = OpenFlipper::Options::applicationDir();
  
  #ifdef OPENFLIPPER_DATADIR
    dataDir_.cd(OPENFLIPPER_DATADIR);
  #else
    dataDir_ = OpenFlipper::Options::applicationDir();
  #endif
  
  // Set the Path to the Shaders
  shaderDir_ = dataDir_;
  shaderDir_.cd("Shaders");
  ACG::ShaderProgGenerator::setShaderDir(OpenFlipper::Options::shaderDirStr());
  
  // Set the Path to the textures
  textureDir_ = dataDir_;
  textureDir_.cd("Textures");

  // Set the Path to the Scripts
  scriptDir_ = dataDir_;
  scriptDir_.cd("Scripts");
  
  
  // Set the Path to the Icons
  iconDir_ = dataDir_;
  iconDir_.cd("Icons");
  
  // Set the Path to the translations
  translationsDir_ = dataDir_;
  translationsDir_.cd("Translations");
  
  // Set the Path to the Fonts
  fontsDir_ = dataDir_;
  fontsDir_.cd("Fonts");
  
  // Set the Path to the License files
  licenseDir_ = dataDir_;
  licenseDir_.cd("Licenses");
  
  // Set the Path to the Help
  helpDir_ = dataDir_;
  helpDir_.cd("Help");
  
  //==================================================================================================
  // Initialize with default values if not already set
  //==================================================================================================  

  // User Interface Settings
  if ( ! settings_->contains("Core/Gui/glViewer/defaultBackgroundColor") )
    settings_->setValue("Core/Gui/glViewer/defaultBackgroundColor",QColor("black"));  
  
  if ( ! settings_->contains("Core/Gui/glViewer/showControlWheels") )
    settings_->setValue("Core/Gui/glViewer/showControlWheels",false);  
  
  
  // Mouse Controls
  if ( ! settings_->contains("Core/Mouse/Wheel/ZoomFactor") )
    settings_->setValue("Core/Mouse/Wheel/ZoomFactor",1.0);
  
  if ( ! settings_->contains("Core/Mouse/Wheel/ZoomFactorShift") )
    settings_->setValue("Core/Mouse/Wheel/ZoomFactorShift",0.2);
  
  if ( ! settings_->contains("Core/Mouse/Wheel/Invert") )
      settings_->setValue("Core/Mouse/Wheel/Invert",false);
  
  // General Stereo Settings
  if ( ! settings_->contains("Core/Stereo/FocalLength") )
    settings_->setValue("Core/Stereo/FocalLength",0.5);
  
  if ( ! settings_->contains("Core/Stereo/EyeDistance") )
    settings_->setValue("Core/Stereo/EyeDistance",0.07);
    
  return true;
  
}

void closeSettings() {
  
  // Delete the settings object. This will flush all data to the disk.
  delete settings_;
}

QString compilerInfo() {
  return compilerInfo_;
}
 
}
}

OpenFlipperQSettings& OpenFlipperSettings() {
  // Empty standard settings object if the right settings are not available!
  static OpenFlipperQSettings emptySettings;
  
  if ( OpenFlipper::Options::settings_ )
    return *OpenFlipper::Options::settings_;
  else
    return emptySettings;
}



//=============================================================================

