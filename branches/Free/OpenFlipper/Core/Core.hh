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
//  CLASS Core
//
//=============================================================================

/**
 * \file Core.hh
 * This File contains the header of the frameworks system core
*/

#ifndef MVIEWWIDGET_HH
#define MVIEWWIDGET_HH


//== INCLUDES =================================================================


#include "OpenFlipper/common/Types.hh"

#include <ACG/QtWidgets/QtExaminerViewer.hh>

// QT INCLUDES
#include <qvariant.h>
#include <qmainwindow.h>
#include <QToolBox>
#include <QStackedWidget>
#include <QSplashScreen>

#include <QDockWidget>
#include <QVector>

#include <QtScript/QScriptEngine>
#include <QtScript/QtScript>

#include "OpenFlipper/INIFile/INIFile.hh"

#include "OpenFlipper/Logging/Logging.hh"
#include "OpenFlipper/Scripting/Scripting.hh"

// Prototypes for scripting
#include "OpenFlipper/Scripting/scriptPrototypes/prototypeVec3d.hh"
#include "OpenFlipper/Scripting/scriptPrototypes/prototypeMatrix4x4.hh"
#include <OpenFlipper/Scripting/scriptWrappers/vec3dWrapper.hh>
#include <OpenFlipper/Scripting/scriptWrappers/matrix4x4Wrapper.hh>
// #include <OpenFlipper/Scripting/scriptWrappers/ObjectIdWrapper.hh>

// Required Interface definition ( Some variables were defined there )
#include "OpenFlipper/BasePlugin/BaseInterface.hh"
#include "OpenFlipper/BasePlugin/FileInterface.hh"
#include "OpenFlipper/BasePlugin/MenuInterface.hh"
#include "OpenFlipper/BasePlugin/ContextMenuInterface.hh"

#include <OpenFlipper/widgets/coreWidget/CoreWidget.hh>

#include <OpenFlipper/common/GlobalOptions.hh>

#include <OpenFlipper/Core/PluginInfo.hh>

//== CLASS DEFINITION =========================================================

struct fileTypes {
  DataType type;
  QString loadFilters;
  QString saveFilters;
  FileInterface* plugin;
};

/** Core Widget of the Application
 */
class Core : public QObject
{
  Q_OBJECT

public:

  /// constructor
  Core();

  void init();

  /// destructor
  ~Core();

  /// Synchronise two viewers
  bool add_sync_host(const QString& _name);


private slots:

  /// Handle Mouse events when in Identifier mode
  void slotMouseEventIdentify( QMouseEvent* _event );

protected:

    //===========================================================================
    /** @name Signals send to Plugins
    * @{ */
   //===========================================================================
signals:
   /// Signal send to plugins when whole scene is cleared
   void allCleared();

   /// When this Signal is emitted all Plugins are informed that the object list changed
   void ObjectListUpdated(int);

   /// When this Signal is emitted when a Wheel Event occures
   void PluginWheelEvent(QWheelEvent * , const std::string & );

   /// When this Signal is emitted when a Mouse Event occures
   void PluginMouseEvent(QMouseEvent* );

   /// Emitted when an identify event occurs
   void PluginMouseEventIdentify( QMouseEvent* );

   /// This signal is emitted if the active object has been changed
   void activeObjectChanged();

   /// The texture with the given name and filename has been added
   void addTexture( QString, QString , uint );

   /// Tell the plugins to update the given texture
   void updateTexture( QString , int );

   /// Update all textures in the plugins
   void updateAllTextures( );

   /// This Signal is send to the plugins if a texture has been updated
   void updatedTextures( QString , int );

   /// A texture mode should be changed
   void setTextureMode(QString _textureName ,QString _mode);

   /// Switch Texture Plugins to a given Mode
   void switchTexture( QString );

   /// If an ini File is opened, this signal is send to Plugins capable of handling ini files
   void iniLoad( INIFile&, int );

   /// This signal is used to tell the plugins to save the data of _id to the given file
   void iniSave( INIFile& _ini ,int _id );

   /// This signal is used to tell the plugins to save their current status
   void iniSaveOptions(  INIFile& _ini );

