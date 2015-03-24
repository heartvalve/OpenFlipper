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
#include <ACG/GL/GLError.hh>
#include <ACG/GL/AntiAliasing.hh>

#include <OpenFlipper/common/RendererInfo.hh>
#include <OpenFlipper/BasePlugin/PostProcessorInterface.hh>


#include "PostProcessing.hh"




PostProcessing::PostProcessing()
  : debugLevel_(10), backbufferFBO_(0), backbufferTarget_(GL_BACK), stereoMode_(false)
{
  backbufferViewport_[0] = backbufferViewport_[1] = backbufferViewport_[2] = backbufferViewport_[3] = 0;
}


PostProcessing::~PostProcessing()
{

}


int PostProcessing::setupScene( int _viewerID, int _width, int _height, int _samples /*= 0*/, int _stereoEye /*= -1*/ )
{
  /*
  try to keep memory usage minimal
  -> prefer unorm over fp format
  */

  const int numProcs = postProcessorManager().numActive(_viewerID);

  if (!numProcs && _stereoEye < 0)
    return -1; // nothing to do

  /* create enough textures for the postprocessing chain:

  - Scene Rendering pass: 1 FBO, maybe multisampled
  - Resolve MSAA: 1 FBO, non msaa target,  post processor input,  can be skipped if multisampling is disabled
  - 1st PostProc:
            if no more following PostProcs, write to hw fbo
            otherwise, write to temp fbo

  - 2nd PostProc:
            write to 


  At least 2 FBO's required, 3 for multiple PostProcs.

  */


  // All render targets have the same format for now.
  // If one PostProc requires floating point, then all PostProcs work on fp data.

  // Search for a post proc that requires floating pt.
  bool requiresFP = false;

  for (int i = 0; i < numProcs && !requiresFP; ++i) 
  {
    PostProcessorInfo* pi = postProcessorManager().active(_viewerID, i);

    if (pi && pi->plugin)
    {
      PostProcessorFormatDesc fmt;
      fmt.outputFormat_ = PostProcessorFormatDesc::PostProcessorFormat_DONTCARE;
      pi->plugin->getFormatDesc(&fmt);

      if (!fmt.inputFormats_.empty())
      {
        size_t numInputs = fmt.inputFormats_.size();

        for (size_t k = 0; k < numInputs; ++k)
        {
          if (fmt.inputFormats_[k] == PostProcessorFormatDesc::PostProcessorFormat_FLOAT)
          {
            requiresFP = true;
            break;
          }
        }
      }

      if (fmt.outputFormat_ == PostProcessorFormatDesc::PostProcessorFormat_FLOAT)
        requiresFP = true;
    }
  }

  // avoid dealing with multisampling texture type for only one sample
  if (_samples < 2)
    _samples = 0;

  // number of eyes from which to scene is rendered
  const int numEyes = _stereoEye < 0 ? 1 : 2;

  GLuint texInternalFmt = requiresFP ? GL_RGBA32F : GL_RGBA;

  // allocate enough FBOs for the pipeline
  if (_stereoEye < 1)
  {
    // save output target FBO
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, (GLint*)&backbufferFBO_);
    glGetIntegerv(GL_DRAW_BUFFER, (GLint*)&backbufferTarget_);
    glGetIntegerv(GL_VIEWPORT, backbufferViewport_);

    // in stereo mode, allocation for both eyes is done while setting up the left eye

    // alloc/resize scene targets
    for (int i = 0; i < numEyes; ++i)
      setupFBO(sceneFBO_ + i, _width, _height, texInternalFmt, _samples);

    // alloc/resize processing double buffer

    int numTempBuffers = std::min(numProcs - 1, 2);

    if (_samples > 0) // resolve msaa into temp buffer, requires additional step
      numTempBuffers = std::min(numProcs, 2);

    for (int i = 0; i < numTempBuffers; ++i)
      setupFBO(procFBO_ + i, _width, _height, texInternalFmt, 0);

    // alloc/resize stereo final targets
    for (int i = 0; i < (_stereoEye >= 0 ? 2 : 0); ++i)
      setupFBO(stereoFBO_ + i, _width, _height, texInternalFmt, 0);
  }


  // bind scene render target FBO
  int targetFboId = std::max(std::min(_stereoEye, 1), 0); // clamp(eye, 0, 1)
  sceneFBO_[targetFboId].bind();
  glDrawBuffer(GL_COLOR_ATTACHMENT0);
  glViewport(0, 0, _width, _height);

  // clear fbo
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // save stereo setting
  stereoMode_ = _stereoEye >= 0;

  // enable multisample FBO
  if (_samples > 0)
    glEnable(GL_MULTISAMPLE);

  return 1;
}


