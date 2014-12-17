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
*   $LastChangedBy$                                                 *
*   $Date$                    *
*                                                                            *
\*===========================================================================*/


//=============================================================================
//
//  CLASS CoreWidget
//
//=============================================================================

/**
 * \file CoreWidget.hh
 * This File contains the header of the frameworks main widget
*/

#ifndef COREWIDGET_HH
#define COREWIDGET_HH


//== INCLUDES =================================================================

#include <set>

#include "OpenFlipper/common/Types.hh"
#include <OpenFlipper/common/GlobalOptions.hh>
#include <OpenFlipper/BasePlugin/ContextMenuInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>
#include <OpenFlipper/BasePlugin/MenuInterface.hh>
#include <OpenFlipper/BasePlugin/ViewInterface.hh>
#include <OpenFlipper/BasePlugin/StatusbarInterface.hh>

#include <OpenFlipper/widgets/coreWidget/SideArea.hh>
#include <OpenFlipper/widgets/coreWidget/ColorStatusBar.hh>

#include <OpenFlipper/widgets/glWidget/QtBaseViewer.hh>
#include <OpenFlipper/widgets/glWidget/QtGLGraphicsScene.hh>
#include <OpenFlipper/widgets/glWidget/QtGLGraphicsView.hh>
#include <OpenFlipper/widgets/glWidget/QtMultiViewLayout.hh>
#include <OpenFlipper/widgets/glWidget/QtSlideWindow.hh>

// QT INCLUDES
#include <QMainWindow>
#include <QStackedWidget>
#include <QTextEdit>
#include <QToolBar>
#include <QStatusBar>

#include <QFuture>
#include <QFutureWatcher>

#include <QDockWidget>

#include <OpenFlipper/widgets/aboutWidget/aboutWidget.hh>
#include <OpenFlipper/widgets/loggerWidget/loggerWidget.hh>
#include <OpenFlipper/widgets/optionsWidget/optionsWidget.hh>
#include <OpenFlipper/widgets/helpWidget/helpWidget.hh>
#include <OpenFlipper/widgets/stereoSettingsWidget/stereoSettingsWidget.hh>
#include <OpenFlipper/widgets/postProcessorWidget/postProcessorWidget.hh>
#include <OpenFlipper/widgets/rendererWidget/rendererWidget.hh>
#include <OpenFlipper/widgets/rendererWidget/rendererObjectWidget.hh>

#include <OpenFlipper/Core/PluginInfo.hh>

#include <ACG/QtWidgets/QtSceneGraphWidget.hh>

#include <ACG/Scenegraph/DrawModes.hh>
#include <ACG/Scenegraph/CoordsysNode.hh>

#include <OpenFlipper/INIFile/INIFile.hh>
#include <OpenFlipper/BasePlugin/PluginFunctions.hh>

/** \brief ViewMode struct
* This struct contains a ViewMode and its status information such as used widgets,toolbars or context menus
*
*/





struct ViewMode{

  /// Name of the View Mode
  QString name;

  /// Icon of the View Mode
  /// TODO Specify size for Icons
  QString icon;

  /// Is this a user defined custom view mode or a plugin generated one
  bool custom;

  /// List of Visible Toolboxes in this view mode
  QStringList visibleToolboxes;

  /// List of Toolbars in this view mode
  QStringList visibleToolbars;

  /// List of context Menus in this view mode
  QStringList visibleContextMenus;
};

class StackWidgetInfo {
  public:
    StackWidgetInfo(bool _editable,QString _name,QWidget* _widget);

    bool editable;
    QString name;
    QWidget* widget;
};

/** Info struct containing information about custom context menus
 */
class MenuInfo {
  public:
    MenuInfo():
      action(0),
      type(CONTEXTBACKGROUNDMENU)
    {}

    /// The context item
    QAction*          action;

    /// Type of objects for which the context Menu should be visible
    DataType        contextType;

    /// Type of the context Menu ( Context for what type .. Background,Object,Node)
    ContextMenuType type;
};

/** \brief Struct containing information about pickModes
 *
 * This is an internal DataStructure! Don't use it. It's used to
 * manage the pickmode information internally.
*/
class PickMode
{
  public:
    /// Constructor
    PickMode(const std::string& _n, const bool _t, const bool _v, QCursor _c, QToolBar *_tb = 0 );

    /// Destructor
    ~PickMode() {};


    /** \brief PickMode visible
     *
     * @param _visible Should this be a visible or a hidden pick mode?
     */
    void visible(const bool _visible);

    /** \brief PickMode visible
     *
     * Visible or hidden pick mode
     */
    bool visible() const;

    /** \brief PickMode Name
    *
    * Name of the pick mode
    */
    std::string name() const;

    /** \brief PickMode mouse tracking
     *
     * If the pick mode has mouse tracking enabled, it will emit
     * mouse events with every movement and not only on button events.
     *
     * @param _tracking mouse tracking enabled or not
     */
    void tracking(const bool _tracking);

    /** \brief PickMode mouse tracking
    *
    * If the pick mode has mouse tracking enabled, it will emit
    * mouse events with every movement and not only on button events.
    */
    bool tracking() const;

