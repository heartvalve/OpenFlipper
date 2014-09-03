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
 *   $Revision$                                                       *
 *   $Author$                                                      *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/

#include <ACG/GL/acg_glew.hh>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <cstdlib>
#include <QFile>
#include <QTextStream>

#include <ACG/GL/gl.hh>

#include <ACG/GL/IRenderer.hh>

#include <ACG/GL/VertexDeclaration.hh>
#include <ACG/GL/GLState.hh>
#include <ACG/GL/GLError.hh>
#include <ACG/Scenegraph/DrawModes.hh>
#include <ACG/Scenegraph/MaterialNode.hh>

#include <ACG/ShaderUtils/GLSLShader.hh>
#include <ACG/GL/ShaderCache.hh>
#include <ACG/GL/ScreenQuad.hh>
#include <ACG/GL/FBO.hh>
#include <ACG/GL/globjects.hh>



namespace ACG
{

IRenderer::IRenderer()
: numLights_(0), 
renderObjects_(0), 
depthMapUsed_(false), 
curViewerID_(0),
prevFbo_(0), 
prevFboSaved_(false),
depthCopyShader_(0),
errorDetectionLevel_(1),
enableLineThicknessGL42_(false)
{
  prevViewport_[0] = 0;
  prevViewport_[1] = 0;
  prevViewport_[2] = 0;
  prevViewport_[3] = 0;

  // set global ambient scale to default OpenGL value
  globalLightModelAmbient_ = ACG::Vec3f(0.2f, 0.2f, 0.2f);
}


IRenderer::~IRenderer()
{
  delete depthCopyShader_;

  // free depth map fbos
  for (std::map<int, ACG::FBO*>::iterator it = depthMaps_.begin(); it != depthMaps_.end(); ++it)
    delete it->second;
}

void IRenderer::addRenderObject(ACG::RenderObject* _renderObject)
{
  // do some more checks for error detection
  if (!_renderObject->vertexDecl)
    std::cout << "error: missing vertex declaration in renderobject: " << _renderObject->debugName << std::endl;
  else
  {
    if (errorDetectionLevel_ > 0)
    {
      // commonly encountered rendering errors

      //  Why is my object invisible/black?
      if (!_renderObject->depthWrite && 
        !_renderObject->colorWriteMask[0] && !_renderObject->colorWriteMask[1] &&
        !_renderObject->colorWriteMask[2] && !_renderObject->colorWriteMask[3])
        std::cout << "warning: depth write and color write disabled in renderobject: " << _renderObject->debugName << std::endl;

      if (errorDetectionLevel_ > 1 && _renderObject->shaderDesc.shadeMode == SG_SHADE_UNLIT)
      {
        if (_renderObject->emissive.max() < 1e-3f)
          std::cout << "warning: unlit object rendered with black emissive color: " << _renderObject->debugName << std::endl;
        else
        {
          // rendering with clear color

          float clearColor[4];
          glGetFloatv(GL_COLOR_CLEAR_VALUE, clearColor);

          if (checkEpsilon(clearColor[0] - _renderObject->emissive[0]) &&
            checkEpsilon(clearColor[1] - _renderObject->emissive[1]) &&
            checkEpsilon(clearColor[2] - _renderObject->emissive[2]))
          {
            std::cout << "warning: unlit object rendered with clear color as emissive color: " << _renderObject->debugName << std::endl;
            std::cout << "         Should this be intentional, disable color writing instead via obj->glColorMask(0,0,0,0)" << std::endl;
          }
        }
      }


      if (errorDetectionLevel_ > 1)
      {
        // Why is there nothing written to the depth-buffer?
        // this might very well be intentional, so put it in higher level

        if (_renderObject->depthWrite && !_renderObject->depthTest)
        {
          std::cout << "warning: trying to write to depth buffer with depth-testing disabled does not work in: " << _renderObject->debugName << std::endl;
          std::cout << "         If depth-writing was intended, enable depth-testing and set depth-func to GL_ALWAYS" << std::endl;
        }
      }


      // Why are there gl errors and/or nothing gets drawn?
      if (_renderObject->shaderDesc.shadeMode != SG_SHADE_UNLIT)
      {
        // check for normals
        if (!_renderObject->vertexDecl->findElementByUsage(VERTEX_USAGE_NORMAL))
          std::cout << "warning: missing normals for lighting in renderobject: " << _renderObject->debugName << std::endl;
      }

      if (_renderObject->shaderDesc.textured())
      {
        // check for texcoords
        if (!_renderObject->vertexDecl->findElementByUsage(VERTEX_USAGE_TEXCOORD))
          std::cout << "warning: missing texcoords for textured mode in renderobject: " << _renderObject->debugName << std::endl;
      }

      if (_renderObject->shaderDesc.vertexColors)
      {
        // check for vertex colors
        if (!_renderObject->vertexDecl->findElementByUsage(VERTEX_USAGE_COLOR))
          std::cout << "warning: missing colors for vertexcolor mode in renderobject: " << _renderObject->debugName << std::endl;
      }


      // Why is alpha blending not work?
      if (fabsf(_renderObject->alpha - 1.0f) > 1e-3f && !(_renderObject->alphaTest || _renderObject->blending))
        std::cout << "warning: alpha value != 1 but no alpha blending or testing enabled in renderobject: " << _renderObject->debugName << std::endl;
    }


    renderObjects_.push_back(*_renderObject);


    ACG::RenderObject* p = &renderObjects_.back();

    if (!p->shaderDesc.numLights)
      p->shaderDesc.numLights = numLights_;

    else if (p->shaderDesc.numLights < 0 || p->shaderDesc.numLights >= SG_MAX_SHADER_LIGHTS)
      p->shaderDesc.numLights = 0;

    p->internalFlags_ = 0;

    // precompile shader
    GLSL::Program* shaderProg = ACG::ShaderCache::getInstance()->getProgram(&p->shaderDesc);



    // check primitive type and geometry shader
    if (errorDetectionLevel_ > 1 && p->shaderDesc.geometryTemplateFile.length())
    {
#ifdef GL_VERSION_3_2
      GLint geomInputType = 0;
      glGetProgramiv(shaderProg->getProgramId(), GL_GEOMETRY_INPUT_TYPE, &geomInputType);


      if (geomInputType != p->primitiveMode)
      {

        switch (geomInputType)
        {
        case GL_POINTS: std::cout << "warning: primitive mode is incompatible with points-geometryshader in renderobject: " << _renderObject->debugName << std::endl; break;
        
        case GL_LINES: 
          {
            if (p->primitiveMode != GL_LINE_STRIP && p->primitiveMode != GL_LINE_LOOP)
              std::cout << "warning: primitive mode is incompatible with lines-geometryshader in renderobject: " << _renderObject->debugName << std::endl;
          } break;

        case GL_LINES_ADJACENCY:
          {
            if (p->primitiveMode != GL_LINE_STRIP_ADJACENCY)
              std::cout << "warning: primitive mode is incompatible with lines_adjacency-geometryshader in renderobject: " << _renderObject->debugName << std::endl;
          } break;

        case GL_TRIANGLES:
          {
            if (p->primitiveMode != GL_TRIANGLE_STRIP && p->primitiveMode != GL_TRIANGLE_FAN)
              std::cout << "warning: primitive mode is incompatible with triangles-geometryshader in renderobject: " << _renderObject->debugName << std::endl;
          } break;
          
        case GL_TRIANGLES_ADJACENCY:
          {
            if (p->primitiveMode != GL_TRIANGLE_STRIP_ADJACENCY)
              std::cout << "warning: primitive mode is incompatible with triangles_adjacency-geometryshader in renderobject: " << _renderObject->debugName << std::endl;
          } break;

        default: std::cout << "warning: unknown input_type for geometryshader in renderobject: " << _renderObject->debugName << std::endl;
        }
        
        

      }


#else
      if (_renderObject->isDefaultLineObject())
      {
        if (_renderObject->primitiveMode != GL_LINES && _renderObject->primitiveMode != GL_LINE_STRIP && _renderObject->primitiveMode != GL_LINE_LOOP)
          std::cout << "warning: primitive mode is incompatible with lines-geometryshader in renderobject: " << _renderObject->debugName << std::endl;
      }
      else if (_renderObject->isDefaultPointObject())
      {
        if (_renderObject->primitiveMode != GL_POINTS)
          std::cout << "warning: primitive mode is incompatible with points-geometryshader in renderobject: " << _renderObject->debugName << std::endl;
      }
#endif
    }

  }
}




void IRenderer::collectRenderObjects( ACG::GLState* _glState, ACG::SceneGraph::DrawModes::DrawMode _drawMode, ACG::SceneGraph::BaseNode* _sceneGraphRoot )
{
  // collect light sources
//  collectLightNodes(_sceneGraphRoot);
  numLights_ = 0; // reset light counter

//  // flush render objects
//  for (size_t i = 0; i < renderObjects_.size(); ++i)
//  {
//    renderObjects_[i].uniformPool_.clear();
//  }
  renderObjects_.resize(0);


  // default material needed
  ACG::SceneGraph::Material defMat;
  defMat.baseColor(ACG::Vec4f(0.0f, 0.0f, 0.0f, 1.0f));
  defMat.ambientColor(ACG::Vec4f(0.2f, 0.2f, 0.2f, 1.0f));
  defMat.diffuseColor(ACG::Vec4f(0.6f, 0.6f, 0.6f, 1.0f));
  defMat.specularColor(ACG::Vec4f(0.0f, 0.0f, 0.0f, 1.0f));
  defMat.shininess(1.0f);
  //  defMat.alphaValue(1.0f);

  // collect renderables
  traverseRenderableNodes(_glState, _drawMode, _sceneGraphRoot, &defMat);
}





void IRenderer::traverseRenderableNodes( ACG::GLState* _glState, ACG::SceneGraph::DrawModes::DrawMode _drawMode, ACG::SceneGraph::BaseNode* _node, const ACG::SceneGraph::Material* _mat )
{
  if (_node)
  {
    ACG::SceneGraph::BaseNode::StatusMode status(_node->status());

    ACG::SceneGraph::DrawModes::DrawMode nodeDM = _node->drawMode();

    if (nodeDM == ACG::SceneGraph::DrawModes::DEFAULT)
      nodeDM = _drawMode;


    // If the subtree is hidden, ignore this node and its children while rendering
    if (status != ACG::SceneGraph::BaseNode::HideSubtree)
    {

      if ( _node->status() != ACG::SceneGraph::BaseNode::HideNode )
        _node->enter(*_glState, _drawMode);


      // fetch material (Node itself can be a material node, so we have to
      // set that in front of the nodes own rendering
      ACG::SceneGraph::MaterialNode* matNode = dynamic_cast<ACG::SceneGraph::MaterialNode*>(_node);
      if (matNode)
        _mat = &matNode->material();

      if (_node->status() != ACG::SceneGraph::BaseNode::HideNode)
        _node->getRenderObjects(this, *_glState, nodeDM, _mat);

      // Process children?
      if (status != ACG::SceneGraph::BaseNode::HideChildren)
      {

        ACG::SceneGraph::BaseNode::ChildIter cIt, cEnd(_node->childrenEnd());

        // Process all children which are not second pass
        for (cIt = _node->childrenBegin(); cIt != cEnd; ++cIt)
          if (~(*cIt)->traverseMode() & ACG::SceneGraph::BaseNode::SecondPass)
            traverseRenderableNodes( _glState, _drawMode, *cIt, _mat);

        // Process all children which are second pass
        for (cIt = _node->childrenBegin(); cIt != cEnd; ++cIt)
          if ((*cIt)->traverseMode() & ACG::SceneGraph::BaseNode::SecondPass)
            traverseRenderableNodes( _glState, _drawMode, *cIt, _mat);

      }


      if (_node->status() != ACG::SceneGraph::BaseNode::HideNode )
        _node->leave(*_glState, nodeDM);
    }
  }
}


int IRenderer::cmpPriority(const void* _a, const void* _b)
{
  const ACG::RenderObject* a = *(const ACG::RenderObject**)_a;
  const ACG::RenderObject* b = *(const ACG::RenderObject**)_b;

  return a->priority - b->priority;
}


void IRenderer::prepareRenderingPipeline(ACG::GLState* _glState, ACG::SceneGraph::DrawModes::DrawMode _drawMode, ACG::SceneGraph::BaseNode* _scenegraphRoot)
{
  // First, all render objects get collected.
  collectRenderObjects(_glState, _drawMode, _scenegraphRoot);

  // Check if there is anything to render
  if (renderObjects_.empty())
    return;

  // ==========================================================
  // Sort renderable objects based on their priority
  // Filter for overlay, lines etc.
  // ==========================================================

  const size_t numRenderObjects = getNumRenderObjects(),
    numOverlayObjects = getNumOverlayObjects(),
    numLineObjects = getNumLineGL42Objects();

  // sort for priority
  if (sortedObjects_.size() < numRenderObjects)
    sortedObjects_.resize(numRenderObjects);

  if (overlayObjects_.size() < numOverlayObjects)
    overlayObjects_.resize(numOverlayObjects);

  if (lineGL42Objects_.size() < numLineObjects)
    lineGL42Objects_.resize(numLineObjects);

  // init sorted objects array
  size_t sceneObjectOffset = 0, overlayObjectOffset = 0, lineObjectOffset = 0;
  for (size_t i = 0; i < renderObjects_.size(); ++i)
  {
    if (renderObjects_[i].overlay)
      overlayObjects_[overlayObjectOffset++] = &renderObjects_[i];
    else if (enableLineThicknessGL42_ && numLineObjects && renderObjects_[i].isDefaultLineObject())
    {
      renderObjects_[i].shaderDesc.geometryTemplateFile = "Wireframe/gl42/geometry.tpl";
      renderObjects_[i].shaderDesc.fragmentTemplateFile = "Wireframe/gl42/fragment.tpl";

      // disable color write to fbo, but allow RenderObject to control depth write
      renderObjects_[i].glColorMask(0,0,0,0);

//      sortedObjects_[sceneObjectOffset++] = &renderObjects_[i];
      lineGL42Objects_[lineObjectOffset++] = &renderObjects_[i];
    }
    else
      sortedObjects_[sceneObjectOffset++] = &renderObjects_[i];
  }

  sortRenderObjects();


  // ---------------------------
  // Initialize the render state
  // ---------------------------
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

  // save active fbo and viewport
  saveInputFbo();


  // get global ambient factor
  GLfloat lightModelAmbient[4];
  glGetFloatv(GL_LIGHT_MODEL_AMBIENT, lightModelAmbient);
  globalLightModelAmbient_ = ACG::Vec3f(lightModelAmbient[0], lightModelAmbient[1], lightModelAmbient[2]);


  // search list of render object for objects requiring the scene depth map
  bool requiresSceneDepths = false;

  for (size_t i = 0; i < renderObjects_.size(); ++i)
  {
    if (renderObjects_[i].depthMapUniformName)
      requiresSceneDepths = true;
  }

  // render scene depth map
  if (requiresSceneDepths)
    renderDepthMap(curViewerID_, _glState->viewport_width(), _glState->viewport_height());
}



void IRenderer::finishRenderingPipeline(bool _drawOverlay)
{
  // draw thick lines
  if (enableLineThicknessGL42_)
    renderLineThicknessGL42();

  if (_drawOverlay)
  {
    const int numOverlayObj = getNumOverlayObjects();
    // two-pass overlay rendering:
    // 1. clear depth buffer at pixels of overlay objects
    // 2. render overlay with correct depth-testing

    // 1. pass: clear depth to max value at overlay footprint
    for (int i = 0; i < numOverlayObj; ++i)
    {
      RenderObject* obj = getOverlayRenderObject(i);

      if (obj->depthTest && obj->depthFunc != GL_ALWAYS)
      {
        float depthMax = 1.0f;

        if (obj->depthFunc == GL_GREATER || obj->depthFunc == GL_GEQUAL)
          depthMax = 0.0f;

        // save depth setting of renderobject
        Vec2f depthRange = obj->depthRange;
        bool depthWrite = obj->depthWrite;
        GLenum depthFunc = obj->depthFunc;

        // reset depth to maximal distance by using depth range
        obj->depthRange = Vec2f(depthMax, depthMax);
        obj->depthWrite = true;
        obj->depthFunc = GL_ALWAYS;

        renderObject(obj);

        // restore depth setting
        obj->depthRange = depthRange;
        obj->depthWrite = depthWrite;
        obj->depthFunc = depthFunc;
      }

    }

    // 2. render overlay with correct depth-testing
    for (int i = 0; i < numOverlayObj; ++i)
    {
      RenderObject* obj = getOverlayRenderObject(i);
      renderObject(obj);
    }

  }

  glDepthMask(1);
  glColorMask(1,1,1,1);

  glUseProgram(0);

  glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
  glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);

