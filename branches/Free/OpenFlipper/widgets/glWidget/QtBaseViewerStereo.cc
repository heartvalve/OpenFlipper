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

#include "crc32.hh"

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
glViewer::setStereoMode(bool _b)
{
  stereo_ = _b;

  if (!stereo_) {
    makeCurrent();
    glDrawBuffer(GL_BACK);
  }

  updateProjectionMatrix ();

  updateGL();
}

//-----------------------------------------------------------------------------


void
glViewer::drawScene_glStereo()
{
  double l, r, t, b, w, h, a, radians, wd2, ndfl, zerop, xrange;

  w = glWidth();
  h = glHeight();
  a = w / h;

  radians = fovy_ * 0.5 / 180.0 * M_PI;
  wd2     = near_ * tan(radians);
  zerop   = near_ + ((far_ - near_) * OpenFlipper::Options::focalDistance ());
  ndfl    = near_ / zerop ;
  xrange  = a * wd2 * 2 * zerop / near_;

  l = -a*wd2;
  r =  a*wd2;
  t =  wd2;
  b = -wd2;

  double offset  = 0.5 * OpenFlipper::Options::eyeDistance () * xrange;
  double offset2 = offset * ndfl;

  // left eye
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum(l+offset2, r+offset2, b, t, near_, far_);
  glTranslatef(+offset, 0.0, 0.0);
  glMatrixMode(GL_MODELVIEW);
  glDrawBuffer(GL_BACK_LEFT);
  glstate_->clearBuffers ();
  glClear(GL_DEPTH_BUFFER_BIT);
  drawScene_mono();


  // right eye
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum(l-offset2, r-offset2, b, t, near_, far_);
  glTranslatef(-offset, 0.0, 0.0);
  glMatrixMode(GL_MODELVIEW);
  glDrawBuffer(GL_BACK_RIGHT);
  glstate_->clearBuffers ();
  glClear(GL_DEPTH_BUFFER_BIT);
  drawScene_mono();
  glDrawBuffer(GL_BACK);
}


