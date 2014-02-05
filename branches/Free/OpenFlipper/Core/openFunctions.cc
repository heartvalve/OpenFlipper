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
*   $LastChangedBy$                                                *
*   $Date$                     *
*                                                                            *
\*===========================================================================*/





#include "Core.hh"

#include <ACG/QtWidgets/QtFileDialog.hh>
#include <ACG/Scenegraph/SceneGraphAnalysis.hh>

#include "OpenFlipper/common/GlobalOptions.hh"
#include <OpenFlipper/common/RecentFiles.hh>
#include "OpenFlipper/BasePlugin/PluginFunctions.hh"

#include "OpenFlipper/widgets/loadWidget/loadWidget.hh"
#include "OpenFlipper/widgets/addEmptyWidget/addEmptyWidget.hh"

#include <OpenFlipper/common/Types.hh>
#include <ObjectTypes/PolyMesh/PolyMesh.hh>

#include <OpenFlipper/common/DataTypes.hh>

void Core::resetScenegraph( bool _resetTrackBall  ) {

  if ( OpenFlipper::Options::gui() && !OpenFlipper::Options::sceneGraphUpdatesBlocked() ) {

    unsigned int maxPases = 1;
    ACG::Vec3d bbmin,bbmax;
    ACG::SceneGraph::analyzeSceneGraph(PluginFunctions::getSceneGraphRootNode(),maxPases,bbmin,bbmax);

    for ( unsigned int i = 0 ; i < OpenFlipper::Options::examinerWidgets() ; ++i ) {
      // update scene graph (get new bounding box and set projection right, including near and far plane)
      PluginFunctions::viewerProperties(i).lockUpdate();
      coreWidget_->examiner_widgets_[i]->sceneGraph(root_node_scenegraph_,maxPases,bbmin,bbmax, _resetTrackBall );
      PluginFunctions::viewerProperties(i).unLockUpdate();
      coreWidget_->examiner_widgets_[i]->updateGL();
    }

  }

}

//========================================================================================
// ===            Open/Add-Empty Functions                    ============================
//========================================================================================



void Core::slotGetAllFilters ( QStringList& _list){
  /// \todo check why the supported Type is used here!
  // Iterate over all types
  for (int i=0; i < (int)supportedTypes().size(); i++){
    QString f = supportedTypes()[i].plugin->getLoadFilters();
    f = f.section(")",0,0).section("(",1,1).trimmed();
    _list << f;
  }
}

void Core::commandLineOpen(const char* _filename, bool _asPolyMesh ){

  QString file(_filename);

  // Modify filename to contain full paths if they were given as relative paths
  if ( !file.startsWith("/") && !file.contains(":") ) {
    file = QDir::currentPath();
    file += OpenFlipper::Options::dirSeparator();
    file += _filename;
  }

  // Add to the open list
  commandLineFileNames_.push_back(std::pair< std::string , bool >(file.toStdString(), _asPolyMesh));
}

void Core::commandLineScript(const char* _filename ) {

  QString file(_filename);
  
  // Modify filename to contain full paths if they were given as relative paths
  if ( !file.startsWith("/") && !file.contains(":") ) {
    file = QDir::currentPath();
    file += OpenFlipper::Options::dirSeparator();
    file += _filename;
  }
  
  // Add to the open list
  commandLineScriptNames_.push_back(file.toStdString());
}