   /// This signal is used to tell the plugins to load their new status
   void iniLoadOptions(  INIFile& _ini );

   /// Tell plugins to create a backup
   void createBackup( int _id , QString _name , int nextBackupId_);

   /// Tell the plugins that a file has been opened ( -> Database)
   void openedFile( int _id );

   /// Called after all plugins are loaded
   void pluginsInitialized();

   /** @} */

   //===========================================================================
    /** @name Slots called by the plugins
    * @{ */
   //===========================================================================
   private slots :

      /** Called by the plugins if they changed something in the object list (deleted, added,...)
       * @param _identifier Id of the object
       */
      void slotObjectsListUpdated(int _identifier);

      /// Called by Plugins if they changed the active object
      void slotActiveObjectChanged();

      /// Add a new picking mode to the examiner_widget_
      void slotAddPickMode( const std::string _mode );

      /// Add a new and invisible picking mode to the examiner_widget_
      void slotAddHiddenPickMode( const std::string _mode );

      /// Add a new and visible picking mode to the examiner_widget_ with its own cursor
      void slotAddPickMode( const std::string _mode , QCursor _cursor);

      /// Add a new and invisible picking mode to the examiner_widget_ with its own cursor
      void slotAddHiddenPickMode( const std::string _mode , QCursor _cursor);

      /// Called by a plugin if it creates a texture
      void slotAddTexture( QString _textureName , QString _filename , uint _dimension );

      /// Tell the plugins to update the given texture
      void slotUpdateTexture( QString _name , int _identifier);

      /// Update all textures in the plugins
      void slotUpdateAllTextures( );

      /// A Texture has been updated
      void slotTextureUpdated( QString _textureName , int _identifier );

      /// A texture mode should be changed
      void slotSetTextureMode(QString _textureName ,QString _mode);

      /// Tells Plugins to switch to the given Texture
      void slotSwitchTexture( QString _textureName );

      /// Called if a backup is requested by the plugins
      void backupRequest( int _id , QString _name );

      /// A plugin wants to load a given file
      void slotLoad(QString _filename, DataType _type, int& _id);

      /// A plugin wants to save an object;
      void slotSave(int _id , QString _filename);

      /// Called when a file has been opened
      void slotObjectOpened ( int _id );

      /// Called when an empty object has been Added
      void slotEmptyObjectAdded ( int _id );

      /// Called when a plugin requests a list of file-filters
      void slotGetAllFilters ( QStringList& _list);

   /** @} */

    //===========================================================================
    /** @name scriptable slots
    * @{ */
   //===========================================================================
public slots:

    /// Called when a  plugin requests an update in the viewer
    void updateView();

    /// Clear all data objects
    void clearAll();

    /// exit the current application
    void exitApplication();

    /// translate Scene
    void translate( Vector _vec );

    /// Rotate Scene
    void rotate( Vector _axis, double _angle, Vector _center );

    /// Set the viewing direction
    void setViewingDirection( Vector _direction, Vector _upvector );

    /// toggle fullscreen mode
    void fullscreen();

    /// Hide or show logging window
    void logger();

    /// Hide or show toolbox window
    void toolbox();

    /// Set the drawMode ( ; separated list )
    void setDrawMode(QString _mode);

    /** @} */

   //===========================================================================
    /** @name Basic functions
    * @{ */
   //===========================================================================
private:
  /** Load a new Object ( this may also be an ini file )
   *  @param _filename Filename of the data
   *  @return id of object otherwise -1
   */
  int loadObject ( QString _filename );

  /// Do a reset of the scenegraph (bounding box update,...)
  void resetScenegraph();

 public :

  /// Load an object from the commandline on application start
  void commandLineOpen(const char* _filename, bool asPolyMesh );

  /// Execute a script from the command Line
  void commandLineScript(const char* _filename );

  /** Add an empty Object of the given Type
  * @return id of the new object or -1 if unsupported
  */
  int addEmptyObject( DataType _type );

  /** Load object of the given Type with given filename
   * @return id of the new object or -1 if unsupported
   */
  int loadObject( DataType _type, QString _filename);

  /** Save object with given id
   * @param _id id of the object
   */
  bool saveObject( int _id, QString _filename );

