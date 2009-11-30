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





#include "Core.hh"

#include <ACG/QtWidgets/QtFileDialog.hh>

#include "OpenFlipper/common/GlobalOptions.hh"
#include "OpenFlipper/BasePlugin/PluginFunctions.hh"

#include "OpenFlipper/widgets/loadWidget/loadWidget.hh"
#include "OpenFlipper/widgets/addEmptyWidget/addEmptyWidget.hh"

#include <OpenFlipper/common/Types.hh>
#include <ObjectTypes/PolyMesh/PolyMesh.hh>

#include <time.h>

void Core::resetScenegraph( bool _resetTrackBall  ) {
  if ( OpenFlipper::Options::gui() && !OpenFlipper::Options::loadingSettings() ) {

    for ( unsigned int i = 0 ; i < OpenFlipper::Options::examinerWidgets() ; ++i ) {
      // update scene graph (get new bounding box and set projection right, including near and far plane)
      PluginFunctions::viewerProperties(i).lockUpdate();
      coreWidget_->examiner_widgets_[i]->sceneGraph(root_node_scenegraph_, _resetTrackBall );
      PluginFunctions::viewerProperties(i).unLockUpdate();
      coreWidget_->examiner_widgets_[i]->updateGL();
    }

  }

  // Update the draw Modes Menu
  if ( OpenFlipper::Options::gui() )
    coreWidget_->slotUpdateGlobalDrawMenu();
}

//========================================================================================
// ===            Open/Add-Empty Functions                    ============================
//========================================================================================


