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

#include <QtGui>

#include "PostProcessorDepthImagePlugin.hh"

#include <iostream>
#include <ACG/GL/GLState.hh>

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include <OpenFlipper/common/GlobalOptions.hh>


PostProcessorDepthImagePlugin::PostProcessorDepthImagePlugin() :
depthStencilTextureBufferWidth_(0),
depthStencilTextureBufferHeight_(0)
{

}

QString PostProcessorDepthImagePlugin::postProcessorName() {
  return QString("Show Depth Image");
}



void PostProcessorDepthImagePlugin::updateDepthStencilTextureBuffer(ACG::GLState* _glstate) {

  if ( !ACG::checkExtensionSupported("GL_ARB_texture_rectangle") ) {
    std::cerr << "GL_ARB_texture_rectangle not supported! " << std::endl;
    return;
  }

  int vp_l, vp_b, vp_w, vp_h;
  _glstate->get_viewport (vp_l, vp_b, vp_w, vp_h);

  // Does depth stencil texture exist?
  if (!pDepthStencilTexture_.is_valid()) {
    // ======================================================================================================
    // creating an 24-bit depth + 8-bit stencil texture
    // ======================================================================================================

    pDepthStencilTexture_.enable();
    pDepthStencilTexture_.bind();
    GLenum texTarget = GL_TEXTURE_2D;
    GLenum texInternalFormat = GL_DEPTH24_STENCIL8_EXT;
    GLenum texFormat = GL_DEPTH_STENCIL_EXT;
    GLenum texType = GL_UNSIGNED_INT_24_8_EXT;
    GLenum texFilterMode = GL_NEAREST;
    glTexImage2D(texTarget, 0, texInternalFormat, vp_w, vp_h, 0, texFormat, texType, NULL);

    glTexParameterf(texTarget, GL_TEXTURE_MIN_FILTER, texFilterMode);
    glTexParameterf(texTarget, GL_TEXTURE_MAG_FILTER, texFilterMode);
    glTexParameterf(texTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(texTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(texTarget, GL_TEXTURE_COMPARE_FUNC_ARB, GL_LEQUAL);

  }

  // Resize target texture
  if (_glstate->viewport_width() != depthStencilTextureBufferWidth_ || _glstate->viewport_height() != depthStencilTextureBufferHeight_) {

    // Depth stencil texture
    pDepthStencilTexture_.bind();

    glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGB, _glstate->viewport_width(), _glstate->viewport_height(), 0,
        GL_RGB, GL_UNSIGNED_BYTE, 0);

    ACG::GLState::bindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);

    depthStencilTextureBufferWidth_  = _glstate->viewport_width();
    depthStencilTextureBufferHeight_ = _glstate->viewport_height();
  }

}


void PostProcessorDepthImagePlugin::postProcess(ACG::GLState* _glstate) {

  // ======================================================================================================
  // Adjust buffer to correct size
  // ======================================================================================================
  updateDepthStencilTextureBuffer(_glstate);

  // ======================================================================================================
  // Get current viewport size
  // ======================================================================================================
  int vp_l, vp_b, vp_w, vp_h;
  _glstate->get_viewport(vp_l, vp_b, vp_w, vp_h);

  // ======================================================================================================
  // Bind depth Stencil texture
  // ======================================================================================================
  pDepthStencilTexture_.enable();
  pDepthStencilTexture_.bind();

  // ======================================================================================================
  // Copy depth component of rendered image to texture
  // ======================================================================================================
  glCopyTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8_EXT, vp_l, vp_b, vp_w , vp_h, 0);

  // ======================================================================================================
  // Render plain textured
  // ======================================================================================================
  ACG::GLState::disable(GL_LIGHTING);
  ACG::GLState::disable(GL_COLOR_MATERIAL);
  ACG::GLState::disable(GL_DEPTH_TEST);

  // ======================================================================================================
  // Setup orthogonal projection
  // ======================================================================================================
  _glstate->push_projection_matrix();
  _glstate->push_modelview_matrix();

  _glstate->reset_projection();
  _glstate->reset_modelview();

  // Setup orthogonal projection (remember that we are in a viewport of the current glstate)
  _glstate->ortho(0, vp_w, 0, vp_h, 0, 1);


  // ======================================================================================================
  // Clear rendering buffer
  // ======================================================================================================
  _glstate->clearBuffers();

  // ======================================================================================================
  // Render a simple quad (rest is done by the texture)
  // ======================================================================================================
  glBegin(GL_QUADS);
  glTexCoord2f(0.0f, 0.0f);
  glVertex2i(0, 0);
  glTexCoord2f(1.0f, 0.0f);
  glVertex2i(vp_w, 0);
  glTexCoord2f(1.0f, 1.0f);
  glVertex2i(vp_w, vp_h);
  glTexCoord2f(0.0f, 1.0f);
  glVertex2i(0, vp_h);
  glEnd();

  // Disable depth stencil buffer
  pDepthStencilTexture_.disable();

  // ======================================================================================================
  // Reset projection and modelview
  // ======================================================================================================
  _glstate->pop_projection_matrix();
  _glstate->pop_modelview_matrix();


}

Q_EXPORT_PLUGIN2( postprocessordepthimageplugin , PostProcessorDepthImagePlugin );

