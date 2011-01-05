/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2010 by Computer Graphics Group, RWTH Aachen       *
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

#include <OpenFlipper/LicenseManager/LicenseManager.hh>
#include <OpenFlipper/common/GlobalOptions.hh>
#include <QFile>
#include <QCryptographicHash>
#include <QNetworkInterface>

/*
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
*/


LicenseManager::~LicenseManager() 
{ 
  exit(0); 
}

LicenseManager::LicenseManager()
{
  authenticated_ = false;
  
  // On startup, block all signals by default until the plugin is authenticated!
  QObject::blockSignals( true );
}

// Override default block signals. Transparent if authenticated, otherwise
// the function will always block the signals automatically
void LicenseManager::blockSignals( bool _state) {

  if ( !authenticated() ) {
    QObject::blockSignals( true );
  } else {
    QObject::blockSignals(_state);
  }

}

// Plugin authentication function.
bool LicenseManager::authenticate() {

  // Construct license string (will be cleaned up if license valid)
  authstring_ = "==\n";
  authstring_ += "PluginName: " + pluginFileName() + "\n";
  
  // ===============================================================================================
  // Compute hash value of Core application binary
  // ===============================================================================================

  #ifdef WIN32
    QFile coreApp(OpenFlipper::Options::applicationDirStr() + QDir::separator() + "OpenFlipper.exe");
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
  // Compute hash of Plugin binary
  // ===============================================================================================

  #ifdef WIN32
    QFile pluginFile(OpenFlipper::Options::pluginDirStr() + QDir::separator() + pluginFileName() + ".dll");
  #else
    QFile pluginFile(OpenFlipper::Options::pluginDirStr() + QDir::separator() + "lib" + pluginFileName() + ".so");
  #endif

  if ( ! pluginFile.exists() ) {
    std::cerr << "Error finding plugin file for security check!" << std::endl;
    std::cerr << "Path: " << pluginFile.fileName().toStdString() << std::endl;
    return false;
  }

  pluginFile.open(QIODevice::ReadOnly);
  QCryptographicHash sha1sumPlugin( QCryptographicHash::Sha1 );
  sha1sumPlugin.addData(pluginFile.readAll());
  pluginFile.close();

  QString pluginHash = QString(sha1sumPlugin.result().toHex());

  // ===============================================================================================
  // Compute hash of network interfaces
  // ===============================================================================================  

  QString mac;

  // Get all Network Interfaces
  QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
  foreach ( QNetworkInterface netInterface, interfaces ) {
    mac = mac + netInterface.hardwareAddress().remove(":");
  }

  QString macHash = QCryptographicHash::hash ( mac.toAscii()  , QCryptographicHash::Sha1 ).toHex();

//   std::cerr << "CPUID Supported : " << CpuIDSupported() << std::endl;
//   std::cerr << "GenuineIntel    : " << GenuineIntel() << std::endl;

  QString saltPre;
  ADD_SALT_PRE(saltPre);
  
  QString saltPost;
  ADD_SALT_POST(saltPost);

  QString licenseFileName = OpenFlipper::Options::licenseDirStr() + QDir::separator() + pluginFileName() + ".lic";
  QFile file( licenseFileName );

  if (!file.open(QIODevice::ReadOnly|QIODevice::Text)) {
    std::cerr << "Unable to find license File " <<  licenseFileName.toStdString() << std::endl;
  } else {
    QString licenseContents = file.readAll();
    QStringList elements = licenseContents.split('\n');

    for ( int i = 0 ; i < elements.size(); ++i )
      elements[i] = elements[i].simplified();

    if ( elements.size() != 6 ) {
      QString sizeMismatchMessage = "The license file for plugin \"" + name() + "\" is invalid!";
      QMessageBox::critical(0,tr("License file size invalid"),sizeMismatchMessage );
    } else {

      // Check signature of license file
      QString license = saltPre + elements[0] + elements[1] + elements[2] + elements[3] + elements[4] + saltPost;
      QString licenseHash = QCryptographicHash::hash ( license.toAscii()  , QCryptographicHash::Sha1 ).toHex();
      
      QDate currentDate = QDate::currentDate();
      QDate expiryDate  = QDate::fromString(elements[4],Qt::ISODate);

      if ( licenseHash !=  elements[5] ) {
        authstring_ += tr("License Error: The license file signature for plugin \"") + name() + tr("\" is invalid!\n\n");
      } else  if ( elements[0] != pluginFileName() ) {
        authstring_ += tr("License Error: The license file contains plugin name\"") + elements[0] + tr("\" but this is plugin \"") + name() + "\"!\n\n";
      } else if ( elements[1] != coreHash ) {
        authstring_ += tr("License Error: The license file for plugin \"") + name() + tr("\" is invalid for the currently running OpenFlipper Core!\n\n");
      } else if ( elements[2] != pluginHash ) {
        authstring_ += tr("License Error: The plugin \"") + name() + tr("\" is a different version than specified in license file!\n\n");
      } else if ( elements[3] != macHash ) {
        authstring_ += "License Error: The plugin \"" + name() + "\" is not allowed to run on the current system (Changed Hardware?)!\n\n";
      } else if ( currentDate > expiryDate ) {
        authstring_ += tr("License Error: The license for plugin \"") + name() + tr("\" has expired on ") + elements[1] + "!\n\n";
      } else {
        authenticated_ = true;
      }

      // Clean it on success
      if (  authenticated_ ) 
        authstring_ = "";
      
    }
  }

  if ( authenticated_ ) {
    blockSignals(false);
  } else {
    authstring_ += tr("Message: License check for plugin failed.\n");
    authstring_ += tr("Message: Please get a valid License!\n");
    authstring_ += tr("Message: Send the following Information to \n");
    authstring_ += tr("Contact mail: ") + CONTACTMAIL + "\n\n";
    authstring_ += pluginFileName() +"\n";
    authstring_ += coreHash +"\n";
    authstring_ += pluginHash +"\n";
    authstring_ += macHash +"\n";

    QString keyRequest = saltPre + pluginFileName() + coreHash+pluginHash+macHash +saltPost;
    QString requestSig = QCryptographicHash::hash ( keyRequest.toAscii()  , QCryptographicHash::Sha1 ).toHex();
    authstring_ += requestSig + "\n";

    authenticated_ = false;
  }

  return authenticated_;
}

QString LicenseManager::licenseError() {
  return authstring_;
}

bool LicenseManager::authenticated() {
  // Function to check if the plugin is authenticated
  return authenticated_;
}

void LicenseManager::connectNotify ( const char * /*signal*/ ) {

  // if the plugin is not authenticated and something wants to connect, we block all signals and force a direct disconnect
  // here, rendering all signal/slot connections useless.
  if ( !authenticated() ) {
    blockSignals(true);
    disconnect();
  }
  
}

QString LicenseManager::pluginFileName() {
  // FileName of the plugin. has to be set via the salt file
  QString pluginFileName;
  ADD_PLUGIN_FILENAME(pluginFileName);
  return pluginFileName;
}

