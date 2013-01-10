
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
  }

  static PeelLayerModifier instance;
};


class PeelInitModifier : public ShaderModifier
{
public:
  void modifyFragmentEndCode(QStringList* _code)
  {
    _code->push_back("outFragment = vec4(sg_cColor.rgb * sg_cColor.a, 1.0 - sg_cColor.a);");
  }

  static PeelInitModifier instance;
};

PeelInitModifier PeelInitModifier::instance;
PeelLayerModifier PeelLayerModifier::instance;

// internal shader-attribute flags

#define RENDERFLAG_ALLOW_PEELING 1


// =================================================

DepthPeeling::DepthPeeling()
: screenQuadVBO_(0), screenQuadDecl_(0),
peelBlend_(0), peelFinal_(0), peelDepthCopy_(0), peelQueryID_(0)
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

  const int numRenderObjects = getNumRenderObjects();



  // clear back buffer
  ACG::Vec4f clearColor = _properties.backgroundColor();
  glClearColor(clearColor[0], clearColor[1], clearColor[2], 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


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
    updateViewerResources(_properties.viewerId(), _glState->viewport_width(), _glState->viewport_height());

    ViewerResources* viewRes = &viewerRes_[_properties.viewerId()];


    // begin peeling
    //  draw  first layer
    //  target depth buffer: viewRes->peelTarget_[0]

    glBindFramebuffer(GL_FRAMEBUFFER, viewRes->peelBlendFbo_);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);

    glDepthMask(1);
    glColorMask(1,1,1,1);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    // draw opaque objects first
    for (int i = 0; i <= lastPeeledObject; ++i)
    {
      if (!(sortedObjects_[i]->internalFlags_ & RENDERFLAG_ALLOW_PEELING) || sortedObjects_[i]->alpha >= 0.99f)
        renderObject(sortedObjects_[i]);
    }


    glDepthMask(1);
    glColorMask(1,1,1,1);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glDisable(GL_BLEND);
    glDisable(GL_ALPHA_TEST);

    for (int i = 0; i < numRenderObjects; ++i)
    {
      if ((sortedObjects_[i]->internalFlags_ & RENDERFLAG_ALLOW_PEELING) && sortedObjects_[i]->alpha < 0.99f)
      {
        GLSL::Program* initProg = ShaderCache::getInstance()->getProgram(&sortedObjects_[i]->shaderDesc, PeelInitModifier::instance);

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
          peelProg->use();
          peelProg->setUniform("g_DepthLayer", 4);

          renderObject(sortedObjects_[k], peelProg, true);
        }
      }

      glEndQuery(GL_SAMPLES_PASSED);



      // 2nd underblend layer with current scene
      //  (fullscreen pass)

      glBindFramebuffer(GL_FRAMEBUFFER, viewRes->peelBlendFbo_);

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


    // draw rest of opaque objects
    for (int i = lastPeeledObject + 1; i < numRenderObjects; ++i)
      renderObject(sortedObjects_[i]);

  }



  // restore common opengl state
  // log window remains hidden otherwise
  finishRenderingPipeline();

//  dumpRenderObjectsToText("../../dump_ro.txt", &sortedObjects_[0]);
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
: glWidth_(0), glHeight_(0), peelBlendTex_(0), peelBlendFbo_(0)
{
  memset(peelTargets_, 0, sizeof(peelTargets_));
}

void DepthPeeling::updateViewerResources(int _viewerId,
                                     unsigned int _newWidth,
                                     unsigned int _newHeight)
{
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
  for (int i = 0; i < 2; ++i)
  {
    PeelLayer* dst = view->peelTargets_ + i;

    if (!dst->colorTex)
      glGenTextures(1, &dst->colorTex);

    if (!dst->depthTex)
      glGenTextures(1, &dst->depthTex);

    if (!dst->colorTex || !dst->depthTex) // out of memory
      continue;


    glBindTexture(GL_TEXTURE_2D, dst->colorTex);

    // clamp, point filter
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _newWidth, _newHeight, 0, GL_RGBA, GL_FLOAT, 0);


    glBindTexture(GL_TEXTURE_2D, dst->depthTex);

    // clamp, point filter
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, _newWidth, _newHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

    ACG::glCheckErrors();

    // fbo
    if (!dst->fbo)
    {
      glGenFramebuffers(1, &dst->fbo);

      glBindFramebuffer(GL_FRAMEBUFFER, dst->fbo);

      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, dst->colorTex, 0);
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, dst->depthTex, 0);


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
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, view->peelTargets_[0].depthTex, 0);

    GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(fboStatus != GL_FRAMEBUFFER_COMPLETE)
      printf("ViewWorld: fbo failed to initialize : %d\n", fboStatus);
  }




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



void DepthPeeling::drawProjQuad(GLSL::Program* _prog)
{
  glBindBuffer(GL_ARRAY_BUFFER, screenQuadVBO_);
  screenQuadDecl_->activateShaderPipeline(_prog);

  glPolygonMode(GL_FRONT, GL_FILL);

  glDrawArrays(GL_QUADS, 0, 4);

  screenQuadDecl_->deactivateShaderPipeline(_prog);
}





Q_EXPORT_PLUGIN2( depthpeelingshaderrenderer , DepthPeeling );
