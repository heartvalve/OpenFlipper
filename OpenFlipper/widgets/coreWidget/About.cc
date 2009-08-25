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




//=============================================================================
//
//  CLASS Core - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================

#include "CoreWidget.hh"

#include <QGLFormat>

#include <OpenFlipper/common/GlobalOptions.hh>

//== IMPLEMENTATION ==========================================================



void CoreWidget::showAboutWidget( ) {

  if ( OpenFlipper::Options::nogui() )
    return;

  if ( aboutWidget_ == 0 ) {
    aboutWidget_ = new AboutWidget( this );
    aboutWidget_->OpenFlipperAbout->append(tr("OpenFlipper Core Version: ") + OpenFlipper::Options::coreVersion() ) ;

    aboutWidget_->OpenFlipperAbout->append("\n");
    aboutWidget_->OpenFlipperAbout->append(tr("OpenFlipper Directories:"));
    // Set the Path to the shaders
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
    aboutWidget_->OpenFlipperAbout->append(tr("OpenGL Specific Info:"));


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



    aboutWidget_->OpenFlipperAbout->append(tr("Supported Extensions:"));
    QString glExtensions = QString((const char*)glGetString(GL_EXTENSIONS));
    aboutWidget_->OpenFlipperAbout->append(glExtensions);

    aboutWidget_->OpenFlipperAbout->moveCursor(QTextCursor::Start);
    
    aboutWidget_->OpenFlipperAbout->append("\n");
    aboutWidget_->OpenFlipperAbout->append(tr("Qt Version Info:"));
    aboutWidget_->OpenFlipperAbout->append(tr("Currently used Version:\t") + qVersion() );
    aboutWidget_->OpenFlipperAbout->append(tr("Link time Version:\t\t") + QT_VERSION_STR );
    


  }

  aboutWidget_->show();

}
//=============================================================================
