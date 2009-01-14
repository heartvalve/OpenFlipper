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
#include <OpenFlipper/BasePlugin/ContextMenuInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>
#include <OpenFlipper/BasePlugin/MenuInterface.hh>
#include <OpenFlipper/BasePlugin/ViewInterface.hh>
#include <OpenFlipper/BasePlugin/StatusbarInterface.hh>


#include <ACG/QtWidgets/QtExaminerViewer.hh>

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
#include <OpenFlipper/widgets/helpBrowser/helpWidget.hh>

#include <OpenFlipper/Core/PluginInfo.hh>


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
  /// The context Menu
  QMenu*          menu;

  /// Type of objects for which the context Menu should be visible
  DataType        contextType;

  /// Position of the context Menu inside the main widgets context Menu
  ContextMenuType position;
};

/** Core Widget of the Application
 */
class CoreWidget : public QMainWindow
{
  Q_OBJECT

public:

  /// constructor
  CoreWidget( QVector<ViewMode*>& _viewModes, std::vector<PluginInfo>& _plugins );

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
    void unloadPlugin();

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

    ///vector of keys registered to the core
    std::vector<KeyBinding> coreKeys_;

    ///mapping of all keys to registered keys and the corresponding plugins
    KeyMap keys_;

    ///mapping of all registered keys and the corresponding plugins to currently assigned keys
    InverseKeyMap invKeys_;

  private slots:
    void slotRegisterKey(int _key, Qt::KeyboardModifiers _modifiers, QString _description,
                         bool _multiUse = false);

  public slots:
    void slotAddKeyMapping(int _key, Qt::KeyboardModifiers _modifiers, QObject* _plugin, int _keyBindingID);

   /** @} */

  //===========================================================================
  /** @name GUI creation
  * @{ */
  //===========================================================================

  public:

  /// Setup the main menubar
  void setupMenuBar();

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
    void showLogger(bool _state);

    /// Hide or show toolbox area
    void toggleToolbox();

    /// Show or hide toolbox
    void showToolbox( bool _state );

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

      ///DockWidget and Button for changing viewMode via dialog
      QDockWidget* dockViewMode_;

    private:

      QPushButton* viewModeButton_;

      /// Submenu holding all ViewMode actions
      QMenu* viewModeMenu_;

      /// Group for all menu items
      QActionGroup* viewGroup_;

    public:
      /// Tab all DockWidgets which belong to ToolWidgets together
      void tabDockWidgets(QVector< QDockWidget* > _widgets);

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

      /// Slot is called when the visibility of a toolWidget changes
      void slotVisibilityChanged ( bool visible );

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
    std::vector< ACG::QtWidgets::QtExaminerViewer* >  examiner_widgets_;

    /// temporary pointer pointing to first element of above vector for porting to multi view mode
    ACG::QtWidgets::QtExaminerViewer* examiner_widget_;

    ///Spliter between toplevel objects and the textedit at the bottom
    QSplitter* splitter_;

    /// Textedit at the bottom for log messages
    LoggerWidget* logWidget_;

    /// Size of the logging window ( defaults to 240 )
    int originalLoggerSize_;

    /// QMenu containing the recently opened files
    QMenu* recentFilesMenu_;

    /// Plugins Menu
    QMenu* pluginsMenu_;

    /// Help Menu
    QMenu* helpMenu_;

  /** @} */

  private slots:
   /** Display a log message in the Mainwidget
    * @param _type Type of Message (OUT,WARN,ERR)
    * @param _message The message to be displayed
    */
   void slotLog(Logtype _type, QString _message);

  //===========================================================================
    /** @name Menubar controls
      * @{ */
  //===========================================================================
  private slots :

    void slotAddMenu( QMenu* _menu , MenuType _type );

  private:

    /// File Menu
    QMenu *fileMenu_;

    /// View Menu
    QMenu *viewMenu_;

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

  /** @} */

  //===========================================================================
    /** @name Context Menu
      * @{ */
  //===========================================================================
  signals :
    // tells the plugins to update their context menu
    void updateContextMenu(int) ;

  private slots:
    /// This slot is called by the examiner widgets gl area when a context menu is requested
    void slotCustomContextMenu( const QPoint& _point );

    /// called by plugins to add a new context menu
    void slotAddContextMenu(QMenu* _menu);

    /// called by plugins to add a real context menu depending on DataType
    void slotAddContextMenu( QMenu* _menu , DataType _dataType ,ContextMenuType type_);

    /// update globalOptions for actions in the contexMenu
    void updateGlobalOptions(bool _enable);

    /// change the background color
    void changeBackgroundColor();

    /// Paste the view to the last active examiner
    void slotPasteView( );

    /// Copy view from the last active examiner
    void slotCopyView( );

  private :
    /** Update the contextmenu for the given position inside an examiner widget
     *
     * @param _point      Picking position in coordinates of the viewer
     * @param _examinerId Id of the examinerWidget used
     */
    void updatePopupMenu(const QPoint& _point, unsigned int _examinerId);

  private :
    /// context Menu for the gl area
    QMenu*  contextMenu_;

    /// Context Menu containing all selection elements
    QMenu*  contextSelectionMenu_;

    /// All context menu entries which will always be available
    std::vector< MenuInfo > persistentContextMenus_;

    /// All real context menu entries
    std::vector< MenuInfo > contextMenus_;

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
    void showHelpBrowserUser();

    /// Display the help Browser
    void showHelpBrowserDeveloper();

  private:
    /// Pointer to the help Browser
    HelpWidget* helpBrowserDeveloper_;

    /// Pointer to the help Browser
    HelpWidget* helpBrowserUser_;

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
};


//=============================================================================
#endif // COREWIDGET_HH defined
//=============================================================================

