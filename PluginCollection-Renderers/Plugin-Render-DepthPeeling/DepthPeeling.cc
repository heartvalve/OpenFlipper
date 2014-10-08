
#include <ACG/GL/acg_glew.hh>

#include "DepthPeeling.hh"

#include <OpenFlipper/common/GlobalOptions.hh>
#include <OpenFlipper/BasePlugin/PluginFunctions.hh>


#include <ACG/ShaderUtils/GLSLShader.hh>
#include <ACG/GL/ShaderCache.hh>
#include <ACG/GL/VertexDeclaration.hh>
#include <ACG/GL/GLError.hh>
#include <ACG/GL/ScreenQuad.hh>

using namespace ACG;


// =================================================
// depth peeling shader modifier

class PeelLayerModifier : public ShaderModifier
{
public:


  void modifyFragmentIO(ShaderGenerator* _shader)
  {
    _shader->addUniform("sampler2D g_DepthLayer");

    _shader->addOutput("float outDepth");
  }

  void modifyFragmentBeginCode(QStringList* _code)
  {
    // compare current depth with previous layer
    _code->push_back("float dp_prevDepth = texture(g_DepthLayer, sg_vScreenPos).x;");
    _code->push_back("if (gl_FragCoord.z <= dp_prevDepth) discard;");
  }

  void modifyFragmentEndCode(QStringList* _code)
  {
    _code->push_back("outFragment = vec4(sg_cColor.rgb * sg_cColor.a, sg_cColor.a);");
    _code->push_back("outDepth = gl_FragCoord.z;");
  }

  static PeelLayerModifier instance;
};


class PeelInitModifier : public ShaderModifier
{
public:

  void modifyFragmentIO(ShaderGenerator* _shader)
  {
    _shader->addOutput("float outDepth");
  }

  void modifyFragmentEndCode(QStringList* _code)
  {
    _code->push_back("outFragment = vec4(sg_cColor.rgb * sg_cColor.a, 1.0 - sg_cColor.a);");
    _code->push_back("outDepth = gl_FragCoord.z;");
  }

  static PeelInitModifier instance;
};

// for dual depth peeling only
class PeelDualLayerModifier : public ShaderModifier
{
public:

  void modifyFragmentIO(ShaderGenerator* _shader)
  {
    _shader->addUniform("sampler2D g_DepthLayer");
    _shader->addUniform("sampler2D g_FrontLayer");

    // MRT: color + depth output + last layer (eventually)
    _shader->addOutput("vec4 outDepth");
    _shader->addOutput("vec4 outBackColor");
  }

  void modifyFragmentBeginCode(QStringList* _code)
  {
    _code->push_back("float fragDepth = gl_FragCoord.z;");

    _code->push_back("vec2 depthLayer = texture2D(g_DepthLayer, sg_vScreenPos).xy;");
    _code->push_back("vec4 forwardTemp = texture2D(g_FrontLayer, sg_vScreenPos);");

    _code->push_back("outDepth = vec4(depthLayer, 0, 0);");
    _code->push_back("outFragment = forwardTemp;");
    _code->push_back("outBackColor = vec4(0,0,0,0);");


    _code->push_back("float nearestDepth = -depthLayer.x;");
    _code->push_back("float farthestDepth = depthLayer.y;");
    _code->push_back("float alphaMultiplier = 1.0 - forwardTemp.w;");


    _code->push_back("if (fragDepth < nearestDepth || fragDepth > farthestDepth) {");
    _code->push_back("outDepth = vec4(-1,-1,-1,-1);");
    _code->push_back("return;");
    _code->push_back("}");

    _code->push_back("if (fragDepth > nearestDepth && fragDepth < farthestDepth) {");
    _code->push_back("outDepth = vec4(-fragDepth, fragDepth, 0, 0);");
    _code->push_back("return;");
    _code->push_back("}");
  }


  void modifyFragmentEndCode(QStringList* _code)
  {
    _code->push_back("outDepth = vec4(-1,-1,-1,-1);");
    _code->push_back("outFragment = forwardTemp;");

    _code->push_back("if (fragDepth == nearestDepth) {");
    _code->push_back("outFragment.xyz += sg_cColor.rgb * sg_cColor.a * alphaMultiplier;");
    _code->push_back("outFragment.w = 1.0 - alphaMultiplier * (1.0 - sg_cColor.a);");

//    _code->push_back("outFragment = vec4(alphaMultiplier,alphaMultiplier,alphaMultiplier,1);");

    _code->push_back("} else {");
    _code->push_back("outBackColor += sg_cColor;");
    _code->push_back("}");
  }

