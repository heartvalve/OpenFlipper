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
#include <QScrollArea>

#include <QPluginLoader>
#include "OpenFlipper/BasePlugin/BaseInterface.hh"
#include "OpenFlipper/BasePlugin/KeyInterface.hh"
#include "OpenFlipper/BasePlugin/BackupInterface.hh"
#include "OpenFlipper/BasePlugin/LoggingInterface.hh"
#include "OpenFlipper/BasePlugin/MouseInterface.hh"
#include "OpenFlipper/BasePlugin/PickingInterface.hh"
#include "OpenFlipper/BasePlugin/ToolboxInterface.hh"
#include "OpenFlipper/BasePlugin/OptionsInterface.hh"
#include "OpenFlipper/BasePlugin/ToolbarInterface.hh"
#include "OpenFlipper/BasePlugin/TextureInterface.hh"
#include "OpenFlipper/BasePlugin/MenuInterface.hh"
#include "OpenFlipper/BasePlugin/ContextMenuInterface.hh"
#include "OpenFlipper/BasePlugin/ViewInterface.hh"
#include "OpenFlipper/BasePlugin/LoadSaveInterface.hh"
#include "OpenFlipper/BasePlugin/StatusbarInterface.hh"
#include "OpenFlipper/BasePlugin/INIInterface.hh"
#include "OpenFlipper/BasePlugin/FileInterface.hh"
#include "OpenFlipper/BasePlugin/RPCInterface.hh"
#include "OpenFlipper/BasePlugin/ScriptInterface.hh"
#include "OpenFlipper/BasePlugin/SecurityInterface.hh"

#include "OpenFlipper/INIFile/INIFile.hh"

#include "OpenFlipper/common/GlobalOptions.hh"

#include <QMessageBox>
#include <QtScript/QScriptValueIterator>

#include <ACG/QtWidgets/QtFileDialog.hh>
#include "OpenFlipper/widgets/PluginDialog/PluginDialog.hh"

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
  QDir tempDir = QDir(OpenFlipper::Options::pluginDir());

  QStringList filters;


  filters << "*.dll";
  filters << "*.dylib";
  filters << "*.so";


  // Get all files in the Plugin dir
  QStringList pluginlist = tempDir.entryList(filters,QDir::Files);

  for (int i=0; i < pluginlist.size(); i++)
     pluginlist[i] = tempDir.absoluteFilePath(pluginlist[i]);

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
              emit log(LOGOUT,tr("Additional Plugin from ini file: %1").arg( additionalPlugins[i] ) );
          }
        }
      }

      ini.disconnect();

   } else
      emit log(LOGWARN,tr("Failed to connect to ProgramOptions.ini file: %1").arg( configFiles[fileCount]) );
  }

  pluginlist = iniPlugins << pluginlist;


  // Sort plugins to load FilePlugins first
  QStringList filePlugins;
  QStringList textureControl;
  QStringList otherPlugins;
  //plugin Liste sortieren
  for (int i=0; i < pluginlist.size(); i++)
    if (pluginlist[i].contains("Plugin-File") )
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
    emit log(LOGWARN,tr("Skipping Plugins :\t %1").arg( dontLoadPlugins_[i] ) );

  emit log(LOGOUT,"=============================================================================================");


  // Try to load each file as a plugin
  for ( int i = 0 ; i < pluginlist.size() ; ++i) {

    if ( OpenFlipper::Options::gui() && OpenFlipper::Options::splash() ) {
      splashMessage_ = tr("Loading Plugin %1/%2").arg(i).arg(pluginlist.size()) ;
      splash_->showMessage( splashMessage_ , Qt::AlignBottom | Qt::AlignLeft , Qt::white);
      QApplication::processEvents();
    }

    loadPlugin(pluginlist[i],true);
  }

  splashMessage_ = "";

  emit pluginsInitialized();

  emit log(LOGOUT,tr("Loaded %n Plugin(s)","",plugins.size()) );
}

/** @brief slot for loading Plugins
 */