    /** \brief PickMode cursor
    *
    * The cursor associated with this pick mode. You can define a non standard
    * cursor that is visible, if this pick mode is active.
    */
    QCursor cursor() const;

    /** \brief PickMode cursor
    *
    * The cursor associated with this pick mode. You can define a non standard
    * cursor that is visible, if this pick mode is active.
    *
    * @param _cursor The cursor to be used in this pickmode
    */
    void cursor(const QCursor _cursor);

    /** \brief PickMode toolbar
    *
    * Each pickmode can have a toolbar that will be visible at the top of the screen.
    * This toolbar is stored here.
    *
    * @param _toolbar The used toolbar
    */
    void toolbar(QToolBar* _toolbar);

    /** \brief PickMode toolbar
    *
    * Each pickmode can have a toolbar that will be visible at the top of the screen.
    * This toolbar is stored here.
    */
    QToolBar* toolbar() const;

  private:

    /// Name of the pickMode
    std::string  name_;

    /** MouseTracking enabled for this mode?
    */
    bool         tracking_;

    /** Defines if the Mode will be visible in the popup Menu
    */
    bool         visible_;

    /** Cursor used in  this pickMode
    */
    QCursor      cursor_;

    /** Additional toolbar showed in this pickMode
    */
    QToolBar *   toolbar_;
};

/** Core Widget of the Application
 */
class CoreWidget : public QMainWindow
{
  Q_OBJECT

public:

  friend class Core;

  /// constructor
  CoreWidget( QVector<ViewMode*>& _viewModes, std::vector<PluginInfo>& _plugins, QList< SlotInfo >& _coreSlots );

  /// destructor
  ~CoreWidget();

  signals:
    void clearAll();
    void loadMenu();
    void addEmptyObjectMenu();
    void saveMenu();
    void saveToMenu();
    void loadIniMenu();
    void saveIniMenu();
    void recentOpen(QAction*);
    void exit();

    void loadPlugin();
    void showPlugins();

  private slots:

    /// Called on applications close
    void closeEvent ( QCloseEvent * event );

   //===========================================================================
  /** @name Logger
  * @{ */
  //===========================================================================

  signals :
      void log(Logtype _type, QString _message);
      void log(QString _message);

   /** @} */

   //===========================================================================
  /** @name Keys
  * @{ */
  //===========================================================================

  protected :

    //! Handle key events
    virtual void keyPressEvent (QKeyEvent* _e);
    virtual void keyReleaseEvent(QKeyEvent* _e);

    // Filter alt key events under windows
    //bool eventFilter(QObject *obj, QEvent *ev);

  private:
    //event handling
    bool event( QEvent *event );

    bool eventFilter(QObject *_obj, QEvent *_event);

  signals :

    /** When this Signal is emitted when a Keyevent Event occures
     *  The signal is connected to all Plugins implementing KeyInterface by the pluginLoader
     */
    void PluginKeyEvent(QKeyEvent* );

    /** When this Signal is emitted when a Keyevent Release occures
     *  The signal is connected to all Plugins implementing KeyInterface by the pluginLoader
     */
    void PluginKeyReleaseEvent(QKeyEvent* );

    /// internal signal to register CoreWidget keys
    void registerKey(int _key, Qt::KeyboardModifiers _modifiers, QString _description,
                     bool _multiUse = false);

    ///call a scripting function
    void call( QString _expression , bool& _success  );

  private:
    ///typedefs
    typedef std::multimap<  std::pair< int, Qt::KeyboardModifiers >  ,  std::pair< QObject*, int > > KeyMap;

    typedef std::pair< KeyMap::iterator, KeyMap::iterator > KeyRange;

    /** \brief Register all events related to the core
     */
    void registerCoreKeys();

    /** \brief handle key events for the core
     *
     * This function is called to handle key events which are registered to the core
     */
    void coreKeyPressEvent();

    /// Handle core key events
    void coreKeyPressEvent  (QKeyEvent* _e);
    void coreKeyReleaseEvent(QKeyEvent* _e);

    KeyBinding getKeyBinding(QObject* _plugin, int _keyIndex );
    QString    getRPCName(QObject* _plugin );

    ///vector of keys registered to the core
    std::vector<KeyBinding> coreKeys_;

    ///mapping of all keys to registered keys and the corresponding plugins
    KeyMap keys_;

    ///mapping of all registered keys and the corresponding plugins to currently assigned keys
    InverseKeyMap invKeys_;

    ///list of scripting slots from core
    QList< SlotInfo >& coreSlots_;

  private slots:
     /** \brief key registration
     *
     * internal slot that registers a keyCombination to the sender() who emitted the registerKey() signal
     * only after registering keyCombinations plugins receive keyEvents to the corresponding keyCombinations via KeyInterface.
     */
    void slotRegisterKey(int _key, Qt::KeyboardModifiers _modifiers, QString _description,
                         bool _multiUse = false);

  public slots:
     /** \brief add a new key Mapping
     *
     * internal function that stores a new keyCombination assignment for an existing keyBinding
     */
    void slotAddKeyMapping(int _key, Qt::KeyboardModifiers _modifiers, QObject* _plugin, int _keyBindingID);

