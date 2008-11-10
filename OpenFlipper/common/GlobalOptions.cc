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
//  Options used throughout the System
//
//=============================================================================

#include <QDir>

#include "GlobalOptions.hh"
#include <stdlib.h>
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

/// Stores the Path to the Icons
static QDir iconDir_;

/// Stores all available option files in the Order in which they should be used
static QStringList optionFiles_;

/// Currentdirectory of the application
static QDir currentDir_;

/// Current script directory of the application
static QDir currentScriptDir_;

/// Experience level of the user
static Experience experienceLevel_ = NOVICE;

/// Show gui or not?
static bool nogui_ = false;

/// Store the stereo mode
static bool stereo_ = false;

/// Store the animation mode
static bool animation_ = false;

/// Store the backfaceCulling mode
static bool backfaceCulling_ = false;

/// Store the wheelZoomFactor mode
static double wheelZoomFactor_ = 1.0;

/// Store the wheelZoomFactorShift mode
static double wheelZoomFactorShift_ = 0.2;

/// Store the restrictFrameRate_ mode
static bool restrictFrameRate_ = false;

/// Store the maxFrameRate_ mode
static int maxFrameRate_ = 35;

/// Store the standardDrawMode_ mode
static uint standardDrawMode_ = ACG::SceneGraph::DrawModes::SOLID_SMOOTH_SHADED;

/// Store the disableRedraw_ mode
static bool redrawDisabled_ = false;

/// Set if a drawMode Menu should always be in the context menu
static bool drawModesInContextMenu_ = true;

/// Store the toolbox gui Mode mode
static bool hideToolbox_ = false;

/// Store the logger gui Mode mode
static bool hideLogger_ = false;

/// Start in Fullscreen Mode?
static bool fullScreen_ = false;

static QString defaultToolboxMode_ = "";

static QString title_ = "OpenFlipper v?";

/// Show a splash screen on init?
static bool splash_ = true;

/// default background color for the viewer
static QRgb defaultBackgroundColor_ = 0;

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

# if __WORDSIZE == 64
  const bool is64 = true;
#else
  const bool is64 = false;
#endif

#ifdef WIN32
  const bool linux_ = false;
  const bool windows_ = true;
#else
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

QDir applicationDir()  { return applicationDir_; }
QDir configDir()       { return configDir_;  }
QDir pluginDir()       { return pluginDir_;  }
QDir shaderDir()       { return shaderDir_;  }
QDir textureDir()      { return textureDir_; }
QDir iconDir()         { return iconDir_;    }
QDir currentDir()      { return currentDir_; }
QDir currentScriptDir(){ return currentScriptDir_; }

QString applicationDirStr()  { return applicationDir_.absolutePath() ; }
QString configDirStr()       { return configDir_.absolutePath();       }
QString pluginDirStr()       { return pluginDir_.absolutePath();       }
QString shaderDirStr()       { return shaderDir_.absolutePath();       }
QString textureDirStr()      { return textureDir_.absolutePath();      }
QString iconDirStr()         { return iconDir_.absolutePath();         }
QString currentDirStr()      { return currentDir_.absolutePath();      }
QString currentScriptDirStr(){ return currentScriptDir_.absolutePath();}

QStringList optionFiles()   { return optionFiles_; }

void applicationDir(QDir _dir)       { applicationDir_   = _dir; }
void pluginDir(QDir _dir)            { pluginDir_        = _dir; }
void shaderDir(QDir _dir)            { shaderDir_        = _dir; }
void textureDir(QDir _dir)           { textureDir_       = _dir; }
void iconDir(QDir _dir)              { iconDir_          = _dir; }
void configDir(QDir _dir)            { configDir_        = _dir; }
void currentDir(QDir _dir)           { currentDir_       = _dir; }
void currentScriptDir(QDir _dir)     { currentScriptDir_ = _dir; }

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

bool iconDir(QString      _dir) {
  QDir tmp(_dir);
  if (tmp.exists()) {
    iconDir_      = tmp;
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

/// Store stereo mode setting
void stereo( bool _stereo ) {
  stereo_ = _stereo;
}

/// get current stereo setting
bool stereo( ) {
  return stereo_;
}

/// Store animation mode setting
void animation( bool _animation ) {
  animation_ = _animation;
}

/// get current animation setting
bool animation( ) {
  return animation_;
}

/// Store backfaceCulling setting
void backfaceCulling( bool _backfaceCulling ) {
  backfaceCulling_ = _backfaceCulling;
}

/// get backfaceCulling setting
bool backfaceCulling( ) {
  return backfaceCulling_;
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

bool restrictFrameRate( ){
  return restrictFrameRate_;
}

void maxFrameRate( int _fps ){
  maxFrameRate_ = _fps;
}

int maxFrameRate(){
  return maxFrameRate_;
}

void standardDrawMode( uint _mode ){
  standardDrawMode_ = _mode;
}

uint standardDrawMode(){
  return standardDrawMode_;
}

void redrawDisabled( bool disable ){
  redrawDisabled_ = disable;
}

bool redrawDisabled( ){
  return redrawDisabled_;
}

bool drawModesInContextMenu() {
  return drawModesInContextMenu_;
}

void drawModesInContextMenu(bool _show) {
  drawModesInContextMenu_ = _show;
}

/// Set if we start the logging widget closed
void hideLogger( bool _hide) {
  hideLogger_ = _hide;
}

/// Start the logging widget closed?
bool hideLogger( ) {
  return hideLogger_;
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

void defaultBackgroundColor( QRgb _color ) {
  defaultBackgroundColor_ = _color;
}

QRgb defaultBackgroundColor() {
  return defaultBackgroundColor_;
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
  return QString("0.9.1");
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