void Core::slotExecuteAfterStartup() {


  // Update logger
  if ( OpenFlipper::Options::gui())
    coreWidget_->updateInSceneLoggerGeometry();

  //check if we have scripting support:
  bool scriptingSupport = false;
  slotPluginExists("scripting",scriptingSupport);
  if ( ! scriptingSupport ) {
    emit log(LOGERR ,tr("No scripting support available, please check if we load a scripting plugin .. Skipping script execution on startup"));
  }

  // Collect all script files from the scripting subdirectory and execute them if possible.
  // You can use this directory to execute scipts that modify for example modify the ui at
  // every startup.
  if ( scriptingSupport ) {

    // Get the files in the directory
    QDir scriptDir = OpenFlipper::Options::scriptDir();
    QStringList scriptFiles = scriptDir.entryList(QDir::Files,QDir::Name);

    // Execute all files ending with ofs
    for ( int i = 0 ; i  < scriptFiles.size(); ++i )
      if ( scriptFiles[i].endsWith("ofs",Qt::CaseInsensitive) )
        emit executeFileScript(scriptDir.path() + "/" + scriptFiles[i]);

    // Clear scripting window afterexecuting the coresubdir scripts
    bool ok = false;
    slotCall( "scripting" ,"clearEditor()",ok);
  }

  OpenFlipper::Options::blockSceneGraphUpdates();

  // Open all files given at the commandline
  for ( uint i = 0 ; i < commandLineFileNames_.size() ; ++i ) {

    // Skip scripts here as they will be handled by a different function
    QString tmp = QString::fromStdString(commandLineFileNames_[i].first);
    if ( tmp.endsWith("ofs",Qt::CaseInsensitive) ) {
      commandLineScriptNames_.push_back(commandLineFileNames_[i].first);
      continue;
    }

    // If the file was given with the polymesh option, open them as polymeshes.
    if (commandLineFileNames_[i].second)
      loadObject(DATA_POLY_MESH, QString::fromStdString(commandLineFileNames_[i].first));
    else {
      loadObject(QString::fromStdString(commandLineFileNames_[i].first));
    }
  }

  OpenFlipper::Options::unblockSceneGraphUpdates();

  // Reset the scenegraph once to make sure everything is fine
  resetScenegraph( true );

  // If we have scripting support, execute the scripts given at the commandline.
  if ( scriptingSupport )
    for ( uint i = 0 ; i < commandLineScriptNames_.size() ; ++i ) {
      emit executeFileScript(QString::fromStdString(commandLineScriptNames_[i]));
    }

  // If we don't have a gui and we are not under remote control,
  // exit the application as there would be no way to execute further commands
  if ( !OpenFlipper::Options::gui() && !OpenFlipper::Options::remoteControl())
    exitApplication();
}


int Core::loadObject ( QString _filename ) {
  /** \todo Check if this function is ok. It should check all plugins for the given files and do not depend
            on Triangle meshes only! 
            Rewrite function to get the plugin only and then open the file. So iterate over all plugins and find the
            matching ones. open it with this plugin.
  */
  
  if (_filename.endsWith(".ini",Qt::CaseInsensitive)) {

    // Load all information from the given ini file
    openIniFile(_filename,true,true,true);

    if ( OpenFlipper::Options::gui() )
      coreWidget_->addRecent(_filename, DATA_UNKNOWN);

    return -2;
  } else if (_filename.endsWith(".ofs",Qt::CaseInsensitive)) {
     emit log(LOGINFO ,tr("Starting script execution of %1.").arg( _filename)) ;
     emit executeFileScript(_filename);
  } else {
    
    QFileInfo fi(_filename);

    for (int i=0; i < (int)supportedTypes().size(); i++){

      QString filters = supportedTypes()[i].loadFilters;

      // Only take the parts inside the brackets
      filters = filters.section("(",1).section(")",0,0);

      // Split into blocks
      QStringList separateFilters = filters.split(" ");

      bool found = false;

      // for all filters associated with this plugin
      for ( int filterId = 0 ; filterId < separateFilters.size(); ++filterId ) {
        separateFilters[filterId] = separateFilters[filterId].trimmed();

        //check extension
        if ( separateFilters[filterId].endsWith( "*." + fi.completeSuffix() , Qt::CaseInsensitive) ) {
          found = true;
          break;
        }

        if (  separateFilters[filterId].endsWith( "*." + fi.suffix() , Qt::CaseInsensitive) ) {
          found = true;
          emit log(LOGWARN,"Found supported datatype but only the suffix is matched not the complete suffix!");
          break;
        }

      }

      // continue processing only if found
      if ( ! found )
        continue;

      if ( OpenFlipper::Options::gui() ) {
        coreWidget_->statusMessage( tr("Loading %1 ... ").arg(_filename));
        if ( !OpenFlipper::Options::sceneGraphUpdatesBlocked() )
          coreWidget_->setStatus(ApplicationStatus::PROCESSING );
      }

      //load file
      int id = supportedTypes()[i].plugin->loadObject(_filename);

      if ( OpenFlipper::Options::gui() ) {
        if ( id != -1 ) {
          coreWidget_->statusMessage( tr("Loading %1 ... done").arg(_filename), 4000 );
          
          // Get the object to figure out the data type
          BaseObject* object;
          PluginFunctions::getObject(id,object);

          // Security check, if object really exists         
          if ( object != 0 ) { 

            // Add to recent files with the given datatype
            if ( OpenFlipper::Options::gui() )
              coreWidget_->addRecent(_filename, object->dataType());
          } else {
            emit log(LOGERR, tr("Unable to add recent as object with id %1 could not be found!").arg(id) );
          }
          
        } else
          coreWidget_->statusMessage( tr("Loading %1 ... failed!").arg(_filename), 4000 );

        if ( !OpenFlipper::Options::sceneGraphUpdatesBlocked() )
          coreWidget_->setStatus(ApplicationStatus::READY );
      }

      return id;
    }
  }

  emit log(LOGERR, tr("Unable to load object (type unknown). No suitable plugin found!") );

  return -1;
}

