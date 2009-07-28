#include <qapplication.h>
#include <QtGui>
#include <QFile>
#include <iostream>
#include "salt.hh"

int main(int argc, char **argv)
{	
  QApplication a( argc, argv );

  if (argc == 2) {
    std::cerr  << " Call ./keygen <InfoFile> <Expiry date>" << std::endl;
    std::cerr << "Date is of the form: YYYY-MM-DD" << std::endl;
    exit(1);
  }

  QFile file(argv[1]);

  if (!file.open(QIODevice::ReadOnly|QIODevice::Text)) {
    std::cerr << "Unable to open file " << std::endl;
    return 1;
  }

  // Get license information
  QString name       = file.readLine().simplified();
  QString coreHash   = file.readLine().simplified();
  QString pluginHash = file.readLine().simplified();
  QString macHash    = file.readLine().simplified();

  file.close();

  std::cerr << "Generating key for Plugin : " << name.toStdString() << std::endl;
  std::cerr << "Core Hash   : " << coreHash.toStdString() << std::endl;
  std::cerr << "Plugin Hash : " << pluginHash.toStdString() << std::endl;
  std::cerr << "macHash is  : " << macHash.toStdString()  << std::endl;


  std::cerr << "Writing License file to output : " << name.toStdString() << std::endl;
  QFile outFile(name + ".lic");

  if (!outFile.open(QIODevice::WriteOnly|QIODevice::Text)) {
    std::cerr << "Unable to open file " << std::endl;
    return 1;
  }

  QTextStream output(&outFile);

  // Add basic hashes
  output << name         << "\n";
  output << coreHash     << "\n";
  output << pluginHash   << "\n";
  output << macHash      << "\n";

  // Add expiryDate
  QDate date = QDate::fromString(argv[2],Qt::ISODate);

  output << date.toString(Qt::ISODate) << "\n";
  
  // Get the salts
  QString saltPre;
  ADD_SALT_PRE(saltPre);
  QString saltPost;
  ADD_SALT_POST(saltPost);

  // Sign the license file
  QString license = saltPre + name + coreHash + pluginHash + macHash + date.toString(Qt::ISODate) + saltPost;

  license = saltPre + license + saltPost;
  QString licenseHash = QCryptographicHash::hash ( license.toAscii()  , QCryptographicHash::Sha1 ).toHex();
  
  output << licenseHash;

  outFile.close();

  return 0;

}

