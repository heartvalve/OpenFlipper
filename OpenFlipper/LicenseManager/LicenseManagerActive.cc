/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2014 by Computer Graphics Group, RWTH Aachen       *
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



/*
License File format:

0:    Signature     over all other entries
1:    Expiry date
2:    Plugin filename
3:    coreHash
4:    pluginHash
5:    cpuHash
6:    windowsProductId (Windows only otherwise filled with "-" before hashing)
7..?: mac Address hashes

*/


// Windows required to get network address infos
#ifdef WIN32
  #include <winsock2.h>
  #include <iphlpapi.h>
  #pragma comment(lib, "IPHLPAPI.lib")
#endif

#ifdef ARCH_DARWIN
  #include <sys/types.h>
  #include <sys/sysctl.h>
#endif


#include <OpenFlipper/LicenseManager/LicenseManagerActive.hh>
#include <OpenFlipper/common/GlobalOptions.hh>
#include <QFile>
#include <QCryptographicHash>
#include <QNetworkInterface>




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
  // Read License file, if exists
  // ===============================================================================================
  QString saltPre;
  ADD_SALT_PRE(saltPre);

  QString saltPost;
  ADD_SALT_POST(saltPost);

  QString licenseFileName = OpenFlipper::Options::licenseDirStr() + QDir::separator() + pluginFileName() + ".lic";
  QFile file( licenseFileName );
  QStringList elements; //has no elements, if file is invalid or was not found
  bool signatureOk = false;
  QByteArray (QString::*codingfun)()const = &QString::toUtf8;

  if (file.open(QIODevice::ReadOnly|QIODevice::Text))
  {
    QString licenseContents = file.readAll();
    elements = licenseContents.split('\n',QString::SkipEmptyParts);
    bool fileOk = !elements.empty() && elements[0] != "ERROR";

    if (fileOk)
    {
      // simplify license file entries
      for ( int i = 0 ; i < elements.size(); ++i )
        elements[i] = elements[i].simplified();

      // Check the signature of the file (excluding first element as this is the signature itself)
      QString license = saltPre;
      for ( int i = 1 ; i < elements.size(); ++i )
        license += elements[i];
      license += saltPost;
      QString licenseHash = QCryptographicHash::hash ( license.toUtf8()  , QCryptographicHash::Sha1 ).toHex();
      signatureOk = licenseHash == elements[0];

      if (signatureOk)
        codingfun = &QString::toUtf8;
      else
      {
        licenseHash = QCryptographicHash::hash ( license.toLatin1()  , QCryptographicHash::Sha1 ).toHex();
        signatureOk = licenseHash == elements[0];
        if (signatureOk)
          codingfun = &QString::toLatin1;
      }

    }
    else
      elements = QStringList();
  }

  // ===============================================================================================
  // Compute hash value of Core application binary
  // ===============================================================================================

  #ifdef WIN32
    QFile coreApp(OpenFlipper::Options::applicationDirStr() + QDir::separator() + "OpenFlipper.exe");
  #elif defined ARCH_DARWIN
    QFile coreApp(OpenFlipper::Options::applicationDirStr() + QDir::separator() + ".." + 
                                                              QDir::separator() + "MacOS"+
							      QDir::separator() + "OpenFlipper");
  #else 
    QFile coreApp(OpenFlipper::Options::applicationDirStr() + QDir::separator() + "bin" + QDir::separator() + TOSTRING(PRODUCT_STRING));
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
  #elif defined ARCH_DARWIN
    QFile pluginFile(OpenFlipper::Options::pluginDirStr() + QDir::separator() + "lib" + pluginFileName() + ".so");
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

  QStringList macHashes;

