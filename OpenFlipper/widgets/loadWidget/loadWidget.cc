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

LoadWidget::LoadWidget(std::vector<fileTypes>& _supportedTypes , QWidget *parent)
  : QFileDialog(parent),
    loadMode_(true),
    supportedTypes_(_supportedTypes)

{

  // Get our layout
  QGridLayout *gridLayout = (QGridLayout*)layout();

  //supported Types
  typeBox_ = new QComboBox(this);
  typeBox_->setDuplicatesEnabled(false);
  for (int i=0; i < (int)supportedTypes_.size(); i++)
    typeBox_->addItem(supportedTypes_[i].plugin->typeName(),QVariant(supportedTypes_[i].type));

  // add the type box to the bottom
  gridLayout->addWidget( typeBox_, gridLayout->rowCount() , 1 );

  // Add a nice label for it
  QLabel* typeLabel = new QLabel("Object type:" , this);
  gridLayout->addWidget( typeLabel, gridLayout->rowCount() -1 , 0 );

  // Add a frame to the right of the dialog
  box_ = new QGroupBox( this ) ;
  box_->setFlat ( false );
  box_->setSizePolicy( QSizePolicy ( QSizePolicy::Preferred , QSizePolicy::Expanding ) );
  gridLayout->addWidget( box_, 0 , gridLayout->columnCount(),gridLayout->rowCount(),1 );

  //init layout for the frame
  boxLayout_ = new QHBoxLayout();
  box_->setLayout(boxLayout_);

  //set last used DataType as currentItem
  for (int i=0; i < typeBox_->count(); i++){
    if (typeBox_->itemText(i) == OpenFlipper::Options::lastDataType()){
      typeBox_->setCurrentIndex(i);
      if (loadMode_)
        slotSetLoadFilters(i);
      else
        slotSetSaveFilters(i);
    }
  }

  connect(this, SIGNAL(filterSelected(QString)), this, SLOT(currentFilterChanged(QString)));

  //overwrite dialog shouldn't be handled by the qfiledialog
  setConfirmOverwrite(false);

  setDirectory(OpenFlipper::Options::currentDirStr());
}

/// adjust the loadWidget / saveWidget when the selected nameFilter changed
void LoadWidget::currentFilterChanged(QString _currentFilter){

  for (int i=0; i < (int)supportedTypes_.size(); i++){
    if (supportedTypes_[i].loadWidget != 0){
      boxLayout_->removeWidget(supportedTypes_[i].loadWidget);
      supportedTypes_[i].loadWidget->setParent(0);
    }
    if (supportedTypes_[i].saveWidget != 0){
      boxLayout_->removeWidget(supportedTypes_[i].saveWidget);
      supportedTypes_[i].saveWidget->setParent(0);
    }
  } 

  const DataType type = (DataType) typeBox_->itemData( typeBox_->currentIndex() ).toInt();

  if ( loadMode_ ){
  for (int i=0; i < (int)supportedTypes_.size(); i++)
    if (supportedTypes_[i].type == type){
      //add Widget for new Filter
      QWidget* nuWidget = 0;
      nuWidget = supportedTypes_[i].plugin->loadOptionsWidget(_currentFilter);

      if (nuWidget != 0){
        boxLayout_->addWidget( nuWidget );
        box_->show();
      }else
      if ( supportedTypes_[i].loadWidget != 0 ) {
        boxLayout_->addWidget(supportedTypes_[i].loadWidget);
        box_->show();
      }
    }
  }else{
    for (int i=0; i < (int)supportedTypes_.size(); i++)
      if (supportedTypes_[i].type == type){
        //add Widget for new Filter
        QWidget* nuWidget = 0;
        nuWidget = supportedTypes_[i].plugin->saveOptionsWidget(_currentFilter);
  
        if (nuWidget != 0){
          boxLayout_->addWidget( nuWidget );
          box_->show();
        }else
        if ( supportedTypes_[i].saveWidget != 0 ) {
          boxLayout_->addWidget(supportedTypes_[i].saveWidget);
          box_->show();
        }
      }
  }
}

/// adjust load-filters to current datatype
void LoadWidget::slotSetLoadFilters(int _typeIndex){
  box_->setTitle("Load Options");

  for (int i=0; i < (int)supportedTypes_.size(); i++){
    if (supportedTypes_[i].loadWidget != 0){
      boxLayout_->removeWidget(supportedTypes_[i].loadWidget);
      supportedTypes_[i].loadWidget->setParent(0);
    }
    if (supportedTypes_[i].saveWidget != 0){
      boxLayout_->removeWidget(supportedTypes_[i].saveWidget);
      supportedTypes_[i].saveWidget->setParent(0);
    }
  }


  const DataType type = (DataType) typeBox_->itemData(_typeIndex).toInt();
  for (int i=0; i < (int)supportedTypes_.size(); i++)
    if (supportedTypes_[i].type == type){
      QStringList filters = supportedTypes_[i].loadFilters.split(";;");
      for (int f=0; f < filters.size(); f++)
        if (filters[f].trimmed() == "") filters.removeAt(f);
      setNameFilters(filters);

      //add Widget for new Filter
      if ( supportedTypes_[i].loadWidget != 0 ) {
        boxLayout_->addWidget(supportedTypes_[i].loadWidget);
//         boxLayout_->removeWidget(supportedTypes_[i].saveWidget);
        box_->show();
      }

    }

  if (box_->children().count() == 1) //only the layout is left
    box_->hide();
}

