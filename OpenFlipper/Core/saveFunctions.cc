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

#include "OpenFlipper/common/GlobalOptions.hh"

#include "OpenFlipper/BasePlugin/PluginFunctions.hh"

#include "OpenFlipper/widgets/loadWidget/loadWidget.hh"

//========================================================================================
// ===  Public Slots (called by CoreWidget's File-Menu / Scripting / Plugins)    =========
//========================================================================================

/// Save an object
/// get object with given id, find a plugin to save dataType, save!
/// (existing files will be overwritten)
bool Core::saveObject( int _id, QString _filename ) {
  BaseObjectData* object;
  PluginFunctions::get_object(_id,object);

  for (int i=0; i < (int)supportedTypes_.size(); i++)
    if (object->dataType() == supportedTypes_[i].type) {

      if ( OpenFlipper::Options::gui() ) {
        coreWidget_->statusMessage( "Saving " + _filename + " ...");
        if ( !OpenFlipper::Options::savingSettings() )
          coreWidget_->setStatus(ApplicationStatus::PROCESSING );
      }

      //save object
      bool ok = supportedTypes_[i].plugin->saveObject(_id,_filename);
    
      if ( OpenFlipper::Options::gui() ) {
        if (ok)
          coreWidget_->statusMessage( "Saving " + _filename + " ... done", 4000 );
        else
          coreWidget_->statusMessage( "Saving " + _filename + " ... failed!", 4000 );
    
        if ( !OpenFlipper::Options::savingSettings() )
          coreWidget_->setStatus(ApplicationStatus::READY );
      }

      //add to recent files
      if (ok && !OpenFlipper::Options::savingSettings()
             &&  OpenFlipper::Options::gui() )
        coreWidget_->addRecent( _filename, object->dataType() );

      return ok;
    }

  // no plugin found
  if ( OpenFlipper::Options::gui() )
      coreWidget_->statusMessage( "Saving " + _filename + " ... failed!", 4000 );

  return false;
}

//-----------------------------------------------------------------------------------------------------

/// Save an object to a new location
/// Show the save-widget to determine a filename, the widget calls slotSave to save the object
bool Core::saveObjectTo( int _id, QString _filename ) {

  if ( OpenFlipper::Options::gui() ){

    BaseObjectData* object;
    PluginFunctions::get_object(_id,object);
  
    //init widget
    static LoadWidget* widget = 0;
    if ( !widget ){
      widget = new LoadWidget(supportedTypes_);
      connect(widget,SIGNAL(load(QString, DataType, int&)),this,SLOT(slotLoad(QString, DataType, int&)));
      connect(widget,SIGNAL(save(int, QString)),this,SLOT(saveObject(int, QString)));
    }
  
    if (supportedTypes_.size() != 0)
      return widget->showSave(_id,_filename);
    else{
      emit log(LOGERR,"Could not show 'save objects' dialog. Missing file-plugins.");
      return false;
    }

  }else
    return false;
}

//-----------------------------------------------------------------------------------------------------

/// Save changes for all target objects (slot is called from CoreWidget's File-Menu)
void Core::saveAllObjects(){

  if ( OpenFlipper::Options::gui() ){

    //ensure that all options are on their default values
    for (int i=0; i < (int)supportedTypes_.size(); i++)
      supportedTypes_[i].plugin->saveOptionsWidget("");
  
    //iterate over all target objects
    bool overwrite_all = false;
  
    for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS) ;
          o_it != PluginFunctions::objects_end(); ++o_it)  {
  
            if ( !QDir(o_it->path()).exists() || o_it->path().trimmed() == "" ) // if path isn't valid use 'save object to'
              saveObjectTo(o_it->id(),o_it->name());
            else{
              //ask for overwriting
              QString filename = o_it->path() + OpenFlipper::Options::dirSeparator() + o_it->name() ;
              QFile file(filename);
              if ( file.exists() && !overwrite_all){
                int ret = QMessageBox::warning(coreWidget_, tr("File exists"),QString("The file '" +filename+ "' already exists.\n"+
                    "Do you want to overwrite the file?"),QMessageBox::YesToAll |
                        QMessageBox::Yes|QMessageBox::No|QMessageBox::NoToAll,QMessageBox::No);
                switch(ret){
                  case QMessageBox::YesToAll: overwrite_all = true;break;
                  case QMessageBox::Yes: break;
                  case QMessageBox::No: continue;
                  case QMessageBox::NoToAll: return;
                }
              }
  
              // and save
              saveObject(o_it->id(),filename);
            }
    }
  }
}

//-----------------------------------------------------------------------------------------------------

/// Save all target objects under new name (slot is called from CoreWidget's File-Menu)
void Core::saveAllObjectsTo(){
  if ( OpenFlipper::Options::gui() ){
    if (supportedTypes_.size() != 0){
      //iterate over all target objects
      for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS);
            o_it != PluginFunctions::objects_end(); ++o_it)  {
        QString filename = o_it->path() + OpenFlipper::Options::dirSeparator() + o_it->name();
        saveObjectTo(o_it->id(),filename);
      }
    }else
      emit log(LOGERR,"Could not show 'save objects' dialog. Missing file-plugins.");
  }
}



