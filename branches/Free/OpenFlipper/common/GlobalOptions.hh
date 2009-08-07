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

/**
\file GlobalOptions.hh
 * This Header provides access to globally defined variables such as
 * Aplication Directories. You may use these Functions inside your plugins.
*/

#ifndef OPTIONS_HH
#define OPTIONS_HH

#include <OpenFlipper/common/GlobalDefines.hh>
#include <QDir>
#include <QStringList>
#include <QColor>
#include <QIcon>

namespace OpenFlipper {
namespace Options {

/// Returns the base Path of the application
DLLEXPORT
QDir applicationDir();

/// Returns the Path to the Standard Plugins
DLLEXPORT
QDir pluginDir();

/// Returns the Path to the Textures
DLLEXPORT
QDir textureDir();

/// Returns the Path to the License files
DLLEXPORT
QDir licenseDir();

/// Returns the Path to the Scripts
DLLEXPORT
QDir scriptDir();

/// Returns the Path to the shaders
DLLEXPORT
QDir shaderDir();

/// Returns the Path to the Icons
DLLEXPORT
QDir iconDir();

/// Returns the Path to the translation files
DLLEXPORT
QDir translationsDir();

/// Returns the Path to Fonts
DLLEXPORT
QDir fontsDir();

/// Returns the Path to the main data directory
DLLEXPORT
QDir dataDir();

/// Returns the base Path of the application
DLLEXPORT
QString applicationDirStr();

/// Returns the Path to the Standard Plugins
DLLEXPORT
QString pluginDirStr();

/// Returns the Path to the Shader directory
DLLEXPORT
QString shaderDirStr();

/// Returns the Path to the Textures
DLLEXPORT
QString textureDirStr();

/// Returns the Path to the License files
DLLEXPORT
QString licenseDirStr();

/// Returns the Path to the Scripts
DLLEXPORT
QString scriptDirStr();

/// Returns the Path to the Icons
DLLEXPORT
QString iconDirStr();

/// Returns the Path to the main data directory
DLLEXPORT
QString dataDirStr();

/// Returns the OpenFlipper Icon
DLLEXPORT
QIcon& OpenFlipperIcon();

/// Returns the Path to the translation files
DLLEXPORT
QString translationsDirStr();

/// Returns the Path to Fonts
DLLEXPORT
QString fontsDirStr();

/// Returns the Path to Help
DLLEXPORT
QString helpDirStr();

/// Sets the base Path of the application
DLLEXPORT
void applicationDir(QDir _dir);

/// Sets the Path to the Plugins
DLLEXPORT
void pluginDir(QDir _dir);

/// Sets the Path to the shaders
DLLEXPORT
void shaderDir(QDir _dir);

/// Sets the Path to the Textures
DLLEXPORT
void textureDir(QDir _dir);

/// Sets the Path to the Licenses
DLLEXPORT
void licenseDir(QDir _dir);

/// Sets the Path to the Scripts
DLLEXPORT
void scriptDir( QDir _dir);

/// Sets the Path to the Icons
DLLEXPORT
void iconDir(QDir _dir);

/// Sets the Path to the translation files
DLLEXPORT
void translationsDir( QDir _dir);

/// Sets the Path to the Fonts
DLLEXPORT
void fontsDir(QDir _dir);

/// Sets the Path to the Help
DLLEXPORT
void helpDir(QDir _dir);

/// Sets the Path to the data directory
DLLEXPORT
void dataDir(QDir _dir);

/// Sets the base Path of the application
DLLEXPORT
bool applicationDir(QString _dir);

/// Sets the Path to the Plugins
DLLEXPORT
bool pluginDir(QString _dir);

/// Sets the Path to the shaders
DLLEXPORT
bool shaderDir(QString _dir);

/// Sets the Path to the Textures
DLLEXPORT
bool textureDir(QString _dir);

/// Sets the Path to the License files
DLLEXPORT
bool licenseDir(QString _dir);

/// Sets the Path to the Scripts
DLLEXPORT
bool scriptDir(QString _dir);

/// Sets the Path to the Icons
DLLEXPORT
bool iconDir(QString _dir);

/// Sets the Path to the translation files
DLLEXPORT
bool translationsDir(QString _dir);

/// Sets the Path to the Fonts
DLLEXPORT
void fontsDir(QDir _dir);

/// Sets the Path to the Help
DLLEXPORT
void helpDir(QDir _dir);

/// Sets the Path to the data direcotry
DLLEXPORT
bool dataDir(QString _dir);


//===========================================================================
/** @name Option files / paths
* @{ */
//===========================================================================

