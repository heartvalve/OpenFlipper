
#include <ACG/GL/gl.hh>
#include <ACG/ShaderUtils/GLSLShader.hh>

#include "Renderer.hh"
#include <ACG/GL/ShaderCache.hh>
#include <ACG/GL/VertexDeclaration.hh>

using namespace ACG;

// =================================================

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


      // fetch material (Node itself can be a material node, so we have to
      // set that in front of the nodes own rendering
      SceneGraph::MaterialNode* matNode = dynamic_cast<SceneGraph::MaterialNode*>(_node);
      if (matNode)
        _mat = &matNode->material();

      if (_node->status() != SceneGraph::BaseNode::HideNode)
        _node->getRenderObjects(this, *_glState, nodeDM, _mat);

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

