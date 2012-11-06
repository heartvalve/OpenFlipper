#include <stdio.h>
#include <string.h>
#include <iostream>
#include <stdlib.h>

#include <ACG/GL/gl.hh>
#include <ACG/ShaderUtils/GLSLShader.hh>

#include "Renderer.hh"
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


// =================================================

// internal flags

#define RENDERFLAG_ALLOW_PEELING 1



// =================================================

Renderer::Renderer()
: numLights_(0), renderObjects_(0),
  screenQuadVBO_(0), screenQuadDecl_(0), depthPeelingSupport_(1),
  peelBlend_(0), peelFinal_(0), peelDepthCopy_(0), peelQueryID_(0)
{
  for (int i = 0; i < SG_MAX_SHADER_LIGHTS; ++i)
    lightTypes_[i] = SG_LIGHT_POINT;
}


Renderer::~Renderer()
{
}


void Renderer::initializePlugin()
{
  ACG::ShaderProgGenerator::setShaderDir(OpenFlipper::Options::shaderDirStr());
}


void Renderer::addRenderObject(RenderObject* _renderObject)
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




void Renderer::collectRenderObjects( GLState* _glState, SceneGraph::DrawModes::DrawMode _drawMode, SceneGraph::BaseNode* _sceneGraphRoot )
{
  // collect light sources
  collectLightNodes(_sceneGraphRoot);

  // flush render objects
  // clear() may actually free memory, resulting in capacity = 0
  renderObjects_.resize(0);


  // default material needed
  ACG::SceneGraph::Material defMat;
  defMat.baseColor(Vec4f(0.0f, 0.0f, 0.0f, 1.0f));
  defMat.ambientColor(Vec4f(0.2f, 0.2f, 0.2f, 1.0f));
  defMat.diffuseColor(Vec4f(0.6f, 0.6f, 0.6f, 1.0f));
  defMat.specularColor(Vec4f(0.0f, 0.0f, 0.0f, 1.0f));
  defMat.shininess(1.0f);
//  defMat.alphaValue(1.0f);

  // collect renderables
  traverseRenderableNodes(_glState, _drawMode, _sceneGraphRoot, &defMat);
}





void Renderer::traverseRenderableNodes( GLState* _glState, SceneGraph::DrawModes::DrawMode _drawMode, SceneGraph::BaseNode* _node, const SceneGraph::Material* _mat )
{
  if (_node)
  {
    SceneGraph::BaseNode::StatusMode status(_node->status());
    bool process_children(status != SceneGraph::BaseNode::HideChildren);

    SceneGraph::DrawModes::DrawMode nodeDM = _node->drawMode();

    if (nodeDM == SceneGraph::DrawModes::DEFAULT)
      nodeDM = _drawMode;


    // If the subtree is hidden, ignore this node and its children while rendering
    if (status != SceneGraph::BaseNode::HideSubtree)
    {

      if ( _node->status() != SceneGraph::BaseNode::HideNode )
        _node->enter(*_glState, _drawMode);


      if (_node->status() != SceneGraph::BaseNode::HideNode)
        _node->getRenderObjects(this, *_glState, nodeDM, _mat);

      // fetch material
      SceneGraph::MaterialNode* matNode = dynamic_cast<SceneGraph::MaterialNode*>(_node);
      if (matNode)
        _mat = &matNode->material();

      if (process_children)
      {

        SceneGraph::BaseNode::ChildIter cIt, cEnd(_node->childrenEnd());

        // Process all children which are not second pass
        for (cIt = _node->childrenBegin(); cIt != cEnd; ++cIt)
          if (~(*cIt)->traverseMode() & SceneGraph::BaseNode::SecondPass)
            traverseRenderableNodes( _glState, _drawMode, *cIt, _mat);

        // Process all children which are second pass
        for (cIt = _node->childrenBegin(); cIt != cEnd; ++cIt)
          if ((*cIt)->traverseMode() & SceneGraph::BaseNode::SecondPass)
            traverseRenderableNodes( _glState, _drawMode, *cIt, _mat);

      }


      if (_node->status() != SceneGraph::BaseNode::HideNode )
        _node->leave(*_glState, nodeDM);
    }
  }
}


int Renderer::cmpPriority(const void* _a, const void* _b)
{
  const RenderObject* a = *(const RenderObject**)_a;
  const RenderObject* b = *(const RenderObject**)_b;

  return a->priority - b->priority;
}