  // Renderer check:
  // Print a warning if the currently active fbo / viewport is not the same as the saved fbo.
  // Restore to previous fbo and viewport if not done already.

  GLint curFbo;
  GLint curViewport[4];
  glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &curFbo);
  glGetIntegerv(GL_VIEWPORT, curViewport);
  
  if (curFbo != prevFbo_)
  {
    std::cout << "warning: input and output fbo are not the same in renderer implementation" << std::endl;
    restoreInputFbo();
  }

  if (curViewport[0] != prevViewport_[0] ||
    curViewport[1] != prevViewport_[1] ||
    curViewport[2] != prevViewport_[2] ||
    curViewport[3] != prevViewport_[3])
  {
    std::cout << "warning: input and output viewport are not the same in renderer implementation" << std::endl;
    restoreInputFbo();
  }
}


void IRenderer::saveInputFbo()
{
  // save active fbo
  saveActiveFbo(&prevFbo_, prevViewport_);
  prevFboSaved_ = true;
}

void IRenderer::restoreInputFbo()
{
  if (prevFboSaved_)
    restoreFbo(prevFbo_, prevViewport_);
}

void IRenderer::saveActiveFbo( GLint* _outFboId, GLint* _outViewport ) const
{
  // save active fbo
  glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, _outFboId);
  glGetIntegerv(GL_VIEWPORT, _outViewport);
}

