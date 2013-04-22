
#include "DepthPeeling.hh"

#include <OpenFlipper/common/GlobalOptions.hh>
#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include <QGLFormat>


#include <ACG/GL/gl.hh>
#include <ACG/ShaderUtils/GLSLShader.hh>
#include <ACG/GL/ShaderCache.hh>
#include <ACG/GL/VertexDeclaration.hh>

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
: screenQuadVBO_(0), screenQuadDecl_(0),
peelBlend_(0), peelFinal_(0), peelDepthCopy_(0), peelQueryID_(0),
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





void DepthPeeling::render(ACG::GLState* _glState, Viewer::ViewerProperties& _properties)
{
  // collect renderobjects
  prepareRenderingPipeline(_glState, _properties.drawMode(), PluginFunctions::getSceneGraphRootNode());


  // clear back buffer
  ACG::Vec4f clearColor = _properties.backgroundColor();
  glClearColor(clearColor[0], clearColor[1], clearColor[2], 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);





//  renderFrontPeeling(_glState, _properties);
  renderDualPeeling(_glState, _properties);








  // restore common opengl state
  // log window remains hidden otherwise
  finishRenderingPipeline();

//  dumpRenderObjectsToText("../../dump_ro.txt", &sortedObjects_[0]);
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

    // ensure correct rendertarget size
    updateViewerResources(0, _properties.viewerId(), _glState->viewport_width(), _glState->viewport_height());

    ViewerResources* viewRes = &viewerRes_[_properties.viewerId()];


    // MRT:
    //  RT0 - color
    //  RT1 - depth buffer copy
    GLenum peelDrawTargets[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};

    // begin peeling
    //  draw  first layer
    //  target depth buffer: viewRes->peelTarget_[0]
    glBindFramebuffer(GL_FRAMEBUFFER, viewRes->peelBlendFbo_);

    glDepthMask(1);
    glColorMask(1,1,1,1);

    // clear depth-tex copy
    glDrawBuffer(GL_COLOR_ATTACHMENT1);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // clear color and depth buffer
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    glDrawBuffers(2, peelDrawTargets);

//     // draw opaque objects first
//     for (int i = 0; i <= lastPeeledObject; ++i)
//     {
//       if (!(sortedObjects_[i]->internalFlags_ & RENDERFLAG_ALLOW_PEELING) || sortedObjects_[i]->alpha >= 0.99f)
//         renderObject(sortedObjects_[i]);
//     }


    glDepthMask(1);
    glColorMask(1,1,1,1);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glDisable(GL_BLEND);
    glDisable(GL_ALPHA_TEST);

    for (int i = 0; i < numRenderObjects; ++i)
    {
//      if ((sortedObjects_[i]->internalFlags_ & RENDERFLAG_ALLOW_PEELING) && sortedObjects_[i]->alpha < 0.99f)
      {
        GLSL::Program* initProg = ShaderCache::getInstance()->getProgram(&sortedObjects_[i]->shaderDesc, PeelInitModifier::instance);

        initProg->bindFragDataLocation(0, "outFragment");
        initProg->bindFragDataLocation(1, "outDepth");
        initProg->link();

        renderObject(sortedObjects_[i], initProg, true);
      }
    }


    // TODO:
    //  copy front layer depth buffer to window depth buffer
    //  or even better: peel from back to front

    // peel layers, we start at index 1 instead of 0
    // since the front layer is already initialized in
    // depth buffer 0 and we want to extract the second one now
    for (int i = 1; i < 10; ++i)
    {

      // pointer to current and previous layer
      PeelLayer* curr = viewRes->peelTargets_ + (i & 1);
      PeelLayer* prev = viewRes->peelTargets_ + 1 - (i & 1);

      // 1st peel next layer

      glBindFramebuffer(GL_FRAMEBUFFER, curr->fbo);
      glDrawBuffers(2, peelDrawTargets);

      glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      glDisable(GL_BLEND);
      glEnable(GL_DEPTH_TEST);

      // count # passed fragments
      glBeginQuery(GL_SAMPLES_PASSED, peelQueryID_);


      // bind previous depth layer to texture unit 4
      glActiveTexture(GL_TEXTURE4);
      glBindTexture(GL_TEXTURE_2D, prev->depthTex);

      // render scene
      for (int k = 0; k < numRenderObjects; ++k)
      {
        if ((sortedObjects_[k]->internalFlags_ & RENDERFLAG_ALLOW_PEELING) && sortedObjects_[k]->alpha < 0.99f)
        {
          GLSL::Program* peelProg = ShaderCache::getInstance()->getProgram(&sortedObjects_[k]->shaderDesc, PeelLayerModifier::instance);

          peelProg->bindFragDataLocation(0, "outFragment");
          peelProg->bindFragDataLocation(1, "outDepth");
          peelProg->link();

          peelProg->use();
          peelProg->setUniform("g_DepthLayer", 4);

          renderObject(sortedObjects_[k], peelProg, true);
        }
      }

      glEndQuery(GL_SAMPLES_PASSED);



      // 2nd underblend layer with current scene
      //  (fullscreen pass)

      glBindFramebuffer(GL_FRAMEBUFFER, viewRes->peelBlendFbo_);
      glDrawBuffer(GL_COLOR_ATTACHMENT0);

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
      glBindTexture(GL_TEXTURE_2D, curr->colorTex);

      drawProjQuad(peelBlend_);


      glDisable(GL_BLEND);



      GLuint passedSamples;
      glGetQueryObjectuiv(peelQueryID_, GL_QUERY_RESULT, &passedSamples);
      if (passedSamples == 0)
        break;
    }


    // copy to back buffer

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDrawBuffer(GL_BACK);

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
    glBindTexture(GL_TEXTURE_2D, viewRes->peelBlendTex_);

    drawProjQuad(peelFinal_);

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

    // ensure correct rendertarget size
    updateViewerResources(1, _properties.viewerId(), _glState->viewport_width(), _glState->viewport_height());

    ViewerResources* viewRes = &viewerRes_[_properties.viewerId()];


    // enable color/depth write access
    glDepthMask(1);
    glColorMask(1,1,1,1);


    // clear render targets
    glBindFramebuffer(GL_FRAMEBUFFER, viewRes->dualFbo_);

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


    // 2. peeling passes + color accumulation

    int currID = 0; // ping-pong render targets

//if (0){

    for (int pass = 1; 1 && pass < 10; ++pass)
    {
      currID = pass & 1;
      int prevID = 1 - currID;
      int bufID = currID * 3;

      GLenum targetBuffer[3];
      targetBuffer[0] = GL_COLOR_ATTACHMENT0 + bufID;
      targetBuffer[1] = GL_COLOR_ATTACHMENT1 + bufID;
      targetBuffer[2] = GL_COLOR_ATTACHMENT2 + bufID;
      GLenum sourceBuffer[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};

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

        if (peelProg->getFragDataLocation("outFragment") != 1 ||
            peelProg->getFragDataLocation("outDepth") != 0 ||
            peelProg->getFragDataLocation("outBackColor") != 2)
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
      
      drawProjQuad(peelBlendDual_);

      glEndQuery(GL_SAMPLES_PASSED_ARB);



      // termination check
      GLuint passedSamples;
      glGetQueryObjectuiv(peelQueryID_, GL_QUERY_RESULT, &passedSamples);
      if (passedSamples == 0)
        break;
    }

//}

    // Final pass: combine accumulated front and back colors
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDrawBuffer(GL_BACK);

    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT);

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

    drawProjQuad(peelFinalDual_);

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