void
glViewer::drawScenePhilipsStereo()
{
  
  int vp_l, vp_b, vp_w, vp_h;
  glstate_->get_viewport (vp_l, vp_b, vp_w, vp_h);
  
  // ======================================================================================================
  // creating a color texture
  // ======================================================================================================
  ACG::Texture2D colorTexture;
  colorTexture.enable();
  colorTexture.bind();
  GLenum texTarget         = GL_TEXTURE_2D;
  GLenum texInternalFormat = GL_RGBA;
  GLenum texFormat         = GL_RGBA;
  GLenum texType           = GL_UNSIGNED_BYTE;
  GLenum texFilterMode     = GL_NEAREST;
  glTexImage2D(texTarget, 0, texInternalFormat, vp_w, vp_h, 0, texFormat, texType, NULL);
  
  glTexParameterf(texTarget, GL_TEXTURE_MIN_FILTER, texFilterMode);
  glTexParameterf(texTarget, GL_TEXTURE_MAG_FILTER, texFilterMode);
  glTexParameterf(texTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameterf(texTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(texTarget, GL_TEXTURE_COMPARE_FUNC_ARB, GL_LEQUAL);
  
  // ======================================================================================================
  // creating an 24-bit depth + 8-bit stencil texture
  // ======================================================================================================
  ACG::Texture2D depthStencilTexture;
  depthStencilTexture.enable();
  depthStencilTexture.bind();
  texTarget         = GL_TEXTURE_2D;
  texInternalFormat = GL_DEPTH24_STENCIL8_EXT;
  texFormat         = GL_DEPTH_STENCIL_EXT;
  texType           = GL_UNSIGNED_INT_24_8_EXT;
  texFilterMode     = GL_NEAREST;
  glTexImage2D(texTarget, 0, texInternalFormat, vp_w, vp_h, 0, texFormat, texType, NULL);

  glTexParameterf(texTarget, GL_TEXTURE_MIN_FILTER, texFilterMode);
  glTexParameterf(texTarget, GL_TEXTURE_MAG_FILTER, texFilterMode);
  glTexParameterf(texTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameterf(texTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(texTarget, GL_TEXTURE_COMPARE_FUNC_ARB, GL_LEQUAL);

  // ======================================================================================================
  // Disable textures
  // ======================================================================================================
  depthStencilTexture.disable();
  colorTexture.disable();
  
  // ======================================================================================================
  // Render the scene
  // ======================================================================================================
  glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
  drawScene_mono();
  
  // ======================================================================================================
  // Copy Scene to Textures
  // ======================================================================================================
  colorTexture.enable();
  colorTexture.bind();
  
  glCopyTexSubImage2D (GL_TEXTURE_2D, 0, 0, 0, vp_l, vp_b, vp_w, vp_h);
  
  colorTexture.disable();
  
  depthStencilTexture.enable();
  depthStencilTexture.bind();
  
  glCopyTexSubImage2D (GL_TEXTURE_2D, 0, 0, 0, vp_l, vp_b, vp_w, vp_h);
  
  depthStencilTexture.disable();

  // ======================================================================================================
  // Setup the shaders used to render color and depth info next to each other
  // ======================================================================================================
  GLSL::PtrVertexShader   vertexShader;
  GLSL::PtrFragmentShader fragmentShader;
  GLSL::PtrProgram        program;
  
  QString vshaderFile = OpenFlipper::Options::shaderDirStr() + QDir::separator() + "Philips/Vertex.glsl";
  QString fshaderFile = OpenFlipper::Options::shaderDirStr() + QDir::separator() + "Philips/Fragment42.glsl";
  
  ////
  vertexShader            = GLSL::loadVertexShader(  vshaderFile.toStdString().c_str() );
  fragmentShader          = GLSL::loadFragmentShader( fshaderFile.toStdString().c_str() );
  program                 = GLSL::PtrProgram(new GLSL::Program());
  
  if ( (vertexShader == 0)   ||
       (fragmentShader == 0) ||
       (program == 0) ) {
    std::cerr << "Unable to load shaders for philips display rendering!";
    return;
  }
  
  program->attach(vertexShader);
  program->attach(fragmentShader);
  program->link();
  program->use();

  // ======================================================================================================
  // Bind textures to different texture units and tell shader where to find them
  // ======================================================================================================
  glActiveTextureARB(GL_TEXTURE0_ARB);
  colorTexture.enable(); 
  colorTexture.bind(); 
  
  glActiveTextureARB(GL_TEXTURE1_ARB);
  depthStencilTexture.enable();  
  depthStencilTexture.bind();  
  
  program->setUniform("ColorTexture",0);
  program->setUniform("DepthStencil",1);
  


  // ======================================================================================================
  // Render plain textured 
  // ======================================================================================================
  glDisable(GL_LIGHTING);
  glDisable(GL_COLOR_MATERIAL);
  glDisable(GL_DEPTH_TEST);

  
  // ======================================================================================================
  // Setup orthogonal projection
  // ======================================================================================================
  glstate_->push_projection_matrix();
  glstate_->push_modelview_matrix();

  glstate_->reset_projection();
  glstate_->reset_modelview();

  glstate_->ortho(0, vp_w, 0, vp_h, 0, 1);


  // ======================================================================================================
  // Bind textures to different texture units and tell shader where to find them
  // ======================================================================================================
  glColor3f(1.0,1.0,1.0);
  
  // ======================================================================================================
  // Clear buffers
  // ======================================================================================================
  glClearColor(.0, .0, .0, 0);
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

  // ======================================================================================================
  // Render a simple quad (rest is done by shader)
  // ======================================================================================================
  glBegin(GL_QUADS);
  glTexCoord2f(0.0f, 1.0f); glVertex2i( 0, vp_h);
  glTexCoord2f(1.0f, 1.0f); glVertex2i( vp_w, vp_h);
  glTexCoord2f(1.0f, 0.0f); glVertex2i( vp_w, 0);
  glTexCoord2f(0.0f, 0.0f); glVertex2i( 0, 0);
  glEnd();
  
  program->disable();
  
  glBindTexture(GL_TEXTURE_2D, 0);
  
  // ======================================================================================================
  // Cleanup (color and depth textures)  
  // ======================================================================================================
  depthStencilTexture.del();
  colorTexture.del();

  // ======================================================================================================
  // Compute the header required for the display
  // ======================================================================================================
  uchar *header = new uchar[ 6 ];

  //   Header ID
  //   Basic identifier used by the display to verify the header
  header[0] = 241; // Header_ID1 = 11110001
  
  //   Header content type
  //   This entry controls the displays internal rendering based on the input data specified here.
  //   There is no info about how this changes the rendering
  //   Possible values:
  //   0 No Depth
  //   1 Signage
  //   2 Movie
  //   3 Game
  //   4 CGI
  //   5 Still
  header[1] = 3;   // Hdr_Content_type (Game) = 00000011 (Gaming Mode)
  
  //   Header Factor
  //   Each 3D Display has a 'Display recommended depth value', which corresponds to an
  //   acceptable maximum depth factor value for that specific type of display. This value strongly
  //   depends on the lens design. The factor field in the header contains the percentage to be
  //   used from the display recommended depth value. The value of 64 corresponds with the 100%
  //   of the display recommended depth value. It is allowed to use values higher than 64. The factor
  //   works on a linear scale and is multiplied with the factor controlled by the user in the Display
  //   Control Tool.
  //   Value range: 0-255 (default 64)
  header[2] = 64;  // Hdr_Factor
  
  //   Header Offset CC
  //   Values in the Depth map equal to the header-offset value will be located on the plane of the
  //   display. All values in the disparity map with a higher value will de displayed in front of the
  //   display.
  //   Offset_CC is the offset controlled by the Content Creator. In the system there is also an
  //   Offset_user present, which is controlled by the user using the Display Control Tool.
  //   Value Range: 0-255 (default 128)
  header[3] = 128; // Hdr_Offset_CC
  
  //   Header select
  //   When all select signals are low the rendering settings are set to optimal settings for the content
  //   type denoted by Hdr_content_type. By making select signals high the settings for Factor and
  //   Offset_cc can be controlled individually by the header.
  //   Possible Values:
  //   0 Use Displays defaults and automatic optimizations
  //   1 Use Header provided factor
  //   2 Use Header provided offset
  //   3 Use both factor and offset
  header[4] = 0;   // Hdr_Factor_select(1) + Hdr_Offset_select(1) + reserved(6)
  
  //   Unused Header entry (leave at 0 !)
  header[5] = 0;   // Reserved

  //   Header checksum.
  //   The 4-byte EDC field H(6) − H(9) contains an Error Detection Code computed over the first 6
  //   header bytes. This EDC uses the standard CRC-32 polynomial as defined in IEEE 802.3 and ITU-T
  //   V.42. The initial value and final XOR value are both 0.
  //   unsigned long has 32bit = 4Byte
  unsigned long checksum = CalcCRC32(&header[0], 6);

  //   Store the complete header in a bit vector
  std::vector< uchar > bitVector;

  // For all bytes of the header
  for (int i = 0; i < 6; i++) {
    
    // For each bit of a headers byte
    for ( int j = 7 ; j >= 0 ; --j ) {
      
      // Red and Green component have to be 0
      bitVector.push_back(0);
      bitVector.push_back(0);
      
      // If bit is set, the full component will be set to one otherwise zero
      // And the order of the bits has to be reversed!
      if ( header[i] & (1 << j ) ) {
        bitVector.push_back(255);
      } else {
        bitVector.push_back(0);
      }
      
      // Only every second pixel is used for the header
      // Skip every odd one by filling in 0 for RGB
      bitVector.push_back(0);
      bitVector.push_back(0);
      bitVector.push_back(0);
        
    }
    
  }

  // Append checksum to header. 
  // Reversed bit order!
  for (int i=31; i >= 0; i--) {
    
    // Red and Green component have to be 0
    bitVector.push_back(0);
    bitVector.push_back(0);
    
    if (  checksum & (1 << i ) )
      bitVector.push_back( 255 );
    else
      bitVector.push_back( 0 );
    
    // Only every second pixel is used for the header
    // Skip every odd one by filling in 0 for RGB    
    bitVector.push_back(0);
    bitVector.push_back(0);
    bitVector.push_back(0);
  }


  // Select the top left of the renderbuffer and
  // write complete header into these bits
  glRasterPos2i (0,glHeight()-1); 
  glDrawPixels(bitVector.size()/3, 1,GL_RGB ,GL_UNSIGNED_BYTE , &bitVector[0]);
  
  // ======================================================================================================
  // Reset projection and modelview
  // ======================================================================================================     
  glstate_->pop_projection_matrix();
  glstate_->pop_modelview_matrix();

}




//-----------------------------------------------------------------------------


void
glViewer::drawScene_anaglyphStereo()
{
 double l, r, t, b, w, h, a, radians, wd2, ndfl, zerop, xrange;

  w = glWidth();
  h = glHeight();
  a = w / h;

  radians = fovy_ * 0.5 / 180.0 * M_PI;
  wd2     = near_ * tan(radians);
  zerop   = near_ + ((far_ - near_) * OpenFlipper::Options::focalDistance ());
  ndfl    = near_ / zerop ;
  xrange  = a * wd2 * 2 * zerop / near_;

  l = -a*wd2;
  r =  a*wd2;
  t =  wd2;
  b = -wd2;

  double offset  = 0.5 * OpenFlipper::Options::eyeDistance () * xrange;
  double offset2 = offset * ndfl;

  // left eye
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum(l+offset2, r+offset2, b, t, near_, far_);
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
  glFrustum(l-offset2, r-offset2, b, t, near_, far_);
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
    glBindTexture (GL_TEXTURE_RECTANGLE_NV, agTexture_[0]);
    glTexImage2D (GL_TEXTURE_RECTANGLE_NV, 0, GL_RGB, glstate_->viewport_width (),
                  glstate_->viewport_height (), 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    glBindTexture (GL_TEXTURE_RECTANGLE_NV, agTexture_[1]);
    glTexImage2D (GL_TEXTURE_RECTANGLE_NV, 0, GL_RGB, glstate_->viewport_width (),
                  glstate_->viewport_height (), 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    glBindTexture (GL_TEXTURE_RECTANGLE_NV, 0);

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

  radians = fovy_ * 0.5 / 180.0 * M_PI;
  wd2     = near_ * tan(radians);
  zerop   = near_ + ((far_ - near_) * OpenFlipper::Options::focalDistance ());
  ndfl    = near_ / zerop ;
  xrange  = a * wd2 * 2 * zerop / near_;

  l = -a*wd2;
  r =  a*wd2;
  t =  wd2;
  b = -wd2;

  double offset  = 0.5 * OpenFlipper::Options::eyeDistance () * xrange;
  double offset2 = offset * ndfl;

  int vp_l, vp_b, vp_w, vp_h;
  glstate_->get_viewport (vp_l, vp_b, vp_w, vp_h);

  std::vector<float> le = OpenFlipper::Options::anaglyphLeftEyeColorMatrix();
  std::vector<float> re = OpenFlipper::Options::anaglyphRightEyeColorMatrix();

  // left eye
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum(l+offset2, r+offset2, b, t, near_, far_);
  glTranslatef(offset, 0.0, 0.0);
  glMatrixMode(GL_MODELVIEW);
  glstate_->clearBuffers ();
  glClear(GL_DEPTH_BUFFER_BIT);
  drawScene_mono();

  glBindTexture (GL_TEXTURE_RECTANGLE_NV, agTexture_[0]);
  glCopyTexSubImage2D (GL_TEXTURE_RECTANGLE_NV, 0, 0, 0, vp_l, vp_b, vp_w, vp_h);
  glBindTexture (GL_TEXTURE_RECTANGLE_NV, 0);

  // right eye
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum(l-offset2, r-offset2, b, t, near_, far_);
  glTranslatef(-offset, 0.0, 0.0);
  glMatrixMode(GL_MODELVIEW);
  glstate_->clearBuffers ();
  glClear(GL_DEPTH_BUFFER_BIT);
  drawScene_mono();

  glBindTexture (GL_TEXTURE_RECTANGLE_NV, agTexture_[1]);
  glCopyTexSubImage2D (GL_TEXTURE_RECTANGLE_NV, 0, 0, 0, vp_l, vp_b, vp_w, vp_h);
  glBindTexture (GL_TEXTURE_RECTANGLE_NV, 0);

  glActiveTexture (GL_TEXTURE0);
  glBindTexture (GL_TEXTURE_RECTANGLE_NV, agTexture_[0]);
  glEnable (GL_TEXTURE_RECTANGLE_NV);

  glActiveTexture (GL_TEXTURE1);
  glBindTexture (GL_TEXTURE_RECTANGLE_NV, agTexture_[1]);
  glEnable (GL_TEXTURE_RECTANGLE_NV);

  glEnable (GL_FRAGMENT_PROGRAM_ARB);
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

  glDisable (GL_DEPTH_TEST);

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

  glEnable (GL_DEPTH_TEST);

  glBindProgramARB (GL_FRAGMENT_PROGRAM_ARB, 0);
  glDisable (GL_FRAGMENT_PROGRAM_ARB);
  
  glActiveTexture (GL_TEXTURE1);
  glBindTexture (GL_TEXTURE_RECTANGLE_NV, 0);
  glDisable (GL_TEXTURE_RECTANGLE_NV);

  glActiveTexture (GL_TEXTURE0);
  glBindTexture (GL_TEXTURE_RECTANGLE_NV, 0);
  glDisable (GL_TEXTURE_RECTANGLE_NV);

}


//=============================================================================
//=============================================================================