  /// Set the list of option files
  DLLEXPORT
  void optionFiles(QStringList _list);

  /// returns a list of all available option files
  DLLEXPORT
  QStringList optionFiles();

  /// Sets the Path to the configuration directory
  DLLEXPORT
  void configDir(QDir _dir);

  /// Sets the Path to the configuration directory
  DLLEXPORT
  bool configDir(QString _dir);

  /// Returns the base Path of the config Dir
  DLLEXPORT
  QString configDirStr();

  /// Returns the base Path of the config Dir
  DLLEXPORT
  QDir configDir();

/** @} */

//===========================================================================
/** @name current status
* @{ */
//===========================================================================

  /// return the current directory
  DLLEXPORT
  QString currentDirStr();

  /// return the current directory
  DLLEXPORT
  QDir currentDir();

  /// Sets the Path to the current directory
  DLLEXPORT
  void currentDir(QDir _dir);

  /// Sets the Path to the current directory
  DLLEXPORT
  bool currentDir(QString _dir);

  /// return the current script-directory
  DLLEXPORT
  QString currentScriptDirStr();

  /// return the current Script-directory
  DLLEXPORT
  QDir currentScriptDir();

  /// Sets the Path to the current Script-directory
  DLLEXPORT
  void currentScriptDir(QDir _dir);

  /// Sets the Path to the current Script-directory
  DLLEXPORT
  bool currentScriptDir(QString _dir);



  /// return the current texture-directory
  DLLEXPORT
  QString currentTextureDirStr();

  /// return the current texture-directory
  DLLEXPORT
  QDir currentTextureDir();

  /// Sets the Path to the current texture-directory
  DLLEXPORT
  void currentTextureDir(QDir _dir);

  /// Sets the Path to the current texture-directory
  DLLEXPORT
  bool currentTextureDir(QString _dir);



  /// return last used dataType
  DLLEXPORT
  QString lastDataType();

  /// set the last used dataType
  DLLEXPORT
  void lastDataType(QString _type);

/** @} */

//===========================================================================
/** @name Architecture detection and Architecture specific stuff
* @{ */
//===========================================================================

  DLLEXPORT
  bool is64bit();

  DLLEXPORT
  bool is32bit();

  DLLEXPORT
  bool isLinux();

  DLLEXPORT
  bool isWindows();

  DLLEXPORT
  bool isDarwin();

  /// Returns the right separator String for this architecture ( Linux: "/" Windows: "\" )
  DLLEXPORT
  QString dirSeparator();

/** @} */

//===========================================================================
/** @name Environment
* @{ */
//===========================================================================

  /// Returns the LANG environment variable string
  DLLEXPORT
  QString lang();

  /// Returns the translation language (locale, de_DE, en_US)
  DLLEXPORT
  QString translation();

  /// Set the translation language (locale, de_DE, en_US)
  DLLEXPORT
  void translation(QString _translation);

/** @} */

//===========================================================================
/** @name Experience level  of user
* @{ */
//===========================================================================
  enum Experience {
    NOVICE,
    ADVANCED,
    EXPERT
  };

  /// Get experience level
  DLLEXPORT
  Experience getExperience();

  /// Set experience level
  DLLEXPORT
  void setExperience( Experience _exp );

/** @} */

//===========================================================================
/** @name Visualization Control
* @{ */
//===========================================================================

  /// Store if a gui should be visible or not
  DLLEXPORT
  void nogui( bool _nogui );

  /// get if a gui should be visible or not
  DLLEXPORT
  bool nogui( );

