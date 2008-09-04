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

#include <QFileDialog>
#include <QGroupBox>
#include <QBoxLayout>

//========================================================================================
// ===             Save Functions                             ============================
//========================================================================================

/// Save an object
bool Core::saveObject( int _id, QString _filename ) {
  BaseObjectData* object;
  PluginFunctions::get_object(_id,object);

  for (int i=0; i < (int)supportedTypes_.size(); i++)
    if (object->dataType() == supportedTypes_[i].type) {

      if ( OpenFlipper::Options::gui() ) {
        coreWidget_->statusMessage( "Saving " + _filename + " ...");
        if ( !OpenFlipper::Options::openingIni() )
          coreWidget_->setStatus(ApplicationStatus::PROCESSING );
      }

      //load file
      bool ok = supportedTypes_[i].plugin->saveObject(_id,_filename);

      if ( OpenFlipper::Options::gui() ) {
        if (ok)
          coreWidget_->statusMessage( "Saving " + _filename + " ... done", 4000 );
        else
          coreWidget_->statusMessage( "Saving " + _filename + " ... failed!", 4000 );

        if ( !OpenFlipper::Options::openingIni() )
          coreWidget_->setStatus(ApplicationStatus::READY );
      }

      return ok;
    }

  return false; //no plugin found
}

/// Save an object to a new location
bool Core::saveObjectTo( int _id, QString _filename ) {
  BaseObjectData* object;
  PluginFunctions::get_object(_id,object);

   //init widget
  static LoadWidget* widget = 0;
  if ( !widget ){
    widget = new LoadWidget(supportedTypes_);
    connect(widget,SIGNAL(load(QString, DataType, int&)),this,SLOT(slotLoad(QString, DataType, int&)));
    connect(widget,SIGNAL(save(int, QString)),this,SLOT(slotSave(int, QString)));
  }

  if (supportedTypes_.size() != 0)
    return widget->showSave(_id,_filename);
  else{
    emit log(LOGERR,"Could not show 'save objects' dialog. Missing file-plugins.");
    return false;
  }
}

//========================================================================================
// ===             Save Slots                                 ============================
//========================================================================================

/// A plugin wants to save an object
/// existing filename will be overwritten!!!
void Core::slotSave(int _id , QString _filename) {
  saveObject(_id,_filename);

  BaseObjectData* object;
  PluginFunctions::get_object(_id,object);
  if ( object != 0 && OpenFlipper::Options::gui() )
    coreWidget_->addRecent( _filename, object->dataType() );
}

//========================================================================================
// ===             Menu Slots                                 ============================
//========================================================================================

/// Save changes to objects
void Core::slotSaveMenu(){
  //iterate over all target objects
  bool overwrite_all = false;
  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS) ;
        o_it != PluginFunctions::objects_end(); ++o_it)  {
          if ( !QDir(o_it->path()).exists() || o_it->path().trimmed() == "" ) // if path isn't valid use 'save object to'
            saveObjectTo(o_it->id(),o_it->name());
          else{
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
            saveObject(o_it->id(),filename);
          }
        }
}

/// Save object as
void Core::slotSaveToMenu(){
  //init widget
  static LoadWidget* widget = 0;
  if ( !widget ){
    widget = new LoadWidget(supportedTypes_);
    connect(widget,SIGNAL(load(QString, DataType, int&)),this,SLOT(slotLoad(QString, DataType, int&)));
    connect(widget,SIGNAL(save(int, QString)),this,SLOT(slotSave(int, QString)));
  }

  if (supportedTypes_.size() != 0){
    //iterate over all target objects
    for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS);
          o_it != PluginFunctions::objects_end(); ++o_it)  {
      QString filename = o_it->path() + OpenFlipper::Options::dirSeparator() + o_it->name();
      widget->showSave(o_it->id(),filename);
    }
  }else
    emit log(LOGERR,"Could not show 'save objects' dialog. Missing file-plugins.");
}