void Core::slotLoadPlugin(){

  if ( OpenFlipper::Options::nogui() )
    return;


  QString filter;
  if ( OpenFlipper::Options::isWindows() )
    filter = "Plugins (*.dll)";
  else if ( OpenFlipper::Options::isDarwin() )
    filter = "Plugins (*.dylib)";
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

/** @brief slot for showing loaded Plugins
 */
void Core::slotShowPlugins(){

  if ( OpenFlipper::Options::gui() ){

    int ret = 0;

    while (ret == 0){

      PluginDialog* dialog = new PluginDialog(plugins, coreWidget_);

      //connect signals
      connect(dialog, SIGNAL(unloadPlugin(QString)), this, SLOT(unloadPlugin(QString)));
      connect(dialog, SIGNAL(dontLoadPlugins(QStringList)), this, SLOT(dontLoadPlugins(QStringList)));
      connect(dialog, SIGNAL( loadPlugin() ), this, SLOT( slotLoadPlugin() ));

      //if a plugin was deleted/loaded the dialog returns 0 and it needs to be loaded again
      ret = dialog->exec();

      delete dialog;
    }
  }
}

/** @brief Unload a Plugin with given name (slot)
 *  @param name plugin name
 */
void Core::unloadPlugin(QString name){
  for (uint i=0; i < plugins.size(); i++)
    if (plugins[i].rpcName == name){
      if ( checkSlot( plugins[i].plugin , "exit()" ) )
        QMetaObject::invokeMethod(plugins[i].plugin, "exit",  Qt::DirectConnection);

      //remove toolbox widget
      QString name_nospace = name;
      name_nospace.remove(" ");
      if ( coreWidget_->viewModes_[0]->visibleToolboxes.contains(name_nospace) )
        coreWidget_->viewModes_[0]->visibleToolboxes.removeAt(coreWidget_->viewModes_[0]->visibleToolboxes.indexOf(name_nospace));
      for ( uint j = 0 ; j < plugins[i].widgets.size() ; ++j )
        if (plugins[i].widgets[j].second ){
          plugins[i].widgets[j].second->setVisible(false);
          delete plugins[i].widgets[j].second;
        }

      plugins.erase(plugins.begin() + i);

      emit log(LOGOUT,tr("Unloaded Plugin :\t\t %1").arg( name) );

      return;
    }

    log(LOGERR, tr("Unable to unload plugin '%1' (plugin wasn't found)").arg(name));
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
      emit log(LOGOUT,tr("Found Plugin :\t\t %1 at %2.").arg(basePlugin->name()).arg( filename));

      if ( OpenFlipper::Options::gui() && OpenFlipper::Options::splash() ) {
        splashMessage_ = splashMessage_ + " " + basePlugin->name() ;
        splash_->showMessage( splashMessage_ , Qt::AlignBottom | Qt::AlignLeft , Qt::white);
        QApplication::processEvents();
      }

      //Check if plugin is already loaded
      for (uint k=0; k < plugins.size(); k++){

          QString name_nospace =  basePlugin->name();
          name_nospace.remove(" ");

          if (plugins[k].name == name_nospace){
            if (silent || OpenFlipper::Options::nogui() ){ //dont load the plugin
              emit log(LOGWARN, tr("\t\t\t Already loaded from %1").arg( plugins[k].path) );
              emit log(LOGOUT,"=============================================================================================");
              return;
          }else{ //ask the user
            int ret = QMessageBox::question(coreWidget_,
                                            tr("Plugin already loaded"),
                                            tr("A Plugin with the same name was already loaded from %1.\n" 
                                               "You can only load the new plugin if you unload the existing one first.\n\n"
                                               "Do you want to unload the existing plugin first?").arg( plugins[k].path),
                                            QMessageBox::Yes|QMessageBox::No, QMessageBox::No);
            if (ret == QMessageBox::Yes)
              unloadPlugin(plugins[k].name);
            else{
              emit log(LOGWARN, tr("\t\t\t Already loaded from %1.").arg( plugins[k].path));
              emit log(LOGOUT,"=============================================================================================");
              return;
            }
          }
        }
      }

      if ( dontLoadPlugins_.contains(basePlugin->name(), Qt::CaseInsensitive) ) {
        emit log(LOGWARN,tr("OpenFlipper.ini prevented Plugin %1 from being loaded! ").arg( basePlugin->name() ));
        emit log(LOGOUT,"=============================================================================================");
        return;
      }

      //Check if it is a BasePlugin
      SecurityInterface * securePlugin = qobject_cast< SecurityInterface * >(plugin);
      if ( securePlugin ) {
        emit log(LOGINFO,tr("Plugin uses security interface. Trying to authenticate against plugin ..."));

        bool success = false;
        QMetaObject::invokeMethod(plugin,"authenticate", Q_RETURN_ARG( bool , success ) ) ;

        if ( success )
          emit log(LOGINFO,tr("... ok. Loading plugin "));
        else {
          emit log(LOGERR,tr("... failed. Plugin access denied."));
          emit log(LOGOUT,"=============================================================================================");
          return;
        }
      }


      emit log(LOGOUT,tr("Plugin Desciption :\t %1 ").arg( basePlugin->description()) );

      supported = "BaseInterface ";

      info.name          = basePlugin->name();
      info.description   = basePlugin->description();
      info.plugin        = plugin;
      info.path          = filename;

      if ( checkSlot(plugin,"version()") )
        info.version = basePlugin->version();
      else
        info.version = QString::number(-1);

      if ( OpenFlipper::Options::nogui() ) {

        if ( ! checkSlot( plugin , "noguiSupported()" ) ) {
          emit log(LOGWARN,tr("Running in nogui mode which is unsupported by this plugin, skipping") );
          emit log(LOGOUT,"=============================================================================================");
          return;
        }

      }


      // Check for baseInterface of old style!
      if ( checkSignal(plugin,"updated_objects(int)") ) {
        log(LOGERR,tr("Plugin Uses old style updated_objects! Convert to updatedObject!"));
        emit log(LOGOUT,"=============================================================================================");
        return;
      }

      if ( checkSignal(plugin,"update_view()") ) {
        log(LOGERR,tr("Plugin Uses old style update_view! Convert to updateView!"));
        emit log(LOGOUT,"=============================================================================================");
        return;
      }

      if ( checkSignal(plugin,"updateView()") )
        connect(plugin,SIGNAL(updateView()),this,SLOT(updateView()));



      if ( checkSignal(plugin,"updatedObject(int)") )
        connect(plugin,SIGNAL(updatedObject(int)),this,SLOT(slotObjectUpdated(int)), Qt::DirectConnection);

      if ( checkSlot( plugin , "slotObjectUpdated(int)" ) )
        connect(this,SIGNAL(signalObjectUpdated(int)),plugin,SLOT(slotObjectUpdated(int)), Qt::DirectConnection);


      if ( checkSignal(plugin,"objectPropertiesChanged(int)"))
        connect(plugin,SIGNAL(objectPropertiesChanged(int)),this,SLOT(slotObjectPropertiesChanged(int)), Qt::DirectConnection);
      
      if ( checkSlot( plugin , "slotViewChanged()" ) )
        connect(this,SIGNAL(pluginViewChanged()),plugin,SLOT(slotViewChanged()), Qt::DirectConnection);

      if ( checkSlot(plugin,"slotObjectPropertiesChanged(int)"))
        connect(this,SIGNAL(objectPropertiesChanged(int)),plugin,SLOT(slotObjectPropertiesChanged(int)), Qt::DirectConnection);

      if ( checkSignal(plugin,"visibilityChanged()" ) )
        emit log (LOGERR,tr("Signal visibilityChanged() now requires objectid or -1 as argument " ));

      if ( checkSignal(plugin,"visibilityChanged(int)") )
        connect(plugin,SIGNAL(visibilityChanged(int)),this,SLOT(slotVisibilityChanged(int)), Qt::DirectConnection);

      if ( checkSlot(plugin,"slotVisibilityChanged(int)") )
        connect(this,SIGNAL(visibilityChanged(int)),plugin,SLOT(slotVisibilityChanged(int)), Qt::DirectConnection);

      if ( checkSignal(plugin,"activeObjectChanged()" ) )
        emit log (LOGERR,tr("Signal activeObjectChanged() is now objectSelectionChanged( int _objectId ) ") );

      if ( checkSlot(plugin,"slotActiveObjectChanged()" ) )
        emit log (LOGERR,tr("Slot slotActiveObjectChanged() is now slotObjectSelectionChanged( int _objectId ) ") );

      if ( checkSlot(plugin,"slotAllCleared()") )
        connect(this,SIGNAL(allCleared()),plugin,SLOT(slotAllCleared()));


      if ( checkSignal(plugin,"objectSelectionChanged(int)") )
        connect(plugin,SIGNAL(objectSelectionChanged(int)),this,SLOT(slotObjectSelectionChanged(int) ));

      if ( checkSlot( plugin , "slotObjectSelectionChanged(int)" ) )
        connect(this,SIGNAL(objectSelectionChanged(int)),plugin,SLOT(slotObjectSelectionChanged(int) ));


      if ( checkSlot( plugin , "pluginsInitialized()" ) )
        connect(this,SIGNAL(pluginsInitialized()),plugin,SLOT(pluginsInitialized()), Qt::DirectConnection);

      if ( checkSignal(plugin,"setSlotDescription(QString,QString,QStringList,QStringList)") )
        connect(plugin, SIGNAL(setSlotDescription(QString,QString,QStringList,QStringList)),
                this,   SLOT(slotSetSlotDescription(QString,QString,QStringList,QStringList)) );
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

      if ( checkSignal(plugin,"addMenubarAction(QAction*,QString)") )
        connect(plugin      , SIGNAL(addMenubarAction(QAction*,QString)),
                coreWidget_ , SLOT(slotAddMenubarAction(QAction*,QString)),Qt::DirectConnection);
      if ( checkSignal(plugin,"getMenubarMenu (QString,QMenu*&,bool)") )
        connect(plugin      , SIGNAL(getMenubarMenu (QString,QMenu*&,bool)),
                coreWidget_ , SLOT(slotGetMenubarMenu (QString,QMenu*&,bool)),Qt::DirectConnection);
    }

    //Check if the plugin supports ContextMenuInterface
    ContextMenuInterface* contextMenuPlugin = qobject_cast< ContextMenuInterface * >(plugin);
    if ( contextMenuPlugin && OpenFlipper::Options::gui() ) {
      supported = supported + "ContextMenu ";

      if ( checkSignal(plugin,"addContextMenuItem(QAction*,ContextMenuType)") )
        connect(plugin      , SIGNAL(addContextMenuItem(QAction*,ContextMenuType)),
                coreWidget_ , SLOT(slotAddContextItem(QAction*,ContextMenuType)),Qt::DirectConnection);

      if ( checkSignal(plugin,"addContextMenuItem(QAction*,DataType,ContextMenuType)") )
        connect(plugin      , SIGNAL(addContextMenuItem(QAction*,DataType,ContextMenuType)),
                coreWidget_ , SLOT(slotAddContextItem(QAction*,DataType,ContextMenuType)),Qt::DirectConnection);

      if ( checkSlot(plugin,"slotUpdateContextMenu(int)") )
        connect(coreWidget_ , SIGNAL(updateContextMenu(int)),
                plugin      , SLOT(slotUpdateContextMenu(int)),Qt::DirectConnection);

      if ( checkSlot(plugin,"slotUpdateContextMenuNode(int)") )
        connect(coreWidget_ , SIGNAL(updateContextMenuNode(int)),
                plugin      , SLOT(slotUpdateContextMenuNode(int)),Qt::DirectConnection);

      if ( checkSlot(plugin,"slotUpdateContextMenuBackground()") )
        connect(coreWidget_ , SIGNAL(updateContextMenuBackground()),
                plugin      , SLOT(slotUpdateContextMenuBackground()),Qt::DirectConnection);
    }

    //Check if the plugin supports Toolbox-Interface
    ToolboxInterface* toolboxPlugin = qobject_cast< ToolboxInterface * >(plugin);
    if ( toolboxPlugin && OpenFlipper::Options::gui() ) {
      supported = supported + "Toolbox ";

      QWidget* widget = 0;
      if ( toolboxPlugin->initializeToolbox( widget ) ) {

            info.widgets.push_back( std::pair< QString,QWidget* >( info.name , widget) );

            // add widget name to viewMode 'all'
            if ( !viewModes_[0]->visibleToolboxes.contains(info.name) ){
              viewModes_[0]->visibleToolboxes << info.name;
              viewModes_[0]->visibleToolboxes.sort();
            }
      }

      if ( checkSignal(plugin, "defineViewMode(QString,QStringList)"))
        connect(plugin, SIGNAL( defineViewMode(QString, QStringList) ),
                coreWidget_, SLOT( slotAddViewMode(QString, QStringList) ),Qt::DirectConnection );

      if ( checkSignal(plugin, "addToolbox(QString,QWidget*)"))
        connect(plugin, SIGNAL( addToolbox(QString,QWidget*) ),
                this, SLOT( addToolbox(QString,QWidget*) ),Qt::DirectConnection );



    }

    //Check if the plugin supports Options-Interface
    OptionsInterface* optionsPlugin = qobject_cast< OptionsInterface * >(plugin);
    if ( optionsPlugin && OpenFlipper::Options::gui() ) {
      supported = supported + "Options ";

      QWidget* widget = 0;
      if ( optionsPlugin->initializeOptionsWidget( widget ) ) {
            info.optionsWidget = widget;

            if ( checkSlot(plugin,"applyOptions()") )
              connect(coreWidget_ , SIGNAL( applyOptions() ),
                      plugin      , SLOT( applyOptions() ),Qt::DirectConnection);
      }
    }

    //Check if the plugin supports Toolbar-Interface
    ToolbarInterface* toolbarPlugin = qobject_cast< ToolbarInterface * >(plugin);
    if ( toolbarPlugin && OpenFlipper::Options::gui() ) {
      supported = supported + "Toolbars ";

      if ( checkSignal(plugin,"addToolbar(QToolBar*)") )
        connect(plugin,SIGNAL(addToolbar(QToolBar*)),
                coreWidget_,SLOT(slotAddToolbar(QToolBar*)),Qt::DirectConnection);

      if ( checkSignal(plugin,"removeToolbar(QToolBar*)") )
        connect(plugin,SIGNAL(removeToolbar(QToolBar*)),
                coreWidget_,SLOT(slotRemoveToolbar(QToolBar*)),Qt::DirectConnection);

      if ( checkSignal(plugin,"getToolBar(QString,QToolBar*&)") )
        connect(plugin,SIGNAL(getToolBar(QString,QToolBar*&)),
                coreWidget_,SLOT(getToolBar(QString,QToolBar*&)),Qt::DirectConnection);
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
        connect(coreWidget_,SIGNAL(signalPickModeChanged (const std::string &)),
                plugin,SLOT(slotPickModeChanged( const std::string &)));

      if ( checkSignal(plugin,"addPickMode(const std::string)") )
        connect(plugin,SIGNAL(addPickMode( const std::string )),
                this,SLOT(slotAddPickMode( const std::string )),Qt::DirectConnection);

      if ( checkSignal(plugin,"addHiddenPickMode(const std::string)") )
        connect(plugin,SIGNAL(addHiddenPickMode( const std::string )),
                this,SLOT(slotAddHiddenPickMode( const std::string )),Qt::DirectConnection);

      if ( checkSignal(plugin,"setPickModeCursor(const std::string,QCursor)") )
        for ( uint i = 0 ; i < OpenFlipper::Options::examinerWidgets() ; ++i )
          connect(plugin,SIGNAL(setPickModeCursor( const std::string ,QCursor)),
                  coreWidget_,SLOT(setPickModeCursor( const std::string ,QCursor)),Qt::DirectConnection);

      if ( checkSignal(plugin,"setPickModeMouseTracking(const std::string,bool)") )
        for ( uint i = 0 ; i < OpenFlipper::Options::examinerWidgets() ; ++i )
          connect(plugin,SIGNAL(setPickModeMouseTracking( const std::string ,bool)),
                  coreWidget_,SLOT(setPickModeMouseTracking( const std::string ,bool)),Qt::DirectConnection);

      if ( checkSignal(plugin,"setPickModeToolbar(const std::string,QToolBar*)") )
        connect(plugin,SIGNAL(setPickModeToolbar (const std::string, QToolBar*)),
                coreWidget_,SLOT(setPickModeToolbar (const std::string, QToolBar*)),Qt::DirectConnection);

      if ( checkSignal(plugin,"removePickModeToolbar(const std::string)") )
        connect(plugin,SIGNAL(removePickModeToolbar( const std::string)),
                coreWidget_,SLOT(removePickModeToolbar( const std::string)),Qt::DirectConnection);

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

      if ( checkSlot( plugin , "saveOnExit(INIFile&)" ) )
        connect(this    , SIGNAL(saveOnExit( INIFile& )),
                plugin  , SLOT( saveOnExit( INIFile& ) ),Qt::DirectConnection);

      if ( checkSlot( plugin , "loadIniFileOptions(INIFile&)" ) )
        connect(this    , SIGNAL(iniLoadOptions( INIFile& )),
                plugin  , SLOT( loadIniFileOptions( INIFile& ) ),Qt::DirectConnection);

      if ( checkSlot( plugin , "loadIniFileOptionsLast(INIFile&)" ) )
        connect(this    , SIGNAL(iniLoadOptionsLast( INIFile& )),
                plugin  , SLOT( loadIniFileOptionsLast( INIFile& ) ),Qt::DirectConnection);
    }

    //Check if the plugin supports Texture-Interface
    TextureInterface* texturePlugin = qobject_cast< TextureInterface * >(plugin);
    if ( texturePlugin && OpenFlipper::Options::gui() ) {
      supported = supported + "Textures ";

      if ( checkSignal(plugin,"addTexture(QString,QString,uint,int)") )
        connect(plugin , SIGNAL(addTexture( QString , QString , uint , int )),
                this   , SLOT(slotAddTexture(QString, QString, uint, int)),Qt::DirectConnection);

      if ( checkSlot( plugin , "slotTextureAdded(QString,QString,uint,int)" ) )
        connect(this   , SIGNAL(addTexture(QString,QString, uint, int)),
                plugin , SLOT(slotTextureAdded(QString,QString, uint, int)),Qt::DirectConnection);

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

      if ( checkSignal(plugin,"setTextureMode(QString,QString,int)") )
        connect(plugin , SIGNAL(setTextureMode(QString, QString, int )),
                this   , SLOT(slotSetTextureMode(QString, QString, int )),Qt::DirectConnection );

      if ( checkSlot( plugin , "slotSetTextureMode(QString,QString,int)" ) )
        connect(this   , SIGNAL(setTextureMode(QString, QString, int )),
                plugin , SLOT(slotSetTextureMode(QString, QString, int )),Qt::DirectConnection );

      if ( checkSignal(plugin,"setTextureMode(QString,QString)") )
        connect(plugin , SIGNAL(setTextureMode(QString  ,QString )),
                this   , SLOT(slotSetTextureMode(QString  ,QString )),Qt::DirectConnection );

      if ( checkSlot( plugin , "slotSetTextureMode(QString,QString)" ) )
        connect(this   , SIGNAL(setTextureMode(QString  ,QString )),
                plugin , SLOT(slotSetTextureMode(QString  ,QString )),Qt::DirectConnection );

      if ( checkSignal(plugin,"switchTexture(QString,int)") )
        connect(plugin , SIGNAL(switchTexture(QString, int )),
                this   , SLOT(slotSwitchTexture(QString, int )),Qt::DirectConnection);

      if ( checkSlot( plugin , "slotSwitchTexture(QString,int)" ) )
        connect(this   , SIGNAL(switchTexture(QString, int )),
                plugin , SLOT(slotSwitchTexture(QString, int )),Qt::DirectConnection);

      if ( checkSignal(plugin,"switchTexture(QString)") )
        connect(plugin , SIGNAL(switchTexture(QString )),
                this   , SLOT(slotSwitchTexture(QString )),Qt::DirectConnection);

      if ( checkSlot( plugin , "slotSwitchTexture(QString)" ) )
        connect(this   , SIGNAL(switchTexture(QString )),
                plugin , SLOT(slotSwitchTexture(QString )),Qt::DirectConnection);



      if ( checkSignal( plugin , "textureChangeImage(QString,QImage&,int)" ) )
        connect(plugin   , SIGNAL(textureChangeImage(QString,QImage&,int)),
                this , SLOT(slotTextureChangeImage(QString,QImage&,int)),Qt::DirectConnection);

      if ( checkSlot( plugin , "slotTextureChangeImage(QString,QImage&,int)" ) )
        connect(this   , SIGNAL(textureChangeImage(QString,QImage&,int)),
                plugin , SLOT(slotTextureChangeImage(QString,QImage&,int)),Qt::DirectConnection);

      if ( checkSignal( plugin , "textureChangeImage(QString,QImage&)" ) )
        connect(plugin   , SIGNAL(textureChangeImage(QString,QImage&)),
                this , SLOT(slotTextureChangeImage(QString,QImage&)),Qt::DirectConnection);

      if ( checkSlot( plugin , "slotTextureChangeImage(QString,QImage&)" ) )
        connect(this   , SIGNAL(textureChangeImage(QString,QImage&)),
                plugin , SLOT(slotTextureChangeImage(QString,QImage&)),Qt::DirectConnection);

      if ( checkSignal( plugin , "addMultiTexture(QString,QString,QString,int,int&)" ) )
        connect(plugin   , SIGNAL(addMultiTexture(QString,QString,QString,int,int&) ),
                this , SLOT(slotMultiTextureAdded(QString,QString,QString,int,int&) ),Qt::DirectConnection);

      if ( checkSlot( plugin , "slotMultiTextureAdded( QString,QString,QString,int,int&)" ) )
        connect(this   , SIGNAL(addMultiTexture(QString,QString,QString,int,int&) ),
                plugin , SLOT(slotMultiTextureAdded( QString,QString,QString,int,int&) ),Qt::DirectConnection);

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
                this   , SLOT( saveObject(int,QString) ), Qt::DirectConnection);

      if ( checkSlot( plugin , "fileOpened(int)" ) )
        connect(this   , SIGNAL( openedFile( int) ) ,
                plugin , SLOT( fileOpened( int ) ),Qt::DirectConnection);

      if ( checkSignal(plugin,"openedFile(int)" ) )
        connect(plugin , SIGNAL( openedFile( int ) ) ,
                this   , SLOT( slotObjectOpened ( int ) ),Qt::DirectConnection);

      if ( checkSignal(plugin,"addEmptyObject(DataType,int&)" ) )
        connect(plugin , SIGNAL( addEmptyObject( DataType, int& )) ,
                this   , SLOT( slotAddEmptyObject( DataType, int&) ),Qt::DirectConnection);

      if ( checkSignal(plugin,"copyObject(int,int&)" ) )
        connect(plugin , SIGNAL( copyObject( int, int& )) ,
                this   , SLOT( slotCopyObject( int, int&) ),Qt::DirectConnection);

      if ( checkSignal(plugin,"emptyObjectAdded(int)" ) )
        connect(plugin , SIGNAL( emptyObjectAdded( int ) ) ,
                this   , SLOT( slotEmptyObjectAdded ( int ) ),Qt::DirectConnection);

      if ( checkSlot(plugin,"addedEmptyObject(int)" ) )
        connect(this ,   SIGNAL( emptyObjectAdded( int ) ) ,
                plugin   , SLOT( addedEmptyObject( int ) ),Qt::DirectConnection);

      if ( checkSignal(plugin,"getAllFilters(QStringList&)" ) )
        connect(plugin , SIGNAL( getAllFilters( QStringList& ) ) ,
                this   , SLOT( slotGetAllFilters ( QStringList& ) ),Qt::DirectConnection);

      if ( checkSignal(plugin,"deleteObject(int)" ) )
        connect(plugin , SIGNAL( deleteObject( int ) ) ,
                this   , SLOT( slotDeleteObject( int ) ),Qt::DirectConnection);

      if ( checkSignal(plugin,"deleteAllObjects()" ) )
        connect(plugin , SIGNAL( deleteAllObjects() ) ,
                this   , SLOT( slotDeleteAllObjects() ),Qt::DirectConnection);

      if ( checkSlot(plugin,"objectDeleted(int)" ) )
        connect(this ,   SIGNAL( objectDeleted( int ) ) ,
                plugin   , SLOT( objectDeleted( int ) ),Qt::DirectConnection);

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

    info.rpcName = scriptingName;

    QScriptValueIterator it(scriptInstance);
    while (it.hasNext()) {
      it.next();

      /// Skip all signals for function calls
      if ( checkSignal( plugin, it.name().toAscii() ) )
        continue;

      info.rpcFunctions.push_back( it.name() );

      scriptingFunctions_.push_back( scriptingName + "." + it.name() );

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

      // Function descriptions
      if ( checkSignal(plugin,"getDescription(QString,QString&,QStringList&,QStringList&)") )
        connect(plugin    , SIGNAL( getDescription(QString,QString&,QStringList&,QStringList&) ),
                this      , SLOT( slotGetDescription(QString,QString&,QStringList&,QStringList&) ));

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

    //========================================================================================
    //========================================================================================

    plugins.push_back(info);

    // Initialize Plugin
    if ( basePlugin ) {
      if ( checkSlot(plugin,"initializePlugin()") )
         QMetaObject::invokeMethod(plugin, "initializePlugin",  Qt::DirectConnection);
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
    }

    emit log(LOGOUT,"=============================================================================================");
  } else {
    emit log(LOGERR,tr("Unable to load Plugin :\t %1").arg( filename ) );
    emit log(LOGERR,tr("Error was : ") + loader.errorString() );
    emit log(LOGOUT,"=============================================================================================");
  }



}
