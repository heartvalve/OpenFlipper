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
*   $Revision: $                                                       *
*   $LastChangedBy: $                                                *
*   $Date: $                     *
*                                                                            *
\*===========================================================================*/

#include <GL/glew.h>
#include "ClassicDepthPeeling.hh"

#include <ACG/GL/GLError.hh>
#include <OpenFlipper/common/GlobalOptions.hh>
#include <OpenFlipper/BasePlugin/PluginFunctions.hh>

#include <QDir>

void DepthPeelingPlugin::reloadResources(int _viewerId) {
  // called in constructor and resizeEvent()

  ACG::GLState::syncFromGL();

  ViewerResources* p = &viewerRes_[_viewerId];

  if (!p->glWidth_ || !p->glHeight_) return;

  destroyResources(_viewerId);

  // dual depth peeling rt's
  glGenTextures(7, p->blendDualPeelTexID_);
  GLint DualPeelIntFmt[] = {GL_RG32F, GL_RG32F, // depth0, depth1
                 GL_RGBA, GL_RGBA, // front blender0, ..._1
                 GL_RGBA, GL_RGBA, // back_temp_tex0, ..._1,
                 GL_RGB};   // back_blender

  for (int i = 0; i < 7; ++i)
  {
    GLint fmt = GL_RGB; // fmt for depth textures

    if (i >= 2) fmt = GL_RGBA; // fmt for front_blender01 and back_temp01
    if (i == 6) fmt = GL_RGB; // fmt for back_blender

    ACG::GLState::bindTexture(GL_TEXTURE_RECTANGLE_EXT, p->blendDualPeelTexID_[i]);
    // texture access: clamped
    glTexParameteri(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_WRAP_T, GL_CLAMP);
    // filter: none
    glTexParameteri(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_RECTANGLE_EXT, 0, DualPeelIntFmt[i], p->glWidth_, p->glHeight_, 0, fmt, GL_FLOAT, 0);




    ACG::glCheckErrors();
  }

  p->rtWidth_ = p->glWidth_;
  p->rtHeight_ = p->glHeight_;

  // dual depth peeling fbo's
  glGenFramebuffersEXT(1, &p->blendDualPeelFbo_);
  {
    // layer_peel fbo
    ACG::GLState::bindFramebuffer(GL_FRAMEBUFFER, p->blendDualPeelFbo_);

    // color_attachment order:
    // depth0, front_blend0, back_temp0,   depth1, front_blend1, back_temp1,    back_blender_tex_id

    for (int i = 0; i < 6; ++i)
    {
      glFramebufferTexture2DEXT(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+i,
        GL_TEXTURE_RECTANGLE_EXT, p->blendDualPeelTexID_[i < 3 ? 2*i : 2*(i-3) +1], 0);
    }

    // back_blender_tex_id
      glFramebufferTexture2DEXT(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT6, GL_TEXTURE_RECTANGLE_EXT,
                                p->blendDualPeelTexID_[6], 0);
  }

  ACG::GLState::bindFramebuffer(GL_FRAMEBUFFER, 0);


  // load shaders

  const char* ShaderFiles[] = {"Blending/dual_peeling_init_vertex.glsl",
    "Blending/dual_peeling_init_fragment.glsl",
    "Blending/dual_peeling_blend_vertex.glsl",
    "Blending/dual_peeling_blend_fragment.glsl",
    "Blending/dual_peeling_final_vertex.glsl",
    "Blending/dual_peeling_final_fragment.glsl"};

  for (int i = 0; i < 6; ++i)
  {
    QString shaderFile = OpenFlipper::Options::shaderDirStr() + QDir::separator() + QString(ShaderFiles[i]);

    if (blendShaders_[i]) continue;

    if (i & 1) // alternating vertex/fragment shader
      blendShaders_[i] = GLSL::loadFragmentShader(shaderFile.toUtf8());
    else
      blendShaders_[i] = GLSL::loadVertexShader(shaderFile.toUtf8());

    if (!blendShaders_[i]) {
      log(LOGERR, QString(ShaderFiles[i]) + QString(" could not be loaded and compiled"));
      return;
    }
  }

  // dual depth peeling programs

  if (!blendDualPeelProg_[0])
  {
    for (int i = 0; i < 4; ++i)
    {
      if (i != 1)
        blendDualPeelProg_[i] = new GLSL::Program();
    }

    // init shaders
    blendDualPeelProg_[0]->attach(blendShaders_[0]);
    blendDualPeelProg_[0]->attach(blendShaders_[1]);
    blendDualPeelProg_[0]->link();

    // blend shaders
    blendDualPeelProg_[2]->attach(blendShaders_[2]);
    blendDualPeelProg_[2]->attach(blendShaders_[3]);
    blendDualPeelProg_[2]->link();

    // final shaders
    blendDualPeelProg_[3]->attach(blendShaders_[4]);
    blendDualPeelProg_[3]->attach(blendShaders_[5]);
    blendDualPeelProg_[3]->link();
  }

  if (!blendQueryID_)
    glGenQueries(1, &blendQueryID_);

  ACG::glCheckErrors();
}