/// Function for loading a given file
int Core::loadObject( DataType _type, QString _filename) {
  /** \todo this function has to be checked. test for the plugin which can handle 
            the given file and then use it. 
  */
  
  if (_type == DATA_UNKNOWN)
    return loadObject(_filename);

  QFileInfo fi(_filename);
  
  std::vector<int> typeIds;
  for (int i=0; i < (int)supportedTypes().size(); i++) {
    if (supportedTypes()[i].type & _type || supportedTypes()[i].type == _type) {

      QString filters = supportedTypes()[i].loadFilters;

      // Only take the parts inside the brackets
      filters = filters.section("(",1).section(")",0,0);

      // Split into blocks
      QStringList separateFilters = filters.split(" ");

      // for all filters associated with this plugin
      for ( int filterId = 0 ; filterId < separateFilters.size(); ++filterId ) {
        separateFilters[filterId] = separateFilters[filterId].trimmed();

        //check extension
        if ( separateFilters[filterId].endsWith( "*." + fi.completeSuffix() , Qt::CaseInsensitive) ) {
          typeIds.push_back(i);
          continue;
        }

        if (  separateFilters[filterId].endsWith( "*." + fi.suffix() , Qt::CaseInsensitive) ) {
          typeIds.push_back(i);
          emit log(LOGWARN,"Found supported datatype but only the suffix is matched not the complete suffix!");
          continue;
        }
      }
    }
  }

  // load file with plugins
  size_t nPlugins = typeIds.size();
  if (nPlugins > 0) {

    int i = -1;

    // several plugins can load this type
    if (nPlugins > 1) {
      // let the user choose the plugin for loading
      if ( OpenFlipper::Options::gui() ) {
        QStringList items;
        for (size_t j = 0; j < nPlugins; ++j) {
          // only add the plugin name if it does not already exist in the itemlist
          if (items.indexOf(supportedTypes()[typeIds[j]].name) == -1)
            items << supportedTypes()[typeIds[j]].name;
        }

        bool ok;
        QString item = QInputDialog::getItem(coreWidget_, tr("File Plugins"),
                                             tr("Please choose a plugin for loading:"), items, 0, false, &ok);
        if (!ok) {
          emit log(LOGERR, tr("Unable to load object. No suitable plugin found!") );
          return -1; //no plugin found
        } else
          i = typeIds[items.indexOf(item)];
      }
      // if there is no gui just take the first one for now
      else {
        i = 0;
      }
    } else
      i = typeIds[0];

    if ( OpenFlipper::Options::gui() ) {
      coreWidget_->statusMessage( tr("Loading %1 ... ").arg(_filename));
      if ( !OpenFlipper::Options::sceneGraphUpdatesBlocked() )
        coreWidget_->setStatus(ApplicationStatus::PROCESSING );
    }

    int id = -1;

    //load file
    if ( checkSlot( supportedTypes()[i].object , "loadObject(QString,DataType)" ) )
      id = supportedTypes()[i].plugin->loadObject(_filename, _type);
    else
      id = supportedTypes()[i].plugin->loadObject(_filename);

    if ( OpenFlipper::Options::gui() ) {
      if ( id != -1 ) {
        coreWidget_->statusMessage( tr("Loading %1 ... done").arg(_filename), 4000 );

        // Get the object to figure out the data type
        BaseObject* object;
        PluginFunctions::getObject(id,object);

        // Add to recent files with the given datatype
        if ( OpenFlipper::Options::gui() )
          coreWidget_->addRecent(_filename, object->dataType());

      } else
        coreWidget_->statusMessage( tr("Loading %1 ... failed!").arg(_filename), 4000 );

      if ( !OpenFlipper::Options::sceneGraphUpdatesBlocked() )
        coreWidget_->setStatus(ApplicationStatus::READY );
    }

    return id;
  } else {

    emit log(LOGERR, tr("Unable to load object. No suitable plugin found!") );

    return -1; //no plugin found
  }
}


