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

#pragma once


#include <ACG/Config/ACGDefines.hh>
#include <ACG/Math/GLMatrixT.hh>
#include <ACG/GL/gl.hh>

#include <QStringList>
#include <vector>


namespace ACG
{

// Forward Declaration
class FBO;



class ACGDLLEXPORT BaseSeparableFilterKernel
{
public:

  /* \brief Create separable filter
   *
   * @param _texWidth width of input texture
   * @param _texHeight height of input texture
   * @param _internalfmt internal gl format of the texture
  */
  BaseSeparableFilterKernel(int _texWidth, int _texHeight, GLenum _internalfmt = GL_RGBA);

  /// Class destructor
  virtual ~BaseSeparableFilterKernel();


  /* \brief Execute filter
   *
   * Perform the two filter passes.
   * If a custom fbo and a _tempColorAttachment is provided, the first pass is 
   * rendered into that temporary render texture.
   * If a custom fbo and a _dstColorAttachment is provided, the second pass is 
   * rendered into that destination render texture.
   * 
   * If _dstFBO or _tempColorAttachment is invalid, an internal fbo is created as target for the first pass.
   * If _dstFBO or _dstColorAttachment is invalid, the second pass renders into whatever is currently bound as render target.
   *
   * @param _srcTexture 2d input texture
   * @param _dstFBO custom target fbo (optional)
   * @param _dstColorAttachment target attachment of the custom target fbo (optional)
   * @param _tempColorAttachment temporary attachment of the custom fbo, that is different from _dstColorAttachment (optional)
   * @return true on success, false otherwise
  */
  bool execute(GLuint _srcTexture, ACG::FBO* _dstFBO = 0, GLuint _dstColorAttachment = GL_COLOR_ATTACHMENT0, GLuint _tempColorAttachment = 0);


  /* \brief Resize input texture
   *
   * @param _texWidth new input texture width
   * @param _texHeight new input texture height
  */
  void resizeInput(int _texWidth, int _texHeight);

  /// input texture width
  int texWidth() const {return texWidth_;}

  /// input texture height
  int texHeight() const {return texHeight_;}

  /// texel size in uv space
  const ACG::Vec2f& texelSize() const {return texelSize_;}

  /// internal format of the input texture
  GLenum internalFormat() const {return internalfmt_;}

protected:

  /* \brief Setup shader with uniforms
   *
   * @param _pass pass number: 0 or 1
   * @param _scrTex source texture for pass
   * @return shader program for screen quad
  */
  virtual GLSL::Program* setupPass(int _pass, GLuint _srcTex) = 0;


  /* \brief Update kernel after resizing
   *
  */
  virtual void updateKernel() = 0;

private:

  int texWidth_,
    texHeight_;

  GLenum internalfmt_,
    externalfmt_;

  // size of a texel in uv space
  ACG::Vec2f texelSize_;

  // temp render targets if none supplied by user
  //  attachment0 : target for first axis pass
  ACG::FBO* tempRT_;
};


// separable 2d gaussian blur
class ACGDLLEXPORT GaussianBlurFilter : public BaseSeparableFilterKernel
{
public:

  /* \brief Create separable gauss filter
   *
   * @param _texWidth width of input texture
   * @param _texHeight height of input texture
   * @param _blurRadius radius in pixel coords of the blur kernel
   * @param _blurSigma blur smoothness, standard deviation of the gaussian function
   * @param _internalfmt internal gl format of the texture
  */
  GaussianBlurFilter(int _texWidth, int _texHeight, int _blurRadius, float _blurSigma = 1.0f, GLenum _internalfmt = GL_RGBA);

  /// Class destructor
  virtual ~GaussianBlurFilter();

  /* \brief Change kernel settings
   *
   * @param _blurRadius new radius
   * @param _blurSigma new sigma
  */
  void setKernel(int _blurRadius, float _blurSigma);

  /// radius
  int radius() const {return radius_;}

  /// number of samples
  int samples() const {return samples_;}

  /// blur sigma
  int sigma() const {return sigma_;}

  /// sample offsets along x direction
  const std::vector<ACG::Vec2f>& offsetsX() const {return offsetsX_;}

  /// sample offsets along y direction
  const std::vector<ACG::Vec2f>& offsetsY() const {return offsetsY_;}

  /// sample weights
  const std::vector<float>& weights() const {return weights_;}

protected:
  
  virtual GLSL::Program* setupPass(int _pass, GLuint _srcTex);

  void updateKernel();

private:

  int radius_,
    samples_;

  /// blur std
  float sigma_;

  /// shader macros
  QStringList macros_;

  /// filter taps
  std::vector<ACG::Vec2f> offsetsX_;
  std::vector<ACG::Vec2f> offsetsY_;

  /// kernel weights
  std::vector<float> weights_;
};


// bilateral blur: gaussian blur with silhouette preservation
class ACGDLLEXPORT BilateralBlurFilter : public BaseSeparableFilterKernel
{
public:

