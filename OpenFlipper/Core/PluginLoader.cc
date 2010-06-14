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
 *   $Revision$                                                       *
 *   $Author$                                                       *
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
#include "OpenFlipper/BasePlugin/ProcessInterface.hh"
#include "OpenFlipper/BasePlugin/ViewInterface.hh"
#include "OpenFlipper/BasePlugin/ViewModeInterface.hh"
#include "OpenFlipper/BasePlugin/LoadSaveInterface.hh"
#include "OpenFlipper/BasePlugin/StatusbarInterface.hh"
#include "OpenFlipper/BasePlugin/INIInterface.hh"
#include "OpenFlipper/BasePlugin/FileInterface.hh"
#include "OpenFlipper/BasePlugin/RPCInterface.hh"
#include "OpenFlipper/BasePlugin/ScriptInterface.hh"
#include "OpenFlipper/BasePlugin/SecurityInterface.hh"
#include "OpenFlipper/BasePlugin/TypeInterface.hh"

#include "OpenFlipper/INIFile/INIFile.hh"

#include "OpenFlipper/common/GlobalOptions.hh"

#include <QMessageBox>
#include <QtScript/QScriptValueIterator>

#include <ACG/QtWidgets/QtFileDialog.hh>
#include "OpenFlipper/widgets/PluginDialog/PluginDialog.hh"

class PreloadThread : public QThread
{
  public: 
    
    /** \brief Preload thread constructor
    *
    * This constructor initializes the preloading thread.
    * @param _fileName Filename of the Plugin to be loaded.
    */
    PreloadThread(QString _fileName):
      filename_(_fileName) {      
    }
  
  public:
    
    /** \brief get an instance of the loaded plugin
    *
    * This function returns an instance of the loaded plugin or 0 if something went wrong.
    * This mehod has to be called from the main thread as the plugins have to stay in the
    * core thread of the application.
    */
    QObject* getInstance() { return loader_.instance(); }
    
    /** \brief function to get the filename used in this preloader */
    QString filename() { return filename_; }
    
    /// If an error occured, this function will return the error message
    QString getError() { return loader_.errorString(); }
    
    /** \brief preload function
    *
    * This function is used in the thread to preload a plugin. The name is given in the constructor.
    */
    void run() {
      loader_.setFileName( filename_ );
      loader_.load();
    }
    
  private:
    QString filename_;
    QPluginLoader loader_;
};


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

  // Possible Plugin extensions
  QStringList filter;
  if ( OpenFlipper::Options::isWindows() )
    filter << "*.dll";
  else if ( OpenFlipper::Options::isDarwin() )
    filter << "*.so";