void DepthPeelingPlugin::destroyResources(int _viewerId)
{
  ViewerResources* p = &viewerRes_[_viewerId];

  if (p->blendDualPeelFbo_) glDeleteFramebuffersEXT(1, &p->blendDualPeelFbo_);
  p->blendDualPeelFbo_ = 0;

  if (p->blendDualPeelTexID_) glDeleteTextures(7, p->blendDualPeelTexID_);
  memset(p->blendDualPeelTexID_, 0, sizeof(p->blendDualPeelTexID_));
}

void DepthPeelingPlugin::destroyResources() {
  // called in destructor and reloadBlendingTech()

  if (blendQueryID_)
    glDeleteQueries(1, &blendQueryID_);
  blendQueryID_ = 0;

  for (int i = 0; i < 4; ++i)
  {
    delete blendDualPeelProg_[i]; blendDualPeelProg_[i] = 0;
  }

  for (unsigned int i = 0; i < sizeof(blendShaders_) / sizeof(blendShaders_[0]); ++i)
  {
    delete blendShaders_[i];
    blendShaders_[i] = 0;
  }

  for (unsigned int i = 0; i < sizeof(peelShaders_) / sizeof(peelShaders_[0]); ++i)
  {
    delete peelShaders_[i]; peelShaders_[i] = 0;
  }

  for (unsigned int i = 0; i < sizeof(peelProgs_) / sizeof(peelProgs_[0]); ++i)
  {
    delete peelProgs_[i]; peelProgs_[i] = 0;
  }

  // free all viewer specific resources
  std::map<int, ViewerResources>::iterator resIt = viewerRes_.begin();
  for (; resIt != viewerRes_.end(); ++resIt)
    destroyResources(resIt->first);
}

