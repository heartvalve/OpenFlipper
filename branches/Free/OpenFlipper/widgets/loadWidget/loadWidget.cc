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




#include "loadWidget.hh"
#include <OpenFlipper/common/GlobalOptions.hh>
#include "OpenFlipper/BasePlugin/PluginFunctions.hh"
#include <QCompleter>
#include <QMessageBox>
#include <QFile>
#include <QFileDialog>
#include <QDir>
#include <QDebug>

#include <QHBoxLayout>

#include "FileOptionsDialog.hh"

#include <OpenFlipper/INIFile/INIFile.hh>
#include <OpenFlipper/common/GlobalOptions.hh>

LoadWidget::LoadWidget(std::vector<fileTypes>& _supportedTypes , QWidget *parent)
  : QFileDialog(parent),
    loadMode_(true),
    supportedTypes_(_supportedTypes)
{
  setOption (QFileDialog::DontUseNativeDialog, true);

  // Get our layout
  QGridLayout *gridLayout = (QGridLayout*)layout();

  //supported Types
  optionsBox_ = new QCheckBox(tr("use defaults"), this);
  optionsBox_->setChecked( OpenFlipperSettings().value("Core/File/UseLoadDefaults",false).toBool() );
  
  // add the options box to the bottom
  gridLayout->addWidget( optionsBox_, gridLayout->rowCount() , 1 );

  // Add a very nice label for it
  QLabel* typeLabel = new QLabel(tr("Options:") , this);
  gridLayout->addWidget( typeLabel, gridLayout->rowCount() -1 , 0 );

  //overwrite dialog shouldn't be handled by the qfiledialog
  setConfirmOverwrite(false);

  setDirectory(OpenFlipper::Options::currentDirStr());
}

/// Desctructor
LoadWidget::~LoadWidget()
{

}

/// adjust load-filters to current datatype
void LoadWidget::slotSetLoadFilters(){

  QStringList allFilters;

  //TODO All files filter
  
  for (int i=0; i < (int)supportedTypes_.size(); i++){
    QStringList filters = supportedTypes_[i].loadFilters.split(";;");
    for (int f=filters.size()-1; f >= 0;  f--){
      if (filters[f].trimmed() == "") { filters.removeAt(f); continue; }
      if (filters[f].contains( tr("All files") ) ) filters.removeAt(f);
    }

    allFilters.append( filters );
  }

  allFilters.removeDuplicates();
  allFilters.sort();

  QStringList allExt;
  
  //get all possible extensions
  for (int i=0; i < allFilters.size(); i++){
     QString ext = allFilters[i].section("(",1).section(")",0,0);
     allExt.append( ext.split(" ") );
  }

  for (int f=allExt.size()-1; f >= 0;  f--)
    if (allExt[f].trimmed() == "") allExt.removeAt(f);
  
  allExt.removeDuplicates();
  allExt.sort();
  
  QString allFiles = tr("All Files (");
  
  for (int i=0; i < allExt.size(); i++)
    allFiles += " " + allExt[i];
  
  allFiles += " )";

  allFilters.push_front(allFiles);
  
  setNameFilters(allFilters);
}

/// adjust save-filters to current datatype
void LoadWidget::slotSetSaveFilters(DataType _type){

  QStringList allFilters;

  for (int i=0; i < (int)supportedTypes_.size(); i++)
    if (supportedTypes_[i].type & _type){
      QStringList filters = supportedTypes_[i].saveFilters.split(";;");
      for (int f=filters.size()-1; f >= 0;  f--){
        if (filters[f].trimmed() == "") { filters.removeAt(f); continue; }
        if (filters[f].contains( tr("All files") ) ) filters.removeAt(f);
      }
      
      allFilters.append( filters );
    }

  allFilters.removeDuplicates();
  allFilters.sort();

  QStringList allExt;
  
  //get all possible extensions
  for (int i=0; i < allFilters.size(); i++){
     QString ext = allFilters[i].section("(",1).section(")",0,0);
     allExt.append( ext.split(" ") );
  }

  for (int f=allExt.size()-1; f >= 0;  f--)
    if (allExt[f].trimmed() == "") allExt.removeAt(f);
  
  allExt.removeDuplicates();
  allExt.sort();
  
  QString allFiles = tr("All Files (");
  
  for (int i=0; i < allExt.size(); i++)
    allFiles += " " + allExt[i];
  
  allFiles += " )";

  allFilters.push_front(allFiles);
  
  setNameFilters(allFilters);
}


