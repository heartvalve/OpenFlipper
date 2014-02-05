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


#include "SSAO.hh"
#include <OpenFlipper/common/GlobalOptions.hh>
#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include <ACG/GL/GLError.hh>

#undef QT_NO_OPENGL
#include <QGLFormat>
#define QT_NO_OPENGL

// shader debug mode triggers a shader reload after resizing the view window
//#define SSAO_SHADER_DEBUG_MODE

const unsigned int SSAOPlugin::numSamples_ = 32;


SSAOPlugin::SSAOPlugin() :
    randomVecTex_(0)
{
  for (unsigned int i = 0; i < 10; ++i)
    shaders_[i] = 0;

  for (unsigned int i = 0; i < 6; ++i)
    programs_[i] = 0;
}

SSAOPlugin::~SSAOPlugin()
{

}


SSAOPlugin::ViewerResources::ViewerResources()
{
  memset(this, 0, sizeof(ViewerResources));
}

void SSAOPlugin::initializePlugin()
{
  memset(shaders_, 0, sizeof(shaders_));
  memset(programs_, 0, sizeof(programs_));

  randomVecTex_ = 0;

  generateSamplingKernel();
}

//////////////////////////////////////////////////////////////////////////

void SSAOPlugin::generateSamplingKernel()
{
  for (unsigned int i = 0; i < numSamples_; ++i)
  {
    ACG::Vec3f r; // get 3 random floats in [-0.5, 0.5]
    for (int k = 0; k < 3; ++k)
    {
      unsigned int x = (rand()*rand()*rand()) & RAND_MAX;
      r[k] = float(x) / float(RAND_MAX); // [0, 1]
      r[k] -= 0.5f;
    }
    // sphere to hemisphere
    r[2] = fabsf(r[2]);

    r.normalize();

    // more samples near the fragment
    // compute a sample distance accordingly
    float d = float(i+1) / float(numSamples_);
    d *= d;
    if (d < 0.1f) d = 0.1f;

    r *= d;

    samplingKernel_[i] = r;
  }
}

//////////////////////////////////////////////////////////////////////////

void SSAOPlugin::exit()
{
  destroyResources();
}

//////////////////////////////////////////////////////////////////////////

QString SSAOPlugin::rendererName() {
  return QString("SSAO Renderer");
}

//////////////////////////////////////////////////////////////////////////

void SSAOPlugin::supportedDrawModes(ACG::SceneGraph::DrawModes::DrawMode& _mode) 
{
  _mode =  ACG::SceneGraph::DrawModes::DEFAULT;
}

//////////////////////////////////////////////////////////////////////////

