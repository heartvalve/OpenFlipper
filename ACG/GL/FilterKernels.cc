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


#include <ACG/GL/ShaderCache.hh>
#include <ACG/GL/ScreenQuad.hh>
#include <ACG/GL/GLFormatInfo.hh>
#include <ACG/ShaderUtils/GLSLShader.hh>

#include <ACG/GL/FBO.hh>

#include "FilterKernels.hh"

#include <fstream>


namespace ACG
{


BaseSeparableFilterKernel::BaseSeparableFilterKernel( int _texWidth, int _texHeight, GLenum _internalfmt )
  : texWidth_(_texWidth),
  texHeight_(_texHeight),
  internalfmt_(_internalfmt),
  externalfmt_(_internalfmt),
  tempRT_(new FBO())
{
  externalfmt_ = ACG::GLFormatInfo(internalfmt_).format();
  texelSize_ = ACG::Vec2f(1.0f / float(_texWidth), 1.0f / float(_texHeight));
}


BaseSeparableFilterKernel::~BaseSeparableFilterKernel()
{
  delete tempRT_;
}


bool BaseSeparableFilterKernel::execute( GLuint _srcTexture, ACG::FBO* _dstFBO, GLuint _dstColorAttachment, GLuint _tempColorAttachment )
{
  bool success = true;

  GLuint tempTexture = 0;
  if (!_dstFBO || !_tempColorAttachment)
  {
    tempTexture = tempRT_->getAttachment(GL_COLOR_ATTACHMENT0);

    if (!tempTexture)
    {
      tempRT_->attachTexture2D(GL_COLOR_ATTACHMENT0, texWidth_, texHeight_, internalfmt_, externalfmt_, GL_CLAMP, GL_LINEAR, GL_LINEAR);

      tempTexture = tempRT_->getAttachment(GL_COLOR_ATTACHMENT0);
    }
  }
  else
  {
    tempTexture = _dstFBO->getAttachment(tempTexture);

    if (!tempTexture)
      return false;
  }

  GLint vp[4];
  glGetIntegerv(GL_VIEWPORT, vp);
  glViewport(0, 0, texWidth_, texHeight_);

  // temp target

  ACG::FBO* passFBO = 0;
  if (_tempColorAttachment && _dstFBO)
  {
    _dstFBO->bind();
    glDrawBuffer(_tempColorAttachment);
    passFBO = _dstFBO;
  }
  else
  {
    tempRT_->bind();
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    passFBO = tempRT_;
  }


  // 1. pass

  GLSL::Program* passShader = setupPass(0, _srcTexture);

  if (passShader)
    ACG::ScreenQuad::draw(passShader);
  else
    success = false;

  passFBO->unbind();

  // 2. pass

  if (_dstFBO && _dstColorAttachment)
  {
    _dstFBO->bind();
    glDrawBuffer(_dstColorAttachment);
    passFBO = _dstFBO;
  }
  else
  {
    // render to previously bound fbo
    passFBO = 0;
  }


  passShader = setupPass(1, tempTexture);

  if (passShader)
    ACG::ScreenQuad::draw(passShader);
  else
    success = false;

  // restore input fbo
  if (passFBO)
    passFBO->unbind();
  glViewport(vp[0], vp[1], vp[2], vp[3]);

  return success;
}

void BaseSeparableFilterKernel::resizeInput( int _texWidth, int _texHeight )
{
  if ( (texWidth_ != _texWidth || texHeight_ != _texHeight) )
  {
    texWidth_ = _texWidth;
    texHeight_ = _texHeight;
    texelSize_ = ACG::Vec2f(1.0f / float(_texWidth), 1.0f / float(_texHeight));

    if (tempRT_->width())
      tempRT_->resize(_texWidth, _texHeight);

    updateKernel();
  }
}

// ----------------------------------------------------------------------------


GaussianBlurFilter::GaussianBlurFilter( int _texWidth,
  int _texHeight,
  int _blurRadius,
  float _blurSigma,
  GLenum _internalfmt )
  : BaseSeparableFilterKernel(_texWidth, _texHeight, _internalfmt),
  radius_(_blurRadius),
  samples_(2 * _blurRadius + 1),
  sigma_(_blurSigma)
{
  updateKernel();
}

GaussianBlurFilter::~GaussianBlurFilter()
{

}


void GaussianBlurFilter::updateKernel()
{
  samples_ = radius_ * 2 + 1;

  offsetsX_.resize(samples_);
  offsetsY_.resize(samples_);
  weights_.resize(samples_);

  // center sample
  offsetsX_[radius_] = ACG::Vec2f(0.0f, 0.0f);
  offsetsY_[radius_] = ACG::Vec2f(0.0f, 0.0f);
  weights_[radius_] = 1.0f;

  float weightSum = 1.0f; // 1.0 is the weight of the center sample

  // left and right taps
  for (int i = 0; i < radius_; ++i)
  {
    // offset
    ACG::Vec2f v = texelSize() * float(i + 1);
    offsetsX_[i] = ACG::Vec2f(-v[0], 0.0f);
    offsetsX_[radius_ + i + 1] = ACG::Vec2f(v[0], 0.0f);

    offsetsY_[i] = ACG::Vec2f(0.0f, -v[1]);
    offsetsY_[radius_ + i + 1] = ACG::Vec2f(0.0f, v[1]);

    // weight
    float w = expf(-float((i+1)*(i+1)) / (sigma_ * sigma_));
    weights_[radius_ + i + 1] = weights_[i] = w;
    weightSum += 2.0f * w;
  }


  // normalize
  for (int i = 0; i < samples_; ++i)
    weights_[i] /= weightSum;

  QString blurSamplesMacro;
  blurSamplesMacro.sprintf("#define BLUR_SAMPLES %i", samples_);
  macros_.clear();
  macros_.push_back(blurSamplesMacro);
}



void GaussianBlurFilter::setKernel( int _blurRadius, float _blurSigma )
{
  radius_ = _blurRadius;
  sigma_ = _blurSigma;
  updateKernel();
}

GLSL::Program* GaussianBlurFilter::setupPass(int _pass, GLuint _srcTex)
{
  GLSL::Program* blurShader = ACG::ShaderCache::getInstance()->getProgram("ScreenQuad/screenquad.glsl", "Blur/kernel.glsl", &macros_);
  
  if (_pass == 0)
  {
    // 1. pass horizontal blur

    blurShader->use();

    blurShader->setUniform("g_InputTex", 0);
    blurShader->setUniform("g_SampleOffsets", &offsetsX_[0], samples_);
    blurShader->setUniform("g_SampleWeights", &weights_[0], samples_);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _srcTex);
  }
  else
  {
    // 2. pass vertical blur

    blurShader->setUniform("g_SampleOffsets", &offsetsY_[0], samples_);

    glBindTexture(GL_TEXTURE_2D, _srcTex);
  }

