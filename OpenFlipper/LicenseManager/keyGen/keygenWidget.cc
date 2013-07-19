/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2011 by Computer Graphics Group, RWTH Aachen       *
*                           www.openflipper.org                              *
*                                                                            *
*--------------------------------------------------------------------------- *
*  This file is part of OpenFlipper.                                         *
*                                                                            *
*  OpenFlipper is free software: you can redistribute it and/or modify       *
*  it under the terms of the GNU Lesser General Public License as            *
*  published by the Free Software Foundation, either version 3 of            *
*  the License, or (at your option) any later version with the               *
*  following exceptions:                                                     *
*                                                                            *
*  If other files instantiate templates or use macros                        *
*  or inline functions from this file, or you compile this file and          *
*  link it with other files to produce an executable, this file does         *
*  not by itself cause the resulting executable to be covered by the         *
*  GNU Lesser General Public License. This exception does not however        *
*  invalidate any other reasons why the executable file might be             *
*  covered by the GNU Lesser General Public License.                         *
*                                                                            *
*  OpenFlipper is distributed in the hope that it will be useful,            *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
*  GNU Lesser General Public License for more details.                       *
*                                                                            *
*  You should have received a copy of the GNU LesserGeneral Public           *
*  License along with OpenFlipper. If not,                                   *
*  see <http://www.gnu.org/licenses/>.                                       *
*                                                                            *
\*===========================================================================*/

/*===========================================================================*\
*                                                                            *
*   $Revision$                                                       *
*   $LastChangedBy$                                                *
*   $Date$                     *
*                                                                            *
\*===========================================================================*/



#if QT_VERSION >= 0x050000 
  #include <QtWidgets>
#else
  #include <QtGui>
#endif

#include "keygenWidget.hh"
#include <iostream>

#include "salt.hh"

KeyGenWidget::KeyGenWidget(QMainWindow *parent)
    : QMainWindow(parent),
    valid_(false)
{
  setupUi(this);
  connect(generateButton,SIGNAL(clicked()),this,SLOT(slotGenerateButton()));
  
  
  connect(splitButton,SIGNAL(clicked()),this,SLOT(slotSplit()));
  
  connect(requestData,SIGNAL(textChanged()),this,SLOT(slotAnalyze()));
  
  // connect spinboxes forexpiry date
  connect(days  ,SIGNAL(valueChanged(int)),this,SLOT(slotDate()));
  connect(months,SIGNAL(valueChanged(int)),this,SLOT(slotDate()));
  connect(years ,SIGNAL(valueChanged(int)),this,SLOT(slotDate()));
  
  // Automatically set expire date to current date
  // For security reasons no default span is set here!
  expires->setDate( QDate::currentDate());
  
}

void KeyGenWidget::slotDate() {
  QDate today = QDate::currentDate();
  today = today.addDays(days->value());
  today = today.addMonths(months->value());
  today = today.addYears(years->value());
  
  expires->setDate(today);
}

void KeyGenWidget::slotAnalyze() {
  
  // Convert to text and split to elements
  QString inputData = requestData->toPlainText();
  QStringList data = inputData.split('\n',QString::SkipEmptyParts);
  
  // This is never a valid request!
  if ( data.size() < 6 ) {
    
    QPalette p = requestData->palette();
    p.setColor( QPalette::Base, QColor(255,0,0) );
    requestData->setPalette(p);
    
    valid_ = false;
    
    return;
  } else {
    QPalette p = requestData->palette();
    p.setColor( QPalette::Base, QColor(255,255,255) );
    requestData->setPalette(p);
  }
  
  // Get strings
  QString name        = data[0].simplified();
  QString coreHash    = data[1].simplified();
  QString pluginHash  = data[2].simplified();
  QString cpuHash     = data[3].simplified();
  QString productHash = data[4].simplified();
  
  QStringList macHashes;
  for ( int i = 5 ; i < data.size() - 1; ++i)
    macHashes.push_back(data[i].simplified());
  
  QString requestSig = data[data.size() - 1].simplified();

  fileNameBox->setText(name);
  coreHashBox->setText(coreHash);
  pluginHashBox->setText(pluginHash);
  cpuHashBox->setText(cpuHash);
  productIDBox->setText(productHash);
  
  macHashBox->setText(macHashes.join("\n"));
  
  signatureBox->setText(requestSig);
  
  
  // Get the salts
  QString saltPre;
  ADD_SALT_PRE(saltPre);
  QString saltPost;
  ADD_SALT_POST(saltPost);
  

  QString keyRequest      = saltPre + name + coreHash + pluginHash + cpuHash + productHash + macHashes.join("") +  saltPost;
  QString requestSigCheck = QCryptographicHash::hash ( keyRequest.toAscii()  , QCryptographicHash::Sha1 ).toHex();
  
  if ( requestSig != requestSigCheck ) {
    QPalette p = signatureBox->palette();
    p.setColor( QPalette::Base, QColor(255,0,0) );
    signatureBox->setPalette(p);
    
    valid_ = false;
    if (! ignoreSigBox->isChecked() )
      return;
    
  } else {
    QPalette p = signatureBox->palette();
    p.setColor( QPalette::Base, QColor(0,255,0) );
    signatureBox->setPalette(p);
    
  }

  QString expiryDate = expires->date().toString(Qt::ISODate);

  license_ = "";
  
  // Add basic hashes
  license_ += expiryDate + "\n";
  license_ += name + "\n";
  license_ += coreHash + "\n";
  license_ += pluginHash + "\n";
  license_ += cpuHash + "\n";
  license_ += productHash + "\n";
  license_ += macHashes.join("\n") + "\n";
  
  QString licenseTmp = saltPre + expiryDate + name + coreHash + pluginHash + cpuHash + productHash + macHashes.join("") +  saltPost;
  QString licenseHash = QCryptographicHash::hash ( licenseTmp.toAscii()  , QCryptographicHash::Sha1 ).toHex();
  
  // Prepend signature
  license_ = licenseHash + "\n" + license_;
  
  std::cerr << "Full license : \n" << license_.toStdString() << std::endl;
  
  // Only set valid, if the the request and the signature boxes match
  if ( requestSig == requestSigCheck )
    valid_ = true;
  
  licenseFileName_ = name;
}


void KeyGenWidget::slotSplit() {
  // Get request data
  QString inputData = requestData->toPlainText();
  
  // Split with ;
  QStringList data = inputData.split(";",QString::SkipEmptyParts);
  
  QString newText = data.join("\n");
  
  requestData->setText(newText);
  
}

KeyGenWidget::~KeyGenWidget() {

}


void KeyGenWidget::slotGenerateButton() {

  if ( ! valid_  ) {
    if (  ! ignoreSigBox->isChecked( ) ) {
      std::cerr << "Invalid Request " << std::endl;
      return;
    } else {
      std::cerr << "Invalid Request but overriding!!" << std::endl;
    }
  }
  
  std::cerr << "Writing License file to output : " << licenseFileName_.toStdString() << std::endl;
  QFile outFile(licenseFileName_ + ".lic");

  if (!outFile.open(QIODevice::WriteOnly|QIODevice::Text)) {
    QMessageBox::critical(this,tr("Unable to open file"),tr("Unable to Open output File"));
    return;
  }

  QTextStream output(&outFile);
  
  output << license_;

  outFile.close();
  
}
