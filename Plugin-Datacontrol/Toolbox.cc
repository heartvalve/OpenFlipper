#include "Toolbox.hh"
#include <QtGui>
#include <iostream>

DatacontrolToolboxWidget::DatacontrolToolboxWidget(QWidget* _parent)
    : QWidget(_parent)
{
}


void DatacontrolToolboxWidget::keyPressEvent (QKeyEvent * _event ) {
  if ( _event->key() == Qt::Key_Escape )
    _event->ignore();
  else
    emit keyEvent( _event );
}
