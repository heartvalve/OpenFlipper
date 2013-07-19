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
*   $LastChangedBy$                                         *
*   $Date$                     *
*                                                                            *
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
#include <QMessageBox>
#include <QtScript/QScriptValueIterator>
#include <QThread>
#include <QMutexLocker>

#include <QPluginLoader>
#include "OpenFlipper/BasePlugin/BaseInterface.hh"
#include "OpenFlipper/BasePlugin/AboutInfoInterface.hh"
#include "OpenFlipper/BasePlugin/KeyInterface.hh"
#include "OpenFlipper/BasePlugin/BackupInterface.hh"
#include "OpenFlipper/BasePlugin/LoggingInterface.hh"
#include "OpenFlipper/BasePlugin/MouseInterface.hh"
#include "OpenFlipper/BasePlugin/PickingInterface.hh"
#include "OpenFlipper/BasePlugin/ToolboxInterface.hh"
#include "OpenFlipper/BasePlugin/OptionsInterface.hh"
#include "OpenFlipper/BasePlugin/ToolbarInterface.hh"
#include "OpenFlipper/BasePlugin/TextureInterface.hh"
#include "OpenFlipper/BasePlugin/RenderInterface.hh"
#include "OpenFlipper/BasePlugin/PostProcessorInterface.hh"
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
#include "OpenFlipper/BasePlugin/SelectionInterface.hh"
#include "OpenFlipper/BasePlugin/TypeInterface.hh"
#include "OpenFlipper/BasePlugin/PluginConnectionInterface.hh"

#include "OpenFlipper/common/RendererInfo.hh"

#include "OpenFlipper/INIFile/INIFile.hh"

#include "OpenFlipper/common/GlobalOptions.hh"


#include <ACG/QtWidgets/QtFileDialog.hh>
#include "OpenFlipper/widgets/PluginDialog/PluginDialog.hh"

#include <deque>
#include <limits>

/**
 * The number of plugins to load simultaneously.

 */
static const int PRELOAD_THREADS_COUNT = (QThread::idealThreadCount() != -1) ? QThread::idealThreadCount() : 8;

class PreloadAggregator {
    public:
        PreloadAggregator() : expectedLoaders_(0) {}

        /**
         * Intended to be called by a PreloadThread
         * before count subsequent calls to loaderReady().
         * May be called multiple times in which case
         * the counts accumulate.
         *
         * @param count The number of times the calling
         * PreloadThread intents to call loaderReady().
         */
        void expectLoaders(int count) {
            QMutexLocker loadersLock(&loadersMutex_);
            expectedLoaders_ += count;
        }

        /**
         * Intended to be called by a PreloadThread.
         *
         * Transfers ownership of the supplied loader
         * to the PreloadAggregator.
         */
        void loaderReady(QPluginLoader *loader) {
            QMutexLocker loadersLock(&loadersMutex_);
            loaders_.push_back(loader);
            --expectedLoaders_;
            pluginAvailable_.wakeOne();
        }

        /**
         * If there are still loaders expected, block
         * until the next loader becomes available and return
         * it.
         *
         * Ownership of the returned object is transferred
         * to the caller. (I.e. the caller has to delete it.)
         *
         * @return Null if there are no more loaders expected.
         * The next loader, otherwise.
         */
        QPluginLoader *waitForNextLoader() {
            QMutexLocker loadersLock(&loadersMutex_);
            if (loaders_.empty()) {
                if (expectedLoaders_ > 0) {
                    pluginAvailable_.wait(&loadersMutex_);
                } else {
                    return 0;
                }
            }

            /*
             * At this point, it is guaranteed that
             * loaders_.size() > 0.
             */
            QPluginLoader *result = loaders_.front();
            loaders_.pop_front();
            return result;
        }

    protected:
        std::deque<QPluginLoader *> loaders_;
        QWaitCondition pluginAvailable_;
        QMutex loadersMutex_;

        int expectedLoaders_;
};

class PreloadThread : public QThread
{
  public: 
    
    /** \brief Preload thread constructor
    *
    * This constructor initializes the preloading thread.
    *
    * @param aggregator aggregator managing the different threads
    */
    PreloadThread(PreloadAggregator *aggregator) : aggregator_(aggregator) {
    }
  
  public:
    
    void addFilename(const QString &filename) {
        QMutexLocker filenamesLock(&filenamesMutex_);
        filenames_.push_back(filename);
        aggregator_->expectLoaders(1);
    }

    /** \brief preload function
    *
    * This function is used in the thread to preload a plugin. The name is given in the constructor.
    */
    void run() {
      for (;;) {
          QString fileName;
          {
              /*
               * Just to be on the safe side, we protect
               * filenames_. (addFilename() could be called from
               * a different thread.)
               */
              QMutexLocker filenamesLock(&filenamesMutex_);
              if (filenames_.empty()) break;
              fileName = filenames_.front();
              filenames_.pop_front();
          }

          QPluginLoader *loader = new QPluginLoader;
          loader->setFileName(fileName);
          loader->load();
          aggregator_->loaderReady(loader);
      }
    }
    
  private:
    std::deque<QString> filenames_;
    QMutex filenamesMutex_;
    PreloadAggregator *aggregator_;
};

/**
 * @brief Defines the order in which plugins have to be loaded.
 *
 * First load all type Plugins to register the dataTypes to the core
 * Than load the file plugins to load objects
 * Next is textureControl to control texture based properties
 * Then load everything else.
 */
class PluginInitializationOrder {
    public:
        int getTypeOrdinal(const QString &name) const {
            const QString basename = QFileInfo(name).baseName();
            if (basename.contains("Plugin-Type"))
                return 0;
            else if (basename.contains("Plugin-File"))
                return 1;
            else if (basename.contains("TextureControl"))
                return 2;
            else
                return 3;

        }
        bool operator() (const QString &a, const QString &b) const {
            const int typeA = getTypeOrdinal(a);
            const int typeB = getTypeOrdinal(b);
            if (typeA != typeB) { return typeA < typeB; }
            return a < b;
        }

        bool operator() (const QPluginLoader *a, const QPluginLoader *b) const {
            return operator() (a->fileName(), b->fileName());
        }
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
  
  QString licenseTexts = "";
  
  //try to load plugins from new location
  QDir tempDir = QDir(OpenFlipper::Options::pluginDir());

  // Possible Plugin extensions
  // Windows gets DLLs
  // Mac and Linux use so
  // We don't use the dylib extension on Mac at the moment.
  QStringList filter;
  if ( OpenFlipper::Options::isWindows() )
    filter << "*.dll";
  else 
    filter << "*.so";
  
  // Get all files in the Plugin dir
  QStringList pluginlist = tempDir.entryList(filter,QDir::Files);

  // Convert local file path to absolute path
  for (int i=0; i < pluginlist.size(); i++) {
     pluginlist[i] = tempDir.absoluteFilePath(pluginlist[i]);
  }

  // get all Plugin Names which will not be loaded
  QStringList dontLoadPlugins = OpenFlipperSettings().value("PluginControl/DontLoadNames", QStringList()).toStringList();
  
  // Output info about additional plugins
  for ( int i = 0 ; i < dontLoadPlugins.size(); ++i) 
    emit log(LOGOUT,tr("dontLoadPlugins Plugin from ini file: %1").arg( dontLoadPlugins[i] ) );
  
  // get all Plugins which should be loaded in addition to the standard plugins 
  QStringList additionalPlugins = OpenFlipperSettings().value("PluginControl/AdditionalPlugins", QStringList()).toStringList();
  
  // Output info about additional plugins
  for ( int i = 0 ; i < additionalPlugins.size(); ++i) {
    emit log(LOGOUT,tr("Additional Plugin from file: %1").arg( additionalPlugins[i] ) );
  }
  
  // Prepend the additional Plugins to the plugin list
  pluginlist = additionalPlugins << pluginlist;

  /*
   * Note: This call is not necessary, anymore. Initialization order
   * is determined later.
   */
  std::sort(pluginlist.begin(), pluginlist.end(), PluginInitializationOrder());

  for ( int i = 0 ; i < dontLoadPlugins.size(); ++i )
    emit log(LOGWARN,tr("Skipping Plugins :\t %1").arg( dontLoadPlugins[i] ) );

  emit log(LOGOUT,"================================================================================");

  QTime time;
  
  time.start();
  
  PreloadAggregator preloadAggregator;
  std::vector< PreloadThread* > loaderThreads(PRELOAD_THREADS_COUNT);

  /*
   * Initialize loaderThreads.
   */
  for (std::vector< PreloadThread* >::iterator it = loaderThreads.begin();
          it != loaderThreads.end(); ++it) {
      *it = new PreloadThread(&preloadAggregator);
  }
  
  /*
   * Distribute plugins onto loader threads in a round robin fashion.
   * (only load them in seperate thread. Instance will be created in main thread)
   */
  for ( int i = 0 ; i < pluginlist.size() ; ++i) {
    loaderThreads[i % loaderThreads.size()]->addFilename(pluginlist[i]);
  }

  /*
   * Start plugin preloading.
   */
  for (std::vector< PreloadThread* >::iterator it = loaderThreads.begin();
          it != loaderThreads.end(); ++it) {
      (*it)->start();
  }
  
  /*
   * Wait for the plugins to get preloaded
   */
  std::vector<QPluginLoader*> loadedPlugins;
  loadedPlugins.reserve(pluginlist.size());

