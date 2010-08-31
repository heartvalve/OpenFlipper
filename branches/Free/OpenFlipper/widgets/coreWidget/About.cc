/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2010 by Computer Graphics Group, RWTH Aachen      *
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




//=============================================================================
//
//  CLASS Core - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================

#include "CoreWidget.hh"

#include <QGLFormat>

#include <OpenFlipper/common/GlobalOptions.hh>

#ifndef WIN32
  #ifndef ARCH_DARWIN
    #include <malloc.h>
  #endif
#endif

//== IMPLEMENTATION ==========================================================



void CoreWidget::showAboutWidget( ) {

  if ( OpenFlipper::Options::nogui() )
    return;
  
  if ( aboutWidget_ == 0 ) {
    aboutWidget_ = new AboutWidget( this );
  } else {
    aboutWidget_->OpenFlipperAbout->clear();
  }
  
  QFont standardFont = aboutWidget_->OpenFlipperAbout->currentFont();
  QFont boldFont = standardFont;
  boldFont.setBold(true);

  aboutWidget_->OpenFlipperAbout->setCurrentFont(boldFont);
  aboutWidget_->OpenFlipperAbout->append(tr("OpenFlipper Core Version: ") + OpenFlipper::Options::coreVersion() ) ;
  aboutWidget_->OpenFlipperAbout->setCurrentFont(standardFont);

  // =====================================================================================
  // Directory info
  // =====================================================================================
  aboutWidget_->OpenFlipperAbout->append("\n");
  aboutWidget_->OpenFlipperAbout->setCurrentFont(boldFont);
  aboutWidget_->OpenFlipperAbout->append(tr("OpenFlipper Directories:"));
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
  
  
  // =====================================================================================
  // Memory infos
  // =====================================================================================
  aboutWidget_->OpenFlipperAbout->append("\n");
  aboutWidget_->OpenFlipperAbout->setCurrentFont(boldFont);
  aboutWidget_->OpenFlipperAbout->append(tr("Memory Information:"));    
  aboutWidget_->OpenFlipperAbout->setCurrentFont(standardFont);
 
 
  #ifdef WIN32 
 
    aboutWidget_->OpenFlipperAbout->append(tr("Not available for this platform (WIN32)"));   

  #elif defined ARCH_DARWIN 

    aboutWidget_->OpenFlipperAbout->append(tr("Not available for this platform (MacOS)"));   

  #else
    unsigned long memory = 0;
    
    struct mallinfo info;
    info = mallinfo();
    
    //    std::cout << "**********************" << std::endl;
    //    std::cout << "arena: " << info.arena << std::endl;
    //    std::cout << "ordblks: " << info.ordblks << std::endl;
    //    std::cout << "hblks: " << info.hblks << std::endl;
    //    std::cout << "hblkhd: " << info.hblkhd << std::endl;
    //    std::cout << "uordblks: " << info.uordblks << std::endl;
    //    std::cout << "fordblks: " << info.fordblks << std::endl;
    //    std::cout << "keepcost: " << info.keepcost << std::endl;
    
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
  // OpenGL Renderer/Vendor and version info
  // =====================================================================================    
  aboutWidget_->OpenFlipperAbout->append("\n");
  aboutWidget_->OpenFlipperAbout->setCurrentFont(boldFont);
  aboutWidget_->OpenFlipperAbout->append(tr("OpenGL Specific Info:"));
  aboutWidget_->OpenFlipperAbout->setCurrentFont(standardFont);


  QString vendor = QString((const char*)glGetString(GL_VENDOR));
  aboutWidget_->OpenFlipperAbout->append(tr("Vendor:\t") + vendor);
  QString renderer = QString((const char*)glGetString(GL_RENDERER));
  aboutWidget_->OpenFlipperAbout->append(tr("Renderer:\t") + renderer);
  
  QGLFormat::OpenGLVersionFlags flags = QGLFormat::openGLVersionFlags();

  if ( flags.testFlag(QGLFormat::OpenGL_Version_3_0) )
    aboutWidget_->OpenFlipperAbout->append(tr("Version:\t3.0 or higher") );
  else if (flags.testFlag(QGLFormat::OpenGL_Version_2_1))
    aboutWidget_->OpenFlipperAbout->append(tr("Version:\t2.1 or higher" ));
  else if (flags.testFlag(QGLFormat::OpenGL_Version_2_0))
    aboutWidget_->OpenFlipperAbout->append(tr("Version:\t2.0 or higher" ));
  else if (flags.testFlag(QGLFormat::OpenGL_Version_1_5))
    aboutWidget_->OpenFlipperAbout->append(tr("Version:\t1.5 or higher" ));
  else if (flags.testFlag(QGLFormat::OpenGL_Version_1_4))
    aboutWidget_->OpenFlipperAbout->append(tr("Version:\t1.4 or higher" ));
  else if (flags.testFlag(QGLFormat::OpenGL_Version_1_3))
    aboutWidget_->OpenFlipperAbout->append(tr("Version:\t1.3 or higher" ));
  else if (flags.testFlag(QGLFormat::OpenGL_Version_1_2))
    aboutWidget_->OpenFlipperAbout->append(tr("Version:\t1.2 or higher" ));
  else if (flags.testFlag(QGLFormat::OpenGL_Version_1_1))
    aboutWidget_->OpenFlipperAbout->append(tr("Version:\t1.1 or higher" ));
  else
    aboutWidget_->OpenFlipperAbout->append(tr("Version:\tUNKNOWN!" ));



  // =====================================================================================
  // OpenGL Extensions
  // =====================================================================================
  aboutWidget_->OpenFlipperAbout->append(tr("Supported Extensions:"));
  QString glExtensions = QString((const char*)glGetString(GL_EXTENSIONS));
  aboutWidget_->OpenFlipperAbout->append(glExtensions);
  
  
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
  aboutWidget_->OpenFlipperAbout->append("\n");
  aboutWidget_->OpenFlipperAbout->setCurrentFont(boldFont);
  aboutWidget_->OpenFlipperAbout->append(tr("GLEW Specific Info:"));
  aboutWidget_->OpenFlipperAbout->setCurrentFont(standardFont);
  
  QString glewVersion = QString((const char *)glewGetString(GLEW_VERSION));
  aboutWidget_->OpenFlipperAbout->append(tr("GLEW Version:\t") + glewVersion);
  
  // =====================================================================================
  // Qt information
  // =====================================================================================
  
  aboutWidget_->OpenFlipperAbout->append("\n");
  aboutWidget_->OpenFlipperAbout->setCurrentFont(boldFont);
  aboutWidget_->OpenFlipperAbout->append(tr("Qt Version Info:"));
  aboutWidget_->OpenFlipperAbout->setCurrentFont(standardFont);
  aboutWidget_->OpenFlipperAbout->append(tr("Currently used Version:\t") + qVersion() );
  aboutWidget_->OpenFlipperAbout->append(tr("Link time Version:\t\t") + QT_VERSION_STR );
  
  // =====================================================================================
  // Compiler information
  // =====================================================================================    
  aboutWidget_->OpenFlipperAbout->append("\n");
  aboutWidget_->OpenFlipperAbout->setCurrentFont(boldFont);
  aboutWidget_->OpenFlipperAbout->append(tr("Compiler Version Info:"));
  aboutWidget_->OpenFlipperAbout->setCurrentFont(standardFont);
  aboutWidget_->OpenFlipperAbout->append( OpenFlipper::Options::compilerInfo() );
  
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
    currentType++;
  }
  
  aboutWidget_->OpenFlipperAbout->append( types );

  aboutWidget_->show();

}
//=============================================================================
