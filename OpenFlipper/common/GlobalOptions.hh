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

/**
\file GlobalOptions.hh
 * This Header provides access to globally defined variables such as
 * Aplication Directories. You may use these Functions inside your plugins.
*/

#ifndef OPTIONS_HH
#define OPTIONS_HH

#ifdef WIN32
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif

#include <QDir>
#include <QStringList>

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

/// Returns the Path to the Icons
DLLEXPORT
QDir iconDir();

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

/// Returns the Path to the Icons
DLLEXPORT
QString iconDirStr();

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

/// Sets the Path to the Icons
DLLEXPORT
void iconDir(QDir _dir);

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

/// Sets the Path to the Icons
DLLEXPORT
bool iconDir(QString _dir);


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

  /// Store stereo mode setting
  DLLEXPORT
  void stereo( bool _stereo );

  /// get current stereo setting
  DLLEXPORT
  bool stereo( );

  /// Store animation mode setting
  DLLEXPORT
  void animation( bool _animation );

  /// get current animation setting
  DLLEXPORT
  bool animation( );

  /// Store backfaceCulling setting
  DLLEXPORT
  void backfaceCulling( bool _backfaceCulling );

  /// get backface Culling setting
  DLLEXPORT
  bool backfaceCulling( );

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

  /// Store maxFrameRate setting
  DLLEXPORT
  void maxFrameRate( int _fps );

  /// get maxFrameRate setting
  DLLEXPORT
  int maxFrameRate( );

  /// Store standardDrawMode setting
  DLLEXPORT
  void standardDrawMode( uint _mode );

  /// get standardDrawMode setting
  DLLEXPORT
  uint standardDrawMode( );

  /// Store disableRedraw setting
  DLLEXPORT
  void redrawDisabled( bool disable );

  /// get disableRedraw setting
  DLLEXPORT
  bool redrawDisabled( );

/** @} */

//===========================================================================
/** @name GUI Control
* @{ */
//===========================================================================

  /// Set if we start the logging widget closed
  DLLEXPORT
  void hideLogger( bool _hide);

  /// Start the logging widget closed?
  DLLEXPORT
  bool hideLogger( );

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
/** @name INI operations
* @{ */
//===========================================================================

  /// Do backups?
  DLLEXPORT
  bool openingIni( );

  /// Do backups?
  DLLEXPORT
  void openingIni(bool _openingIni );


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
}
}

//=============================================================================
#endif // OPTIONS_HH defined
//=============================================================================