  for (QPluginLoader *loader = preloadAggregator.waitForNextLoader(); loader != 0;
          loader = preloadAggregator.waitForNextLoader()) {

      loadedPlugins.push_back(loader);

      if ( OpenFlipper::Options::gui() && OpenFlipperSettings().value("Core/Gui/splash",true).toBool() ) {
        splashMessage_ = tr("Loading Plugin %1/%2").arg(loadedPlugins.size()).arg(pluginlist.size()) ;
        splash_->showMessage( splashMessage_ , Qt::AlignBottom | Qt::AlignLeft , Qt::white);
      }
  }

  /*
   * Finalize PreloadThreads.
   */
  for (std::vector< PreloadThread* >::iterator it = loaderThreads.begin();
          it != loaderThreads.end(); ++it) {
      (*it)->wait();
      delete *it;
  }

  /*
   * Initialize preloaded plugins in the correct order.
   */
  std::sort(loadedPlugins.begin(), loadedPlugins.end(), PluginInitializationOrder());
  for (std::vector<QPluginLoader*>::iterator it = loadedPlugins.begin();
          it != loadedPlugins.end(); ++it) {

      if ( OpenFlipper::Options::gui() && OpenFlipperSettings().value("Core/Gui/splash",true).toBool() ) {
        splashMessage_ = tr("Initializing Plugin %1/%2")
                .arg(std::distance(loadedPlugins.begin(), it) + 1)
                .arg(loadedPlugins.size());
        splash_->showMessage( splashMessage_ , Qt::AlignBottom | Qt::AlignLeft , Qt::white);
      }

      if ((*it)->instance() != 0 ) {
        QString pluginLicenseText  = "";
        loadPlugin((*it)->fileName(),true,pluginLicenseText, (*it)->instance());
        licenseTexts += pluginLicenseText;
      } else {
        emit log(LOGERR,tr("Unable to load Plugin :\t %1").arg( (*it)->fileName() ) );
        emit log(LOGERR,tr("Error was : ") + (*it)->errorString() );
        emit log(LOGOUT,"================================================================================");
      }
      delete *it;
  }

  emit log(LOGINFO, tr("Total time needed to load plugins was %1 ms.").arg(time.elapsed()));

  splashMessage_ = "";
  
  if ( licenseTexts != "" ) {
    if ( OpenFlipper::Options::gui() ) {
      
      // split for each license block
      QStringList licenseBlocks = licenseTexts.split("==");

      // Cleanup lists to get only the ones containing valid plugins.
      for ( QStringList::iterator it = licenseBlocks.begin(); it != licenseBlocks.end() ; ++it )
        if ( ! it->contains("PluginName") ) {
          licenseBlocks.erase(it);
          it = licenseBlocks.begin();
        }
      
      // sort by the contact mails
      QMap< QString , QString > contacts;
      
      for ( QStringList::iterator it = licenseBlocks.begin(); it != licenseBlocks.end() ; ++it ) {
        QStringList lines = it->split("\n");
        
        lines = lines.filter ( "Contact mail", Qt::CaseInsensitive );
        
        // Corect one found:
        if (lines.size() == 1)  {
          QString mail = lines[0].section(":",-1).simplified();
          QString list = contacts.take(mail);
          list.append(*it);
          contacts.insert(mail,list);
        } else {
          emit log(LOGWARN,tr("Can't extract mail contact from license request"));
        }
          
      }
      
      for ( QMap<QString , QString>::iterator it = contacts.begin() ; it != contacts.end() ; ++it ) {
        
        QStringList request = it.value().split("\n");
        
        // Cleanup lists to get only the relevant part
        for ( QStringList::iterator lit = request.begin(); lit != request.end() ; ++lit ) {
          
          if ( lit->contains("Message:")  ) {
            *lit = lit->section(":",-1).simplified();
          }
          
          if ( lit->contains("Contact mail:")  ) {
            *lit = lit->section(":",-1).simplified();
          }
           
        }

        QDialog licenseBox;
          
        QTextEdit *edit = new QTextEdit(&licenseBox);
        edit->setText(request.join("\n"));

        QLabel* mailLabel = new QLabel(&licenseBox);
        mailLabel->setText(tr("The text has been copied to your clipboard. Open in Mail program?"));

        QPushButton* noButton = new QPushButton(&licenseBox);
        noButton->setText(tr("No"));
        connect( noButton, SIGNAL(clicked ()), &licenseBox, SLOT(reject()) );

        QPushButton* yesButton = new QPushButton(&licenseBox);
        yesButton->setText(tr("Yes"));
        connect( yesButton, SIGNAL(clicked ()), &licenseBox, SLOT(accept()) );

        QGridLayout *layout = new QGridLayout;
        layout->addWidget(edit,0,0,1,2);
        layout->addWidget(mailLabel,1,0,1,2);
        layout->addWidget(noButton,2,0);
        layout->addWidget(yesButton,2,1);
        licenseBox.setLayout(layout);

        licenseBox.resize(500,500);
        licenseBox.setModal(true);
        licenseBox.setWindowTitle(tr("Plugin License check failed, issuer is: %1").arg( it.key() ));  
        int userAnswer =licenseBox.exec();

        // set a text to the Clipboard
        QClipboard *cb = QApplication::clipboard();
        cb->setText(request.join("\n"));

        if ( userAnswer == 1 ) {
           QString url = "mailto:" + it.key();
           url += "?subject=License Request&body=";
#ifdef WIN32
           url += request.join(";;");
#else
           url += request.join("\n");
#endif

           QUrl encodedURL(url, QUrl::TolerantMode);
           QDesktopServices::openUrl(encodedURL);
        }
        
      }

      
    } else {
      emit log(LOGWARN,tr("Plugin License check failed: "));
      std::cerr << licenseTexts.toStdString() << std::endl;
    }
  } 

  emit pluginsInitialized();

  emit log(LOGOUT,tr("Loaded %n Plugin(s)","",plugins_.size()) );
}

/** @brief slot for loading Plugins
 */
void Core::slotLoadPlugin(){

  if ( OpenFlipper::Options::nogui() )
    return;

  // Setup filters for possible plugin extensions
  // Windows gets DLLs
  // Mac and Linux use so
  // We don't use the dylib extension on Mac at the moment.
  QString filter;
  if ( OpenFlipper::Options::isWindows() )
    filter = "Plugins (*.dll)";
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
  QStringList dontLoadPlugins = OpenFlipperSettings().value("PluginControl/DontLoadNames", QStringList()).toStringList();
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
  QStringList additionalPlugins = OpenFlipperSettings().value("PluginControl/AdditionalPlugins", QStringList()).toStringList();
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
  
  QString licenseText = "";
  loadPlugin(filename,false,licenseText);
  
  if ( licenseText != "" ) {
    if ( OpenFlipper::Options::gui() ) {
      QMessageBox::warning ( 0, tr("Plugin License check failed"),  licenseText );
      
      std::cerr << "OpenURL: " << std::endl;
      QDesktopServices::openUrl(QUrl(tr("mailto:contact@openflipper.com?subject=License Request&body=%1").arg(licenseText), QUrl::TolerantMode));
    } else {
      std::cerr << "Plugin License check failed" << std::endl;
      std::cerr << licenseText.toStdString() << std::endl;
    }
  }
}

/** @brief slot for showing loaded Plugins
 */
void Core::slotShowPlugins(){

  if ( OpenFlipper::Options::gui() ){

    int ret = 0;

    while (ret == 0){

      PluginDialog* dialog = new PluginDialog(plugins_, coreWidget_);

      //connect signals
      connect(dialog, SIGNAL( loadPlugin() ), this, SLOT( slotLoadPlugin() ));
      connect(dialog, SIGNAL(blockPlugin(const QString&)), this, SLOT(slotBlockPlugin(const QString&)));
      connect(dialog, SIGNAL(unBlockPlugin(const QString&)), this, SLOT(slotUnBlockPlugin(const QString&)));
      connect(dialog, SIGNAL(loadPlugin(const QString& ,const bool , QString& , QObject* )),
          this,SLOT(loadPlugin(const QString& ,const bool , QString& , QObject* )));

      //if a plugin was deleted/loaded the dialog returns 0 and it needs to be loaded again
      ret = dialog->exec();

      delete dialog;
    }
  }
}

void Core::slotBlockPlugin(const QString &_name)
{
  QStringList dontLoadPlugins = OpenFlipperSettings().value("PluginControl/DontLoadNames",QStringList()).toStringList();
  if ( !dontLoadPlugins.contains(_name) ){
    dontLoadPlugins << _name;
    OpenFlipperSettings().setValue("PluginControl/DontLoadNames",dontLoadPlugins);
  }

  for (size_t i = 0; i < plugins_.size();++i)
    if (plugins_[i].name == _name)
      plugins_[i].status = PluginInfo::BLOCKED;
}

void Core::slotUnBlockPlugin(const QString &_name)
{
  QStringList dontLoadPlugins = OpenFlipperSettings().value("PluginControl/DontLoadNames",QStringList()).toStringList();
  dontLoadPlugins.removeAll(_name);
  OpenFlipperSettings().setValue("PluginControl/DontLoadNames",dontLoadPlugins);

  for (size_t i = 0; i < plugins_.size();++i)
      if (plugins_[i].name == _name)
        plugins_[i].status = PluginInfo::UNLOADED;
}