void DepthPeelingPlugin::render(ACG::GLState* _glState, Viewer::ViewerProperties& _properties)
{
  glStateTmp_ = _glState;

  glPushAttrib(GL_ALL_ATTRIB_BITS);

  const GLuint targetFbo = ACG::GLState::getFramebufferDraw();

  int viewerId = _properties.viewerId();

  ViewerResources* pViewer = &viewerRes_[viewerId];
  pViewer->glWidth_ = _glState->viewport_width();
  pViewer->glHeight_ = _glState->viewport_height();

  if (pViewer->glWidth_ != pViewer->rtWidth_ || pViewer->glHeight_ != pViewer->rtHeight_)
    reloadResources(viewerId);

//  updatePeelingShaderSet(viewerId, _properties.drawMode());
  updatePeelingShaderSet();
  BaseNode* sceneGraphRoot = PluginFunctions::getSceneGraphRootNode();

  ACG::GLState::depthFunc(GL_LESS);


  ACG::GLState::disable(GL_CULL_FACE);
  ACG::GLState::disable(GL_LIGHTING);
  ACG::GLState::disable(GL_NORMALIZE);

  ACG::GLState::lockDepthFunc();
  ACG::GLState::lockState(GL_CULL_FACE);
  ACG::GLState::lockState(GL_LIGHTING);
  ACG::GLState::lockState(GL_NORMALIZE);

  // from nvidia demo code:
  //  needed some minor adjustments

  GLenum drawBuffers[] = {GL_COLOR_ATTACHMENT0_EXT,
    GL_COLOR_ATTACHMENT1_EXT,
    GL_COLOR_ATTACHMENT2_EXT,
    GL_COLOR_ATTACHMENT3_EXT,
    GL_COLOR_ATTACHMENT4_EXT,
    GL_COLOR_ATTACHMENT5_EXT,
    GL_COLOR_ATTACHMENT6_EXT};

  // the farthest depth value possible in the depth buffer
  const float maxDepth = 1.0f;

  ACG::GLState::disable(GL_DEPTH_TEST);
  ACG::GLState::enable(GL_BLEND);

  ACG::GLState::lockState(GL_DEPTH_TEST);
  ACG::GLState::lockState(GL_BLEND);



  /* FIXED: VIEWPORT BUG

  log-window causes viewport shift by 16 units upward in window's y axis
  therefore the scene gets rendered only in the upper part of the viewport:
    ->  glViewport(0, 16, glWidht, glHeight_)

  glHeight_() is NOT the height of the back buffer (i.e. glViewer window),
    but the height of scene target view
    -> glHeight_() is 16 units less than the back buffer

  -> all render targets are 16 units less in height than back buffer

  since the scene has to use the full render targets size,
  use glViewport(0, 0, glWidth_, glHeight_) for all offscreen rendering

  in the final pass, shift the viewport up by 16 units and use the shift amount
  in the shader (uniform g_Offset), to finally get the correct sampling coordinates


  note: shift amount is not hardcoded, but fetched from glGetIntegerv(GL_VIEWPORT)
  */


  GLint old_viewport[4];
  glGetIntegerv(GL_VIEWPORT, old_viewport);
  glViewport(0, 0, pViewer->glWidth_, pViewer->glHeight_);

  for (int i = 0; i < 6; ++i)
  {
    ACG::GLState::activeTexture(GL_TEXTURE0 + i);
    ACG::GLState::bindTexture(GL_TEXTURE_RECTANGLE_EXT, 0);
  }


  // ---------------------------------------------------------------------
  // 1. Initialize Min-Max Depth Buffer
  // ---------------------------------------------------------------------

  ACG::GLState::bindFramebuffer(GL_FRAMEBUFFER_EXT, pViewer->blendDualPeelFbo_);

  // Render targets 1 and 2 store the front and back colors
  // Clear to 0.0 and use MAX blending to filter written color
  // At most one front color and one back color can be written every pass
  ACG::GLState::drawBuffers(2, &drawBuffers[1]);
  glClearColor(0, 0, 0, 0);
  glClear(GL_COLOR_BUFFER_BIT);

  // Render target 0 stores (-minDepth, maxDepth, alphaMultiplier)
  ACG::GLState::drawBuffer(drawBuffers[0]);
  glClearColor(-maxDepth, -maxDepth, 0, 0);
  glClear(GL_COLOR_BUFFER_BIT);
  ACG::GLState::blendEquation(GL_MAX_EXT);
  ACG::GLState::lockBlendEquation();

  blendDualPeelProg_[0]->use();
  ACG::GLState::lockProgram();
  drawScenePass(_glState, _properties.drawMode(), sceneGraphRoot);
  ACG::GLState::unlockProgram();



  int currId = 0;

  // ---------------------------------------------------------------------
  // 2. Dual Depth Peeling + Blending
  // ---------------------------------------------------------------------

  // Since we cannot blend the back colors in the geometry passes,
  // we use another render target to do the alpha blending
  //glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, g_dualBackBlenderFboId);
  ACG::GLState::drawBuffer(drawBuffers[6]);
  glClearColor(_glState->clear_color()[0], _glState->clear_color()[1], _glState->clear_color()[2], 0);

  glClear(GL_COLOR_BUFFER_BIT);

  // Geometry layers are peeled until the sample query returns 0
  GLuint sampleCount = 1;
  for (int pass = 1; sampleCount; ++pass)
  {
    currId = pass % 2;
    int prevId = 1 - currId;
    int bufId = currId * 3;

    ACG::GLState::drawBuffers(2, &drawBuffers[bufId+1]);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    ACG::GLState::drawBuffer(drawBuffers[bufId+0]);
    glClearColor(-maxDepth, -maxDepth, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    // Render target 0: RG32F MAX blending
    // Render target 1: RGBA MAX blending
    // Render target 2: RGBA MAX blending
    ACG::GLState::drawBuffers(3, &drawBuffers[bufId+0]);
    ACG::GLState::unlockBlendEquation();
    ACG::GLState::blendEquation(GL_MAX_EXT);
    ACG::GLState::lockBlendEquation();

    ACG::GLState::activeTexture(GL_TEXTURE5); // front_blender_tex  base_offset: 2
    ACG::GLState::bindTexture(GL_TEXTURE_RECTANGLE_EXT, pViewer->blendDualPeelTexID_[2 + prevId]);

    ACG::GLState::activeTexture(GL_TEXTURE4); // depth_tex  base_offset: 0
    ACG::GLState::bindTexture(GL_TEXTURE_RECTANGLE_EXT, pViewer->blendDualPeelTexID_[0 + prevId]);


    // scene geometry peeling pass
    //  note that the peel shader is set right before rendering in the traverser, based on a node's drawmode
    ACG::GLState::activeTexture(GL_TEXTURE0);

    ACG::GLState::shadeModel(GL_SMOOTH); // flat shading is emulated in Geometry Shader, which only works with interpolated vertex shader output
    ACG::GLState::lockShadeModel();
    drawScenePeelPass(_glState, _properties.drawMode(), sceneGraphRoot, pass);
    ACG::GLState::unlockShadeModel();


    // Full screen pass to alpha-blend the back color
    ACG::GLState::drawBuffer(drawBuffers[6]);

    ACG::GLState::unlockBlendEquation();
    ACG::GLState::blendEquation(GL_FUNC_ADD);
    ACG::GLState::lockBlendEquation();
    ACG::GLState::blendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // start samples counter query
    glBeginQuery(GL_SAMPLES_PASSED_ARB, blendQueryID_);

    blendDualPeelProg_[2]->use();
    blendDualPeelProg_[2]->setUniform("TempTex", 4);

    ACG::GLState::activeTexture(GL_TEXTURE4); // back_temp_tex  base_offset: 4
    ACG::GLState::bindTexture(GL_TEXTURE_RECTANGLE_EXT, pViewer->blendDualPeelTexID_[4 + currId]);

    drawQuadProj(); // full screen quad, already projected

    glEndQuery(GL_SAMPLES_PASSED_ARB);
    glGetQueryObjectuiv(blendQueryID_, GL_QUERY_RESULT_ARB, &sampleCount);
  }

  ACG::GLState::unlockBlendEquation();

  ACG::GLState::unlockState(GL_BLEND);
  ACG::GLState::disable(GL_BLEND);



  // ---------------------------------------------------------------------
  // 3. Final Pass
  //  operates on screen pixels only
  // ---------------------------------------------------------------------

  // enable back buffer
  ACG::GLState::bindFramebuffer(GL_FRAMEBUFFER_EXT, targetFbo);
  ACG::GLState::drawBuffer(GL_BACK);

  // program id 3
  blendDualPeelProg_[3]->use();
  blendDualPeelProg_[3]->setUniform("FrontBlenderTex", 4);
  blendDualPeelProg_[3]->setUniform("BackBlenderTex", 5);

  // bugfix for multiple viewports:
  //  gl_FragCoord represents the screen space coordinate of a pixel into the back buffer
  //  this has to be back-shifted by the upper left viewport coordinate to get correct texture coordinates
  blendDualPeelProg_[3]->setUniform("ViewportOffset", ACG::Vec2f(old_viewport[0], old_viewport[1]));

  ACG::GLState::activeTexture(GL_TEXTURE5); // back_blender:  offset 6
  ACG::GLState::bindTexture(GL_TEXTURE_RECTANGLE_EXT, pViewer->blendDualPeelTexID_[6]);


  ACG::GLState::activeTexture(GL_TEXTURE4); // front_blender_tex  base_offset: 2
  ACG::GLState::bindTexture(GL_TEXTURE_RECTANGLE_EXT, pViewer->blendDualPeelTexID_[2 + currId]);


  glViewport(old_viewport[0], old_viewport[1], old_viewport[2], old_viewport[3]);
  drawQuadProj(-1.0f, 1.0f, 2.0f, 2.0f);


  blendDualPeelProg_[3]->disable();



  ACG::glCheckErrors();


  // unlock states
  ACG::GLState::unlockDepthFunc();
  ACG::GLState::unlockState(GL_CULL_FACE);
  ACG::GLState::unlockState(GL_LIGHTING);
  ACG::GLState::unlockState(GL_NORMALIZE);

  ACG::GLState::unlockState(GL_DEPTH_TEST);
  ACG::GLState::unlockState(GL_BLEND);


  glPopAttrib();
}