  static PeelDualLayerModifier instance;
};

class PeelDualInitModifier : public ShaderModifier
{
public:
  void modifyFragmentEndCode(QStringList* _code)
  {
    _code->push_back("outFragment = vec4(-gl_FragCoord.z, gl_FragCoord.z, 0, 0);");
  }

  static PeelDualInitModifier instance;
};


PeelInitModifier PeelInitModifier::instance;
PeelLayerModifier PeelLayerModifier::instance;
PeelDualLayerModifier PeelDualLayerModifier::instance;
PeelDualInitModifier PeelDualInitModifier::instance;

// internal shader-attribute flags

#define RENDERFLAG_ALLOW_PEELING 1


// =================================================

DepthPeeling::DepthPeeling()
 : peelMode_(1), copyFrontDepth_(1), maxPeelCount_(20), peelBlend_(0), peelFinal_(0), peelQueryID_(0),
peelBlendDual_(0), peelFinalDual_(0)
{
}


DepthPeeling::~DepthPeeling()
{
}


void DepthPeeling::initializePlugin()
{
  ACG::ShaderProgGenerator::setShaderDir(OpenFlipper::Options::shaderDirStr());
}

void DepthPeeling::slotModeChanged( QAction *  _action) 
{
  // Prepare Picking Debugger Flag
  if ( _action->text() == "Front to Back") {
    peelMode_ = 0;
  } else if ( _action->text() == "Dual") {
    peelMode_ = 1;
  } else {
    std::cerr << "Error : optionHandling unable to find peeling mode!!! " << _action->text().toStdString() << std::endl;
    peelMode_ = 1;
  }
}

void DepthPeeling::render(ACG::GLState* _glState, Viewer::ViewerProperties& _properties)
{
  // debugging utilities
//   if (!dbgProg_)
//     dbgProg_ = GLSL::loadProgram("DepthPeeling/screenquad.glsl", "DepthPeeling/dbg_shader.glsl");


  // collect renderobjects
  prepareRenderingPipeline(_glState, _properties.drawMode(), PluginFunctions::getSceneGraphRootNode());


  if (peelMode_ == 0)
    renderFrontPeeling(_glState, _properties);
  else
    renderDualPeeling(_glState, _properties);

  // restore common opengl state
  // log window remains hidden otherwise
  finishRenderingPipeline();
}