  /// get if a gui should be visible or not
  DLLEXPORT
  bool gui( );

  /// Store stereo support setting
  DLLEXPORT
  void stereo( bool _stereo );

  /// get current stereo support setting
  DLLEXPORT
  bool stereo( );

  /// Stereo mode
  enum StereoMode {
    OpenGL,
    AnaglyphRedCyan,
    AnaglyphCustom
  };

  /// Store opengl stereo support setting
  DLLEXPORT
  void glStereo( bool _stereo );

  /// will be set in core is opengl stereo is supported
  DLLEXPORT
  bool glStereo( );

  /// Store stereo mode setting
  DLLEXPORT
  void stereoMode( StereoMode _mode );

  /// get current stereo mode setting
  DLLEXPORT
  StereoMode stereoMode( );

  /// Store stereo eye distance
  DLLEXPORT
  void eyeDistance( float _eye );

  /// get stereo eye distance
  DLLEXPORT
  float eyeDistance( );

  /// Store stereo focal distance relative to scene near plane
  DLLEXPORT
  void focalDistance( float _focal );

  /// get stereo focal distance relative to scene near plane
  DLLEXPORT
  float focalDistance( );

  /// Store the 3x3 left eye color matrix values for custom anaglyph stereo mode
  DLLEXPORT
  void anaglyphLeftEyeColorMatrix( std::vector<float> _mat );

  /// get the 3x3 left eye color matrix values for custom anaglyph stereo mode
  DLLEXPORT
  std::vector<float> anaglyphLeftEyeColorMatrix( );

  /// Store the 3x3 right eye color matrix values for custom anaglyph stereo mode
  DLLEXPORT
  void anaglyphRightEyeColorMatrix( std::vector<float> _mat );

  /// get the 3x3 right eye color matrix values for custom anaglyph stereo mode
  DLLEXPORT
  std::vector<float> anaglyphRightEyeColorMatrix( );

  /// Enables/Disables mouse cursor depth picking during stereo mode
  DLLEXPORT
  void stereoMousePick( bool _stereoPick );

  /// mouse cursor depth picking during stereo mode
  DLLEXPORT
  bool stereoMousePick( );

  /// Store synchronization setting
  DLLEXPORT
  void synchronization( bool _synchronization );

  /// get current synchronization setting
  DLLEXPORT
  bool synchronization( );

  /// Store wheelZoomFactor setting
  DLLEXPORT
  void wheelZoomFactor( double _factor );

  /// get wheelZoomFactor setting
  DLLEXPORT
  double wheelZoomFactor( );

  /// Store wheelZoomFactorShift setting
  DLLEXPORT
  void wheelZoomFactorShift( double _factor );

  /// get wheelZoomFactorShift setting
  DLLEXPORT
  double wheelZoomFactorShift( );

  /// Store restrictFrameRate setting
  DLLEXPORT
  void restrictFrameRate( bool _enable );

  /// get restrictFrameRate setting
  DLLEXPORT
  bool restrictFrameRate( );

  /// set wheel appearance setting
  DLLEXPORT
  void showWheelsAtStartup( bool _show );

  /// get wheel appearance setting
  DLLEXPORT
  bool showWheelsAtStartup( );

  /// Store maxFrameRate setting
  DLLEXPORT
  void maxFrameRate( int _fps );

  /// get maxFrameRate setting
  DLLEXPORT
  int maxFrameRate( );

  /// Store defaultDrawMode setting
  DLLEXPORT
  void defaultDrawMode( uint _mode, int _viewer );

  /// get defaultDrawMode setting
  DLLEXPORT
  uint defaultDrawMode( int _viewer );

  /// Store defaultProjectionMode setting
  DLLEXPORT
  void defaultProjectionMode( int _mode, int _viewer );

  /// get defaultProjectionMode setting
  DLLEXPORT
  int defaultProjectionMode( int _viewer );

  /// Store defaultViewingDirection setting
  DLLEXPORT
  void defaultViewingDirection( int _mode, int _viewer );

