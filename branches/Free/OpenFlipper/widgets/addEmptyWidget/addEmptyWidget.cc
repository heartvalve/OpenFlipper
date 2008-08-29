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




#include "addEmptyWidget.hh"
#include <OpenFlipper/common/GlobalOptions.hh>

addEmptyWidget::addEmptyWidget(std::vector< DataType > _types , QStringList _typeNames, QWidget *parent)
  : QDialog(parent),
    types_(_types),
    typeNames_(_typeNames)
{
  setupUi(this);
  
  typeBox->setDuplicatesEnabled(false);

  for (int i=0; i < (int)types_.size(); i++)
    typeBox->addItem(typeNames_[i],QVariant(types_[i]));
  
    //set last used DataType as currentItem
  for (int i=0; i < typeBox->count(); i++)
    if (typeBox->itemText(i) == OpenFlipper::Options::lastDataType())
      typeBox->setCurrentIndex(i);
  
  connect(cancelButton,SIGNAL(clicked()),this,SLOT(close()));
  connect(addButton,SIGNAL(clicked()),this,SLOT(slotAddEmpty()));
}

void addEmptyWidget::slotAddEmpty(){
  int index = typeBox->currentIndex();
  int unused;
  
  DataType type = (DataType) typeBox->itemData(index).toInt();
  for (int i=0; i < (int)types_.size(); i++)
    if (types_[i] == type){
      emit chosen(type,unused);
      OpenFlipper::Options::lastDataType(typeBox->currentText());
      this->close();
      return;
    }
  emit chosen(DATA_NONE,unused);
  this->close();
}
