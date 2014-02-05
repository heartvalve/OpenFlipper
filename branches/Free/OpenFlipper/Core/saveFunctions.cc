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

#include "OpenFlipper/common/GlobalOptions.hh"

#include "OpenFlipper/BasePlugin/PluginFunctions.hh"

#include "OpenFlipper/widgets/loadWidget/loadWidget.hh"

#include <OpenFlipper/common/DataTypes.hh>

//========================================================================================
// ===  Public Slots (called by CoreWidget's File-Menu / Scripting / Plugins)    =========
//========================================================================================

/** \brief Save an object
 *
 * get object with given id, find a plugin to save dataType, save!
 *  (existing files will be overwritten)
 *
 * @param _id       Id of the object that should be saved
 * @param _filename Filename to save to
 * @return Successful?
 */
bool Core::saveObject( int _id, QString _filename ) {
  BaseObjectData* object;
  PluginFunctions::getObject(_id,object);

  QString file_extension = QFileInfo(_filename).suffix();
  
  for (int i=0; i < (int)supportedTypes().size(); i++) {
    if ( supportedTypes()[i].type.contains(object->dataType()) &&
       ( supportedTypes()[i].saveFilters.contains(file_extension) || file_extension.isEmpty() ) ) {

      if ( OpenFlipper::Options::gui() ) {
        coreWidget_->statusMessage( tr("Saving ") + _filename + " ...");
        if ( !OpenFlipper::Options::savingSettings() )
          coreWidget_->setStatus(ApplicationStatus::PROCESSING );
      }

      //save object
      
      bool ok = supportedTypes()[i].plugin->saveObject(_id,_filename);
    
      if ( OpenFlipper::Options::gui() ) {
        if (ok)
          coreWidget_->statusMessage( tr("Saving ") + _filename + tr(" ... done"), 4000 );
        else{
          emit log(LOGERR, tr("Unable to save '%1'. Plugin failed. DataType %2").arg(_filename, dataTypeName(object->dataType()) )  );
          emit log(LOGERR, tr("Plugin was: '%1'. File Extension was: %2").arg(supportedTypes()[i].name, file_extension )  );
          coreWidget_->statusMessage( tr("Saving ") + _filename + tr(" ... failed!"), 4000 );
        }
    
        if ( !OpenFlipper::Options::savingSettings() )
          coreWidget_->setStatus(ApplicationStatus::READY );
      }

      //add to recent files
      if (ok && !OpenFlipper::Options::savingSettings()
             &&  OpenFlipper::Options::gui() )
        coreWidget_->addRecent( _filename, object->dataType() );

      return ok;
    }
  }
  
  // no plugin found
  if ( OpenFlipper::Options::gui() ){
      emit log(LOGERR, tr("Unable to save '%1'. No plugin found. DataType %2").arg(_filename, dataTypeName(object->dataType()) )  );
      coreWidget_->statusMessage( tr("Saving ") + _filename + tr(" ... failed!"), 4000 );
  }

  return false;
}

//-----------------------------------------------------------------------------------------------------

/// Save an object
/// get object with given id, find a plugin to save dataType, save!
/// (existing files will be overwritten)
void Core::saveObject( int _id, QString _filename, int _pluginID ) {
  BaseObjectData* object;
  PluginFunctions::getObject(_id,object);


  if ( OpenFlipper::Options::gui() ) {
    coreWidget_->statusMessage( tr("Saving ") + _filename + " ...");
    if ( !OpenFlipper::Options::savingSettings() )
      coreWidget_->setStatus(ApplicationStatus::PROCESSING );
  }

  //save object
  bool ok = supportedTypes()[_pluginID].plugin->saveObject(_id,_filename);

  if ( OpenFlipper::Options::gui() ) {
    if (ok)
      coreWidget_->statusMessage( tr("Saving ") + _filename + tr(" ... done"), 4000 );
    else
      coreWidget_->statusMessage( tr("Saving ") + _filename + tr(" ... failed!"), 4000 );

    if ( !OpenFlipper::Options::savingSettings() )
      coreWidget_->setStatus(ApplicationStatus::READY );
  }

  //add to recent files
  if (ok && !OpenFlipper::Options::savingSettings()
          &&  OpenFlipper::Options::gui() )
    coreWidget_->addRecent( _filename, object->dataType() );

}

//-----------------------------------------------------------------------------------------------------