void DepthPeeling::renderFrontPeeling(ACG::GLState* _glState,
                                      Viewer::ViewerProperties& _properties)
{
  const int numRenderObjects = getNumRenderObjects();

  // begin rendering
  // find last transparent object
  int lastPeeledObject = -1;

  for (int i = numRenderObjects - 1; i > lastPeeledObject; --i)
  {
    if ((sortedObjects_[i]->internalFlags_ & RENDERFLAG_ALLOW_PEELING) && sortedObjects_[i]->alpha < 0.99f)
      lastPeeledObject = i;
  }

  if (lastPeeledObject == -1)
  {
    // no transparent objects
    for (int i = 0; i < numRenderObjects; ++i)
      renderObject(sortedObjects_[i]);
  }
  else
  {
    // depth peeling code

    // make sure shaders and occlusion query are initialized
    initDepthPeeling();

    // resize fbo as necessary
    ViewerResources* viewRes = &viewerRes_[_properties.viewerId()];
    viewRes->resize(false, (unsigned int)_glState->viewport_width(), (unsigned int)_glState->viewport_height());


    // init MRT slots:
    //  RT0 - color blend texture
    //  RT1 - depth buffer copy
    const GLenum depthTarget = GL_COLOR_ATTACHMENT0;
    const GLenum colorTarget = GL_COLOR_ATTACHMENT1;
    const GLenum colorBlendTarget = GL_COLOR_ATTACHMENT4;
    GLenum peelLayerTargets[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};

    const float clearDepth = 1.0f;

    // begin peeling
    //  draw  first layer
    //  target depth buffer: depth0, front0
    glViewport(0, 0, viewRes->width_, viewRes->height_);
    glBindFramebuffer(GL_FRAMEBUFFER, viewRes->singleFbo_->getFboID());

    // allow color+depth write access
    glDepthMask(1);
    glColorMask(1,1,1,1);

    // clear color blend target
    glDrawBuffer(colorBlendTarget);
    glClearColor(_glState->clear_color()[0], _glState->clear_color()[1], _glState->clear_color()[2], 0);
    glClear(GL_COLOR_BUFFER_BIT);

    // clear colors  (front and back colors per pass)
    glDrawBuffer(colorTarget);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // clear depths
    glDrawBuffer(depthTarget);
    glClearColor(clearDepth, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    // 1. Pass: initialize depth values
    //  the init shader also writes the first front layer colors in accumulation buffer
    peelLayerTargets[0] = colorBlendTarget;
    peelLayerTargets[1] = depthTarget;
    glDrawBuffers(2, peelLayerTargets); // write to MRT slots (color, depth)

    glDisable(GL_BLEND);
    glDisable(GL_ALPHA_TEST);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDisable(GL_CULL_FACE);

    for (int i = 0; i < numRenderObjects; ++i)
    {
//      if ((sortedObjects_[i]->internalFlags_ & RENDERFLAG_ALLOW_PEELING) && sortedObjects_[i]->alpha < 0.99f)
      {
        GLSL::Program* initProg = ShaderCache::getInstance()->getProgram(&sortedObjects_[i]->shaderDesc, PeelInitModifier::instance);

        if (initProg->getFragDataLocation("outFragment") != 0 || initProg->getFragDataLocation("outDepth") != 1)
        {
          initProg->bindFragDataLocation(0, "outFragment"); // color -> slot0
          initProg->bindFragDataLocation(1, "outDepth");    // depth -> slot1
          initProg->link();
        }

        renderObject(sortedObjects_[i], initProg, true);
      }
    }

    // copy depth values to input depth buffer if requested
    if (copyFrontDepth_)
      copyDepthToBackBuffer(viewRes->singleDepthTex_[0], 1.0f);

    // peel layers, we start at index 1 instead of 0
    // since the front layer is already initialized in
    // depth buffer 0 and we want to extract the second one now

    for (int pass = 1; pass < maxPeelCount_; ++pass)
    {
      const int currID = pass & 1;
      const int prevID = 1 - currID;
      const int bufID = currID * 2;

      // peeling MRT slots:
      //  RT0 - color
      //  RT1 - depth buffer copy
      const GLenum targetBuffer[2] = {colorTarget + bufID, depthTarget + bufID};

      // ---------------------------------------------------------------
      // 1st peel next layer

      // clear color texture
      glDrawBuffer(targetBuffer[0]);
      glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
      glClear(GL_COLOR_BUFFER_BIT);
      // clear depth texture
      glDrawBuffer(targetBuffer[1]);
      glClearColor(clearDepth, 0.0f, 0.0f, 0.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


      glDrawBuffers(2, targetBuffer);


      glDisable(GL_BLEND);
      glEnable(GL_DEPTH_TEST);

      // count # passed fragments
      glBeginQuery(GL_SAMPLES_PASSED, peelQueryID_);


      // bind previous depth layer to texture unit 4
      glActiveTexture(GL_TEXTURE4);
      glBindTexture(GL_TEXTURE_2D, viewRes->singleDepthTex_[prevID]);

      // render scene
      for (int k = 0; k < numRenderObjects; ++k)
      {
        if ((sortedObjects_[k]->internalFlags_ & RENDERFLAG_ALLOW_PEELING) && sortedObjects_[k]->alpha < 0.99f)
        {
          GLSL::Program* peelProg = ShaderCache::getInstance()->getProgram(&sortedObjects_[k]->shaderDesc, PeelLayerModifier::instance);

          if (peelProg->getFragDataLocation("outFragment") != 0 || peelProg->getFragDataLocation("outDepth") != 1)
          {
            peelProg->bindFragDataLocation(0, "outFragment");
            peelProg->bindFragDataLocation(1, "outDepth");
            peelProg->link();
          }

          peelProg->use();
          peelProg->setUniform("g_DepthLayer", 4);

          renderObject(sortedObjects_[k], peelProg, true);
        }
      }

      glEndQuery(GL_SAMPLES_PASSED);



      // ---------------------------------------------------------------
      // 2nd underblend layer with current scene
      //  (fullscreen pass)
      glDrawBuffer(colorBlendTarget);

      glDepthMask(1);
      glColorMask(1,1,1,1);

      glDisable(GL_DEPTH_TEST);
      glEnable(GL_BLEND);

      glBlendEquation(GL_FUNC_ADD);
      glBlendFuncSeparate(GL_DST_ALPHA, GL_ONE,
        GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);


      peelBlend_->use();
      peelBlend_->setUniform("BlendTex", 0);

      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, viewRes->singleFrontTex_[currID]);

      ACG::ScreenQuad::draw(peelBlend_);


      glDisable(GL_BLEND);



      GLuint passedSamples;
      glGetQueryObjectuiv(peelQueryID_, GL_QUERY_RESULT, &passedSamples);
      if (passedSamples == 0)
        break;
    }


    // copy to back buffer
    restoreInputFbo();

    glDepthMask(1);
    glColorMask(1,1,1,1);

    glDisable(GL_DEPTH_TEST);

    peelFinal_->use();


    ACG::Vec3f bkgColor;
    bkgColor[0] = _properties.backgroundColor()[0];
    bkgColor[1] = _properties.backgroundColor()[1];
    bkgColor[2] = _properties.backgroundColor()[2];

    peelFinal_->setUniform("BkgColor", bkgColor);

    peelFinal_->setUniform("SceneTex", 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, viewRes->singleBlendTex_);

    ACG::ScreenQuad::draw(peelFinal_);

    ACG::glCheckErrors();




    // draw rest of scene
    glEnable(GL_DEPTH_TEST);


//     // draw rest of opaque objects
//     for (int i = lastPeeledObject + 1; i < numRenderObjects; ++i)
//       renderObject(sortedObjects_[i]);

  }
}


void DepthPeeling::renderDualPeeling(ACG::GLState* _glState, Viewer::ViewerProperties& _properties)
{
  const int numRenderObjects = getNumRenderObjects();

  // begin rendering
  // find last transparent object
  int lastPeeledObject = -1;

  for (int i = numRenderObjects - 1; i > lastPeeledObject; --i)
  {
    if ((sortedObjects_[i]->internalFlags_ & RENDERFLAG_ALLOW_PEELING) && sortedObjects_[i]->alpha < 0.99f)
      lastPeeledObject = i;
  }

  if (0) //lastPeeledObject == -1)
  {
    // no transparent objects
    for (int i = 0; i < numRenderObjects; ++i)
      renderObject(sortedObjects_[i]);
  }
  else
  {
    // depth peeling code

    // make sure shaders and occlusion query are initialized
    initDualDepthPeeling();

    // resize fbo as necessary
    ViewerResources* viewRes = &viewerRes_[_properties.viewerId()];
    viewRes->resize(true, (unsigned int)_glState->viewport_width(), (unsigned int)_glState->viewport_height());


    // enable color/depth write access
    glDepthMask(1);
    glColorMask(1,1,1,1);


    // clear render targets
//    viewRes->dualFboACG_->bind();
    glViewport(0, 0, _glState->viewport_width(), _glState->viewport_height());
    glBindFramebuffer(GL_FRAMEBUFFER, viewRes->dualFbo_->getFboID());

    const GLenum depthTarget = GL_COLOR_ATTACHMENT0; // stores (-minDepth, maxDepth)
    const GLenum colorTargets[] = {GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
    const GLenum colorBlendTarget = GL_COLOR_ATTACHMENT6;

    // clear colors  (front and back colors per pass)
    glDrawBuffers(2, colorTargets);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // clear color blend target
    glDrawBuffer(colorBlendTarget);
    glClearColor(_glState->clear_color()[0], _glState->clear_color()[1], _glState->clear_color()[2], 0);
    glClear(GL_COLOR_BUFFER_BIT);

    // clear depths
    glDrawBuffer(depthTarget);
    glClearColor(-1.0f, -1.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);


    // 1. Pass: initialize depth values
    //  the init shader writes min/max depth values (-z, +z)
    glEnable(GL_BLEND);
    glBlendEquation(GL_MAX_EXT);  // get min/max depth
    
    glDisable(GL_ALPHA_TEST);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDisable(GL_CULL_FACE);

    for (int i = 0; i < numRenderObjects; ++i)
    {
      GLSL::Program* initProg = ShaderCache::getInstance()->getProgram(&sortedObjects_[i]->shaderDesc, PeelDualInitModifier::instance);

      renderObject(sortedObjects_[i], initProg, true);
    }


    // copy depth values of first front layer to input depth buffer if requested
    if (copyFrontDepth_)
    {
      glDisable(GL_BLEND);
      copyDepthToBackBuffer(viewRes->dualDepthTex_[0], -1.0f);
      // dual peeling requires blending
      glEnable(GL_BLEND);
    }

    // 2. peeling passes + color accumulation

    int currID = 0; // ping-pong render targets

//if (0){

    for (int pass = 1; 1 && pass < maxPeelCount_; ++pass)
    {
      currID = pass & 1;
      const int prevID = 1 - currID;
      const int bufID = currID * 3;

      GLenum targetBuffer[3];
      targetBuffer[0] = GL_COLOR_ATTACHMENT0 + bufID;
      targetBuffer[1] = GL_COLOR_ATTACHMENT1 + bufID;
      targetBuffer[2] = GL_COLOR_ATTACHMENT2 + bufID;
//      GLenum sourceBuffer[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};

      // clear target colors
      glDrawBuffers(2, targetBuffer + 1);
      glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
      glClear(GL_COLOR_BUFFER_BIT);

      // clear target depth
      glDrawBuffer(targetBuffer[0]);
      glClearColor(-1.0f, -1.0f, 0.0f, 0.0f);
      glClear(GL_COLOR_BUFFER_BIT);



      // Peel pass

      // set target
      glBlendEquation(GL_MAX_EXT);
      glDrawBuffers(3, targetBuffer); // shader writes to (depth_tex, front_color_tex, back_color_tex)

      // set texture source (provide previous layer)
      // slot 4 - depth
      // slot 5 - front color

      glActiveTexture(GL_TEXTURE5);
      glBindTexture(GL_TEXTURE_2D, viewRes->dualFrontTex_[prevID]);

      glActiveTexture(GL_TEXTURE4);
      glBindTexture(GL_TEXTURE_2D, viewRes->dualDepthTex_[prevID]);

      // restore active tex marker to default slot
      glActiveTexture(GL_TEXTURE0);

      // peel scene
      for (int i = 0; i < numRenderObjects; ++i)
      {
        GLSL::Program* peelProg = ShaderCache::getInstance()->getProgram(&sortedObjects_[i]->shaderDesc, PeelDualLayerModifier::instance);

        peelProg->use();
        peelProg->setUniform("g_DepthLayer", 4);
        peelProg->setUniform("g_FrontLayer", 5);

        // setup MRT
        //  outFragment -> front
        //  outDepth -> depth
        //  outBackColor -> back

        int locOutFrag = peelProg->getFragDataLocation("outFragment");
        int locOutDepth = peelProg->getFragDataLocation("outDepth");
        int locOutBackColor = peelProg->getFragDataLocation("outBackColor");

        if (locOutFrag != 1 ||
            locOutDepth != 0 ||
            locOutBackColor != 2)
        {
          // linking is slow; only link if necessary
          peelProg->bindFragDataLocation(1, "outFragment");
          peelProg->bindFragDataLocation(0, "outDepth");
          peelProg->bindFragDataLocation(2, "outBackColor");

          peelProg->link();
        }
        


        renderObject(sortedObjects_[i], peelProg, true);
      }




      // ----------------------
      // debugging
/*
      if (pass == 1)
      {
        dbgDrawTex(viewRes->dualFrontTex_[currID]);
        return;
      }
*/
      //




      // blend back color into accumulation buffer
      glDrawBuffer(colorBlendTarget);

      glBlendEquationEXT(GL_FUNC_ADD);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      // bind back color texture to slot 0
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, viewRes->dualBackTex_[currID]);


      glBeginQuery(GL_SAMPLES_PASSED_ARB, peelQueryID_);

      peelBlendDual_->use();
      peelBlendDual_->setUniform("BlendTex", 0);
      
      ACG::ScreenQuad::draw(peelBlendDual_);

      glEndQuery(GL_SAMPLES_PASSED_ARB);



      // termination check
      GLuint passedSamples;
      glGetQueryObjectuiv(peelQueryID_, GL_QUERY_RESULT, &passedSamples);
      if (passedSamples == 0)
        break;
    }

//}

    // Final pass: combine accumulated front and back colors
    restoreInputFbo();

    glDepthMask(1);
    glColorMask(1,1,1,1);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);


    peelFinalDual_->use();


    ACG::Vec3f bkgColor;
    bkgColor[0] = _properties.backgroundColor()[0];
    bkgColor[1] = _properties.backgroundColor()[1];
    bkgColor[2] = _properties.backgroundColor()[2];

    peelFinalDual_->setUniform("BkgColor", bkgColor);

    // slot 0 - front colors
    // slot 1 - back colors

    peelFinalDual_->setUniform("FrontSceneTex", 0);
    peelFinalDual_->setUniform("BackSceneTex", 1);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, viewRes->dualBlendTex_);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, viewRes->dualFrontTex_[currID]);