/** @brief Load a Plugin with given filename
 *  @param _filename complete path + filename of the plugin
 *  @param _silent if 'true': user isn't asked what to do if a plugin is already loaded
 *  @param _licenseErrors String will be epty when no license issues occured, otherwise it contains the required information for obtaining a license
 *  @param _plugin You can provide a preloaded plugin here. If this is 0 the filename will be used to load the plugin.
 */
void Core::loadPlugin(const QString& _filename,const bool _silent, QString& _licenseErrors, QObject* _plugin){

  _licenseErrors = "";
  
  // Only load .dll under windows
  if ( OpenFlipper::Options::isWindows() ) {
    QString dllname = _filename;
    if ( ! dllname.endsWith( ".dll" ) )
      return;
  }
  // Only load .so under linux
  if ( OpenFlipper::Options::isLinux() ) {
    QString soname = _filename;
    if ( ! soname.endsWith( ".so" ) )
      return;
  }

  // This will be the reference to our plugin
  QObject *plugin = 0;

  // Try to open the file if we did not get a plugin,
  // Otherwise use the supplied plugin pointer
  if ( _plugin == 0 ) {
    QPluginLoader loader( _filename );
    plugin = loader.instance();  
    
    if ( !plugin) {
      emit log(LOGERR,tr("Unable to load Plugin :\t %1").arg( _filename ) );
      emit log(LOGERR,tr("Error was : ") + loader.errorString() );
      emit log(LOGOUT,"================================================================================");       
    }
    emit log (LOGOUT,tr("Plugin loaded: \t %1").arg(_filename));
    
  } else {
      plugin = _plugin;
  }
  
  // Check if a plugin has been loaded
  PluginInfo info;
  int alreadyLoadedAt = -1;
  for (unsigned int k=0; k < plugins_.size(); k++)
  {
    if (plugins_[k].path == _filename)
      alreadyLoadedAt = static_cast<int>(k);
  }
  info.status = PluginInfo::FAILED;
  info.path = _filename;
  QString supported;

  emit log(LOGOUT,tr("Location : \t %1").arg( _filename) );
  
  // Check if it is a BasePlugin
  BaseInterface* basePlugin = qobject_cast< BaseInterface * >(plugin);
  if ( basePlugin ) {

    //set basic information about plugin
    info.name = basePlugin->name();
    info.rpcName = info.name.remove(" ").toLower();
    info.description = basePlugin->description();

    QStringList additionalPlugins = OpenFlipperSettings().value("PluginControl/AdditionalPlugins", QStringList()).toStringList();
    info.buildIn = !additionalPlugins.contains(info.path);

    emit log(LOGOUT,tr("Found Plugin : \t %1").arg(basePlugin->name()) );

    if ( OpenFlipper::Options::gui() && OpenFlipperSettings().value("Core/Gui/splash",true).toBool() ) {
      splashMessage_ = splashMessage_ + " " + basePlugin->name() ;
      splash_->showMessage( splashMessage_ , Qt::AlignBottom | Qt::AlignLeft , Qt::white);
    }

    //Check if plugin is already loaded
    for (unsigned int k=0; k < plugins_.size(); k++){

        QString name_nospace =  basePlugin->name();
        name_nospace.remove(" ");

        if (plugins_[k].name == name_nospace && plugins_[k].path != _filename && plugins_[k].status == PluginInfo::LOADED){
          if (_silent || OpenFlipper::Options::nogui() ){ //dont load the plugin
            emit log(LOGWARN, tr("\t\t\t Already loaded from %1").arg( plugins_[k].path) );
            emit log(LOGOUT,"================================================================================");
            info.description = basePlugin->description() + tr(" *Already loaded.*");
            return;
        }else{ //ask the user
          int ret = QMessageBox::question(coreWidget_,
                                          tr("Plugin already loaded"),
                                          tr("A Plugin with the same name was already loaded from %1.\n" 
                                              "You can only load the new plugin if you unload the existing one first.\n\n"
                                              "Do you want to unload the existing plugin first?").arg( plugins_[k].path),
                                          QMessageBox::Yes|QMessageBox::No, QMessageBox::No);
          if (ret == QMessageBox::No)
          {
            emit log(LOGWARN, tr("\t\t\t Already loaded from %1.").arg( plugins_[k].path));
            emit log(LOGOUT,"================================================================================");
            info.description = basePlugin->description() + tr(" *Already loaded.*");
            return;
          }
        }
      }
    }

    QStringList dontLoadPlugins = OpenFlipperSettings().value("PluginControl/DontLoadNames",QStringList()).toStringList();

    if ( dontLoadPlugins.contains(basePlugin->name(), Qt::CaseInsensitive) ) {
      emit log(LOGWARN,tr("OpenFlipper.ini prevented Plugin %1 from being loaded! ").arg( basePlugin->name() ));
      emit log(LOGOUT,"================================================================================");
      info.status = PluginInfo::BLOCKED;
      return;
    }

    //Check if it is a BasePlugin
    SecurityInterface * securePlugin = qobject_cast< SecurityInterface * >(plugin);
    if ( securePlugin ) {
      emit log(LOGINFO,tr("Plugin uses security interface. Trying to authenticate against plugin ..."));

      bool success = false;
      QMetaObject::invokeMethod(plugin,"authenticate", Q_RETURN_ARG( bool , success ) ) ;
      
      QString message = "";
      QMetaObject::invokeMethod(plugin,"licenseError", Q_RETURN_ARG( QString , message ) ) ;
      _licenseErrors = message;

      if ( success )
        emit log(LOGINFO,tr("... ok. Loading plugin "));
      else {
        emit log(LOGERR,tr("... failed. Plugin access denied."));
        emit log(LOGOUT,"================================================================================");
        info.description = basePlugin->description() + tr(" *Plugin access denied.*");
        // Abort here, as the plugin will not do anything else until correct authentication.
        return;
      }
    }


    emit log(LOGOUT,tr("Plugin Desciption :\t %1 ").arg( basePlugin->description()) );

    supported = "BaseInterface ";

    info.plugin        = plugin;
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


    if ( checkSignal(plugin,"updatedObject(int)") && checkSignal(plugin,"updatedObject(int,const UpdateType&)") ){
      
      log(LOGERR,tr("Plugin uses deprecated and(!) new updatedObject. Only new updatedObject will be active."));
      connect(plugin,SIGNAL(updatedObject(int,const UpdateType&)),this,SLOT(slotObjectUpdated(int,const UpdateType&)), Qt::DirectConnection);
      
    } else {

      if ( checkSignal(plugin,"updatedObject(int)") ){
        log(LOGWARN,tr("Plugin uses deprecated updatedObject."));
        connect(plugin,SIGNAL(updatedObject(int)),this,SLOT(slotObjectUpdated(int)), Qt::DirectConnection);
      }

      if ( checkSignal(plugin,"updatedObject(int,const UpdateType&)") )
        connect(plugin,SIGNAL(updatedObject(int,const UpdateType&)),this,SLOT(slotObjectUpdated(int,const UpdateType&)), Qt::DirectConnection);
    }

    if ( checkSlot( plugin , "slotObjectUpdated(int)" ) && checkSlot( plugin , "slotObjectUpdated(int,const UpdateType&)" ) ){
      
      log(LOGERR,tr("Plugin uses deprecated and(!) new slotObjectUpdated. Only new slotObjectUpdated will be active."));
      connect(this,SIGNAL(signalObjectUpdated(int,const UpdateType&)),plugin,SLOT(slotObjectUpdated(int,const UpdateType&)), Qt::DirectConnection);
    
    } else {

      if ( checkSlot( plugin , "slotObjectUpdated(int)" ) ){
        log(LOGWARN,tr("Plugin uses deprecated slotObjectUpdated."));
        connect(this,SIGNAL(signalObjectUpdated(int)),plugin,SLOT(slotObjectUpdated(int)), Qt::DirectConnection);
      }

      if ( checkSlot( plugin , "slotObjectUpdated(int,const UpdateType&)" ) )
        connect(this,SIGNAL(signalObjectUpdated(int,const UpdateType&)),plugin,SLOT(slotObjectUpdated(int,const UpdateType&)), Qt::DirectConnection);
    }

    if ( checkSignal(plugin,"objectPropertiesChanged(int)")) {
      emit log (LOGERR,tr("Signal objectPropertiesChanged(int) is deprecated. " ));
      emit log (LOGERR,tr("The signal will be automatically emitted by the object that has been changed and the core will deliver it to the plugins!. "));
      emit log (LOGERR,tr("Please remove this signal from your plugins!. "));
    }
    
    if ( checkSlot( plugin , "slotViewChanged()" ) )
      connect(this,SIGNAL(pluginViewChanged()),plugin,SLOT(slotViewChanged()), Qt::DirectConnection);

    if ( checkSlot( plugin , "slotSceneDrawn()" ) )
      connect(this,SIGNAL(pluginSceneDrawn()),plugin,SLOT(slotSceneDrawn()), Qt::DirectConnection);

    if ( checkSlot( plugin , "slotDrawModeChanged(int)" ) )
          connect(coreWidget_,SIGNAL(drawModeChanged(int)),plugin,SLOT(slotDrawModeChanged(int)), Qt::DirectConnection);

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

    // =============================================
    // Function allowing switching of renderers from other plugins
    // =============================================
    if ( checkSignal(plugin,"setRenderer(unsigned int,QString)" ) ) {
      connect(plugin,SIGNAL(setRenderer(unsigned int,QString)),this,SLOT(slotSetRenderer(unsigned int,QString)));
    }

    if ( checkSignal(plugin,"getCurrentRenderer(unsigned int,QString&)" ) ) {
      connect(plugin,SIGNAL(getCurrentRenderer(unsigned int,QString&)),this,SLOT(slotGetCurrentRenderer(unsigned int,QString&)), Qt::DirectConnection);
    }


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
    if ( checkSignal(plugin,"addMenubarActions(std::vector<QAction*>, QString)") )
      connect(plugin      , SIGNAL(addMenubarActions(std::vector<QAction*>,QString)),
              coreWidget_ , SLOT(slotAddMenubarActions(std::vector<QAction*>,QString)),Qt::DirectConnection);
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

    if ( checkSignal(plugin,"hideContextMenu()") )
          connect(plugin      , SIGNAL(hideContextMenu()),
                  coreWidget_ , SLOT(slotHideContextMenu()),Qt::DirectConnection);

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

    if ( checkSignal(plugin, "addToolbox(QString,QWidget*,QIcon*)"))
      connect(plugin, SIGNAL( addToolbox(QString,QWidget*,QIcon*) ),
              this, SLOT( addToolbox(QString,QWidget*,QIcon*) ),Qt::DirectConnection );
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

    if ( checkSignal(plugin, "setViewMode(QString,bool)"))
      connect(plugin,    SIGNAL( setViewMode(QString, bool) ),
              coreWidget_, SLOT( setViewMode(QString, bool) ),Qt::DirectConnection );
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

    if ( checkSlot( plugin , "slotMouseEventLight(QMouseEvent*)" ) )
      connect(this   , SIGNAL(PluginMouseEventLight(QMouseEvent*)),
              plugin , SLOT(slotMouseEventLight(QMouseEvent*)));

  }

  //Check if the plugin supports InformationInterface
  InformationInterface* infoPlugin = qobject_cast< InformationInterface * >(plugin);
  if ( infoPlugin && OpenFlipper::Options::gui()  ) {
    supported = supported + "TypeInformation ";

    DataType dtype = infoPlugin->supportedDataTypes();
    supportedInfoTypes().insert(std::pair<InformationInterface*,DataType>(infoPlugin,dtype));
  }

  //Check if the plugin supports Picking-Interface
  PickingInterface* pickPlugin = qobject_cast< PickingInterface * >(plugin);
  if ( pickPlugin && OpenFlipper::Options::gui() ) {
    supported = supported + "Picking ";

    if ( checkSlot( plugin , "slotPickModeChanged(const std::string&)" ) )
      connect(coreWidget_,SIGNAL(signalPickModeChanged (const std::string &)),
              plugin,SLOT(slotPickModeChanged( const std::string &)));

    if ( checkSignal(plugin,"addPickMode(const std::string&)") )
      connect(plugin,SIGNAL(addPickMode( const std::string& )),
              this,SLOT(slotAddPickMode( const std::string& )),Qt::DirectConnection);

    if ( checkSignal(plugin,"addHiddenPickMode(const std::string&)") )
      connect(plugin,SIGNAL(addHiddenPickMode( const std::string& )),
              this,SLOT(slotAddHiddenPickMode( const std::string& )),Qt::DirectConnection);

    if ( checkSignal(plugin,"setPickModeCursor(const std::string&,QCursor)") )
      for ( unsigned int i = 0 ; i < OpenFlipper::Options::examinerWidgets() ; ++i )
        connect(plugin,SIGNAL(setPickModeCursor( const std::string& ,QCursor)),
                coreWidget_,SLOT(setPickModeCursor( const std::string& ,QCursor)),Qt::DirectConnection);

    if ( checkSignal(plugin,"setPickModeMouseTracking(const std::string&,bool)") )
      for ( unsigned int i = 0 ; i < OpenFlipper::Options::examinerWidgets() ; ++i )
        connect(plugin,SIGNAL(setPickModeMouseTracking( const std::string& ,bool)),
                coreWidget_,SLOT(setPickModeMouseTracking( const std::string& ,bool)),Qt::DirectConnection);

    if ( checkSignal(plugin,"setPickModeToolbar(const std::string&,QToolBar*)") )
      connect(plugin,SIGNAL(setPickModeToolbar (const std::string&, QToolBar*)),
              coreWidget_,SLOT(setPickModeToolbar (const std::string&, QToolBar*)),Qt::DirectConnection);

    if ( checkSignal(plugin,"removePickModeToolbar(const std::string&)") )
      connect(plugin,SIGNAL(removePickModeToolbar( const std::string&)),
              coreWidget_,SLOT(removePickModeToolbar( const std::string&)),Qt::DirectConnection);

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
  
  //Check if the plugin supports Selection-Interface
  SelectionInterface* selectionPlugin = qobject_cast< SelectionInterface * >(plugin);
  if ( selectionPlugin && OpenFlipper::Options::gui() ) {
    supported = supported + "SelectionBase ";
    
    if ( checkSignal(plugin,"addSelectionEnvironment(QString,QString,QIcon,QString&)") )
      log(LOGERR,tr("Plugin uses deprecated addSelectionEnvironment(QString,QString,QIcon,QString&) , Replace the qicon by the path to the icon!"));

    if ( checkSignal(plugin,"addSelectionEnvironment(QString,QString,QString,QString&)") )
      connect(plugin , SIGNAL(addSelectionEnvironment(QString,QString,QString,QString&)),
              this   , SLOT(slotAddSelectionEnvironment(QString,QString,QString,QString&)),Qt::DirectConnection);

    // ===============

    if ( checkSlot(plugin,"slotAddSelectionEnvironment(QString,QString,QIcon,QString&)") )
          log(LOGERR,tr("Plugin uses deprecated slotAddSelectionEnvironment(QString,QString,QIcon,QString&) , Replace the qicon by the path to the icon!"));

    if ( checkSlot( plugin , "slotAddSelectionEnvironment(QString,QString,QString,QString&)" ) )
      connect(this   , SIGNAL(addSelectionEnvironment(QString,QString,QString,QString&)),
              plugin , SLOT(slotAddSelectionEnvironment(QString,QString,QString,QString&)),Qt::DirectConnection);

    // ===============
              
    if ( checkSignal(plugin,"registerType(QString,DataType)") )
      connect(plugin , SIGNAL(registerType(QString,DataType)),
              this   , SLOT(slotRegisterType(QString,DataType)),Qt::DirectConnection);

    // ===============

    if ( checkSlot( plugin , "slotRegisterType(QString,DataType)" ) )
      connect(this   , SIGNAL(registerType(QString,DataType)),
              plugin , SLOT(slotRegisterType(QString,DataType)),Qt::DirectConnection);

    // ===============

    if ( checkSignal(plugin,"addPrimitiveType(QString,QString,QIcon,SelectionInterface::PrimitiveType&)") )
              log(LOGERR,tr("Plugin uses deprecated addPrimitiveType(QString,QString,QIcon,SelectionInterface::PrimitiveType&) , Replace the qicon by the path to the icon!"));

    if ( checkSignal(plugin,"addPrimitiveType(QString,QString,QString,SelectionInterface::PrimitiveType&)") )
      connect(plugin , SIGNAL(addPrimitiveType(QString,QString,QString,SelectionInterface::PrimitiveType&)),
              this   , SLOT(slotAddPrimitiveType(QString,QString,QString,SelectionInterface::PrimitiveType&)),Qt::DirectConnection);

    // ===============

    if ( checkSlot(plugin,"slotAddPrimitiveType(QString,QString,QIcon,SelectionInterface::PrimitiveType&)") )
              log(LOGERR,tr("Plugin uses deprecated slotAddPrimitiveType(QString,QString,QIcon,SelectionInterface::PrimitiveType&) , Replace the qicon by the path to the icon!"));

    if ( checkSlot( plugin , "slotAddPrimitiveType(QString,QString,QString,SelectionInterface::PrimitiveType&)" ) )
      connect(this   , SIGNAL(addPrimitiveType(QString,QString,QString,SelectionInterface::PrimitiveType&)),
              plugin , SLOT(slotAddPrimitiveType(QString,QString,QString,SelectionInterface::PrimitiveType&)),Qt::DirectConnection);

    // ===============

    if ( checkSignal(plugin,"addCustomSelectionMode(QString,QString,QString,QIcon,SelectionInterface::PrimitiveType,QString&)") )
              log(LOGERR,tr("Plugin uses deprecated addCustomSelectionMode(QString,QString,QString,QIcon,SelectionInterface::PrimitiveType,QString&) , Replace the qicon by the path to the icon!"));

    if ( checkSignal(plugin,"addCustomSelectionMode(QString,QString,QString,QString,SelectionInterface::PrimitiveType,QString&)") )
      connect(plugin , SIGNAL(addCustomSelectionMode(QString,QString,QString,QString,SelectionInterface::PrimitiveType,QString&)),
              this   , SLOT(slotAddCustomSelectionMode(QString,QString,QString,QString,SelectionInterface::PrimitiveType,QString&)),Qt::DirectConnection);

    // ===============

    if ( checkSignal(plugin,"addCustomSelectionMode(QString,QString,QString,QIcon,SelectionInterface::PrimitiveType,QString&,DataType)") )
              log(LOGERR,tr("Plugin uses deprecated addCustomSelectionMode(QString,QString,QString,QIcon,SelectionInterface::PrimitiveType,QString&,DataType) , Replace the qicon by the path to the icon!"));

    if ( checkSignal(plugin,"addCustomSelectionMode(QString,QString,QString,QIcon,SelectionInterface::PrimitiveType,QString&,DataType)") )
      connect(plugin , SIGNAL(addCustomSelectionMode(QString,QString,QString,QIcon,SelectionInterface::PrimitiveType,QString&,DataType)),
              this   , SLOT(slotAddCustomSelectionMode(QString,QString,QString,QIcon,SelectionInterface::PrimitiveType,QString&,DataType)),Qt::DirectConnection);

    // ===============

    if ( checkSlot(plugin,"slotAddCustomSelectionMode(QString,QString,QString,QIcon,SelectionInterface::PrimitiveType,QString&)") )
              log(LOGERR,tr("Plugin uses deprecated slotAddCustomSelectionMode(QString,QString,QString,QIcon,SelectionInterface::PrimitiveType,QString&) , Replace the qicon by the path to the icon!"));

    if ( checkSlot( plugin , "slotAddCustomSelectionMode(QString,QString,QString,QString,SelectionInterface::PrimitiveType,QString&)" ) )
      connect(this   , SIGNAL(addCustomSelectionMode(QString,QString,QString,QString,SelectionInterface::PrimitiveType,QString&)),
              plugin , SLOT(slotAddCustomSelectionMode(QString,QString,QString,QString,SelectionInterface::PrimitiveType,QString&)),Qt::DirectConnection);

    // ===============

    if ( checkSlot(plugin,"slotAddCustomSelectionMode(QString,QString,QString,QIcon,SelectionInterface::PrimitiveType,QString&,DataType)") )
              log(LOGERR,tr("Plugin uses deprecated slotAddCustomSelectionMode(QString,QString,QString,QIcon,SelectionInterface::PrimitiveType,QString&,DataType) , Replace the qicon by the path to the icon!"));

    if ( checkSlot( plugin , "slotAddCustomSelectionMode(QString,QString,QString,QString,SelectionInterface::PrimitiveType,QString&,DataType)" ) )
      connect(this   , SIGNAL(addCustomSelectionMode(QString,QString,QString,QString,SelectionInterface::PrimitiveType,QString&,DataType)),
              plugin , SLOT(slotAddCustomSelectionMode(QString,QString,QString,QString,SelectionInterface::PrimitiveType,QString&,DataType)),Qt::DirectConnection);
              
    // ===============


    if ( checkSignal(plugin,"addSelectionOperations(QString,QStringList,QString,SelectionInterface::PrimitiveType)") )
      connect(plugin , SIGNAL(addSelectionOperations(QString,QStringList,QString,SelectionInterface::PrimitiveType)),
              this   , SLOT(slotAddSelectionOperations(QString,QStringList,QString,SelectionInterface::PrimitiveType)),Qt::DirectConnection);

    if ( checkSlot( plugin , "slotAddSelectionOperations(QString,QStringList,QString,SelectionInterface::PrimitiveType)" ) )
      connect(this   , SIGNAL(addSelectionOperations(QString,QStringList,QString,SelectionInterface::PrimitiveType)),
              plugin , SLOT(slotAddSelectionOperations(QString,QStringList,QString,SelectionInterface::PrimitiveType)),Qt::DirectConnection);
              
    if ( checkSignal(plugin,"selectionOperation(QString)") )
      connect(plugin , SIGNAL(selectionOperation(QString)),
              this   , SLOT(slotSelectionOperation(QString)),Qt::DirectConnection);

    if ( checkSlot( plugin , "slotSelectionOperation(QString)" ) )
      connect(this   , SIGNAL(selectionOperation(QString)),
              plugin , SLOT(slotSelectionOperation(QString)),Qt::DirectConnection);
              
    if ( checkSignal(plugin,"showToggleSelectionMode(QString,bool,SelectionInterface::PrimitiveType)") )
      connect(plugin , SIGNAL(showToggleSelectionMode(QString,bool,SelectionInterface::PrimitiveType)),
              this   , SLOT(slotShowToggleSelectionMode(QString,bool,SelectionInterface::PrimitiveType)),Qt::DirectConnection);

    if ( checkSlot( plugin , "slotShowToggleSelectionMode(QString,bool,SelectionInterface::PrimitiveType)" ) )
      connect(this   , SIGNAL(showToggleSelectionMode(QString,bool,SelectionInterface::PrimitiveType)),
              plugin , SLOT(slotShowToggleSelectionMode(QString,bool,SelectionInterface::PrimitiveType)),Qt::DirectConnection);
              
    if ( checkSignal(plugin,"showLassoSelectionMode(QString,bool,SelectionInterface::PrimitiveType)") )
      connect(plugin , SIGNAL(showLassoSelectionMode(QString,bool,SelectionInterface::PrimitiveType)),
              this   , SLOT(slotShowLassoSelectionMode(QString,bool,SelectionInterface::PrimitiveType)),Qt::DirectConnection);

    if ( checkSlot( plugin , "slotShowLassoSelectionMode(QString,bool,SelectionInterface::PrimitiveType)" ) )
      connect(this   , SIGNAL(showLassoSelectionMode(QString,bool,SelectionInterface::PrimitiveType)),
              plugin , SLOT(slotShowLassoSelectionMode(QString,bool,SelectionInterface::PrimitiveType)),Qt::DirectConnection);
              
    if ( checkSignal(plugin,"showVolumeLassoSelectionMode(QString,bool,SelectionInterface::PrimitiveType)") )
      connect(plugin , SIGNAL(showVolumeLassoSelectionMode(QString,bool,SelectionInterface::PrimitiveType)),
              this   , SLOT(slotShowVolumeLassoSelectionMode(QString,bool,SelectionInterface::PrimitiveType)),Qt::DirectConnection);

    if ( checkSlot( plugin , "slotShowVolumeLassoSelectionMode(QString,bool,SelectionInterface::PrimitiveType)" ) )
      connect(this   , SIGNAL(showVolumeLassoSelectionMode(QString,bool,SelectionInterface::PrimitiveType)),
              plugin , SLOT(slotShowVolumeLassoSelectionMode(QString,bool,SelectionInterface::PrimitiveType)),Qt::DirectConnection);
              
    if ( checkSignal(plugin,"showSurfaceLassoSelectionMode(QString,bool,SelectionInterface::PrimitiveType)") )
      connect(plugin , SIGNAL(showSurfaceLassoSelectionMode(QString,bool,SelectionInterface::PrimitiveType)),
              this   , SLOT(slotShowSurfaceLassoSelectionMode(QString,bool,SelectionInterface::PrimitiveType)),Qt::DirectConnection);

    if ( checkSlot( plugin , "slotShowSurfaceLassoSelectionMode(QString,bool,SelectionInterface::PrimitiveType)" ) )
      connect(this   , SIGNAL(showSurfaceLassoSelectionMode(QString,bool,SelectionInterface::PrimitiveType)),
              plugin , SLOT(slotShowSurfaceLassoSelectionMode(QString,bool,SelectionInterface::PrimitiveType)),Qt::DirectConnection);
              
    if ( checkSignal(plugin,"showSphereSelectionMode(QString,bool,SelectionInterface::PrimitiveType)") )
      connect(plugin , SIGNAL(showSphereSelectionMode(QString,bool,SelectionInterface::PrimitiveType)),
              this   , SLOT(slotShowSphereSelectionMode(QString,bool,SelectionInterface::PrimitiveType)),Qt::DirectConnection);

    if ( checkSlot( plugin , "slotShowSphereSelectionMode(QString,bool,SelectionInterface::PrimitiveType)" ) )
      connect(this   , SIGNAL(showSphereSelectionMode(QString,bool,SelectionInterface::PrimitiveType)),
              plugin , SLOT(slotShowSphereSelectionMode(QString,bool,SelectionInterface::PrimitiveType)),Qt::DirectConnection);
              
    if ( checkSignal(plugin,"showClosestBoundarySelectionMode(QString,bool,SelectionInterface::PrimitiveType)") )
      connect(plugin , SIGNAL(showClosestBoundarySelectionMode(QString,bool,SelectionInterface::PrimitiveType)),
              this   , SLOT(slotShowClosestBoundarySelectionMode(QString,bool,SelectionInterface::PrimitiveType)),Qt::DirectConnection);

    if ( checkSlot( plugin , "slotShowClosestBoundarySelectionMode(QString,bool,SelectionInterface::PrimitiveType)" ) )
      connect(this   , SIGNAL(showClosestBoundarySelectionMode(QString,bool,SelectionInterface::PrimitiveType)),
              plugin , SLOT(slotShowClosestBoundarySelectionMode(QString,bool,SelectionInterface::PrimitiveType)),Qt::DirectConnection);
              
    if ( checkSignal(plugin,"showFloodFillSelectionMode(QString,bool,SelectionInterface::PrimitiveType)") )
      connect(plugin , SIGNAL(showFloodFillSelectionMode(QString,bool,SelectionInterface::PrimitiveType)),
              this   , SLOT(slotShowFloodFillSelectionMode(QString,bool,SelectionInterface::PrimitiveType)),Qt::DirectConnection);

    if ( checkSlot( plugin , "slotShowFloodFillSelectionMode(QString,bool,SelectionInterface::PrimitiveType)" ) )
      connect(this   , SIGNAL(showFloodFillSelectionMode(QString,bool,SelectionInterface::PrimitiveType)),
              plugin , SLOT(slotShowFloodFillSelectionMode(QString,bool,SelectionInterface::PrimitiveType)),Qt::DirectConnection);

    if ( checkSignal(plugin,"showComponentsSelectionMode(QString,bool,SelectionInterface::PrimitiveType)") )
      connect(plugin , SIGNAL(showComponentsSelectionMode(QString,bool,SelectionInterface::PrimitiveType)),
              this   , SLOT(slotShowComponentsSelectionMode(QString,bool,SelectionInterface::PrimitiveType)),Qt::DirectConnection);

    if ( checkSlot( plugin , "slotShowComponentsSelectionMode(QString,bool,SelectionInterface::PrimitiveType)" ) )
      connect(this   , SIGNAL(showComponentsSelectionMode(QString,bool,SelectionInterface::PrimitiveType)),
              plugin , SLOT(slotShowComponentsSelectionMode(QString,bool,SelectionInterface::PrimitiveType)),Qt::DirectConnection);
              
    if ( checkSignal(plugin,"toggleSelection(QMouseEvent*,SelectionInterface::PrimitiveType,bool)") )
      connect(plugin , SIGNAL(toggleSelection(QMouseEvent*,SelectionInterface::PrimitiveType,bool)),
              this   , SLOT(slotToggleSelection(QMouseEvent*,SelectionInterface::PrimitiveType,bool)),Qt::DirectConnection);

    if ( checkSlot( plugin , "slotToggleSelection(QMouseEvent*,SelectionInterface::PrimitiveType,bool)" ) )
      connect(this   , SIGNAL(toggleSelection(QMouseEvent*,SelectionInterface::PrimitiveType,bool)),
              plugin , SLOT(slotToggleSelection(QMouseEvent*,SelectionInterface::PrimitiveType,bool)),Qt::DirectConnection);
              
    if ( checkSignal(plugin,"lassoSelection(QMouseEvent*,SelectionInterface::PrimitiveType,bool)") )
      connect(plugin , SIGNAL(lassoSelection(QMouseEvent*,SelectionInterface::PrimitiveType,bool)),
              this   , SLOT(slotLassoSelection(QMouseEvent*,SelectionInterface::PrimitiveType,bool)),Qt::DirectConnection);

    if ( checkSlot( plugin , "slotLassoSelection(QMouseEvent*,SelectionInterface::PrimitiveType,bool)" ) )
      connect(this   , SIGNAL(lassoSelection(QMouseEvent*,SelectionInterface::PrimitiveType,bool)),
              plugin , SLOT(slotLassoSelection(QMouseEvent*,SelectionInterface::PrimitiveType,bool)),Qt::DirectConnection);
              
    if ( checkSignal(plugin,"volumeLassoSelection(QMouseEvent*,SelectionInterface::PrimitiveType,bool)") )
      connect(plugin , SIGNAL(volumeLassoSelection(QMouseEvent*,SelectionInterface::PrimitiveType,bool)),
              this   , SLOT(slotVolumeLassoSelection(QMouseEvent*,SelectionInterface::PrimitiveType,bool)),Qt::DirectConnection);

    if ( checkSlot( plugin , "slotVolumeLassoSelection(QMouseEvent*,SelectionInterface::PrimitiveType,bool)" ) )
      connect(this   , SIGNAL(volumeLassoSelection(QMouseEvent*,SelectionInterface::PrimitiveType,bool)),
              plugin , SLOT(slotVolumeLassoSelection(QMouseEvent*,SelectionInterface::PrimitiveType,bool)),Qt::DirectConnection);
              
    if ( checkSignal(plugin,"surfaceLassoSelection(QMouseEvent*,SelectionInterface::PrimitiveType,bool)") )
      connect(plugin , SIGNAL(surfaceLassoSelection(QMouseEvent*,SelectionInterface::PrimitiveType,bool)),
              this   , SLOT(slotSurfaceLassoSelection(QMouseEvent*,SelectionInterface::PrimitiveType,bool)),Qt::DirectConnection);

    if ( checkSlot( plugin , "slotSurfaceLassoSelection(QMouseEvent*,SelectionInterface::PrimitiveType,bool)" ) )
      connect(this   , SIGNAL(surfaceLassoSelection(QMouseEvent*,SelectionInterface::PrimitiveType,bool)),
              plugin , SLOT(slotSurfaceLassoSelection(QMouseEvent*,SelectionInterface::PrimitiveType,bool)),Qt::DirectConnection);
              
    if ( checkSignal(plugin,"sphereSelection(QMouseEvent*,double,SelectionInterface::PrimitiveType,bool)") )
      connect(plugin , SIGNAL(sphereSelection(QMouseEvent*,double,SelectionInterface::PrimitiveType,bool)),
              this   , SLOT(slotSphereSelection(QMouseEvent*,double,SelectionInterface::PrimitiveType,bool)),Qt::DirectConnection);

    if ( checkSlot( plugin , "slotSphereSelection(QMouseEvent*,double,SelectionInterface::PrimitiveType,bool)" ) )
      connect(this   , SIGNAL(sphereSelection(QMouseEvent*,double,SelectionInterface::PrimitiveType,bool)),
              plugin , SLOT(slotSphereSelection(QMouseEvent*,double,SelectionInterface::PrimitiveType,bool)),Qt::DirectConnection);
              
    if ( checkSignal(plugin,"closestBoundarySelection(QMouseEvent*,SelectionInterface::PrimitiveType,bool)") )
      connect(plugin , SIGNAL(closestBoundarySelection(QMouseEvent*,SelectionInterface::PrimitiveType,bool)),
              this   , SLOT(slotClosestBoundarySelection(QMouseEvent*,SelectionInterface::PrimitiveType,bool)),Qt::DirectConnection);

    if ( checkSlot( plugin , "slotClosestBoundarySelection(QMouseEvent*,SelectionInterface::PrimitiveType,bool)" ) )
      connect(this   , SIGNAL(closestBoundarySelection(QMouseEvent*,SelectionInterface::PrimitiveType,bool)),
              plugin , SLOT(slotClosestBoundarySelection(QMouseEvent*,SelectionInterface::PrimitiveType,bool)),Qt::DirectConnection);
              
    if ( checkSignal(plugin,"floodFillSelection(QMouseEvent*,double,SelectionInterface::PrimitiveType,bool)") )
      connect(plugin , SIGNAL(floodFillSelection(QMouseEvent*,double,SelectionInterface::PrimitiveType,bool)),
              this   , SLOT(slotFloodFillSelection(QMouseEvent*,double,SelectionInterface::PrimitiveType,bool)),Qt::DirectConnection);

    if ( checkSlot( plugin , "slotFloodFillSelection(QMouseEvent*,double,SelectionInterface::PrimitiveType,bool)" ) )
      connect(this   , SIGNAL(floodFillSelection(QMouseEvent*,double,SelectionInterface::PrimitiveType,bool)),
              plugin , SLOT(slotFloodFillSelection(QMouseEvent*,double,SelectionInterface::PrimitiveType,bool)),Qt::DirectConnection);

    if ( checkSignal(plugin,"componentsSelection(QMouseEvent*,SelectionInterface::PrimitiveType,bool)") )
      connect(plugin , SIGNAL(componentsSelection(QMouseEvent*,SelectionInterface::PrimitiveType,bool)),
              this   , SLOT(slotComponentsSelection(QMouseEvent*,SelectionInterface::PrimitiveType,bool)),Qt::DirectConnection);

    if ( checkSlot( plugin , "slotComponentsSelection(QMouseEvent*,SelectionInterface::PrimitiveType,bool)" ) )
      connect(this   , SIGNAL(componentsSelection(QMouseEvent*,SelectionInterface::PrimitiveType,bool)),
              plugin , SLOT(slotComponentsSelection(QMouseEvent*,SelectionInterface::PrimitiveType,bool)),Qt::DirectConnection);
              
    if ( checkSignal(plugin,"customSelection(QMouseEvent*,SelectionInterface::PrimitiveType,QString,bool)") )
      connect(plugin , SIGNAL(customSelection(QMouseEvent*,SelectionInterface::PrimitiveType,QString,bool)),
              this   , SLOT(slotCustomSelection(QMouseEvent*,SelectionInterface::PrimitiveType,QString,bool)),Qt::DirectConnection);

    if ( checkSlot( plugin , "slotCustomSelection(QMouseEvent*,SelectionInterface::PrimitiveType,QString,bool)" ) )
      connect(this   , SIGNAL(customSelection(QMouseEvent*,SelectionInterface::PrimitiveType,QString,bool)),
              plugin , SLOT(slotCustomSelection(QMouseEvent*,SelectionInterface::PrimitiveType,QString,bool)),Qt::DirectConnection);
              
    if ( checkSignal(plugin,"getActiveDataTypes(SelectionInterface::TypeList&)") )
      connect(plugin , SIGNAL(getActiveDataTypes(SelectionInterface::TypeList&)),
              this   , SLOT(slotGetActiveDataTypes(SelectionInterface::TypeList&)),Qt::DirectConnection);

    if ( checkSlot( plugin , "slotGetActiveDataTypes(SelectionInterface::TypeList&)" ) )
      connect(this   , SIGNAL(getActiveDataTypes(SelectionInterface::TypeList&)),
              plugin , SLOT(slotGetActiveDataTypes(SelectionInterface::TypeList&)),Qt::DirectConnection);
              
    if ( checkSignal(plugin,"getActivePrimitiveType(SelectionInterface::PrimitiveType&)") )
      connect(plugin , SIGNAL(getActivePrimitiveType(SelectionInterface::PrimitiveType&)),
              this   , SLOT(slotGetActivePrimitiveType(SelectionInterface::PrimitiveType&)),Qt::DirectConnection);

    if ( checkSlot( plugin , "slotGetActivePrimitiveType(SelectionInterface::PrimitiveType&)" ) )
      connect(this   , SIGNAL(getActivePrimitiveType(SelectionInterface::PrimitiveType&)),
              plugin , SLOT(slotGetActivePrimitiveType(SelectionInterface::PrimitiveType&)),Qt::DirectConnection);
              
    if ( checkSignal(plugin,"targetObjectsOnly(bool&)") )
      connect(plugin , SIGNAL(targetObjectsOnly(bool&)),
              this   , SLOT(slotTargetObjectsOnly(bool&)),Qt::DirectConnection);

    if ( checkSlot( plugin , "slotTargetObjectsOnly(bool&)" ) )
      connect(this   , SIGNAL(targetObjectsOnly(bool&)),
              plugin , SLOT(slotTargetObjectsOnly(bool&)),Qt::DirectConnection);
              
    if ( checkSignal(plugin,"loadSelection(const INIFile&)") )
      connect(plugin , SIGNAL(loadSelection(const INIFile&)),
              this   , SLOT(slotLoadSelection(const INIFile&)),Qt::DirectConnection);

    if ( checkSlot( plugin , "slotLoadSelection(const INIFile&)" ) )
      connect(this   , SIGNAL(loadSelection(const INIFile&)),
              plugin , SLOT(slotLoadSelection(const INIFile&)),Qt::DirectConnection);
              
    if ( checkSignal(plugin,"saveSelection(INIFile&)") )
      connect(plugin , SIGNAL(saveSelection(INIFile&)),
              this   , SLOT(slotSaveSelection(INIFile&)),Qt::DirectConnection);

    if ( checkSlot( plugin , "slotSaveSelection(INIFile&)" ) )
      connect(this   , SIGNAL(saveSelection(INIFile&)),
              plugin , SLOT(slotSaveSelection(INIFile&)),Qt::DirectConnection);
              
    if ( checkSignal(plugin,"registerKeyShortcut(int,Qt::KeyboardModifiers)") )
      connect(plugin , SIGNAL(registerKeyShortcut(int,Qt::KeyboardModifiers)),
              this   , SLOT(slotRegisterKeyShortcut(int,Qt::KeyboardModifiers)),Qt::DirectConnection);

    if ( checkSlot( plugin , "slotRegisterKeyShortcut(int,Qt::KeyboardModifiers)" ) )
      connect(this   , SIGNAL(registerKeyShortcut(int,Qt::KeyboardModifiers)),
              plugin , SLOT(slotRegisterKeyShortcut(int,Qt::KeyboardModifiers)),Qt::DirectConnection);
              
    if ( checkSignal(plugin,"keyShortcutEvent(int,Qt::KeyboardModifiers)") )
      connect(plugin , SIGNAL(keyShortcutEvent(int,Qt::KeyboardModifiers)),
              this   , SLOT(slotKeyShortcutEvent(int,Qt::KeyboardModifiers)),Qt::DirectConnection);

    if ( checkSlot( plugin , "slotKeyShortcutEvent(int,Qt::KeyboardModifiers)" ) )
      connect(this   , SIGNAL(keyShortcutEvent(int,Qt::KeyboardModifiers)),
              plugin , SLOT(slotKeyShortcutEvent(int,Qt::KeyboardModifiers)),Qt::DirectConnection);
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
              
    if ( checkSignal( plugin , "textureIndexPropertyName(int,QString&)" ) )
      connect(plugin   , SIGNAL(textureIndexPropertyName(int,QString&)),
              this ,   SLOT(slotTextureIndexPropertyName(int,QString&)),Qt::DirectConnection);
    
    if ( checkSlot( plugin , "slotTextureIndexPropertyName(int,QString&)" ) )
      connect(this   , SIGNAL(textureIndexPropertyName(int,QString&)),
              plugin , SLOT(slotTextureIndexPropertyName(int,QString&)),Qt::DirectConnection);
              
    if ( checkSignal( plugin , "textureName(int,int,QString&)" ) )
      connect(plugin   , SIGNAL(textureName(int,int,QString&)),
              this ,   SLOT(slotTextureName(int,int,QString&)),Qt::DirectConnection);

    if ( checkSlot( plugin , "slotTextureName(int,int,QString&)" ) )
      connect(this   , SIGNAL(textureName(int,int,QString&)),
              plugin , SLOT(slotTextureName(int,int,QString&)),Qt::DirectConnection);
              
    if ( checkSignal( plugin , "textureFilename(int,QString,QString&)" ) )
      connect(plugin , SIGNAL(textureFilename(int,QString,QString&)),
              this   , SLOT(slotTextureFilename(int,QString,QString&)),Qt::DirectConnection);
              
    if ( checkSlot( plugin , "slotTextureFilename(int,QString,QString&)" ) )
      connect(this   , SIGNAL(textureFilename(int,QString,QString&)),
              plugin , SLOT(slotTextureFilename(int,QString,QString&)),Qt::DirectConnection);
              
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
    if ( checkSignal( plugin , "createBackup(int,QString,UpdateType)" ) ) {
      connect(plugin , SIGNAL(createBackup(int,QString,UpdateType)) ,
              this   , SIGNAL(createBackup(int,QString,UpdateType)),Qt::DirectConnection );
    }
    // Signal send from core to plugins that they should create a backup
    if ( checkSlot( plugin , "slotCreateBackup(int,QString,UpdateType)" ) ) {
      connect(this   , SIGNAL(createBackup(int,QString,UpdateType)),
              plugin , SLOT( slotCreateBackup(int,QString,UpdateType) ),Qt::DirectConnection);
    }
    
    // Incoming Signal that a backup should be created 
    if ( checkSignal( plugin , "createBackup(IdList,QString,std::vector<UpdateType>)" ) ) {
      connect(plugin , SIGNAL(createBackup(IdList,QString,std::vector<UpdateType>)) ,
              this   , SIGNAL(createBackup(IdList,QString,std::vector<UpdateType>)),Qt::DirectConnection );
    }
    // Signal send from core to plugins that they should create a backup
    if ( checkSlot( plugin , "slotCreateBackup(IdList,QString,std::vector<UpdateType>)" ) ) {
      connect(this   , SIGNAL(createBackup(IdList,QString,std::vector<UpdateType>)),
              plugin , SLOT( slotCreateBackup(IdList,QString,std::vector<UpdateType>) ),Qt::DirectConnection);
    }


    // Signal from plugin to restore an object with the given id
    if ( checkSignal( plugin , "undo(int)" ) ) {
      connect(plugin , SIGNAL(undo(int)) ,
              this   , SIGNAL(undo(int)),Qt::DirectConnection );
    }
    
    // Signal send from core to backup plugin that it should restore the given object
    if ( checkSlot( plugin , "slotUndo(int)" ) ) {
      connect(this   , SIGNAL(undo(int)),
              plugin , SLOT( slotUndo(int) ),Qt::DirectConnection);
    }

    // Signal from plugin to restore an object with the given id
    if ( checkSignal( plugin , "redo(int)" ) ) {
      connect(plugin , SIGNAL(redo(int)) ,
              this   , SIGNAL(redo(int)),Qt::DirectConnection );
    }
    
    // Signal send from core to backup plugin that it should restore the given object
    if ( checkSlot( plugin , "slotRedo(int)" ) ) {
      connect(this   , SIGNAL(redo(int)),
              plugin , SLOT( slotRedo(int) ),Qt::DirectConnection);
    }

    // Signal from plugin to restore an object with the given id
    if ( checkSignal( plugin , "undo()" ) ) {
      connect(plugin , SIGNAL(undo()) ,
              this   , SIGNAL(undo()),Qt::DirectConnection );
    }
    
    // Signal send from core to backup plugin that it should restore the given object
    if ( checkSlot( plugin , "slotUndo()" ) ) {
      connect(this   , SIGNAL(undo()),
              plugin , SLOT( slotUndo() ),Qt::DirectConnection);
    }

    // Signal from plugin to restore an object with the given id
    if ( checkSignal( plugin , "redo()" ) ) {
      connect(plugin , SIGNAL(redo()) ,
              this   , SIGNAL(redo()),Qt::DirectConnection );
    }
    
    // Signal send from core to backup plugin that it should restore the given object
    if ( checkSlot( plugin , "slotRedo()" ) ) {
      connect(this   , SIGNAL(redo()),
              plugin , SLOT( slotRedo() ),Qt::DirectConnection);
    }

    //====================================================================================
    // Backup Plugin signals for communication with the other plugins about restore state
    //====================================================================================
    
    // Stage one : restore will happen soon
    if ( checkSignal( plugin , "aboutToRestore(int)" ) ) {
      connect(plugin  , SIGNAL( aboutToRestore(int)) ,
              this    , SIGNAL( aboutToRestore(int) ),Qt::DirectConnection);
    }
    
    // Stage two: Restore complete
    if ( checkSignal( plugin , "restored(int)" ) ) {
      connect(plugin  , SIGNAL(restored(int)) ,
              this    , SIGNAL( restored(int) ),Qt::DirectConnection);
    }
    
    //====================================================================================
    //  Plugin slots about restore state
    //====================================================================================
     
     // Stage one : restore will happen soon
     if ( checkSlot( plugin , "slotAboutToRestore(int)" ) ) {
       connect(this   , SIGNAL( aboutToRestore(int)) ,
               plugin , SLOT( slotAboutToRestore(int) ),Qt::DirectConnection);
     }
     
     // Stage two : restore will happen soon
     if ( checkSlot( plugin , "slotRestored(int)" ) ) {
       connect(this   , SIGNAL( restored(int)) ,
               plugin , SLOT( slotRestored(int) ),Qt::DirectConnection);
     }
     
     // Signal from plugin to restore a group with the given id
     if ( checkSignal( plugin , "generateBackup(int,QString,UpdateType)" ) ) {
       connect(plugin ,  SIGNAL(generateBackup(int,QString,UpdateType)) ,
              this   , SLOT(slotGenerateBackup(int,QString,UpdateType)),Qt::DirectConnection );
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

    if ( checkSignal(plugin,"deleteObject(int)" ) )
      connect(plugin , SIGNAL( deleteObject( int ) ) ,
              this   , SLOT( deleteObject( int ) ),Qt::DirectConnection);

    if ( checkSignal(plugin,"deleteAllObjects()" ) )
      connect(plugin , SIGNAL( deleteAllObjects() ) ,
              this   , SLOT( slotDeleteAllObjects() ),Qt::DirectConnection);

   if ( checkSignal(plugin,"getAllFileFilters(QStringList&)" ) )
      connect(plugin , SIGNAL( getAllFileFilters(QStringList&) ) ,
              this   , SLOT( slotGetAllFilters(QStringList&) ),Qt::DirectConnection);

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
  
  //Check if the plugin supports PluginConnectionInterface
  PluginConnectionInterface* interconnectionPlugin = qobject_cast< PluginConnectionInterface * >(plugin);
  if ( interconnectionPlugin ) {
    supported = supported + "Plugin Interconnection ";
    
    if ( checkSignal(plugin,"crossPluginConnect(QString,const char*,QString,const char*)" ) ) {
      connect(plugin  , SIGNAL( crossPluginConnect(QString,const char*,QString,const char*) ),
              this    , SLOT( slotCrossPluginConnect(QString,const char*,QString,const char*) ));
    }
  }

  //Check if the plugin supports RenderInterface
  RenderInterface* renderPlugin = qobject_cast< RenderInterface * >(plugin);
  if ( renderPlugin ) {
    supported = supported + "Rendering ";

    if ( checkSlot( plugin , "rendererName()" ) ) {
      QString rendererNameString = "";

      // Get the name of the renderer
      QMetaObject::invokeMethod(plugin,"rendererName", Qt::DirectConnection,   Q_RETURN_ARG(QString,rendererNameString) ) ;

      // Let the plugin check its OpenGL support requirements
      QString openGLCheck = "";
      QMetaObject::invokeMethod(plugin,"checkOpenGL", Qt::DirectConnection,   Q_RETURN_ARG(QString,openGLCheck) ) ;

      if ( openGLCheck != "" ) {
        emit log(LOGERR,tr("Error: Insufficient OpenGL capabilities in Plugin") + rendererNameString + " !");
        emit log(LOGERR,openGLCheck);
      }

      // Check if it already exists and add it if not.
      RendererInfo* rendererInfo = 0;
      if ( ! renderManager().rendererExists(rendererNameString) ) {
        rendererInfo = renderManager().newRenderer(rendererNameString);
      } else {
        emit log(LOGERR,tr("Error: Renderer Plugin %1 already exists").arg(rendererNameString));
      }




      // Retrieve and store renderer information
      if ( rendererInfo != 0) {
        rendererInfo->plugin = renderPlugin;
        rendererInfo->name          = basePlugin->name();
        rendererInfo->version       = basePlugin->version();
        rendererInfo->description   = basePlugin->description();

        ACG::SceneGraph::DrawModes::DrawMode supportedModes;

        // Get the supported draw modes of the renderer
        QMetaObject::invokeMethod(plugin,"supportedDrawModes", Q_ARG(ACG::SceneGraph::DrawModes::DrawMode& ,supportedModes) );

        rendererInfo->modes = supportedModes;

        if ( checkSlot( plugin , "optionsAction()" ) ) {
          //Get an action for the post processor options
          rendererInfo->optionsAction = renderPlugin->optionsAction();

        } else {
          rendererInfo->optionsAction = 0;
        }
      }

    } else {
      emit log(LOGERR,tr("Error: Renderer Plugin without rendererName Function?!"));
    }

  }

  //Check if the plugin supports PostProcessorInterface
  PostProcessorInterface* postProcessorPlugin = qobject_cast< PostProcessorInterface * >(plugin);
  if ( postProcessorPlugin ) {
    supported = supported + "PostProcessor ";

    if ( checkSlot( plugin , "postProcessorName()" ) ) {
      QString postProcessorNameString = "";

      // Get the name of the PostProcessor
      QMetaObject::invokeMethod(plugin,"postProcessorName", Qt::DirectConnection,   Q_RETURN_ARG(QString,postProcessorNameString) ) ;

      // Check if it already exists and add it if not.
      PostProcessorInfo* postProcessorInfo = 0;
      if ( ! postProcessorManager().postProcessorExists(postProcessorNameString) ) {
        postProcessorInfo = postProcessorManager().newPostProcessor(postProcessorNameString);
      } else {
        emit log(LOGERR,tr("Error: PostProcessor Plugin %1 already exists").arg(postProcessorNameString));
      }

      // Retrieve and store PostProcessor information
      if ( postProcessorInfo != 0) {
        postProcessorInfo->plugin        = postProcessorPlugin;
        postProcessorInfo->name          = basePlugin->name();
        postProcessorInfo->version       = basePlugin->version();
        postProcessorInfo->description   = basePlugin->description();

        if ( checkSlot( plugin , "optionsAction()" ) ) {
          //Get an action for the post processor options
          postProcessorInfo->optionsAction = postProcessorPlugin->optionsAction();

        } else {
          postProcessorInfo->optionsAction = 0;
        }
      }

    } else {
      emit log(LOGERR,tr("Error: PostProcessor Plugin without postProcessorName Function?!"));
    }
  }

  //Check if the plugin supports AboutInfo-Interface
  AboutInfoInterface* aboutInfoPlugin = qobject_cast< AboutInfoInterface * >(plugin);
  if ( aboutInfoPlugin && OpenFlipper::Options::gui() ) {
    supported = supported + "AboutInfo ";

    if ( checkSignal(plugin,"addAboutInfo(QString,QString)") )
      connect(plugin      , SIGNAL(addAboutInfo(QString,QString)),
              coreWidget_ , SLOT(addAboutInfo(QString,QString)),Qt::DirectConnection);
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
  QString scriptingName = info.rpcName;

  scriptEngine_.globalObject().setProperty(scriptingName, scriptInstance);

  QScriptValueIterator it(scriptInstance);
  while (it.hasNext()) {
    it.next();

    /// Skip all signals for function calls
    if ( checkSignal( plugin, it.name().toLatin1() ) )
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

    // Plugins to Core
    if ( checkSignal(plugin,"executeFileScript(QString)") )
      connect(plugin      , SIGNAL(executeFileScript(QString)),
              this       , SLOT(slotExecuteFileScript(QString)));

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

  info.status = PluginInfo::LOADED;
  if (alreadyLoadedAt != -1)
    plugins_[alreadyLoadedAt] = info;
  else
    plugins_.push_back(info);

  // Initialize Plugin
  if ( basePlugin ) {
    if ( checkSlot(plugin,"initializePlugin()") )
        QMetaObject::invokeMethod(plugin, "initializePlugin",  Qt::DirectConnection);
  }


  //Check if its a filePlugin
  FileInterface* filePlugin = qobject_cast< FileInterface * >(plugin);
  if ( filePlugin ){
    supported = supported + "File ";

    QStringList loadFilters = filePlugin->getLoadFilters().split(";;");
    QStringList saveFilters = filePlugin->getSaveFilters().split(";;");

    // Collect supported Data from file plugin
    for (int i = 0; i < loadFilters.size(); ++i) {
      fileTypes ft;
      ft.name = basePlugin->name();
      ft.type = filePlugin->supportedType();
      ft.loadFilters = loadFilters[i];
      ft.saveFilters = "";
      ft.plugin = filePlugin;
      ft.object = plugin;
      ft.saveMultipleObjects = checkSlot(plugin,"saveObjects(IdList,QString)");

      supportedTypes().push_back(ft);
    }
    for (int i = 0; i < saveFilters.size(); ++i) {
      fileTypes ft;
      ft.name = basePlugin->name();
      ft.type = filePlugin->supportedType();
      ft.loadFilters = "";
      ft.saveFilters = saveFilters[i];
      ft.plugin = filePlugin;
      ft.object = plugin;
      ft.saveMultipleObjects = checkSlot(plugin,"saveObjects(IdList,QString)");

      supportedTypes().push_back(ft);
    }


    if ( checkSignal(plugin,"openedFile(int)" ) )
      connect(plugin , SIGNAL( openedFile( int ) ) ,
              this   , SLOT( slotFileOpened ( int ) ),Qt::DirectConnection);
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