int Core::addEmptyObject( DataType _type ) {
  // Iterate over all plugins. The first plugin supporting the addEmpty function for the
  // specified type will be used to create the new object. If adding the object failed,
  // we iterate over the remaining plugins.
  
  // Iterate over type plugins
  for (int i=0; i < (int)supportedDataTypes_.size(); i++)
    if ( supportedDataTypes_[i].type & _type ) {
      int retCode = supportedDataTypes_[i].plugin->addEmpty();
      if ( retCode != -1 )
        return retCode;
    }
  
  return -1; // no plugin found
}

//========================================================================================
// ===             Open/Add-Empty Slots                       ============================
//========================================================================================

/// Slot for adding an empty object of given DataType
void Core::slotAddEmptyObject( DataType _type , int& _id ) {
  
  _id = addEmptyObject( _type );
  
  if ( OpenFlipper::Options::doSlotDebugging() ) {
    if ( sender() != 0 ) {
      if ( sender()->metaObject() != 0 ) {
        emit log(LOGINFO,"slotAddEmptyObject( " + _type.name() + "," + QString::number(_id) +  tr(" ) called by ") +
        QString( sender()->metaObject()->className() ) );
      }
    } else {
      emit log(LOGINFO,"slotAddEmptyObject( " + _type.name() + ","  + QString::number(_id) +  tr(" ) called by Core") );
    }
  }
}

/// Slot creating a copy of an existing object
void Core::slotCopyObject( int _oldId , int& _newId ) {

  if ( _oldId == -1 ) {
    emit log(LOGERR,tr("Requested copy for illegal Object id: %1").arg(_oldId) );
    _newId = -1;
    return;
  }

  // get the node
  BaseObject* object = objectRoot_->childExists(_oldId);

  if ( !object ) {
    emit log(LOGERR,tr("Requested copy for unknown Object id: %1 ").arg(_oldId) );
    _newId = -1;
    return ;
  }

  // Copy the item
  BaseObject* copy = object->copy();

  if ( copy == 0 ) {
    emit log(LOGERR,tr("Unable to create a copy of the object."));
    return;
  }

  // Integrate into object tree
  copy->setParent( object->parent() );

  // return the new id
  _newId = copy->id();

  // tell plugins that a new object has been created
  slotEmptyObjectAdded(_newId);

  // tell plugins that the object has been updated
  slotObjectUpdated(_newId);

}

