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





#include "Core.hh"

#include <ACG/QtWidgets/QtFileDialog.hh>

#include "OpenFlipper/common/GlobalOptions.hh"
#include "OpenFlipper/BasePlugin/PluginFunctions.hh"

#include "OpenFlipper/widgets/loadWidget/loadWidget.hh"
#include "OpenFlipper/widgets/addEmptyWidget/addEmptyWidget.hh"

void Core::resetScenegraph() {
  if ( OpenFlipper::Options::gui() && !OpenFlipper::Options::loadingSettings() ) {

    for ( unsigned int i = 0 ; i < OpenFlipper::Options::examinerWidgets() ; ++i ) {
      // update scene graph (get new bounding box and set projection right, including near and far plane)
      PluginFunctions::viewerProperties(i).lockUpdate();
      coreWidget_->examiner_widgets_[i]->sceneGraph(root_node_scenegraph_);
      PluginFunctions::viewerProperties(i).unLockUpdate();
      coreWidget_->examiner_widgets_[i]->updateGL();
    }

  }
}

//========================================================================================
// ===            Open/Add-Empty Functions                    ============================
//========================================================================================


void Core::slotGetAllFilters ( QStringList& _list){

  for (int i=0; i < (int)supportedTypes_.size(); i++){
    QString f = supportedTypes_[i].plugin->getLoadFilters();
    f = f.section(")",0,0).section("(",1,1).trimmed();
    _list << (QString::number(supportedTypes_[i].plugin->supportedType()) + " " + f);
  }
}

void Core::commandLineOpen(const char* _filename, bool asPolyMesh ){
  if (asPolyMesh)
    loadObject(DATA_POLY_MESH, _filename);
  else
    loadObject(_filename);
}

void Core::commandLineScript(const char* _filename ) {
  //check if we have scripting support:
  bool ok = false;
  slotPluginExists("scripting",ok);
  if ( ! ok ) {
    emit log(LOGERR ,"No scripting support available, please check if we load a scripting plugin");
    return;
  }

  emit executeFileScript(_filename);
}

/// Load object by guessing DataType depending on the files extension
int Core::loadObject ( QString _filename ) {
  if (_filename.endsWith(".ini")) {

    // Load all information from the given ini file
    openIniFile(_filename,true,true,true);

    if ( OpenFlipper::Options::gui() )
      coreWidget_->addRecent(_filename, DATA_NONE);

    return -2;
  } else
  if (_filename.endsWith(".ofs")) {
     emit log(LOGINFO ,"Starting script execution of " + _filename);
     emit executeFileScript(_filename);
  } else
    return loadObject( DATA_TRIANGLE_MESH, _filename);

  return -1;
}

/// Function for loading a given file
int Core::loadObject( DataType _type, QString _filename) {

  if (_type == DATA_NONE)
    return loadObject(_filename);

  for (int i=0; i < (int)supportedTypes_.size(); i++)
    if (supportedTypes_[i].type == _type) {


      if ( OpenFlipper::Options::gui() ) {
        coreWidget_->statusMessage( "Loading " + _filename + " ...");
        if ( !OpenFlipper::Options::loadingSettings() )
          coreWidget_->setStatus(ApplicationStatus::PROCESSING );
      }

      //load file
      int id = supportedTypes_[i].plugin->loadObject(_filename);

      if ( OpenFlipper::Options::gui() ) {
        if ( id != -1 )
          coreWidget_->statusMessage( "Loading " + _filename + " ... done", 4000 );
        else
          coreWidget_->statusMessage( "Loading " + _filename + " ... failed!", 4000 );

        if ( !OpenFlipper::Options::loadingSettings() )
          coreWidget_->setStatus(ApplicationStatus::READY );
      }

      return id;
    }
  return -1; //no plugin found
}

