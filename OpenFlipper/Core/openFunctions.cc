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
  if ( OpenFlipper::Options::gui() && !OpenFlipper::Options::openingIni() ) {
    // update scene graph
    coreWidget_->examiner_widget_->lockUpdate();
    coreWidget_->examiner_widget_->sceneGraph(root_node_scenegraph_);
    coreWidget_->examiner_widget_->viewAll();
    coreWidget_->examiner_widget_->setScenePos( coreWidget_->examiner_widget_->scene_center() ,
                                                coreWidget_->examiner_widget_->scene_radius() /* 10.0*/  );

    coreWidget_->examiner_widget_->setHome();
    coreWidget_->examiner_widget_->drawMode(standard_draw_mode_);
    coreWidget_->examiner_widget_->unlockUpdate();
    coreWidget_->examiner_widget_->updateGL();
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
    openIniFile(_filename);
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
        if ( !OpenFlipper::Options::openingIni() )
          coreWidget_->setStatus(ApplicationStatus::PROCESSING );
      }

      //load file
      int id = supportedTypes_[i].plugin->loadObject(_filename);

      if ( OpenFlipper::Options::gui() ) {
        if ( id != -1 )
          coreWidget_->statusMessage( "Loading " + _filename + " ... done", 4000 );
        else
          coreWidget_->statusMessage( "Loading " + _filename + " ... failed!", 4000 );

        if ( !OpenFlipper::Options::openingIni() )
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
   ///@todo Add Globaloption for setRandomBaseColor
//    if ( set_random_base_color_ )
//      object->setBaseColor(ACG::Vec4f((float)(rand()%255)/255.0,
//                           (float)(rand()%255)/255.0,
//                            (float)(rand()%255)/255.0,
//                             1.0 ));
//    else
//      object->setBaseColor(ACG::Vec4f(255,255,255,1));

   resetScenegraph();

   emit openedFile( _id );

  // Tell the Plugins that the Object List and the active object have changed
   emit ObjectListUpdated(_id);
   emit activeObjectChanged();

   backupRequest(_id,"Original Object");

  // get the opened object
   BaseObjectData* object;
   PluginFunctions::get_object(_id,object);

   QString filename = object->path() + OpenFlipper::Options::dirSeparator() + object->name();

   BaseObject* object2;
   PluginFunctions::get_object(_id,object2);

   if ( OpenFlipper::Options::gui() )
    coreWidget_->addRecent( filename, object2->dataType() );

  // objectRoot_->dumpTree();
 }

 /// Slot gets called after a file-plugin has opened an object
 void Core::slotEmptyObjectAdded ( int _id ) {
  // Tell the Plugins that the Object List and the active object have changed
   emit ObjectListUpdated(_id);
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
      for (int i=0; i < (int)supportedTypes_.size(); i++)
        if (supportedTypes_[i].plugin->supportAddEmpty()){
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

/// Open Load-Object Widget
void Core::slotLoadMenu() {
  if (supportedTypes_.size() != 0){
    static LoadWidget* widget = 0;

    // Open Widget
    if ( !widget ){
      widget = new LoadWidget(supportedTypes_);
      connect(widget,SIGNAL(load(QString, DataType, int&)),this,SLOT(slotLoad(QString, DataType, int&)));
      connect(widget,SIGNAL(save(int, QString)),this,SLOT(slotSave(int, QString)));
    }
    widget->showLoad();
  }else
    emit log(LOGERR,"Could not show 'load objects' dialog. Missing file-plugins.");
}

/// Load settings from ini-file
void Core::slotLoadIniMenu(){

  QString complete_name;

  complete_name = ACG::getOpenFileName(coreWidget_,
                                       tr("Load Settings"),
                                       tr("INI files (*.ini);;OBJ files (*.obj)"),
                                       OpenFlipper::Options::currentDirStr());
  if (complete_name.isEmpty())
    return;

  QString newpath = complete_name.section(OpenFlipper::Options::dirSeparator(),0,-2);
  OpenFlipper::Options::currentDir(newpath);

  if ( complete_name.endsWith("ini") ) {
    openIniFile(complete_name);
    applyOptions();
  } else if ( complete_name.endsWith("obj") ) {
    openObjFile(complete_name);
    applyOptions();
  }

  if ( OpenFlipper::Options::gui() )
    coreWidget_->addRecent(complete_name, DATA_NONE);
}