     /** \brief register scripting slots to allow keyBindings
     *
     * internal function that registers all public slots without parameters of each plugin (or core)
     * to the key management system. This allows the assignment of keyBindings for each of these slots.
     */
    void slotRegisterSlotKeyBindings();

  public:
    ///Load key assignments from a given INI file
    void loadKeyBindings(INIFile& _ini);

    ///Store current key assignments to a given INI file
    void saveKeyBindings(INIFile& _ini);

    void showReducedMenuBar(bool reduced);

  private:
    /// Store the state of the shift key
    bool shiftPressed_;

   /** @} */

  //===========================================================================
  /** @name GUI creation
  * @{ */
  //===========================================================================

  public:

    /// Setup the main menubar
    void setupMenuBar();

   /** @} */

  //===========================================================================
  /** @name Video Capturing
  * @{ */
  //===========================================================================

  signals:
    void startVideoCapture(QString _baseName, int _fps, bool _captureViewers);
    void stopVideoCapture();

    void resizeViewers(int _width, int _height);
    void resizeApplication(int _width, int _height);

  private slots:
    void startVideoCaptureDialog();

  /** @} */

  //===========================================================================
  /** @name Recent File Menu handling
   * @{ */
  //===========================================================================

  public:

  /// Add a recent file and update menu
  void addRecent(QString _filename, DataType _type);

  /// Update the recent files menu
  void updateRecent();

  /** @} */

  //===========================================================================
  /** @name GUI Controls
  * @{ */
  //===========================================================================

  public :

    /// Show logger in splitter or not
    void showLoggerInSplitView(bool _show);

    /// Set in-scene logger geometry right
    void updateInSceneLoggerGeometry();

    /// Use native or gl painted cursor
    void setForceNativeCursor ( bool _state );

    QSize defaultIconSize();

    QScrollArea* getToolboxScrollArea() { return toolBoxScroll_; }
    QWidget* getToolboxArea() { return toolBoxArea_; }
    SideArea* getToolbox() { return toolBox_; }

  public slots:

    /// Set application to Fullscreen and back
    void toggleFullscreen();

    /// Enable or disable fullscreen mode
    void setFullscreen(bool _state );

    /// Hide or show logging area
    void toggleLogger();

    /// Change visibility of the logger
    void showLogger(OpenFlipper::Options::LoggerState _state);

    /// Hide or show toolbox area
    void toggleToolbox();
    
    /// Show or hide toolbox
    void showToolbox( bool _state );

    /// Change visibility of the Status Bar
    void toggleStatusBar();

    /// Show or hide status bar
    void showStatusBar( bool _state );

    /// Show or hide menubar
    void showMenuBar( bool _state );

    /// Hide or show menu bar
    void toggleMenuBar();

    /// Hide or show current toolbar
    void toggleToolBar();

    /// Show or hide toolbar, emits toolBarToggled( bool _state )
    void showToolBar( bool _state );
  signals:
    /// will be emitted if the visibility of the toolbar is changed
    void toolBarVisChanged( bool _state );

    /// will be emitted if the visibility of the toolbox is changed
    void toolBoxVisChanged( bool _state );

    /// will be emitted if the visibility of the statusbar is changed
    void statusBarVisChanged( bool _state );

    /// will be emitted if the visibility of the menubar is changed
    void menuBarVisChanged( bool _state );

    /// will be emitted if the fullscreen state is changed (_state = true => in fullscreen)
    void fullScreenChanged( bool _state );

  private:
    OpenFlipper::Options::LoggerState loggerState_;
    
    QSize defaultIconSize_;

  /** @} */

  //===========================================================================
    /** @name Tool Widgets / View Modes
    * @{ */
  //===========================================================================

    private:
      /// a List of all widgets in the toolbar
      QVector<QWidget*> toolWidgets_;

    public:
      /// List of currently available viewModes
      QVector<ViewMode*>& viewModes_;

    private:

      QAction* viewModeButton_;

      /// Submenu holding all ViewMode actions
      QMenu* viewModeMenu_;

    public slots:

      /// init ViewModes that were loaded via ini-file
      void initViewModes(  );

      /// Add or change Toolboxes for a ViewMode (non-userdefined viewMode)
      void slotAddViewModeToolboxes(QString _mode, QStringList _usedWidgets);

      /// Add or change Toolboxes for a ViewMode (_custom == userdefined viewMode)
      void slotAddViewModeToolboxes(QString _mode, bool _custom, QStringList _usedWidgets);

      /// Add or change Toolbars for a ViewMode (non-userdefined viewMode)
      void slotAddViewModeToolbars(QString _mode, QStringList _usedToolbars);

      /// Add or change Toolbars for a ViewMode (_custom == userdefined viewMode)
      void slotAddViewModeToolbars(QString _mode, bool _custom, QStringList _usedToolbars);

      /// Add or change Toolbars for a ViewMode (non-userdefined viewMode)
      void slotAddViewModeContextMenus(QString _mode, QStringList _usedToolbars);