/// adjust save-filters to current datatype
void LoadWidget::slotSetSaveFilters(int _typeIndex){
  box_->setTitle("Save Options");

  for (int i=0; i < (int)supportedTypes_.size(); i++){
    if (supportedTypes_[i].loadWidget != 0){
      boxLayout_->removeWidget(supportedTypes_[i].loadWidget);
      supportedTypes_[i].loadWidget->setParent(0);
    }
    if (supportedTypes_[i].saveWidget != 0){
      boxLayout_->removeWidget(supportedTypes_[i].saveWidget);
      supportedTypes_[i].saveWidget->setParent(0);
    }
  }

  const DataType type = (DataType) typeBox_->itemData(_typeIndex).toInt();
  for (int i=0; i < (int)supportedTypes_.size(); i++)
    if (supportedTypes_[i].type == type){
      QStringList filters = supportedTypes_[i].saveFilters.split(";;");
      for (int f=0; f < filters.size(); f++)
        if (filters[f].trimmed() == "") filters.removeAt(f);
      setNameFilters(filters);

      //add Widget for new Filter
      if ( supportedTypes_[i].saveWidget != 0 ) {
        boxLayout_->addWidget(supportedTypes_[i].saveWidget);
//         boxLayout_->removeWidget(supportedTypes_[i].saveWidget);
        box_->show();
      }

    }
      //remove Widget from last Filter
//       if ( supportedTypes_[i].loadWidget != 0 ) {
//         boxLayout_->removeWidget(supportedTypes_[i].loadWidget);
//         boxLayout_->removeWidget(supportedTypes_[i].saveWidget);
//         supportedTypes_[i].saveWidget->setParent(0);
//       }

  if (box_->children().count() == 1) //only the layout is left
    box_->hide();
}



/// find suitable plugin for loading current file
void LoadWidget::loadFile(){

  //get selection
  QStringList files = selectedFiles();

  bool success = false;

  //iterate over selection
  for (int i=0; i < files.size(); i++){
    QFileInfo fi(files[i]);
    QString filename = fi.absoluteFilePath();
    QFile file(filename);
    if (fi.isDir() || !file.exists()) continue; //do nothing if its a not a valid file
    QString ext = fi.suffix();

    //emit load signal
    DataType type = (DataType) typeBox_->itemData(typeBox_->currentIndex()).toInt();
    int unused;
    emit load(filename, type, unused);
    success = true;
  }

  if (success) //at least one item was loaded successfully
    OpenFlipper::Options::lastDataType(typeBox_->currentText());
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

  QFile f(filename);
  if (f.exists()){ //check for extension
    int ret = QMessageBox::warning(this, tr("File exists"),tr("This file already exists.\n"
        "Do you want to overwrite the file?"),QMessageBox::Yes|QMessageBox::No,QMessageBox::No);
    if (ret == QMessageBox::No)
      return; //abort if users doesn't want to overwrite
  }
  QFileInfo fi(filename);
  QString ext = fi.suffix();

  emit save(id_,filename);
  OpenFlipper::Options::lastDataType(typeBox_->currentText());

  OpenFlipper::Options::currentDir(fi.absolutePath());

}

/// show Widget for loading Files
int LoadWidget::showLoad(){
  setAcceptMode ( QFileDialog::AcceptOpen );
  setWindowTitle("Load Object");
  typeBox_->setEnabled(true);
  loadMode_ = true;

  setFileMode(QFileDialog::ExistingFiles);
  disconnect(typeBox_, SIGNAL(activated(int)), 0, 0);
  connect(typeBox_,SIGNAL(activated(int)),this,SLOT(slotSetLoadFilters(int)));

  slotSetLoadFilters(typeBox_->currentIndex());

  return this->exec();
}

/// show Widget for saving Files
int LoadWidget::showSave(int _id, QString _filename){
  setAcceptMode ( QFileDialog::AcceptSave );
  setFileMode( QFileDialog::AnyFile );
  setWindowTitle("Save Object");
  typeBox_->setEnabled(false);
  loadMode_ = false;

  id_ = _id;

  //set dataType
  BaseObjectData* object;
  PluginFunctions::get_object(_id,object);

  bool typeFound = false;
  for (int i=0; i < typeBox_->count(); i++){
    DataType type = (DataType) typeBox_->itemData(i).toInt();
    if (object->dataType(type)){
      typeBox_->setCurrentIndex(i);
      slotSetSaveFilters(i);
      typeFound = true;
    }
  }

  if (!typeFound){
    std::cerr << "No suitable plugin for saving this dataType." << std::endl;
    return QDialog::Rejected;
  }

  disconnect(typeBox_, SIGNAL(activated(int)), 0, 0);
  connect(typeBox_,SIGNAL(activated(int)),this,SLOT(slotSetSaveFilters(int)));

  slotSetSaveFilters(typeBox_->currentIndex());

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

//workarround to force repaint
  if (x() > 0){
    if (step_)
      setGeometry(x(),y(),width()+1,height());
    else
      setGeometry(x(),y(),width()-1,height());
  }
  step_ = !step_;

  return this->exec();
}

void LoadWidget::accept() {
  if ( loadMode_ )
    loadFile();
  else
    saveFile();


  QFileDialog::accept();
}
