#include <qapplication.h>
#include <QtGui>
#include <QFile>
#include <QMessageBox>
#include <iostream>
#include <OpenFlipper/LicenseManager/keyGen/keygenWidget.hh>
#include "salt.hh"

int main(int argc, char **argv)
{	
  QApplication a( argc, argv );

  // Get license information
  QString name;
  QString coreHash;
  QString pluginHash;
  QString macHash;
  QString requestSig;

  if (argc == 3) {
  
    QFile file(argv[1]);

    if (!file.open(QIODevice::ReadOnly|QIODevice::Text)) {
      std::cerr << "Unable to open file " << std::endl;
      std::cerr  << " Call ./keygen <InfoFile> <Expiry date>" << std::endl;
      std::cerr << "Date is of the form: YYYY-MM-DD" << std::endl;
      return 1;
    }

    // Get license information
    name       = file.readLine().simplified();
    coreHash   = file.readLine().simplified();
    pluginHash = file.readLine().simplified();
    macHash    = file.readLine().simplified();
    requestSig = file.readLine().simplified();

    file.close();

  } else {

    MainWindow* mainWindow = new MainWindow();
    mainWindow->show();

    a.exec();

    QString inputData = mainWindow->textEdit_->toPlainText();

    QStringList data = inputData.split('\n');
    
    if ( data.size() != 5 ) {
      std::cerr << "Request data has to containe 5 lines!" << std::endl;
      exit(1);
    }
  
    // Clean strings
    name       = data[0].simplified();
    coreHash   = data[1].simplified();
    pluginHash = data[2].simplified();
    macHash    = data[3].simplified();
    requestSig = data[4].simplified();
   
  } 

  std::cerr << "Generating key for Plugin : " << name.toStdString()       << std::endl;
  std::cerr << "Core Hash                 : " << coreHash.toStdString()   << std::endl;
  std::cerr << "Plugin Hash               : " << pluginHash.toStdString() << std::endl;
  std::cerr << "macHash is                : " << macHash.toStdString()    << std::endl;
  std::cerr << "requestSignature is       : " << requestSig.toStdString() << std::endl;

  // Get the salts
  QString saltPre;
  ADD_SALT_PRE(saltPre);
  QString saltPost;
  ADD_SALT_POST(saltPost);

  QString keyRequest = saltPre + name + coreHash + pluginHash + macHash + saltPost;
  QString requestSigCheck = QCryptographicHash::hash ( keyRequest.toAscii()  , QCryptographicHash::Sha1 ).toHex();
  
  if ( requestSig != requestSigCheck ) {
    QMessageBox::critical(0,"Signature of request invalid","The signature of the request is not valid");
    return 1;
  }

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
  


  // Sign the license file
  QString license = saltPre + name + coreHash + pluginHash + macHash + date.toString(Qt::ISODate) + saltPost;

  license = saltPre + license + saltPost;
  QString licenseHash = QCryptographicHash::hash ( license.toAscii()  , QCryptographicHash::Sha1 ).toHex();
  
  output << licenseHash;

  outFile.close();

  return 0;

}