      /// Add or change Toolbars for a ViewMode (_custom == userdefined viewMode)
      void slotAddViewModeContextMenus(QString _mode, bool _custom, QStringList _usedToolbars);

      /// Completly configure a view mode ( set toolbars, toolboxes, context menus, ... )
      void slotAddViewModeComplete(QString _mode , bool _custom, QStringList _toolboxes, QStringList _toolbars, QStringList _contextmenus);

      /// Sets the Icon for a given View Mode (non-userdefined viewMode)
      void slotSetViewModeIcon(QString _mode, QString _iconName);

      /// Sets the Icon for a given View Mode (_custom == userdefined viewMode)
      void slotSetViewModeIcon(QString _mode, bool _custom, QString _iconName);

      /// Slot for Changing visible toolWidgets
      void slotChangeView(QString _mode, QStringList _toolboxWidgets, QStringList _toolbars, QStringList _contextmenus, bool _expandAll = false);
      
      /// Set orientation of tool box (either on the right or the left side of the screen)
      void setToolBoxOrientationOnTheRight(bool _toolBoxRight);
      
      /// Hide or show the View Mode controls
      void showViewModeControls(bool _show);

      /// Hide the context menu
      void slotHideContextMenu();

    private slots:
      /// Remove viewMode
      void slotRemoveViewMode(QString _name);

      /// Slot for setting ViewMode from Menu
      void slotSetViewMode( QAction* action);

      /// Show a dialog in which the viewMode can be edited
      void slotViewModeDialog();

      /// Show a dialog in which the viewMode can be changed
      void slotViewChangeDialog();

      /// update the content of the specified examiner
      void slotUpdateExaminer(unsigned _id);

    public slots :
      
      /// Set the view Mode to the given Mode
      void setViewMode( QString _mode, bool _expandAll = false );
      
      /// Move a specific toolbox widget to the top of the side area
      void moveToolBoxToTop(QString _name);
      
      /// Move a specific toolbox widget to the bottom of the side area
      void moveToolBoxToBottom(QString _name);

   /** @} */



  //===========================================================================
    /** @name Basic Gui Elements
    * @{ */
  //===========================================================================

  private:

    /// Examiner Widget
    std::vector< glViewer* >  examiner_widgets_;

    ///Spliter between toplevel objects and the textedit at the bottom
    QSplitter* splitter_;

    /// Group box containing Task Switcher Controls
    QGroupBox *viewModeControlBox_;
    
    /// Textedit at the bottom for log messages
    LoggerWidget* logWidget_;

    /// Size of the logging window ( defaults to 240 )
    int originalLoggerSize_;

    /// QMenu containing the recently opened files
    QMenu* recentFilesMenu_;

    /// Algorithms Menu
    QMenu* algorithmMenu_;

    /// Help Menu
    QMenu* helpMenu_;
    
    /// Window Menu
    QMenu* windowMenu_;
    
    /// Action for View Mode Widget Conrol in Menu
    QAction* AC_ShowViewModeControls_;
    
    /// gl widget used as drawing area to paint the graphics scene
    QGLWidget* glWidget_;

    /// graphics scene used to paint gl context and widgets
    QtGLGraphicsScene* glScene_;

    /// graphics view that holds the gl scene
    QtGLGraphicsView* glView_;

    /// center widged
    QGraphicsWidget* centerWidget_;

    /// Base layout that holds gl views
    QtMultiViewLayout* baseLayout_;

    /// Class that holds the animated log widget
    QtSlideWindow* slidingLogger_;

    /// Widget for toolBox
    QWidget* toolBoxArea_;

    /// Button to change viewModes
    QPushButton* vmChangeButton_;

    /// Spliter between toplevel objects and toolbox
    QSplitter* toolSplitter_;

    /// Toolbox
    SideArea* toolBox_;

    /// Toolbox scroll area
    QScrollArea* toolBoxScroll_;

    /// Handle to picking toolbar
    typedef std::map<QToolBar*,QGraphicsProxyWidget*> PickToolBarMap;
    PickToolBarMap curPickingToolbarItems_;

    /// Extra toolbar not in scene for picking
    QToolBar* pickToolBarExternal_;

    /// Store all toolbars that once have been registered
    std::set<QToolBar*> registeredToolbars_;

    /// Cursor handling
    CursorPainter* cursorPainter_;


    // widget showing the scenegraph
    ACG::QtWidgets::QtSceneGraphDialog* sceneGraphDialog_;

  /** @} */

  private slots:

    /** Creates and shows the Scenegraph Dialog
     */
    void slotShowSceneGraphDialog();


   /** Makes the sender examiner active
    */
   void slotActivateExaminer ();


   /** Display a log message in the Mainwidget
    * @param _type Type of Message (OUT,WARN,ERR)
    * @param _message The message to be displayed
    */
   void slotLog(Logtype _type, QString _message);

   /** Updates the size of the main graphics widget in the scene
     */
   void sceneRectChanged(const QRectF &rect);

  //===========================================================================
    /** @name Menubar controls
      * @{ */
  //===========================================================================
  private slots :