//    filter << "*.dylib";
  else
    filter << "*.so";
  
  // Get all files in the Plugin dir
  QStringList pluginlist = tempDir.entryList(filter,QDir::Files);

  // Convert local file path to absolute path
  for (int i=0; i < pluginlist.size(); i++) {
     pluginlist[i] = tempDir.absoluteFilePath(pluginlist[i]);
  }

  // get all Plugin Names which will not be loaded
  QStringList dontLoadPlugins = OpenFlipperSettings().value("PluginControl/DontLoadNames").toStringList();
  
  // Output info about additional plugins
  for ( int i = 0 ; i < dontLoadPlugins.size(); ++i) 
    emit log(LOGOUT,tr("dontLoadPlugins Plugin from ini file: %1").arg( dontLoadPlugins[i] ) );
  
  // get all Plugins which should be loaded in addition to the standard plugins 
  QStringList additionalPlugins = OpenFlipperSettings().value("PluginControl/AdditionalPlugins").toStringList();
  
  // Output info about additional plugins
  for ( int i = 0 ; i < additionalPlugins.size(); ++i) {
    emit log(LOGOUT,tr("Additional Plugin from file: %1").arg( additionalPlugins[i] ) );
  }
  
  // Prepend the additional Plugins to the plugin list
  pluginlist = additionalPlugins << pluginlist;
  
  // Sort plugins to load FilePlugins first
  QStringList typePlugins;
  QStringList filePlugins;
  QStringList textureControl;
  QStringList otherPlugins;
  //plugin Liste sortieren
  for (int i=0; i < pluginlist.size(); i++)
    if (pluginlist[i].contains("Plugin-Type") )
      typePlugins.push_back(pluginlist[i]);
    else if (pluginlist[i].contains("Plugin-File") )
      filePlugins.push_back(pluginlist[i]);
    else if (pluginlist[i].contains("TextureControl"))
      textureControl.push_back(pluginlist[i]);
    else
      otherPlugins.push_back(pluginlist[i]);

  // This is the order in which plugins have to be loaded:
  // First load all type Plugins to register the dataTypes to the core
  // Than load the file plugins to load objects 
  // Next is textureControl to control texture based properties
  // Than load everything else.
  pluginlist = typePlugins << filePlugins << textureControl << otherPlugins;

  for ( int i = 0 ; i < dontLoadPlugins.size(); ++i )
    emit log(LOGWARN,tr("Skipping Plugins :\t %1").arg( dontLoadPlugins[i] ) );

  emit log(LOGOUT,"================================================================================");

  QTime time;
  
  time.start();
  
  std::vector< PreloadThread* > loaderThreads;
  
  // Try to load each file as a plugin in a separate thread (only load them in seperate thread. Instance will be created in main thread)
  for ( int i = 0 ; i < pluginlist.size() ; ++i) {
    
    // create loader thread
    PreloadThread* load = new PreloadThread(pluginlist[i]);
    
    // remember loader thread
    loaderThreads.push_back(load);
    
    // start loader thread
    load->start();
  }
  
  // Check each thread for a valid plugin
  for ( uint i = 0 ; i < loaderThreads.size() ; ++i) {
    
    if ( OpenFlipper::Options::gui() && OpenFlipperSettings().value("Core/Gui/splash",true).toBool() ) {
      splashMessage_ = tr("Loading Plugin %1/%2").arg(i).arg(loaderThreads.size()) ;
      splash_->showMessage( splashMessage_ , Qt::AlignBottom | Qt::AlignLeft , Qt::white);
      QApplication::processEvents();
    }
    
    loaderThreads[i]->wait();
    
    if ( loaderThreads[i]->getInstance() != 0 ) {
      loadPlugin(loaderThreads[i]->filename(),true,loaderThreads[i]->getInstance());
    } else {
      emit log(LOGERR,tr("Unable to load Plugin :\t %1").arg( loaderThreads[i]->filename() ) );
      emit log(LOGERR,tr("Error was : ") + loaderThreads[i]->getError() );
      emit log(LOGOUT,"================================================================================");       
    }
    
    delete loaderThreads[i];
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

  // Setup filters for possible plugin extensions
  QString filter;
  if ( OpenFlipper::Options::isWindows() )
    filter = "Plugins (*.dll)";
  else if ( OpenFlipper::Options::isDarwin() )
    // filter = "Plugins (*.dylib)";
    filter = "Plugins (*.so)";
  else
    filter = "Plugins (*.so)";

  // Ask the user to select the file to load
  QString filename = ACG::getOpenFileName(coreWidget_,tr("Load Plugin"),filter, OpenFlipperSettings().value("Core/CurrentDir").toString() );

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

  // Ask if the plugin is on the block list
  QStringList dontLoadPlugins = OpenFlipperSettings().value("PluginControl/DontLoadNames").toStringList();
  if (dontLoadPlugins.contains(name)){
      int ret = QMessageBox::question(0, tr("Plugin Loading Prevention"),
                   tr("OpenFlipper is currently configured to prevent loading this plugin.\n"
                      "Do you want to enable this plugin permanently?"),
                   QMessageBox::Yes | QMessageBox::No,
                   QMessageBox::Yes);
      if (ret == QMessageBox::Yes) {
        dontLoadPlugins.removeAll(name);
        OpenFlipperSettings().setValue("PluginControl/DontLoadNames",dontLoadPlugins);
      } else
         return;
  }
  
  // check if the plugin is not on the additional plugin list
  QStringList additionalPlugins = OpenFlipperSettings().value("PluginControl/AdditionalPlugins").toStringList();
  if (!additionalPlugins.contains(name)){
    int ret = QMessageBox::question(0, tr("Plugin Loading ..."),
                                       tr("Should OpenFlipper load this plugin on next startup?"),
                                    QMessageBox::Yes | QMessageBox::No,
                                    QMessageBox::Yes);
    if (ret == QMessageBox::Yes) {
      additionalPlugins << filename;
      std::cerr << "Added: " << filename.toStdString() << std::endl;
      OpenFlipperSettings().setValue("PluginControl/AdditionalPlugins",additionalPlugins);
    }
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
      for ( uint j = 0 ; j < plugins[i].toolboxWidgets.size() ; ++j )
        if (plugins[i].toolboxWidgets[j].second ){
          plugins[i].toolboxWidgets[j].second->setVisible(false);
          delete plugins[i].toolboxWidgets[j].second;
        }

      plugins.erase(plugins.begin() + i);

      emit log(LOGOUT,tr("Unloaded Plugin :\t\t %1").arg( name) );

      return;
    }

    log(LOGERR, tr("Unable to unload plugin '%1' (plugin wasn't found)").arg(name));
}

