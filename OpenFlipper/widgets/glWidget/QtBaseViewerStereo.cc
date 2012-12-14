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
//  CLASS glViewer - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================

#include "QtBaseViewer.hh"
#include <QStatusBar>
#include <QToolButton>

#include <OpenFlipper/common/GlobalOptions.hh>

#include <QGLFramebufferObject>

#include <ACG/ShaderUtils/GLSLShader.hh>
#include <ACG/GL/globjects.hh>

#include <ACG/ShaderUtils/gldebug.h>

//== NAMESPACES ===============================================================


//== IMPLEMENTATION ==========================================================

static const char* customAnaglyphProg = {
  "!!ARBfp1.0"
  "TEMP left, right, lmul, rmul;"
  "TEX left, fragment.texcoord[0], texture[0], RECT;"
  "TEX right, fragment.texcoord[0], texture[1], RECT;"
  "DP3 lmul.r, left, program.env[0];"
  "DP3 lmul.g, left, program.env[1];"
  "DP3 lmul.b, left, program.env[2];"
  "DP3 rmul.r, right, program.env[3];"
  "DP3 rmul.g, right, program.env[4];"
  "DP3 rmul.b, right, program.env[5];"
  "ADD result.color, lmul, rmul;"
  "END"
};

//-----------------------------------------------------------------------------

void
glViewer::drawScene_glStereo()
{
  double l, r, t, b, w, h, a, radians, wd2, ndfl, zerop, xrange;

  w = glWidth();
  h = glHeight();
  a = w / h;

  double fovy = OpenFlipperSettings().value("Core/Projection/FOVY",45.0).toDouble();
  radians = fovy * 0.5 / 180.0 * M_PI;
  wd2     = properties_.nearPlane() * tan(radians);
  zerop   = properties_.nearPlane() + ((properties_.farPlane() - properties_.nearPlane()) * OpenFlipperSettings().value("Core/Stereo/FocalDistance",0.5).toDouble() );
  ndfl    = properties_.nearPlane() / zerop ;
  xrange  = a * wd2 * 2 * zerop / properties_.nearPlane();

  l = -a*wd2;
  r =  a*wd2;
  t =  wd2;
  b = -wd2;

  double offset  = 0.5 * OpenFlipperSettings().value("Core/Stereo/EyeDistance",0.07).toDouble()  * xrange;
  double offset2 = offset * ndfl;

  // left eye
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum(l+offset2, r+offset2, b, t, properties_.nearPlane(), properties_.farPlane());
  glTranslatef(+offset, 0.0, 0.0);
  glMatrixMode(GL_MODELVIEW);
  ACG::GLState::drawBuffer(GL_BACK_LEFT);
  glstate_->clearBuffers ();
  glClear(GL_DEPTH_BUFFER_BIT);
  drawScene_mono();


  // right eye
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum(l-offset2, r-offset2, b, t, properties_.nearPlane(), properties_.farPlane());
  glTranslatef(-offset, 0.0, 0.0);
  glMatrixMode(GL_MODELVIEW);
  ACG::GLState::drawBuffer(GL_BACK_RIGHT);
  glstate_->clearBuffers ();
  glClear(GL_DEPTH_BUFFER_BIT);
  drawScene_mono();
  ACG::GLState::drawBuffer(GL_BACK);
}



//-----------------------------------------------------------------------------


void
glViewer::drawScene_anaglyphStereo()
{
 double l, r, t, b, w, h, a, radians, wd2, ndfl, zerop, xrange;

  w = glWidth();
  h = glHeight();
  a = w / h;

  double fovy = OpenFlipperSettings().value("Core/Projection/FOVY", 45.0).toDouble();
  radians = fovy * 0.5 / 180.0 * M_PI;
  wd2     = properties_.nearPlane() * tan(radians);
  zerop   = properties_.nearPlane() + ((properties_.farPlane() - properties_.nearPlane()) * OpenFlipperSettings().value("Core/Stereo/FocalDistance", 0.5).toDouble() );
  ndfl    = properties_.nearPlane() / zerop ;
  xrange  = a * wd2 * 2 * zerop / properties_.nearPlane();

  l = -a*wd2;
  r =  a*wd2;
  t =  wd2;
  b = -wd2;

  double offset  = 0.5 * OpenFlipperSettings().value("Core/Stereo/EyeDistance", 0.07).toDouble() * xrange;
  double offset2 = offset * ndfl;

  // left eye
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum(l+offset2, r+offset2, b, t, properties_.nearPlane(), properties_.farPlane());
  glTranslatef(offset, 0.0, 0.0);

  glMatrixMode(GL_MODELVIEW);
  glstate_->clearBuffers ();
  glClear(GL_DEPTH_BUFFER_BIT);

  // draw red channel for left eye
  glColorMask(GL_TRUE,GL_FALSE,GL_FALSE,GL_TRUE);
  drawScene_mono();
  glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);


  // right eye
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum(l-offset2, r-offset2, b, t, properties_.nearPlane(), properties_.farPlane());
  glTranslatef(-offset, 0.0, 0.0);
  glMatrixMode(GL_MODELVIEW);
  glClear(GL_DEPTH_BUFFER_BIT);

  // draw green and blue channel for right eye
  glColorMask(GL_FALSE,GL_TRUE,GL_TRUE,GL_TRUE);
  drawScene_mono();
  glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);

}

