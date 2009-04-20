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

#include "OpenFlipper/common/Types.hh"
#include <OpenFlipper/common/GlobalOptions.hh>
#include <OpenFlipper/BasePlugin/ContextMenuInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>
#include <OpenFlipper/BasePlugin/MenuInterface.hh>
#include <OpenFlipper/BasePlugin/ViewInterface.hh>
#include <OpenFlipper/BasePlugin/StatusbarInterface.hh>

#include <OpenFlipper/widgets/coreWidget/SideArea.hh>

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

#include <QDockWidget>

#include <OpenFlipper/widgets/aboutWidget/aboutWidget.hh>
#include <OpenFlipper/widgets/loggerWidget/loggerWidget.hh>
#include <OpenFlipper/widgets/optionsWidget/optionsWidget.hh>
#include <OpenFlipper/widgets/helpWidget/helpWidget.hh>

#include <OpenFlipper/Core/PluginInfo.hh>

#include <ACG/QtWidgets/QtSceneGraphWidget.hh>

#include <OpenFlipper/INIFile/INIFile.hh>
#include <OpenFlipper/BasePlugin/PluginFunctions.hh>


struct ViewMode{
      QString name;
      bool custom; //is the mode defined by the user?
      QStringList visibleWidgets;
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
struct MenuInfo {
  /// The context item
  QAction*          action;

  /// Type of objects for which the context Menu should be visible
  DataType        contextType;

  /// Type of the context Menu ( Context for what type .. Background,Object,Node)
  ContextMenuType type;
};

/** Core Widget of the Application
 */
class CoreWidget : public QMainWindow
{
  Q_OBJECT

public:

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

    typedef std::multimap< std::pair< QObject*, int >, std::pair< int, Qt::KeyboardModifiers > > InverseKeyMap;

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

  public:
    QAction* startVideoCaptureAction_;
    QAction* stopVideoCaptureAction_;

  /** @} */

  //===========================================================================
  /** @name Recent File Menu handling
   * @{ */
  //===========================================================================

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

  private:
    OpenFlipper::Options::LoggerState loggerState_;


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

      QPushButton* viewModeButton_;

      /// Submenu holding all ViewMode actions
      QMenu* viewModeMenu_;

    public slots:

      /// init ViewModes that were loaded via ini-file
      void initViewModes(  );

      /// Add a new ViewMode (non-userdefined viewMode)
      void slotAddViewMode(QString _mode, QStringList _usedWidgets);

      /// Add a new ViewMode (_custom == userdefined viewMode)
      void slotAddViewMode(QString _mode, bool _custom, QStringList _usedWidgets);

      /// Slot for Changing visible toolWidgets
      void slotChangeView(QString _mode, QStringList _toolWidgets);

    private slots:
      /// Remove viewMode
      void slotRemoveViewMode(QString _name);

      /// Slot for setting ViewMode from Menu
      void slotSetViewMode( QAction* action);


      /// Show a dialog in which the viewMode can be changed
      void slotViewModeDialog();

      /// Set the view Mode to the given Mode
    public slots :
      void setViewMode( QString _mode );

   /** @} */



  //===========================================================================
    /** @name Basic Gui Elements
    * @{ */
  //===========================================================================

  public :

    /// Examiner Widget
    std::vector< glViewer* >  examiner_widgets_;

    ///Spliter between toplevel objects and the textedit at the bottom
    QSplitter* splitter_;

    /// Textedit at the bottom for log messages
    LoggerWidget* logWidget_;

    /// Size of the logging window ( defaults to 240 )
    int originalLoggerSize_;

    /// QMenu containing the recently opened files
    QMenu* recentFilesMenu_;

    /// Help Menu
    QMenu* helpMenu_;

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

    /// Temporary widget
    QWidget* tempLogWidget;

    /// Widget for toolBox
    QWidget* toolBoxArea_;

    /// Spliter between toplevel objects and toolbox
    QSplitter* toolSplitter_;

    /// Toolbox
    SideArea* toolBox_;

    /// Toolbox scroll area
    QScrollArea* toolBoxScroll_;


    // widget showing the scenegraph
    ACG::QtWidgets::QtSceneGraphDialog* sceneGraphDialog_;

  /** @} */

  private slots:

    /** Creates and shows the Scenegraph Dialog
     */
    void slotShowSceneGraphDialog();


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

    void slotAddMenubarAction( QAction* _action , MenuActionType _type );

  private:

    /// File Menu
    QMenu *fileMenu_;

    /// View Menu
    QMenu *viewMenu_;

    /// Tools Menu
    QMenu *toolsMenu_;

    /// First entry after all relevant parts of the File Menu
    QAction* fileMenuEnd_;

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
    QToolButton* lightButton_;
    QToolButton* pickButton_;
    QToolButton* questionButton_;

