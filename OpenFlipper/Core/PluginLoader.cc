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
//  CLASS Core - IMPLEMENTATION of the Plugin Loading
//
//=============================================================================


//== INCLUDES =================================================================

// -------------------- mview
#include "Core.hh"

#include <QMenuBar>
#include <QToolBox>
#include <QMessageBox>
#include <QApplication>

#include <QPluginLoader>
#include "OpenFlipper/BasePlugin/BaseInterface.hh"
#include "OpenFlipper/BasePlugin/KeyInterface.hh"
#include "OpenFlipper/BasePlugin/BackupInterface.hh"
#include "OpenFlipper/BasePlugin/LoggingInterface.hh"
#include "OpenFlipper/BasePlugin/MouseInterface.hh"
#include "OpenFlipper/BasePlugin/PickingInterface.hh"
#include "OpenFlipper/BasePlugin/ToolboxInterface.hh"
#include "OpenFlipper/BasePlugin/ToolbarInterface.hh"
#include "OpenFlipper/BasePlugin/TextureInterface.hh"
#include "OpenFlipper/BasePlugin/MenuInterface.hh"
#include "OpenFlipper/BasePlugin/ContextMenuInterface.hh"
#include "OpenFlipper/BasePlugin/ViewInterface.hh"
#include "OpenFlipper/BasePlugin/LoadSaveInterface.hh"
#include "OpenFlipper/BasePlugin/StatusbarInterface.hh"
#include "OpenFlipper/BasePlugin/INIInterface.hh"
#include "OpenFlipper/BasePlugin/GlobalAccessInterface.hh"
#include "OpenFlipper/BasePlugin/FileInterface.hh"
#include "OpenFlipper/BasePlugin/RPCInterface.hh"
#include "OpenFlipper/BasePlugin/ScriptInterface.hh"

#include "OpenFlipper/INIFile/INIFile.hh"

#include "OpenFlipper/common/GlobalOptions.hh"

#include <QMessageBox>
#include <QtScript/QScriptValueIterator>

#include <ACG/QtWidgets/QtFileDialog.hh>
#include "OpenFlipper/widgets/unloadPluginsWidget/unloadPluginsWidget.hh"

//== IMPLEMENTATION ==========================================================


bool Core::checkSlot(QObject* _plugin , const char* _slotSignature) {
  const QMetaObject* meta = _plugin->metaObject();
  int id = meta->indexOfSlot( QMetaObject::normalizedSignature( _slotSignature ) );
  return ( id != -1 );
}

bool Core::checkSignal(QObject* _plugin , const char* _signalSignature) {
  const QMetaObject* meta = _plugin->metaObject();
  int id = meta->indexOfSignal( QMetaObject::normalizedSignature( _signalSignature ) );
  return ( id != -1 );
}

 /** Load plugins from dir Plugins. The plugin information gets stored, toolbox and menu entries are created and the required signals and slots are connected.
  */
void Core::loadPlugins()
{
  //try to load plugins from new location
  QDir tempDir = QDir(OpenFlipper::Options::applicationDir());
  tempDir.cd("Plugins");

  #ifdef WIN32
    tempDir.cd("Windows");
  #else
    tempDir.cd("Linux");
  #endif

  if ( OpenFlipper::Options::is64bit() )
    tempDir.cd("64");
  else
    tempDir.cd("32");

  #ifdef DEBUG
    tempDir.cd("Debug");
  #else
    tempDir.cd("Release");
  #endif

  QStringList pluginlist = tempDir.entryList(QDir::Files);

  for (int i=0; i < pluginlist.size(); i++)
    pluginlist[i] = tempDir.absoluteFilePath(pluginlist[i]);

  //try to load plugins from old location

  emit log(LOGOUT,"Trying to find Plugins at " + OpenFlipper::Options::pluginDir().absolutePath() );

  // Get all files in the Plugin dir
  QStringList pluginlist2 = OpenFlipper::Options::pluginDir().entryList(QDir::Files);

  for (int i=0; i < pluginlist2.size(); i++)
    pluginlist2[i] = tempDir.absoluteFilePath(pluginlist2[i]);

  pluginlist += pluginlist2;

  // Get all config files to be used
  QStringList configFiles = OpenFlipper::Options::optionFiles();

  QStringList iniPlugins;
  for ( int fileCount = 0 ; fileCount < (int)configFiles.size() ; ++fileCount) {
    INIFile ini;

    if (  ini.connect(configFiles[fileCount],false) ) {

      // get all Plugin Names which will not be loaded
      QString dontLoadString;
      if (ini.get_entry( dontLoadString,"Plugins","DontLoad" ) ) {
        dontLoadPlugins_ = dontLoadPlugins_ + dontLoadString.split(";",QString::SkipEmptyParts);
      }

      std::vector< QString > loadKeys;

      if ( OpenFlipper::Options::is64bit() )
        loadKeys.push_back("Load64");

      if ( OpenFlipper::Options::is32bit() )
        loadKeys.push_back("Load32");

      loadKeys.push_back("Load");

      for ( uint keyit = 0 ; keyit < loadKeys.size() ; ++ keyit) {
        //Load additional Plugins given by ini file
        std::vector< QString > additionalPlugins;
        if (ini.get_entry( additionalPlugins,"Plugins",loadKeys[keyit] ) ) {
          for ( uint i = 0 ; i < additionalPlugins.size(); ++i) {
            iniPlugins << additionalPlugins[i];
              emit log(LOGOUT,"Additional Plugin from ini file : " +  additionalPlugins[i] );
          }
        }
      }

      ini.disconnect();

   } else
      emit log(LOGWARN,"Failed to connect to  ProgramOptions.ini file: " + configFiles[fileCount]);
  }

  pluginlist = iniPlugins << pluginlist;


  // Sort plugins to load FilePlugins first
  QStringList filePlugins;
  QStringList textureControl;
  QStringList otherPlugins;
  //plugin Liste sortieren
  for (int i=0; i < pluginlist.size(); i++)
    if (pluginlist[i].contains("FilePolyLine") || pluginlist[i].contains("FileTriangleMesh") ||
        pluginlist[i].contains("FilePolyMesh"))
      filePlugins.push_back(pluginlist[i]);
    else if (pluginlist[i].contains("TextureControl"))
      textureControl.push_back(pluginlist[i]);
    else
      otherPlugins.push_back(pluginlist[i]);

  pluginlist = filePlugins << textureControl << otherPlugins;

  // Remove Whitespace from beginning and end of Plugin Names
  for ( int i = 0 ; i < dontLoadPlugins_.size() ; ++i )
    dontLoadPlugins_[i] = dontLoadPlugins_[i].trimmed();

  for ( int i = 0 ; i < dontLoadPlugins_.size(); ++i )
    emit log(LOGWARN,"Skipping Plugins :\t " + dontLoadPlugins_[i] );

  emit log(LOGOUT,"=============================================================================================");


  // Try to load each file as a plugin
  for ( int i = 0 ; i < pluginlist.size() ; ++i) {

    if ( OpenFlipper::Options::gui() && OpenFlipper::Options::splash() ) {
      splashMessage_ = "Loading Plugin " + QString::number(i) + "/"  + QString::number(pluginlist.size());
      splash_->showMessage( splashMessage_ , Qt::AlignBottom | Qt::AlignLeft , Qt::white);
      QApplication::processEvents();
    }

    loadPlugin(pluginlist[i],true);
  }

  splashMessage_ = "";

  emit pluginsInitialized();

  emit log(LOGOUT,"Loaded " + QString::number(plugins.size()) + " Plugin(s)" );
}