void PostProcessing::setupFBO( ACG::FBO* _dst, int _width, int _height, GLuint _intfmt , int _samples )
{
  // render to multisampled texture -> no texfilter!
  GLuint filterMode = _samples ? GL_NEAREST : GL_LINEAR;

  if (!_dst->getFboID())
  {
    // alloc
    _dst->setMultisampling(_samples);
    _dst->attachTexture2D(GL_COLOR_ATTACHMENT0, _width, _height, _intfmt, GL_RGBA, GL_CLAMP, filterMode, filterMode);

    // reading from GL_DEPTH24_STENCIL8 is buggy on ati
    _dst->attachTexture2DDepth(_width, _height, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL);

    // GL_DEPTH_COMPONENT32: has weird effect on color buffer - colors are darker!
    //  reading from depth works, but no stencil support
//    _dst->attachTexture2DDepth(_width, _height, GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT);

    // GL_DEPTH_COMPONENT: same as GL_DEPTH_COMPONENT32
//    _dst->attachTexture2DDepth(_width, _height, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT);

    // colors are darker only in classical renderer!!
  }
  else
  {
    // resize
    _dst->setMultisampling(_samples);
    _dst->resize(_width, _height);

    // reformat
    if (_dst->getInternalFormat(GL_COLOR_ATTACHMENT0) != _intfmt)
      _dst->attachTexture2D(GL_COLOR_ATTACHMENT0, _width, _height, _intfmt, GL_RGBA, GL_CLAMP, filterMode, filterMode);
  }
}