void IRenderer::restoreFbo( GLint _fboId, const GLint* _outViewport ) const
{
  glBindFramebuffer(GL_FRAMEBUFFER, _fboId);
  glDrawBuffer(_fboId == 0 ? GL_BACK : GL_COLOR_ATTACHMENT0);
  glViewport(_outViewport[0], _outViewport[1], _outViewport[2], _outViewport[3]);
}

void IRenderer::clearInputFbo( const ACG::Vec4f& clearColor )
{
  glClearColor(clearColor[0], clearColor[1], clearColor[2], 1.0f);

  // glClear will affect the whole back buffer, not only the area in viewport.
  // Temporarily use glScissor to only clear the viewport area in the back buffer.
  if (!prevFboSaved_)
  {
    GLint oldViewport[4];
    glGetIntegerv(GL_VIEWPORT, oldViewport);
    glScissor(oldViewport[0], oldViewport[1], oldViewport[2], oldViewport[3]);
  }
  else
    glScissor(prevViewport_[0], prevViewport_[1], prevViewport_[2], prevViewport_[3]);

  glEnable(GL_SCISSOR_TEST);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // disable scissors again, draw calls only affect the area in glViewport anyway
  glDisable(GL_SCISSOR_TEST);
}


void IRenderer::sortRenderObjects()
{
  if (!sortedObjects_.empty())
    qsort(&sortedObjects_[0], getNumRenderObjects(), sizeof(ACG::RenderObject*), cmpPriority);
  if (!overlayObjects_.empty())
    qsort(&overlayObjects_[0], getNumOverlayObjects(), sizeof(ACG::RenderObject*), cmpPriority);
}