void SSAOPlugin::reloadResources(int _viewerId, unsigned int _sceneTexWidth, unsigned int _sceneTexHeight) 
{
  ViewerResources* p = &viewerRes_[_viewerId];

  // save window size
  unsigned int vpWidth = p->glWidth_, vpHeight = p->glHeight_;
  if (!p->glWidth_ || !p->glHeight_) return;

  destroyResources(_viewerId);

  p->glWidth_ = vpWidth;
  p->glHeight_ = vpHeight;

  p->rtWidth_ = p->glWidth_;
  p->rtHeight_ = p->glHeight_;

  p->rtDownWidth_ = p->rtWidth_ / 2;
  p->rtDownHeight_ = p->rtHeight_ / 2;

  p->rtSceneWidth_ = _sceneTexWidth;
  p->rtSceneHeight_ = _sceneTexHeight;
  
  // the scene texture contains the color result of a standard scene render pass
  // format: R8G8B8A8
  glGenTextures(1, &p->sceneBufTex_);
  ACG::GLState::bindTexture(GL_TEXTURE_2D, p->sceneBufTex_);
  // texture access: clamped
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  // filter: none (1 to 1 mapping in final pass)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, p->rtSceneWidth_, p->rtSceneHeight_, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

  // depth buf render texture
  // format: R32F, maybe change to R16F if it works ok
  glGenTextures(1, &p->depthBufTex_);
  ACG::GLState::bindTexture(GL_TEXTURE_2D, p->depthBufTex_);
  // texture access: clamped
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  // filter: linear
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, p->glWidth_, p->glHeight_, 0, GL_RGB, GL_FLOAT, 0);

  // scene normals
  glGenTextures(1, &p->sceneNormalTex_);
  ACG::GLState::bindTexture(GL_TEXTURE_2D, p->sceneNormalTex_);
  // texture access: clamped
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  // filter: linear
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, p->glWidth_, p->glHeight_, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

  // downsampled render textures
  // format: R32F
  for (int i = 0; i < 2; ++i)
  {
    glGenTextures(1, i ? (&p->downsampledTex_) : (&p->downsampledTmpTex_));
    ACG::GLState::bindTexture(GL_TEXTURE_2D, i ? (p->downsampledTex_) : (p->downsampledTmpTex_));
    // texture access: clamped
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // filter: linear
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, p->rtDownWidth_, p->rtDownHeight_, 0, GL_RGB, GL_FLOAT, 0);
  }

  glGenTextures(1, &p->occlusionTex_);
  ACG::GLState::bindTexture(GL_TEXTURE_2D, p->occlusionTex_);
  // texture access: clamped
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  // filter: linear
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, p->glWidth_, p->glHeight_, 0, GL_RGB, GL_FLOAT, 0);

  // end of texture creation
  ACG::GLState::bindTexture(GL_TEXTURE_2D, 0);

  ACG::glCheckErrors();

  // create depth render buffer
  glGenRenderbuffersEXT(1, &p->depthSSAORenderBuf_);
  glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, p->depthSSAORenderBuf_);
  glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, p->glWidth_, p->glHeight_);
  glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
  
  // initialize the fbo
  glGenFramebuffersEXT(1, &p->ssaoFbo_);
  ACG::GLState::bindFramebuffer(GL_FRAMEBUFFER_EXT, p->ssaoFbo_);

  // color_attachment order:
  // scene color, depth, scene normals, occlusion
  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, p->depthBufTex_, 0);
  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_TEXTURE_2D, p->sceneNormalTex_, 0);
  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT2_EXT, GL_TEXTURE_2D, p->occlusionTex_, 0);
  glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, p->depthSSAORenderBuf_);

  GLenum fboStatus = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
  if(fboStatus != GL_FRAMEBUFFER_COMPLETE_EXT)
    printf("SSAO Plugin: ssaoFbo failed to initialize\n");


  glGenFramebuffersEXT(1, &p->sceneFbo_);
  ACG::GLState::bindFramebuffer(GL_FRAMEBUFFER_EXT, p->sceneFbo_);
  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, p->sceneBufTex_, 0);

  if (p->rtSceneWidth_ > p->rtWidth_ || p->rtSceneHeight_ > p->rtHeight_)
  {
    // use new depth buffer for multisampling
    glGenRenderbuffersEXT(1, &p->depthSceneRenderBuf_);
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, p->depthSceneRenderBuf_);
    glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, p->rtSceneWidth_, p->rtSceneHeight_);
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
   
    glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, p->depthSceneRenderBuf_);
  }
  else
    glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, p->depthSSAORenderBuf_);

  fboStatus = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
  if(fboStatus != GL_FRAMEBUFFER_COMPLETE_EXT)
    printf("SSAO Plugin: sceneFbo failed to initialize\n");


  glGenFramebuffersEXT(1, &p->blurFbo_);
  ACG::GLState::bindFramebuffer(GL_FRAMEBUFFER_EXT, p->blurFbo_);
  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, p->downsampledTex_, 0);
  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_TEXTURE_2D, p->downsampledTmpTex_, 0);


  fboStatus = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
  if(fboStatus != GL_FRAMEBUFFER_COMPLETE_EXT)
    printf("SSAO Plugin: blurFbo failed to initialize\n");


  ACG::GLState::bindFramebuffer(GL_FRAMEBUFFER_EXT, 0);


  // load shaders

  const char* ShaderFiles[] = {"SSAO/init_vertex.glsl",
    "SSAO/fullscreen_vertex.glsl",
    "SSAO/init_fragment.glsl",
    "SSAO/downsampling_fragment.glsl",
    "SSAO/blur_fragment.glsl",
    "SSAO/ssao_fragment.glsl",
    "SSAO/final_fragment.glsl",
    "SSAO/final_MSAA_vertex.glsl",
    "SSAO/final_MSAA_fragment.glsl"};

  for (int i = 0; i < 9; ++i)
  {
    QString shaderFile = OpenFlipper::Options::shaderDirStr() + QDir::separator() + QString(ShaderFiles[i]);

#ifdef SSAO_SHADER_DEBUG_MODE
    delete shaders_[i];
#else
    if (shaders_[i]) continue;
#endif

    if (i < 2 || i == 7) // first two are vertex shaders
      shaders_[i] = GLSL::loadVertexShader(shaderFile.toUtf8());
    else
      shaders_[i] = GLSL::loadFragmentShader(shaderFile.toUtf8());

    if (!shaders_[i])
    {
      log(LOGERR, QString(ShaderFiles[i]) + QString(" could not be loaded and compiled"));
      return;
    }
  }

  // all needed glprograms
  for (int i = 0; i < 6; ++i)
  {
#ifndef SSAO_SHADER_DEBUG_MODE
    if (!programs_[i])
#endif
    {
      delete programs_[i];
      programs_[i] = new GLSL::Program();
      GLSL::Program* pr = programs_[i];

      switch (i)
      {
      case PROG_INIT:
        pr->attach(shaders_[0]);
        pr->attach(shaders_[2]); break;

      case PROG_DOWNSAMPLING:
        pr->attach(shaders_[1]);
        pr->attach(shaders_[3]); break;

      case PROG_BLUR:
        pr->attach(shaders_[1]);
        pr->attach(shaders_[4]); break;

      case PROG_SSAO:
        pr->attach(shaders_[1]);
        pr->attach(shaders_[5]); break;

      case PROG_FINAL:
        pr->attach(shaders_[1]);
        pr->attach(shaders_[6]); break;

      case PROG_FINAL_MSAA:
        pr->attach(shaders_[7]);
        pr->attach(shaders_[8]); break;
      }

      pr->link();
    }
  }

  if (!randomVecTex_)
  {
    // random vector texture
    glGenTextures(1, &randomVecTex_);
    ACG::GLState::bindTexture(GL_TEXTURE_2D, randomVecTex_);
    // texture access: wrapped
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // filter: none
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    ACG::Vec4f randVecs[16];
    for (int i = 0; i < 16; ++i)
    {
      ACG::Vec3f x;
      for (int k = 0; k < 3; ++k)
        x[k] = float(rand()) / float(RAND_MAX);

      float theta = x[0] * 6.2831853f; // 2pi
      float phi = x[1] * 6.2831853f;
      randVecs[i][0] = sinf(phi);
      randVecs[i][1] = cosf(phi);
      randVecs[i][2] = sinf(theta);
      randVecs[i][3] = cosf(theta);
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F_ARB, 4, 4, 0, GL_RGBA, GL_FLOAT, randVecs);
  }
  
  ACG::GLState::bindTexture(GL_TEXTURE_2D, 0);

  ACG::glCheckErrors();
}

