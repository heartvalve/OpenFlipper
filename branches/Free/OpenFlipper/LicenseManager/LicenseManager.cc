//=============================================================================
//
//                               OpenFlipper
//        Copyright (C) 2008 by Computer Graphics Group, RWTH Aachen
//                           www.openflipper.org
//
//-----------------------------------------------------------------------------
//
//                                License
//
//  OpenFlipper is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  OpenFlipper is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License
//  along with OpenFlipper.  If not, see <http://www.gnu.org/licenses/>.
//
//-----------------------------------------------------------------------------
//
//   $Revision: 6574 $
//   $Author: moebius $
//   $Date: 2009-07-08 14:06:06 +0200 (Mi, 08. Jul 2009) $
//
//=============================================================================

#include <OpenFlipper/LicenseManager/LicenseManager.hh>
#include <OpenFlipper/common/GlobalOptions.hh>
#include <QFile>
#include <QCryptographicHash>
#include <QNetworkInterface>

//
// CpuIDSupported will return 0 if CPUID instruction is unavailable. Otherwise, it will return 
// the maximum supported standard function.
//

unsigned int CpuIDSupported(void)
{
  unsigned int MaxInputValue;
  // If CPUID instruction is supported
  #ifndef WIN32
      try
      {
          MaxInputValue = 0;
	  // call cpuid with eax = 0
	  asm
	  (
	      "xorl %%eax,%%eax\n\t"
	      "cpuid\n\t"
		      : "=a" (MaxInputValue)
		      :
		      : "%ebx", "%ecx", "%edx"
		  );
      }
      catch (...)
      {
	  return(0);                   // cpuid instruction is unavailable
      }
  #else //Win32
      try
      {
	  MaxInputValue = 0;
	  // call cpuid with eax = 0
	  __asm
	  {
	      xor eax, eax
	      cpuid
	      mov MaxInputValue, eax
	  }
      }
      catch (...)
      {
	  return(0);                   // cpuid instruction is unavailable
      }
  #endif
  return MaxInputValue;
}

//
// GenuineIntel will return 0 if the processor is not a Genuine Intel Processor
//
unsigned int GenuineIntel(void)
{
  #ifndef WIN32
    unsigned int VendorIDb = 0,VendorIDd = 0, VendorIDc = 0;
    try
    // If CPUID instruction is supported
    {
      // Get vendor id string
      asm
      (
        //get the vendor string
        // call cpuid with eax = 0
              "xorl %%eax, %%eax\n\t"
              "cpuid\n\t"
          :   "=b" (VendorIDb),
              "=d" (VendorIDd),
              "=c" (VendorIDc)
          :
          : "%eax"
      );
    }
    catch (...)
    {
      return(0);                   // cpuid instruction is unavailable
    }
    return (
                (VendorIDb  == 'uneG') &&
                (VendorIDd  == 'Ieni') &&
                (VendorIDc  == 'letn')
            );
  #else
    unsigned int VendorID[3] = {0, 0, 0};
    try    // If CPUID instruction is supported
    {
      __asm
      {
          xor eax, eax            // call cpuid with eax = 0
          cpuid                    // Get vendor id string
          mov VendorID, ebx
          mov VendorID + 4, edx
          mov VendorID + 8, ecx
      }
    }
    catch (...)
    {
      return(0);
      unsigned int MaxInputValue =0;
      // cpuid instruction is unavailable
    }
    return (
               (VendorID[0] == 'uneG') &&
               (VendorID[1] == 'Ieni') &&
               (VendorID[2] == 'letn')
           );
  #endif
}



LicenseManager::~LicenseManager() 
{ 
  exit(0); 
}

LicenseManager::LicenseManager()
{
  authenticated_ = false;
  std::cerr << "Constructor Security Interface" << std::endl;
  QObject::blockSignals( true );
}

void LicenseManager::blockSignals( bool _state) {
  std::cerr << "Block Signals called" << std::endl;
  if ( !authenticated() ) {
    QObject::blockSignals( true );
    std::cerr << "Request to unblock plugin denied" << std::endl;
  } else {
    QObject::blockSignals(_state);
    std::cerr << "Unblocked Signal" << std::endl;
  }

}