  /// get defaultViewingDirection setting
  DLLEXPORT
  int defaultViewingDirection( int _viewer );

  /// Store defaultViewerLayout setting
  DLLEXPORT
  void defaultViewerLayout( int _layout );

  /// get defaultViewerLayout setting
  DLLEXPORT
  int defaultViewerLayout();

  /// Store disableRedraw setting
  DLLEXPORT
  void redrawDisabled( bool disable );

  /// get disableRedraw setting
  DLLEXPORT
  bool redrawDisabled( );

  /// Get the flag if a drawMode Menu should always be visible in the context menu
  DLLEXPORT
  bool drawModesInContextMenu();

  /// Set the flag if a drawMode Menu should always be visible in the context menu
  DLLEXPORT
  void drawModesInContextMenu(bool _show);

  /// Get the flag if a grid should be drawn in every viewer
  DLLEXPORT
  bool gridVisible();

  /// Set the flag if a grid should be drawn in every viewer
  DLLEXPORT
  void gridVisible(bool _visible);

  /// Store Gl based cursor painting
  DLLEXPORT
  void glMouse( bool _glMouse );

  /// Should the mouse cursor be painted with gl
  DLLEXPORT
  bool glMouse( );

/** @} */

//===========================================================================
/** @name GUI Control
* @{ */
//===========================================================================

  /// Use multiview Mode?
  DLLEXPORT
  void multiView( bool _multiView);

  /// Use multiview Mode?
  DLLEXPORT
  bool multiView();

  /// Number of examiner widgets
  DLLEXPORT
  unsigned int examinerWidgets();

  /// State of the logging widget
  enum LoggerState {
    InScene,
    Normal,
    Hidden
  };

  /// Set the logging widget state
  DLLEXPORT
  void loggerState( LoggerState _state);

  /// What is the current state of the logging widget?
  DLLEXPORT
  LoggerState loggerState( );

  /// Set if we start the toolbox widget closed
  DLLEXPORT
  void hideToolbox( bool _hide);

  /// Start the toolbox widget closed?
  DLLEXPORT
  bool hideToolbox( );

  /// Start fullscreen Mode?
  DLLEXPORT
  void fullScreen( bool _fs );

  /// Start fullscreen Mode?
  DLLEXPORT
  bool fullScreen( );

  /// Which mode should be the default for the toolbar?
  DLLEXPORT
  QString defaultToolboxMode( );

  /// Which mode should be the default for the toolbar?
  DLLEXPORT
  void defaultToolboxMode( QString _mode );

  /// Titel of the main window
  DLLEXPORT
  QString windowTitle( );

  /// Set title of the main window
  DLLEXPORT
  void windowTitle( QString _titel );

  /// Display splash screen?
  DLLEXPORT
  void splash( bool _splash );

  /// Display splash screen?
  DLLEXPORT
  bool splash( );

  /// Default background color
  DLLEXPORT
  void defaultBackgroundColor( QColor _color );

  /// Default background color
  DLLEXPORT
  QColor defaultBackgroundColor();

  /// Default base color
  DLLEXPORT
  void defaultBaseColor( QColor _color );

  /// Default base color
  DLLEXPORT
  QColor defaultBaseColor();

  /// use randomized base color
  DLLEXPORT
  void randomBaseColor(bool _random);

  /// use randomized base color
  DLLEXPORT
  bool randomBaseColor();

/** @} */

//===========================================================================
/** @name Scripting Options
 * @{ */
//===========================================================================

  /** Scripting status. If this flag is set to true,
   * you should not call functions for multiple objects. Or do
   * remote procedure calls to functions which add scripting commands.
   * As this might call functions twice when executed via scripts. See the
   * scripting plugin for additional information
   */
  DLLEXPORT
  bool scripting( );

  /** Toggle scripting status (normally done by the Scripting plugin only)
   */
  DLLEXPORT
  void scripting(bool _status );

  /** @} */

//===========================================================================
/** @name Logging Options
 * @{ */
//===========================================================================

  /// Log to console output?
  DLLEXPORT
  bool logToConsole( );