//    drawProjQuad(peelFinalDual_);
    ACG::ScreenQuad::draw(peelFinalDual_);


    peelFinalDual_->disable();





    // ------------------------------------------
    // debugging

//    dbgDrawTex(viewRes->dualDepthTex_[0]);


    // ------------------------------------------








    ACG::glCheckErrors();


//     // draw rest of scene
//     glEnable(GL_DEPTH_TEST);
// 
// 
//     // draw rest of opaque objects
//     for (int i = lastPeeledObject + 1; i < numRenderObjects; ++i)
//       renderObject(sortedObjects_[i]);

  }
}

void DepthPeeling::addRenderObject( RenderObject* _renderObject )
{
  // do some more checks for error detection
  if (!_renderObject->vertexDecl)
    std::cout << "error: missing vertex declaration" << std::endl;
  else
  {
    renderObjects_.push_back(*_renderObject);


    RenderObject* p = &renderObjects_.back();

    if (!p->shaderDesc.numLights)
      p->shaderDesc.numLights = numLights_;

    else if (p->shaderDesc.numLights < 0 || p->shaderDesc.numLights >= SG_MAX_SHADER_LIGHTS)
      p->shaderDesc.numLights = 0;

    p->internalFlags_ = 0;

    // allow potential depth peeling only for compatible
    //  render states

    if (p->alpha < 1.0f &&
      p->depthTest && 
      p->depthWrite && (p->depthFunc == GL_LESS ||
      p->depthFunc == GL_LEQUAL))
      p->internalFlags_ = RENDERFLAG_ALLOW_PEELING;


    // precompile shader
    ShaderCache::getInstance()->getProgram(&p->shaderDesc);

  }
}