/// Function for loading a given file
void Core::slotLoad(QString _filename, int _pluginID) {

  if ( OpenFlipper::Options::gui() ) {
    coreWidget_->statusMessage( tr("Loading %1 ... ").arg(_filename));
    if ( !OpenFlipper::Options::sceneGraphUpdatesBlocked() )
      coreWidget_->setStatus(ApplicationStatus::PROCESSING );
  }

  //load file
  int id = supportedTypes()[_pluginID].plugin->loadObject(_filename);

  if ( OpenFlipper::Options::gui() ) {
    if ( id != -1 )
      coreWidget_->statusMessage( tr("Loading %1 ... done").arg(_filename), 4000 );
    else
      coreWidget_->statusMessage( tr("Loading %1 ... failed!").arg(_filename), 4000 );

    if ( !OpenFlipper::Options::sceneGraphUpdatesBlocked() )
      coreWidget_->setStatus(ApplicationStatus::READY );
  }
  
  // Initialize as unknown type
  DataType type = DATA_UNKNOWN;

  // An object has been added. Get it and do some processing!
  if ( id > 0 ) {

    BaseObjectData* object;
    PluginFunctions::getObject(id,object);
    
    if ( !object ) {

      BaseObject* baseObj = 0;
      GroupObject* group = 0;
      
      PluginFunctions::getObject(id,baseObj);
      
      if (baseObj){

        group = dynamic_cast< GroupObject* > (baseObj);

        if (group)
          type = DATA_GROUP;
      }
      
      if ( group == 0 ){
        emit log(LOGERR,tr("Object id returned but no object with this id has been found! Error in one of the file plugins!"));
        return;
      }
    }
    
    // Get the objects type
    if (object)
      type = object->dataType();
  }
  
  // If the id was greater than zero, add the file to the recent files.
  if ( id >= 0 )
    if ( OpenFlipper::Options::gui() )
      coreWidget_->addRecent(_filename, type);
}

/// Slot for loading a given file
void Core::slotLoad(QString _filename, DataType _type, int& _id) {
  _id = loadObject(_type,_filename);

  if ( _id < 0 )
    _id = -1;
  else
    if ( OpenFlipper::Options::gui() )
      coreWidget_->addRecent(_filename,_type);
}

/// Slot gets called after a file-plugin has opened an object
void Core::slotFileOpened ( int _id ) {
  if ( OpenFlipper::Options::doSlotDebugging() ) {
    if ( sender() != 0 ) {
      if ( sender()->metaObject() != 0 ) {
        emit log(LOGINFO,tr("slotObjectOpened( ") + QString::number(_id) + tr(" ) called by ") +
                  QString( sender()->metaObject()->className() ) );
      }
    } else {
      emit log(LOGINFO,tr("slotObjectOpened( ") + QString::number(_id) + tr(" ) called by Core") );
    }
  }

  // get the opened object
  BaseObjectData* object;
  PluginFunctions::getObject(_id,object);

  // ================================================================================
  // Recompute bounding box and scenegraph info
  // Reset scene center here to include new object
  // ================================================================================
  resetScenegraph(true);

  // ================================================================================
  // Tell plugins, that a file has been opened
  // ================================================================================
  emit openedFile( _id );
  
  // ================================================================================
  // Print Info to logger
  // ================================================================================
  emit log( LOGINFO ,object->getObjectinfo() );

  // ================================================================================
  // Tell plugins, that the Object is updated and the active object has changed
  // ================================================================================
  emit signalObjectUpdated(_id );
  emit signalObjectUpdated(_id, UPDATE_ALL);

  // ================================================================================
  // Create initial backup
  // ================================================================================
  emit createBackup(_id,"Original Object");

  // ================================================================================
  // Add the file to the recent files menu
  // ================================================================================
  QString filename = object->path() + OpenFlipper::Options::dirSeparator() + object->name();
  BaseObject* object2;
  PluginFunctions::getObject(_id,object2);

  // ================================================================================
  // if this is the first object opend, reset the global view
  // ================================================================================
  if ( PluginFunctions::objectCount() == 1 && OpenFlipper::Options::gui() && !OpenFlipper::Options::sceneGraphUpdatesBlocked() )
    for ( unsigned int i = 0 ; i < OpenFlipper::Options::examinerWidgets() ; ++i ) {
      coreWidget_->examiner_widgets_[i]->viewAll();
    }

  // objectRoot_->dumpTree();
}

 /// Slot gets called after a file-plugin has opened an object