//-----------------------------------------------------------------------------

void
glViewer::updateCustomAnaglyphStereo()
{
  if (!customAnaglyphSupported_)
    return;

  if ( !ACG::checkExtensionSupported("GL_ARB_texture_rectangle") ) {
    std::cerr << "GL_ARB_texture_rectangle not supported! " << std::endl;
    customAnaglyphSupported_ = false;
    return;
  }

  if (!agProgram_)
  {
    GLint errorPos;

    glGenProgramsARB (1, &agProgram_);

    glGetError ();
    glBindProgramARB (GL_FRAGMENT_PROGRAM_ARB, agProgram_);
    glProgramStringARB (GL_FRAGMENT_PROGRAM_ARB, GL_PROGRAM_FORMAT_ASCII_ARB,
                        strlen (customAnaglyphProg), customAnaglyphProg);

    glGetIntegerv (GL_PROGRAM_ERROR_POSITION_ARB, &errorPos);
    if (glGetError () != GL_NO_ERROR || errorPos != -1)
    {
      printf("Error loading program %d %s\n",errorPos, glGetString(GL_PROGRAM_ERROR_STRING_ARB));
      glDeleteProgramsARB (1, &agProgram_);
      customAnaglyphSupported_ = false;
      return;
    }
  }

  if (!agTexture_[0])
    glGenTextures (2, agTexture_);

  if (!agTexture_[0])
  {
    finiCustomAnaglyphStereo ();
    customAnaglyphSupported_ = false;
    return;
  }

  if (glstate_->viewport_width () != agTexWidth_ ||
      glstate_->viewport_height () != agTexHeight_)
  {
    ACG::GLState::bindTexture (GL_TEXTURE_RECTANGLE_ARB, agTexture_[0]);
    glTexImage2D (GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGB, glstate_->viewport_width (),
                  glstate_->viewport_height (), 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    ACG::GLState::bindTexture (GL_TEXTURE_RECTANGLE_ARB, agTexture_[1]);
    glTexImage2D (GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGB, glstate_->viewport_width (),
                  glstate_->viewport_height (), 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    ACG::GLState::bindTexture (GL_TEXTURE_RECTANGLE_ARB, 0);

    agTexWidth_ = glstate_->viewport_width ();
    agTexHeight_ = glstate_->viewport_height ();
  }
}

//-----------------------------------------------------------------------------

void
glViewer::finiCustomAnaglyphStereo()
{
  if (!customAnaglyphSupported_)
    return;

  if (agProgram_)
    glDeleteProgramsARB (1, &agProgram_);

  if (agTexture_[0])
    glDeleteTextures (2, agTexture_);
}

//-----------------------------------------------------------------------------

void
glViewer::drawScene_customAnaglyphStereo()
{
  updateCustomAnaglyphStereo ();

  if (!customAnaglyphSupported_)
    return;

  double l, r, t, b, w, h, a, radians, wd2, ndfl, zerop, xrange;

  w = glWidth();
  h = glHeight();
  a = w / h;

  double fovy = OpenFlipperSettings().value("Core/Projection/FOVY",45.0).toDouble();
  radians = fovy * 0.5 / 180.0 * M_PI;
  wd2     = properties_.nearPlane() * tan(radians);
  zerop   = properties_.nearPlane() + ((properties_.farPlane() - properties_.nearPlane()) * OpenFlipperSettings().value("Core/Stereo/FocalDistance",0.5).toDouble() );
  ndfl    = properties_.nearPlane() / zerop ;
  xrange  = a * wd2 * 2 * zerop / properties_.nearPlane();

  l = -a*wd2;
  r =  a*wd2;
  t =  wd2;
  b = -wd2;

  double offset  = 0.5 * OpenFlipperSettings().value("Core/Stereo/EyeDistance",0.07).toDouble() * xrange;
  double offset2 = offset * ndfl;

  int vp_l, vp_b, vp_w, vp_h;
  glstate_->get_viewport (vp_l, vp_b, vp_w, vp_h);

  std::vector<float> le = OpenFlipper::Options::anaglyphLeftEyeColorMatrix();
  std::vector<float> re = OpenFlipper::Options::anaglyphRightEyeColorMatrix();

  // left eye
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum(l+offset2, r+offset2, b, t, properties_.nearPlane(), properties_.farPlane());
  glTranslatef(offset, 0.0, 0.0);
  glMatrixMode(GL_MODELVIEW);
  glstate_->clearBuffers ();
  glClear(GL_DEPTH_BUFFER_BIT);
  drawScene_mono();

  ACG::GLState::bindTexture (GL_TEXTURE_RECTANGLE_ARB, agTexture_[0]);
  glCopyTexSubImage2D (GL_TEXTURE_RECTANGLE_ARB, 0, 0, 0, vp_l, vp_b, vp_w, vp_h);
  ACG::GLState::bindTexture (GL_TEXTURE_RECTANGLE_ARB, 0);

  // right eye
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum(l-offset2, r-offset2, b, t, properties_.nearPlane(), properties_.farPlane());
  glTranslatef(-offset, 0.0, 0.0);
  glMatrixMode(GL_MODELVIEW);
  glstate_->clearBuffers ();
  glClear(GL_DEPTH_BUFFER_BIT);
  drawScene_mono();

  ACG::GLState::bindTexture (GL_TEXTURE_RECTANGLE_ARB, agTexture_[1]);
  glCopyTexSubImage2D (GL_TEXTURE_RECTANGLE_ARB, 0, 0, 0, vp_l, vp_b, vp_w, vp_h);
  ACG::GLState::bindTexture (GL_TEXTURE_RECTANGLE_ARB, 0);

  ACG::GLState::activeTexture (GL_TEXTURE0);
  ACG::GLState::bindTexture (GL_TEXTURE_RECTANGLE_ARB, agTexture_[0]);
  ACG::GLState::enable (GL_TEXTURE_RECTANGLE_ARB);

  ACG::GLState::activeTexture (GL_TEXTURE1);
  ACG::GLState::bindTexture (GL_TEXTURE_RECTANGLE_ARB, agTexture_[1]);
  ACG::GLState::enable (GL_TEXTURE_RECTANGLE_ARB);

  ACG::GLState::enable (GL_FRAGMENT_PROGRAM_ARB);
  glBindProgramARB (GL_FRAGMENT_PROGRAM_ARB, agProgram_);

  glProgramEnvParameter4fARB (GL_FRAGMENT_PROGRAM_ARB, 0, le[0], le[3], le[6], 0.0);
  glProgramEnvParameter4fARB (GL_FRAGMENT_PROGRAM_ARB, 1, le[1], le[4], le[7], 0.0);
  glProgramEnvParameter4fARB (GL_FRAGMENT_PROGRAM_ARB, 2, le[2], le[5], le[8], 0.0);

  glProgramEnvParameter4fARB (GL_FRAGMENT_PROGRAM_ARB, 3, re[0], re[3], re[6], 0.0);
  glProgramEnvParameter4fARB (GL_FRAGMENT_PROGRAM_ARB, 4, re[1], re[4], re[7], 0.0);
  glProgramEnvParameter4fARB (GL_FRAGMENT_PROGRAM_ARB, 5, re[2], re[5], re[8], 0.0);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, vp_w, vp_h, 0, 0, 1.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  ACG::GLState::disable (GL_DEPTH_TEST);

  glBegin (GL_QUADS);
  glMultiTexCoord2f (GL_TEXTURE0, 0, vp_h);
  glMultiTexCoord2f (GL_TEXTURE1, 0, vp_h);
  glVertex2i(0, 0);
  glMultiTexCoord2f (GL_TEXTURE0, 0, 0);
  glMultiTexCoord2f (GL_TEXTURE1, 0, 0);
  glVertex2i(0, vp_h);
  glMultiTexCoord2f (GL_TEXTURE0, vp_w, 0);
  glMultiTexCoord2f (GL_TEXTURE1, vp_w, 0);
  glVertex2i(vp_w, vp_h);
  glMultiTexCoord2f (GL_TEXTURE0, vp_w, vp_h);
  glMultiTexCoord2f (GL_TEXTURE1, vp_w, vp_h);
  glVertex2i(vp_w, 0);
  glEnd ();

  ACG::GLState::enable (GL_DEPTH_TEST);

  glBindProgramARB (GL_FRAGMENT_PROGRAM_ARB, 0);
  ACG::GLState::disable (GL_FRAGMENT_PROGRAM_ARB);

  ACG::GLState::activeTexture (GL_TEXTURE1);
  ACG::GLState::bindTexture (GL_TEXTURE_RECTANGLE_ARB, 0);
  ACG::GLState::disable (GL_TEXTURE_RECTANGLE_ARB);

  ACG::GLState::activeTexture (GL_TEXTURE0);
  ACG::GLState::bindTexture (GL_TEXTURE_RECTANGLE_ARB, 0);
  ACG::GLState::disable (GL_TEXTURE_RECTANGLE_ARB);

}


//=============================================================================
//=============================================================================