DepthPeeling::ViewerResources::ViewerResources()
: width_(0), height_(0), singleBlendTex_(0), singleFbo_(0), dualBlendTex_(0), dualFbo_(0)
{
  memset(singleDepthTex_, 0, sizeof(singleDepthTex_));
  memset(singleFrontTex_, 0, sizeof(singleFrontTex_));
 
  memset(dualDepthTex_, 0, sizeof(dualDepthTex_));
  memset(dualFrontTex_, 0, sizeof(dualFrontTex_));
  memset(dualBackTex_, 0, sizeof(dualBackTex_));
}


DepthPeeling::ViewerResources::~ViewerResources()
{
  delete singleFbo_;
  delete dualFbo_;
}


void DepthPeeling::ViewerResources::resize(bool _dualPeeling, unsigned int _width, unsigned int _height)
{
  if (!_width ||  !_height)
    return;

  width_ = _width;
  height_ = _height;

  if (!_dualPeeling)
  {
    if (!singleFbo_)
    {
      singleFbo_ = new ACG::FBO();

      singleFbo_->init();

      // texture formats:
      //  depth textures: store layer depth as float
      //  front color textures: store (r,g,b,a) colors as R8G8B8A8
      //  color blending and accumulation texture: (r,g,b,a) color as R8G8B8A8

      singleFbo_->attachTexture2D(GL_COLOR_ATTACHMENT0, width_, height_, GL_R32F, GL_RGB, GL_CLAMP, GL_NEAREST, GL_NEAREST);
      singleFbo_->attachTexture2D(GL_COLOR_ATTACHMENT1, width_, height_, GL_RGBA, GL_RGBA, GL_CLAMP, GL_NEAREST, GL_NEAREST);

      singleFbo_->attachTexture2D(GL_COLOR_ATTACHMENT2, width_, height_, GL_R32F, GL_RGB, GL_CLAMP, GL_NEAREST, GL_NEAREST);
      singleFbo_->attachTexture2D(GL_COLOR_ATTACHMENT3, width_, height_, GL_RGBA, GL_RGBA, GL_CLAMP, GL_NEAREST, GL_NEAREST);

      singleFbo_->attachTexture2D(GL_COLOR_ATTACHMENT4, width_, height_, GL_RGBA, GL_RGBA, GL_CLAMP, GL_NEAREST, GL_NEAREST);

      singleFbo_->attachTexture2DDepth(width_, height_);

      singleDepthTex_[0] = singleFbo_->getAttachment(GL_COLOR_ATTACHMENT0);
      singleDepthTex_[1] = singleFbo_->getAttachment(GL_COLOR_ATTACHMENT2);

      singleFrontTex_[0] = singleFbo_->getAttachment(GL_COLOR_ATTACHMENT1);
      singleFrontTex_[1] = singleFbo_->getAttachment(GL_COLOR_ATTACHMENT3);

      singleBlendTex_ = singleFbo_->getAttachment(GL_COLOR_ATTACHMENT4);


      singleFbo_->checkFramebufferStatus();

      ACG::glCheckErrors();
    }
    else
      singleFbo_->resize(_width, _height);

  }
  else
  {
    // dual peeling render targets

    // fbo
    if (!dualFbo_)
    {
      dualFbo_ = new ACG::FBO();

      dualFbo_->init();

      // texture formats:
      //  depth textures: store (min,max) depth as float2
      //  front+back color textures: store (r,g,b,a) colors as R8G8B8A8
      //  color blending and accumulation texture: (r,g,b) color as R8G8B8X8
      
      dualFbo_->attachTexture2D(GL_COLOR_ATTACHMENT0, width_, height_, GL_RG32F, GL_RGB, GL_CLAMP, GL_NEAREST, GL_NEAREST);
      dualFbo_->attachTexture2D(GL_COLOR_ATTACHMENT1, width_, height_, GL_RGBA, GL_RGBA, GL_CLAMP, GL_NEAREST, GL_NEAREST);
      dualFbo_->attachTexture2D(GL_COLOR_ATTACHMENT2, width_, height_, GL_RGBA, GL_RGBA, GL_CLAMP, GL_NEAREST, GL_NEAREST);

      dualFbo_->attachTexture2D(GL_COLOR_ATTACHMENT3, width_, height_, GL_RG32F, GL_RGB, GL_CLAMP, GL_NEAREST, GL_NEAREST);
      dualFbo_->attachTexture2D(GL_COLOR_ATTACHMENT4, width_, height_, GL_RGBA, GL_RGBA, GL_CLAMP, GL_NEAREST, GL_NEAREST);
      dualFbo_->attachTexture2D(GL_COLOR_ATTACHMENT5, width_, height_, GL_RGBA, GL_RGBA, GL_CLAMP, GL_NEAREST, GL_NEAREST);

      dualFbo_->attachTexture2D(GL_COLOR_ATTACHMENT6, width_, height_, GL_RGB, GL_RGB, GL_CLAMP, GL_NEAREST, GL_NEAREST);

      dualDepthTex_[0] = dualFbo_->getAttachment(GL_COLOR_ATTACHMENT0);
      dualDepthTex_[1] = dualFbo_->getAttachment(GL_COLOR_ATTACHMENT3);

      dualFrontTex_[0] = dualFbo_->getAttachment(GL_COLOR_ATTACHMENT1);
      dualFrontTex_[1] = dualFbo_->getAttachment(GL_COLOR_ATTACHMENT4);

      dualBackTex_[0] = dualFbo_->getAttachment(GL_COLOR_ATTACHMENT2);
      dualBackTex_[1] = dualFbo_->getAttachment(GL_COLOR_ATTACHMENT5);

      dualBlendTex_ = dualFbo_->getAttachment(GL_COLOR_ATTACHMENT6);


      dualFbo_->checkFramebufferStatus();

      ACG::glCheckErrors();
    }
    else
      dualFbo_->resize(_width, _height);

  }
  

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  ACG::glCheckErrors();
}

