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
*   $Revision: 19248 $                                                       *
*   $LastChangedBy: moebius $                                                *
*   $Date: 2014-07-21 14:27:08 +0200 (Mon, 21 Jul 2014) $                     *
*                                                                            *
\*===========================================================================*/

#include <ACG/GL/acg_glew.hh>

#include "PostProcessorAnaglyphPlugin.hh"

#include <iostream>
#include <ACG/GL/GLState.hh>
#include <ACG/GL/gl.hh>
#include <ACG/GL/ScreenQuad.hh>
#include <ACG/GL/ShaderCache.hh>

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include <OpenFlipper/common/GlobalOptions.hh>

PostProcessorAnaglyphPlugin::PostProcessorAnaglyphPlugin()
{
}

PostProcessorAnaglyphPlugin::~PostProcessorAnaglyphPlugin()
{
}

QString PostProcessorAnaglyphPlugin::checkOpenGL() {
  if ( ! ACG::openGLVersion(3, 0) )
    return QString("Insufficient OpenGL Version! OpenGL 3.0 or higher required");

  return QString("");
}



QString PostProcessorAnaglyphPlugin::postProcessorName() {
  return QString("AnaglyphStereo");
}


void PostProcessorAnaglyphPlugin::postProcess(ACG::GLState* _glstate, const std::vector<const PostProcessorInput*>& _input, const PostProcessorOutput& _output) {

  if (_input.size() != 2) {
    std::cerr << "PostProcessorAnaglyphPlugin: two input images required!" << std::endl;
    return;
  }

  // ======================================================================================================
  // Fetch shader from cache
  // ======================================================================================================

  QStringList macros;

  if (OpenFlipper::Options::stereoMode () == OpenFlipper::Options::AnaglyphCustom)
    macros.push_back("#define ANAGLYPH_CUSTOM");

  GLSL::Program* shader = ACG::ShaderCache::getInstance()->getProgram("ScreenQuad/screenquad.glsl", "AnaglyphStereo/anaglyph.glsl", &macros);

  // ======================================================================================================
  // Bind input texture
  // ======================================================================================================

  _input[1]->bindColorTex(1);
  _input[0]->bindColorTex(0);

  // ======================================================================================================
  // Bind output FBO
  // ======================================================================================================

  _output.bind();

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

  shader->use();
  shader->setUniform("SceneLeft", 0);
  shader->setUniform("SceneRight", 1);

  if (OpenFlipper::Options::stereoMode () == OpenFlipper::Options::AnaglyphCustom)
  {
    // column major
    std::vector<float> le = OpenFlipper::Options::anaglyphLeftEyeColorMatrix();
    std::vector<float> re = OpenFlipper::Options::anaglyphRightEyeColorMatrix();

    ACG::GLMatrixf ml, mr;

    for (int r = 0; r < 3; ++r)
    {
      for (int c = 0; c < 3; ++c)
      {
        ml(r,c) = le[c*3 + r];
        mr(r,c) = re[c*3 + r];
      }
    }

    shader->setUniformMat3("EyeColMatrixLeft", ml);
    shader->setUniformMat3("EyeColMatrixRight", mr);
  }

  // ======================================================================================================
  // Execute
  // ======================================================================================================

  ACG::ScreenQuad::draw(shader);


  shader->disable();
}


#if QT_VERSION < 0x050000
  Q_EXPORT_PLUGIN2( postprocessoranaglyphstereoplugin , PostProcessorAnaglyphPlugin );
#endif

