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




//=============================================================================
//
//  CLASS Core - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================

#include "CoreWidget.hh"
#include <OpenFlipper/common/FileTypes.hh>

#include <QGLFormat>

#include <OpenFlipper/common/GlobalOptions.hh>

#ifndef WIN32
  #ifndef ARCH_DARWIN
    #include <malloc.h>
    #include <unistd.h>
  #endif
#endif

#ifdef ARCH_DARWIN
  #include <sys/types.h>
  #include <sys/sysctl.h>
#endif

//== IMPLEMENTATION ==========================================================

void CoreWidget::addAboutInfo(QString _text, QString _tabName ) {
  additionalAboutTabs_[_tabName] = _text;
}

void CoreWidget::showAboutWidget( ) {

  if ( OpenFlipper::Options::nogui() )
    return;
  
  if ( aboutWidget_ == 0 ) {
    aboutWidget_ = new AboutWidget( this );


    // Add all additional tabs
    QMap<QString, QString>::const_iterator it = additionalAboutTabs_.constBegin();
    while (it != additionalAboutTabs_.constEnd()) {
      QTextEdit* tmp =  new QTextEdit();
      tmp->insertHtml( it.value() );

      aboutWidget_->About->addTab( tmp, it.key()  );
      ++it;
    }

  } else {
    aboutWidget_->OpenFlipperAbout->clear();
  }
  
  QFont standardFont = aboutWidget_->OpenFlipperAbout->currentFont();
  QFont boldFont = standardFont;
  boldFont.setBold(true);

  aboutWidget_->OpenFlipperAbout->setCurrentFont(boldFont);
  aboutWidget_->OpenFlipperAbout->append(tr("%1 Core Version: %2", "PRODUCT_NAME Core Version:").arg(TOSTRING(PRODUCT_NAME)).arg(OpenFlipper::Options::coreVersion()) ) ;
  aboutWidget_->OpenFlipperAbout->setCurrentFont(standardFont);

  // =====================================================================================
  // Directory info
  // =====================================================================================
  aboutWidget_->OpenFlipperAbout->append("\n");
  aboutWidget_->OpenFlipperAbout->setCurrentFont(boldFont);
  aboutWidget_->OpenFlipperAbout->append(tr("%1 Directories:").arg(TOSTRING(PRODUCT_NAME)));
  aboutWidget_->OpenFlipperAbout->setCurrentFont(standardFont);
  
  // Get the dataDir
  QDir tempDir = QDir(OpenFlipper::Options::applicationDir());
  #ifdef OPENFLIPPER_DATADIR
    tempDir.cd(OPENFLIPPER_DATADIR);
  #endif
  aboutWidget_->OpenFlipperAbout->append("Data:\t " + tempDir.absolutePath() );
  aboutWidget_->OpenFlipperAbout->append("Shaders:\t " + OpenFlipper::Options::shaderDirStr() );
  aboutWidget_->OpenFlipperAbout->append("Textures:\t " + OpenFlipper::Options::textureDirStr() );
  aboutWidget_->OpenFlipperAbout->append("Scripts:\t " + OpenFlipper::Options::scriptDirStr() );
  aboutWidget_->OpenFlipperAbout->append("Icons:\t " + OpenFlipper::Options::iconDirStr() );
  aboutWidget_->OpenFlipperAbout->append("Fonts:\t" + OpenFlipper::Options::fontsDirStr() );
  aboutWidget_->OpenFlipperAbout->append("Help:\t" + OpenFlipper::Options::helpDirStr() );
  aboutWidget_->OpenFlipperAbout->append("\n");

  // =====================================================================================
  // Config files
  // =====================================================================================
  aboutWidget_->OpenFlipperAbout->append("\n");
  aboutWidget_->OpenFlipperAbout->setCurrentFont(boldFont);
  aboutWidget_->OpenFlipperAbout->append(tr("%1 configuration files:").arg(TOSTRING(PRODUCT_NAME)));
  aboutWidget_->OpenFlipperAbout->setCurrentFont(standardFont);

  aboutWidget_->OpenFlipperAbout->append(tr("ConfigDir:\t\t\t") +   OpenFlipper::Options::configDirStr() );
  aboutWidget_->OpenFlipperAbout->append(tr("Window states:\t\t") + OpenFlipper::Options::configDirStr()  + OpenFlipper::Options::dirSeparator() + "WindowStates.dat");

  aboutWidget_->OpenFlipperAbout->append(tr("Main option file:\t\t") + OpenFlipperSettings().fileName());
  aboutWidget_->OpenFlipperAbout->append(tr("Additional option files:"));
  for ( int i = 0 ; i < OpenFlipper::Options::optionFiles().size() ; ++i)
    aboutWidget_->OpenFlipperAbout->append(OpenFlipper::Options::optionFiles()[i]);



  
  // =====================================================================================
  // Memory infos
  // =====================================================================================
  aboutWidget_->OpenFlipperAbout->append("\n");
  aboutWidget_->OpenFlipperAbout->setCurrentFont(boldFont);
  aboutWidget_->OpenFlipperAbout->append(tr("Memory Information:"));    
  aboutWidget_->OpenFlipperAbout->setCurrentFont(standardFont);
 
 
  #ifdef WIN32 

    // Define memory structure
    MEMORYSTATUSEX ms;
    // Set the size ( required according to spec ... why???? )
    ms.dwLength = sizeof (ms);

    // Get the info
    GlobalMemoryStatusEx(&ms);

    // Output to widget
    aboutWidget_->OpenFlipperAbout->append(tr("Physical Memory:\t")+ QString::number(ms.ullAvailPhys/1024/1024) + "MB/"+ 
                                                                     QString::number(ms.ullTotalPhys/1024/1024) + "MB used ("+
                                                                     QString::number(ms.dwMemoryLoad) + "%)");   
    aboutWidget_->OpenFlipperAbout->append(tr("Pagefile Memory:\t")+ QString::number(ms.ullAvailPageFile/1024/1024) + "MB/"
                                                                   + QString::number(ms.ullTotalPageFile/1024/1024) + "MB used");   

 
  #elif defined ARCH_DARWIN 
  
  aboutWidget_->OpenFlipperAbout->append(tr("Not available for this platform (MacOS)"));   
    
    
  #else
    unsigned long memory = 0;
    
    struct mallinfo info;
    info = mallinfo();

    // add mmap-allocated memory
    memory += info.hblkhd;
    
    // add sbrk-allocated memory
    memory += info.uordblks;
    
    double mmapAllocated  = double(info.hblkhd  ) / 1024 / 1024;
    double sbrkAllocated  = double(info.uordblks) / 1024 / 1024;
    double totalAllocated = double(memory)        / 1024 / 1024;
    
    double pageSize       = double(getpagesize()) /1024 ;
    double availablePages = double( sysconf (_SC_AVPHYS_PAGES) );
    double freeMem        = availablePages * pageSize / 1024;
    
    aboutWidget_->OpenFlipperAbout->append(tr("Total Memory allocated:\t ") + QString::number(totalAllocated,'f' ,2 ) + tr("MB ")
                                           + tr("( mmap: ") + QString::number(mmapAllocated ,'f' ,2 ) + tr("MB") 
                                           + tr(", sbrk: ") + QString::number(sbrkAllocated ,'f' ,2 ) + tr("MB )") );
    
    aboutWidget_->OpenFlipperAbout->append(tr("Free Memory:\t\t ") + QString::number(freeMem,'f' ,2 )  + tr("MB ")
                                           + tr("(") + QString::number(availablePages,'f' ,0 ) + tr(" pages of ") 
                                           + QString::number(pageSize,'f' ,2 ) + tr("KB size)"));
    
  #endif
  
  // =====================================================================================
  // Memory infos
  // =====================================================================================
  aboutWidget_->OpenFlipperAbout->append("\n");
  aboutWidget_->OpenFlipperAbout->setCurrentFont(boldFont);
  aboutWidget_->OpenFlipperAbout->append(tr("CPU Information:"));    
  aboutWidget_->OpenFlipperAbout->setCurrentFont(standardFont);  
  
  #ifdef WIN32 
    
    QSettings registryCPU("HKEY_LOCAL_MACHINE\\HARDWARE\\DESCRIPTION\\System\\CentralProcessor", QSettings::NativeFormat);

    QStringList cpus = registryCPU.childGroups(); 
    if ( cpus.size() != 0 ) { 

      aboutWidget_->OpenFlipperAbout->append(tr("CPU vendor:\t\t ") + 
                                            registryCPU.value( cpus[0]+"/VendorIdentifier", "Unknown" ).toString() );
      aboutWidget_->OpenFlipperAbout->append(tr("CPU model:\t\t ") + 
                                            registryCPU.value( cpus[0]+"/ProcessorNameString", "Unknown" ).toString() );
      aboutWidget_->OpenFlipperAbout->append(tr("CPU identifier:\t\t ") + 
                                            registryCPU.value( cpus[0]+"/Identifier", "Unknown" ).toString() );
      aboutWidget_->OpenFlipperAbout->append(tr("CPU Speed:\t\t ") + 
                                            registryCPU.value( cpus[0]+"/~MHz", "Unknown" ).toString()+ " MHz" );

      aboutWidget_->OpenFlipperAbout->append("CPU Cores:\t\t " + QString::number(cpus.size())); 

    } else {
      aboutWidget_->OpenFlipperAbout->append(tr("Unable to retrieve CPU information"));
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
    aboutWidget_->OpenFlipperAbout->append(tr("CPU Brand:\t\t ") + QString(pCPU) );
    
    // free memory
    free(pCPU);
    
    int physicalCPUS = 0;
    
    // Get data
    lenCPU = sizeof(int);
    sysctlbyname("hw.physicalcpu", &physicalCPUS, &lenCPU , NULL, 0);
    
    // Output
    aboutWidget_->OpenFlipperAbout->append(tr("Physical Cores:\t\t ") + QString::number(physicalCPUS) );

    int logicalCPUS = 0;
    
    // Get data
    lenCPU = sizeof(int);
    sysctlbyname("hw.logicalcpu", &logicalCPUS, &lenCPU, NULL, 0);
    
    // Output
    aboutWidget_->OpenFlipperAbout->append(tr("LogicalCores:\t\t ") + QString::number(logicalCPUS) );
    
  #else
    QFile cpuinfo("/proc/cpuinfo");
    if (! cpuinfo.exists() )
      aboutWidget_->OpenFlipperAbout->append(tr("Unable to retrieve CPU information"));
    else {
      
      cpuinfo.open(QFile::ReadOnly);
      QTextStream stream(&cpuinfo);
      QStringList splitted = stream.readAll().split("\n",QString::SkipEmptyParts);
    
      int position = splitted.indexOf ( QRegExp("^vendor_id.*") );
      if ( position != -1 ){
        QString cpuVendor = splitted[position].section(':', -1).simplified();
        aboutWidget_->OpenFlipperAbout->append(tr("CPU vendor:\t\t ") + cpuVendor );
      } else {
        aboutWidget_->OpenFlipperAbout->append(tr("CPU vendor:\t\t vendor specification not found")); 
      }
      
      position = splitted.indexOf ( QRegExp("^model name.*") );
      if ( position != -1 ){
        QString cpuModel = splitted[position].section(':', -1).simplified();
        aboutWidget_->OpenFlipperAbout->append(tr("CPU model:\t\t ") + cpuModel );
      } else {
        aboutWidget_->OpenFlipperAbout->append(tr("CPU model:\t\t Model specification not found")); 
      }
      
      position = splitted.indexOf ( QRegExp("^cpu cores.*") );
      if ( position != -1 ){
        QString cpuCoresPhysical = splitted[position].section(':', -1).simplified();
        aboutWidget_->OpenFlipperAbout->append(tr("Physical CPU cores:\t\t ") + cpuCoresPhysical );
      } else {
        aboutWidget_->OpenFlipperAbout->append(tr("Physical CPU cores:\t\t CPU Core specification not found")); 
      }
      
      position = splitted.indexOf ( QRegExp("^siblings.*") );
      if ( position != -1 ){
        QString cpuCoresLogical = splitted[position].section(':', -1).simplified();
        aboutWidget_->OpenFlipperAbout->append(tr("Logical CPU cores:\t\t ") + cpuCoresLogical );
      } else {
        aboutWidget_->OpenFlipperAbout->append(tr("Logical CPU cores:\t\t CPU Core specification not found")); 
      }    
      
      position = splitted.indexOf ( QRegExp("^flags.*") );
      if ( position != -1 ){
        QString cpuFlags = splitted[position].section(':', -1).simplified();
        aboutWidget_->OpenFlipperAbout->append( tr("CPU capabilities:") );
        aboutWidget_->OpenFlipperAbout->append( cpuFlags );
      } else {
        aboutWidget_->OpenFlipperAbout->append(tr("CPU capabilities:\t\t CPU flag specification not found")); 
      }    
      
      #if Q_BYTE_ORDER == Q_BIG_ENDIAN
        aboutWidget_->OpenFlipperAbout->append(tr("System is Big Endian"));
      #endif

      #if Q_BYTE_ORDER == Q_LITTLE_ENDIAN
        aboutWidget_->OpenFlipperAbout->append(tr("System is Little Endian"));
      #endif
      
    }
  
  #endif
  
  // =====================================================================================
  // OS info
  // =====================================================================================    
  aboutWidget_->OpenFlipperAbout->append("\n");
  aboutWidget_->OpenFlipperAbout->setCurrentFont(boldFont);
  aboutWidget_->OpenFlipperAbout->append(tr("Operating System Info:"));
  aboutWidget_->OpenFlipperAbout->setCurrentFont(standardFont);

  #ifdef WIN32 
     QSettings registryOS("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", QSettings::NativeFormat);
     aboutWidget_->OpenFlipperAbout->append(tr("Product Name:\t\t ") + 
                                               registryOS.value( "ProductName", "Unknown" ).toString() );
     aboutWidget_->OpenFlipperAbout->append(tr("Current Version:\t\t ") + 
                                               registryOS.value( "CurrentVersion", "Unknown" ).toString() );
  #elif defined ARCH_DARWIN 
    
    int mib[2];
    size_t len;
    char *p;
    
    mib[0] = CTL_KERN;
    mib[1] = KERN_VERSION;
    
    sysctl(mib, 2, NULL, &len, NULL, 0);
    p = (char * )malloc(len);
    sysctl(mib, 2, p, &len, NULL, 0);
    
    aboutWidget_->OpenFlipperAbout->append(tr("OS Version:\t\t ") + QString(p) );
    
    free(p);

  #else
    QFile versionInfo("/proc/version");
    if (! versionInfo.exists() )
      aboutWidget_->OpenFlipperAbout->append(tr("Unable to retrieve Kernel information"));
    else {
      versionInfo.open(QFile::ReadOnly);
      QTextStream versionStream(&versionInfo);
      aboutWidget_->OpenFlipperAbout->append(tr("Kernel Version:\t\t ") + versionStream.readAll().simplified()); 
      
      QString program = "/usr/bin/lsb_release";
      
      QFile lsb(program);
      if ( lsb.exists() ) {
        QStringList arguments;
        arguments << "-a";
        
        QProcess myProcess;
        myProcess.start(program, arguments);
        
        if ( myProcess.waitForFinished ( 4000 ) ) {
          QStringList outputLSB = QString(myProcess.readAllStandardOutput()).split('\n');
          
          
          int position = outputLSB.indexOf ( QRegExp("^Distributor ID.*") );
          if ( position != -1 ){
            QString distributorID = outputLSB[position].section(':', -1).simplified();
            aboutWidget_->OpenFlipperAbout->append(tr("Distributor ID:\t\t ") + distributorID );
          } else {
            aboutWidget_->OpenFlipperAbout->append(tr("Distributor ID:\t\t Unknown")); 
          }
          
          position = outputLSB.indexOf ( QRegExp("^Description.*") );
          if ( position != -1 ){
            QString description = outputLSB[position].section(':', -1).simplified();
            aboutWidget_->OpenFlipperAbout->append(tr("Description:\t\t ") + description );
          } else {
            aboutWidget_->OpenFlipperAbout->append(tr("Description:\t\t Unknown")); 
          }          
          
          position = outputLSB.indexOf ( QRegExp("^Release.*") );
          if ( position != -1 ){
            QString release = outputLSB[position].section(':', -1).simplified();
            aboutWidget_->OpenFlipperAbout->append(tr("Release number:\t\t ") + release );
          } else {
            aboutWidget_->OpenFlipperAbout->append(tr("Release number:\t\t Unknown")); 
          }    
          
          position = outputLSB.indexOf ( QRegExp("^Codename.*") );
          if ( position != -1 ){
            QString codename = outputLSB[position].section(':', -1).simplified();
            aboutWidget_->OpenFlipperAbout->append(tr("Codename:\t\t ") + codename );
          } else {
            aboutWidget_->OpenFlipperAbout->append(tr("Codename:\t\t Unknown")); 
          }                      
          
        } else {
          aboutWidget_->OpenFlipperAbout->append(tr("Unable to get LSB info")); 
        }
        
      } else {
        aboutWidget_->OpenFlipperAbout->append(tr("No additional information. Unable to get info via LSB.")); 
      }
      
    }
  #endif

  QString systemPathVariable = getenv("PATH");
  aboutWidget_->OpenFlipperAbout->append(tr("System Path:\n ") + systemPathVariable);

#ifndef WIN32
  QString systemLibraryPathVariable = getenv("LD_LIBRARY_PATH");
  aboutWidget_->OpenFlipperAbout->append(tr("LD_LIBRARY_PATH:\n ") + systemLibraryPathVariable);
#endif



  // =====================================================================================
  // OpenGL Renderer/Vendor and version info
  // =====================================================================================    
  aboutWidget_->OpenFlipperAbout->append("\n");
  aboutWidget_->OpenFlipperAbout->setCurrentFont(boldFont);
  aboutWidget_->OpenFlipperAbout->append(tr("OpenGL Specific Info:"));
  aboutWidget_->OpenFlipperAbout->setCurrentFont(standardFont);

  QString vendor = QString((const char*)glGetString(GL_VENDOR));
  aboutWidget_->OpenFlipperAbout->append(tr("Qt reports Vendor:\t\t") + vendor);
  QString renderer = QString((const char*)glGetString(GL_RENDERER));
  aboutWidget_->OpenFlipperAbout->append(tr("Qt reports Renderer:\t\t") + renderer);
  
  QGLFormat::OpenGLVersionFlags flags = QGLFormat::openGLVersionFlags();

  QString openGLQtVersion = tr("Qt reports Version:\t\t");

  if ( flags.testFlag(QGLFormat::OpenGL_Version_4_0) )
    openGLQtVersion += tr("4.0 or higher") ;
  else if (flags.testFlag(QGLFormat::OpenGL_Version_3_3))
    openGLQtVersion += tr("3.3") ;
  else if (flags.testFlag(QGLFormat::OpenGL_Version_3_2))
    openGLQtVersion += tr("3.2") ;
  else if (flags.testFlag(QGLFormat::OpenGL_Version_3_1))
    openGLQtVersion += tr("3.1") ;
  else if (flags.testFlag(QGLFormat::OpenGL_Version_3_0))
    openGLQtVersion += tr("3.0") ;
  else if (flags.testFlag(QGLFormat::OpenGL_Version_3_0))
    openGLQtVersion += tr("3.0") ;
  else if (flags.testFlag(QGLFormat::OpenGL_Version_2_1))
    openGLQtVersion += tr("2.1" );
  else if (flags.testFlag(QGLFormat::OpenGL_Version_2_0))
    openGLQtVersion += tr("2.0" );
  else if (flags.testFlag(QGLFormat::OpenGL_Version_1_5))
    openGLQtVersion += tr("1.5" );
  else if (flags.testFlag(QGLFormat::OpenGL_Version_1_4))
    openGLQtVersion += tr("1.4" );
  else if (flags.testFlag(QGLFormat::OpenGL_Version_1_3))
    openGLQtVersion += tr("1.3" );
  else if (flags.testFlag(QGLFormat::OpenGL_Version_1_2))
    openGLQtVersion += tr("1.2" );
  else if (flags.testFlag(QGLFormat::OpenGL_Version_1_1))
    openGLQtVersion += tr("1.1" );
  else
    openGLQtVersion += tr("Version:\tUNKNOWN!" );

  aboutWidget_->OpenFlipperAbout->append(openGLQtVersion);

  aboutWidget_->OpenFlipperAbout->append("\n");

  const QGLContext* context = QGLContext::currentContext();
  if (context) {
    QString openGLprofile = tr("Qt reports the OpenGL profile:\t");
    QGLFormat::OpenGLContextProfile profile = context->format().profile();
    if (profile == QGLFormat::NoProfile)
      openGLprofile += tr("no profile");
    else if (profile == QGLFormat::CoreProfile)
      openGLprofile += tr("Core profile");
    else if (profile == QGLFormat::CompatibilityProfile)
      openGLprofile += tr("Compatibility profile");
    else
      openGLprofile += tr("unknown profile");

    aboutWidget_->OpenFlipperAbout->append(openGLprofile);
    aboutWidget_->OpenFlipperAbout->append("\n");
  }


  QString openGLVendor = tr("GL reports Vendor:\t\t");
  openGLVendor += QString((const char*)glGetString(GL_VENDOR));
  aboutWidget_->OpenFlipperAbout->append(openGLVendor);

  QString openGLRenderer = tr("GL reports Renderer:\t\t");
  openGLRenderer += QString((const char*)glGetString(GL_RENDERER));
  aboutWidget_->OpenFlipperAbout->append(openGLRenderer);

  QString openGLGLVersion = tr("GL reports Version:\t\t");
  openGLGLVersion += QString((const char*)glGetString(GL_VERSION));
  aboutWidget_->OpenFlipperAbout->append(openGLGLVersion);

  QString openGLShadingLanguageVersion = tr("GL reports Shading Language Version:\t");
  openGLShadingLanguageVersion += QString((const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));
  aboutWidget_->OpenFlipperAbout->append(openGLShadingLanguageVersion);


  // =====================================================================================
  // OpenGL Extensions
  // =====================================================================================
  aboutWidget_->OpenFlipperAbout->append("\n");
  aboutWidget_->OpenFlipperAbout->append(tr("Supported Extensions:"));
  QString glExtensions = QString((const char*)glGetString(GL_EXTENSIONS));
  aboutWidget_->OpenFlipperAbout->append(glExtensions);
  

  // =====================================================================================
  // OpenGL Properties
  // =====================================================================================
  aboutWidget_->OpenFlipperAbout->append("\n");
  aboutWidget_->OpenFlipperAbout->setCurrentFont(boldFont);
  aboutWidget_->OpenFlipperAbout->append(tr("OpenGL Values:"));
  aboutWidget_->OpenFlipperAbout->setCurrentFont(standardFont);
  GLint getBuffer[2];
  glGetIntegerv(GL_MAX_VIEWPORT_DIMS,  &getBuffer[0] );
  aboutWidget_->OpenFlipperAbout->append(tr("Maximal Viewport size(GL_MAX_VIEWPORT_DIMS):\t\t\t\t ") + QString::number(getBuffer[0]) + "x" + QString::number(getBuffer[1]) );

  glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS,  &getBuffer[0] );
  aboutWidget_->OpenFlipperAbout->append(tr("Maximum supported texture image units(GL_MAX_TEXTURE_IMAGE_UNITS):\t") + QString::number(getBuffer[0]) );





  
  // =====================================================================================
  // glu Information
  // =====================================================================================
  aboutWidget_->OpenFlipperAbout->append("\n");
  aboutWidget_->OpenFlipperAbout->setCurrentFont(boldFont);
  aboutWidget_->OpenFlipperAbout->append(tr("Glu Specific Info:"));
  aboutWidget_->OpenFlipperAbout->setCurrentFont(standardFont);
  
  QString gluVersion = QString((const char *)gluGetString(GLU_VERSION));
  aboutWidget_->OpenFlipperAbout->append(tr("GLU Version:\t") + gluVersion);
  
  aboutWidget_->OpenFlipperAbout->append(tr("Supported GLU Extensions:"));
  QString gluExtensions = QString((const char*)gluGetString(GLU_EXTENSIONS));
  aboutWidget_->OpenFlipperAbout->append(gluExtensions);
  
  
  aboutWidget_->OpenFlipperAbout->moveCursor(QTextCursor::Start);
  
  // =====================================================================================
  // glew Information
  // =====================================================================================
  #ifndef __APPLE__
  aboutWidget_->OpenFlipperAbout->append("\n");
  aboutWidget_->OpenFlipperAbout->setCurrentFont(boldFont);
  aboutWidget_->OpenFlipperAbout->append(tr("GLEW Specific Info:"));
  aboutWidget_->OpenFlipperAbout->setCurrentFont(standardFont);
  
  QString glewVersion = QString((const char *)glewGetString(GLEW_VERSION));
  aboutWidget_->OpenFlipperAbout->append(tr("GLEW Version:\t") + glewVersion);
  #endif
  
  // =====================================================================================
  // Qt information
  // =====================================================================================
  
  aboutWidget_->OpenFlipperAbout->append("\n");
  aboutWidget_->OpenFlipperAbout->setCurrentFont(boldFont);
  aboutWidget_->OpenFlipperAbout->append(tr("Qt Version Info:"));
  aboutWidget_->OpenFlipperAbout->setCurrentFont(standardFont);
  aboutWidget_->OpenFlipperAbout->append(tr("Currently used Version:\t") + qVersion() );
  aboutWidget_->OpenFlipperAbout->append(tr("Link time Version:\t\t") + QT_VERSION_STR );

  QList<QByteArray> imageFormats = QImageReader::supportedImageFormats();
  QString formatsString = "";
  for ( int i = 0 ; i < imageFormats.size() ; ++i) {
    formatsString += imageFormats[i] + " ";

    if ( (i != 0)  && (i % 15) == 0 )
      formatsString+= "\n";
  }

  aboutWidget_->OpenFlipperAbout->append(tr("Supported image read formats:") );
  aboutWidget_->OpenFlipperAbout->append(formatsString);

  aboutWidget_->OpenFlipperAbout->append("\n");

  aboutWidget_->OpenFlipperAbout->append(tr("Currently used Library paths:") );
  QStringList libPaths = QCoreApplication::libraryPaths();
  for(int i = 0 ; i < libPaths.size() ; ++i)
    aboutWidget_->OpenFlipperAbout->append(" " + libPaths[i]);


  // =========================================================
  // Qt Documentation files
  // =========================================================
/*
  aboutWidget_->OpenFlipperAbout->append("\n");
  aboutWidget_->OpenFlipperAbout->append(tr("Currently Loaded documentations:") );

  QStringList registeredDocumentations = helpEngine_->registeredDocumentations ();

  for ( int i = 0 ; i < registeredDocumentations.size(); ++i) {
    QString documentationInfo = registeredDocumentations[i];
    documentationInfo += " ";
    documentationInfo += helpEngine_->documentationFileName(registeredDocumentations[i]);
    aboutWidget_->OpenFlipperAbout->append(documentationInfo);
  }
*/
  // =====================================================================================
  // Compiler information
  // =====================================================================================    
  aboutWidget_->OpenFlipperAbout->append("\n");
  aboutWidget_->OpenFlipperAbout->setCurrentFont(boldFont);
  aboutWidget_->OpenFlipperAbout->append(tr("Compiler Info:"));
  aboutWidget_->OpenFlipperAbout->setCurrentFont(standardFont);
  aboutWidget_->OpenFlipperAbout->append( OpenFlipper::Options::compilerInfo() );
  
  QString definitions;

  // Trying to check some defines:
  #ifdef WIN32
    definitions += "WIN32 ";
  #endif

  #ifdef WIN64
    definitions += "WIN64 ";
  #endif

  #ifdef DLL
    definitions += "DLL ";
  #endif


  #ifdef APPLE
    definitions += "APPLE ";
  #endif

  #ifdef ARCH_DARWIN
    definitions += "ARCH_DARWIN ";
  #endif

  #ifdef INCLUDE_TEMPLATES
    definitions += "INCLUDE_TEMPLATES ";
  #endif

  #ifdef DEBUG
    definitions += "DEBUG ";
  #endif

  #ifdef NDEBUG
    definitions += "NDEBUG ";
  #endif

  #ifdef QT_NO_OPENGL
    definitions += "QT_NO_OPENGL ";
  #endif

  #ifdef OPENMP
    definitions += "OPENMP ";
  #endif

  #ifdef USE_OPENMP
    definitions += "USE_OPENMP ";
  #endif

  aboutWidget_->OpenFlipperAbout->append( tr("Definitions (incomplete):\n") + definitions );

  // =====================================================================================
  // List the currently registered data types
  // =====================================================================================
  aboutWidget_->OpenFlipperAbout->append("\n");
  aboutWidget_->OpenFlipperAbout->setCurrentFont(boldFont);
  aboutWidget_->OpenFlipperAbout->append(tr("Registered data types:"));
  aboutWidget_->OpenFlipperAbout->setCurrentFont(standardFont);

  QString types;

  // Iterate over all Types known to the core
  // Start at 1:
  // 0 type is defined as DATA_UNKNOWN
  DataType currentType = 1;
  for ( uint i = 0 ; i < typeCount() - 2  ; ++i) {
    types += typeName( currentType ) + "\t\t typeId: " + QString::number(currentType.value()) + "\n";
    
    // Advance to next type ( Indices are bits so multiply by to to get next bit)
    ++currentType;
  }
  
  aboutWidget_->OpenFlipperAbout->append( types );

  // =====================================================================================
  // List The File Plugins with their filters
  // =====================================================================================
  aboutWidget_->OpenFlipperAbout->append("\n");
  aboutWidget_->OpenFlipperAbout->setCurrentFont(boldFont);
  aboutWidget_->OpenFlipperAbout->append(tr("Registered File Plugins:"));
  aboutWidget_->OpenFlipperAbout->setCurrentFont(standardFont);

  for ( uint i = 0 ; i < supportedTypes().size() ; ++i ) {
    aboutWidget_->OpenFlipperAbout->setCurrentFont(boldFont);
    aboutWidget_->OpenFlipperAbout->append( "\t" + supportedTypes()[i].name );
    aboutWidget_->OpenFlipperAbout->setCurrentFont(standardFont);
    aboutWidget_->OpenFlipperAbout->append( "\t\t Load: " + supportedTypes()[i].loadFilters );
    aboutWidget_->OpenFlipperAbout->append( "\t\t Save: " + supportedTypes()[i].saveFilters );
  }


  aboutWidget_->show();

}
//=============================================================================