QString DepthPeeling::checkOpenGL()
{
  return QString("");
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
: glWidth_(0), glHeight_(0), peelBlendTex_(0), peelBlendFbo_(0), dualBlendTex_(0), dualFbo_(0)
{
  memset(peelTargets_, 0, sizeof(peelTargets_));
 
  memset(dualDepthTex_, 0, sizeof(dualDepthTex_));
  memset(dualFrontTex_, 0, sizeof(dualFrontTex_));
  memset(dualBackTex_, 0, sizeof(dualBackTex_));
}

void DepthPeeling::updateViewerResources(bool _dualPeeling,
                                         int _viewerId,
                                         unsigned int _newWidth,
                                         unsigned int _newHeight)
{
  if (_dualPeeling)
    initDualDepthPeeling();
  else
    initDepthPeeling();

  // allocate opengl resources

  // screenquad
  if (!screenQuadDecl_)
  {
    screenQuadDecl_ = new VertexDeclaration();
    screenQuadDecl_->addElement(GL_FLOAT, 3, VERTEX_USAGE_POSITION);
  }

  if (!screenQuadVBO_)
  {
    float quad[] = 
    {
      -1.0f, -1.0f, -1.0f, 
      1.0f, -1.0f, -1.0f,
      1.0f,  1.0f, -1.0f,
      -1.0f,  1.0f, -1.0f
    };

    glGenBuffers(1, &screenQuadVBO_);

    glBindBuffer(GL_ARRAY_BUFFER, screenQuadVBO_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);

    ACG::glCheckErrors();
  }


  if (!_newWidth || !_newHeight) return;

  ViewerResources* view = &viewerRes_[_viewerId];

  if (view->glHeight_ == _newHeight &&
    view->glWidth_  == _newWidth)
    return;

  view->glWidth_ = _newWidth;
  view->glHeight_ = _newHeight;

  // update depth peeling textures

  if (!_dualPeeling)
  {
    for (int i = 0; i < 2; ++i)
    {
      PeelLayer* dst = view->peelTargets_ + i;

      if (!dst->colorTex)
        glGenTextures(1, &dst->colorTex);

      if (!dst->depthBuf)
        glGenTextures(1, &dst->depthBuf);
      
      if (!dst->depthTex)
        glGenTextures(1, &dst->depthTex);

      if (!dst->colorTex || !dst->depthBuf || !dst->depthTex) // out of memory
        continue;


      glBindTexture(GL_TEXTURE_2D, dst->colorTex);

      // clamp, point filter
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _newWidth, _newHeight, 0, GL_RGBA, GL_FLOAT, 0);


      glBindTexture(GL_TEXTURE_2D, dst->depthBuf);

      // clamp, point filter
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

      glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, _newWidth, _newHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);


      glBindTexture(GL_TEXTURE_2D, dst->depthTex);

      // clamp, point filter
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

      glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, _newWidth, _newHeight, 0, GL_R, GL_FLOAT, 0);

      ACG::glCheckErrors();

      // fbo
      if (!dst->fbo)
      {
        glGenFramebuffers(1, &dst->fbo);

        glBindFramebuffer(GL_FRAMEBUFFER, dst->fbo);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, dst->colorTex, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, dst->depthTex, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, dst->depthBuf, 0);


        GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if(fboStatus != GL_FRAMEBUFFER_COMPLETE)
          printf("ViewWorld: fbo failed to initialize : %d\n", fboStatus);
      }
    }


    if (!view->peelBlendTex_)
      glGenTextures(1, &view->peelBlendTex_);

    if (view->peelBlendTex_)
    {
      glBindTexture(GL_TEXTURE_2D, view->peelBlendTex_);

      // clamp, point filter
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _newWidth, _newHeight, 0, GL_RGBA, GL_FLOAT, 0);

    }

    if (!view->peelBlendFbo_)
    {
      glGenFramebuffers(1, &view->peelBlendFbo_);

      glBindFramebuffer(GL_FRAMEBUFFER, view->peelBlendFbo_);

      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, view->peelBlendTex_, 0);
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, view->peelTargets_[0].depthTex, 0);
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, view->peelTargets_[0].depthBuf, 0);

      GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
      if(fboStatus != GL_FRAMEBUFFER_COMPLETE)
        printf("FrontDepthPeeling: fbo failed to initialize : 0x%X\n", fboStatus);
    }
  }
  else
  {
    // dual peeling render targets

    if (!view->dualDepthTex_[0])
      glGenTextures(2, view->dualDepthTex_);

    if (!view->dualFrontTex_[0]) 
      glGenTextures(2, view->dualFrontTex_);

    if (!view->dualBackTex_[0]) 
      glGenTextures(2, view->dualBackTex_);

    if (!view->dualBlendTex_) 
      glGenTextures(1, &view->dualBlendTex_);

    for (int i = 0; i < 2; ++i)
    {
      // depth texture
      glBindTexture(GL_TEXTURE_2D, view->dualDepthTex_[i]);

      // texture access: clamped
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
      // filter: none
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

      glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, view->glWidth_, view->glHeight_, 0, GL_RGB, GL_FLOAT, 0);


      ACG::glCheckErrors();


      // front color texture
      glBindTexture(GL_TEXTURE_2D, view->dualFrontTex_[i]);

      // texture access: clamped
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
      // filter: none
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, view->glWidth_, view->glHeight_, 0, GL_RGBA, GL_FLOAT, 0);


      ACG::glCheckErrors();


      // back color texture
      glBindTexture(GL_TEXTURE_2D, view->dualBackTex_[i]);

      // texture access: clamped
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
      // filter: none
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, view->glWidth_, view->glHeight_, 0, GL_RGBA, GL_FLOAT, 0);

      ACG::glCheckErrors();
    }


    // back color-blend texture
    glBindTexture(GL_TEXTURE_2D, view->dualBlendTex_);

    // texture access: clamped
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    // filter: none
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, view->glWidth_, view->glHeight_, 0, GL_RGB, GL_FLOAT, 0);

    ACG::glCheckErrors();



    // fbo
    if (!view->dualFbo_)
    {
      glGenFramebuffersEXT(1, &view->dualFbo_);

      glBindFramebufferEXT(GL_FRAMEBUFFER, view->dualFbo_);

      // color_attachment order:
      // depth0, front_color0, back_color0,   depth1, front_color1, back_color1,    back_color_blend

      glFramebufferTexture2DEXT(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, view->dualDepthTex_[0], 0);
      glFramebufferTexture2DEXT(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, view->dualFrontTex_[0], 0);
      glFramebufferTexture2DEXT(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, view->dualBackTex_[0], 0);
      
      glFramebufferTexture2DEXT(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, view->dualDepthTex_[1], 0);
      glFramebufferTexture2DEXT(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, view->dualFrontTex_[1], 0);
      glFramebufferTexture2DEXT(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, GL_TEXTURE_2D, view->dualBackTex_[1], 0);

      glFramebufferTexture2DEXT(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT6, GL_TEXTURE_2D, view->dualBlendTex_, 0);

      GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
      if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
        printf("DualDepthPeeling: fbo failed to initialize : 0x%X\n", fboStatus);

      ACG::glCheckErrors();
    }

  }
  



  // debugging utilities