void IRenderer::bindObjectVBO(ACG::RenderObject* _obj,
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
  _obj->vertexDecl->activateShaderPipeline(_prog);
}


void IRenderer::bindObjectUniforms( ACG::RenderObject* _obj, GLSL::Program* _prog )
{
  // transforms
  ACG::GLMatrixf mvp = _obj->proj * _obj->modelview;
  ACG::GLMatrixf mvIT = _obj->modelview;
  mvIT.invert();
  mvIT.transpose();

  _prog->setUniform("g_mWVP", mvp);
  _prog->setUniform("g_mWV", _obj->modelview);
  _prog->setUniformMat3("g_mWVIT", mvIT);
  _prog->setUniform("g_mP", _obj->proj);


  // material
  
  // modify the emissive color such that it also contains the global ambient light model
  ACG::Vec3f emissiveA = _obj->emissive + globalLightModelAmbient_ * _obj->ambient;
  _prog->setUniform("g_cEmissive", emissiveA);

  _prog->setUniform("g_cDiffuse", _obj->diffuse);
  _prog->setUniform("g_cAmbient", _obj->ambient);
  _prog->setUniform("g_cSpecular", _obj->specular);

  ACG::Vec4f materialParams(_obj->shininess, _obj->alpha, 0.0f, 0.0f);
  _prog->setUniform("g_vMaterial", materialParams);


  // Additional Uniforms defined in the render Object
  if ( !_obj->uniformPool_.empty() )
    _obj->uniformPool_.bind(_prog);

  // textures:

  // maybe consider using bindless textures some time in the future
  // to get rid of the old and problematic glActiveTexture(), glBindTexture() state machine usage:
  // https://www.opengl.org/registry/specs/ARB/bindless_texture.txt

  int maxTextureStage = 0;
  for (std::map<size_t,RenderObject::Texture>::const_iterator iter = _obj->textures().begin();
      iter != _obj->textures().end();++iter)
  {
    //check for valid texture id
    const size_t texture_stage = iter->first;
    const RenderObject::Texture tex = iter->second;
    if (!tex.id)
      continue;

    glActiveTexture(GL_TEXTURE0 + (GLenum)texture_stage);
    glBindTexture(iter->second.type, tex.id);
    _prog->setUniform(QString("g_Texture%1").arg(texture_stage).toStdString().c_str(), (int)texture_stage);

    maxTextureStage = std::max(maxTextureStage, (int)texture_stage);
  }


  // scene depth texture
  if (_obj->depthMapUniformName)
  {
    // bind to (hopefully) unused texture stage
    int depthMapSlot = maxTextureStage + 1;

    _prog->setUniform(_obj->depthMapUniformName, depthMapSlot);
    glActiveTexture(GL_TEXTURE0 + depthMapSlot);
    glBindTexture(GL_TEXTURE_2D, depthMaps_[curViewerID_]->getAttachment(GL_COLOR_ATTACHMENT0));
  }


  // lights
  for (int i = 0; i < numLights_; ++i)
  {
    LightData* lgt = lights_ + i;

    char szUniformName[256];

    sprintf(szUniformName, "g_cLightDiffuse_%d", i);
    _prog->setUniform(szUniformName, lgt->diffuse);

    sprintf(szUniformName, "g_cLightAmbient_%d", i);
    _prog->setUniform(szUniformName, lgt->ambient);

    sprintf(szUniformName, "g_cLightSpecular_%d", i);
    _prog->setUniform(szUniformName, lgt->specular);


    if (lgt->ltype == ACG::SG_LIGHT_POINT || lgt->ltype == ACG::SG_LIGHT_SPOT)
    {
      sprintf(szUniformName, "g_vLightPos_%d", i);
      _prog->setUniform(szUniformName, lgt->pos);

      sprintf(szUniformName, "g_vLightAtten_%d", i);
      _prog->setUniform(szUniformName, lgt->atten);
    }

    if (lgt->ltype == ACG::SG_LIGHT_DIRECTIONAL || lgt->ltype == ACG::SG_LIGHT_SPOT)
    {
      sprintf(szUniformName, "g_vLightDir_%d", i);
      _prog->setUniform(szUniformName, lgt->dir);
    }

    if (lgt->ltype == ACG::SG_LIGHT_SPOT)
    {
      sprintf(szUniformName, "g_vLightAngleExp_%d", i);
      _prog->setUniform(szUniformName, lgt->spotCutoffExponent);
    }
  }
}