//////////////////////////////////////////////////////////////////////////

void SSAOPlugin::destroyResources() {

  for (unsigned int i = 0; i < sizeof(programs_) / sizeof(programs_[0]); ++i)
  {
    delete programs_[i]; programs_[i] = 0;
  }

  for (unsigned int i = 0; i < sizeof(shaders_) / sizeof(shaders_[0]); ++i)
  {
    delete shaders_[i];
    shaders_[i] = 0;
  }

  if (randomVecTex_) glDeleteTextures(1, &randomVecTex_);
  randomVecTex_ = 0;

  // free all viewer specific resources
  std::map<int, ViewerResources>::iterator resIt = viewerRes_.begin();
  for (; resIt != viewerRes_.end(); ++resIt)
    destroyResources(resIt->first);
}

//////////////////////////////////////////////////////////////////////////

void SSAOPlugin::destroyResources(int _viewerId)
{
  ViewerResources* p = &viewerRes_[_viewerId];

  if (p->sceneFbo_) glDeleteFramebuffersEXT(1, &p->sceneFbo_);
  if (p->ssaoFbo_) glDeleteFramebuffersEXT(1, &p->ssaoFbo_);
  if (p->blurFbo_) glDeleteFramebuffersEXT(1, &p->blurFbo_);

  if (p->depthSSAORenderBuf_) glDeleteRenderbuffersEXT(1, &p->depthSSAORenderBuf_);
  
  if (p->depthSceneRenderBuf_) glDeleteRenderbuffersEXT(1, &p->depthSceneRenderBuf_);

  if (p->sceneBufTex_) glDeleteTextures(1, &p->sceneBufTex_);
  if (p->depthBufTex_) glDeleteTextures(1, &p->depthBufTex_);
  if (p->downsampledTex_) glDeleteTextures(1, &p->downsampledTex_);
  if (p->downsampledTmpTex_) glDeleteTextures(1, &p->downsampledTmpTex_);
  if (p->occlusionTex_) glDeleteTextures(1, &p->occlusionTex_);
  if (p->sceneNormalTex_) glDeleteTextures(1, &p->sceneNormalTex_);

  // zero out
  memset(p, 0, sizeof(ViewerResources));
}