/// find suitable plugin for loading current file
void LoadWidget::loadFile(){

  //get selection
  QStringList files = selectedFiles();

  bool success = false;

  //get all extensions
  QStringList ext;

  for (int i=0; i < files.size(); i++)
    ext.push_back( QFileInfo(files[i]).suffix() );
  
   
  //find plugins that can handle the current extensions
  pluginForExtension_.clear();
  
  for (int i=0; i < ext.size(); i++){
    for (uint t=0; t < supportedTypes_.size(); t++){
      
      QString filters = supportedTypes_[t].loadFilters;  
      
      if (filters.contains(ext[i])){
        pluginForExtension_[ ext[i] ] = t;
        break;
      }
    }
  }
  
  getPluginForExtensionINI(ext);
  
  // display options for all dataTypes
  if ( !optionsBox_->isChecked() ){

    FileOptionsDialog options(supportedTypes_,ext, loadMode_);

    connect(&options, SIGNAL(setPluginForExtension(QString,int)), this, SLOT(slotSetPluginForExtension(QString,int)) );

    if ( !options.exec() )
      return;
  }

  //load the selected files
  for (int i=0; i < files.size(); i++){
    
    QFileInfo fi(files[i]);
    QString filename = fi.absoluteFilePath();
    OpenFlipper::Options::currentDir(fi.absolutePath());
    QFile file(filename);

    if (fi.isDir() || !file.exists()) continue; //do nothing if its a not a valid file
    QString ext = fi.suffix();

    //emit load signal
    if ( pluginForExtension_.find( fi.suffix() ) != pluginForExtension_.end() ){

      emit load(filename, pluginForExtension_[ fi.suffix() ]);
      success = true;
    }
  }
}

/// find suitable plugin for saving current file
void LoadWidget::saveFile(){
  //get selection
  QStringList files = selectedFiles();

  if ( files.size() != 1 ) {
    std::cerr << "Error no or multiple save files selected" << std::endl;
    return;
  }

  QString filename = files[0];

  //get filename
  if (!filename.contains(".",Qt::CaseSensitive)){ //check for extension

    int s = selectedFilter().indexOf("*")+1;
    int e = selectedFilter().indexOf(" ", s);
    int e2 = selectedFilter().indexOf(")", s);
    if (e == -1 || e2 < e) e = e2;

    QString ext = selectedFilter().mid(s,e-s);
    filename += ext;
  }

  QFile      f(filename);
  QFileInfo fi(filename);
   
  //find plugin that can handle the current extension
  pluginForExtension_.clear();
  
  for (uint t=0; t < supportedTypes_.size(); t++){

    QString filters = supportedTypes_[t].loadFilters;  

    if (filters.contains( fi.suffix() )){
      pluginForExtension_[ fi.suffix() ] = t;
      break;
    }
  }
  
  getPluginForExtensionINI(QStringList(fi.suffix()));
  
  // display options for all dataTypes
  if ( !optionsBox_->isChecked() ){

    FileOptionsDialog options(supportedTypes_,QStringList(fi.suffix()), loadMode_);

    connect(&options, SIGNAL(setPluginForExtension(QString,int)), this, SLOT(slotSetPluginForExtension(QString,int)) );

    if ( !options.exec() )
      return;
  }

  if (f.exists()){ //check for extension
    int ret = QMessageBox::warning(this, tr("File exists"),tr("This file already exists.\n"
        "Do you want to overwrite the file?"),QMessageBox::Yes|QMessageBox::No,QMessageBox::No);
    if (ret == QMessageBox::No)
      return; //abort if users doesn't want to overwrite
  }

  if ( pluginForExtension_.find( fi.suffix() ) != pluginForExtension_.end() )
    emit save(id_,filename, pluginForExtension_[fi.suffix()] );

  OpenFlipper::Options::currentDir(fi.absolutePath());

}