  return blurShader;
}


// ----------------------------------------------------------------------------


BilateralBlurFilter::BilateralBlurFilter(int _texWidth, int _texHeight,
  int _blurRadius, 
  float _blurSigmaS,
  float _blurSigmaR,
  GLenum _internalfmt)
  : BaseSeparableFilterKernel(_texWidth, _texHeight, _internalfmt),
  radius_(_blurRadius),
  samples_(2 * _blurRadius + 1),
  sigma_(_blurSigmaS, _blurSigmaR),
  depthTex_(0)
{
  updateKernel();
}

BilateralBlurFilter::~BilateralBlurFilter()
{

}


void BilateralBlurFilter::updateKernel()
{
  samples_ = radius_ * 2 + 1;

  sigma2Rcp_ = ACG::Vec2f(-1.0f / (2.0f * sigma_[0] * sigma_[0]),
    -1.0f / (2.0f * sigma_[1] * sigma_[1]));


  // compute filter kernel

  offsetsX_.resize(samples_);
  offsetsY_.resize(samples_);
  spatialKernel_.resize(samples_);


  // center sample
  offsetsX_[radius_] = ACG::Vec2f(0.0f, 0.0f);
  offsetsY_[radius_] = ACG::Vec2f(0.0f, 0.0f);
  spatialKernel_[radius_] = 0.0f;

  // left and right taps
  for (int i = 0; i < radius_; ++i)
  {
    // offset
    ACG::Vec2f v = texelSize() * float(i + 1);
    offsetsX_[i] = ACG::Vec2f(-v[0], 0.0f);
    offsetsX_[radius_ + i + 1] = ACG::Vec2f(v[0], 0.0f);

    offsetsY_[i] = ACG::Vec2f(0.0f, -v[1]);
    offsetsY_[radius_ + i + 1] = ACG::Vec2f(0.0f, v[1]);

    // spatial kernel
    float r2 = float((i+1)*(i+1));
    spatialKernel_[radius_ + i + 1] = spatialKernel_[i] = r2 * sigma2Rcp_[0];
  }


  macros_.clear();

  QString radiusMacro;
  radiusMacro.sprintf("#define BLUR_SAMPLES %i", samples_);

  int numChannels = GLFormatInfo(internalFormat()).channelCount();
  numChannels = std::max(std::min(numChannels, 4), 1);

  const char* blurChannels[4] = 
  {
    "BLUR_R",
    "BLUR_RG",
    "BLUR_RGB",
    "BLUR_RGBA"
  };

  QString channelMacro;
  channelMacro.sprintf("#define BLUR_CHANNELS %s", blurChannels[numChannels - 1]);

  macros_.push_back(radiusMacro);
  macros_.push_back(channelMacro);
}