  /** Show dialog for saving an object to a new location
   * @param _id id of the object
   * @param _filename current filename of the object
   */
  bool saveObjectTo( int _id, QString _filename );

  /** @} */

  //===========================================================================
    /** @name Basic slots
    * @{ */
  //===========================================================================
   private slots :

      /// Gets called by examiner widget when mouse is moved in picking mode
      void slotMouseEvent( QMouseEvent* _event );

      /// Gets called by examiner widget when Wheel is moved in picking mode
      void slotWheelEvent( QWheelEvent * _event, const std::string & _mode);

      /// The current Toolbox item has changed
//       void slotToolboxSwitched(int _index);

      /// Slot adding empty object from Menu
      void slotAddEmptyObjectMenu();

      /// Slot adding empty object from Menu of a given type
      void slotAddEmptyObject( DataType _type , int& _id );

      /// Slot for saving objects from Menu
      void slotSaveMenu();

      /// Slot for saving objects to a new location
      void slotSaveToMenu();

      /// Load status from ini file
      void slotLoadIniMenu();

      /// Save current status to ini file
      void slotSaveIniMenu();

      /// Exit Application
      void slotExit();

      /// Open Load Widget
      void slotLoadMenu();

      /// Open Recent file
      void slotRecentOpen(QAction* _action);

   private :

   //===========================================================================
    /** @name Scenegraph
    * @{ */
   //===========================================================================

   private:
      /// Scenegraphs root node
      SeparatorNode* root_node_scenegraph_;

      /// Data root node
      SeparatorNode*  root_node_;

   /** @} */

   //===========================================================================
    /** @name IniFile and Options Handling
    * @{ */
   //===========================================================================

   /// Get all ini files and set basic paths and options
   void setupOptions();

   /// Get and set Application options from ini file
   void readApplicationOptions(INIFile& _ini);

   /// Write Application options to ini file
   void writeApplicationOptions(INIFile& _ini);

  public slots:

   /// Save the current options to the standard ini file
   void saveOptions();

   /// after ini-files have been loaded and core is up or if options have been changed -> apply Options
   void applyOptions();

   /// Open an ini file and load everything in it ( Application/File Options )
   void openIniFile(QString _filename);

   /// Open an obj file and load everything in it ( Application/File Options )
   void openObjFile(QString _filename);

   /// Preprocesses obj file to find out which data type it stores
   void preprocessObjFile(QString _filename);

  private:

   /** \brief Write current status to ini file (Application and File Options)
    *
    *  Writes the complete status to an ini file ( All open objects and their Information )
    */
   void writeIniFile(QString _filename, bool _relativePaths);

   /** \brief Write current status to obj file (Application and File Options)
    *
    *  Writes the complete status to an obj file ( All open objects and their Information )
    */
   void writeObjFile(QString _filename, bool _relativePaths);

   /// Called if app is closed and writes all information to ini file
   void writeOnExit();

   /** @} */

  //===========================================================================
  /** @name Logging
  * @{ */
  //===========================================================================

  signals:
    /// Logg with OUT,WARN or ERR as type
    void log(Logtype _type , QString _message );

    /// Default logging as OUT
    void log(QString _message );

    /// Logging signal for ScriptEngine
    void scriptLog(QString _message);

    /// This signal is emitted to send log data to a plugin
    void externalLog(Logtype _type , QString _message);

  private slots:

    /// Console logger
    void slotLog(Logtype _type, QString _message);

    /// log to file
    void slotLogToFile(Logtype _type, QString _message);

  private:
    ///stream for logging to file
    QTextStream* logStream_;

    ///logfile
    QFile* logFile_;

  public :
    void scriptLogFunction( QString _output);

  /** @} */


  //===========================================================================
  /** @name Signal and Slot processing for Plugins
  * @{ */
  //===========================================================================

  private :
    /// Check if a plugin has a slot
    bool checkSlot(QObject* _plugin , const char* _slotSignature);

    /// Check if a plugin has a signal
    bool checkSignal(QObject* _plugin , const char* _signalSignature);

  /** @} */

  //===========================================================================
    /** @name Plugin Management
      * @{ */
  //===========================================================================

