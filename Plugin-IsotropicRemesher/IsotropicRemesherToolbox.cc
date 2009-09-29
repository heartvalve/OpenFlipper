#include "IsotropicRemesherToolbox.hh"
#include <QtGui>

IsotropicRemesherToolBox::IsotropicRemesherToolBox(QWidget *parent)
    : QWidget(parent)
{
    setupUi(this);
}

void IsotropicRemesherToolBox::showEvent ( QShowEvent * _event )
{
  QWidget::showEvent ( _event );
  emit showing();
}