#ifdef WIN32

  #define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
  #define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

  // Pointer for iterating over adapters
  PIP_ADAPTER_ADDRESSES pAddresses = NULL;

  // Length of the buffer to get information
  ULONG outBufLen = 0;

  // Allocate enough for one info struct
  outBufLen = sizeof (IP_ADAPTER_ADDRESSES);
  pAddresses = (IP_ADAPTER_ADDRESSES *) MALLOC(outBufLen);

  // default to unspecified address family ( get all interfaces .. 4 and 6)
  ULONG family = AF_UNSPEC;

  // Set the flags to pass to GetAdaptersAddresses
  ULONG flags = GAA_FLAG_INCLUDE_PREFIX;

  // Make an initial call to GetAdaptersAddresses to get the 
  // size needed into the outBufLen variable
  if (GetAdaptersAddresses(family, flags, NULL, pAddresses, &outBufLen) == ERROR_BUFFER_OVERFLOW) {
      FREE(pAddresses);
      pAddresses = (IP_ADAPTER_ADDRESSES *) MALLOC(outBufLen);
  }

  // If we allocated the required memory
  if (pAddresses != NULL) {
    
    // Get the required info
    DWORD dwRetVal = GetAdaptersAddresses(family, flags, NULL, pAddresses, &outBufLen);

    if (dwRetVal == NO_ERROR) {
        
      // pointer to iterate over all available structs .. initialize to first one
      PIP_ADAPTER_ADDRESSES pCurrAddresses = pAddresses;

      while (pCurrAddresses) {

        // Check if this device contains a mac
        if (pCurrAddresses->PhysicalAddressLength != 0) {
          QString currentMac = "";
          
          for (uint i = 0; i < pCurrAddresses->PhysicalAddressLength; i++) {
            
            currentMac += QString("%1").arg( (int) pCurrAddresses->PhysicalAddress[i] , 2 ,16,QChar('0'));
            
            if (i != (pCurrAddresses->PhysicalAddressLength - 1))
              currentMac +=":";
          }

          // Ignore non ethernet macs with more than 5 blocks
          // Only check ethernet and wireless interfaces
          if ( (currentMac.count(":") == 5) && 
               ( pCurrAddresses->IfType == IF_TYPE_IEEE80211 || pCurrAddresses->IfType == IF_TYPE_ETHERNET_CSMACD )  ) {
                // Cleanup and remember mac adress
                currentMac = (currentMac.*codingfun)().toUpper();
                currentMac = currentMac.remove(":");
                macHashes.push_back(currentMac);
          }

        }

        // Next interface
        pCurrAddresses = pCurrAddresses->Next;
      }
      
    }

  }

  FREE(pAddresses);

#else

  // Get all Network Interfaces
  QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
  foreach ( QNetworkInterface netInterface, interfaces ) {
    
    // Ignore loopback interfaces
    if ( ( netInterface.flags() & QNetworkInterface::IsLoopBack ) ) {
      continue;
    }
    
    // Ignore non ethernet macs
    if ( netInterface.hardwareAddress().count(":") != 5 ) {
      continue;
    }

    // Cleanup mac adress
    QString currentMac = (netInterface.hardwareAddress().*codingfun)().toUpper();
    currentMac = currentMac.remove(":");
    
    macHashes.push_back(currentMac);
  }

