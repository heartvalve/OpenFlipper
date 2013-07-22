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
#include <ACG/Scenegraph/DrawModes.hh>
#include <QDir>
#include <QStringList>
#include <QColor>
#include <QIcon>
#include "OpenFlipperQSettings.hh"

//===========================================================================
/** @name Persistent settings
* @{ */
//===========================================================================

/** \brief QSettings object containing all program settings of OpenFlipper.
*
* To get the settings use:\n
* OpenFlipperSettings().value("<Key>").toDouble();\n
* OpenFlipperSettings().value("<Key>").toBool();
*
* To change settings do:\n
* OpenFlipperSettings().setValue("<Key>","<Value>");\n
* For detailed info about the values see the documentation of QSettings and the QVariant.\n
*
* \note For each of your plugins, the key has to start with your plugin name!!!
*
* The following entries are available through the settings:
*
* <b>User Interface Settings</b>\n
* * Core/Gui/glViewer/defaultBackgroundColor This controls the default background color of the glViewer (QColor)\n
* * Core/Gui/glViewer/showControlWheels      This setting controls if the control wheels should be visible in the glViewer (bool)\n
* * Core/Gui/glViewer/glMouse                This setting controls if the mouse is rendered in OpenGL or in the window manager(bool)\n
* * Core/Gui/glViewer/stereoMousePick        This setting controls if the mouse depth is picked in stereo mode(bool)\n
* * Core/Gui/glViewer/restrictFrameRate      This setting controls if the maximum frame rate should be restricted (bool)\n
* * Core/Gui/glViewer/maxFrameRate           If restrictFrameRate is set to yes, this setting controls the maximum frameRate used (unsigned int)\n
* * Core/Gui/glViewer/useMultipleViewers     Use only one viewer or automatically generate multiple viewers (bool)\n
* * Core/Gui/glViewer/minimalSceneSize       The minimal scene radius. If the scene is smaller, than the size will be set to this value (double)\n
* * Core/Gui/ToolBoxes/hidden                Hide the toolbox sidebar (bool)\n
* * Core/Gui/ToolBoxes/ToolBoxOnTheRight     Toolbox on the right or on the left (bool)\n
* * Core/Gui/ToolBars/PickToolbarInScene     Render picking toolbar into gl scene or as toolbox into main widget
* * Core/Gui/TaskSwitcher/Hide               Show or Hide the task switcher(bool)\n
* * Core/Gui/LogWindow/LogWindowMode         Log Window Mode .. 0 InScene, 1 Window, 2 Hidden (int)\n
* * Core/Gui/LogWindow/OpenMeshErrors        Log OpenMesh Errors? (bool)\n
* * Core/Gui/splash                          This setting controls if the splash screen is shown at startup(bool)\n
* * Core/Gui/fullscreen                      Show OpenFlipper in Fullscreen Mode(bool)\n
* * Core/Projection/FOVY                     Field of view for projection matrix in perspective mode(double)\n
*
* <b>User Interface Settings for each of the viewers</b>\n
* * Viewer<viewer>/perspectiveProjection     Orthogonal = false,Perspective = true (bool)\n
* * Viewer<viewer>/defaultViewingDirection   0:VIEW_FREE; 1:VIEW_TOP; 2:VIEW_BOTTOM; 3:VIEW_LEFT; 4:VIEW_RIGHT; 5:VIEW_FRONT; 6:VIEW_BACK (int)\n
* * Viewer<viewer>/defaultLockRotation       true: locked , false: unlocked (bool)\n
* * Viewer<viewer>/Animation                 true: enabled, false: disabled (bool)\n
* * Viewer<viewer>/BackfaceCulling           true: enabled, false: disabled (bool)\n
* * Viewer<viewer>/TwoSidedLighting          true: enabled, false: disabled (bool)\n
* * Viewer<viewer>/BackgroundColor           Color of the viewer background (QColor)\n
* * Viewer<viewer>/DefaultRenderer           Name of the default Renderer   (QString)\n
*
* <b>File Handling</b>\n
* * Core/CurrentDir                          The current Directory of the Application \n
* * Core/File/RecentFiles                    List of Recent Files \n
* * Core/File/RecentTypes                    DataTypes of Recent Files \n
* * Core/File/MaxRecent                      Maximum entries in recent File list \n
* * Core/File/UseLoadDefaults                Use the defaults saved in the file plugins \n
*
* <b>Log Settings</b>\n
* * Core/Log/Logfile                         This is the location of the log file \n
* * Core/Log/logFileEnabled                  This controls, if logging to file is enabled(bool)\n
*
* <b>Language Settings</b>\n
* * Core/Language/Translation                This is the translation to use ( default is en_US)  \n
*
* <b>Update Settings</b>\n
* * Core/Update/UserName                     Update UserName
* * Core/Update/Pass                         Update Password
* * Core/Update/URL                          Update URL
* 
* <b>Mouse Controls</b>\n
* * Core/Mouse/Wheel/ZoomFactor       Zoom factor used when turning the mouse wheel (double)\n
* * Core/Mouse/Wheel/ZoomFactorShift  Zoom factor used when turning the mouse wheel while shift is pressed (makes zoom slower while shift is pressed)  (double)\n
* * Core/Mouse/Wheel/Invert           Invert mouse wheel zoom direction  (bool)\n
*
* <b>General Stereo Settings</b>\n
* * Core/Stereo/EyeDistance    Distance between eyes. Defaults to 7 cm (double)\n
* * Core/Stereo/FocalDistance  Focal distance. Defaults to center of scene (double)\n
* \n
* <b>Philips stereo display settings</b>\n
* * Core/Stereo/Philips/Content  Philips stereo header content type (int)\n
* * Core/Stereo/Philips/Factor   Philips stereo header factor (int)\n
* * Core/Stereo/Philips/Offset   Philips stereo header offset cc (int)\n
* * Core/Stereo/Philips/Select   Philips stereo header select (int)\n
* \n
* <b>Rendering debugger Settings</b>\n
* * Core/Debug/Picking/RenderPicking      Enable picking rendering (bool)\n
* * Core/Debug/Picking/RenderPickingMode  The Mode used for picking rendering(QString) \n
* * Core/Debug/Picking/RenderDepthImage   Renders the depth buffer as a grayscale image (bool) \n
* \n
* Anaglyph stereo mode settings:\n
*/