bool LicenseManager::authenticate() {
  std::cerr << "Auth slot" << std::endl;

  std::cerr << "Application Directory: " << OpenFlipper::Options::applicationDirStr().toStdString() << std::endl;
  std::cerr << "Plugin Directory: " << OpenFlipper::Options::pluginDirStr().toStdString() << std::endl;
  

  std::cerr << "Name: " << name().toStdString() << std::endl;

  // ===============================================================================================
  // Compute hash of Core application
  // ===============================================================================================

  #ifdef WIN32
    QFile coreApp(OpenFlipper::Options::applicationDirStr() + QDir::separator() + "bin" + QDir::separator() + "OpenFlipper.exe");
  #else
    QFile coreApp(OpenFlipper::Options::applicationDirStr() + QDir::separator() + "bin" + QDir::separator() + "OpenFlipper");
  #endif

  if ( ! coreApp.exists() ) {
    std::cerr << "Error finding core application for security check! : " << coreApp.fileName().toStdString() << std::endl;
    return false;
  }

  coreApp.open(QIODevice::ReadOnly);
  QCryptographicHash sha1sumCore( QCryptographicHash::Sha1 );
  sha1sumCore.addData(coreApp.readAll() );
  coreApp.close();

  QString coreHash = QString(sha1sumCore.result().toHex());
  

  // ===============================================================================================
  // Compute hash of Plugin application
  // ===============================================================================================

  std::cerr << "Filename of Plugin is: " << pluginFileName().toStdString() << std::endl;

  #ifdef WIN32
    QFile pluginFile(OpenFlipper::Options::pluginDirStr() + QDir::separator() + pluginFileName() + ".dll");
  #else
    QFile pluginFile(OpenFlipper::Options::pluginDirStr() + QDir::separator() + pluginFileName() + ".so");
  #endif

  if ( ! pluginFile.exists() ) {
    std::cerr << "Error finding plugin file for security check!" << std::endl;
    return false;
  }

  pluginFile.open(QIODevice::ReadOnly);
  QCryptographicHash sha1sumPlugin( QCryptographicHash::Sha1 );
  sha1sumPlugin.addData(pluginFile.readAll());
  pluginFile.close();

  QString pluginHash = QString(sha1sumPlugin.result().toHex());

  std::cerr << "Core Hash : " << coreHash.toStdString() << std::endl;
  std::cerr << "Plugin Hash : " << pluginHash.toStdString() << std::endl;

  QString mac;

  // Get all Network Interfaces
  QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
  foreach ( QNetworkInterface interface, interfaces ) {
    std::cerr << "Name : " << interface.name().toStdString() << std::endl;
    std::cerr << "Found Interface : " << interface.hardwareAddress().toStdString() << std::endl; 
    mac = mac + interface.hardwareAddress().remove(":");
  }

  QString macHash = QCryptographicHash::hash ( mac.toAscii()  , QCryptographicHash::Sha1 ).toHex();

  std::cerr << "macHash is: " << macHash.toStdString()  << std::endl;

  std::cerr << "CPUID Supported : " << CpuIDSupported() << std::endl;
  std::cerr << "GenuineIntel    : " << GenuineIntel() << std::endl;

  
  QString saltPre;
  ADD_SALT_PRE(saltPre);

  QString saltPost;
  ADD_SALT_POST(saltPost);

  QString keyClear = coreHash + saltPre + pluginHash + saltPost + macHash;
  std::cerr << "keyClear is: " << keyClear.toStdString() << std::endl;

  QString keyHash = QCryptographicHash::hash ( keyClear.toAscii()  , QCryptographicHash::Sha1 ).toHex();
  std::cerr << "key is: " << keyHash.toStdString() << std::endl;

  if ( authenticated_ ) 
    std::cerr << "Authentication succcessfull" << std::endl;
  else {
    QString text = "License check for plugin has failed.\n";
    text += "Please get a valid License!\n";
    text += "Send the following Information to contact@openflipper.org:\n";
    text += pluginFileName() +"\n";
    text += coreHash +"\n";
    text += pluginHash +"\n";
    text += macHash +"\n";
    QMessageBox::warning ( 0, "Plugin License check failed",  text );
    std::cerr << "Authentication failed" << std::endl;
    authenticated_ = false;
  }

//   authenticated_ = true;
  return authenticated_;
}

bool LicenseManager::authenticated() {
  return authenticated_;
}

void LicenseManager::connectNotify ( const char * /*signal*/ ) {
  std::cerr << "connectNotify Security Interface" << std::endl;
  if ( !authenticated() ) {
    blockSignals(true);
    std::cerr << "Connects failed due to blocked Plugin" << std::endl;
  }

  disconnect();
}

QString LicenseManager::pluginFileName() {
  return QString("unset");
}

