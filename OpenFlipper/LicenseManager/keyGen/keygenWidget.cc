/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2009 by Computer Graphics Group, RWTH Aachen      *
 *                           www.openflipper.org                             *
 *                                                                           *
 *---------------------------------------------------------------------------*
 *  This file is part of OpenFlipper.                                        *
 *                                                                           *
 *  OpenFlipper is free software: you can redistribute it and/or modify      *
 *  it under the terms of the GNU Lesser General Public License as           *
 *  published by the Free Software Foundation, either version 3 of           *
 *  the License, or (at your option) any later version with the              *
 *  following exceptions:                                                    *
 *                                                                           *
 *  If other files instantiate templates or use macros                       *
 *  or inline functions from this file, or you compile this file and         *
 *  link it with other files to produce an executable, this file does        *
 *  not by itself cause the resulting executable to be covered by the        *
 *  GNU Lesser General Public License. This exception does not however       *
 *  invalidate any other reasons why the executable file might be            *
 *  covered by the GNU Lesser General Public License.                        *
 *                                                                           *
 *  OpenFlipper is distributed in the hope that it will be useful,           *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU Lesser General Public License for more details.                      *
 *                                                                           *
 *  You should have received a copy of the GNU LesserGeneral Public          *
 *  License along with OpenFlipper. If not,                                  *
 *  see <http://www.gnu.org/licenses/>.                                      *
 *                                                                           *
\*===========================================================================*/

/*===========================================================================*\
 *                                                                           *
 *   $Revision$                                                         *
 *   $Author$                                                      *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/


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
    QMessageBox::critical(this,tr("Wrong request data"),tr("The request has to contain 5 lines of data"));
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
      QMessageBox::critical(this,tr("Signature of request invalid"),tr("The signature of the request is not valid"));
      return;
    }

    std::cerr << "Writing License file to output : " << name.toStdString() << std::endl;
    QFile outFile(name + ".lic");

    if (!outFile.open(QIODevice::WriteOnly|QIODevice::Text)) {
      QMessageBox::critical(this,tr("Unable to open file"),tr("Unable to Open output File"));
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