/** @brief slot for loading Plugins
 */
void Core::slotLoadPlugin(){

  if ( OpenFlipper::Options::nogui() )
    return;


  QString filter;
  if ( OpenFlipper::Options::isWindows() )
    filter = "Plugins (*.dll)";
  else
    filter = "Plugins (*.so)";

  QString filename = ACG::getOpenFileName(coreWidget_,tr("Load Plugin"),filter,OpenFlipper::Options::currentDirStr());

  if (filename.isEmpty())
    return;

  // get the plugin name
  // and check if Plugin is in the dontLoad List

  QPluginLoader loader( filename );
  QObject *plugin = loader.instance();
  QString name;

  // Check if a plugin has been loaded
  if (plugin) {
    // Check if it is a BasePlugin
    BaseInterface* basePlugin = qobject_cast< BaseInterface * >(plugin);
    if ( basePlugin ) {
      name = basePlugin->name();
    }else
      return;
  }else
    return;

  if (dontLoadPlugins_.contains(name)){
      int ret = QMessageBox::question(0, tr("Plugin Loading Prevention"),
                   tr("OpenFlipper is currently configured to prevent loading this plugin.\n"
                      "Do you want to enable this plugin?"),
                   QMessageBox::Yes | QMessageBox::No,
                   QMessageBox::Yes);
      if (ret == QMessageBox::Yes)
        dontLoadPlugins_.removeAll(name);
      else
        return;
  }
  loadPlugin(filename,false);
}

/** @brief slot for unloading Plugins
 */
void Core::slotUnloadPlugin(){
  static unloadPluginsWidget* widget = 0;

  //make a list of pluginNames
  QStringList pluginNames;
  for (uint i=0; i < plugins.size(); i++)
    pluginNames << plugins[i].name;

  //init and show the widget
  if (!widget){
    widget = new unloadPluginsWidget(pluginNames);
    connect( widget, SIGNAL(unload(QString)), this, SLOT(unloadPlugin(QString)));
    connect( widget, SIGNAL(dontLoadPlugins(QStringList)), this, SLOT(dontLoadPlugins(QStringList)));
  }else
    widget->setPlugins(pluginNames);

  widget->show();
}

/** @brief Unload a Plugin with given name (slot)
 *  @param name plugin name
 */
void Core::unloadPlugin(QString name){
  for (uint i=0; i < plugins.size(); i++)
    if (plugins[i].name == name){
      if ( checkSlot( plugins[i].plugin , "exit()" ) )
        QMetaObject::invokeMethod(plugins[i].plugin, "exit",  Qt::DirectConnection);

      //remove toolbox widget
      QString name_nospace = name;
      name_nospace.remove(" ");
      if ( coreWidget_->viewModes_[0]->visibleWidgets.contains(name_nospace) )
        coreWidget_->viewModes_[0]->visibleWidgets.removeAt(coreWidget_->viewModes_[0]->visibleWidgets.indexOf(name_nospace));
      if (plugins[i].widget){
        plugins[i].widget->setVisible(false);
        delete plugins[i].widget;
      }

      plugins.erase(plugins.begin() + i);

      emit log(LOGOUT,"Unloaded Plugin :\t\t " + name);

      return;
    }

    log(LOGERR, "Unable to unload plugin '" + name + "' (plugin wasn't found)");
}

