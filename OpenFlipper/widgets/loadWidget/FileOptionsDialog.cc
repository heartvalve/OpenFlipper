/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2011 by Computer Graphics Group, RWTH Aachen       *
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

#include "FileOptionsDialog.hh"

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
    
    for (unsigned int t=0; t < supportedTypes_.size(); t++){
      
      QString filters;
      
      if (loadMode_)
        filters = supportedTypes_[t].loadFilters;
      else
        filters = supportedTypes_[t].saveFilters;

      if (filters.contains("." + ext_[i],Qt::CaseInsensitive)){
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
  
  QGroupBox* group = new QGroupBox(tr("Extensions with multiple plugins"));
  group->setLayout(grid);
  
  if (boxes_.count() == 0)
    group->setVisible(false);

  
  //add option widgets from all fileplugins
  for (unsigned int t=0; t < supportedTypes_.size(); t++){
    
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
  for (unsigned int t=0; t < supportedTypes_.size(); t++){
    
    QWidget* widget;
    
    if (loadMode_)
      widget = supportedTypes_[t].plugin->loadOptionsWidget("");
    else
      widget = supportedTypes_[t].plugin->saveOptionsWidget("");

    if (widget != 0)
      widget->setParent(0);
  }
}

int FileOptionsDialog::exec(){
  
  if ( tabs_.count() == 0 && boxes_.count() == 0 )
    return QDialog::Accepted;
  else
    return QDialog::exec();
}

void FileOptionsDialog::slotPluginChanged(QString _name){
  
  QComboBox* box = dynamic_cast<QComboBox*>(QObject::sender());

  for (unsigned int t=0; t < supportedTypes_.size(); t++)
    if ( supportedTypes_[t].name == _name ){
      emit setPluginForExtension(box->accessibleName(), t ); //accessibleName contains the extension
      break;
    }
}


