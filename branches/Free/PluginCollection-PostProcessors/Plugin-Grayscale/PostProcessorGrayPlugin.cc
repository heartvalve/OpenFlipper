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

#include <ACG/GL/acg_glew.hh>

#include "PostProcessorGrayPlugin.hh"

#include <iostream>
#include <ACG/GL/GLState.hh>
#include <ACG/GL/gl.hh>
#include <ACG/GL/ScreenQuad.hh>

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include <OpenFlipper/common/GlobalOptions.hh>

PostProcessorGrayscalePlugin::PostProcessorGrayscalePlugin() :
shader_(0)
{
}

PostProcessorGrayscalePlugin::~PostProcessorGrayscalePlugin()
{
  delete shader_;
}


QString PostProcessorGrayscalePlugin::postProcessorName() {
  return QString("Grayscale");
}


void PostProcessorGrayscalePlugin::postProcess(ACG::GLState* _glstate, const std::vector<const PostProcessorInput*>& _input, const PostProcessorOutput& _output) {

  // ======================================================================================================
  // Load shader if needed
  // ======================================================================================================
  if (!shader_)
    shader_ = GLSL::loadProgram("Grayscale/screenquad.glsl", "Grayscale/gray.glsl");

  // ======================================================================================================
  // Bind input texture
  // ======================================================================================================

  glActiveTexture(GL_TEXTURE0);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, _input[0]->colorTex_);

  // ======================================================================================================
  // Bind output FBO
  // ======================================================================================================

  glBindFramebuffer(GL_FRAMEBUFFER, _output.fbo_);
  glDrawBuffer(_output.drawBuffer_);

  // ======================================================================================================
  // Clear rendering buffer
  // ======================================================================================================
   _glstate->clearBuffers();

  // ======================================================================================================
  // Setup render states
  // ======================================================================================================

  glDepthMask(1);
  glColorMask(1,1,1,1);

  glDisable(GL_DEPTH_TEST);
  glDisable(GL_BLEND);

  // ======================================================================================================
  // Setup shader
  // ======================================================================================================

  shader_->use();
  shader_->setUniform("textureSampler", 0);

  ACG::Vec2f texcoordOffset(1.0f / float(_input[0]->width), 1.0f / float(_input[0]->height));
  shader_->setUniform("texcoordOffset", texcoordOffset);

  // ======================================================================================================
  // Execute
  // ======================================================================================================

  ACG::ScreenQuad::draw(shader_);


  shader_->disable();
}


#if QT_VERSION < 0x050000
  Q_EXPORT_PLUGIN2( postprocessorgrayscaleplugin , PostProcessorGrayscalePlugin );
#endif