//////////////////////////////////////////////////////////////////////////

void SSAOPlugin::drawQuadProj(float x0, float y0, float w, float h)
{
  // quad in projection space
  // here only position are passed to GL
  // tex-coords can be generated in a vertex-shader as follows
  //  uv = pos * (.5, -.5) + (.5, .5)

  glBegin(GL_QUADS);
  {
    glVertex2f(x0,  y0);
    glVertex2f(x0, y0-h);
    glVertex2f(x0+w, y0-h);
    glVertex2f(x0+w, y0);
  }
  glEnd();
}

//////////////////////////////////////////////////////////////////////////

void SSAOPlugin::drawScenePass(ACG::GLState* _glState, Viewer::ViewerProperties& _properties, BaseNode* _sceneGraphRoot)
{
  ACG::SceneGraph::DrawAction action(_properties.drawMode(), *_glState, false);
  ACG::SceneGraph::traverse_multipass(_sceneGraphRoot, action, *_glState, _properties.drawMode());
}

//////////////////////////////////////////////////////////////////////////

void SSAOPlugin::gaussianBlurPass(const ViewerResources* _pViewer, const float* _texelSize, 
                                  GLenum _targetAttachement, GLuint _srcTexture)
{
  // standard deviation for gaussian blur filter
  const float gaussStDev = 1.0f;

  ACG::GLState::drawBuffer(_targetAttachement);

  float gaussKernel[5];
  float sum = 0.0f; // sum of kernel tabs
  for (int i = 0; i < 5; ++i)
  {
    // 1 / (4 pi s^2) e^(-x^2 / s^2 ),  constant factor useless here
    gaussKernel[i] = powf(2.71828f, -float(i*i)*(_texelSize[0]*_texelSize[0] + _texelSize[1]*_texelSize[1]) /
      (gaussStDev*gaussStDev));
    sum += gaussKernel[i];
  }
  // normalize kernel
  for (int i = 0; i < 5; ++i)
    gaussKernel[i] /= sum;

  ACG::GLState::activeTexture(GL_TEXTURE0);
  ACG::GLState::bindTexture(GL_TEXTURE_2D, _srcTexture);

  programs_[PROG_BLUR]->setUniform("Tex", 0);
  programs_[PROG_BLUR]->setUniform("TexelSize", ACG::Vec2f(_texelSize));
  programs_[PROG_BLUR]->setUniform("Kernel", gaussKernel, 5);

  drawQuadProj();
}