void Core::slotEmptyObjectAdded ( int _id ) {

  if ( OpenFlipper::Options::doSlotDebugging() ) {
    if ( sender() != 0 ) {
      if ( sender()->metaObject() != 0 ) {
        emit log(LOGINFO,tr("slotEmptyObjectAdded( ") + QString::number(_id) + tr(" ) called by ") +
                  QString( sender()->metaObject()->className() ) );
      }
    } else {
      emit log(LOGINFO,tr("slotEmptyObjectAdded( ") + QString::number(_id) + tr(" ) called by Core") );
    }
  }

  // get the opened object
  BaseObjectData* object;
  PluginFunctions::getObject(_id,object);

  
  emit emptyObjectAdded( _id );

  // Tell the Plugins that the Object List and the active object have changed
  emit signalObjectUpdated(_id);
  emit signalObjectUpdated(_id,UPDATE_ALL);

  resetScenegraph(false);

  ///@todo : set a default path for new objects
//    QString filename = object->path() + OpenFlipper::Options::dirSeparator() + object->name();

//    addRecent(filename);
}

//========================================================================================
// ===             Menu Slots                                 ============================
//========================================================================================

/// Opens AddEmpty-Object widget
void Core::slotAddEmptyObjectMenu() {
  std::vector< DataType > types;
  QStringList             typeNames;
  
  DataType currentType = 2;
  
  // Iterate over all Types known to the core
  // Start at 2:
  // 0 type is defined as DATA_UNKNOWN
  // 1 type is defined as DATA_GROUP
  // Therefore we have two types less then reported
  // 
  for ( uint i = 0 ; i < typeCount() - 2  ; ++i) {
    
    // Iterate over all supported types (created from plugins on load)
    // Check if a plugin supports addEmpty for the current type.
    // Only if the type is supported, add it to the addEmpty Dialog
    
    // typePlugin
    for ( uint j = 0 ; j < supportedDataTypes_.size(); j++) {
      
      // Check if a plugin supports the current type
      if ( supportedDataTypes_[j].type & currentType ) {
        types.push_back(currentType);
        typeNames.push_back( typeName( currentType ) );
        
        // Stop here as we need only one plugin supporting addEmpty for a given type
        break;
      }
    }
    
    // filePlugin
    for ( uint j = 0 ; j < supportedTypes().size(); j++) {
      
      // Check if a plugin supports the current Type
      if ( supportedTypes()[j].type & currentType ) {
	
	// Avoid duplicates
	bool duplicate = false;
	for(std::vector< DataType >::iterator it = types.begin(); it != types.end(); ++it) {
	  if(*it == currentType) {
	    duplicate = true;
	    break;
	  }
	}
	  
	if(!duplicate) {
	  types.push_back(currentType);
	  typeNames.push_back( typeName( currentType ) );
        
	  // Stop here as we need only one plugin supporting addEmpty for a given type
	  break;
	}
      }
    }
    
    // Advance to next type ( Indices are bits so multiply by two to get next bit)
    ++currentType;
  }
  
  

  if (supportedTypes().size() != 0) {
    
    static addEmptyWidget* widget = 0;

    if ( !widget ){
      widget = new addEmptyWidget(types,typeNames);
      widget->setWindowIcon( OpenFlipper::Options::OpenFlipperIcon() );
      connect(widget,SIGNAL(chosen(DataType, int&)),this,SLOT(slotAddEmptyObject(DataType, int&)));
    }
    
    widget->show();
    
  } else
    emit log(LOGERR,tr("Could not show 'add Empty' dialog. Missing file-plugins ?"));
  
}

//========================================================================================
// ===             Public Slots                                 ============================
//========================================================================================

/// Open Load-Object Widget
void Core::loadObject() {

  if ( OpenFlipper::Options::gui() ){

    if (supportedTypes().size() != 0){
      LoadWidget* widget = new LoadWidget(supportedTypes());
      connect(widget,SIGNAL(load(QString, int)),this,SLOT(slotLoad(QString, int)));
      connect(widget,SIGNAL(save(int, QString, int)),this,SLOT(saveObject(int, QString, int)));

      widget->setWindowIcon( OpenFlipper::Options::OpenFlipperIcon() );

      widget->showLoad();

      widget->disconnect();
      delete widget;

    }else
      emit log(LOGERR,tr("Could not show 'load objects' dialog. Missing file-plugins."));

  }
}