void BilateralBlurFilter::setKernel( int _blurRadius, float _blurSigmaS, float _blurSigmaR )
{
  radius_ = _blurRadius;
  sigma_ = ACG::Vec2f(_blurSigmaS, _blurSigmaR);
  updateKernel();
}

GLSL::Program* BilateralBlurFilter::setupPass(int _pass, GLuint _srcTex)
{
  GLSL::Program* blurShader = ACG::ShaderCache::getInstance()->getProgram("ScreenQuad/screenquad.glsl", "Blur/bilateral.glsl", &macros_);

  if (_pass == 0)
  {
    // 1. pass horizontal blur

    blurShader->use();

    blurShader->setUniform("g_InputTex", 0);
    blurShader->setUniform("g_DepthTex", 1);
    blurShader->setUniform("g_P", proj_);

    blurShader->setUniform("g_BlurSigmaRcp2", sigma2Rcp_[1]);
    blurShader->setUniform("g_SampleOffsets", &offsetsX_[0], samples_);
    blurShader->setUniform("g_SpatialKernel", &spatialKernel_[0], samples_);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, depthTex_);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _srcTex);
  }
  else
  {
    // 2. pass vertical blur

    blurShader->setUniform("g_SampleOffsets", &offsetsY_[0], samples_);

    glBindTexture(GL_TEXTURE_2D, _srcTex);
  }

  return blurShader;
}


// ----------------------------------------------------------------------------


RadialBlurFilter::RadialBlurFilter( int _numSamples )
  : samples_(0)
{
  setKernel(_numSamples);
}

bool RadialBlurFilter::execute( GLuint _srcTexture, float _blurRadius, float _blurIntensity, const ACG::Vec2f& _blurCenter )
{
  GLSL::Program* blurShader = ACG::ShaderCache::getInstance()->getProgram("ScreenQuad/screenquad.glsl", "Blur/radial.glsl", &macros_);

  if (blurShader)
  {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _srcTexture);


    blurShader->use();

    blurShader->setUniform("g_InputTex", 0);
    blurShader->setUniform("g_BlurCenter", _blurCenter);
    blurShader->setUniform("g_BlurRadiusRcp2", 1.0f / (_blurRadius * _blurRadius));
    blurShader->setUniform("g_BlurIntensity", _blurIntensity);

    ACG::ScreenQuad::draw(blurShader);

    return true;
  }

  return false;
}

void RadialBlurFilter::setKernel( int _numSamples )
{
  samples_ = _numSamples;

  macros_.clear();
  QString sampleMacro;
  sampleMacro.sprintf("#define BLUR_SAMPLES %i", _numSamples);
  macros_.push_back(sampleMacro);
}


// ----------------------------------------------------------------------------