    void slotAddMenubarAction( QAction* _action , QString _name );
    void slotAddMenubarActions( std::vector<QAction*> _actions , QString _name );
    void slotGetMenubarMenu (QString _name, QMenu *& _menu, bool _create);

  private:

    /// File Menu
    QMenu *fileMenu_;

    /// View Menu
    QMenu *viewMenu_;

    /// Tools Menu
    QMenu *toolsMenu_;

    /// First entry after all relevant parts of the File Menu
    QAction* fileMenuEnd_;

    /// All available menus
    QMap<QString, QMenu *> menus_;

  /** @} */

  //===========================================================================
    /** @name ToolBar controls
      * @{ */
  //===========================================================================
  private:
    QToolBar* viewerToolbar_;

    int toolbarCount_;

    QToolButton* stereoButton_;
    QToolButton* moveButton_;
    QToolButton* pickButton_;
    QToolButton* questionButton_;

    QComboBox* viewerLayoutBox_;

  /** @} */

  //===========================================================================
    /** @name View Menu
      * @{ */
  //===========================================================================

  public slots:
    /// Setup and update the global draw menu
    void slotUpdateGlobalDrawMenu();

    /// Setup and update the global renderer menu
    void slotUpdateRendererMenu();

    /// Change viewer layout that was selected in the combo box
    void setViewerLayout(int _idx);

  private slots:
    /// Switches over to the next view mode
    void nextViewerLayout();

    /// Called when the global drawMode is selected
    void slotGlobalDrawMenu(QAction * _action);

    /// Called before the view Menu is shown
    void slotViewMenuAboutToShow();

    /// Called when the global renderer is selected
    void slotGlobalRendererMenu(QAction * _action);

    /// Called when the global postprocessor is selected
    void slotGlobalPostProcessorMenu(QAction * _action);

  private:
    /// This variable holds the global draw menu
    QMenu* globalDrawMenu_;

    /// This variable holds the global renderer menu
    QMenu* rendererMenu_;

    QActionGroup * drawGroup_;

    /// Group for all renderers
    QActionGroup * rendererGroup_;

    /// Group for all menu items
    QActionGroup* viewGroup_;

    QAction* perspectiveProjectionAction_;
    QAction* orthogonalProjectionAction_;

    /// Action to globally set animation
    QAction* globalAnimationAction_;

    /// Action to globally set backface culling
    QAction* globalBackfaceCullingAction_;

    /// Action to globally set two-sided lighting
    QAction* globalTwosidedLightingAction_;

    /// Action to globally set multisampling
    QAction* globalMultisamplingAction_;
    
    /// Action to globally set mipmapping
    QAction* globalMipmappingAction_;

    ACG::SceneGraph::DrawModes::DrawMode activeDrawModes_;

    ACG::SceneGraph::DrawModes::DrawMode availableGlobalDrawModes_;

  /** @} */

  //===========================================================================
    /** @name Context Menu
     * @{ */
  //===========================================================================

  signals :
    /// tells the plugins to update their context menu when an object is picked
    void updateContextMenu(int);

    /// tells the plugins to update their context menu when a node is picked
    void updateContextMenuNode(int);

    /// tells the plugins to update their context menu when the background is picked
    void updateContextMenuBackground();


  private slots:
    /// This slot is called by the examiner widgets gl area when a context menu is requested
    void slotCustomContextMenu( const QPoint& _point );

    /// called by plugins to add a new context menu item
    void slotAddContextItem(QAction* _entry, ContextMenuType _type);

    /// called by plugins to add a real context menu item depending on DataType
    void slotAddContextItem( QAction* _entry , DataType _dataType ,ContextMenuType type_);
    
    /// called by slotAddContextItem to add the item to the view mode
    void slotAddContextItemToViewMode( QAction* _entry );

    /// Paste the view to the last active examiner
    void slotPasteView( );

    /// Paste the view, the window and toolbox size to the last active examiner
    void slotPasteViewAndWindow( );

    /// Set the supplied serialized view
    void slotSetView( QString view );

    /// Set the supplied serialized view
    void slotSetViewAndWindowGeometry( QString view );

    /// Copy view from the last active examiner
    void slotCopyView( );

    /// Called when a coordsys drawMode has been changed
    void slotViewerDrawMenu( QAction * _action );

    /// Called when a different post processor has been chosen
    void slotPostProcessorMenu( QAction * _action);

    /// Called when a different renderer has been chosen
    void slotRenderMenu( QAction * _action);

    /// Creates a draw Menu for the currently active Viewer
    void slotUpdateViewerDrawMenu();

    /// Create a snapshot of the last active examiner
    void slotExaminerSnapshot();

    /// Show / hide wheels
    void slotSwitchWheels(bool _state);

    /// Switch navigation mode
    void slotSwitchNavigation(bool _egomode);

    /// Set the snapShot name for all examiners
    void slotSnapshotName();

    /// Hide coordinate systems in all viewers
    void slotCoordSysVisibility(bool _visible);

  private :
    /** Update the contextmenu for the given position inside an examiner widget
     *
     * @param _point      Picking position in coordinates of the viewer
     */
    void updatePopupMenu(const QPoint& _point);