  private:
    /// List of all loaded plugins
    std::vector<PluginInfo> plugins;

    /// List of Plugins that should not be loaded
    QStringList dontLoadPlugins_;

    /// Index of Plugins toolbox widget
    int toolboxindex_;

    /// Function for loading Plugins
    void loadPlugin(QString filename, bool silent);

    /// Load all plugins from default plugin directory and from INI-File
    void loadPlugins();
  private slots:
    /// Unload Plugins from menu
    void slotUnloadPlugin();

    /// Function for unloading Plugins
    void unloadPlugin(QString name);

    /// Dont Load the given _plugins on next startup
    void dontLoadPlugins(QStringList _plugins);

    /// Load Plugins from menu
    void slotLoadPlugin();

  /** @} */


  //===========================================================================
    /** @name Scripting support
      * @{ */
  //===========================================================================

  signals:

    void scriptInfo( QString _pluginName , QString _functionName  );

    void executeScript( QString _script );

    void executeFileScript( QString _filename );

  private :
    /// Core scripting engine
    QScriptEngine scriptEngine_;

    /// Wrappers for plugin scripting
    std::vector<ScriptingWrapper*> scriptingWrappers_;

    /// List of all registered scripting functions
    QStringList scriptingFunctions_;

    /// Prototype for the Vector type
    prototypeVec3d vec3dPrototype_;

    /// Prototype for the Vector type
    prototypeMatrix4x4 matrix4x4Prototype_;


  private slots:
    void slotScriptInfo( QString _pluginName , QString _functionName  );

    void slotExecuteScript( QString _script );

    void slotGetScriptingEngine( QScriptEngine*& _engine  );

    void slotGetAllAvailableFunctions( QStringList& _functions  );

  /** @} */

  //===========================================================================
    /** @name RPC ( Remote procedure Call) support
      * @{ */
  //===========================================================================
  private slots:
    void slotPluginExists( QString _pluginName , bool& _exists );


    void slotFunctionExists( QString _pluginName , QString _functionName , bool& _exists  );

    /** This slot executes a function of a plugin.
     * If it does not exist, _success will be false
     * @param _pluginName Plugin where the function is implemented
     * @param _functionName name of the function ( Standard name, no parameters allowed!)
     * @param _success successfull execution?
     */
    void slotCall( QString _pluginName , QString _functionName , bool& _success  );

    /** This slot executes the given expression.
     * No checks are performed if the remote function is available.
     * @param _expression command to execute
     * @param _success successfull execution?
     */
    void slotCall( QString _expression , bool& _success  );

    /** This slot executes the given expression and returns the result
     *  No checks are performed if the remote function is available.
     *  if an error occurs the result contains an invalid QVariant
     *
     * ex. getValue("databaseplugin.tableExists(\"meshes\")", result) )
     *     result.toBool() == true
     *
     * @param _expression command to execute
     * @param _result The return value of the command
     */
    void slotGetValue(QString _expression, QVariant& _result );

  /** @} */

  private :

  /// Id for the next backup
  int nextBackupId_;

  /// Standard draw mode
  unsigned int standard_draw_mode_;

  /// Use white or a random color for new objects
  bool set_random_base_color_;

  /// Logger interfaces between plugins and core logger
  std::vector<PluginLogger*> loggers_;

  /// File-Plugins
  std::vector<fileTypes> supportedTypes_;

  /// addEmpty Widget
//   addEmptyWidget* addEmptyWidget_;

  private :
    ///Pointer to the data rootNode;
    BaseObject* objectRoot_;

    /// The main applications widget ( only created in gui mode )
    CoreWidget* coreWidget_;

    /// SplashScreen, only used in gui mode
    QSplashScreen* splash_;

    /// Last Splash message;
    QString splashMessage_;

    /// List of available draw modes
    QVector<ViewMode*> viewModes_;

    /// If enabled, this timer will block screen refresh if done more then 30 times per second.
    QTimer* redrawTimer_;
};

/// Special print function for core logger
QScriptValue myPrintFunction(QScriptContext *context, QScriptEngine *engine);

//=============================================================================
#endif // MVIEWWIDGET_HH defined
//=============================================================================