/// Save to Ini File
void Core::slotSaveIniMenu(){

  QString complete_name;

  QFileDialog fileDialog( coreWidget_,
                          tr("Save Settings"),
                          OpenFlipper::Options::currentDirStr(),
                          tr("INI files (*.ini);;OBJ files (*.obj )") );

  fileDialog.setAcceptMode ( QFileDialog::AcceptSave );
  fileDialog.setFileMode ( QFileDialog::AnyFile );
  fileDialog.setDefaultSuffix("ini");
  QGridLayout *layout = (QGridLayout*)fileDialog.layout();

  QGroupBox* optionsBox = new QGroupBox( &fileDialog ) ;
//   optionsBox->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
  optionsBox->setSizePolicy( QSizePolicy ( QSizePolicy::Expanding , QSizePolicy::Preferred ) );
  optionsBox->setTitle("Options");
  layout->addWidget( optionsBox, layout->rowCount() , 0 , 1,layout->columnCount() );

  QCheckBox *saveAllBox = new QCheckBox(optionsBox);
  saveAllBox->setText("Save everything to same folder");
  saveAllBox->setToolTip("Save all open files to the same folder as the ini file");
  saveAllBox->setCheckState( Qt::Checked );

  QCheckBox *askOverwrite = new QCheckBox(optionsBox);
  askOverwrite->setText("Ask before overwriting files");
  askOverwrite->setToolTip("If a file exists you will get asked what to do");
  askOverwrite->setCheckState( Qt::Checked );

  QBoxLayout* frameLayout = new QBoxLayout(QBoxLayout::TopToBottom,optionsBox);
  frameLayout->addWidget( saveAllBox   , 0 , 0);
  frameLayout->addWidget( askOverwrite , 1 , 0);
  frameLayout->addStretch();

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

  std::cout << "new path: " << newpath.toStdString() << std::endl;
  std::cout << "complete_name: " << complete_name.toStdString() << std::endl;

  //init save-as widget
  static LoadWidget* widget = 0;
  if ( !widget ){
    widget = new LoadWidget(supportedTypes_);
    connect(widget,SIGNAL(load(QString, DataType, int&)),this,SLOT(slotLoad(QString, DataType, int&)));
    connect(widget,SIGNAL(save(int, QString)),this,SLOT(slotSave(int, QString)));
  }

  //iterate over opened objects and save them
  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS) ;
                                        o_it != PluginFunctions::objects_end(); ++o_it)
  {
      if ( saveAllBox->isChecked() )
      {
        // Use path of ini file for all objects
        QString filename = newpath + OpenFlipper::Options::dirSeparator() + o_it->name();

        // enforce that all files end with obj extension
        if ( complete_name.endsWith("obj") )
        {
          if (!filename.endsWith("obj"))
          {
            std::cout << "\nold filename: " << filename.toStdString() << std::endl;
            // remove old extension
            int pos = filename.lastIndexOf(".");
            filename.remove(pos+1, filename.length() - pos);
            std::cout << "cleared filename = " << filename.toStdString() << std::endl;
            // add obj extension
            filename += "obj";
            std::cout << "new filename: " << filename.toStdString() << std::endl;
          }
        }


        // normally the file does not exist when saving all to one folder
        if ( !QFile(filename).exists() || !askOverwrite->isChecked() )
          saveObject( o_it->id(), filename);
        else
          widget->showSave(o_it->id(), filename);

      }
      else
      {
//         QString filename = o_it->path() + OpenFlipper::Options::dirSeparator() + o_it->name();
        QString filename = newpath + OpenFlipper::Options::dirSeparator() + o_it->name();

        std::cerr << "o_it->path  : " << o_it->path().toStdString() << std::endl;

        // handle the case that the object was created in current session and not loaded from disk
        if (o_it->path() == ".") {
          filename = newpath + OpenFlipper::Options::dirSeparator() + o_it->name();
          std::cerr << "newpath : " << newpath.toStdString() << std::endl;
          std::cerr << "name  : " << o_it->name().toStdString() << std::endl;

        }

        // enforce that all files end with obj extension
        if ( complete_name.endsWith("obj") )
        {
          if (!filename.endsWith("obj"))
          {
            std::cout << "\nold filename: " << filename.toStdString() << std::endl;
            // remove old extension
            int pos = filename.lastIndexOf(".");
            filename.remove(pos+1, filename.length() - pos);
            std::cout << "cleared filename = " << filename.toStdString() << std::endl;
            // add obj extension
            filename += "obj";
            std::cout << "new filename: " << filename.toStdString() << std::endl;
          }
        }

        if ( QFile(filename).exists() && !askOverwrite->isChecked() )
          saveObject( o_it->id(), filename);
        else
          widget->showSave(o_it->id(), filename);
      }

  }


  if ( complete_name.endsWith("ini") ) {
    //write to ini
    writeIniFile(complete_name, saveAllBox->isChecked());
  } else if ( complete_name.endsWith("obj") ) {
    //write to obj
    writeObjFile(complete_name, saveAllBox->isChecked());
  }


}