void IRenderer::bindObjectRenderStates(ACG::RenderObject* _obj)
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
  {
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(_obj->alphaFunc, _obj->alphaRef);
  }
  else
    glDisable(GL_ALPHA_TEST);

  if (_obj->depthTest)
    glEnable(GL_DEPTH_TEST);
  else
    glDisable(GL_DEPTH_TEST);


  glDepthMask(_obj->depthWrite ? GL_TRUE : GL_FALSE);

  glColorMask(_obj->colorWriteMask[0], _obj->colorWriteMask[1], _obj->colorWriteMask[2], _obj->colorWriteMask[3]);

  glDepthFunc(_obj->depthFunc);

  glDepthRange(_obj->depthRange[0], _obj->depthRange[1]);

  //  ACG::GLState::shadeModel(_obj->shadeModel);

  glBlendFunc(_obj->blendSrc, _obj->blendDest);
}

void IRenderer::drawObject(ACG::RenderObject* _obj)
{
  if (_obj->numIndices)
  {
    // indexed drawing?
    bool noIndices = true;
    if (_obj->indexBuffer || _obj->sysmemIndexBuffer)
      noIndices = false;

    glPolygonMode(GL_FRONT_AND_BACK, _obj->fillMode);

    if (noIndices) {
      glDrawArrays(_obj->primitiveMode, _obj->indexOffset, _obj->numIndices);
    }
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
  else
  {
    // user defined draw-call
    _obj->executeImmediateMode();
  }
}

void IRenderer::renderObject(ACG::RenderObject* _obj, 
                                      GLSL::Program* _prog,
                                      bool _constRenderStates,
                                      unsigned int _shaderModifiers)
{
  // select shader from cache
  GLSL::Program* prog = _prog ? _prog : ACG::ShaderCache::getInstance()->getProgram(&_obj->shaderDesc, _shaderModifiers);


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


  ACG::glCheckErrors();

  // deactivate vertex declaration to avoid errors
  _obj->vertexDecl->deactivateShaderPipeline(prog);

}


void IRenderer::addLight(const LightData& _light)
{
  if (numLights_ < SG_MAX_SHADER_LIGHTS)
  {
    lights_[numLights_] = _light;

    // normalize direction
    if (_light.ltype != SG_LIGHT_POINT)
      lights_[numLights_].dir.normalize();

    ++numLights_;
  }
}


int IRenderer::getNumRenderObjects() const
{
  int n = 0;
  for (size_t i = 0; i < renderObjects_.size(); ++i)
    if (!renderObjects_[i].overlay && !(renderObjects_[i].isDefaultLineObject() && enableLineThicknessGL42_))
//    if (!renderObjects_[i].overlay)
      ++n;

  return n;
}

int IRenderer::getNumOverlayObjects() const
{
  int n = 0;
  for (size_t i = 0; i < renderObjects_.size(); ++i)
    if (renderObjects_[i].overlay)
      ++n;

  return n;
}

int IRenderer::getNumLineGL42Objects() const
{
  int n = 0;

  if (enableLineThicknessGL42_)
  {
    for (size_t i = 0; i < renderObjects_.size(); ++i)
      if (renderObjects_[i].isDefaultLineObject())
        ++n;
  }

  return n;
}

int IRenderer::getNumLights() const
{
  return numLights_;
}


ACG::RenderObject* IRenderer::getRenderObject( int i )
{
  if (sortedObjects_.empty())
    return &renderObjects_[i];
  
  return sortedObjects_[i];
}

ACG::RenderObject* IRenderer::getOverlayRenderObject( int i )
{
  if (overlayObjects_.empty())
    return &renderObjects_[i];

  return overlayObjects_[i];
}

ACG::RenderObject* IRenderer::getLineGL42RenderObject( int i )
{
  if (lineGL42Objects_.empty())
    return 0;

  return lineGL42Objects_[i];
}

IRenderer::LightData* IRenderer::getLight( int i )
{
  return &lights_[i];
}


void IRenderer::dumpRenderObjectsToFile(const char* _fileName, ACG::RenderObject** _sortedList) const
{
  QFile fileOut(_fileName);
  if (fileOut.open(QFile::WriteOnly | QFile::Truncate))
  {
    QTextStream outStrm(&fileOut);
    for (int i = 0; i < getNumRenderObjects(); ++i)
    {
      if (_sortedList)
        outStrm << "\n" << _sortedList[i]->toString();
      else
        outStrm << "\n" << renderObjects_[i].toString();
    }

    fileOut.close();
  }
}


QString IRenderer::dumpCurrentRenderObjectsToString(ACG::RenderObject** _list, bool _outputShaders, std::vector<ACG::ShaderModifier*>* _modifiers) {

  QString objectString;

  QTextStream outStrm(&objectString);
  std::vector<ACG::ShaderModifier*>::iterator it;
  unsigned int usage = 0;
  if (_modifiers) {
    for (it = _modifiers->begin(); it != _modifiers->end(); ++it)
      usage |= (*it)->getID();
  }
  for (int i = 0; i < getNumRenderObjects(); ++i)
  {
    const RenderObject* obj = _list ? _list[i] : &renderObjects_[i];

    if (obj) {
      outStrm << "\n" << obj->toString();

      if ( _outputShaders ) {

        outStrm << "\n";

        outStrm << obj->shaderDesc.toString();

        ShaderProgGenerator progGen(&(obj->shaderDesc), usage);

        if (!usage)
          progGen.generateShaders();

        outStrm << "\n---------------------vertex-shader--------------------\n\n";
        for (int i = 0; i < progGen.getVertexShaderCode().size(); ++i)
          outStrm << progGen.getVertexShaderCode()[i] << "\n";
        outStrm << "\n---------------------end-vertex-shader--------------------\n\n";

        if (progGen.hasTessControlShader())
        {
          outStrm << "\n---------------------tesscontrol-shader--------------------\n\n";
          for (int i = 0; i < progGen.getTessControlShaderCode().size(); ++i)
            outStrm << progGen.getTessControlShaderCode()[i] << "\n";
          outStrm << "\n---------------------end-tesscontrol-shader--------------------\n\n";
        }

        if (progGen.hasTessControlShader())
        {
          outStrm << "\n---------------------tesseval-shader--------------------\n\n";
          if (progGen.hasTessEvaluationShader())
            for (int i = 0; i < progGen.getTessEvaluationShaderCode().size(); ++i)
              outStrm << progGen.getTessEvaluationShaderCode()[i] << "\n";
          outStrm << "\n---------------------end-tesseval-shader--------------------\n\n";
        }

        if (progGen.hasGeometryShader())
        {
          outStrm << "\n---------------------geometry-shader--------------------\n\n";
          for (int i = 0; i < progGen.getGeometryShaderCode().size(); ++i)
            outStrm << progGen.getGeometryShaderCode()[i] << "\n";
          outStrm << "\n---------------------end-geometry-shader--------------------\n\n";
        }


        outStrm << "\n---------------------fragment-shader--------------------\n\n";
        for (int i = 0; i < progGen.getFragmentShaderCode().size(); ++i)
          outStrm << progGen.getFragmentShaderCode()[i] << "\n";
        outStrm << "\n---------------------end-fragment-shader--------------------\n\n";
      }

    } 

  }

  return objectString;
}

void IRenderer::copyDepthToBackBuffer( GLuint _depthTex, float _scale /*= 1.0f*/ )
{
  if (!_depthTex) return;

  if (!depthCopyShader_)
    depthCopyShader_ = GLSL::loadProgram("ScreenQuad/screenquad.glsl", "ScreenQuad/depth_copy.glsl");


  if (depthCopyShader_)
  {
    // save important opengl states
    GLint curFbo;
    GLint curViewport[4];
    saveActiveFbo(&curFbo, curViewport);

    GLboolean colorMask[4], depthMask;
    glGetBooleanv(GL_COLOR_WRITEMASK, colorMask);
    glGetBooleanv(GL_DEPTH_WRITEMASK, &depthMask);

    GLboolean depthTestEnable;
    GLint depthFunc;
    glGetBooleanv(GL_DEPTH_TEST, &depthTestEnable);
    glGetIntegerv(GL_DEPTH_FUNC, &depthFunc);

    // write to depth buffer of input fbo
    restoreInputFbo();

    depthCopyShader_->use();
    depthCopyShader_->setUniform("offset", ACG::Vec2f(0.0f, 0.0f));
    depthCopyShader_->setUniform("size", ACG::Vec2f(1.0f, 1.0f));
    depthCopyShader_->setUniform("DepthTex", 0); // depth tex at texture slot 0
    depthCopyShader_->setUniform("DepthSign", _scale);

    // write to depth buffer only, disable writing to color buffer
    glColorMask(0,0,0,0);
    glDepthMask(1);

    // depth test enabled + pass always 
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_ALWAYS);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _depthTex);

    ACG::ScreenQuad::draw(depthCopyShader_);


    // restore important opengl states

    restoreFbo(curFbo, curViewport);

    glColorMask(colorMask[0], colorMask[1], colorMask[2], colorMask[3]);
    glDepthMask(depthMask);

    if (!depthTestEnable)
      glDisable(GL_DEPTH_TEST);

    if (depthFunc != GL_ALWAYS)
      glDepthFunc(depthFunc);

    glBindTexture(GL_TEXTURE_2D, 0);
  }
}


