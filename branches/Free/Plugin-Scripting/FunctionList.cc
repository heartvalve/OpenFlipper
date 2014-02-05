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

#include "FunctionList.hh"

#include <QApplication>

FunctionList::FunctionList(QWidget *parent)
    : QListWidget(parent)
{

}

void FunctionList::mousePressEvent(QMouseEvent *event)
{
  if (event->button() == Qt::LeftButton){
    dragStartPosition_ = event->pos();
    QListWidgetItem* item = itemAt(event->pos().x(), event->pos().y());
    if (item) 
      currentFn_ = item->text();
    else
      currentFn_ = "";
  }

  QListWidget::mousePressEvent(event);
}

void FunctionList::mouseMoveEvent(QMouseEvent *event)
{
    if (!(event->buttons() & Qt::LeftButton))
        return;
    if ((event->pos() - dragStartPosition_).manhattanLength()
          < QApplication::startDragDistance())
        return;

    QDrag *drag = new QDrag(this);
    QMimeData *mimeData = new QMimeData;

    mimeData->setText( currentFn_ );
    drag->setMimeData(mimeData);

    drag->exec(Qt::CopyAction);
}

void FunctionList::filter(QString _filter){

  for (int i=0; i < count(); i++){

    QString fnName = item(i)->text();
    QString slotDescription;
    QStringList params;
    QStringList descriptions;

    emit getDescription(fnName, slotDescription, params, descriptions);

    if ( fnName.contains(_filter,Qt::CaseInsensitive) || slotDescription.contains(_filter,Qt::CaseInsensitive) )
      item(i)->setHidden(false);
    else
      item(i)->setHidden(true);
  }
}

void FunctionList::reset(){
  for (int i=0; i < count(); i++)
    item(i)->setHidden(false);
}
