#include <qapplication.h>
#include <QtGui>
#include <QFile>
#include <QMessageBox>
#include <iostream>
#include <OpenFlipper/LicenseManager/keyGen/keygenWidget.hh>

int main(int argc, char **argv)
{	
  QApplication a( argc, argv );

  KeyGenWidget* keyGenWidget = new KeyGenWidget(0);

  // Show the widget
  keyGenWidget->show();
  a.exec();



  return 0;

}