/// Save multiple objects to one file
/// get object with given id, find a plugin to save dataType, save!
/// (existing files will be overwritten)
void Core::saveObjects( IdList _ids, QString _filename, int _pluginID ) {
  
  DataType type = 0;
  
  for (uint i=0; i < _ids.size(); i++){
  
    BaseObjectData* object;
    PluginFunctions::getObject(_ids[i],object);

    type |= object->dataType();
  }

  if ( OpenFlipper::Options::gui() ) {
    coreWidget_->statusMessage( tr("Saving ") + _filename + " ...");
    if ( !OpenFlipper::Options::savingSettings() )
      coreWidget_->setStatus(ApplicationStatus::PROCESSING );
  }

  //save objects
  if ( !supportedTypes()[_pluginID].saveMultipleObjects){
    emit log(LOGERR, tr("Unable to save objects. Plugin does not allow multiple objects."));
    return;
  }
  
  bool ok = supportedTypes()[_pluginID].plugin->saveObjects(_ids,_filename);

  if ( OpenFlipper::Options::gui() ) {
    if (ok)
      coreWidget_->statusMessage( tr("Saving ") + _filename + tr(" ... done"), 4000 );
    else
      coreWidget_->statusMessage( tr("Saving ") + _filename + tr(" ... failed!"), 4000 );

    if ( !OpenFlipper::Options::savingSettings() )
      coreWidget_->setStatus(ApplicationStatus::READY );
  }

  //add to recent files
  if (ok && !OpenFlipper::Options::savingSettings()
          &&  OpenFlipper::Options::gui() )
    coreWidget_->addRecent( _filename, type );
}

//-----------------------------------------------------------------------------------------------------

/// Save an object to a new location
/// Show the save-widget to determine a filename, the widget calls slotSave to save the object
bool Core::saveObjectTo( int _id, QString _filename ) {

  bool result = false;

  if ( OpenFlipper::Options::gui() ){
 
    //init widget
    LoadWidget* widget = new LoadWidget(supportedTypes());
    widget->setWindowIcon( OpenFlipper::Options::OpenFlipperIcon() );

    connect(widget,SIGNAL(save(int, QString, int)),this,SLOT(saveObject(int, QString, int)));
  
    if (supportedTypes().size() != 0)
      result = widget->showSave(_id,_filename);
    else
      emit log(LOGERR,tr("Could not show 'save objects' dialog. Missing file-plugins."));

    widget->disconnect();
    delete widget;
  }

  return result;
}

//-----------------------------------------------------------------------------------------------------

/// Save multiple objects to a new location
/// Show the save-widget to determine a filename, the widget calls slotSave to save the objects
bool Core::saveObjectsTo( IdList _ids, QString _filename ) {

  bool result = false;

  if ( OpenFlipper::Options::gui() ){

    //init widget
    LoadWidget* widget = new LoadWidget(supportedTypes());
    widget->setWindowIcon( OpenFlipper::Options::OpenFlipperIcon() );

    connect(widget,SIGNAL(save(IdList, QString, int)),this,SLOT(saveObjects(IdList, QString, int)));
  
    if (supportedTypes().size() != 0)
      result = widget->showSave(_ids,_filename);
    else
      emit log(LOGERR,tr("Could not show 'save objects' dialog. Missing file-plugins."));

    widget->disconnect();
    delete widget;
  }

  return result;
}

//-----------------------------------------------------------------------------------------------------

/// Save changes for all target objects (slot is called from CoreWidget's File-Menu)
void Core::saveAllObjects(){

  if ( OpenFlipper::Options::gui() ){

    //ensure that all options are on their default values
    for (int i=0; i < (int)supportedTypes().size(); i++)
      supportedTypes()[i].plugin->saveOptionsWidget("");
  
    //iterate over all target objects
    for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS) ;
          o_it != PluginFunctions::objectsEnd(); ++o_it)  {
  
            if ( !QDir(o_it->path()).exists() || o_it->path().trimmed() == "" || o_it->path().trimmed() == "." ) // if path isn't valid use 'save object to'
              saveObjectTo(o_it->id(),o_it->name());
            else{
              //save (existing files will be overwritten)
              QString filename = o_it->path() + OpenFlipper::Options::dirSeparator() + o_it->name() ;
              saveObject(o_it->id(),filename);
            }
    }
  }
}

//-----------------------------------------------------------------------------------------------------

/// Save all target objects under new name (slot is called from CoreWidget's File-Menu)
void Core::saveAllObjectsTo(){

  if ( OpenFlipper::Options::gui() ){

    //ensure that all options are on their default values
    for (int i=0; i < (int)supportedTypes().size(); i++)
      supportedTypes()[i].plugin->saveOptionsWidget("");
  
    //get all dataTypes that want to be saved
    DataType types = DATA_UNKNOWN;
    IdList ids;
    
    for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS) ;
          o_it != PluginFunctions::objectsEnd(); ++o_it){
      types |= o_it->dataType();
      ids.push_back( o_it->id() );
    }
    
    //check if a plugin can save all types to one file
    bool multiSave = false;
    
    for (int i=0; i < (int)supportedTypes().size(); i++)
      if ( (supportedTypes()[i].saveMultipleObjects) && (supportedTypes()[i].type.contains(types)) )
        multiSave = true;

      
    if (ids.size() > 1 && multiSave){
      //save all objets to one file and use name of first object
      PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS);
      
      saveObjectsTo(ids,o_it->name());
      
    } else {
      //save each object separately
      
      //iterate over all target objects
      for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS) ;
          o_it != PluginFunctions::objectsEnd(); ++o_it)
            saveObjectTo(o_it->id(),o_it->name());
    }
  }
}