/** @brief prevent OpenFlipper from loading plugins on the next start (slot)
 *  @param _plugins semicolon separated string with plugin names
 */
void Core::dontLoadPlugins(QStringList _plugins){
  for (int i=0; i < _plugins.size(); i++)
    dontLoadPlugins_ << _plugins[i];
}

/** @brief Load a Plugin with given filename
 *  @param filename complete path + filename of the plugin
 *  @param silent if 'true': user isn't asked what to do if a plugin is already loaded
 */
void Core::loadPlugin(QString filename, bool silent){

  // Only load .dll under windows
  if ( OpenFlipper::Options::isWindows() ) {
    QString dllname = filename;
    if ( ! dllname.endsWith( ".dll" ) )
      return;
  }

  // Only load .so under linux
  if ( OpenFlipper::Options::isLinux() ) {
    QString soname = filename;
    if ( ! soname.endsWith( ".so" ) )
      return;
  }

  // Try to open the file
  QPluginLoader loader( filename );
  QObject *plugin = loader.instance();

  // Check if a plugin has been loaded
  if (plugin) {
    PluginInfo info;
    QString supported;

    // Check if it is a BasePlugin
    BaseInterface* basePlugin = qobject_cast< BaseInterface * >(plugin);
    if ( basePlugin ) {
      emit log(LOGOUT,"Found Plugin :\t\t " + basePlugin->name() + " at " + filename);

      if ( OpenFlipper::Options::gui() && OpenFlipper::Options::splash() ) {
        splashMessage_ = splashMessage_ + " " + basePlugin->name() ;
        splash_->showMessage( splashMessage_ , Qt::AlignBottom | Qt::AlignLeft , Qt::white);
        QApplication::processEvents();
      }

      //Check if plugin is already loaded
      for (uint k=0; k < plugins.size(); k++)
        if (plugins[k].name == basePlugin->name()){
          if (silent || OpenFlipper::Options::nogui() ){ //dont load the plugin
            emit log(LOGWARN, "\t\t\t Already loaded from " + plugins[k].path);
            emit log(LOGOUT,"=============================================================================================");
            return;
          }else{ //ask the user
            int ret = QMessageBox::question(coreWidget_,
                                            tr("Plugin already loaded"),
                                            "A Plugin with the same name was already loaded from " +
                                            plugins[k].path + ".\n"
                                            "You can only load the new plugin if you unload the existing one first.\n\n"
                                            "Do you want to unload the existing plugin first?",
                                            QMessageBox::Yes|QMessageBox::No, QMessageBox::No);
            if (ret == QMessageBox::Yes)
              unloadPlugin(plugins[k].name);
            else{
              emit log(LOGWARN, "\t\t\t Already loaded from " + plugins[k].path);
              emit log(LOGOUT,"=============================================================================================");
              return;
            }
          }
        }

      if ( dontLoadPlugins_.contains(basePlugin->name(), Qt::CaseInsensitive) ) {
        emit log(LOGWARN,"OpenFlipper.ini prevented Plugin " + basePlugin->name() + " from being loaded! ");
        emit log(LOGOUT,"=============================================================================================");
        return;
      }

      emit log(LOGOUT,"Plugin Desciption :\t\t " + basePlugin->description());

      supported = "BaseInterface ";

      info.name          = basePlugin->name();
      info.description   = basePlugin->description();
      info.plugin        = plugin;
      info.path          = filename;
      info.widget        = 0;

      if ( checkSlot(plugin,"version()") )
        info.version = basePlugin->version();
      else
        info.version = QString::number(-1);

      if ( OpenFlipper::Options::nogui() ) {

        if ( ! checkSlot( plugin , "noguiSupported()" ) ) {
          emit log(LOGWARN,"Running in nogui mode which is unsupported by this plugin, skipping" );
          emit log(LOGOUT,"=============================================================================================");
          return;
        }

      }


      if ( checkSignal(plugin,"update_view()") )
        connect(plugin,SIGNAL(update_view()),this,SLOT(updateView()));

      if ( checkSignal(plugin,"updated_objects(int)") )
        connect(plugin,SIGNAL(updated_objects(int)),this,SLOT(slotObjectsListUpdated(int)));

      if ( checkSlot(plugin,"slotAllCleared()") )
        connect(this,SIGNAL(allCleared()),plugin,SLOT(slotAllCleared()));

      if ( checkSignal(plugin,"activeObjectChanged()") )
        connect(plugin,SIGNAL(activeObjectChanged()),this,SLOT(slotActiveObjectChanged() ));

      if ( checkSlot( plugin , "slotActiveObjectChanged()" ) )
        connect(this,SIGNAL(activeObjectChanged()),plugin,SLOT(slotActiveObjectChanged() ));

      if ( checkSlot( plugin , "slotObjectUpdated(int)" ) )
        connect(this,SIGNAL(ObjectListUpdated(int)),plugin,SLOT(slotObjectUpdated(int)));

      if ( checkSlot( plugin , "pluginsInitialized()" ) )
        connect(this,SIGNAL(pluginsInitialized()),plugin,SLOT(pluginsInitialized()), Qt::DirectConnection);
    }

    //Check if its a filePlugin
    FileInterface* filePlugin = qobject_cast< FileInterface * >(plugin);
    if ( filePlugin ){
      supported = supported + "File ";

      // Collect supported Data from file plugin
      fileTypes ft;
      ft.type = filePlugin->supportedType();
      ft.loadFilters = filePlugin->getLoadFilters();
      ft.saveFilters = filePlugin->getSaveFilters();
      ft.plugin = filePlugin;

      supportedTypes_.push_back(ft);
      filePlugin->setObjectRoot( objectRoot_ );
    }

    GlobalAccessInterface* globalAccessPlugin = qobject_cast< GlobalAccessInterface * >(plugin);
    if ( globalAccessPlugin && OpenFlipper::Options::gui() ) {
      supported = supported + "!!GLOBAL ACCESS!! ";

      globalAccessPlugin->set_examiner( coreWidget_->examiner_widget_);

    }

    //Check if the plugin supports Logging
    LoggingInterface* logPlugin = qobject_cast< LoggingInterface * >(plugin);
    if ( logPlugin ) {
      supported = supported + "Logging ";

      // Create intermediate logger class which will mangle the output
      PluginLogger* newlog = new PluginLogger(info.name);
      loggers_.push_back(newlog);
      connect(plugin,SIGNAL(log(Logtype, QString )),newlog,SLOT(slotLog(Logtype, QString )),Qt::DirectConnection);
      connect(plugin,SIGNAL(log(QString )),newlog,SLOT(slotLog(QString )),Qt::DirectConnection);

      // Connect it to the core widget logger
      if ( OpenFlipper::Options::gui() )
        connect(newlog,SIGNAL(log(Logtype, QString )),coreWidget_,SLOT(slotLog(Logtype, QString )),Qt::DirectConnection);

      // connection to console logger
      connect(newlog,SIGNAL(log(Logtype, QString )),this,SLOT(slotLog(Logtype, QString )),Qt::DirectConnection);

      // connection to file logger
      connect(newlog,SIGNAL(log(Logtype, QString )),this,SLOT(slotLogToFile(Logtype, QString )),Qt::DirectConnection);

      // connection to external plugin logger
      if ( checkSlot(plugin,"logOutput(Logtype,QString)") )
        connect(this,SIGNAL(externalLog(Logtype,QString)), plugin, SLOT(logOutput(Logtype,QString)) ) ;
    }

    //Check if the plugin supports Menubar-Interface
    MenuInterface* menubarPlugin = qobject_cast< MenuInterface * >(plugin);
    if ( menubarPlugin && OpenFlipper::Options::gui() ) {
      supported = supported + "Menubar ";

      if ( checkSignal(plugin,"addMenu(QMenu*,MenuType)") )
        connect(plugin      , SIGNAL(addMenu(QMenu*,MenuType)),
                coreWidget_ , SLOT(slotAddMenu(QMenu*,MenuType)),Qt::DirectConnection);
    }

    //Check if the plugin supports ContextMenuInterface
    ContextMenuInterface* contextMenuPlugin = qobject_cast< ContextMenuInterface * >(plugin);
    if ( contextMenuPlugin && OpenFlipper::Options::gui() ) {
      supported = supported + "ContextMenu ";

      if ( checkSignal(plugin,"addContextMenu(QMenu*)") )
        connect(plugin      , SIGNAL(addContextMenu(QMenu*)),
                coreWidget_ , SLOT(slotAddContextMenu(QMenu*)),Qt::DirectConnection);

      if ( checkSignal(plugin,"addContextMenu(QMenu*,DataType,ContextMenuType)") )
        connect(plugin      , SIGNAL(addContextMenu(QMenu*,DataType,ContextMenuType)),
                coreWidget_ , SLOT(slotAddContextMenu(QMenu*,DataType,ContextMenuType)),Qt::DirectConnection);

      if ( checkSlot(plugin,"slotUpdateContextMenu(int)") )
        connect(coreWidget_ , SIGNAL(updateContextMenu(int)),
                plugin      , SLOT(slotUpdateContextMenu(int)),Qt::DirectConnection);

    }

    //Check if the plugin supports Toolbox-Interface
    ToolboxInterface* toolboxPlugin = qobject_cast< ToolboxInterface * >(plugin);
    if ( toolboxPlugin && OpenFlipper::Options::gui() ) {
      supported = supported + "Toolbox ";

      QWidget* widget = 0;
      if ( toolboxPlugin->initializeToolbox( widget ) ) {

            QDockWidget* dock = new QDockWidget(info.name , coreWidget_ );
            dock->setWidget(widget);
            widget->setParent(dock);
            widget->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Minimum);
            widget->setMinimumWidth(300);
            widget->setMaximumWidth(600);


            coreWidget_->setDockOptions(QMainWindow::AllowTabbedDocks | QMainWindow::VerticalTabs);
            coreWidget_->addDockWidget(Qt::RightDockWidgetArea,dock);

            dock->resize(300,widget->height() );
            connect(dock, SIGNAL( visibilityChanged (bool) ), coreWidget_ , SLOT( slotVisibilityChanged(bool) ));

            widget->show();

            info.widget = dock;

            // add widget name to viewMode 'all'
            QString name_nospace = info.name;
            name_nospace.remove(" ");
            if ( !viewModes_[0]->visibleWidgets.contains(name_nospace) ){
              viewModes_[0]->visibleWidgets << name_nospace;
              viewModes_[0]->visibleWidgets.sort();
            }
      }

      if ( checkSignal(plugin, "defineViewMode(QString,QStringList)"))
        connect(plugin, SIGNAL( defineViewMode(QString, QStringList) ),
                coreWidget_, SLOT( slotAddViewMode(QString, QStringList) ),Qt::DirectConnection );

    }

    //Check if the plugin supports Toolbox-Interface
    ToolbarInterface* toolbarPlugin = qobject_cast< ToolbarInterface * >(plugin);
    if ( toolbarPlugin && OpenFlipper::Options::gui() ) {
      supported = supported + "Toolbars ";

      if ( checkSignal(plugin,"addToolbar(QToolBar*)") )
        connect(plugin,SIGNAL(addToolbar(QToolBar*)),
                coreWidget_,SLOT(slotAddToolbar(QToolBar*)),Qt::DirectConnection);

      if ( checkSignal(plugin,"removeToolbar(QToolBar*)") )
        connect(plugin,SIGNAL(removeToolbar(QToolBar*)),
                coreWidget_,SLOT(slotRemoveToolbar(QToolBar*)),Qt::DirectConnection);
    }

    //Check if the plugin supports StatusBar-Interface
    StatusbarInterface* statusbarPlugin = qobject_cast< StatusbarInterface * >(plugin);
    if ( statusbarPlugin && OpenFlipper::Options::gui() ) {
      supported = supported + "StatusBar ";

      if ( checkSignal(plugin,"showStatusMessage(QString,int)") )
        connect(plugin,SIGNAL(showStatusMessage(QString,int)),
                coreWidget_,SLOT(statusMessage(QString,int)),Qt::DirectConnection);


      if ( checkSignal(plugin,"setStatus(ApplicationStatus::applicationStatus)") )
        connect(plugin,SIGNAL(setStatus(ApplicationStatus::applicationStatus)),
                coreWidget_,SLOT(setStatus(ApplicationStatus::applicationStatus)),Qt::DirectConnection);

      if ( checkSignal(plugin,"clearStatusMessage()") )
        connect(plugin,SIGNAL(clearStatusMessage()),
                coreWidget_,SLOT(clearStatusMessage()));

    }

    //Check if the plugin supports Key-Interface
    KeyInterface* keyPlugin = qobject_cast< KeyInterface * >(plugin);
    if ( keyPlugin && OpenFlipper::Options::gui() ) {
      supported = supported + "KeyboardEvents ";

      if ( checkSignal(plugin,"registerKey(int,Qt::KeyboardModifiers,QString,bool)") )
        connect(plugin,SIGNAL( registerKey(int, Qt::KeyboardModifiers, QString, bool) ),
                coreWidget_,SLOT(slotRegisterKey(int, Qt::KeyboardModifiers, QString, bool)) );

//       if ( checkSlot( plugin , "slotKeyEvent(QKeyEvent*)" ) )
//         connect(coreWidget_,SIGNAL(PluginKeyEvent(QKeyEvent* )), plugin,SLOT(slotKeyEvent(QKeyEvent*)));
//
//       if ( checkSlot( plugin , "slotKeyReleaseEvent(QKeyEvent*)" ) )
//         connect(coreWidget_,SIGNAL(PluginKeyReleaseEvent(QKeyEvent* )), plugin,SLOT(slotKeyReleaseEvent(QKeyEvent*)));
    }

    //Check if the plugin supports Mouse-Interface
    MouseInterface* mousePlugin = qobject_cast< MouseInterface * >(plugin);
    if ( mousePlugin && OpenFlipper::Options::gui()  ) {
      supported = supported + "MouseEvents ";

      if ( checkSlot( plugin , "slotMouseWheelEvent(QWheelEvent*,const std::string&)" ) )
        connect(this   , SIGNAL(PluginWheelEvent(QWheelEvent * , const std::string & )),
                plugin , SLOT(slotMouseWheelEvent(QWheelEvent* , const std::string & )));

      if ( checkSlot( plugin , "slotMouseEvent(QMouseEvent*)" ) )
        connect(this   , SIGNAL(PluginMouseEvent(QMouseEvent*)),
                plugin , SLOT(slotMouseEvent(QMouseEvent*)));

      if ( checkSlot( plugin , "slotMouseEventIdentify(QMouseEvent*)" ) )
        connect(this   , SIGNAL(PluginMouseEventIdentify(QMouseEvent*)),
                plugin , SLOT(slotMouseEventIdentify(QMouseEvent*)));

    }

    //Check if the plugin supports Picking-Interface
    PickingInterface* pickPlugin = qobject_cast< PickingInterface * >(plugin);
    if ( pickPlugin && OpenFlipper::Options::gui() ) {
      supported = supported + "Picking ";

      if ( checkSlot( plugin , "slotPickModeChanged(const std::string&)" ) )
        connect(coreWidget_->examiner_widget_,SIGNAL(signalPickModeChanged (const std::string &)),
                plugin,SLOT(slotPickModeChanged( const std::string &)));

      if ( checkSignal(plugin,"addPickMode(const std::string)") )
        connect(plugin,SIGNAL(addPickMode( const std::string )),
                this,SLOT(slotAddPickMode( const std::string )),Qt::DirectConnection);

      if ( checkSignal(plugin,"addHiddenPickMode(const std::string)") )
        connect(plugin,SIGNAL(addHiddenPickMode( const std::string )),
                this,SLOT(slotAddHiddenPickMode( const std::string )),Qt::DirectConnection);
      if ( checkSignal(plugin,"addPickMode(const std::string,QCursor)") )
        connect(plugin,SIGNAL(addPickMode( const std::string ,QCursor)),
                this,SLOT(slotAddPickMode( const std::string ,QCursor)),Qt::DirectConnection);

      if ( checkSignal(plugin,"addHiddenPickMode(const std::string,QCursor)") )
        connect(plugin,SIGNAL(addHiddenPickMode( const std::string ,QCursor)),
                this,SLOT(slotAddHiddenPickMode( const std::string ,QCursor)),Qt::DirectConnection);
    }

    //Check if the plugin supports INI-Interface
    INIInterface* iniPlugin = qobject_cast< INIInterface * >(plugin);
    if ( iniPlugin ) {
      supported = supported + "INIFile ";

      if ( checkSlot( plugin , "loadIniFile(INIFile&,int)" ) )
        connect(this    , SIGNAL(iniLoad( INIFile&,int)),
                plugin  , SLOT( loadIniFile( INIFile&,int) ),Qt::DirectConnection);

      if ( checkSlot( plugin , "saveIniFile(INIFile&,int)" ) )
        connect(this    , SIGNAL(iniSave( INIFile& , int )),
                plugin  , SLOT( saveIniFile( INIFile& , int ) ),Qt::DirectConnection);

      if ( checkSlot( plugin , "saveIniFileOptions(INIFile&)" ) )
        connect(this    , SIGNAL(iniSaveOptions( INIFile& )),
                plugin  , SLOT( saveIniFileOptions( INIFile& ) ),Qt::DirectConnection);

      if ( checkSlot( plugin , "loadIniFileOptions(INIFile&)" ) )
        connect(this    , SIGNAL(iniLoadOptions( INIFile& )),
                plugin  , SLOT( loadIniFileOptions( INIFile& ) ),Qt::DirectConnection);
    }

    //Check if the plugin supports Texture-Interface
    TextureInterface* texturePlugin = qobject_cast< TextureInterface * >(plugin);
    if ( texturePlugin && OpenFlipper::Options::gui() ) {
      supported = supported + "Textures ";

      if ( checkSignal(plugin,"addTexture(QString,QString,uint)") )
        connect(plugin , SIGNAL(addTexture( QString , QString , uint )),
                this   , SLOT(slotAddTexture(QString, QString, uint)),Qt::DirectConnection);

      if ( checkSlot( plugin , "slotTextureAdded(QString,QString,uint)" ) )
        connect(this   , SIGNAL(addTexture(QString,QString, uint)),
                plugin , SLOT(slotTextureAdded(QString,QString, uint)),Qt::DirectConnection);

      if ( checkSignal(plugin,"updateTexture(QString,int)") )
        connect(plugin , SIGNAL(updateTexture( QString ,int )),
                this   , SLOT(slotUpdateTexture(QString , int)));

      if ( checkSlot( plugin , "slotUpdateTexture(QString,int)" ) )
        connect(this   , SIGNAL(updateTexture(QString ,int)),
                plugin , SLOT(slotUpdateTexture(QString,int )));

      if ( checkSignal(plugin,"updateAllTextures()") )
        connect(plugin , SIGNAL(updateAllTextures()),
                this   , SLOT(slotUpdateAllTextures()));

      if ( checkSlot( plugin , "slotUpdateAllTextures()" ) )
        connect(this   , SIGNAL(updateAllTextures()),
                plugin , SLOT(slotUpdateAllTextures()));

      if ( checkSignal(plugin,"updatedTextures(QString,int)") )
        connect(plugin , SIGNAL(updatedTextures( QString , int )),
                this   , SLOT(slotTextureUpdated( QString, int ) ));

      if ( checkSlot( plugin , "slotTextureUpdated(QString,int)" ) )
        connect(this   , SIGNAL(updatedTextures( QString , int )),
                plugin , SLOT(slotTextureUpdated( QString, int ) ));

      if ( checkSignal(plugin,"setTextureMode(QString,QString)") )
        connect(plugin , SIGNAL(setTextureMode(QString  ,QString )),
                this   , SLOT(slotSetTextureMode(QString  ,QString )),Qt::DirectConnection );

      if ( checkSlot( plugin , "slotSetTextureMode(QString,QString)" ) )
        connect(this   , SIGNAL(setTextureMode(QString  ,QString )),
                plugin , SLOT(slotSetTextureMode(QString  ,QString )),Qt::DirectConnection );

      if ( checkSignal(plugin,"switchTexture(QString)") )
        connect(plugin , SIGNAL(switchTexture(QString )),
                this   , SLOT(slotSwitchTexture(QString )),Qt::DirectConnection);

      if ( checkSlot( plugin , "slotSwitchTexture(QString)" ) )
        connect(this   , SIGNAL(switchTexture(QString )),
                plugin , SLOT(slotSwitchTexture(QString )),Qt::DirectConnection);
    }

    //Check if the plugin supports Backup-Interface
    BackupInterface* backupPlugin = qobject_cast< BackupInterface * >(plugin);
    if ( backupPlugin ) {
      supported = supported + "Backups ";

      if ( checkSignal( plugin , "createBackup(int,QString)" ) )
        connect(plugin , SIGNAL(createBackup( int , QString )) ,
                this   , SLOT(backupRequest( int , QString )),Qt::DirectConnection );

      if ( checkSlot( plugin , "slotBackupRequested(int,QString,int)" ) )
        connect(this   , SIGNAL(createBackup(int,QString,int)) ,
                plugin , SLOT( slotBackupRequested(int,QString,int) ),Qt::DirectConnection);
    }

    //Check if the plugin supports LoadSave-Interface
    LoadSaveInterface* LoadSavePlugin = qobject_cast< LoadSaveInterface * >(plugin);
    if ( LoadSavePlugin ) {
      supported = supported + "Load/Save ";
      if ( checkSignal(plugin,"load( QString,DataType,int& )" ) )
        connect(plugin , SIGNAL(load( QString,DataType,int& )) ,
                this   , SLOT(slotLoad( QString,DataType,int& )),Qt::DirectConnection );
      if ( checkSignal(plugin,"save(int,QString)" ) )
        connect(plugin , SIGNAL( save(int,QString) ) ,
                this   , SLOT( slotSave(int,QString) ), Qt::DirectConnection);

      if ( checkSlot( plugin , "fileOpened(int)" ) )
        connect(this   , SIGNAL( openedFile( int) ) ,
                plugin , SLOT( fileOpened( int ) ),Qt::DirectConnection);

      if ( checkSignal(plugin,"openedFile(int)" ) )
        connect(plugin , SIGNAL( openedFile( int ) ) ,
                this   , SLOT( slotObjectOpened ( int ) ),Qt::DirectConnection);

      if ( checkSignal(plugin,"addEmptyObject(DataType,int&)" ) )
        connect(plugin , SIGNAL( addEmptyObject( DataType, int& )) ,
                this   , SLOT( slotAddEmptyObject( DataType, int&) ),Qt::DirectConnection);

      if ( checkSignal(plugin,"emptyObjectAdded(int)" ) )
        connect(plugin , SIGNAL( emptyObjectAdded( int ) ) ,
                this   , SLOT( slotEmptyObjectAdded ( int ) ),Qt::DirectConnection);

      if ( checkSignal(plugin,"getAllFilters(QStringList&)" ) )
        connect(plugin , SIGNAL( getAllFilters( QStringList& ) ) ,
                this   , SLOT( slotGetAllFilters ( QStringList& ) ),Qt::DirectConnection);

    }

    //Check if the plugin supports View-Interface
    ViewInterface* viewPlugin = qobject_cast< ViewInterface * >(plugin);
    if ( viewPlugin  && OpenFlipper::Options::gui() ) {
      supported = supported + "View ";

      if ( checkSignal(plugin,"getStackWidget(QString,QWidget*&)" ) )
        connect(plugin      , SIGNAL(getStackWidget( QString , QWidget*&)),
                coreWidget_ , SLOT( slotGetStackWidget( QString , QWidget*& ) ) ,Qt::DirectConnection );
      if ( checkSignal(plugin,"addStackWidget(QString,QWidget*)" ) )
        connect(plugin      , SIGNAL(addStackWidget( QString , QWidget*)),
                coreWidget_ , SLOT( slotAddStackWidget( QString , QWidget* ) ) ,Qt::DirectConnection );
      if ( checkSignal(plugin,"updateStackWidget(QString,QWidget*)" ) )
        connect(plugin      , SIGNAL(updateStackWidget( QString , QWidget*)),
                coreWidget_ , SLOT( slotUpdateStackWidget( QString , QWidget* ) ) ,Qt::DirectConnection );
    }

    //Check if the plugin supports RPC-Interface
    RPCInterface* rpcPlugin = qobject_cast< RPCInterface * >(plugin);
    if ( rpcPlugin ) {
      supported = supported + "RPC ";

      if ( checkSignal(plugin,"pluginExists(QString,bool&)" ) )
        connect(plugin  , SIGNAL( pluginExists(QString,bool&) ),
                this    , SLOT( slotPluginExists(QString,bool&) ) ,Qt::DirectConnection );
      if ( checkSignal(plugin,"functionExists(QString,QString,bool&)" ) )
        connect(plugin  , SIGNAL(functionExists(QString,QString,bool&)),
                this    , SLOT( slotFunctionExists(QString,QString,bool&) ) ,Qt::DirectConnection );
      if ( checkSignal(plugin,"call(QString,QString,bool&)" ) )
        connect(plugin  , SIGNAL(call(QString,QString,bool&)),
                this    , SLOT(slotCall(QString,QString,bool&)) ,Qt::DirectConnection );
      if ( checkSignal(plugin,"call(QString,bool&)" ) )
        connect(plugin  , SIGNAL(call(QString,bool&)),
                this    , SLOT(slotCall(QString,bool&)) ,Qt::DirectConnection );
      if ( checkSignal(plugin,"getValue(QString,QVariant&)" ) )
        connect(plugin  , SIGNAL(getValue(QString,QVariant&)),
                this    , SLOT(slotGetValue(QString,QVariant&)) ,Qt::DirectConnection );
//       if ( checkSignal(plugin,"getPlugin(QString,QObject*&)" ) )
//         connect(plugin  , SIGNAL(getPlugin(QString, QObject* &)),
//                 this    , SLOT(slotGetPlugin(QString, QObject* &)) ,Qt::DirectConnection );
    }

    //========================================================================================
    // ===          Collect Scripting Information for Plugin      ============================

    QScriptValue scriptInstance = scriptEngine_.newQObject(plugin,
                                                           QScriptEngine::QtOwnership,
                                                           QScriptEngine::ExcludeChildObjects |
                                                           QScriptEngine::ExcludeSuperClassMethods |
                                                           QScriptEngine::ExcludeSuperClassProperties
                                                           );

    // Make plugin available for scripting
    QString scriptingName = info.name.remove(" ").toLower();

    scriptEngine_.globalObject().setProperty(scriptingName, scriptInstance);
    emit log(LOGOUT,"Registered scriping with name :\t " + scriptingName);

    info.rpcName = scriptingName;

    emit log(LOGOUT,"Available scripting functions :");

    QScriptValueIterator it(scriptInstance);
    while (it.hasNext()) {
      it.next();

      /// Skip all signals for function calls
      if ( checkSignal( plugin, it.name().toAscii() ) )
        continue;

      info.rpcFunctions.push_back( it.name() );

      scriptingFunctions_.push_back( scriptingName + "." + it.name() );

//       QScriptValue function = it.value();
//       scriptEngine_.globalObject().setProperty(it.name(), function );

      emit log(LOGOUT,"\t" + it.name());
    }

        //Check if the plugin supports RPC-Interface
    ScriptInterface* scriptPlugin = qobject_cast< ScriptInterface * >(plugin);
    if ( scriptPlugin ) {
      supported = supported + "Scripting ";

      // Create intermediate wrapper class which will mangle the call information
      ScriptingWrapper* newScript = new ScriptingWrapper(info.rpcName);
      scriptingWrappers_.push_back(newScript);

      //========= Part one, Scriptinfos via wrapper to core and than to scipting Plugin ==========

      if ( checkSignal(plugin,"scriptInfo(QString)" ) ) {

        // Plugin to wrapper
        connect(plugin    , SIGNAL( scriptInfo(QString) ),
                newScript , SLOT( slotScriptInfo(QString) ) ,Qt::DirectConnection );

        // wrapper to core
        connect(newScript , SIGNAL( scriptInfo(QString,QString) ),
                this      , SLOT( slotScriptInfo(QString,QString) ));
      }

      // Core to plugins ( normally only one scripting plugin)
      if ( checkSlot(plugin,"slotScriptInfo(QString,QString)") ) {
        connect(this      , SIGNAL(scriptInfo(QString,QString)),
                plugin    , SLOT(slotScriptInfo(QString,QString)));
      }

      //========= Script Execution ==========

      // Plugins to Core
      if ( checkSignal(plugin,"executeScript(QString)") )
        connect(plugin      , SIGNAL(executeScript(QString)),
                this       , SLOT(slotExecuteScript(QString)));

      // Core to plugins ( normally only one scripting plugin)
      if ( checkSlot(plugin,"slotExecuteScript(QString)") )
        connect(this      , SIGNAL(executeScript(QString)),
                plugin       , SLOT(slotExecuteScript(QString)));

      // Core to plugins ( normally only one scripting plugin)
      if ( checkSlot(plugin,"slotExecuteFileScript(QString)") )
        connect(this      , SIGNAL(executeFileScript(QString)),
                plugin       , SLOT(slotExecuteFileScript(QString)));

      //========= Engine  ==========

      // Plugins to Core
      if ( checkSignal(plugin,"getScriptingEngine(QScriptEngine*&)") )
        connect(plugin      , SIGNAL(getScriptingEngine(QScriptEngine*&)),
                this       , SLOT(slotGetScriptingEngine(QScriptEngine*&)));

      // Plugins to Core
      if ( checkSignal(plugin,"getAvailableFunctions(QStringList&)") )
        connect(plugin      , SIGNAL(getAvailableFunctions(QStringList&)),
                this       , SLOT(slotGetAllAvailableFunctions(QStringList&)));

    }

    emit log(LOGOUT,"Supported Interfaces :\t " + supported);


    //========================================================================================
    //========================================================================================

    plugins.push_back(info);


    //tab all dockwidgets together
    if (OpenFlipper::Options::gui())
      coreWidget_->tabDockWidgets( QVector< QDockWidget* >() );

    // Initialize Plugin
    if ( basePlugin ) {
      if ( checkSlot(plugin,"initializePlugin()") )
         QMetaObject::invokeMethod(plugin, "initializePlugin",  Qt::DirectConnection);
    }

    emit log(LOGOUT,"=============================================================================================");
  } else {
    emit log(LOGERR,"Unable to load Plugin :\t " + filename );
    emit log(LOGERR,"Error was : " + loader.errorString() );
    emit log(LOGOUT,"=============================================================================================");
  }



}