void Core::slotGetAllFilters ( QStringList& _list){

  // Iterate over all types
  for (int i=0; i < (int)supportedTypes_.size(); i++){
    QString f = supportedTypes_[i].plugin->getLoadFilters();
    f = f.section(")",0,0).section("(",1,1).trimmed();
    _list << (QString::number(supportedTypes_[i].plugin->supportedType()) + " " + f);
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
      if ( scriptFiles[i].endsWith("ofs") )
        emit executeFileScript(scriptDir.path() + QDir::separator() + scriptFiles[i]);

  }

  // Open all files given at the commandline
  for ( uint i = 0 ; i < commandLineFileNames_.size() ; ++i ) {

    // Skip scripts here as they will be handled by a different function
    QString tmp = QString::fromStdString(commandLineFileNames_[i].first);
    if ( tmp.endsWith("ofs") ) {
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
  
  if (_filename.endsWith(".ini")) {

    // Load all information from the given ini file
    openIniFile(_filename,true,true,true);

    if ( OpenFlipper::Options::gui() )
      coreWidget_->addRecent(_filename, DATA_UNKNOWN);

    return -2;
  } else
  if (_filename.endsWith(".ofs")) {
     emit log(LOGINFO ,tr("Starting script execution of %1.").arg( _filename)) ;
     emit executeFileScript(_filename);
  } else
    return loadObject( DATA_TRIANGLE_MESH, _filename);

  return -1;
}

/// Function for loading a given file
int Core::loadObject( DataType _type, QString _filename) {
  /** \todo this function has to be checked. test for the plugin which can handle 
            the given file and then use it. 
  */

  if (_type == DATA_UNKNOWN)
    return loadObject(_filename);

  for (int i=0; i < (int)supportedTypes_.size(); i++)
    if (supportedTypes_[i].type == _type) {


      if ( OpenFlipper::Options::gui() ) {
        coreWidget_->statusMessage( tr("Loading %1 ... ").arg(_filename));
        if ( !OpenFlipper::Options::loadingSettings() )
          coreWidget_->setStatus(ApplicationStatus::PROCESSING );
      }

      //load file
      int id = supportedTypes_[i].plugin->loadObject(_filename);

      if ( OpenFlipper::Options::gui() ) {
        if ( id != -1 )
          coreWidget_->statusMessage( tr("Loading %1 ... done").arg(_filename), 4000 );
        else
          coreWidget_->statusMessage( tr("Loading %1 ... failed!").arg(_filename), 4000 );

        if ( !OpenFlipper::Options::loadingSettings() )
          coreWidget_->setStatus(ApplicationStatus::READY );
      }

      return id;
    }
  return -1; //no plugin found
}


int Core::addEmptyObject( DataType _type ) {
  /** \todo Iterate over all plugins but check with bitmask! for a supporting plugin
  */
  for (int i=0; i < (int)supportedTypes_.size(); i++)
    if (supportedTypes_[i].type == _type)
      return supportedTypes_[i].plugin->addEmpty();
  return -1; //no plugin found
}

//========================================================================================
// ===             Open/Add-Empty Slots                       ============================
//========================================================================================

/// Slot for adding an empty object of given DataType
void Core::slotAddEmptyObject( DataType _type , int& _id ) {
  _id = addEmptyObject( _type );
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
  if ( object->parent() )
    object->parent()->appendChild(copy);
  else
    std::cerr << "Unable to add copy to object list" << std::endl;

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
    if ( !OpenFlipper::Options::loadingSettings() )
      coreWidget_->setStatus(ApplicationStatus::PROCESSING );
  }

  //load file
  int id = supportedTypes_[_pluginID].plugin->loadObject(_filename);

  if ( OpenFlipper::Options::gui() ) {
    if ( id != -1 )
      coreWidget_->statusMessage( tr("Loading %1 ... done").arg(_filename), 4000 );
    else
      coreWidget_->statusMessage( tr("Loading %1 ... failed!").arg(_filename), 4000 );

    if ( !OpenFlipper::Options::loadingSettings() )
      coreWidget_->setStatus(ApplicationStatus::READY );
  }
  
  // Initialize as unknown type
  DataType type = DATA_UNKNOWN;

  // An object has been added. Get it and do some processing!
  if ( id > 0 ) {
    /// \todo Move this code to polymesh plugin and do the check there!
    
    BaseObjectData* object;
    PluginFunctions::getObject(id,object);

    if ( !object ) {
      emit log(LOGERR,tr("Object id returned but no object with this id has been found! Error in one of the file plugins!"));
      return;
    }
    
    // Get the objects type
    type = object->dataType();
    
    // Check if it is a polymesh
    if ( object != 0 && type == DATA_POLY_MESH ) {

      PolyMeshObject* poly = 0;
      PluginFunctions::getObject(id,poly);

      if ( poly != 0 ) {
          PolyMesh& mesh = *poly->mesh();

          bool isTriangleMesh = true;

          for ( PolyMesh::FaceIter f_it = mesh.faces_begin(); f_it != mesh.faces_end() ; ++f_it) {

            // Count number of vertices for the current face
            uint count = 0;
            for ( PolyMesh::FaceVertexIter fv_it( mesh,f_it); fv_it; ++fv_it )
              ++count;

            // Check if it is a triangle. If not, this is really a poly mesh
            if ( count != 3 ) {
              isTriangleMesh = false;
              break;
            }

          }

          // Mesh loaded as polymesh is actually a triangle mesh. Ask the user to reload as triangle mesh or keep it as poly mesh.
          if ( isTriangleMesh ) {
            QMessageBox::StandardButton result = QMessageBox::question ( 0,
                                                                        tr("TriMesh loaded as PolyMesh"),
                                                                        tr("You opened the mesh as a poly mesh but actually its a triangle mesh. \nShould it be opened as a triangle mesh?"),
                                                                        (QMessageBox::Yes | QMessageBox::No ),
                                                                        QMessageBox::Yes );
            // User decided to reload as triangle mesh
            if ( result == QMessageBox::Yes ) {
              slotDeleteObject(id);
              id = loadObject(DATA_TRIANGLE_MESH ,_filename);
              type = DATA_TRIANGLE_MESH;
            }
          }

      }
    }
  }
  
  // If the id was greater than zero, add the file to the recent files.
  if ( id >= 0 )
    if ( OpenFlipper::Options::gui() )
      coreWidget_->addRecent(_filename, type);
}

/// Slot for loading a given file
void Core::slotLoad(QString _filename, DataType _type, int& _id) {
  _id = loadObject(_type,_filename);

  // Check if it is a polymesh
  if ( _type == DATA_POLY_MESH ) {

    PolyMeshObject* poly = 0;
    PluginFunctions::getObject(_id,poly);

    if ( poly != 0 ) {
        /// \todo Move this code to polymesh plugin and do the check there!
        PolyMesh& mesh = *poly->mesh();

        bool isTriangleMesh = true;

        for ( PolyMesh::FaceIter f_it = mesh.faces_begin(); f_it != mesh.faces_end() ; ++f_it) {

          // Count number of vertices for the current face
          uint count = 0;
          for ( PolyMesh::FaceVertexIter fv_it( mesh,f_it); fv_it; ++fv_it )
            ++count;

          // Check if it is a triangle. If not, this is really a poly mesh
          if ( count != 3 ) {
            isTriangleMesh = false;
            break;
          }

        }

        // Mesh loaded as polymesh is actually a triangle mesh. Ask the user to reload as triangle mesh or keep it as poly mesh.
        if ( isTriangleMesh ) {
          QMessageBox::StandardButton result = QMessageBox::question ( 0,
                                                                      tr("TriMesh loaded as PolyMesh"),
                                                                      tr("You opened the mesh as a poly mesh but actually its a triangle mesh. \nShould it be opened as a triangle mesh?"),
                                                                      (QMessageBox::Yes | QMessageBox::No ),
                                                                      QMessageBox::Yes );
          // User decided to reload as triangle mesh
          if ( result == QMessageBox::Yes ) {
            slotDeleteObject(_id);
            _id = loadObject(DATA_TRIANGLE_MESH ,_filename);
            _type = DATA_TRIANGLE_MESH;
          }
        }

    }
  }
  if ( _id < 0 )
    _id = -1;
  else
    if ( OpenFlipper::Options::gui() )
      coreWidget_->addRecent(_filename,_type);
}

/// Slot gets called after a file-plugin has opened an object
void Core::slotObjectOpened ( int _id ) {
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

  QColor color;

  if ( OpenFlipper::Options::randomBaseColor() ){
      //init random seed
      srand ( time(NULL) );

      QColor bckgrnd = OpenFlipper::Options::defaultBackgroundColor();
      int diff;

      do{
        color.setRgb(rand()%255, rand()%255, rand()%255);

        diff = (bckgrnd.red()   - color.red())  *(bckgrnd.red()   - color.red())
              +(bckgrnd.green() - color.green())*(bckgrnd.green() - color.green())
              +(bckgrnd.blue()  - color.blue()) *(bckgrnd.blue()  - color.blue());
      }while (diff < 70000);
  }
  else{
      color = OpenFlipper::Options::defaultBaseColor();
  }

  ACG::Vec4f colorV;
  colorV[0] = color.redF();
  colorV[1] = color.greenF();
  colorV[2] = color.blueF();
  colorV[3] = color.alphaF();

  object->setBaseColor( colorV );

  // ================================================================================
  // Set defaults for DrawMode, ViewingDirection and Projection if this is the first opened object
  // If a plugin changes the drawmode later, this setting will be overridden!
  // ================================================================================
  if ( PluginFunctions::objectCount() == 1 && OpenFlipper::Options::gui() && !OpenFlipper::Options::loadingSettings() )
    for ( unsigned int i = 0 ; i < OpenFlipper::Options::examinerWidgets() ; ++i ){
      PluginFunctions::viewerProperties(i).drawMode( OpenFlipper::Options::defaultDrawMode(i) );

      if ( OpenFlipper::Options::defaultProjectionMode(i) == 0 )
	PluginFunctions::orthographicProjection(i);
      else
	PluginFunctions::perspectiveProjection(i);

      PluginFunctions::setFixedView(OpenFlipper::Options::defaultViewingDirection(i), i );
    }

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
  // Tell plugins, that the Object is updated and the active object has changed
  // ================================================================================
  emit signalObjectUpdated(_id);
  emit objectSelectionChanged( _id );

  // ================================================================================
  // Create initial backup
  // ================================================================================
  backupRequest(_id,"Original Object");

  // ================================================================================
  // Add the file to the recent files menu
  // ================================================================================
  QString filename = object->path() + OpenFlipper::Options::dirSeparator() + object->name();
  BaseObject* object2;
  PluginFunctions::getObject(_id,object2);
  if ( OpenFlipper::Options::gui() )
    coreWidget_->addRecent( filename, object2->dataType() );

  // ================================================================================
  // if this is the first object opend, reset the global view
  // ================================================================================
  if ( PluginFunctions::objectCount() == 1 && OpenFlipper::Options::gui() && !OpenFlipper::Options::loadingSettings() )
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
  emit objectSelectionChanged(_id);

  backupRequest(_id,"Original Object");

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
    
    // iterate over all support types (created from plugins on load.
    // Check if a plugin supports addEmpty for the current type.
    // Only if the type is supported, add it to the addEmpty Dialog
    for ( uint j = 0 ; j < supportedTypes_.size(); j++) {
      
      // Check if a plugin supports the current Type
      if ( supportedTypes_[j].type & currentType ) {
        types.push_back(currentType);
        typeNames.push_back( typeName( currentType ) );
        
        // Stop here as we need only one plugin supporting addEmpty for a given type
        break;
      }
    }
    
    // Advance to next type ( Indices are bits so multiply by to to get next bit)
    currentType *= 2;
  }
  
  static addEmptyWidget* widget = 0;
  
  if (supportedTypes_.size() != 0) {
    
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

    if (supportedTypes_.size() != 0){
      LoadWidget* widget = new LoadWidget(supportedTypes_);
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
                            OpenFlipper::Options::currentDirStr(),
                            tr("INI files (*.ini);;OBJ files (*.obj )") );

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
    OpenFlipper::Options::currentDir(newpath);

    if ( complete_name.endsWith("ini") ) {
      openIniFile( complete_name,
                   loadProgramSettings->isChecked(),
                   loadPluginSettings->isChecked(),
                   loadObjectInfo->isChecked());
      if ( loadProgramSettings->isChecked() )
        applyOptions();
    } else if ( complete_name.endsWith("obj") ) {
      openObjFile(complete_name);
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
  OpenFlipper::Options::currentDir(newpath);

  if ( _filename.endsWith("ini") ) {
    // Loaded function for recent files. Load everything.
    openIniFile(_filename,true,true,true);
    applyOptions();
  } else if ( _filename.endsWith("obj") ) {
    openObjFile(_filename);
    applyOptions();
  }

}