    void updatePopupMenuCoordsysNode(QMenu* _menu , const int _part);
    void updatePopupMenuObject(QMenu* _menu , BaseObjectData* _object );
    void updatePopupMenuBackground(QMenu* _menu , const QPoint& _point);
    void updatePopupMenuNode(QMenu* _menu , ACG::SceneGraph::BaseNode* _node);

    bool addContextMenus( QMenu* _menu , ContextMenuType _type, int _id = -1);

  private :
    /// context Menu for the gl area
    QMenu*  contextMenu_;

    /// Context Menu containing all selection elements
    QMenu*  contextSelectionMenu_;

    /// All real context menu entries
    std::vector< MenuInfo > contextMenus_;

    /// DrawGroup for per Viewer Draw Modes
    QActionGroup* drawGroupViewer_;

    /// Draw Menu for per Viewer Draw Modes
    QMenu* viewerDrawMenu_;

    /// owns all the checkboxes of viewerDrawMenu_
    QWidget* viewerDrawMenuWidget_;

    /// Menu which holds the context menu entries for
    /// the coordsys node if invisible
    QMenu* coordSysMenu_;

  /** @} */

  //===========================================================================
    /** @name Snapshots
     * @{ */
  //===========================================================================

  private:

    QString snapshotName_;
    int     snapshotCounter_;

  public slots:

    /// Create a snapshot of the whole app with fileDialog
    void viewerSnapshotDialog();

    /// Create a snapshot of the whole app
    void viewerSnapshot();

    /// Create a snapshot of the whole app with fileDialog
    void applicationSnapshotDialog();

    /// Create a snapshot of the whole app
    void applicationSnapshot();

    /// Set the snapshot name
    void applicationSnapshotName(QString _name);


  /** @} */

  //===========================================================================
    /** @name Thread-safe functions using QFuture
    * @{ */
  //===========================================================================

  public:

    void writeImageAsynchronously(QImage* _image, const QString _name);

  private:

    // Store pointers to QFuture and QFutureWatcher
    std::map<QFutureWatcher<void>*,QFuture<void>*> watcher_garbage_;

    // Mutex for operations on map
    QMutex map_mutex_;

  private slots:

    void delete_garbage();

  /** @} */

  //===========================================================================
    /** @name StackWidget controls
    * @{ */
  //===========================================================================

  private slots:
    void slotGetStackWidget( QString _name, QWidget*& _widget );
    void slotAddStackWidget( QString _name, QWidget* _widget );
    void slotUpdateStackWidget( QString _name, QWidget* _widget );

    void slotViewMenuAction( QAction * _action);

    void slotUpdateStackMenu();


  private :
    /// QMenu containing the recently opened files
    QMenu* stackMenu_;

    std::vector< StackWidgetInfo > stackWidgetList_;

  public :
    /// Container widget for holding multiple views
    QStackedWidget* stackedWidget_;

  /** @} */

  //===========================================================================
  /** @name Help System
  * @{ */
  //===========================================================================

  private:
    static const QString homePage_;

  public slots:
    /// Display the help Browser
    void showHelpBrowser(const QString &page = homePage_);

  signals:
   void changeHelpSite(QUrl);

  private:
    /// Pointer to the help Browser
    HelpWidget* helpWidget_;

  /** @} */

  //===========================================================================
  /** @name PostProcessor dialog
   * @{ */
  //===========================================================================


  public slots:

    void slotShowPostProcessorManager();

  private:
    PostProcessorDialog* postProcessorDialog_;

  /** @} */

  //===========================================================================
  /** @name Renderer dialog
   * @{ */
    //===========================================================================


    public slots:

      /// shows the widget for the rendermanager
      void slotShowRenderManager();

      /// Shows the widget containing the current render objects
      void slotShowRenderObjectWidget();

    private:
      RendererDialog*       rendererDialog_;
      RendererObjectWidget* rendererObjectWidget_;

    /** @} */

  //===========================================================================
  /** @name Stereo View Mode
  * @{ */
  //===========================================================================

  public slots:
      /// Creates custom context menu for stereo viewer settings
    void stereoButtonContextMenu(const QPoint& _pos);

    /// Is called if the user has changed stereo settings
    /// Note: The parameter just makes sure, we can connect
    /// all necessary signals to this slot. It has no use for now.
    void slotApplyStereoSettings(int _tmpParam = 0);


  private:
    /// Widget to change stereo settings
    StereoSettingsWidget* stereoSettingsWidget_;

  /** @} */

  //===========================================================================
  /** @name About System
  * @{ */
  //===========================================================================

  public slots:
    /// Display the about widget
    void showAboutWidget();

    /** \brief Adds an entry to the about dialog
     *
     * @param _text     Content of the tab
     * @param _tabName  Header of the tab
     */
    void addAboutInfo(QString _text, QString _tabName );

  private:
    /** \brief Add license information about core parts
     *
     * Adds the license information to the about dialog for used libraries.
     */
    void addCoreLicenseInfo();

  private:
    /// Pointer to the about widget
    AboutWidget* aboutWidget_;