void PostProcessing::postProcess( int _viewerID, ACG::GLState* _glstate, const ACG::GLMatrixd& _modelview, const ACG::GLMatrixd& _proj1, const ACG::GLMatrixd& _proj2, bool _hwOpenGLStereo )
{
  // disable multisample FBO
  if (sceneFBO_[0].getMultisamplingCount())
    glDisable(GL_MULTISAMPLE);

  // 1st post processing source: active fbo
  int postProcSrc = 1;
  PostProcessorInput postProcInput;

  std::vector<const PostProcessorInput*> postProcInputVec;
  postProcInputVec.push_back(&postProcInput);

  // # post processors in chain
  const int numProcs = postProcessorManager().numActive(_viewerID);

  // # executions of postproc chain, one for each eye
  int numChainExecs = stereoMode_ ? 2 : 1;

  // use GL_LEQUAL for depth testing, restore current depth func afterwards
  GLint saveDepthFunc = GL_LESS;
  glGetIntegerv(GL_DEPTH_FUNC, &saveDepthFunc);

  glDepthMask(1);
  glColorMask(1,1,1,1);


  // execute

  for (int chainId = 0; chainId < numChainExecs; ++chainId) {

    // 1. input: scene render target

    postProcInput.width     = sceneFBO_[chainId].width();
    postProcInput.height    = sceneFBO_[chainId].height();

    postProcInput.depthRange_[0] = 0.0f;
    postProcInput.depthRange_[1] = 1.0f;

    postProcInput.view_ = _modelview;
    postProcInput.proj_ = chainId ? _proj2 : _proj1;

    // resolve multisampling before post processing
    if (sceneFBO_[chainId].getMultisamplingCount())
    {
      // resolve into first temp target or into stereo output (no postprocs, but stereo)
      ACG::FBO* resolveTarget = numProcs ? &procFBO_[0] : &stereoFBO_[chainId];

      if (numProcs || (stereoMode_ && !_hwOpenGLStereo))
        resolveMultisampling(resolveTarget, &sceneFBO_[chainId]);

      // use resolved texture as input for processing
      postProcInput.colorTex_ = resolveTarget->getAttachment(GL_COLOR_ATTACHMENT0);
      postProcInput.depthTex_ = resolveTarget->getAttachment(GL_DEPTH_ATTACHMENT);

      postProcInput.texfmt_ = resolveTarget->getInternalFormat(GL_COLOR_ATTACHMENT0);

      // read from procFBO_[0], write to procFBO_[1]
      postProcSrc = 0;
    }
    else
    {
      // read directly from scene fbo
      postProcInput.colorTex_ = sceneFBO_[chainId].getAttachment(GL_COLOR_ATTACHMENT0);
      postProcInput.depthTex_ = sceneFBO_[chainId].getAttachment(GL_DEPTH_ATTACHMENT);

      postProcInput.texfmt_ = sceneFBO_[chainId].getInternalFormat(GL_COLOR_ATTACHMENT0);

      // write to procFBO_[0]
      postProcSrc = 1;
    }


    // execute post processing chain with 2 FBOs
    for (int i = 0; i < numProcs; ++i)  {

      int postProcTarget = 1 - postProcSrc;

      // specify output target of the next post processor
      GLuint outputFBO = procFBO_[postProcTarget].getFboID();
      GLuint outputDrawbuffer = GL_COLOR_ATTACHMENT0;
      GLint outputViewport[4] = {0, 0, procFBO_[postProcTarget].width(), procFBO_[postProcTarget].height()};

      const GLint* outputViewportPtr = outputViewport;

      // write to stereo or back buffer in last step
      if (i + 1 == numProcs) 
      {
        if (stereoMode_) 
        {
          if (_hwOpenGLStereo)
          {
            outputFBO = backbufferFBO_;
            outputDrawbuffer = chainId ? GL_BACK_RIGHT : GL_BACK_LEFT;

            outputViewportPtr = backbufferViewport_;
          }
          else
          {
            outputFBO = stereoFBO_[chainId].getFboID();
            outputDrawbuffer = GL_COLOR_ATTACHMENT0;

            outputViewport[0] = outputViewport[1] = 0;
            outputViewport[2] = stereoFBO_[chainId].width();
            outputViewport[3] = stereoFBO_[chainId].height();
          }
        }
        else 
        {
          outputFBO = backbufferFBO_;
          outputDrawbuffer = backbufferTarget_;
          outputViewportPtr = backbufferViewport_;
        }
      }

      PostProcessorOutput postProcOutput(outputFBO, outputDrawbuffer,
        procFBO_[postProcTarget].width(), procFBO_[postProcTarget].height(), outputViewportPtr);

      // apply post processor
      PostProcessorInfo* proc = postProcessorManager().active( _viewerID, i );
      if (proc && proc->plugin)
      {
        // set default opengl states
        glDepthMask(1);
        glColorMask(1,1,1,1);

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);
        glDisable(GL_BLEND);
        glDepthFunc(GL_LEQUAL);

        proc->plugin->postProcess(_glstate, postProcInputVec, postProcOutput);

        // make sure that post processor writes to specified output
        if (debugLevel_ > 0)
        {
          GLint testFBO = 0, testDrawbuffer = 0;
          GLint testVp[4];

          glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &testFBO);
          glGetIntegerv(GL_DRAW_BUFFER, &testDrawbuffer);
          glGetIntegerv(GL_VIEWPORT, testVp);

          if (GLuint(testFBO) != postProcOutput.fbo_)
            std::cerr << "Error: PostProcessor does not write to specified FBO: " << proc->plugin->postProcessorName().toStdString() << std::endl;

          if (GLuint(testDrawbuffer) != postProcOutput.drawBuffer_)
            std::cerr << "Error: PostProcessor does not write to specified draw-buffer: " << proc->plugin->postProcessorName().toStdString() << std::endl;

          if (testVp[0] != postProcOutput.viewport_[0] || testVp[1] != postProcOutput.viewport_[1] || 
            testVp[2] != postProcOutput.viewport_[2] || testVp[3] != postProcOutput.viewport_[3])
            std::cerr << "Error: PostProcessor does not write to specified viewport: " << proc->plugin->postProcessorName().toStdString() 
                      << " actual: (" << testVp[0] << " " << testVp[1] << " " << testVp[2] << " " << testVp[3] << ") expected: (" 
                      <<  postProcOutput.viewport_[0] << " " << postProcOutput.viewport_[1] << " " << postProcOutput.viewport_[2] << " " << postProcOutput.viewport_[3] << ")"   << std::endl;
        }
      }

      // swap target/source fbo
      postProcSrc = postProcTarget;

      postProcInput.colorTex_ = procFBO_[postProcSrc].getAttachment(GL_COLOR_ATTACHMENT0);
    }

  }


  // restore backbuffer
  glBindFramebuffer(GL_FRAMEBUFFER, backbufferFBO_);
  glDrawBuffer(backbufferTarget_);
  glViewport(backbufferViewport_[0], backbufferViewport_[1], backbufferViewport_[2], backbufferViewport_[3]);

  // restore depth func
  glDepthFunc(saveDepthFunc);
}

