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
    typeBox->addItem(typeNames_[i],QVariant(types_[i].value()));

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
  emit chosen(DATA_UNKNOWN,unused);
  this->close();
}