/// Load settings from file
void Core::loadSettings(){

  if ( OpenFlipper::Options::gui() ){

    QString complete_name;


    QFileDialog fileDialog( coreWidget_,
                            tr("Load Settings"),
                            OpenFlipperSettings().value("Core/CurrentDir").toString(),
                            tr("INI files (*.ini)") );

    fileDialog.setOption (QFileDialog::DontUseNativeDialog, true);
    fileDialog.setAcceptMode ( QFileDialog::AcceptOpen );
    fileDialog.setFileMode ( QFileDialog::AnyFile );

    QGridLayout *layout = (QGridLayout*)fileDialog.layout();

    QGroupBox* optionsBox = new QGroupBox( &fileDialog ) ;
    optionsBox->setSizePolicy( QSizePolicy ( QSizePolicy::Expanding , QSizePolicy::Preferred ) );
    optionsBox->setTitle(tr("Options"));
    layout->addWidget( optionsBox, layout->rowCount() , 0 , 1,layout->columnCount() );

    QCheckBox *loadProgramSettings = new QCheckBox(optionsBox);
    loadProgramSettings->setText(tr("Load program settings"));
    loadProgramSettings->setToolTip(tr("Load all current program settings from the file ( This will include view settings, colors,...) "));
    loadProgramSettings->setCheckState( Qt::Unchecked );

    QCheckBox *loadPluginSettings = new QCheckBox(optionsBox);
    loadPluginSettings->setText(tr("Load per Plugin Settings"));
    loadPluginSettings->setToolTip(tr("Plugins should load their current global settings from the file"));
    loadPluginSettings->setCheckState( Qt::Checked );

    QCheckBox *loadObjectInfo = new QCheckBox(optionsBox);
    loadObjectInfo->setText(tr("Load all objects defined in the file"));
    loadObjectInfo->setToolTip(tr("Load all objects which are defined in the file"));
    loadObjectInfo->setCheckState( Qt::Checked );

    QBoxLayout* frameLayout = new QBoxLayout(QBoxLayout::TopToBottom,optionsBox);
    frameLayout->addWidget( loadProgramSettings , 0 , 0);
    frameLayout->addWidget( loadPluginSettings  , 1 , 0);
    frameLayout->addWidget( loadObjectInfo      , 2 , 0);
    frameLayout->addStretch();

    fileDialog.resize(550 ,500);

    // ========================================================================================
    // show the saveSettings-Dialog and get the target file
    // ========================================================================================
    QStringList fileNames;
    if (fileDialog.exec()) {
      fileNames = fileDialog.selectedFiles();
    } else {
      return;
    }

    if ( fileNames.size() > 1 ) {
      std::cerr << "Too many save filenames selected" << std::endl;
      return;
    }

    complete_name = fileNames[0];


    QString newpath = complete_name.section(OpenFlipper::Options::dirSeparator(),0,-2);
    OpenFlipperSettings().setValue("Core/CurrentDir", newpath);

    if ( complete_name.endsWith("ini",Qt::CaseInsensitive) ) {
      openIniFile( complete_name,
                   loadProgramSettings->isChecked(),
                   loadPluginSettings->isChecked(),
                   loadObjectInfo->isChecked());
      if ( loadProgramSettings->isChecked() )
        applyOptions();
    } 
    
    coreWidget_->addRecent(complete_name, DATA_UNKNOWN);
  }
}

/// Load settings from file
void Core::loadSettings(QString _filename){

  if ( !QFile(_filename).exists() )
    return;

  QString newpath = _filename.section(OpenFlipper::Options::dirSeparator(),0,-2);
  OpenFlipperSettings().setValue("Core/CurrentDir", newpath);

  if ( _filename.endsWith("obj",Qt::CaseInsensitive) ) {
    loadObject(_filename);
    applyOptions();
  } else {
    // Loaded function for recent files. Load everything.
    openIniFile(_filename,true,true,true);
    applyOptions();
  }
}
