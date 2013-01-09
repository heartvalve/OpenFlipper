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


/** \brief A simple renderer Example for the shader pipeline
 *
 * It has to derive from ACG::IRenderer as the nodes will will pass back the generated render objects
 * via this objects addRenderObject() function.
 */
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


//=========================================================================
// Callback for the scenegraph nodes
//=========================================================================
public:
  /** \brief callback for the nodes, which send new render objects via this function.
   *
   * AddRenderObject is typically called by a scenegraph nodes during the collection of renderable
   * objects.
   *
   * A renderobject is a collection of opengl states, buffers and parameters, that correspond to exactly one draw call.
  */
  virtual void addRenderObject(ACG::RenderObject* _renderObject);

//=========================================================================
// Render object collection
//=========================================================================
private:

  /** \brief Traverse the scenegraph to collect render information
   *
   * Traverses the scenegraph and calls the getRenderObject function of each node.
   * Each node can then add multiple renderobjects via addRenderObject to this renderer.
   *
   * Also collects all light sources in the scenegraph.
   * The currently active list of renderobjects is invalidated too.
   */
  void collectRenderObjects(ACG::GLState* _glState, ACG::SceneGraph::DrawModes::DrawMode _drawMode, ACG::SceneGraph::BaseNode* _sceneGraphRoot);


  /** \brief Scene graph traversal for render object collection
   *
   * Calls getRenderObjects on each node of the scenegraph recursively.
   */
  void traverseRenderableNodes( ACG::GLState* _glState,
                                ACG::SceneGraph::DrawModes::DrawMode _drawMode,
                                ACG::SceneGraph::BaseNode* _node,
                                const ACG::SceneGraph::Material* _mat);

//=========================================================================
// Light node collection
//=========================================================================
private:

  /** \brief Find all light sources in the scene
   *
   * Find all light nodes in the scene and save them for the next render() call.
   *
   * The lights are collected in lights_ while the corresponding type ends up in lightTypes_
   */
  virtual void collectLightNodes(ACG::SceneGraph::BaseNode* _node);

  /** \brief Light Node traverser
   *
   * Finds all Light Nodes in the scenegraph and stores them for later use. This is a recursive function.
   */
  void traverseLightNodes(ACG::SceneGraph::BaseNode* _node);



//=========================================================================
// Sorting
//=========================================================================
private:

  /** \brief Compare priority of render objects
   *
   * compare function for qsort. This is required to compare render objects based
   * on their prioerity and render them in the right order
  */
  static int cmpPriority(const void*, const void*);

//=========================================================================
// Rendering
//=========================================================================
private:

  /** \brief Binding VBOs (First state function)
   *
   * This is the first function called by renderObject().
   *
   * It binds vertex, index buffer and vertex format of a
   * render object.
   *
   */
  virtual void bindObjectVBO(ACG::RenderObject* _obj,
                             GLSL::Program*     _prog);

  /** \brief Binding Uniforms (Second state function)
   *
   * This is the second function called by renderObject().
   *
   * Set common shader constants like model-view-projection matrix,
   * material colors and light params.
   */
  virtual void bindObjectUniforms(ACG::RenderObject* _obj,
                                  GLSL::Program*     _prog);


  /** \brief Binding Render state (Third state function)
   *
   * This is the third function called by renderObject().
   *
   * Prepare the opengl state machine for a renderobject draw call.
   *
   * This includes any glEnable/glDisable states, depth-cmp functions, blend equation..
   */
  virtual void bindObjectRenderStates(ACG::RenderObject* _obj);

  /** \brief Executes the opengl draw call for one object (Fourth function)
   *
   * This is the fourth function called by renderObject().
   *
   * Executes one draw call for the given render object
   */
  virtual void drawObject(ACG::RenderObject* _obj);

  /** \brief Render one render object
   *
   * Fully prepares opengl for a renderobject and executes the draw call.
   * This combines bindObjectVBO, bindObjectUniforms...
   *
   * Optionally render-states may not be changed, in case depth-peeling or
   * similar global shader operations may require a fixed state setting.
  */
  virtual void renderObject(ACG::RenderObject* _obj, GLSL::Program* _prog = 0, bool _constRenderStates = false);

  //=========================================================================
  // Debugging
  //=========================================================================
private:

  /** \brief Debugging function to dump list of render objects into a file
   *
   * Dump list of render objects to text file.
   * @param _fileName name of text file to write to
   * @param _sortedList dump sorted render objects in order, may be 0 to use the unsorted list instead
   */
  void dumpRenderObjectsToText(const char* _fileName, ACG::RenderObject** _sortedList = 0) const;


  //=========================================================================
  // Variables
  //=========================================================================
private:

  /// Current number of lights  (Filled by collectLightNodes)
  int numLights_;

  /// Light sources (Filled by collectLightNodes)
  ACG::SceneGraph::LightSource lights_[SG_MAX_SHADER_LIGHTS];

  /// Type of light sources (Filled by collectLightNodes)
  ACG::ShaderGenLightType lightTypes_[SG_MAX_SHADER_LIGHTS];


  /// Array of renderobjects  (Filled by addRenderObject)
  std::vector<ACG::RenderObject> renderObjects_;


};

