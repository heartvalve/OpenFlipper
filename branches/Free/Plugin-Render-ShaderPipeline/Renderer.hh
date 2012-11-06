/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2011 by Computer Graphics Group, RWTH Aachen       *
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
*   $Revision: 13374 $                                                       *
*   $LastChangedBy: moebius $                                                *
*   $Date: 2012-01-13 09:38:16 +0100 (Fri, 13 Jan 2012) $                     *
*                                                                            *
\*===========================================================================*/


#pragma once


#include <QObject>

#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/RenderInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>

#include <ACG/Scenegraph/SceneGraph.hh>
#include <ACG/Scenegraph/MaterialNode.hh>
#include <ObjectTypes/Light/LightNode.hh>

#include <ACG/GL/IRenderer.hh>


#include <map>


class Renderer : public QObject, BaseInterface, RenderInterface, LoggingInterface, ACG::IRenderer
{
  Q_OBJECT
    Q_INTERFACES(BaseInterface)
    Q_INTERFACES(RenderInterface)
    Q_INTERFACES(LoggingInterface)

signals:
  // LoggingInterface
  void log(Logtype _type, QString _message);
  void log(QString _message);


public:
  Renderer();
  ~Renderer();

  QString name() { return (QString("Renderer Plugin")); };
  QString description( ) { return (QString(tr("Shader Based Rendering Pipeline (Alpha Version!)"))); };

public slots:
  QString version() { return QString("1.0"); };


private slots:

  //BaseInterface
  void initializePlugin();
  void exit(){}

  // RenderInterface
  void render(ACG::GLState* _glState, Viewer::ViewerProperties& _properties);
  QString rendererName() {return QString("Alpha_Version_ShaderPipeline");}
  void supportedDrawModes(ACG::SceneGraph::DrawModes::DrawMode& _mode) {_mode = ACG::SceneGraph::DrawModes::DEFAULT;}
  
  QString checkOpenGL();


public:

  /**
  Traverses the scenegraph and calls the getRenderObject function of each node.
  Each node can then add multiple renderobjects via addRenderObject to this renderer.
  Also collects all light sources in the scenegraph.

  The currently active list of renderobjects is invalidated too.
  */
  void collectRenderObjects(ACG::GLState* _glState, ACG::SceneGraph::DrawModes::DrawMode _drawMode, ACG::SceneGraph::BaseNode* _sceneGraphRoot);


  /**
  addRenderObject is typically called by a scenegraph node.
  A renderobject is a collection of opengl states, buffers and parameters,
  that correspond to exactly one draw call.
  */
  void addRenderObject(ACG::RenderObject* _renderObject);

protected:

  
  /**
  compare function for qsort
  */
  static int cmpPriority(const void*, const void*);


  /** Find all light nodes in the scene and save them for the next render() call.
      Implemention is not possible here:   SceneGraph::LightSource is not defined yet
  */
  virtual void collectLightNodes(ACG::SceneGraph::BaseNode* _node);


  /** Calls getRenderObjects on each node of the scenegraph recursively.
  */
  void traverseRenderableNodes(ACG::GLState* _glState, ACG::SceneGraph::DrawModes::DrawMode _drawMode, ACG::SceneGraph::BaseNode* _node, const ACG::SceneGraph::Material* _mat);


  /**
  Bind vertex, index buffer and vertex format of a render object.
  */
  virtual void bindObjectVBO(ACG::RenderObject* _obj,
                             GLSL::Program*     _prog);

  /**
  Set common shader constants like model-view-projection matrix,
  material colors and light params.
  */
  virtual void bindObjectUniforms(ACG::RenderObject* _obj,
                                  GLSL::Program*     _prog);

  /**
  Prepare the opengl state machine for a renderobject draw call.
  This includes any glEnable/glDisable states, depth-cmp functions, blend equation..
  */
  virtual void bindObjectRenderStates(ACG::RenderObject* _obj);

  /**
  Executes the opengl draw call.
  */
  virtual void drawObject(ACG::RenderObject* _obj);


  /**
  Fully prepares opengl for a renderobject and executes the draw call.
  This combines bindObjectVBO, bindObjectUniforms...
  Optionally render-states may not be changed, in case depth-peeling or
  similar global shader operations may require a fixed state setting.
  */
  virtual void renderObject(ACG::RenderObject* _obj, GLSL::Program* _prog = 0, bool _constRenderStates = false);


  /** \brief Light Node traverser
   *
   * Finds all Light Nodes in the scenegraph and stores them for later use.
   */
  void traverseLightNodes(ACG::SceneGraph::BaseNode* _node);


  int getNumRenderObjects();




  /// lighting setup
  int numLights_;
  ACG::SceneGraph::LightSource lights_[SG_MAX_SHADER_LIGHTS];
  ACG::ShaderGenLightType lightTypes_[SG_MAX_SHADER_LIGHTS];


  /// array of renderobjects, filled by addRenderObject
  std::vector<ACG::RenderObject> renderObjects_;





  // draw a projected quad in [-1, -1] - [1, 1] range
  void drawProjQuad(GLSL::Program* _prog);

  /// vbo containing a quad in projection space
  GLuint screenQuadVBO_;
  ACG::VertexDeclaration* screenQuadDecl_;


  // single layer depth peeling

  struct PeelLayer
  {
    /// target framebuffer for colorTex and depthTex
    GLuint fbo;

    /// color rendertarget
    GLuint colorTex;
    /// depth-buf rendertarget
    GLuint depthTex;
  };

  /// is depth peeling supported
  bool depthPeelingSupport_;

  /// blends one depth-layer into the current scene target
  GLSL::Program* peelBlend_;

  /// final copy into back-buffer
  GLSL::Program* peelFinal_;

  /** 
  Copy depth-texture of first layer into hardware z-buffer.
  This is needed for post-processing or renderobjects with low priority.
  */
  GLSL::Program* peelDepthCopy_;

  /// occlusion query determining end of peeling (last layer)
  GLuint peelQueryID_;


  /// Collection of framebuffers for each viewport
  struct ViewerResources
  {
    ViewerResources();

    /// viewer window width
    unsigned int glWidth_;

    /// viewer window height
    unsigned int glHeight_;


    // depth peeling textures

    /// ping pong between two consecutive layers
    PeelLayer peelTargets_[2];

    GLuint peelBlendTex_;
    GLuint peelBlendFbo_;
  };




  /// Allocate framebuffers and load shaders for depth-peeling.
  void initDepthPeeling();

  /// Change viewport size for allocated rendertargets.
  void updateViewerResources(int _viewerId, unsigned int _newWidth, unsigned int _newHeight);

  /**
  Stores framebuffer resources for each viewport.
  Mapping: viewerID -> ViewerResources
  */
  std::map<int, ViewerResources> viewerRes_;

};