/// show Widget for loading Files
int LoadWidget::showLoad(){
  setAcceptMode ( QFileDialog::AcceptOpen );
  setWindowTitle(tr("Load Object"));
  loadMode_ = true;

  setFileMode(QFileDialog::ExistingFiles);

  slotSetLoadFilters();

  return this->exec();
}

/// show Widget for saving Files
int LoadWidget::showSave(int _id, QString _filename){
  setAcceptMode ( QFileDialog::AcceptSave );
  setFileMode( QFileDialog::AnyFile );
  setWindowTitle(tr("Save Object"));
  loadMode_ = false;

  id_ = _id;

  //set dataType
  BaseObjectData* object;
  PluginFunctions::getObject(_id,object);

  //check if we can save this dataType
  bool typeFound = false;
  
  for (int i=0; i < (int)supportedTypes_.size(); i++)
    if ( object->dataType( supportedTypes_[i].type ) )
      typeFound = true;


  if (!typeFound){
    std::cerr << "No suitable plugin for saving this dataType." << std::endl;
    return QDialog::Rejected;
  }

  slotSetSaveFilters( object->dataType() );

  //display correct path/name
  QFileInfo fi(_filename);
  QFile file(_filename);

  if (file.exists()) {
    setDirectory( fi.absolutePath() );
    selectFile ( fi.fileName() );
  } else {
//     setDirectory(OpenFlipper::Options::currentDir().absolutePath() );
    std::cout << "setting filename = " << _filename.toStdString() << std::endl;
    setDirectory( fi.absolutePath() );
    selectFile ( fi.fileName() );
  }


  //try to select the best fitting name filter
  for (int i=0; i < nameFilters().count(); i++){
    int s = nameFilters()[i].indexOf("*")+2;
    int e = nameFilters()[i].indexOf(" ", s);
    int e2 = nameFilters()[i].indexOf(")", s);
    if (e == -1 || e2 < e) e = e2;

    QString ext = nameFilters()[i].mid(s,e-s);

    if (ext == fi.completeSuffix()){
      selectNameFilter(nameFilters()[i]);
      break;
    }
  }

  return this->exec();
}

void LoadWidget::accept() {
  if ( loadMode_ )
    loadFile();
  else
    saveFile();


  QFileDialog::accept();
}

void LoadWidget::slotSetPluginForExtension(QString _extension, int _pluginId ){
  pluginForExtension_[ _extension ] = _pluginId;
}

void LoadWidget::getPluginForExtensionINI(QStringList _extensions){
    
  QString filename = OpenFlipper::Options::configDirStr() + OpenFlipper::Options::dirSeparator() + "OpenFlipper.ini";
  
  INIFile ini;
  
  if ( ! ini.connect(filename,false) ) {
    std::cerr << (tr("Failed to connect to ini file %1").arg(filename)).toStdString() << std::endl;
    return;
  }

  for (int i=0; i < _extensions.count(); i++){
    QString pluginName;
    
    if ( ini.get_entry(pluginName, "LoadSave" , "Extension_" + _extensions[i] ) ){
      
      for (uint t=0; t < supportedTypes_.size(); t++)
        if ( supportedTypes_[t].name == pluginName ){
          pluginForExtension_[ _extensions[i] ] = t;
          break;
        }
    }
  }

  // close ini file
  ini.disconnect();
}