    /// Additional tab information, that could be provided by plugins
    QMap<QString,QString> additionalAboutTabs_;

  /** @} */

  //===========================================================================
  /** @name Options System
  * @{ */
  //===========================================================================

  public slots:
    /// Display the Options Browser
    void showOptionsWidget();

  signals:
    void applyOptions();
    void saveOptions();

  private:

    /// Pointer to the OptionsWidget
    OptionsWidget* optionsWidget_;

  /** @} */

  //===========================================================================
  /** @name ToolBar controls
  * @{ */
  //===========================================================================
  private slots:
    /// Called by Plugins to add a Toolbar
    void slotAddToolbar(QToolBar* _toolbar);

    /// Called by Plugins to remove a Toolbar
    void slotRemoveToolbar(QToolBar* _toolbar);

    /// Called by Plugins to get access to specific Toolbars by name
    void getToolBar( QString _name, QToolBar*& _toolbar);

  private :
    QToolBar* mainToolbar_;

    std::vector< QToolBar* > toolbars_;

  /** @} */

  //===========================================================================
  /** @name StatusBar controls
   * @{ */
  //===========================================================================
  public slots:
    void statusMessage(QString _message, int _timeout = 0);

    void clearStatusMessage();

    void setStatus( ApplicationStatus::applicationStatus _status);

    void addWidgetToStatusbar(QWidget* _widget);
    
  private:
    void setupStatusBar();

  private :
    ColorStatusBar* statusBar_;

    QLabel* statusIcon_;

  /** @} */

  //===========================================================================
  /** @name Drag and Drop
   * @{ */
  //===========================================================================
  
  signals:
    void dragOpenFile(QString _filename);
  
  private slots:
    void startDrag( QMouseEvent* _event );

    void dragEnterEvent(QDragEnterEvent* _event);

    void dropEvent( QDropEvent* _event );
    
  /** @} */

  private :

  std::vector<PluginInfo>& plugins_;


  //===========================================================================
  /** @name Viewer Controls
   * @{ */
  //===========================================================================

  private slots:

    /// Enable or disable Stereo
    void slotToggleStereoMode();

    /// Set Background Color for all viewers at once.
    void slotSetGlobalBackgroundColor();

    /// Set Background Color for one viewer.
    void slotSetContextBackgroundColor();



    /// Set the active viewer to home position
    void slotContextHomeView();

    /// Set the viewer to home position
    void slotGlobalHomeView();



    /// Set the active viewers home position
    void slotContextSetHomeView();

    /// Set the home position for all viewers
    void slotGlobalSetHomeView();



    /// Change view on active viewer to view complete scene
    void slotContextViewAll();

    /// Change view on all viewers to view complete scene
    void slotGlobalViewAll();


    /// Toggle projection mode of the active viewer.
    void slotContextSwitchProjection();
    
    /// Toggle coordsys projection mode of the active viewer.
    ACG::SceneGraph::CoordsysNode::ProjectionMode getCoordsysProjection();
    
    /// Toggle coordsys projection mode of the active viewer.
    void slotContextSwitchCoordsysProjection();

    /// Toggle projection mode of all viewers to perspective projection
    void slotGlobalPerspectiveProjection();

    /// Toggle projection mode of all viewers to orthographic projection
    void slotGlobalOrthographicProjection();


    /// Change the viewing direction from context-menu
    void slotSetViewingDirection(QAction* _action);
    
    /// Lock rotation in current examiner widget
    void slotLockRotation(bool _lock);

    
    
    /// If animation is disabled in all viewers, enable it in all viewers. Otherwise disable it.
    void slotGlobalToggleAnimation();
    
    /// Set the animation mode for all viewers
    void slotGlobalChangeAnimation(bool _animation);

    /// Set the animation mode for active viewer
    void slotLocalChangeAnimation(bool _animation);


    
    /// If backface culling is disabled in all viewers, enable it in all viewers. Otherwise disable it.
    void slotGlobalToggleBackFaceCulling();

    /// Set backface culling for all viewers
    void slotGlobalChangeBackFaceCulling(bool _backFaceCulling);

    /// Set backface culling for active viewer
    void slotLocalChangeBackFaceCulling(bool _backFaceCulling);



    /// If two-sided lighting is disabled in all viewers, enable it in all viewers. Otherwise disable it.
    void slotGlobalToggleTwoSidedLighting();
    
    /// Set two-sided lighting for all viewers
    void slotGlobalChangeTwoSidedLighting(bool _lighting);

    /// Set two-sided lighting for active viewer
    void slotLocalChangeTwoSidedLighting(bool _lighting);



    /// If multisampling is disabled in all viewers, enable it in all viewers. Otherwise disable it.
    void slotGlobalToggleMultisampling();
    
    /// Set multisampling for all viewers
    void slotGlobalChangeMultisampling(bool _multisampling);

    /// Set multisampling for active viewer
    void slotLocalChangeMultisampling(bool _multisampling);
    
    
    /// If mipmapping is disabled in all viewers, enable it in all viewers. Otherwise disable it.
    void slotGlobalToggleMipmapping();
    