/** @brief Load a Plugin with given filename
 *  @param filename complete path + filename of the plugin
 *  @param silent if 'true': user isn't asked what to do if a plugin is already loaded
 *  @param _plugin You can provide a preloaded plugin here. If this is 0 the filename will be used to load the plugin.
 */
void Core::loadPlugin(QString filename, bool silent, QObject* _plugin){

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

  // This will be the reference to our plugin
  QObject *plugin = 0;

  // Try to open the file if we did not get a plugin,
  // Otherwise use the supplied plugin pointer
  if ( _plugin == 0 ) {
    QPluginLoader loader( filename );
    plugin = loader.instance();  
    
    if ( !plugin) {
      emit log(LOGERR,tr("Unable to load Plugin :\t %1").arg( filename ) );
      emit log(LOGERR,tr("Error was : ") + loader.errorString() );
      emit log(LOGOUT,"================================================================================");       
    }
    
  } else {
      plugin = _plugin;
  }
  
  // Check if a plugin has been loaded
  PluginInfo info;
  QString supported;

  emit log(LOGOUT,tr("Location : \t %1").arg(filename) );
  
  // Check if it is a BasePlugin
  BaseInterface* basePlugin = qobject_cast< BaseInterface * >(plugin);
  if ( basePlugin ) {
    emit log(LOGOUT,tr("Found Plugin : \t %1").arg(basePlugin->name()) );
    emit log(LOGOUT,tr("Location : \t %2").arg(filename) );

    if ( OpenFlipper::Options::gui() && OpenFlipperSettings().value("Core/Gui/splash",true).toBool() ) {
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
            emit log(LOGOUT,"================================================================================");
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
            emit log(LOGOUT,"================================================================================");
            return;
          }
        }
      }
    }

    QStringList dontLoadPlugins = OpenFlipperSettings().value("PluginControl/DontLoadNames").toStringList();
    
    if ( dontLoadPlugins.contains(basePlugin->name(), Qt::CaseInsensitive) ) {
      emit log(LOGWARN,tr("OpenFlipper.ini prevented Plugin %1 from being loaded! ").arg( basePlugin->name() ));
      emit log(LOGOUT,"================================================================================");
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
        emit log(LOGOUT,"================================================================================");
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
        emit log(LOGOUT,"================================================================================");
        return;
      }

    }


    // Check for baseInterface of old style!
    if ( checkSignal(plugin,"updated_objects(int)") ) {
      log(LOGERR,tr("Plugin Uses old style updated_objects! Convert to updatedObject!"));
      emit log(LOGOUT,"================================================================================");
      return;
    }

    if ( checkSignal(plugin,"update_view()") ) {
      log(LOGERR,tr("Plugin Uses old style update_view! Convert to updateView!"));
      emit log(LOGOUT,"================================================================================");
      return;
    }

    if ( checkSignal(plugin,"updateView()") )
      connect(plugin,SIGNAL(updateView()),this,SLOT(updateView()));


    if ( checkSignal(plugin,"updatedObject(int)") && checkSignal(plugin,"updatedObject(int,const UpdateType)") ){
      
      log(LOGERR,tr("Plugin uses deprecated and(!) new updatedObject. Only new updatedObject will be active."));
      connect(plugin,SIGNAL(updatedObject(int,const UpdateType)),this,SLOT(slotObjectUpdated(int,const UpdateType)), Qt::DirectConnection);
      
    } else {

      if ( checkSignal(plugin,"updatedObject(int)") ){
        log(LOGWARN,tr("Plugin uses deprecated updatedObject."));
        connect(plugin,SIGNAL(updatedObject(int)),this,SLOT(slotObjectUpdated(int)), Qt::DirectConnection);
      }

      if ( checkSignal(plugin,"updatedObject(int,const UpdateType)") )
        connect(plugin,SIGNAL(updatedObject(int,const UpdateType)),this,SLOT(slotObjectUpdated(int,const UpdateType)), Qt::DirectConnection);
    }

    if ( checkSlot( plugin , "slotObjectUpdated(int)" ) && checkSlot( plugin , "slotObjectUpdated(int,const UpdateType)" ) ){
      
      log(LOGERR,tr("Plugin uses deprecated and(!) new slotObjectUpdated. Only new slotObjectUpdated will be active."));
      connect(this,SIGNAL(signalObjectUpdated(int,const UpdateType)),plugin,SLOT(slotObjectUpdated(int,const UpdateType)), Qt::DirectConnection);
    
    } else {

      if ( checkSlot( plugin , "slotObjectUpdated(int)" ) ){
        log(LOGWARN,tr("Plugin uses deprecated slotObjectUpdated."));
        connect(this,SIGNAL(signalObjectUpdated(int)),plugin,SLOT(slotObjectUpdated(int)), Qt::DirectConnection);
      }

      if ( checkSlot( plugin , "slotObjectUpdated(int,const UpdateType)" ) )
        connect(this,SIGNAL(signalObjectUpdated(int,const UpdateType)),plugin,SLOT(slotObjectUpdated(int,const UpdateType)), Qt::DirectConnection);
    }

    if ( checkSignal(plugin,"objectPropertiesChanged(int)")) {
      emit log (LOGERR,tr("Signal objectPropertiesChanged(int) is deprecated. " ));
      emit log (LOGERR,tr("The signal will be automatically emitted by the object that has been changed and the core will deliver it to the plugins!. "));
      emit log (LOGERR,tr("Please remove this signal from your plugins!. "));
    }
    
    if ( checkSlot( plugin , "slotViewChanged()" ) )
      connect(this,SIGNAL(pluginViewChanged()),plugin,SLOT(slotViewChanged()), Qt::DirectConnection);

    if ( checkSlot(plugin,"slotObjectPropertiesChanged(int)"))
      connect(this,SIGNAL(objectPropertiesChanged(int)),plugin,SLOT(slotObjectPropertiesChanged(int)), Qt::DirectConnection);

    if ( checkSignal(plugin,"visibilityChanged()" ) )
      emit log (LOGERR,tr("Signal visibilityChanged() now requires objectid or -1 as argument " ));

    if ( checkSignal(plugin,"visibilityChanged(int)") ) {
      emit log (LOGERR,tr("Signal visibilityChanged(int) is deprecated! " ));
      emit log (LOGERR,tr("If an object changes its visibility, it will call the required functions automatically." ));
      emit log (LOGERR,tr("If you change a scenegraph node, call nodeVisibilityChanged(int). See docu of this function for details." ));
    }
    
    if ( checkSignal(plugin,"nodeVisibilityChanged(int)") )
      connect(plugin,SIGNAL(nodeVisibilityChanged(int)),this,SLOT(slotVisibilityChanged(int)), Qt::DirectConnection);
    

    if ( checkSlot(plugin,"slotVisibilityChanged(int)") )
      connect(this,SIGNAL(visibilityChanged(int)),plugin,SLOT(slotVisibilityChanged(int)), Qt::DirectConnection);

    if ( checkSignal(plugin,"activeObjectChanged()" ) )
      emit log (LOGERR,tr("Signal activeObjectChanged() is now objectSelectionChanged( int _objectId ) ") );

    if ( checkSlot(plugin,"slotActiveObjectChanged()" ) )
      emit log (LOGERR,tr("Slot slotActiveObjectChanged() is now slotObjectSelectionChanged( int _objectId ) ") );

    if ( checkSlot(plugin,"slotAllCleared()") )
      connect(this,SIGNAL(allCleared()),plugin,SLOT(slotAllCleared()));


    if ( checkSignal(plugin,"objectSelectionChanged(int)") ) {
      emit log (LOGERR,tr("Signal objectSelectionChanged(in) is deprecated!") );
      emit log (LOGERR,tr("If the selection for an object is changed, the core will emit the required signals itself!") );
    }

    if ( checkSlot( plugin , "slotObjectSelectionChanged(int)" ) )
      connect(this,SIGNAL(objectSelectionChanged(int)),plugin,SLOT(slotObjectSelectionChanged(int) ), Qt::DirectConnection);


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


    if ( checkSignal(plugin, "addToolbox(QString,QWidget*)"))
      connect(plugin, SIGNAL( addToolbox(QString,QWidget*) ),
              this, SLOT( addToolbox(QString,QWidget*) ),Qt::DirectConnection );



  }
  
  //Check if the plugin supports ViewMode-Interface
  ViewModeInterface* viewModePlugin = qobject_cast< ViewModeInterface * >(plugin);
  if ( viewModePlugin && OpenFlipper::Options::gui() ) {
    supported = supported + "ViewMode ";
    
    if ( checkSignal(plugin, "defineViewModeToolboxes(QString,QStringList)"))
      connect(plugin, SIGNAL( defineViewModeToolboxes(QString, QStringList) ),
              coreWidget_, SLOT( slotAddViewModeToolboxes(QString, QStringList) ),Qt::DirectConnection );
              
    if ( checkSignal(plugin, "defineViewModeToolbars(QString,QStringList)"))
      connect(plugin, SIGNAL( defineViewModeToolbars(QString, QStringList) ),
              coreWidget_, SLOT( slotAddViewModeToolbars(QString, QStringList) ),Qt::DirectConnection );    
              
    if ( checkSignal(plugin, "defineViewModeContextMenus(QString,QStringList)"))
      connect(plugin, SIGNAL( defineViewModeContextMenus(QString, QStringList) ),
              coreWidget_, SLOT( slotAddViewModeContextMenus(QString, QStringList) ),Qt::DirectConnection );                
              
    if ( checkSignal(plugin, "defineViewModeIcon(QString,QString)"))
      connect(plugin, SIGNAL( defineViewModeIcon(QString, QString) ),
              coreWidget_, SLOT( slotSetViewModeIcon(QString, QString) ),Qt::DirectConnection );                          
              
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
              
    if ( checkSignal(plugin,"addWidgetToStatusbar(QWidget*)") )
      connect(plugin,SIGNAL(addWidgetToStatusbar(QWidget*)), coreWidget_,SLOT(addWidgetToStatusbar(QWidget*)));
  }

  //Check if the plugin supports Key-Interface
  KeyInterface* keyPlugin = qobject_cast< KeyInterface * >(plugin);
  if ( keyPlugin && OpenFlipper::Options::gui() ) {
    supported = supported + "KeyboardEvents ";

    if ( checkSignal(plugin,"registerKey(int,Qt::KeyboardModifiers,QString,bool)") )
      connect(plugin,SIGNAL( registerKey(int, Qt::KeyboardModifiers, QString, bool) ),
              coreWidget_,SLOT(slotRegisterKey(int, Qt::KeyboardModifiers, QString, bool)) );
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
              this   , SLOT(slotUpdateTexture(QString , int)),Qt::DirectConnection);

    if ( checkSlot( plugin , "slotUpdateTexture(QString,int)" ) )
      connect(this   , SIGNAL(updateTexture(QString ,int)),
              plugin , SLOT(slotUpdateTexture(QString,int )),Qt::DirectConnection);

    if ( checkSignal(plugin,"updateAllTextures()") )
      connect(plugin , SIGNAL(updateAllTextures()),
              this   , SLOT(slotUpdateAllTextures()));

    if ( checkSlot( plugin , "slotUpdateAllTextures()" ) )
      connect(this   , SIGNAL(updateAllTextures()),
              plugin , SLOT(slotUpdateAllTextures()));

    if ( checkSignal(plugin,"updatedTextures(QString,int)") )
      connect(plugin , SIGNAL(updatedTextures( QString , int )),
              this   , SLOT(slotTextureUpdated( QString, int ) ),Qt::DirectConnection);

    if ( checkSlot( plugin , "slotTextureUpdated(QString,int)" ) )
      connect(this   , SIGNAL(updatedTextures( QString , int )),
              plugin , SLOT(slotTextureUpdated( QString, int ) ),Qt::DirectConnection);

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

    if ( checkSignal( plugin , "textureGetImage(QString,QImage&,int)" ) )
      connect(plugin   , SIGNAL(textureGetImage(QString,QImage&,int)),
              this ,   SLOT(slotTextureGetImage(QString,QImage&,int)),Qt::DirectConnection);

    if ( checkSlot( plugin , "slotTextureGetImage(QString,QImage&,int)" ) )
      connect(this   , SIGNAL(textureGetImage(QString,QImage&,int)),
              plugin , SLOT(slotTextureGetImage(QString,QImage&,int)),Qt::DirectConnection);

    if ( checkSignal( plugin , "textureGetImage(QString,QImage&)" ) )
      connect(plugin   , SIGNAL(textureGetImage(QString,QImage&)),
              this ,   SLOT(slotTextureGetImage(QString,QImage&)),Qt::DirectConnection);

    if ( checkSlot( plugin , "slotTextureGetImage(QString,QImage&)" ) )
      connect(this   , SIGNAL(textureGetImage(QString,QImage&)),
              plugin , SLOT(slotTextureGetImage(QString,QImage&)),Qt::DirectConnection);

    if ( checkSignal( plugin , "textureIndex(QString,int,int&)" ) )
      connect(plugin   , SIGNAL(textureIndex(QString,int,int&)),
              this ,   SLOT(slotTextureIndex(QString,int,int&)),Qt::DirectConnection);

    if ( checkSlot( plugin , "slotTextureIndex(QString,int,int&)" ) )
      connect(this   , SIGNAL(textureIndex(QString,int,int&)),
              plugin , SLOT(slotTextureIndex(QString,int,int&)),Qt::DirectConnection);

    if ( checkSignal( plugin , "textureName(int,int,QString&)" ) )
      connect(plugin   , SIGNAL(textureName(int,int,QString&)),
              this ,   SLOT(slotTextureName(int,int,QString&)),Qt::DirectConnection);

    if ( checkSlot( plugin , "slotTextureName(int,int,QString&)" ) )
      connect(this   , SIGNAL(textureName(int,int,QString&)),
              plugin , SLOT(slotTextureName(int,int,QString&)),Qt::DirectConnection);
              
    if ( checkSignal( plugin , "getCurrentTexture(int,QString&)" ) )
      connect(plugin   , SIGNAL(getCurrentTexture(int,QString&)),
              this ,   SLOT(slotGetCurrentTexture(int,QString&)),Qt::DirectConnection);

    if ( checkSlot( plugin , "slotGetCurrentTexture(int,QString&)" ) )
      connect(this   , SIGNAL(getCurrentTexture(int,QString&)),
              plugin , SLOT(slotGetCurrentTexture(int,QString&)),Qt::DirectConnection);

    if ( checkSignal( plugin , "getSubTextures(int,QString,QStringList&)" ) )
      connect(plugin   , SIGNAL(getSubTextures(int,QString,QStringList&)),
              this ,   SLOT(slotGetSubTextures(int,QString,QStringList&)),Qt::DirectConnection);

    if ( checkSlot( plugin , "slotGetSubTextures(int,QString,QStringList&)" ) )
      connect(this   , SIGNAL(getSubTextures(int,QString,QStringList&)),
              plugin , SLOT(slotGetSubTextures(int,QString,QStringList&)),Qt::DirectConnection);
  }
              
  //Check if the plugin supports Backup-Interface
  BackupInterface* backupPlugin = qobject_cast< BackupInterface * >(plugin);
  if ( backupPlugin ) {
    supported = supported + "Backups ";

    // Incoming Signal that a backup should be created 
    // send to local slot generating backup id and delivers to all other plugins
    if ( checkSignal( plugin , "createBackup(int,QString,int&)" ) ) {
      connect(plugin , SIGNAL(createBackup( int , QString , int&)) ,
              this   , SLOT(slotBackup( int , QString , int&)),Qt::DirectConnection );
    }
    
    // send to local slot generating backup id and delivers to all other plugins
    if ( checkSignal( plugin , "createBackup(int,QString)" ) ) {
      connect(plugin , SIGNAL(createBackup( int , QString)) ,
              this   , SLOT(slotBackup( int , QString)),Qt::DirectConnection );
    }
    
    // Signal send from core to plugins that they should create a backup
    if ( checkSlot( plugin , "slotBackup(int,QString,int)" ) ) {
      connect(this   , SIGNAL(createBackup(int,QString,int)) ,
              plugin , SLOT( slotBackup(int,QString,int) ),Qt::DirectConnection);
    }
    
    // Signal send from plugin to core that a backup should be made persistent
    if ( checkSignal( plugin , "makeBackupPersistent(int,int)" ) ) {
      connect(plugin , SIGNAL(makeBackupPersistent(int,int)) ,
              this , SIGNAL( makeBackupPersistent(int,int)) ,Qt::DirectConnection);
    }
    
    // Signal send from core to Backup plugin that the given backup should be persistent
    if ( checkSlot( plugin , "slotMakeBackupPersistent(int,int)" ) ) {
      connect(this   , SIGNAL(makeBackupPersistent(int,int)) ,
              plugin , SLOT( slotMakeBackupPersistent(int,int) ),Qt::DirectConnection);
    }

    
    // Signal from plugin to restore an object with the given id
    if ( checkSignal( plugin , "restoreObject(int,int)" ) ) {
      connect(plugin , SIGNAL(restoreObject(int,int)) ,
              this   , SLOT(slotRestore( int , int)),Qt::DirectConnection );
    }
    
    // Signal send from core to backup plugin that it should restore the given object
    if ( checkSlot( plugin , "slotRestoreObject(int,int)" ) ) {
      connect(this   , SIGNAL(restoreObject(int,int)) ,
              plugin , SLOT( slotRestoreObject(int,int) ),Qt::DirectConnection);
    }
    
    //====================================================================================
    // Backup PLugin signals for communication with the other plugins about restore state
    //====================================================================================
    
    // Stage one : restore will happen soon
    if ( checkSignal( plugin , "aboutToRestore(int,int)" ) ) {
      connect(this   , SIGNAL( aboutToRestore(int,int)) ,
              plugin , SIGNAL( aboutToRestore(int,int) ),Qt::DirectConnection);
    }
    
    // Stage two: Core restore done, plugins should restore
    if ( checkSignal( plugin , "restore(int,int)" ) ) {
      connect(this   , SIGNAL(restore(int,int)) ,
              plugin , SIGNAL( restore(int,int) ),Qt::DirectConnection);
    }
    
    // Stage three: Restore complete
    if ( checkSignal( plugin , "restored(int,int)" ) ) {
      connect(this   , SIGNAL(restored(int,int)) ,
              plugin , SIGNAL( restored(int,int) ),Qt::DirectConnection);
    }
    
    //====================================================================================
    //  Plugin slots about restore state
    //====================================================================================
     
     // Stage one : restore will happen soon
     if ( checkSlot( plugin , "slotAboutToRestore(int,int)" ) ) {
       connect(this   , SIGNAL( aboutToRestore(int,int)) ,
               plugin , SLOT( slotAboutToRestore(int,int) ),Qt::DirectConnection);
     }
     
     // Stage one : restore will happen soon
     if ( checkSlot( plugin , "slotRestore(int,int)" ) ) {
       connect(this   , SIGNAL( restore(int,int)) ,
               plugin , SLOT( slotRestore(int,int) ),Qt::DirectConnection);
     }
     
     // Stage one : restore will happen soon
     if ( checkSlot( plugin , "slotRestored(int,int)" ) ) {
       connect(this   , SIGNAL( restored(int,int)) ,
               plugin , SLOT( slotRestored(int,int) ),Qt::DirectConnection);
     }

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

    // Plugins to core
    if ( checkSignal(plugin,"emptyObjectAdded(int)" ) )
      connect(plugin , SIGNAL( emptyObjectAdded( int ) ) ,
              this   , SLOT( slotEmptyObjectAdded ( int ) ),Qt::DirectConnection);

    // core to plugins
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
  
  //Check if the plugin supports Process-Interface
  ProcessInterface* processPlugin = qobject_cast< ProcessInterface * >(plugin);
  if ( processPlugin ) {
    supported = supported + "Process ";

    if ( checkSignal(plugin,"startJob(QString,QString,int,int,bool)" ) )
      connect(plugin      , SIGNAL(startJob(QString, QString,int,int,bool)),
              this , SLOT( slotStartJob(QString, QString,int,int,bool) ), Qt::DirectConnection );
    else
      emit log(LOGERR,"Process Interface defined but no startJob signal found!");  
              
    if ( checkSignal(plugin,"setJobState(QString,int)" ) )
      connect(plugin      , SIGNAL(setJobState(QString,int)),
              this , SLOT( slotSetJobState(QString,int) ), Qt::QueuedConnection );
    else
      emit log(LOGERR,"Process Interface defined but no setJobState signal found!");
    
    if ( checkSignal(plugin,"setJobName(QString,QString)" ) )
      connect(plugin ,      SIGNAL(setJobName(QString, QString)),
              this , SLOT( slotSetJobName(QString, QString) ), Qt::QueuedConnection );
    else
      emit log(LOGERR,"Process Interface defined but no setJobName signal found!");
    
    if ( checkSignal(plugin,"setJobDescription(QString,QString)" ) )
      connect(plugin ,      SIGNAL(setJobDescription(QString, QString)),
              this , SLOT( slotSetJobDescription(QString, QString) ), Qt::QueuedConnection );
    else
      emit log(LOGERR,"Process Interface defined but no setJobDescription signal found!");  
              
    if ( checkSignal(plugin,"cancelJob(QString)" ) )
      connect(plugin      , SIGNAL(cancelJob(QString)),
              this , SLOT( slotCancelJob(QString) ), Qt::QueuedConnection );
              
    if ( checkSignal(plugin,"finishJob(QString)" ) )
      connect(plugin      , SIGNAL(finishJob(QString)),
              this , SLOT( slotFinishJob(QString) ), Qt::QueuedConnection );
    else
      emit log(LOGERR,"Process Interface defined but no finishJob signal found!");                
              
    if ( checkSlot(plugin,"canceledJob(QString)" ) ) 
      connect(this ,   SIGNAL( jobCanceled( QString ) ) ,
              plugin   , SLOT( canceledJob(QString) ),Qt::QueuedConnection);                
    else
      emit log(LOGERR,"Process Interface defined but no cancel canceledJob slot found!");
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
    ft.name = basePlugin->name();
    ft.type = filePlugin->supportedType();
    ft.loadFilters = filePlugin->getLoadFilters();
    ft.saveFilters = filePlugin->getSaveFilters();
    ft.plugin = filePlugin;
    ft.object = plugin;
    ft.saveMultipleObjects = checkSlot(plugin,"saveObjects(IdList,QString)");
    
    supportedTypes_.push_back(ft);
  }
  
  //Check if it's a typePlugin
  TypeInterface* typePlugin = qobject_cast< TypeInterface * >(plugin);
  if ( typePlugin ){
    supported = supported + "Type ";
    
    // Call register type
    typePlugin->registerType();

    // Collect supported Data from type plugin
    dataTypes dt;
    dt.name = basePlugin->name();
    dt.type = typePlugin->supportedType();
    dt.plugin = typePlugin;
    
    // Add type info
    supportedDataTypes_.push_back(dt);
    
    // Connect signals ( But only if we not already connected in in the loadsave interface )
    if ( !LoadSavePlugin && checkSignal(plugin,"emptyObjectAdded(int)" ) )
      connect(plugin , SIGNAL( emptyObjectAdded( int ) ) ,
              this   , SLOT( slotEmptyObjectAdded ( int ) ),Qt::DirectConnection);
  }

  emit log(LOGOUT,"================================================================================");
  
}
