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




#include "unloadPluginsWidget.hh"

unloadPluginsWidget::unloadPluginsWidget(QStringList plugins, QWidget *parent)
  : QWidget(parent)
{
  setupUi(this);
   
  listWidget->addItems(plugins);
  
  connect(cancelButton,SIGNAL(clicked()),this,SLOT(close()));
  connect(unloadButton,SIGNAL(clicked()),this,SLOT(slotUnload()));
}

void unloadPluginsWidget::slotUnload(){
  int buttonState = QMessageBox::No;
  QStringList dontLoad;

  for (int i=0; i < listWidget->selectedItems().size(); i++){
    QString name = listWidget->selectedItems()[i]->text();
    
    if (listWidget->selectedItems().size() == 1){
      //show messageBox without YESTOALL / NOTOALL
      buttonState = QMessageBox::question(this, tr("Prevent Plugin Loading"),
                   tr("Do you want to prevent OpenFlipper from loading this plugin on the next start?"),
                   QMessageBox::Yes | QMessageBox::No,
                   QMessageBox::No);
    }else{
      //show messageBox with YESTOALL / NOTOALL
      if (buttonState == QMessageBox::Yes || buttonState == QMessageBox::No)
      buttonState = QMessageBox::question(this, tr("Prevent Plugin Loading"),
                   tr("Do you want to prevent OpenFlipper from loading this plugin on the next start?"),
                   QMessageBox::Yes | QMessageBox::YesToAll | QMessageBox::No | QMessageBox::NoToAll ,
                   QMessageBox::No);
    }

  if (buttonState == QMessageBox::Yes || buttonState == QMessageBox::YesToAll)
    dontLoad << name;

    //unload plugin
    emit unload(name);
  }

  //prevent OpenFlipper from loading the plugin on the next start
  if (!dontLoad.isEmpty())
    emit dontLoadPlugins(dontLoad);

  close();
}

void unloadPluginsWidget::setPlugins(QStringList plugins){
  listWidget->clear();
  listWidget->addItems(plugins);
}