#endif


  // cleanup the list from duplicates (virtual interfaces on windows connected to an existing device ... )
  macHashes.removeDuplicates();

  // generate hashes
  for (int i = 0 ; i < macHashes.size(); ++i ) 
    macHashes[i] = QCryptographicHash::hash ( (macHashes[i].*codingfun)() , QCryptographicHash::Sha1 ).toHex();

  // ===============================================================================================
  // Compute hash of processor information
  // ===============================================================================================  

  QString processor = "Unknown";

  #ifdef WIN32
    QSettings registryCPU("HKEY_LOCAL_MACHINE\\HARDWARE\\DESCRIPTION\\System\\CentralProcessor", QSettings::NativeFormat);

    QStringList cpus = registryCPU.childGroups(); 
    if ( cpus.size() != 0 ) { 
      processor = registryCPU.value( cpus[0]+"/ProcessorNameString", "Unknown" ).toString();
    } 

  #elif defined ARCH_DARWIN 

    size_t lenCPU;
    char *pCPU;
    
    // First call to get required size
    sysctlbyname("machdep.cpu.brand_string", NULL, &lenCPU, NULL, 0);
    
    // allocate
    pCPU = (char * )malloc(lenCPU);
    
    // Second call to get data
    sysctlbyname("machdep.cpu.brand_string", pCPU, &lenCPU, NULL, 0);
    
    // Output
    processor = QString(pCPU);
    
    // free memory
    free(pCPU);

  #else
    QFile cpuinfo("/proc/cpuinfo");
    if ( cpuinfo.exists() ) {   
      cpuinfo.open(QFile::ReadOnly);
      QTextStream stream(&cpuinfo);
      QStringList splitted = stream.readAll().split("\n",QString::SkipEmptyParts);
      
      int position = splitted.indexOf ( QRegExp("^model name.*") );
      if ( position != -1 ){
        QString cpuModel = splitted[position].section(':', -1).simplified();
        processor = cpuModel;
      } 
    }
  #endif

  QString cpuHash = QCryptographicHash::hash ( (processor.*codingfun)()  , QCryptographicHash::Sha1 ).toHex();

  // ===============================================================================================
  // Get windows product id
  // =============================================================================================== 
  
  

  #ifdef WIN32
    QSettings registryProduct("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", QSettings::NativeFormat);
    QString productId = registryProduct.value( "ProductId", "Unknown" ).toString();
  #else
    QString productId = "-";
  #endif
  
  QString productHash = QCryptographicHash::hash ( (productId.*codingfun)()  , QCryptographicHash::Sha1 ).toHex();

  // ===============================================================================================
  // Check License or generate request
  // =============================================================================================== 
  if (!elements.empty()) //valid file was found
  {
    // Check expiry date
    QDate currentDate = QDate::currentDate();
    QDate expiryDate  = QDate::fromString(elements[1],Qt::ISODate);
    bool expired = (currentDate > expiryDate);
    // Get number of available mac adresses
    QStringList licensedMacs;
    for ( int i = 7 ; i < elements.size(); ++i ) {
      licensedMacs.push_back(elements[i]);
    }

    bool macFound = false;
    for ( int i = 0; i < macHashes.size(); ++i ) {
      if ( licensedMacs.contains(macHashes[i]) )
        macFound = true;
    }

    if ( !signatureOk ) {
      authstring_ += tr("License Error: The license file signature for Plugin \"") + name() + tr("\" is invalid!\n\n");
    } else if ( expired ) {
      authstring_ += tr("License Error: The license for plugin \"") + name() + tr("\" has expired on ") + elements[1] + "!\n\n";       
    } else if ( elements[2] != pluginFileName() ) {
      authstring_ += tr("License Error: The license file contains plugin name\"") + elements[2] + tr("\" but this is plugin \"") + name() + "\"!\n\n";
    } else if ( elements[3] != coreHash ) {
      authstring_ += tr("License Error: The license file for plugin \"") + name() + tr("\" is invalid for the currently running OpenFlipper Core!\n\n");
    } else if ( elements[4] != pluginHash ) {
      authstring_ += tr("License Error: The plugin \"") + name() + tr("\" is a different version than specified in license file!\n\n");
    } else if ( elements[5] != cpuHash ) {
      authstring_ += "License Error: The plugin \"" + name() + "\" is not allowed to run on the current system (Changed CPU?)!\n\n";       
    } else if ( elements[6] != productHash ) {
      authstring_ += "License Error: The plugin \"" + name() + "\" is not allowed to run on the current system (Changed OS?)!\n\n";       
    } else if ( !macFound ) {
      authstring_ += "License Error: The plugin \"" + name() + "\" is not allowed to run on the current system (Changed Network?)!\n\n";
    } else {
      authenticated_ = true;
    }
    
    // Clean it on success
    if (  authenticated_ ) 
      authstring_ = "";

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
    authstring_ += cpuHash +"\n";
    authstring_ += productHash +"\n";
    
    for ( int i = 0 ; i < macHashes.size(); ++i )  
      authstring_ += macHashes[i] +"\n";

    QString keyRequest = saltPre + pluginFileName() + coreHash + pluginHash + cpuHash + productHash + macHashes.join("") +  saltPost;
    QString requestSig = QCryptographicHash::hash ( keyRequest.toUtf8()  , QCryptographicHash::Sha1 ).toHex();
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

