#include "Toolbox.hh"
#include <QtGui>
#include <iostream>

DatacontrolToolboxWidget::DatacontrolToolboxWidget(QWidget* _parent)
    : QWidget(_parent)
{
}


void DatacontrolToolboxWidget::keyPressEvent (QKeyEvent * _event ) {
  emit keyEvent( _event );
}