/*
  if (!dbgProg_)
  {
    const char* ShaderFiles[] = 
    {
      "DepthPeeling/screenquad.glsl",
      "DepthPeeling/dbg_shader.glsl",
    };

    GLSL::Shader* tempShaders[2] = {0};

    for (int i = 0; i < 2; ++i)
    {
      QString shaderFile = OpenFlipper::Options::shaderDirStr() + QDir::separator() + QString(ShaderFiles[i]);

      if (i == 0) 
        tempShaders[i] = GLSL::loadVertexShader(shaderFile.toUtf8());
      else 
        tempShaders[i] = GLSL::loadFragmentShader(shaderFile.toUtf8());

      if (!tempShaders[i]) {
        log(LOGERR, QString(ShaderFiles[i]) + QString(" could not be loaded and compiled"));
        return;
      }
    }

    dbgProg_ = new GLSL::Program();
    dbgProg_->attach(tempShaders[0]);
    dbgProg_->attach(tempShaders[1]);
    dbgProg_->link();
  }
*/  


  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  ACG::glCheckErrors();
}



void DepthPeeling::initDepthPeeling()
{
  // register shader modifiers
  ShaderProgGenerator::registerModifier(&PeelInitModifier::instance);
  ShaderProgGenerator::registerModifier(&PeelLayerModifier::instance);

  const char* ShaderFiles[] = 
  {
    "DepthPeeling/screenquad.glsl",
    "DepthPeeling/blend.glsl",
    "DepthPeeling/final.glsl",
  };

  GLSL::Shader* tempShaders[3] = {0};

  for (int i = 0; i < 3; ++i)
  {
    QString shaderFile = OpenFlipper::Options::shaderDirStr() + QDir::separator() + QString(ShaderFiles[i]);

    if (i == 0) // screenquad is a vertex shader
      tempShaders[i] = GLSL::loadVertexShader(shaderFile.toUtf8());
    else // "blend", "final" are fragment shaders
      tempShaders[i] = GLSL::loadFragmentShader(shaderFile.toUtf8());

    if (!tempShaders[i]) {
      log(LOGERR, QString(ShaderFiles[i]) + QString(" could not be loaded and compiled"));
      return;
    }
  }


  // create programs

  if (!peelBlend_)
  {
    peelBlend_ = new GLSL::Program();
    peelBlend_->attach(tempShaders[0]);
    peelBlend_->attach(tempShaders[1]);
    peelBlend_->link();
  }

  if (!peelFinal_)
  {
    peelFinal_ = new GLSL::Program();
    peelFinal_->attach(tempShaders[0]);
    peelFinal_->attach(tempShaders[2]);
    peelFinal_->link();
  }

  for (int i = 0; i < 3; ++i)
    delete tempShaders[i];

  // occ query id
  if (!peelQueryID_)
    glGenQueries(1, &peelQueryID_);

  ACG::glCheckErrors();
}