void PostProcessing::resolveMultisampling( ACG::FBO* _dst, ACG::FBO* _src )
{
  const int resolveMethod = 0;

  GLint curFbo = 0;
  glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &curFbo);

  if (resolveMethod == 0)
  {
    // resolve by blitting

    glBindFramebuffer(GL_READ_FRAMEBUFFER, _src->getFboID());
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _dst->getFboID());

    if (debugLevel_ > 0)
    {
      if (_src->size() != _dst->size())
        std::cerr << "PostProcessing::resolveMultisampling - FBO size mismatch : src (" 
          << _src->width() << " " << _src->height() << ") dst ("
          << _dst->width() << " " << _dst->height() << ")" << std::endl;
    }

    glBlitFramebuffer(0, 0, _src->width(), _src->height(),
      0, 0, _dst->width(), _dst->height(),
      GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);

  }
  else
  {
    // resolve manually
    _dst->bind();
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    GLuint multisampledColorTex = _src->getAttachment(GL_COLOR_ATTACHMENT0);
    GLuint multisampledDepthTex = _src->getAttachment(GL_DEPTH_ATTACHMENT);
    glDepthFunc(GL_ALWAYS);
    ACG::MSTextureSampler::filterMSAATexture_Nearest(multisampledColorTex, multisampledDepthTex, _src->getMultisamplingCount());
    glDepthFunc(GL_LEQUAL);
  }

  checkGLError();

  glBindFramebuffer(GL_FRAMEBUFFER, curFbo);

}


void PostProcessing::resolveStereoAnyglyph(int _viewerID)
{
  PostProcessorInfo* procAnaglyph = postProcessorManager().getPostProcessor("Anaglyph Stereo Postprocessor Plugin");

  if (procAnaglyph) 
  {
    const int numProcs = postProcessorManager().numActive(_viewerID);

    PostProcessorInput inputs[2];

    for (int i = 0; i < 2; ++i)
    {
      // if the scene is multisampled or at least one post processor is active, read from stereoFBO_
      // otherwise, read from the non-multisampled and unprocessed sceneFBO_
      ACG::FBO* eyeSrcFBO = &stereoFBO_[i];
      
      if (!numProcs && !sceneFBO_[i].getMultisamplingCount())
        eyeSrcFBO = &sceneFBO_[i];

      inputs[i].colorTex_ = eyeSrcFBO->getAttachment(GL_COLOR_ATTACHMENT0);
      inputs[i].depthTex_ = eyeSrcFBO->getAttachment(GL_DEPTH_ATTACHMENT);
      inputs[i].width = eyeSrcFBO->width();
      inputs[i].height = eyeSrcFBO->height();

      inputs[i].texfmt_ = eyeSrcFBO->getInternalFormat(GL_COLOR_ATTACHMENT0);
    }


    std::vector<const PostProcessorInput*> anaglyphInputVec(2);
    anaglyphInputVec[0] = inputs;
    anaglyphInputVec[1] = inputs + 1;

    PostProcessorOutput anaglyphOutput(backbufferFBO_, 
      backbufferTarget_,
      inputs[0].width, inputs[0].height, backbufferViewport_);

    procAnaglyph->plugin->postProcess(0, anaglyphInputVec, anaglyphOutput);
  }
  else
    std::cerr << "error: stereo anaglyph plugin missing!" << std::endl;
}

