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
//  Options used throughout the System
//
//=============================================================================

#include <QDir>

#include "GlobalOptions.hh"
#include <stdlib.h>
#include <iostream>
#include <ACG/Scenegraph/DrawModes.hh>


namespace OpenFlipper {
namespace Options {

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

/// Experience level of the user
static Experience experienceLevel_ = NOVICE;

/// Show gui or not?
static bool nogui_ = false;

/// Store the stereo support
static bool stereo_ = true;

/// Store the opengl stereo support
static bool glStereo_ = true;

/// Stereo mode
static StereoMode stereoMode_ = OpenGL;

/// get stereo eye distance (default = 7cm)
static float eyeDistance_ = 0.07f;

/// stereo focal distance relative to scene near plane (default = to center of scene)
static float focalDistance_ = 0.5;

/// vectroy containing left/right color matrices for custom anaglyph mode
static std::vector<float> anaglyphLeftEyeColors_ = std::vector<float> (9, 0.0);
static std::vector<float> anaglyphRightEyeColors_ = std::vector<float> (9, 0.0);

/// mouse cursor depth picking in stereo mode
static bool stereoMousePick_ = true;

/// Store the synchronization mode
static bool synchronization_ = false;

/// Store the wheelZoomFactor mode
static double wheelZoomFactor_ = 1.0;

/// Store the wheelZoomFactorShift mode
static double wheelZoomFactorShift_ = 0.2;

/// Store the restrictFrameRate_ mode
static bool restrictFrameRate_ = false;

/// Store the wheel settings
static bool showWheelsAtStartup_ = false;

/// Store the maxFrameRate_ mode
static int maxFrameRate_ = 35;

/// Store the defaultDrawMode_ mode
static std::vector<uint> defaultDrawMode_ = std::vector<uint> (4, ACG::SceneGraph::DrawModes::SOLID_SMOOTH_SHADED);

/// Store the defaultProjectionMode_ mode
static std::vector<int> defaultProjectionMode_ = std::vector<int> (4, 1);

/// Store the defaultViewingDirection_ mode
static std::vector<int> defaultViewingDirection_ = std::vector<int> (4, 0);

/// Store the defaultViewer layout
static int defaultViewerLayout_ = 0;

/// Store the disableRedraw_ mode
static int redrawDisabled_ = 0;

/// Set if a drawMode Menu should always be in the context menu
static bool drawModesInContextMenu_ = true;

/// Set if a grid should be drawn in every viewer
static bool gridVisible_ = false;

/// Should OpenGL be used to draw the mouse cursor
static bool glMouse_ = true;

/// Store the toolbox gui Mode mode
static bool hideToolbox_ = false;

/// Store the logger gui Mode mode
static LoggerState loggerState_ = Normal;

/// Store if we should go into multiview Mode
static bool multiView_ = true;

/// Start in Fullscreen Mode?
static bool fullScreen_ = false;

static QString defaultToolboxMode_ = "";

static QString title_ = "OpenFlipper v?";

/// Show a splash screen on init?
static bool splash_ = true;

/// default background color for the viewer
static QColor defaultBackgroundColor_ = QColor("black");

/// default base color
static QColor defaultBaseColor_ = QColor("white");

/// use random base color
static bool randomBaseColor_ = false;

/// Scripting status
static bool scripting_ = false;

/// Log to logger and console
static bool logToConsole_ = false;

/// Extra debug info
static bool debug_ = false;

/// Slot Debugging
static bool doSlotDebugging_ = false;

/// URL for updates
static QString updateURL_ = "";

/// Username for updates
static QString updateUsername_ = "";

/// Password for updates
static QString updatePassword_ = "";

/// enable logging to file
static bool logFileEnabled_;

/// logfile location (path + filename)
static QString logFile_;

/// last used DataType
static QString lastDataType_ = "Triangle Mesh";

/// translation
static QString translation_ = "en_US";

/// Should backups be enabled
static bool enableBackup_ = true;

/// Currently loading Settings?
static bool loadingSettings_ = false;

/// Currently loading recent file?
static bool loadingRecentFile_ = false;

/// Currently saving Settings?
static bool savingSettings_ = false;

/// Enable picking renderer?
static bool renderPicking_ = false;

/// picking Renderer Mode
static QString pickingRenderMode_ = "PICK_ANYTHING";

static int* argc_;

static char *** argv_;

static bool remoteControl_ = false;

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

QString translation() {
  return translation_;
}

void translation(QString _translation) {
  translation_ = _translation;
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

void applicationDir(QDir _dir)       { applicationDir_    = _dir; }
void pluginDir(QDir _dir)            { pluginDir_         = _dir; }
void shaderDir(QDir _dir)            { shaderDir_         = _dir; }
void textureDir(QDir _dir)           { textureDir_        = _dir; }
void licenseDir(QDir _dir)           { licenseDir_        = _dir; }
void scriptDir(QDir _dir)            { scriptDir_         = _dir; }
void iconDir(QDir _dir)              { iconDir_           = _dir; }
void tanslationsDir(QDir _dir)       { translationsDir_   = _dir; }
void fontsDir(QDir _dir)             { fontsDir_          = _dir; }
void helpDir(QDir _dir)              { helpDir_           = _dir; }
void dataDir(QDir _dir)              { dataDir_           = _dir; }
void configDir(QDir _dir)            { configDir_         = _dir; }
void currentDir(QDir _dir)           { currentDir_        = _dir; }
void currentScriptDir(QDir _dir)     { currentScriptDir_  = _dir; }
void currentTextureDir(QDir _dir)    { currentTextureDir_ = _dir; }

void optionFiles(QStringList _list) { optionFiles_ = _list; }

bool applicationDir(QString _dir) {
  QDir tmp(_dir);
  if (tmp.exists()) {
    applicationDir_ = tmp;
    return true;
  }
  return false;
}

bool pluginDir(QString      _dir) {
  QDir tmp(_dir);
  if (tmp.exists()) {
    pluginDir_      = tmp;
    return true;
  }
  return false;
}

bool shaderDir(QString _dir) {
  QDir tmp(_dir);
  if (tmp.exists()) {
    shaderDir_ = tmp;
    return true;
  }
  return false;
}

bool textureDir(QString     _dir) {
  QDir tmp(_dir);
  if (tmp.exists()) {
    textureDir_     = tmp;
    return true;
  }
  return false;
}

bool licenseDir(QString     _dir) {
  QDir tmp(_dir);
  if (tmp.exists()) {
    licenseDir_     = tmp;
    return true;
  }
  return false;
}

bool scriptDir(QString     _dir) {
  QDir tmp(_dir);
  if (tmp.exists()) {
    scriptDir_     = tmp;
    return true;
  }
  return false;
}

bool iconDir(QString      _dir) {
  QDir tmp(_dir);
  if (tmp.exists()) {
    iconDir_ = tmp;
    return true;
  }
  return false;
}

bool dataDir(QString      _dir) {
  QDir tmp(_dir);
  if (tmp.exists()) {
    dataDir_ = tmp;
    return true;
  }
  return false;
}

QIcon& OpenFlipperIcon() {

  if (OpenFlipperIcon_ == 0){
    OpenFlipperIcon_ = new QIcon( iconDirStr() + dirSeparator() + "OpenFlipper_Icon_128x128x32.png");
    OpenFlipperIcon_->addFile( iconDirStr() + dirSeparator() + "OpenFlipper_Icon_48x48x32.png", QSize(48,48) );
    OpenFlipperIcon_->addFile( iconDirStr() + dirSeparator() + "OpenFlipper_Icon_32x32x32.png", QSize(32,32) );
    OpenFlipperIcon_->addFile( iconDirStr() + dirSeparator() + "OpenFlipper_Icon_16x16x32.png", QSize(16,16) );
  }

  return *OpenFlipperIcon_;
}


bool translationsDir(QString      _dir) {
  QDir tmp(_dir);
  if (tmp.exists()) {
    translationsDir_ = tmp;
    return true;
  }
  return false;
}

bool fontsDir(QString      _dir) {
  QDir tmp(_dir);
  if (tmp.exists()) {
    fontsDir_      = tmp;
    return true;
  }
  return false;
}

bool helpDir(QString      _dir) {
  QDir tmp(_dir);
  if (tmp.exists()) {
    helpDir_      = tmp;
    return true;
  }
  return false;
}

bool configDir(QString      _dir) {
  QDir tmp(_dir);
  if (tmp.exists()) {
    configDir_      = tmp;
    return true;
  }
  return false;
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

/// Get experience level
Experience getExperience() {
  return experienceLevel_;
}

/// Set experience level
void setExperience( Experience _exp) {
  experienceLevel_ = _exp;
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

/// Store stereo eye distance
void eyeDistance( float _eye )
{
  eyeDistance_ = _eye;
}

/// get stereo eye distance
float eyeDistance( )
{
  return eyeDistance_;
}

/// Store stereo focal distance relative to scene near plane
void focalDistance( float _focal )
{
  focalDistance_ = _focal;
}

/// get stereo focal distance relative to scene near plane
float focalDistance( )
{
  return focalDistance_;
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

/// Enables/Disables mouse cursor depth picking during stereo mode
void stereoMousePick( bool _stereoMousePick ) {
  stereoMousePick_ = _stereoMousePick;
}

/// mouse cursor depth picking during stereo mode
bool stereoMousePick( ) {
  return stereoMousePick_;
}

/// Store synchronization mode setting
void synchronization( bool _synchronization ) {
  synchronization_ = _synchronization;
}

/// get current synchronization setting
bool synchronization( ) {
  return synchronization_;
}

/// Store wheelZoomFactor setting
void wheelZoomFactor( double _factor ) {
  wheelZoomFactor_ = _factor;
}

/// get wheelZoomFactor setting
double wheelZoomFactor( ) {
  return wheelZoomFactor_;
}

/// Store wheelZoomFactorShift setting
void wheelZoomFactorShift( double _factor ) {
  wheelZoomFactorShift_ = _factor;
}

/// get wheelZoomFactorShift setting
double wheelZoomFactorShift( ) {
  return wheelZoomFactorShift_;
}

void restrictFrameRate( bool _enable ){
  restrictFrameRate_ = _enable;
}

void showWheelsAtStartup( bool _show ){
  showWheelsAtStartup_ = _show;
}

bool restrictFrameRate( ){
  return restrictFrameRate_;
}

bool showWheelsAtStartup( ){
  return showWheelsAtStartup_;
}

void maxFrameRate( int _fps ){
  maxFrameRate_ = _fps;
}

int maxFrameRate(){
  return maxFrameRate_;
}

void defaultDrawMode( uint _mode, int _viewer){
  defaultDrawMode_[_viewer] = _mode;
}

uint defaultDrawMode(int _viewer){
  return defaultDrawMode_[_viewer];
}

void defaultProjectionMode( int _mode, int _viewer){
  defaultProjectionMode_[_viewer] = _mode;
}

int defaultProjectionMode(int _viewer){
  return defaultProjectionMode_[_viewer];
}

void defaultViewingDirection( int _mode, int _viewer){
  defaultViewingDirection_[_viewer] = _mode;
}

int defaultViewingDirection(int _viewer){
  return defaultViewingDirection_[_viewer];
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

bool gridVisible() {
  return gridVisible_;
}

void gridVisible(bool _visible) {
  gridVisible_ = _visible;
}

void multiView( bool _multiView) {
  multiView_ = _multiView;
}

bool multiView() {
  return multiView_;
}

unsigned int examinerWidgets() {
  if ( multiView_)
    return 4;
  else
    return 1;
}

/// Store Gl based cursor painting
void glMouse( bool _glMouse ) {
  glMouse_ = _glMouse;
}

/// Should the mouse cursor be painted with gl
bool glMouse( ) {
  return glMouse_;
}

/// Set to current
void loggerState( LoggerState _state) {
  loggerState_ = _state;
}

/// Current state of the logging window?
LoggerState loggerState( ) {
  return loggerState_;
}

/// Set if we start the toolbox widget closed
void hideToolbox( bool _hide) {
  hideToolbox_ = _hide;
}

/// Start the toolbox widget closed?
bool hideToolbox( ) {
  return hideToolbox_;
}

/// Start fullscreen Mode?
void fullScreen( bool _fs ) {
  fullScreen_ = _fs;
}

/// Start fullscreen Mode?
bool fullScreen( ) {
  return fullScreen_;
}

/// Which mode should be the default for the toolbar?
QString defaultToolboxMode( ) {
  return defaultToolboxMode_;
}

/// Which mode should be the default for the toolbar?
void defaultToolboxMode( QString _mode ) {
  defaultToolboxMode_ = _mode;
}


QString windowTitle( ) {
  return title_;
}

void windowTitle( QString _titel ) {
  title_ = _titel;
}

void splash( bool _splash ) {
  splash_ = _splash;
}

bool splash( ) {
  return splash_;
}

void defaultBackgroundColor( QColor _color ) {
  defaultBackgroundColor_ = _color;
}

QColor defaultBackgroundColor() {
  return defaultBackgroundColor_;
}

void defaultBaseColor( QColor _color ) {
  defaultBaseColor_ = _color;
}

QColor defaultBaseColor() {
  return defaultBaseColor_;
}

void randomBaseColor(bool _random) {
  randomBaseColor_ = _random;
}

bool randomBaseColor() {
  return randomBaseColor_;
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

void logFileEnabled(bool _enable ){
  logFileEnabled_ = _enable;
}

bool logFileEnabled(){
  return logFileEnabled_;
}

void logFile(QString _filename){
  logFile_ = _filename;
}

QString logFile(){
  return logFile_;
}

bool backupEnabled( ) {
  return enableBackup_;
}

void enableBackup(bool _enableBackup ) {
  enableBackup_ = _enableBackup;
}

bool loadingSettings( ) {
  return loadingSettings_;
}

void loadingSettings(bool _loadingSettings ) {
  loadingSettings_ = _loadingSettings;
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

void doSlotDebugging( bool _debugging ) {
  doSlotDebugging_ = _debugging;
}

bool doSlotDebugging( ) {
  return doSlotDebugging_;
}

void updateUrl( QString _url ) {
  updateURL_ = _url;
}

QString updateUrl( ) {
  return updateURL_;
}

void updateUsername( QString _username ) {
  updateUsername_ = _username;
}

QString updateUsername( ) {
  return updateUsername_;
}

void updatePassword( QString _password ) {
  updatePassword_ = _password;
}

QString updatePassword( ) {
  return updatePassword_;
}

QString coreVersion() {
  #ifndef CORE_VERSION
	#define CORE_VERSION "1.0.0RC2"
  #endif
  return QString(CORE_VERSION);
}

void pickingRenderMode( QString _target ) {
  pickingRenderMode_ = _target;
}

QString pickingRenderMode( ) {
  return pickingRenderMode_;
}

void renderPicking(bool _enable) {
  renderPicking_ = _enable;
}

bool renderPicking( ) {
  return renderPicking_;
}


}
}

//=============================================================================