// depth map computation with a modifier

IRenderer::DepthMapPass IRenderer::DepthMapPass::instance;

void IRenderer::DepthMapPass::modifyFragmentEndCode(QStringList* _code)
{
  _code->push_back("outFragment = gl_FragCoord.zzzz;");
}

void IRenderer::renderDepthMap(int _viewerID, int _width, int _height)
{
  ACG::FBO* fbo = depthMaps_[_viewerID];

  // setup fbo for depth map
  if (!fbo)
  {
    fbo = depthMaps_[_viewerID] = new ACG::FBO();

    fbo->bind();
    fbo->attachTexture2D(GL_COLOR_ATTACHMENT0, _width, _height, GL_R32F, GL_RED);
    fbo->addDepthBuffer(_width, _height);
  }
  else
  {
    fbo->bind();
    fbo->resize(_width, _height);
  }

  // make sure modifier is registered
  ACG::ShaderProgGenerator::registerModifier(&DepthMapPass::instance);

  /* note: 
    It is possible to directly read the depth buffer if it was attached as a texture.
    Then, we would disable the color write mask and just write to the depth buffer in this function.

    However, doing this has shown that the actual depth values written to the depth buffer highly depend on the gpu driver.
    Or, at least sampling from a texture with format GL_DEPTH_COMPONENT returns driver dependent values.
    For instance, in the amd-gl driver sampling the depth texture returns the value of gl_FragDepth (as expected).
    But in the nvidia driver, sampling directly from the depth buffer returns something different!

    Therefore we render the value of gl_FragDepth to a custom floating-point texture bound to a color slot in order to get driver independent behavior.
  */

  fbo->bind();
  glDrawBuffer(GL_COLOR_ATTACHMENT0);
  glViewport(0, 0, _width, _height);

  // set depth to max
  glColorMask(1,1,1,1);
  glDepthMask(1);

  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // render z-prepass
  for (int i = 0; i < getNumRenderObjects(); ++i)
  {
    RenderObject* obj = getRenderObject(i);

    if (obj->inZPrePass)
    {
      // apply depth map modifier to get the depth pass shader
      GLSL::Program* depthPassShader = ShaderCache::getInstance()->getProgram(&obj->shaderDesc, DepthMapPass::instance);

      // temporarily prevent read/write access to the same texture (the depth map)
      const char* depthMapUniformName = obj->depthMapUniformName;
      obj->depthMapUniformName = 0;

      if (depthMapUniformName)
      {
        depthPassShader->use();
        depthPassShader->setUniform(depthMapUniformName, 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
      }

      // we are interested in the depth value only, so temporarily modify the write mask to allow writing to the red channel
      // (for instance, an object might not write a color value, but a depth value)
      const GLboolean redWriteMask = obj->colorWriteMask[0];
      obj->colorWriteMask[0] = obj->depthWrite;

      renderObject(obj, depthPassShader);

      // reset modified render object states
      obj->depthMapUniformName = depthMapUniformName;
      obj->colorWriteMask[0]= redWriteMask;
    }
  }

  // restore input fbo state

  fbo->unbind();

  restoreInputFbo();
}


void IRenderer::setViewerID(int _viewerID)
{
  curViewerID_ = _viewerID;
}


void IRenderer::renderLineThicknessGL42()
{
#ifdef GL_ARB_shader_image_load_store
  // quad extrusion has depth clipping issue
  // GL4.2 method: manually rasterize thick lines into load/store texture, thereby avoiding clipping

  // experimental technique, needs some improvements for rasterization
  /* possible improvement: 
    0. init depth buffer with scene objects
    1. extract visible line object (startpos, endpos) into imageBuffer after depth-test in fragment shader
    1a. line segment id can be found via atomic counter in geometry shader
    1b. use imageAtomicMin/max to find start and end pixel pos of each line segment
         atomic ops work with uint only, thus each segment has 4 texels in the imageBuffer,  offsets are segmentID * 4 + {0,1,2,3}
    2. read imageBuffer in geometry shader and do usual quad extrusion, now without depth testing
        => get hw rasterization and msaa
  */

  //  using image2D does not work: texture always reads 0, even with glGetTexImage2D
  //  imageBuffer works
  //  - check with different gpu

  const int numLines = getNumLineGL42Objects();

  // configs
  const bool useBufferTexture = true;  // imageBuffer or image2D
  const bool useIntegerTexture = true; // color as R32U or RGBA32F



  if (numLines)
  {

    // macro configs for shader
    QStringList macros;

    if (useBufferTexture)
      macros.push_back("#define IMAGE_BUFFER");
    if (useIntegerTexture)
      macros.push_back("#define FMT_UINT");


    // get random access write buffer
    //  32bit uint per viewport pixel, stores rgba8
    Texture2D* lineColorImg2D = 0;
    TextureBuffer* lineColorImgBuf = 0;

    GLsizei w = prevViewport_[2], h = prevViewport_[3];
    GLsizei lineBPP = useIntegerTexture ? 4 : 16; // bytes per pixel


    if (useBufferTexture)
    {
      lineColorImgBuf = dynamic_cast<TextureBuffer*>(lineColorBuffers_[curViewerID_]);

      if (!lineColorImgBuf)
      {
        lineColorImgBuf = new TextureBuffer(GL_TEXTURE0);
        lineColorBuffers_[curViewerID_] = lineColorImgBuf;
      }

      // resize
      if (lineColorImgBuf->getBufferSize() != w * h * lineBPP)
        lineColorImgBuf->setBufferData(w*h*lineBPP, 0, GL_R32UI, GL_DYNAMIC_DRAW);
    }
    else
    {
      lineColorImg2D = dynamic_cast<Texture2D*>(lineColorBuffers_[curViewerID_]);

      if (!lineColorImg2D)
      {
        // allocate and store in map
        lineColorImg2D = new Texture2D(GL_TEXTURE0);
        lineColorBuffers_[curViewerID_] = lineColorImg2D;
      }

      // resize
      if (lineColorImg2D->getWidth() != w || lineColorImg2D->getHeight() != h)
        lineColorImg2D->setData(0,
          useIntegerTexture ? GL_R32UI : GL_RGBA32F,
          w, h, 
          useIntegerTexture ? GL_RED_INTEGER : GL_RGBA, 
          useIntegerTexture ? GL_UNSIGNED_INT : GL_FLOAT, 
          0);  
    }
    

    


    glViewport(0, 0, w, h);



    // ---------------------------
    //  clear line color buffer

    glColorMask(0,0,0,0);
    glDepthMask(0);
    glDisable(GL_DEPTH_TEST);

    // image binding is set to slot 0 in shader already
    if (useBufferTexture)
      lineColorImgBuf->bindAsImage(0, GL_WRITE_ONLY);
    else
      lineColorImg2D->bindAsImage(0, GL_WRITE_ONLY);

    GLSL::Program* shaderClear = ShaderCache::getInstance()->getProgram("ScreenQuad/screenquad.glsl", "Wireframe/gl42/clear.glsl", &macros);

    shaderClear->use();
//     shaderClear->setUniform("offset", Vec2f(0,0));
//     shaderClear->setUniform("size", Vec2f(1,1));
    shaderClear->setUniform("screenSize", Vec2f(w,h));

    ScreenQuad::draw(shaderClear);

    glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
//    GLDebug::dumpTexture2D(lineColorBuf->id(), 0, lineColorBuf->getFormat(), lineColorBuf->getType(), lineBPP*w*h, "c:/dbg/lines_clear.dds");
//    GLDebug::dumpBufferData(GL_TEXTURE_BUFFER, lineColorBuf2->getBufferId(), "c:/dbg/lines_clear.bin");

    // ---------------------------
    // 1. pass
    //  render into line color buffer via imageStore,

    for (int i = 0; i < numLines; ++i)
    {
      RenderObject* obj = getLineGL42RenderObject(i);

      renderObject(obj);
    }

    glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
//    GLDebug::dumpTexture2D(lineColorBuf->id(), 0, lineColorBuf->getFormat(), lineColorBuf->getType(), lineBPP*w*h, "c:/dbg/lines_image.dds");
//    GLDebug::dumpBufferData(GL_TEXTURE_BUFFER, lineColorBuf2->getBufferId(), "c:/dbg/lines_image.bin");


    // ---------------------------
    // 2. pass
    //  composition of line colors and fbo

    restoreInputFbo();

    // image binding is set to slot 0 in shader already
    if (useBufferTexture)
      lineColorImgBuf->bindAsImage(0, GL_READ_ONLY);
    else
      lineColorImg2D->bindAsImage(0, GL_READ_ONLY);


    glColorMask(1,1,1,1);
    glDisable(GL_DEPTH_TEST);

    // enable alpha blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    GLSL::Program* shaderComposite = ShaderCache::getInstance()->getProgram("ScreenQuad/screenquad.glsl", "Wireframe/gl42/composite.glsl", &macros);

    shaderComposite->use();
//     shaderComposite->setUniform("offset", Vec2f(0,0));
//     shaderComposite->setUniform("size", Vec2f(1,1));
    shaderComposite->setUniform("screenSize", Vec2f(w,h));

    ScreenQuad::draw(shaderComposite);


  }
#endif
}

void IRenderer::setLineThicknessRenderingGL42( bool _enable )
{
  if (Texture::supportsImageLoadStore() && Texture::supportsTextureBuffer())
    enableLineThicknessGL42_ = _enable;
}

void IRenderer::setErrorDetectionLevel( int _level )
{
  errorDetectionLevel_ = std::max(_level, 0); // clamp to [0,n]
}

int IRenderer::getErrorDetectionLevel() const
{
  return errorDetectionLevel_;
}

} // namespace ACG end