#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

DLLEXPORT
OpenFlipperQSettings& OpenFlipperSettings();

/** @} */

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
  
  /// Returns true if ini-file should be deleted on exit
  DLLEXPORT
  bool deleteIniFile();
  
  /// Set option to delete ini-files on exit
  DLLEXPORT
  void deleteIniFile(bool _delete);

/** @} */

//===========================================================================
/** @name current status
* @{ */
//===========================================================================

  ///  Returns if the system is currently in a startup phase
  DLLEXPORT
  bool startup();
  
  /// Set status to startup complete
  DLLEXPORT
  void finishedStartup();

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

  /// Store synchronization setting
  DLLEXPORT
  void synchronization( bool _synchronization );

  /// get current synchronization setting
  DLLEXPORT
  bool synchronization( );

  /// Store defaultDrawMode setting
  DLLEXPORT
  void defaultDrawMode( ACG::SceneGraph::DrawModes::DrawMode _mode, int _viewer );

  /// get defaultDrawMode setting
  DLLEXPORT
  ACG::SceneGraph::DrawModes::DrawMode defaultDrawMode( int _viewer );

  /** \brief Store defaultProjectionMode setting

    Perspective projection = true;
    Orthogonal Projection  = false;
  */
  DLLEXPORT
  void defaultPerspectiveProjectionMode( bool _mode, int _viewer );

  /// get defaultProjectionMode setting
  DLLEXPORT
  bool defaultPerspectiveProjectionMode( int _viewer );

  /// Store defaultViewingDirection setting
  DLLEXPORT
  void defaultViewingDirection( int _mode, int _viewer );

  /// get defaultViewingDirection setting
  DLLEXPORT
  int defaultViewingDirection( int _viewer );

  /// Store lockrotation setting
  DLLEXPORT
  void defaultLockRotation( bool _mode, int _viewer );

  /// get lockrotation setting
  DLLEXPORT
  bool defaultLockRotation( int _viewer );

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

/** @} */

//===========================================================================
/** @name GUI Control
* @{ */
//===========================================================================

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

  /// Which view mode is currently selected?
  DLLEXPORT
  QString currentViewMode( );

  /// Which view mode is currently selected?
  DLLEXPORT
  void currentViewMode( QString _mode );

  /// Titel of the main window
  DLLEXPORT
  QString windowTitle( );

  /// Set title of the main window
  DLLEXPORT
  void windowTitle( QString _titel );

  /// Default color for objects
  DLLEXPORT
  void defaultColor( QColor _color );

  /// Default color for objects
  DLLEXPORT
  QColor defaultColor();

  /// use randomized color as default for objects
  DLLEXPORT
  void randomDefaultColor(bool _random);

  /// use randomized color as default for objects
  DLLEXPORT
  bool randomDefaultColor();

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
  /** @name Update blocking
   *
   * Used to speedup system when creating/removing a lot of objects.
   * This disables the analysis of the scenegraph used for rendering.
  * @{ */
  //===========================================================================

  /// Block updates
  DLLEXPORT
  void blockSceneGraphUpdates( );

  /// unblockUpdates
  DLLEXPORT
  void unblockSceneGraphUpdates();

  /// Updates currently blocked?
  DLLEXPORT
  bool sceneGraphUpdatesBlocked( );

  /** @} */

//===========================================================================
/** @name Settings
* @{ */
//===========================================================================

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

/// Get the remote control port
DLLEXPORT
int remoteControlPort();

/// Set the remote control port
DLLEXPORT
void remoteControlPort(int _remotePort);

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
/** @name Version information
 * @{ */
//===========================================================================

/// Version of OpenFlipper Core
DLLEXPORT
QString coreVersion();

/// Information about the used compiler
DLLEXPORT
QString compilerInfo();

/** @} */


/// Internal function called by the core to connect to the program options
DLLEXPORT 
bool initializeSettings();

/// Internal function called by the core to write the program options on exit
DLLEXPORT 
void closeSettings();

}
}

//=============================================================================
#endif // OPTIONS_HH defined
//=============================================================================