void DepthPeeling::initDepthPeeling()
{
  // check if already initialized
  if (peelBlend_ && peelFinal_ && peelQueryID_)
    return;

  // register shader modifiers
  ShaderProgGenerator::registerModifier(&PeelInitModifier::instance);
  ShaderProgGenerator::registerModifier(&PeelLayerModifier::instance);

  // load intermediate blending and final shader
  if (!peelBlend_)
    peelBlend_ = GLSL::loadProgram("DepthPeeling/screenquad.glsl", "DepthPeeling/blend.glsl");

  if (!peelFinal_)
    peelFinal_ = GLSL::loadProgram("DepthPeeling/screenquad.glsl", "DepthPeeling/final.glsl");

  // occ query id
  if (!peelQueryID_)
    glGenQueries(1, &peelQueryID_);

  ACG::glCheckErrors();
}

void DepthPeeling::initDualDepthPeeling()
{
  // check if already initialized
  if (peelBlendDual_ && peelFinalDual_ && peelQueryID_)
    return;

  // register shader modifiers
  ShaderProgGenerator::registerModifier(&PeelDualInitModifier::instance);
  ShaderProgGenerator::registerModifier(&PeelDualLayerModifier::instance);

  // load intermediate blending and final shader
  if (!peelBlendDual_)
    peelBlendDual_ = GLSL::loadProgram("DepthPeeling/screenquad.glsl", "DepthPeeling/blend_dual.glsl");

  if (!peelFinalDual_)
    peelFinalDual_ = GLSL::loadProgram("DepthPeeling/screenquad.glsl", "DepthPeeling/final_dual.glsl");

  // occ query id
  if (!peelQueryID_)
    glGenQueries(1, &peelQueryID_);

  ACG::glCheckErrors();
}