  /** @} */

  //===========================================================================
    /** @name View Menu
      * @{ */
  //===========================================================================

  public slots:
    /// Setup and update the global draw menu
    void slotUpdateGlobalDrawMenu();

  private slots:
    /// Called when the global drawMode is selected
    void slotGlobalDrawMenu(QAction * _action);

    /// Called before the view Menu is shown
    void slotViewMenuAboutToShow();

  private:
    /// This variable holds the global draw menu
    QMenu* globalDrawMenu_;

    QActionGroup * drawGroup_;

    /// Group for all menu items
    QActionGroup* viewGroup_;

    QAction* perspectiveProjectionAction_;
    QAction* orthogonalProjectionAction_;

    int activeDrawModes_;

    int availableGlobalDrawModes_;

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

    /// Paste the view to the last active examiner
    void slotPasteView( );

    /// Copy view from the last active examiner
    void slotCopyView( );

    /// Called when a coordsys drawMode has been changed
    void slotViewerDrawMenu( QAction * _action );

    /// Creates a draw Menu for the currently active Viewer
    void slotUpdateViewerDrawMenu();

    /// Create a snapshot of the last active examiner
    void slotSnapshot();

    /// Set the snapShot name for all examiners
    void slotSnapshotName();

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

    /// Container widget for holding multiple views
    QStackedWidget* stackedWidget_;

  /** @} */

  //===========================================================================
  /** @name Help System
  * @{ */
  //===========================================================================

  public slots:
    /// Display the help Browser
    void showHelpBrowser();

  private:
    /// Pointer to the help Browser
    HelpWidget* helpWidget_;

  /** @} */

  //===========================================================================
  /** @name About System
  * @{ */
  //===========================================================================

  public slots:
    /// Display the about widget
    void showAboutWidget();

  private:
    /// Pointer to the about widget
    AboutWidget* aboutWidget_;

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

  private:
    void setupStatusBar();

  private :
    QStatusBar* statusBar_;

    QLabel* statusIcon_;

  /** @} */

  //===========================================================================
  /** @name Drag and Drop
   * @{ */
  //===========================================================================
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


    /// Toggle projection Mode of the active viewer.
    void slotContextSwitchProjection();

    /// Toggle projection Mode of all viewers to perspective projection
    void slotGlobalPerspectiveProjection();

    /// Toggle projection Mode of all viewers to orthographic projection
    void slotGlobalOrthographicProjection();



    /// Set the animation Mode for all viewers
    void slotGlobalChangeAnimation(bool _animation);

    /// Set the animation Mode for active viewer
    void slotLocalChangeAnimation(bool _animation);



    /// Set Backface culling for all viewers
    void slotGlobalChangeBackFaceCulling(bool _backFaceCulling);

    /// Set Backface culling for active viewer
    void slotLocalChangeBackFaceCulling(bool _backFaceCulling);



    /// Set two sided lighting for all viewers
    void slotGlobalChangeTwoSidedLighting(bool _lighting);

    /// Set two sided lighting for active viewer
    void slotLocalChangeTwoSidedLighting(bool _lighting);


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
    void setLightMode()   { setActionMode(Viewer::LightMode    ); };
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
     *  @param _name Name of the Pick Mode or "Separator" to insert a separator
     *  @param _mouse_tracking true: every mouse movement will emit mouse events not only when mousebutton is pressed
     *  @param _pos position to insert the mode in the popup menu.
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

  public slots:

    /** Switch to given picking mode
     * @param _name Name of the picking mode
     */
    void setPickMode(const std::string  _name);


    void getPickMode(std::string& _name);

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

    void actionPickMenu( QAction * _action );

  signals:
    /** This signal is emitted when the pickMode is changed and contains the new PickMode
     */
    void signalPickModeChanged(const std::string&);

  private:

    QMenu* pickMenu_;

    /** Struct containing information about pickModes
     */
    struct PickMode
    {
      /// Constructor
      PickMode(const std::string& _n, bool _t, bool _v, QCursor _c) :
               name(_n), tracking(_t), visible(_v), cursor(_c) {}

      /// Name of the pickMode
      std::string  name;

      /** MouseTracking enabled for this mode?
       */
      bool         tracking;

      /** Defines if the Mode will be visible in the popup Menu
       */
      bool         visible;

      /** Cursor used in  this pickMode
       */
      QCursor      cursor;
    };

    /** Vector of all Picking modes
     */
    std::vector<PickMode>  pick_modes_;

    /** Name of current pickMode
     */
    std::string            pick_mode_name_;

    /** Index of current pickMode
     */
    int                    pick_mode_idx_;


    /// update pick mode menu
    void updatePickMenu();

  private slots:

    void hidePopupMenus();
  /** @} */

};


//=============================================================================
#endif // COREWIDGET_HH defined
//=============================================================================

