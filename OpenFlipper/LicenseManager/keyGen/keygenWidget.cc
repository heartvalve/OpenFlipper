
#include <QtGui>
#include "keygenWidget.hh"
#include <iostream>

#include "salt.hh"

KeyGenWidget::KeyGenWidget(QMainWindow *parent)
    : QMainWindow(parent)
{
  setupUi(this);
  connect(generateButton,SIGNAL(clicked()),this,SLOT(slotGenerateButton()));
  
  // Automatically set expire date to current date + 1 Year
  QDate today = QDate::currentDate();
  expires->setDate(today.addYears(1));
  
}

KeyGenWidget::~KeyGenWidget() {

}

void KeyGenWidget::slotGenerateButton() {

  QString inputData = requestData->toPlainText();
  QStringList data = inputData.split('\n',QString::SkipEmptyParts);

  if ( data.size() != 5) {
    QMessageBox::critical(this,"Wrong request data","The request has to contain 5 lines of data");
  } else {

    // Clean strings
    QString name       = data[0].simplified();
    QString coreHash   = data[1].simplified();
    QString pluginHash = data[2].simplified();
    QString macHash    = data[3].simplified();
    QString requestSig = data[4].simplified();

    QString expiryDate = expires->date().toString(Qt::ISODate);

    std::cerr << "Generating key for Plugin : " << name.toStdString()       << std::endl;
    std::cerr << "Core Hash                 : " << coreHash.toStdString()   << std::endl;
    std::cerr << "Plugin Hash               : " << pluginHash.toStdString() << std::endl;
    std::cerr << "macHash is                : " << macHash.toStdString()    << std::endl;
    std::cerr << "requestSignature is       : " << requestSig.toStdString() << std::endl;
    std::cerr << "expiryDate is             : " << expiryDate.toStdString() << std::endl;

    // Get the salts
    QString saltPre;
    ADD_SALT_PRE(saltPre);
    QString saltPost;
    ADD_SALT_POST(saltPost);

    QString keyRequest = saltPre + name + coreHash + pluginHash + macHash + saltPost;
    QString requestSigCheck = QCryptographicHash::hash ( keyRequest.toAscii()  , QCryptographicHash::Sha1 ).toHex();
    
    if ( requestSig != requestSigCheck ) {
      QMessageBox::critical(this,"Signature of request invalid","The signature of the request is not valid");
      return;
    }

    std::cerr << "Writing License file to output : " << name.toStdString() << std::endl;
    QFile outFile(name + ".lic");

    if (!outFile.open(QIODevice::WriteOnly|QIODevice::Text)) {
      QMessageBox::critical(this,"Unable to open file","Unable to Open output File");
      return;
    }

    QTextStream output(&outFile);

    // Add basic hashes
    output << name         << "\n";
    output << coreHash     << "\n";
    output << pluginHash   << "\n";
    output << macHash      << "\n";
    output << expiryDate << "\n";
    
    // Sign the license file
    QString license = saltPre + name + coreHash + pluginHash + macHash + expiryDate + saltPost;
    QString licenseHash = QCryptographicHash::hash ( license.toAscii()  , QCryptographicHash::Sha1 ).toHex();
    
    output << licenseHash;

    outFile.close();
  }
  
  close();

  
 
}