    /// Set mipmapping for all viewers
    void slotGlobalChangeMipmapping(bool _multisampling);
    
    /// Set mipmapping for active viewer
    void slotLocalChangeMipmapping(bool _mipmapping);
    

    /// When using first person mode move backward
    void moveBack();

    /// When using first person mode move forward
    void moveForward();

    /// When using first person mode strafe to the left
    void strafeLeft();

    /// When using first person mode strafe to the right
    void strafeRight();

  signals :
    /** \brief The viewer with id _viewerId changed its draw Mode
     *
     */
    void drawModeChanged(int _viewerId);

  private :
    bool stereoActive_;

  /** @} */

  //===========================================================================
  /** @name Action Mode States
   * @{ */
  //===========================================================================

  public slots:

    bool examineMode() { return(actionMode_ == Viewer::ExamineMode  ); };
    bool pickingMode() { return(actionMode_ == Viewer::PickingMode  ); };
    bool lightMode()   { return(actionMode_ == Viewer::LightMode    ); };
    bool questionMode(){ return(actionMode_ == Viewer::QuestionMode ); };

    /** Set action mode.
      The ActionMode determines the type of reaction on mouse events.
    */
    void setExamineMode() { setActionMode(Viewer::ExamineMode  ); };
    void setPickingMode() { setActionMode(Viewer::PickingMode  ); };
    void setQuestionMode(){ setActionMode(Viewer::QuestionMode ); };

    void setActionMode(const Viewer::ActionMode  _am);
    void getActionMode(Viewer::ActionMode& _am);

  public:
    Viewer::ActionMode actionMode() { return actionMode_; };
    Viewer::ActionMode lastActionMode() { return lastActionMode_; };

  private :
    Viewer::ActionMode actionMode_, lastActionMode_;

  /** @} */

  //===========================================================================
  /** @name PickModes
   * @{ */
  //===========================================================================

  public:

    /** \brief  add pick mode
     *
     *  @param _name           Name of the Pick Mode or "Separator" to insert a separator
     *  @param _mouse_tracking true: every mouse movement will emit mouse events not only when mousebutton is pressed
     *  @param _pos            position to insert the mode in the popup menu.
     *  @param _visible        If true, the ode will be visible in the ui, otherwise it is a hidden pick mode
     *  @param _cursor         The cursor that should be used when the given pickmode is active
     */
    void addPickMode(const std::string& _name,
                     bool               _mouse_tracking = false,
                     int                _pos            = -1,
                     bool               _visible        = true,
                     QCursor            _cursor         = Qt::ArrowCursor );

    /** clear all pick modes
     */
    void clearPickModes();

    /** return the currently active pick mode
     */
    const std::string& pickMode() const;

    /** Switch to given picking mode
     * @param _id Id of the picking Mode
     */
    void pickMode( int _id );

    void expandToolBoxWidget(QWidget *widget, bool expand);

  public slots:

    /** Switch to given picking mode
     * @param _name Name of the picking mode
     */
    void setPickMode(const std::string&  _name);


    void getPickMode(std::string& _name);

    /// Set toolbar to be active pick toolbar
    void setActivePickToolBar(QToolBar* _tool);

    /// Hide picking toolbar
    void hidePickToolBar();

  public slots:

    /** \brief  set a new cursor for the pick mode
     *
     *  @param _name Name of the Pick Mode
     *  @param _cursor the new cursor
     */
    void setPickModeCursor(const std::string& _name, QCursor _cursor);

    /** \brief  set mouseTracking for the pick mode
     *
     *  @param _name Name of the Pick Mode
     *  @param _mouseTracking true: every mouse movement will emit mouse events not only when mousebutton is pressed
     */
    void setPickModeMouseTracking(const std::string& _name, bool _mouseTracking);

    /** \brief Set the additional toolbar of the given PickMode
     *
     * Set the additional toolbar that should be shown in the pickMode
     *
     * @param _mode Identifier of Picking mode
     * @param _toolbar the toolbar
     */
    void setPickModeToolbar( const std::string& _mode , QToolBar * _toolbar );

    /** \brief Removes the additional toolbar of the given PickMode
     *
     * Set the additional toolbar that should be shown in the pickMode
     *
     * @param _mode Identifier of Picking mode
     */
    void removePickModeToolbar( const std::string& _mode );

    void actionPickMenu( QAction * _action );

  signals:
    /** This signal is emitted when the pickMode is changed and contains the new PickMode
     */
    void signalPickModeChanged(const std::string&);

  private:

    QMenu* pickMenu_;

    /** Vector of all Picking modes
     */
    std::vector<PickMode>  pick_modes_;

    /** Name of current pickMode
     */
    std::string            pick_mode_name_;

    /** Index of current pickMode
     */
    int                    pick_mode_idx_;

    /**
     * Actions that are hidden in reduced mode.
     */
    std::vector<QAction*> extended_actions;


    /// update pick mode menu
    void updatePickMenu();

  private slots:

    void hidePopupMenus();
  /** @} */
};


//=============================================================================
#endif // COREWIDGET_HH defined
//=============================================================================