PoissonBlurFilter::PoissonBlurFilter( float _radius, float _sampleDistance, int _numTris /*= 30*/ )
  : radius_(_radius), sampleDistance_(_sampleDistance), numTries_(_numTris)
{
  // "Fast Poisson Disk Sampling in Arbitrary Dimensions"
  // http://people.cs.ubc.ca/~rbridson/docs/bridson-siggraph07-poissondisk.pdf

  // rejection test for disk domain

  // domain during generation is [0, 2 * radius]
  // this is mapped to [-radius, radius] afterwards

  // step 0.

  // r = sampleDistance,  n = 2
  float cellSize = _sampleDistance / sqrtf(2.0f);

  int gridSize = int(2.0f * _radius / cellSize) + 1; // account for partially sized cell at the end

  std::vector<int> grid(gridSize * gridSize, -1);



  // step 1.

  ACG::Vec2f x0(0.0f, 0.0f);
  
  // initial uniform sample in disk
  do 
  {
    x0 = ACG::Vec2f(float(rand()) / float(RAND_MAX), float(rand()) / float(RAND_MAX));
  } while ((x0 * 2.0f - ACG::Vec2f(1.0f, 1.0f)).sqrnorm() > _radius*_radius);
  
  x0 = x0 * 2.0f * _radius;

  std::list<int> activeList;

  samples_.reserve(32);
  samples_.push_back(x0);
  activeList.push_back(0);
  int numSamples = 1;

  ACG::Vec2i gridPos0(int(x0[0] / cellSize), int(x0[1] / cellSize));

  grid[gridPos0[1] * gridSize + gridPos0[0]] = 0;


  // step 2.

  float sampleDistance2 = _sampleDistance * _sampleDistance;

  while (!activeList.empty())
  {
    int listSize = activeList.size();
    // random index i
    int i = int((float(rand()) / float(RAND_MAX)) * float(listSize) + 0.5f);

    i = std::min(i, listSize - 1);

//    int sample_i = activeList[i];
    int sample_i = -1;
    std::list<int>::iterator it_i = activeList.begin();
    for (int counter = 0; it_i != activeList.end(); ++it_i, ++counter)
    {
      if (counter == i)
      {
        sample_i = *it_i;
        break;
      }
    }

    ACG::Vec2f x_i = samples_[sample_i];

    ACG::Vec2i gridPos_i(int(x_i[0] / cellSize), int(x_i[1] / cellSize));

    bool foundNextSample_i = false;

    // k uniform point samples in circle around x_i
    for (int s = 0; s < _numTris; ++s)
    {
      float u = float(rand()) / float(RAND_MAX);
      float v = float(rand()) / float(RAND_MAX);

      float alpha = float(M_PI) * 2.0f * u;
      ACG::Vec2f x_s(cosf(alpha), sinf(alpha));

      // between r and 2r, where r = sample distance
      x_s *= _sampleDistance + 2.0f * _sampleDistance * v;

      // centered around x_i
      x_s += x_i;

      ACG::Vec2i gridPos_s(int(x_s[0] / cellSize), int(x_s[1] / cellSize));

      // oob check
      if (x_s[0] < 0.0f || x_s[1] < 0.0f || gridPos_s[0] < 0 || gridPos_s[0] >= gridSize || gridPos_s[1] < 0 || gridPos_s[1] >= gridSize)
        continue;

      // disk check
      if ( (x_s - ACG::Vec2f(_radius, _radius)).sqrnorm() > _radius*_radius)
        continue;

      // neighborhood check

      bool tooClose = false;

      for (int x = -1; x <= 1 && !tooClose; ++x)
      {
        for (int y = -1; y <= 1 && !tooClose; ++y)
        {
          ACG::Vec2i gridPos_t = gridPos_s + ACG::Vec2i(x,y);

          // oob check
          if (gridPos_t[0] < 0 || gridPos_t[0] >= gridSize || gridPos_t[1] < 0 || gridPos_t[1] >= gridSize)
            continue;

          int gridValue = grid[gridPos_t[1] * gridSize + gridPos_t[0]];

          if (gridValue >= 0)
          {
            ACG::Vec2f delta_t = samples_[gridValue] - x_s;
            float d2 = delta_t | delta_t;

            if (d2 < sampleDistance2)
              tooClose = true;
          }
        }
      }

      if (!tooClose)
      {
        // new sample found
        foundNextSample_i = true;

        grid[gridPos_s[1] * gridSize + gridPos_s[0]] = numSamples;
        samples_.push_back(x_s);

        activeList.push_back(numSamples);

        ++numSamples;
      }
    }

    if (!foundNextSample_i)
    {
      // remove from list
      activeList.erase(it_i);
    }

  }

  // map to [-radius, radius]
  for (int i = 0; i < numSamples; ++i)
    samples_[i] = samples_[i] - ACG::Vec2f(_radius, _radius);

  samplesScaled_ = samples_;


  QString samplesMacro;
  samplesMacro.sprintf("#define BLUR_SAMPLES %i", numSamples);
  macros_.push_back(samplesMacro);
}

PoissonBlurFilter::~PoissonBlurFilter()
{

}


void PoissonBlurFilter::dumpSamples( const char* _filename )
{
  std::fstream f(_filename, std::ios_base::out);

  if (f.is_open())
  {
    for (size_t i = 0; i < samples_.size(); ++i)
      f << "v " << samples_[i][0] << " " << samples_[i][1] << " 0" << std::endl;

    f.close();
  }
}

bool PoissonBlurFilter::execute( GLuint _srcTex, float _kernelScale )
{
  // scale kernel
  int n = numSamples();
  for (int i = 0; i < n; ++i)
    samplesScaled_[i] = samples_[i] * _kernelScale;

  GLSL::Program* blurShader = ACG::ShaderCache::getInstance()->getProgram("ScreenQuad/screenquad.glsl", "Blur/poisson.glsl", &macros_);

  if (blurShader)
  {
    blurShader->use();
    blurShader->setUniform("g_InputTex", 0);
    blurShader->setUniform("g_SampleOffsets", &samplesScaled_[0], n);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _srcTex);

    ACG::ScreenQuad::draw(blurShader);

    return true;
  }

  return false;
}


}
