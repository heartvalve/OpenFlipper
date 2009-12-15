#include "FileOptionsDialog.hh"

#include <OpenFlipper/INIFile/INIFile.hh>
#include <OpenFlipper/common/GlobalOptions.hh>

#include <QVBoxLayout>
#include <QGridLayout>

FileOptionsDialog::FileOptionsDialog(std::vector<fileTypes>& _supportedTypes, QStringList _extensions, bool _loadMode, QWidget *parent)
  : QDialog(parent),
    ext_(_extensions),
    supportedTypes_(_supportedTypes),
    loadMode_(_loadMode)
{

  ext_.removeDuplicates();
  
  QGridLayout* grid = new QGridLayout;
  
  QVector< int > usefulPlugins;
  
  //check if more than one plugin is able to handle this extension
  for (int i=0; i < ext_.size(); i++){
    int count = 0;
    QStringList names;
    
    for (uint t=0; t < supportedTypes_.size(); t++){
      
      QString filters;
      
      if (loadMode_)
        filters = supportedTypes_[t].loadFilters;
      else
        filters = supportedTypes_[t].saveFilters;
      
      if (filters.contains(ext_[i])){
        count++;
        names.push_back( supportedTypes_[t].name );
        usefulPlugins.push_back( t );
      }
    }
    
    //more than one plugin can handle the extension
    if (count > 1){
      QLabel*  label = new QLabel( tr("For *.%1 use plugin ").arg(ext_[i]) );
      QComboBox* box = new QComboBox();
      
      box->addItems(names);
      box->setAccessibleName(ext_[i]);

      grid->addWidget(label, grid->rowCount(), 0);
      grid->addWidget(box,   grid->rowCount()-1, 1);
      
      connect(box, SIGNAL(currentIndexChanged(QString)), this, SLOT(slotPluginChanged(QString)) );
      boxes_.push_back(box);
    }
  }
  
  //force TriangleMesh as initial value if available
  for (int i=0; i < boxes_.count(); i++){
    
    for (int t=0; t < (boxes_[i])->count(); t++)
      if ( (boxes_[i])->itemText(t).contains("TriangleMesh") ){
        (boxes_[i])->setCurrentIndex(t);
        break;
      }
  }
  
  //load defaults from ini file
  QString filename = OpenFlipper::Options::configDirStr() + OpenFlipper::Options::dirSeparator() + "OpenFlipper.ini";
  
  INIFile ini;
  
  if ( ! ini.connect(filename,false) ) {
    std::cerr << (tr("Failed to connect to ini file %1").arg(filename)).toStdString() << std::endl;
    return;
  }

  for (int i=0; i < boxes_.count(); i++){
     QString pluginName;
    
    if ( ini.get_entry(pluginName, "LoadSave" , "Extension_" + _extensions[i] ) ){
      
      for (int t=0; t < (boxes_[i])->count(); t++)
        if ( (boxes_[i])->itemText(t) == pluginName ){
          (boxes_[i])->setCurrentIndex(t);
          break;
        }
    }
  }

  // close ini file
  ini.disconnect();
  

  QGroupBox* group = new QGroupBox(tr("Extensions with multiple plugins"));
  group->setLayout(grid);
  
  if (boxes_.count() == 0)
    group->setVisible(false);

  
  //add option widgets from all fileplugins
  for (uint t=0; t < supportedTypes_.size(); t++){
    
    if ( !usefulPlugins.contains(t) ) 
      continue;
    
    QWidget* widget;

    if (loadMode_)
      widget = supportedTypes_[t].plugin->loadOptionsWidget("");
    else
      widget = supportedTypes_[t].plugin->saveOptionsWidget("");

    if (widget != 0)
      tabs_.addTab(widget, supportedTypes_[t].name);
  }

  //add buttons at bottom
  QPushButton* cancel = new QPushButton(tr("&Cancel"));
  QPushButton* ok = new QPushButton(tr("&Ok"));
  
  QHBoxLayout* buttonLayout = new QHBoxLayout;
  
  buttonLayout->addWidget(cancel);
  buttonLayout->addStretch();
  buttonLayout->addWidget(ok);

  QVBoxLayout* layout = new QVBoxLayout;
  layout->addWidget(group);
  layout->addWidget(&tabs_);
  layout->addLayout(buttonLayout);
  
  setLayout(layout);
  
  connect( cancel, SIGNAL(clicked()), this, SLOT(reject()) );
  connect(     ok, SIGNAL(clicked()), this, SLOT(accept()) );
}

FileOptionsDialog::~FileOptionsDialog()
{
  //remove tabs
  for (int i=tabs_.count()-1; i >= 0; i--)
    tabs_.removeTab(i);
  
  //and set parent of widgets to NULL
  for (uint t=0; t < supportedTypes_.size(); t++){
    
    QWidget* widget;
    
    if (loadMode_)
      widget = supportedTypes_[t].plugin->loadOptionsWidget("");
    else
      widget = supportedTypes_[t].plugin->saveOptionsWidget("");

    if (widget != 0)
      widget->setParent(0);
  }
}

void FileOptionsDialog::slotPluginChanged(QString _name){
  
  QComboBox* box = dynamic_cast<QComboBox*>(QObject::sender());

  for (uint t=0; t < supportedTypes_.size(); t++)
    if ( supportedTypes_[t].name == _name ){
      emit setPluginForExtension(box->accessibleName(), t ); //accessibleName contains the extension
      break;
    }
}