QString DepthPeeling::renderObjectsInfo( bool _outputShaderInfo )
{
  QString infoString;

  ACG::ShaderModifier* availableMods[4] = 
  {
    &PeelInitModifier::instance, &PeelLayerModifier::instance,
    &PeelDualInitModifier::instance, &PeelDualLayerModifier::instance
  };


  // write modified shaders for init and peel passes

  infoString += "PeelInit:\n\n\n";

  std::vector<ACG::ShaderModifier*> mods;
  mods.push_back(availableMods[peelMode_*2]);

  infoString += dumpCurrentRenderObjectsToString(&sortedObjects_[0],_outputShaderInfo, &mods);


  infoString += "\n\n-----------------------------------------------\n\n\n\n";
  infoString += "PeelLayer:\n\n\n";

  mods[0] = availableMods[peelMode_*2 + 1];

  infoString += dumpCurrentRenderObjectsToString(&sortedObjects_[0],_outputShaderInfo, &mods);
    
  return infoString;
}


// 
// void DepthPeeling::dbgDrawTex( GLuint _texID )
// {
//   glBindFramebuffer(GL_FRAMEBUFFER, 0);
//   glDrawBuffer(GL_BACK);
//   glClearColor(0,0,0,0);
//   glClear(GL_COLOR_BUFFER_BIT);
// 
// 
//   glDepthMask(1);
//   glColorMask(1,1,1,1);
// 
//   glDisable(GL_DEPTH_TEST);
//   glDisable(GL_BLEND);
// 
//   dbgProg_->use();
// 
//   dbgProg_->setUniform("Tex", 0);
// 
//   glActiveTexture(GL_TEXTURE0);
//   glBindTexture(GL_TEXTURE_2D, _texID);
// 
//   drawProjQuad(dbgProg_);
// 
//   dbgProg_->disable();
// }
// 



#if QT_VERSION < 0x050000
  Q_EXPORT_PLUGIN2( depthpeelingshaderrenderer , DepthPeeling );
#endif