void Renderer::render(ACG::GLState* _glState, Viewer::ViewerProperties& _properties)
{
  collectRenderObjects(_glState, _properties.drawMode(), PluginFunctions::getSceneGraphRootNode());

  if (renderObjects_.empty())
    return;

  const size_t numRenderObjects = getNumRenderObjects();

  // sort for priority
  RenderObject** sortedObjects = new RenderObject*[numRenderObjects];

  // init sorted objects
  for (size_t i = 0; i < numRenderObjects; ++i)
  {
    sortedObjects[i] = &renderObjects_[i];
  }

  qsort(sortedObjects, numRenderObjects, sizeof(RenderObject*), cmpPriority);



  // render

  // gl cleanup

  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_COLOR_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glDisableClientState(GL_INDEX_ARRAY);


  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glDepthMask(GL_TRUE);

  // clear back buffer
  Vec4f clearColor = _properties.backgroundColor();
  glClearColor(clearColor[0], clearColor[1], clearColor[2], 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



  if (depthPeelingSupport_)
  {
    // find last transparent object to peel
    int lastPeeledObject = -1;

    for (int i = numRenderObjects - 1; i > lastPeeledObject; --i)
    {
      if ((sortedObjects[i]->internalFlags_ & RENDERFLAG_ALLOW_PEELING) && sortedObjects[i]->alpha < 0.99f)
        lastPeeledObject = i;
    }

    if (lastPeeledObject == -1)
    {
      // no transparent objects
      for (size_t i = 0; i < numRenderObjects; ++i)
        renderObject(sortedObjects[i]);
    }
    else
    {
      // depth peeling for transparency

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
        if (!(sortedObjects[i]->internalFlags_ & RENDERFLAG_ALLOW_PEELING) || sortedObjects[i]->alpha >= 0.99f)
          renderObject(sortedObjects[i]);
      }


      glDepthMask(1);
      glColorMask(1,1,1,1);

      glEnable(GL_DEPTH_TEST);
      glDepthFunc(GL_LESS);

      glDisable(GL_BLEND);
      glDisable(GL_ALPHA_TEST);

      for (size_t i = 0; i < numRenderObjects; ++i)
      {
        if ((sortedObjects[i]->internalFlags_ & RENDERFLAG_ALLOW_PEELING) && sortedObjects[i]->alpha < 0.99f)
        {
          GLSL::Program* initProg = ShaderCache::getInstance()->getProgram(&sortedObjects[i]->shaderDesc, PeelInitModifier::instance);

          renderObject(sortedObjects[i], initProg, true);
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
        for (size_t k = 0; k < numRenderObjects; ++k)
        {
          if ((sortedObjects[k]->internalFlags_ & RENDERFLAG_ALLOW_PEELING) && sortedObjects[k]->alpha < 0.99f)
          {
            GLSL::Program* peelProg = ShaderCache::getInstance()->getProgram(&sortedObjects[k]->shaderDesc, PeelLayerModifier::instance);
            peelProg->use();
            peelProg->setUniform("g_DepthLayer", 4);

            renderObject(sortedObjects[k], peelProg, true);
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

/*
      // draw rest of opaque objects
      for (size_t i = lastPeeledObject + 1; i < numRenderObjects; ++i)
          renderObject(sortedObjects[i]);
*/
    }



  }
  else
  {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (size_t i = 0; i < numRenderObjects; ++i)
      renderObject(sortedObjects[i]);

  }



  // restore commmon opengl state
  // log window remains hidden otherwise

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glDrawBuffer(GL_BACK);

  glDepthMask(1);
  glColorMask(1,1,1,1);

  glUseProgram(0);

  glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
  glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);




  delete [] sortedObjects;
}



void Renderer::bindObjectVBO(ACG::RenderObject* _obj,
                             GLSL::Program*     _prog)
{
  _prog->use();

  //////////////////////////////////////////////////////////////////////////
  // NOTE:
  //  always bind buffers before glVertexAttribPointer calls!!
  //  freeze in glDrawElements guaranteed (with no error message whatsoever)
  glBindBufferARB(GL_ARRAY_BUFFER_ARB, _obj->vertexBuffer);
  glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, _obj->indexBuffer);


  // activate vertex declaration
  (const_cast<VertexDeclaration*>(_obj->vertexDecl))->activateShaderPipeline(_prog);
}


void Renderer::bindObjectUniforms( ACG::RenderObject* _obj, GLSL::Program* _prog )
{
  // transforms
  GLMatrixf mvp = _obj->proj * _obj->modelview;
  GLMatrixf mvIT = _obj->modelview;
  mvIT.invert();
  mvIT.transpose();

  _prog->setUniform("g_mWVP", mvp);
  _prog->setUniform("g_mWV", _obj->modelview);
  _prog->setUniformMat3("g_mWVIT", mvIT);
  _prog->setUniform("g_mP", _obj->proj);


  // material
  _prog->setUniform("g_cDiffuse", _obj->diffuse);
  _prog->setUniform("g_cAmbient", _obj->ambient);
  _prog->setUniform("g_cEmissive", _obj->emissive);
  _prog->setUniform("g_cSpecular", _obj->specular);

  Vec4f materialParams(_obj->shininess, _obj->alpha, 0.0f, 0.0f);
  _prog->setUniform("g_vMaterial", materialParams);



  // texture
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _obj->texture);
  _prog->setUniform("g_Texture0", 0);


  // lights
  for (int i = 0; i < numLights_; ++i)
  {
    SceneGraph::LightSource* lgt = lights_ + i;
    ShaderGenLightType lgtType = lightTypes_[i];

#define V4toV3(v) (ACG::Vec3f(v[0], v[1], v[2]))

    char szUniformName[256];

    sprintf(szUniformName, "g_cLightDiffuse_%d", i);
    _prog->setUniform(szUniformName, V4toV3(lgt->diffuseColor()));

    sprintf(szUniformName, "g_cLightAmbient_%d", i);
    _prog->setUniform(szUniformName, V4toV3(lgt->ambientColor()));

    sprintf(szUniformName, "g_cLightSpecular_%d", i);
    _prog->setUniform(szUniformName, V4toV3(lgt->specularColor()));


    if (lgtType == SG_LIGHT_POINT || lgtType == SG_LIGHT_SPOT)
    {
      sprintf(szUniformName, "g_vLightPos_%d", i);
      _prog->setUniform(szUniformName, Vec3f(lgt->position()));

      Vec3f atten(lgt->constantAttenuation(), 
        lgt->linearAttenuation(), lgt->quadraticAttenuation());

      sprintf(szUniformName, "g_vLightAtten_%d", i);
      _prog->setUniform(szUniformName, atten);
    }

    if (lgtType == SG_LIGHT_DIRECTIONAL || lgtType == SG_LIGHT_SPOT)
    {
      sprintf(szUniformName, "g_vLightDir_%d", i);
      _prog->setUniform(szUniformName, Vec3f(lgt->direction()));
    }

    if (lgtType == SG_LIGHT_SPOT)
    {
      Vec2f angleexp(lgt->spotCutoff(), lgt->spotExponent());

      sprintf(szUniformName, "g_vLightAngleExp_%d", i);
      _prog->setUniform(szUniformName, angleexp);
    }
  }
}

void Renderer::bindObjectRenderStates(ACG::RenderObject* _obj)
{
  if (_obj->culling)
    glEnable(GL_CULL_FACE);
  else
    glDisable(GL_CULL_FACE);

  if (_obj->blending)
    glEnable(GL_BLEND);
  else
    glDisable(GL_BLEND);

  if (_obj->alphaTest)
    glEnable(GL_ALPHA_TEST);
  else
    glDisable(GL_ALPHA_TEST);

  if (_obj->depthTest)
    glEnable(GL_DEPTH_TEST);
  else
    glDisable(GL_DEPTH_TEST);

  glDepthMask(_obj->depthWrite ? GL_TRUE : GL_FALSE);

  glColorMask(_obj->colorWriteMask[0], _obj->colorWriteMask[1], _obj->colorWriteMask[2], _obj->colorWriteMask[3]);

  glDepthFunc(_obj->depthFunc);

  //  ACG::GLState::shadeModel(_obj->shadeModel);

  glBlendFunc(_obj->blendSrc, _obj->blendDest);
}

void Renderer::drawObject(ACG::RenderObject* _obj)
{
  // indexed drawing?
  bool noIndices = true;
  if (_obj->indexBuffer || _obj->sysmemIndexBuffer)
    noIndices = false;

  glPolygonMode(GL_FRONT_AND_BACK, _obj->fillMode);

  if (noIndices)
    glDrawArrays(_obj->primitiveMode, _obj->indexOffset, _obj->numIndices);
  else
  {
    // ------------------------------------------
    // index offset stuff not tested
    int indexSize = 0;
    switch (_obj->indexType)
    {
    case GL_UNSIGNED_INT: indexSize = 4; break;
    case GL_UNSIGNED_SHORT: indexSize = 2; break;
    default: indexSize = 1; break;
    }

    glDrawElements(_obj->primitiveMode, _obj->numIndices, _obj->indexType,
      ((const char*)_obj->sysmemIndexBuffer) + _obj->indexOffset * indexSize);
  }
}

void Renderer::renderObject(ACG::RenderObject* _obj, 
                            GLSL::Program* _prog,
                            bool _constRenderStates)
{
  // select shader from cache
  GLSL::Program* prog = _prog ? _prog : ShaderCache::getInstance()->getProgram(&_obj->shaderDesc);


  bindObjectVBO(_obj, prog);

  // ---------------------------------------
  // set shader uniforms

  bindObjectUniforms(_obj, prog);

  // render states

  if (!_constRenderStates)
    bindObjectRenderStates(_obj);

  // ----------------------------
  // OpenGL draw call

  drawObject(_obj);


  // deactivate vertex declaration to avoid errors
  _obj->vertexDecl->deactivateShaderPipeline(prog);

}






void Renderer::traverseLightNodes( ACG::SceneGraph::BaseNode* _node )
{
  if (_node && numLights_ < SG_MAX_SHADER_LIGHTS)
  {
    BaseNode::StatusMode status(_node->status());
    bool process_children(status != BaseNode::HideChildren);

    // If the subtree is hidden, ignore this node and its children while rendering
    if (status != BaseNode::HideSubtree)
    {

      if (_node->status() != BaseNode::HideNode)
      {
        ACG::SceneGraph::LightNode* lnode = dynamic_cast<ACG::SceneGraph::LightNode*>(_node);
        if (lnode)
        {
          ACG::SceneGraph::LightSource light;
//          lnode->getLightSource(&light);
          lnode->getLightSourceViewSpace(&light);

          // --------------------------
          // add light to renderer

          if (light.directional())
            lightTypes_[numLights_] = ACG::SG_LIGHT_DIRECTIONAL;
          else if (light.spotCutoff() > 179.5f)
            lightTypes_[numLights_] = ACG::SG_LIGHT_POINT;
          else
            lightTypes_[numLights_] = ACG::SG_LIGHT_SPOT;


          lights_[numLights_] = light;

          ++numLights_;






        }
      }

      if (process_children)
      {

        BaseNode::ChildIter cIt, cEnd(_node->childrenEnd());

        // Process all children which are not second pass
        for (cIt = _node->childrenBegin(); cIt != cEnd; ++cIt)
          if (~(*cIt)->traverseMode() & BaseNode::SecondPass)
            traverseLightNodes(*cIt);

        // Process all children which are second pass
        for (cIt = _node->childrenBegin(); cIt != cEnd; ++cIt)
          if ((*cIt)->traverseMode() & BaseNode::SecondPass)
            traverseLightNodes(*cIt);

      }

    }
  }
}


void Renderer::collectLightNodes( ACG::SceneGraph::BaseNode* _node )
{
  numLights_ = 0;
  traverseLightNodes(_node);
}

int Renderer::getNumRenderObjects()
{
//   int i = 0;
//   for (InternalRenderObject* r = renderObjects_; r; ++i, r = r->next);
//   return i;
  return renderObjects_.size();
}




Renderer::ViewerResources::ViewerResources()
: glWidth_(0), glHeight_(0), peelBlendTex_(0), peelBlendFbo_(0)
{
  memset(peelTargets_, 0, sizeof(peelTargets_));
}

void Renderer::updateViewerResources(int _viewerId,
                                     unsigned int _newWidth,
                                     unsigned int _newHeight)
{
  initDepthPeeling();

  // allocate opengl resources

  // screenquad
  if (!screenQuadDecl_)
  {
    screenQuadDecl_ = new VertexDeclaration();
    screenQuadDecl_->addElement(GL_FLOAT, 3, VERTEX_USAGE_POSITION, 0, 0);
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

  if (depthPeelingSupport_)
  {

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



  }


  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  ACG::glCheckErrors();
}



void Renderer::initDepthPeeling()
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



void Renderer::drawProjQuad(GLSL::Program* _prog)
{
  glBindBuffer(GL_ARRAY_BUFFER, screenQuadVBO_);
  screenQuadDecl_->activateShaderPipeline(_prog);

  glPolygonMode(GL_FRONT, GL_FILL);

  glDrawArrays(GL_QUADS, 0, 4);

  screenQuadDecl_->deactivateShaderPipeline(_prog);
}




QString Renderer::checkOpenGL()
{
  return QString("");
}




Q_EXPORT_PLUGIN2( shaderrenderer , Renderer );