/// Function for adding an empty object of given DataType
int Core::addEmptyObject( DataType _type ) {
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
 void Core::slotObjectOpened ( int _id ) {

  // get the opened object
   BaseObjectData* object;
   PluginFunctions::get_object(_id,object);

   if ( OpenFlipper::Options::randomBaseColor() )
     object->setBaseColor(ACG::Vec4f((float)(rand()%255)/255.0,
                          (float)(rand()%255)/255.0,
                           (float)(rand()%255)/255.0,
                            1.0 ));
   else{
      ACG::Vec4f color;
      QColor defColor = OpenFlipper::Options::defaultBaseColor();

      color[0] = defColor.redF();
      color[1] = defColor.greenF();
      color[2] = defColor.blueF();
      color[3] = defColor.alphaF();

      object->setBaseColor( color );
   }

   resetScenegraph();

   emit openedFile( _id );

  // Tell the Plugins that the Object List and the active object have changed
   emit signalObjectUpdated(_id);
   emit activeObjectChanged();

   backupRequest(_id,"Original Object");

   QString filename = object->path() + OpenFlipper::Options::dirSeparator() + object->name();

   BaseObject* object2;
   PluginFunctions::get_object(_id,object2);

   if ( OpenFlipper::Options::gui() )
    coreWidget_->addRecent( filename, object2->dataType() );

   // if this is the first object opend, reset the global draw mode of all examiners to standard draw mode
   if ( PluginFunctions::object_count() == 1 && OpenFlipper::Options::gui() && !OpenFlipper::Options::loadingSettings() )
    for ( unsigned int i = 0 ; i < OpenFlipper::Options::examinerWidgets() ; ++i )
      coreWidget_->examiner_widgets_[i]->drawMode( OpenFlipper::Options::standardDrawMode() );

  // objectRoot_->dumpTree();
 }

 /// Slot gets called after a file-plugin has opened an object
 void Core::slotEmptyObjectAdded ( int _id ) {
  // Tell the Plugins that the Object List and the active object have changed
   emit signalObjectUpdated(_id);
   emit activeObjectChanged();

   backupRequest(_id,"Original Object");

  // get the opened object
   BaseObjectData* object;
   PluginFunctions::get_object(_id,object);

   ///@todo : set a default path for new objects
//    QString filename = object->path() + OpenFlipper::Options::dirSeparator() + object->name();

//    addRecent(filename);
 }

//========================================================================================
// ===             Menu Slots                                 ============================
//========================================================================================

/// Opens AddEmpty-Object widget
void Core::slotAddEmptyObjectMenu() {
  if (supportedTypes_.size() != 0){
    static addEmptyWidget* widget = 0;
    if ( !widget ){
      std::vector< DataType > types;
      QStringList typeNames;
      for (int i=0; i < (int)supportedTypes_.size(); i++) {
        types.push_back(supportedTypes_[i].type);
        typeNames.push_back(supportedTypes_[i].plugin->typeName());
      }
      widget = new addEmptyWidget(types,typeNames);
      connect(widget,SIGNAL(chosen(DataType, int&)),this,SLOT(slotAddEmptyObject(DataType, int&)));
    }
    widget->show();
  }else
    emit log(LOGERR,"Could not show 'add Empty' dialog. Missing file-plugins.");
}

//========================================================================================
// ===             Public Slots                                 ============================
//========================================================================================

/// Open Load-Object Widget
void Core::loadObject() {

  if ( OpenFlipper::Options::gui() ){

    if (supportedTypes_.size() != 0){
      LoadWidget* widget = new LoadWidget(supportedTypes_);
      connect(widget,SIGNAL(load(QString, DataType, int&)),this,SLOT(slotLoad(QString, DataType, int&)));
      connect(widget,SIGNAL(save(int, QString)),this,SLOT(saveObject(int, QString)));

      widget->showLoad();

      widget->disconnect();
      delete widget;

    }else
      emit log(LOGERR,"Could not show 'load objects' dialog. Missing file-plugins.");

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

    fileDialog.setAcceptMode ( QFileDialog::AcceptOpen );
    fileDialog.setFileMode ( QFileDialog::AnyFile );

    QGridLayout *layout = (QGridLayout*)fileDialog.layout();

    QGroupBox* optionsBox = new QGroupBox( &fileDialog ) ;
    optionsBox->setSizePolicy( QSizePolicy ( QSizePolicy::Expanding , QSizePolicy::Preferred ) );
    optionsBox->setTitle("Options");
    layout->addWidget( optionsBox, layout->rowCount() , 0 , 1,layout->columnCount() );

    QCheckBox *loadProgramSettings = new QCheckBox(optionsBox);
    loadProgramSettings->setText("Load program settings");
    loadProgramSettings->setToolTip("Load all current program settings from the file ( This will include view settings, colors,...) ");
    loadProgramSettings->setCheckState( Qt::Unchecked );

    QCheckBox *loadPluginSettings = new QCheckBox(optionsBox);
    loadPluginSettings->setText("Load per Plugin Settings");
    loadPluginSettings->setToolTip("Plugins should load their current global settings from the file");
    loadPluginSettings->setCheckState( Qt::Checked );

    QCheckBox *loadObjectInfo = new QCheckBox(optionsBox);
    loadObjectInfo->setText("Load all objects defined in the file");
    loadObjectInfo->setToolTip("Load all objects which are defined in the file");
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

    coreWidget_->addRecent(complete_name, DATA_NONE);
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
