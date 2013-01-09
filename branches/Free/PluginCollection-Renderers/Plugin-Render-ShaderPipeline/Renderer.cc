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

Renderer::Renderer()
: numLights_(0), renderObjects_(0)
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

int Renderer::cmpPriority(const void* _a, const void* _b)
{
  const RenderObject* a = *(const RenderObject**)_a;
  const RenderObject* b = *(const RenderObject**)_b;

  return a->priority - b->priority;
}

void Renderer::render(ACG::GLState* _glState, Viewer::ViewerProperties& _properties)
{
  // First, all render objects get collected.
  collectRenderObjects(_glState, _properties.drawMode(), PluginFunctions::getSceneGraphRootNode());

  // Check if there is anything to render
  if (renderObjects_.empty())
    return;

  // ==========================================================
  // Sort renderable objects based on their priority
  // ==========================================================
  const size_t numRenderObjects = renderObjects_.size();

  // sort for priority
  RenderObject** sortedObjects = new RenderObject*[numRenderObjects];

  // init sorted objects
  for (size_t i = 0; i < numRenderObjects; ++i)
    sortedObjects[i] = &renderObjects_[i];

  qsort(sortedObjects, numRenderObjects, sizeof(RenderObject*), cmpPriority);

//	dumpRenderObjectsToText("../../dump_ro.txt", sortedObjects);

  // ==========================================================
  // Now we render them
  // ==========================================================

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

  // clear back buffer
  Vec4f clearColor = _properties.backgroundColor();
  glClearColor(clearColor[0], clearColor[1], clearColor[2], 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // ---------------------------
  // Render objects one by one
  // ---------------------------
  for (size_t i = 0; i < numRenderObjects; ++i)
    renderObject(sortedObjects[i]);

  // ----------------------------------------
  // Restore common opengl state
  // ----------------------------------------
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
  _obj->vertexDecl->activateShaderPipeline(_prog);
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

  _prog->setUniform("g_PointSize", 5.0f);


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
  if (_obj->numIndices)
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
  else
  {
    // user defined draw-call
    _obj->executeImmediateMode();
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


  ACG::glCheckErrors();

  // deactivate vertex declaration to avoid errors
  _obj->vertexDecl->deactivateShaderPipeline(prog);

}

QString Renderer::checkOpenGL()
{
  return QString("");
}

void Renderer::dumpRenderObjectsToText(const char* _fileName, RenderObject** _sortedList) const
{
  QFile fileOut(_fileName);
  if (fileOut.open(QFile::WriteOnly | QFile::Truncate))
  {
    QTextStream outStrm(&fileOut);
    const int numRenderObjects = renderObjects_.size();

    for (int i = 0; i < numRenderObjects; ++i)
    {
      if (_sortedList)
        outStrm << "\n" << _sortedList[i]->toString();
      else
        outStrm << "\n" << renderObjects_[i].toString();
    }

    fileOut.close();
  }
}




Q_EXPORT_PLUGIN2( shaderrenderer , Renderer );