  /// Log to console output?
  DLLEXPORT
  void logToConsole(bool _logToConsole );

  /// Give extra debug information
  DLLEXPORT
  bool debug();

  /// Give extra debug information
  DLLEXPORT
  void debug(bool _debug );

  /// enable logging to file
  DLLEXPORT
  void logFileEnabled(bool _enable );

  /// is logging to file enabled?
  DLLEXPORT
  bool logFileEnabled();

  /// logfile location (path + filename)
  DLLEXPORT
  void logFile(QString _filename);

  /// logfile location (path + filename)
  DLLEXPORT
  QString logFile();

/** @} */

//===========================================================================
/** @name Backup operations
* @{ */
//===========================================================================

  /// Do backups?
  DLLEXPORT
  bool backupEnabled( );

  /// Do backups?
  DLLEXPORT
  void enableBackup(bool _enableBackup );


/** @} */

//===========================================================================
/** @name Settings
* @{ */
//===========================================================================

  /// currently loading Settings ?
  DLLEXPORT
  bool loadingSettings( );

  /// currently loading Settings ?
  DLLEXPORT
  void loadingSettings(bool _loadingSettings );

  /// currently saving Settings ?
  DLLEXPORT
  bool savingSettings( );

  /// currently saving Settings ?
  DLLEXPORT
  void savingSettings(bool _savingSettings );

  /// currently loading recent file ?
  DLLEXPORT
  bool loadingRecentFile( );

  /// currently loading recent file ?
  DLLEXPORT
  void loadingRecentFile(bool _loadingRecentFile );

/** @} */

//===========================================================================
/** @name Core parameters
* @{ */
//===========================================================================

/// Get the argc parameter from the main application
DLLEXPORT
int* argc();

/// Get the argv parameter from the main application
DLLEXPORT
char *** argv();

/// Set the argc parameter from the main application ( Dont use it, only called from Core!!)
DLLEXPORT
void  argc( int* _argc );

/// Set the argv parameter from the main application ( Dont use it, only called from Core!!)
DLLEXPORT
void argv( char*** _argv);

/// is the core remoteControlled?
DLLEXPORT
bool remoteControl();

/// is the core remoteControlled?
DLLEXPORT
void remoteControl( bool _remote );

//===========================================================================
/** @name Debugging Options
 * @{ */
//===========================================================================

/** If you enable slot debugging, for some slots will be displayed who called them.
 * Currently supported slots are:\n
 * - updateView()
 * - updated_objects( objectId )
 */
DLLEXPORT
void doSlotDebugging( bool _debugging );

DLLEXPORT
bool doSlotDebugging( );

/** @} */

//===========================================================================
/** @name Update Options
 * @{ */
//===========================================================================

/** Set Server URL to check for updates
 */
DLLEXPORT
void updateUrl( QString _url );

/** Get Server URL to check for updates
 */
DLLEXPORT
QString updateUrl( );

/** Set Username for updates
 */
DLLEXPORT
void updateUsername( QString _username );

/** Get Username for updates
 */
DLLEXPORT
QString updateUsername( );

/** Set Password for updates
 */
DLLEXPORT
void updatePassword( QString _password );

/** Get Password for updates
 */
DLLEXPORT
QString updatePassword( );


/** @} */

//===========================================================================
/** @name Version information
 * @{ */
//===========================================================================

/// Version of OpenFlipper Core
DLLEXPORT
QString coreVersion();

/** @} */

//===========================================================================
/** @name Picking Debugger
 * @{ */
//===========================================================================

/** Set rendering Mode for Picking Debugger
 */
DLLEXPORT
void pickingRenderMode( QString _target );

/** Get rendering Mode for Picking Debugger
 */
DLLEXPORT
QString pickingRenderMode( );

/** Enable/Disable Picking Debugger
 */
DLLEXPORT
void renderPicking(bool _enable);

/** Get Picking Debugger state
 */
DLLEXPORT
bool renderPicking( );

/** @} */
}
}

//=============================================================================
#endif // OPTIONS_HH defined
//=============================================================================
