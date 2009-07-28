#include <qapplication.h>
#include <QtGui>
#include <QFile>
#include <iostream>
#include "salt.hh"

int main(int argc, char **argv)
{	
  QApplication a( argc, argv );

  if (argc == 2) {
    std::cerr  << " Call ./keygen <InfoFile> <Output Key File>" << std::endl;
    exit(1);
  }

  QFile file(argv[1]);

  if (!file.open(QIODevice::ReadOnly|QIODevice::Text))
    std::cerr << "Unable to open file " << std::endl;

  QString name       = file.readLine();
  QString coreHash   = file.readLine();
  QString pluginHash = file.readLine();
  QString macHash    = file.readLine();

  std::cerr << "Generating key for Plugin : " << name.toStdString() << std::endl;
  QString saltPre;
  ADD_SALT_PRE(saltPre);

  QString saltPost;
  ADD_SALT_POST(saltPost);

  QString keyClear = coreHash + saltPre + pluginHash + saltPost + macHash;

  QString keyHash = QCryptographicHash::hash ( keyClear.toAscii()  , QCryptographicHash::Sha1 ).toHex();
  std::cerr << "key is: " << keyHash.toStdString() << std::endl;

  return 0;

}

