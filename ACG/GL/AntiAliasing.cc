/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2014 by Computer Graphics Group, RWTH Aachen      *
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
 *   $Revision: 18130 $                                                       *
 *   $Author: moebius $                                                      *
 *   $Date: 2014-02-05 10:41:16 +0100 (Wed, 05 Feb 2014) $                   *
 *                                                                           *
\*===========================================================================*/



//=============================================================================
//
//  OpenGL AntiAliasing methods
//
//=============================================================================


//== INCLUDES =================================================================

#include <ACG/GL/AntiAliasing.hh>
#include <ACG/Math/VectorT.hh>
#include <ACG/ShaderUtils/GLSLShader.hh>
#include <ACG/GL/ScreenQuad.hh>


//== DEFINES ==================================================================

// shader files
#define MSAA_SCREENQUAD_SHADER "ScreenQuad/screenquad.glsl"
#define MSAA_NEAREST_SHADER "MSAA/sample_nearest.glsl"
#define MSAA_LINEAR_SHADER "MSAA/sample_linear.glsl"



//== NAMESPACES ===============================================================

namespace ACG {


//== CLASS IMPLEMENTATION =====================================================

#ifdef GL_ARB_texture_multisample


MSFilterWeights::MSFilterWeights(int _numSamples) : numSamples_(_numSamples) {

  weights_.resize(_numSamples);
  float sumWeights = 0.0f;

  // texel center is at (0.5, 0.5) as specified in 
  //  http://www.opengl.org/sdk/docs/man3/xhtml/glGetMultisample.xml
  Vec2f texelCenter(0.5f, 0.5f);

  for (int i = 0; i < _numSamples; ++i) {
    GLfloat val[2];
    glGetMultisamplefv(GL_SAMPLE_POSITION, i, val);
    
    Vec2f samplePosition(val[0], val[1]);

    // weighting based on distance to texel center
    float sampleDist = (samplePosition - texelCenter).norm();

    // samples close to the center are weighted higher than samples farther away
    weights_[i] = 1.0f - sampleDist;

    sumWeights += weights_[i];
  }

  // normalize weights

  for (int i = 0; i < _numSamples; ++i)
    weights_[i] /= sumWeights;
}

//=============================================================================

void MSFilterWeights::asTextureBuffer( TextureBuffer& out ) {
  if (numSamples_)
    out.setBufferData(numSamples_ * 4, &weights_[0], GL_R32F, GL_STATIC_DRAW);
}

//=============================================================================




//=============================================================================


MSTextureSampler::MSTextureSampler() : shaderNearest_(0), shaderLinear_(0) {
}


MSTextureSampler::~MSTextureSampler() {
  delete shaderNearest_;
  delete shaderLinear_;
}

MSTextureSampler& MSTextureSampler::instance() {
  static MSTextureSampler singleton;
  return singleton;
}

//=============================================================================

void MSTextureSampler::init() {
  if (!shaderNearest_)
    shaderNearest_ = GLSL::loadProgram(MSAA_SCREENQUAD_SHADER, MSAA_NEAREST_SHADER);

  if (!shaderLinear_)
    shaderLinear_ = GLSL::loadProgram(MSAA_SCREENQUAD_SHADER, MSAA_LINEAR_SHADER);
}

//=============================================================================

void MSTextureSampler::filterMSAATexture_Nearest( GLuint _texture, int _samples, const float* _weights /*= 0*/ ) {

  MSTextureSampler& sampler = instance();

  // load shader
  if (!sampler.shaderNearest_)
    sampler.init();

  GLSL::Program* shader = sampler.shaderNearest_;

  if (!shader)
    return;
  

  shader->use();

  // offset and scale of screenquad
  shader->setUniform("offset", Vec2f(0.0f, 0.0f));
  shader->setUniform("size", Vec2f(1.0f, 1.0f));
  
  // sample count and filter weights
  shader->setUniform("numSamples", _samples);

  // bind multisampled texture to slot 0
  shader->setUniform("inputTex", 0);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, _texture);

  // run texture filter
  ScreenQuad::draw(shader);

  shader->disable();
}

//=============================================================================

void MSTextureSampler::filterMSAATexture_Linear( GLuint _texture, int _samples, const float* _weights /*= 0*/ ) {

  MSTextureSampler& sampler = instance();

  // load shader
  if (!sampler.shaderLinear_)
    sampler.init();

  GLSL::Program* shader = sampler.shaderLinear_;

  if (!shader)
    return;


  shader->use();

  // offset and scale of screenquad
  shader->setUniform("offset", Vec2f(0.0f, 0.0f));
  shader->setUniform("size", Vec2f(1.0f, 1.0f));

  // sample count and filter weights
  shader->setUniform("numSamples", _samples);

  // bind multisampled texture to slot 0
  shader->setUniform("inputTex", 0);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, _texture);

  // run texture filter
  ScreenQuad::draw(shader);

  shader->disable();
}


//=============================================================================



#endif // GL_ARB_texture_multisample


} // namespace ACG
//=============================================================================