  /* \brief Create bilateral filter
   *
   * @param _texWidth width of input texture
   * @param _texHeight height of input texture
   * @param _blurRadius radius in pixel coords of the blur kernel
   * @param _blurSigmaS blur smoothness in spatial distance (here distance in pixel coords)
   * @param _blurSigmaR blur smoothness in range distance (here linear view space difference)
   * @param _internalfmt internal gl format of the texture
  */
  BilateralBlurFilter(int _texWidth, int _texHeight, int _blurRadius, float _blurSigmaS = 1.0f, float blurSigmaR = 1.0f, GLenum _internalfmt = GL_RGBA);

  /// Class destructor
  virtual ~BilateralBlurFilter();


  /* \brief Set dynamic params before calling execute()
   *
   * @param _proj projection matrix
   * @param _depthTex depthbuffer texture (nonlinear depths)
  */
  void setParams(const ACG::GLMatrixf& _proj, GLuint _depthTex)
  {
    proj_ = _proj;
    depthTex_ = _depthTex;
  }


  /* \brief Change kernel settings
   *
   * @param _blurRadius new radius
   * @param _blurSigmaS new spatial smoothness
   * @param _blurSigmaR new range smoothness
  */
  void setKernel(int _blurRadius, float _blurSigmaS, float _blurSigmaR);


  /// radius
  int radius() const {return radius_;}

  /// number of samples
  int samples() const {return samples_;}

  /// blur (sigmaS, sigmaR)
  const ACG::Vec2f& sigma() const {return sigma_;}

  /// sample offsets along x direction
  const std::vector<ACG::Vec2f>& offsetsX() const {return offsetsX_;}

  /// sample offsets along y direction
  const std::vector<ACG::Vec2f>& offsetsY() const {return offsetsY_;}

protected:
  virtual GLSL::Program* setupPass(int _pass, GLuint _srcTex);

  void updateKernel();

private:

  int radius_,
    samples_;

  /// (sigmaS, sigmaR)
  ACG::Vec2f sigma_;

  /// -1 / (2 * sigma^2)
  ACG::Vec2f sigma2Rcp_;

  /// filter taps
  std::vector<ACG::Vec2f> offsetsX_;
  std::vector<ACG::Vec2f> offsetsY_;

  /// precomputed sample -r^2 / (2 * sigma_s^2)
  std::vector<float> spatialKernel_;

  /// shader macros
  QStringList macros_;

  ACG::GLMatrixf proj_;
  GLuint depthTex_;
};


class ACGDLLEXPORT RadialBlurFilter
{
public:

  /* \brief Create radial blur filter
   *
   * @param _numSamples number of kernel samples
  */
  RadialBlurFilter(int _numSamples);

  /// Class destructor
  virtual ~RadialBlurFilter() {}

  /* \brief Perform radial blur
   *
   * Writes to currently bound render target.
   * A good intensity range for light to very strong blur is [0.0025, 0.01].
   *
   * @param _srcTexture input 2d texture
   * @param _blurRadius blur radius in uv space
   * @param _blurIntensity intensity, quadratic distance factor
   * @param _blurCenter center in uv space
  */
  bool execute(GLuint _srcTexture, float _blurRadius = 1.0f, float _blurIntensity = 0.0025f, const ACG::Vec2f& _blurCenter = ACG::Vec2f(0.5f, 0.5f));


  /* \brief Change kernel settings
   *
   * @param _numSamples new sample count
  */
  void setKernel(int _numSamples);

  /// number of samples
  int samples() const {return samples_;}

private:

  int samples_;

  QStringList macros_;
};


class ACGDLLEXPORT PoissonBlurFilter
{
public:

  /* \brief Create poisson blur filter
   *
   * @param _radius radius of poisson disk
   * @param _sampleDistance min distance between two samples
   * @param _numTries number of tries per sample to find the next sample
  */
  PoissonBlurFilter(float _radius, float _sampleDistance, int _numTries = 30);

  /// Class destructor
  virtual ~PoissonBlurFilter();

  /* \brief Perform poisson blur
   *
   * Writes to currently bound render target.
   * 
   * @param _srcTex input 2d texture
   * @param _kernelScale kernel radius scaling factor
  */
  bool execute(GLuint _srcTex, float _kernelScale = 1.0f);


  /// radius
  float radius() const {return radius_;}

  /// number of samples
  int numSamples() const {return int(samples_.size());}

  /// min distance between two samples
  float sampleDistance() const {return sampleDistance_;}

  /// number of iterations per sample
  int numTries() const {return numTries_;}

  /// disk sample offsets
  const std::vector<ACG::Vec2f>& samples() const {return samples_;}

  /// dump samples as point cloud in obj format
  void dumpSamples(const char* _filename);

private:

  // sampling settings
  float radius_;
  float sampleDistance_;
  int numTries_;

  // poisson disk
  std::vector<ACG::Vec2f> samples_;

  // scaled samples
  std::vector<ACG::Vec2f> samplesScaled_;

  // shader macros
  QStringList macros_;
};



}