//////////////////////////////////////////////////////////////////////////

void SSAOPlugin::render(ACG::GLState* _glState, Viewer::ViewerProperties& _properties)
{
  glPushAttrib(GL_ALL_ATTRIB_BITS);

  const GLuint targetFbo = ACG::GLState::getFramebufferDraw();

  int viewerId = _properties.viewerId();

  ViewerResources* pViewer = &viewerRes_[viewerId];
  pViewer->glWidth_ = _glState->viewport_width();
  pViewer->glHeight_ = _glState->viewport_height();

  if (_properties.multisampling())
  {
    if ((pViewer->glWidth_ * 2 != pViewer->rtSceneWidth_) || (pViewer->glHeight_ * 2 != pViewer->rtSceneHeight_))
      reloadResources(viewerId, pViewer->glWidth_ * 2, pViewer->glHeight_ * 2);
  }
  else if ((pViewer->glWidth_ != pViewer->rtSceneWidth_) || (pViewer->glHeight_ != pViewer->rtSceneHeight_))
    reloadResources(viewerId, pViewer->glWidth_, pViewer->glHeight_);

  BaseNode* sceneGraphRoot = PluginFunctions::getSceneGraphRootNode();  

  GLenum drawBuffers[] = {GL_COLOR_ATTACHMENT0_EXT,
    GL_COLOR_ATTACHMENT1_EXT,
    GL_COLOR_ATTACHMENT2_EXT,
    GL_COLOR_ATTACHMENT3_EXT,
    GL_COLOR_ATTACHMENT4_EXT,
    GL_COLOR_ATTACHMENT5_EXT,
    GL_COLOR_ATTACHMENT6_EXT};

  // the farthest depth value possible in the depth buffer
  const float maxDepth = 1000.0f;

  GLint oldViewport[4];
  glGetIntegerv(GL_VIEWPORT, oldViewport);

  for (int i = 0; i < 6; ++i)
  {
    ACG::GLState::activeTexture(GL_TEXTURE0 + i);
    ACG::GLState::bindTexture(GL_TEXTURE_2D, 0);
  }

  float texelSize[4] = {1.0f / float(pViewer->rtWidth_), 1.0f / float(pViewer->rtHeight_), 0.0f, 0.0f};


  // ---------------------------------------------
  // 1. render scene with standard materials:
  glViewport(0, 0, pViewer->rtSceneWidth_, pViewer->rtSceneHeight_);

  ACG::GLState::bindFramebuffer(GL_FRAMEBUFFER_EXT, pViewer->sceneFbo_);
  ACG::GLState::drawBuffer(drawBuffers[0]); // scene buffer in render target 0
  glClearColor(_glState->clear_color()[0], _glState->clear_color()[1], _glState->clear_color()[2], 0);

  // NOTE: for some reason the early z pass optimization does not work here
  //  using the depth buffer from previous pass gives z fighting
  // early z cull optimization settings:
  //   ACG::GLState::enable(GL_DEPTH_TEST);
  //   ACG::GLState::depthFunc(GL_LEQUAL);
  //   ACG::GLState::lockDepthFunc();
  //   glDepthMask(GL_FALSE); // disable z writing
  //   glClear(GL_COLOR_BUFFER_BIT);

  // without early z cull:
  ACG::GLState::enable(GL_DEPTH_TEST);
  ACG::GLState::depthFunc(GL_LESS);
  glDepthMask(GL_TRUE);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  drawScenePass(_glState, _properties, sceneGraphRoot);

  ACG::GLState::unlockDepthFunc(); // unlock less-equal depth function


  if (pViewer->rtSceneWidth_ != pViewer->glWidth_ || pViewer->rtSceneHeight_ != pViewer->glHeight_)
    glViewport(0, 0, pViewer->glWidth_, pViewer->glHeight_);

  // ---------------------------------------------
  // 2. init depth and normal targets
  ACG::GLState::bindFramebuffer(GL_FRAMEBUFFER_EXT, pViewer->ssaoFbo_);
  ACG::GLState::enable(GL_DEPTH_TEST);
  ACG::GLState::depthFunc(GL_LESS);
  glDepthMask(GL_TRUE);

  // color attachment 0 and 1 stores the scene depth and normals
  // clear first
  ACG::GLState::drawBuffer(drawBuffers[0]);
  glClearColor(maxDepth, 0.0f, 0.0f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  ACG::GLState::drawBuffer(drawBuffers[1]);
  glClearColor(0.5f, 0.5f, 0.0f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  ACG::GLState::drawBuffers(2, drawBuffers);

  programs_[PROG_INIT]->use();
  drawScenePass(_glState, _properties, sceneGraphRoot);
  programs_[PROG_INIT]->disable();

  // ---------------------------------------------
  // 3. compute occlusion
  ACG::GLState::drawBuffer(drawBuffers[2]); // occlusion buffer in render target 2
  ACG::GLState::disable(GL_DEPTH_TEST);

  texelSize[0] = 1.0f / float(pViewer->rtWidth_);
  texelSize[1] = 1.0f / float(pViewer->rtHeight_);

  programs_[PROG_SSAO]->use();
  programs_[PROG_SSAO]->setUniform("TexelSize", ACG::Vec2f(texelSize[0], texelSize[1]));
  programs_[PROG_SSAO]->setUniform("ScreenSize", ACG::Vec2f(pViewer->rtWidth_, pViewer->rtHeight_));
  {
    GLint location = programs_[PROG_SSAO]->getUniformLocation("Kernel");
    glUniform3fv(location, 32, (GLfloat*)samplingKernel_);
  }

  programs_[PROG_SSAO]->setUniform("RandTex", 3);
  programs_[PROG_SSAO]->setUniform("NormalTex", 2);
  programs_[PROG_SSAO]->setUniform("SceneTex", 1);
  programs_[PROG_SSAO]->setUniform("DepthTex", 0);

  ACG::GLState::activeTexture(GL_TEXTURE3);
  ACG::GLState::bindTexture(GL_TEXTURE_2D, randomVecTex_);

  ACG::GLState::activeTexture(GL_TEXTURE2);
  ACG::GLState::bindTexture(GL_TEXTURE_2D, pViewer->sceneNormalTex_);

  ACG::GLState::activeTexture(GL_TEXTURE1);
  ACG::GLState::bindTexture(GL_TEXTURE_2D, pViewer->sceneBufTex_);

  ACG::GLState::activeTexture(GL_TEXTURE0);
  ACG::GLState::bindTexture(GL_TEXTURE_2D, pViewer->depthBufTex_);

  drawQuadProj();


  ACG::GLState::activeTexture(GL_TEXTURE2);
  ACG::GLState::bindTexture(GL_TEXTURE_2D, 0);

  // ---------------------------------------------
  // 4. downsample the occlusion texture to 1/4 its size
  glViewport(0, 0, pViewer->rtDownWidth_, pViewer->rtDownHeight_);

  ACG::GLState::bindFramebuffer(GL_FRAMEBUFFER_EXT, pViewer->blurFbo_);
  ACG::GLState::drawBuffer(drawBuffers[0]);
  // disable depth testing and writing from now on
  ACG::GLState::disable(GL_DEPTH_TEST);

  programs_[PROG_DOWNSAMPLING]->use();
  programs_[PROG_DOWNSAMPLING]->setUniform("TexelSize", ACG::Vec2f(texelSize));

  // bind depth rt
  ACG::GLState::activeTexture(GL_TEXTURE0);
  ACG::GLState::bindTexture(GL_TEXTURE_2D, pViewer->occlusionTex_);
  programs_[PROG_DOWNSAMPLING]->setUniform("Tex", 0);

  drawQuadProj();

  //-----------------------------------------
  // 5. gaussian blur filter

  programs_[PROG_BLUR]->use();
  programs_[PROG_BLUR]->setUniform("DepthTex", 1);
  programs_[PROG_BLUR]->setUniform("EdgeBlur", _properties.multisampling() ? 0.3f : 0.0f);
  ACG::GLState::activeTexture(GL_TEXTURE1);
  ACG::GLState::bindTexture(GL_TEXTURE_2D, pViewer->depthBufTex_);

  // horizontal
  texelSize[0] = 1.0f / float(pViewer->rtDownWidth_);
  texelSize[1] = 0.0f;

  gaussianBlurPass(pViewer, texelSize, drawBuffers[1], pViewer->downsampledTex_);

  // vertical
  texelSize[0] = 0.0f;
  texelSize[1] = 1.0f / float(pViewer->rtDownHeight_);

  gaussianBlurPass(pViewer, texelSize, drawBuffers[0], pViewer->downsampledTmpTex_);
  // blurred result in pViewer->downsampledTex_

  //-----------------------------------------
  // 6. final pass, present result
  glViewport(oldViewport[0], oldViewport[1], oldViewport[2], oldViewport[3]);
  ACG::GLState::bindFramebuffer(GL_FRAMEBUFFER_EXT, targetFbo);

  ACG::GLState::activeTexture(GL_TEXTURE1);
  ACG::GLState::bindTexture(GL_TEXTURE_2D, pViewer->downsampledTex_);

  ACG::GLState::activeTexture(GL_TEXTURE0);
  ACG::GLState::bindTexture(GL_TEXTURE_2D, pViewer->sceneBufTex_);

  GLSL::Program* finalProg = programs_[_properties.multisampling() ? PROG_FINAL_MSAA : PROG_FINAL];

  finalProg->use();
  finalProg->setUniform("OcclusionTex", 1);
  finalProg->setUniform("SceneTex", 0);
  if (_properties.multisampling())
    finalProg->setUniform("SceneTexelSize", ACG::Vec2f(1.0f / float(pViewer->rtSceneWidth_), 1.0f / float(pViewer->rtSceneHeight_)));

  drawQuadProj();

  finalProg->disable();
  //-----------------------------------------


  glPopAttrib();
}

QString SSAOPlugin::checkOpenGL() {

  // Get version and check
  QGLFormat::OpenGLVersionFlags flags = QGLFormat::openGLVersionFlags();
  if ( !flags.testFlag(QGLFormat::OpenGL_Version_3_2) )
    return QString("Insufficient OpenGL Version! OpenGL 3.2 or higher required");

  // Check extensions
  QString glExtensions = QString((const char*)glGetString(GL_EXTENSIONS));
  QString missing("");
  if ( !glExtensions.contains("GL_ARB_vertex_buffer_object") )
    missing += "GL_ARB_vertex_buffer_object extension missing\n";

#ifndef __APPLE__
  if ( !glExtensions.contains("GL_ARB_vertex_program") )
    missing += "GL_ARB_vertex_program extension missing\n";
#endif

  if ( !glExtensions.contains("GL_ARB_texture_float") )
    missing += "GL_ARB_texture_float extension missing\n";

  if ( !glExtensions.contains("GL_EXT_framebuffer_object") )
    missing += "GL_EXT_framebuffer_object extension missing\n";

  return missing;
}

#if QT_VERSION < 0x050000
  Q_EXPORT_PLUGIN2( ssaoplugin , SSAOPlugin );
#endif

