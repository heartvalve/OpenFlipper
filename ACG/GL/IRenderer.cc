/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2011 by Computer Graphics Group, RWTH Aachen      *
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
#include <ACG/Scenegraph/DrawModes.hh>
#include <ACG/Scenegraph/MaterialNode.hh>

#include <ACG/ShaderUtils/GLSLShader.hh>
#include <ACG/GL/ShaderCache.hh>



namespace ACG
{

IRenderer::IRenderer()
: numLights_(0), renderObjects_(0)
{
}


IRenderer::~IRenderer()
{
}

void IRenderer::addRenderObject(ACG::RenderObject* _renderObject)
{
  // do some more checks for error detection
  if (!_renderObject->vertexDecl)
    std::cout << "error: missing vertex declaration" << std::endl;
  else
  {
    renderObjects_.push_back(*_renderObject);


    ACG::RenderObject* p = &renderObjects_.back();

    if (!p->shaderDesc.numLights)
      p->shaderDesc.numLights = numLights_;

    else if (p->shaderDesc.numLights < 0 || p->shaderDesc.numLights >= SG_MAX_SHADER_LIGHTS)
      p->shaderDesc.numLights = 0;

    p->internalFlags_ = 0;

    // precompile shader
    ACG::ShaderCache::getInstance()->getProgram(&p->shaderDesc);

  }
}




void IRenderer::collectRenderObjects( ACG::GLState* _glState, ACG::SceneGraph::DrawModes::DrawMode _drawMode, ACG::SceneGraph::BaseNode* _sceneGraphRoot )
{
  // collect light sources
//  collectLightNodes(_sceneGraphRoot);
  numLights_ = 0; // reset light counter

  // flush render objects
  for (size_t i = 0; i < renderObjects_.size(); ++i)
  {
    delete renderObjects_[i].uniformPool_;
    renderObjects_[i].uniformPool_ = 0;
  }
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
    bool process_children(status != ACG::SceneGraph::BaseNode::HideChildren);

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

      if (process_children)
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
  // ==========================================================

  const size_t numRenderObjects = getNumRenderObjects();

  // sort for priority
  if (sortedObjects_.size() < numRenderObjects)
    sortedObjects_.resize(numRenderObjects);

  // init sorted objects array
  for (size_t i = 0; i < numRenderObjects; ++i)
    sortedObjects_[i] = &renderObjects_[i];

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


  // size of a rendered point is set in vertex-shader via gl_PointSize
  #ifndef __APPLE__
    glEnable(GL_PROGRAM_POINT_SIZE_ARB);
  #endif

  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glDepthMask(GL_TRUE);
}



void IRenderer::finishRenderingPipeline()
{
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glDrawBuffer(GL_BACK);

  glDepthMask(1);
  glColorMask(1,1,1,1);

  glUseProgram(0);

  glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
  glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
}


void IRenderer::sortRenderObjects()
{
  qsort(&sortedObjects_[0], getNumRenderObjects(), sizeof(ACG::RenderObject*), cmpPriority);
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
  _prog->setUniform("g_cDiffuse", _obj->diffuse);
  _prog->setUniform("g_cAmbient", _obj->ambient);
  _prog->setUniform("g_cEmissive", _obj->emissive);
  _prog->setUniform("g_cSpecular", _obj->specular);

  ACG::Vec4f materialParams(_obj->shininess, _obj->alpha, 0.0f, 0.0f);
  _prog->setUniform("g_vMaterial", materialParams);


  // Additional Uniforms defined in the render Object
  if ( _obj->uniformPool_ )
    _obj->uniformPool_->bind(_prog);

  // texture
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
  }

  _prog->setUniform("g_PointSize", 5.0f);


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
                                      bool _constRenderStates)
{
  // select shader from cache
  GLSL::Program* prog = _prog ? _prog : ACG::ShaderCache::getInstance()->getProgram(&_obj->shaderDesc);


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
    lights_[numLights_++] = _light;
}


int IRenderer::getNumRenderObjects() const
{
  return renderObjects_.size();
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


QString IRenderer::dumpCurrentRenderObjectsToString(ACG::RenderObject** _list, bool _outputShaders ) {

  QString objectString;

  QTextStream outStrm(&objectString);
  for (int i = 0; i < getNumRenderObjects(); ++i)
  {
    if (_list) {
      outStrm << "\n" << _list[i]->toString();

      if ( _outputShaders ) {

        outStrm << "\n";

        // TODO: Remove!!!
//        _list[i]->shaderDesc.geometryShader = true;

        outStrm << _list[i]->shaderDesc.toString();

        ShaderProgGenerator progGen(&(_list[i]->shaderDesc));

        outStrm << "\n---------------------vertex-shader--------------------\n\n";
        for (int i = 0; i < progGen.getVertexShaderCode().size(); ++i)
          outStrm << progGen.getVertexShaderCode()[i] << "\n";
        outStrm << "\n---------------------end-vertex-shader--------------------\n\n";

        outStrm << "\n---------------------geometry-shader--------------------\n\n";
        if ( progGen.hasGeometryShader() )
          for (int i = 0; i < progGen.getGeometryShaderCode().size(); ++i)
            outStrm << progGen.getGeometryShaderCode()[i] << "\n";
        else
          outStrm << "No geometry shader\n";
        outStrm << "\n---------------------end-geometry-shader--------------------\n\n";


        outStrm << "\n---------------------fragment-shader--------------------\n\n";
        for (int i = 0; i < progGen.getFragmentShaderCode().size(); ++i)
          outStrm << progGen.getFragmentShaderCode()[i] << "\n";
        outStrm << "\n---------------------end-fragment-shader--------------------\n\n";
      }

    } else {
      outStrm << "\n" << renderObjects_[i].toString();


      if ( _outputShaders ) {

        outStrm << "\n";

        // TODO: Remove!!!
        //_list[i]->shaderDesc.geometryShader = true;

        outStrm << renderObjects_[i].shaderDesc.toString();

        ShaderProgGenerator progGen(&(renderObjects_[i].shaderDesc));

        outStrm << "\n---------------------vertex-shader--------------------\n\n";
        for (int i = 0; i < progGen.getVertexShaderCode().size(); ++i)
          outStrm << progGen.getVertexShaderCode()[i] << "\n";
        outStrm << "\n---------------------end-vertex-shader--------------------\n\n";

        outStrm << "\n---------------------geometry-shader--------------------\n\n";
        if ( progGen.hasGeometryShader() )
          for (int i = 0; i < progGen.getGeometryShaderCode().size(); ++i)
            outStrm << progGen.getGeometryShaderCode()[i] << "\n";
        else
          outStrm << "No geometry shader\n";
        outStrm << "\n---------------------end-geometry-shader--------------------\n\n";

        outStrm << "\n---------------------fragment-shader--------------------\n\n";
        for (int i = 0; i < progGen.getFragmentShaderCode().size(); ++i)
          outStrm << progGen.getFragmentShaderCode()[i] << "\n";
        outStrm << "\n---------------------end-fragment-shader--------------------\n\n";

      }
    }

  }

  return objectString;
}


} // namespace ACG end

