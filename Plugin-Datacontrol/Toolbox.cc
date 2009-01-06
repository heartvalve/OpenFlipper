#include "Toolbox.hh"
#include <QtGui>
#include <iostream>

DatacontrolToolboxWidget::DatacontrolToolboxWidget(QWidget* _parent)
    : QWidget(_parent)
{
}


void DatacontrolToolboxWidget::keyPressEvent (QKeyEvent * _event ) {
  // Only react on our own key events
  if ( ( (_event->modifiers() & Qt::ControlModifier) && ( _event->key() == Qt::Key_A ) ) ||
     ( _event->key() == Qt::Key_Delete ) )
    emit keyEvent( _event );
  else
    _event->ignore();
}
