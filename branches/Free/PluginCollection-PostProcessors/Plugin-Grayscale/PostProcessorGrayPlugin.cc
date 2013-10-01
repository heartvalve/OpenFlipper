/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2013 by Computer Graphics Group, RWTH Aachen       *
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

#if QT_VERSION >= 0x050000 
  #include <QtWidgets>
#else
  #include <QtGui>
#endif

#include "PostProcessorGrayPlugin.hh"

#include <iostream>
#include <ACG/GL/GLState.hh>
#include <ACG/GL/gl.hh>
#include <ACG/GL/ScreenQuad.hh>

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include <OpenFlipper/common/GlobalOptions.hh>

// QT_NO_OPENGL currently has to be undefined first in QT5 because of QT5 and GLEW conflicts
#undef QT_NO_OPENGL
#include <QGLFormat>
#define QT_NO_OPENGL


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

QString PostProcessorGrayscalePlugin::checkOpenGL() {
  QGLFormat::OpenGLVersionFlags flags = QGLFormat::openGLVersionFlags();
  if ( ! flags.testFlag(QGLFormat::OpenGL_Version_3_0) )
    return QString("Insufficient OpenGL Version! OpenGL 3.0 or higher required");

  return QString("");
}


void PostProcessorGrayscalePlugin::postProcess(ACG::GLState* _glstate, const PostProcessorInput& _input, GLuint _targetFBO) {

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
  glBindTexture(GL_TEXTURE_2D, _input.colorTex_);

  // ======================================================================================================
  // Bind output FBO
  // ======================================================================================================

  glBindFramebuffer(GL_FRAMEBUFFER, _targetFBO);

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

  ACG::Vec2f texcoordOffset(1.0f / float(_input.width), 1.0f / float(_input.height));
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