void DepthPeeling::initDualDepthPeeling()
{
  // register shader modifiers
  ShaderProgGenerator::registerModifier(&PeelDualInitModifier::instance);
  ShaderProgGenerator::registerModifier(&PeelDualLayerModifier::instance);

  const char* ShaderFiles[] = 
  {
    "DepthPeeling/screenquad.glsl",
    "DepthPeeling/blend_dual.glsl",
    "DepthPeeling/final_dual.glsl",
  };

  GLSL::Shader* tempShaders[3] = {0};

  for (int i = 0; i < 3; ++i)
  {
    QString shaderFile = OpenFlipper::Options::shaderDirStr() + QDir::separator() + QString(ShaderFiles[i]);

    if (i == 0) // screenquad is a vertex shader
      tempShaders[i] = GLSL::loadVertexShader(shaderFile.toUtf8());
    else // "blend", "final" are fragment shaders
      tempShaders[i] = GLSL::loadFragmentShader(shaderFile.toUtf8());

    if (!tempShaders[i]) {
      log(LOGERR, QString(ShaderFiles[i]) + QString(" could not be loaded and compiled"));
      return;
    }
  }


  // create programs

  if (!peelBlendDual_)
  {
    peelBlendDual_ = new GLSL::Program();
    peelBlendDual_->attach(tempShaders[0]);
    peelBlendDual_->attach(tempShaders[1]);
    peelBlendDual_->link();
  }

  if (!peelFinalDual_)
  {
    peelFinalDual_ = new GLSL::Program();
    peelFinalDual_->attach(tempShaders[0]);
    peelFinalDual_->attach(tempShaders[2]);
    peelFinalDual_->link();
  }

  for (int i = 0; i < 3; ++i)
    delete tempShaders[i];

  // occ query id
  if (!peelQueryID_)
    glGenQueries(1, &peelQueryID_);

  ACG::glCheckErrors();
}


void DepthPeeling::drawProjQuad(GLSL::Program* _prog)
{
  glBindBuffer(GL_ARRAY_BUFFER, screenQuadVBO_);
  screenQuadDecl_->activateShaderPipeline(_prog);

  glPolygonMode(GL_FRONT, GL_FILL);

  glDrawArrays(GL_QUADS, 0, 4);

  screenQuadDecl_->deactivateShaderPipeline(_prog);
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




Q_EXPORT_PLUGIN2( depthpeelingshaderrenderer , DepthPeeling );
