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
//   $Revision: 3388 $
//   $Author: wilden $
//   $Date: 2008-10-15 11:26:18 +0200 (Wed, 15 Oct 2008) $
//
//=============================================================================


#include "Core.hh"

#include "OpenFlipper/common/GlobalOptions.hh"

#include "OpenFlipper/BasePlugin/PluginFunctions.hh"

#include "OpenFlipper/widgets/loadWidget/loadWidget.hh"

#include <QFileDialog>
#include <QGroupBox>
#include <QBoxLayout>

/// Save Settings (slot is called from CoreWidget's File-Menu)
void Core::saveSettings(){

  // ========================================================================================
  // generate the saveSettings-Dialog
  // ========================================================================================

  QString complete_name;

  QFileDialog fileDialog( coreWidget_,
                          tr("Save Settings"),
                          OpenFlipper::Options::currentDirStr(),
                          tr("INI files (*.ini);;OBJ files (*.obj )") );

  fileDialog.setAcceptMode ( QFileDialog::AcceptSave );
  fileDialog.setFileMode ( QFileDialog::AnyFile );

  QGridLayout *layout = (QGridLayout*)fileDialog.layout();

  QGroupBox* optionsBox = new QGroupBox( &fileDialog ) ;
  optionsBox->setSizePolicy( QSizePolicy ( QSizePolicy::Expanding , QSizePolicy::Preferred ) );
  optionsBox->setTitle("Options");
  layout->addWidget( optionsBox, layout->rowCount() , 0 , 1,layout->columnCount() );

  QCheckBox *saveProgramSettings = new QCheckBox(optionsBox);
  saveProgramSettings->setText("Save program settings");
  saveProgramSettings->setToolTip("Save all current program settings to the file ( This will include view settings, colors,...) ");
  saveProgramSettings->setCheckState( Qt::Unchecked );

  QCheckBox *savePluginSettings = new QCheckBox(optionsBox);
  savePluginSettings->setText("Save per Plugin Settings");
  savePluginSettings->setToolTip("Plugins should add their current global settings to the file");
  savePluginSettings->setCheckState( Qt::Checked );

  QCheckBox *saveObjectInfo = new QCheckBox(optionsBox);
  saveObjectInfo->setText("Save open object information to the file");
  saveObjectInfo->setToolTip("Save all open Objects and add them to the settings file ( they will be loaded if opening the settings file");
  saveObjectInfo->setCheckState( Qt::Checked );

  QCheckBox *saveAllBox = new QCheckBox(optionsBox);
  saveAllBox->setText("Save everything to same folder");
  saveAllBox->setToolTip("Save all open files to the same folder as the ini file");
  saveAllBox->setCheckState( Qt::Checked );

  QCheckBox *askOverwrite = new QCheckBox(optionsBox);
  askOverwrite->setText("Ask before overwriting files");
  askOverwrite->setToolTip("If a file exists you will get asked what to do");
  askOverwrite->setCheckState( Qt::Checked );

  QCheckBox *targetOnly = new QCheckBox(optionsBox);
  targetOnly->setText("Save only target objects");
  targetOnly->setToolTip("Only objects with target flag will be handled");
  targetOnly->setCheckState( Qt::Unchecked );

  QBoxLayout* frameLayout = new QBoxLayout(QBoxLayout::TopToBottom,optionsBox);
  frameLayout->addWidget( saveProgramSettings , 0 , 0);
  frameLayout->addWidget( savePluginSettings  , 1 , 0);
  frameLayout->addWidget( saveObjectInfo      , 2 , 0);
  frameLayout->addWidget( saveAllBox          , 3 , 0);
  frameLayout->addWidget( askOverwrite        , 4 , 0);
  frameLayout->addWidget( targetOnly          , 5 , 0);
  frameLayout->addStretch();


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

  //check the extension if its a known one
  if ( !complete_name.endsWith(".ini", Qt::CaseInsensitive) && !complete_name.endsWith(".obj", Qt::CaseInsensitive) ){

    // If its unknown, get the type from the currently selected filter and add this extension to the filename
    if ( fileDialog.selectedNameFilter().contains("INI files (*.ini)") )
      complete_name += ".ini";
    else
      complete_name += ".obj";

  }

  // Get the chosen directory and remember it.
  QString newpath = complete_name.section(OpenFlipper::Options::dirSeparator(),0,-2);
  OpenFlipper::Options::currentDir(newpath);

  // ========================================================================================
  // update status information
  // ========================================================================================
  OpenFlipper::Options::savingSettings(true);

  if ( OpenFlipper::Options::gui() ) {
    coreWidget_->statusMessage( "Saving Settings to " + complete_name + " ...");
    coreWidget_->setStatus(ApplicationStatus::BLOCKED );
  }

  // ========================================================================================
  // Save the objects itself
  // ========================================================================================
  // Depending on the checkbox iterate over all objects or only the selected ones.

  if ( saveObjectInfo->isChecked() ) {

    PluginFunctions::IteratorRestriction restriction;
    if ( targetOnly->isChecked() )
      restriction = PluginFunctions::TARGET_OBJECTS;
    else
      restriction = PluginFunctions::ALL_OBJECTS;

    //Iterate over opened objects and save them
    for ( PluginFunctions::ObjectIterator o_it(restriction);
                                          o_it != PluginFunctions::objects_end(); ++o_it)
    {
      QString filename;

      if ( saveAllBox->isChecked() )
      {
        // Use path of settings file for all objects
        filename = newpath + OpenFlipper::Options::dirSeparator() + o_it->name();
      }
      else
      {
        // Use objects own path if it has one. Otherwise also use path of settings file
        filename = o_it->path() + OpenFlipper::Options::dirSeparator() + o_it->name();

        // handle the case that the object was created in current session and not loaded from disk
        if (o_it->path() == ".") {
          filename = newpath + OpenFlipper::Options::dirSeparator() + o_it->name();
          std::cerr << "newpath : " << newpath.toStdString() << std::endl;
          std::cerr << "name  : " << o_it->name().toStdString() << std::endl;
        }
      }

      // enforce that all files end with obj extension if its an obj-settings file
      if ( complete_name.endsWith("obj") )
      {
        if (!filename.endsWith("obj"))
        {
          // remove old extension
          int pos = filename.lastIndexOf(".");
          filename.remove(pos+1, filename.length() - pos);
          // add obj extension
          filename += "obj";
        }
      }

      // decide whether to use saveObject or saveObjectTo
      if ( !QFile(filename).exists() || !askOverwrite->isChecked() )
        saveObject( o_it->id(), filename);
      else
        saveObjectTo(o_it->id(), filename);

    }
  }


  // ========================================================================================
  // Finally save all Settings
  // ========================================================================================
  if ( complete_name.endsWith("ini") ) {

    // write to ini
    writeIniFile( complete_name,
                  saveAllBox->isChecked(),
                  targetOnly->isChecked(),
                  saveProgramSettings->isChecked(),
                  savePluginSettings->isChecked(),
                  saveObjectInfo->isChecked());

  } else if ( complete_name.endsWith("obj") ) {

    //write to obj
    writeObjFile(complete_name, saveAllBox->isChecked(), targetOnly->isChecked() );

  }

  // update status
  OpenFlipper::Options::savingSettings(false);

  if ( OpenFlipper::Options::gui() ) {
    coreWidget_->statusMessage( "Saving Settings to " + complete_name + " ... Done", 4000);
    coreWidget_->setStatus(ApplicationStatus::READY );
  }

  //add to recent files
  if ( OpenFlipper::Options::gui() )
    coreWidget_->addRecent( complete_name, DATA_NONE );

}
