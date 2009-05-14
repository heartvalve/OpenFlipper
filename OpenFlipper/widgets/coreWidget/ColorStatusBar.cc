#include "ColorStatusBar.hh"

ColorStatusBar::ColorStatusBar(QWidget* parent) : QStatusBar(parent)
{
  connect (this, SIGNAL(messageChanged(const QString&)), this, SLOT(slotMessageChanged(const QString &)) );
  colored_ = false;
}

void ColorStatusBar::showMessage(const QString & message, int timeout)
{
  QStatusBar::showMessage(message, timeout);
}

void ColorStatusBar::showMessage(const QString & message, QColor _color, int timeout)
{
  QPalette pal = palette();
  pal.setColor(QPalette::Active, QPalette::WindowText, _color );
  setPalette(pal);

  colored_ = true;

  QStatusBar::showMessage(message, timeout);
}

void ColorStatusBar::slotMessageChanged(const QString & message)
{
  if ( !colored_ ){

    QPalette pal = palette();
    pal.setColor(QPalette::Active, QPalette::WindowText, QColor(Qt::black) );
    setPalette(pal);

  } else 
    colored_ = false;
}

