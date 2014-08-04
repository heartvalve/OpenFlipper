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




//=============================================================================
//
// CLASS PolyLineNodeT
//
// Author:  David Bommes <bommes@cs.rwth-aachen.de>
//
//=============================================================================


#ifndef ACG_POLYLINENODET_HH
#define ACG_POLYLINENODET_HH


//== INCLUDES =================================================================

#include <ACG/Scenegraph/BaseNode.hh>
#include <ACG/Scenegraph/DrawModes.hh>
#include <ACG/GL/VertexDeclaration.hh>
#include <ACG/GL/IRenderer.hh>
#include <ACG/GL/GLPrimitives.hh>

//== FORWARDDECLARATIONS ======================================================

//== NAMESPACES ===============================================================

namespace ACG {
namespace SceneGraph {

//== CLASS DEFINITION =========================================================




/** \class PolyLineNodeT PolyLineNodeT.hh <ACG/.../PolyLineNodeT.hh>

    Brief Description.

    A more elaborate description follows.
*/

template <class PolyLine>
class PolyLineNodeT : public BaseNode
{
public:

  // typedefs for easy access
  typedef typename PolyLine::Point Point;

  /// Constructor
  PolyLineNodeT(PolyLine& _pl, BaseNode* _parent = 0, std::string _name = "<PolyLineNode>");

  /// Destructor
  ~PolyLineNodeT() {}

  PolyLine& polyline() { return polyline_; }

  /// static name of this class
  ACG_CLASSNAME(PolyLineNodeT);

  /// return available draw modes
  DrawModes::DrawMode availableDrawModes() const;

  /// update bounding box
  void boundingBox(Vec3d& _bbMin, Vec3d& _bbMax);

  /// draw lines and normals
  void draw(GLState& /*_state*/,const DrawModes::DrawMode& _drawMode);

  /// picking
  void pick(GLState& _state, PickTarget _target);

  /** \brief Add the objects to the given renderer
    *
    * @param _renderer The renderer which will be used. Add your geometry into this class
    * @param _state    The current GL State when this object is called
    * @param _drawMode The active draw mode
    * @param _mat      Current material
    */
  void getRenderObjects(ACG::IRenderer* _renderer, ACG::GLState&  _state , const ACG::SceneGraph::DrawModes::DrawMode&  _drawMode , const ACG::SceneGraph::Material* _mat);

  /// Trigger an update of the vbo
  void update() { updateVBO_ = true; };


  /** Add custom data to the vertex buffer for rendering with shaders
   *
   * The element description declares the type of data and the name in the shader.
   * The provided buffer has to store data for each vertex: num elements in buffer = polyline().n_vertices()
   * If the stride offset in  _desc.pointer_ is 0, it is assumed that the buffer stores the data without memory alignment.
   * This function does not create a local copy of the buffer, so the provided memory address
   * has to be valid whenever an internal vbo update is triggered.
   * Custom attributes are then available in the vertex shader by accessing the input with name tangentElement.shaderInputName_.
   *
   * Example:
   *
   * in plugin:
   *  have allocated buffer: tangentData_ of type Vec3f*  (member of a class)
   *  
   *  ACG::VertexElement tangentDesc;
   *  tangentDesc.type_ = GL_FLOAT;
   *  tangentDesc.numElements_ = 3;
   *  tangentDesc.usage_ = ACG::VERTEX_USAGE_USER_DEFINED;
   *  tangentDesc.shaderInputName_ = "myTangent";
   *  tangentDesc.pointer_ = 0;
   *
   *  polylineNode->addCustomBuffer(tangentDesc, tangentData_);
   *
   * in vertex shader:
   *  in vec3 myTangent;
   *  ..
   *  
   *
   * @param _desc  type description of an element in the buffer
   * @param _buffer pointer to data buffer, has to be a valid address for the remaining time
   * @return id of custom buffer
   */
  int addCustomBuffer(const ACG::VertexElement& _desc, const void* _buffer);


  /** Update pointer to custom data
   *
   * @param _id  id of the custom buffer; return value of addCustomBuffer
   * @param _buffer pointer to data buffer, has to be a valid address for the remaining time
   */
  void setCustomBuffer(int _id, const void* _buffer);

private:

  void pick_vertices       ( GLState& _state );
  void pick_spheres        ( GLState& _state );
  void pick_spheres_screen ( GLState& _state );
  void pick_edges          ( GLState& _state, unsigned int _offset);

  /// Copy constructor (not used)
  PolyLineNodeT(const PolyLineNodeT& _rhs);

  /// Assignment operator (not used)
  PolyLineNodeT& operator=(const PolyLineNodeT& _rhs);

  /// Buffer organization
  ACG::VertexDeclaration vertexDecl_;

  /// Custom vertex data for shader based rendering
  std::vector< std::pair<ACG::VertexElement, const void*> > customBuffers_;

  /// Map from custom properties in PolyLine to id in customBuffers_
  std::map< typename PolyLine::CustomPropertyHandle, int> polylinePropMap_;

  /** \brief Trigger an update of the vbo
   *
   * If the polyLine is changed, you have to call this function to update the buffers.
   *
   */
  void updateVBO();

  /** \brief Write vertex data for rendering to a buffer
   *
   * @param _vertex index of polyline vertex
   * @param _dst address of vertex in buffer
   */
  void writeVertex(unsigned int _vertex, void* _dst);

private:

  /// The associated poly line
  PolyLine& polyline_;

  /// VBO used to render the poly line
  unsigned int vbo_;

  /// Index buffer for selected vertices
  std::vector<unsigned int> selectedVertexIndexBuffer_;

  /// Index buffer for selected edges
  std::vector<unsigned int> selectedEdgeIndexBuffer_;

  /// Flag to trigger update of vbo
  bool updateVBO_;

  /// Sphere for VertexSphere DrawMode
  GLSphere* sphere_;

  /// This defines a local point spheres draw mode for all polyLine nodes
  DrawModes::DrawMode POINTS_SPHERES;

  /// This defines a local point spheres draw mode for all polyLine nodes with constant screen size
  DrawModes::DrawMode POINTS_SPHERES_SCREEN;
};


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
#if defined(INCLUDE_TEMPLATES) && !defined(ACG_POLYLINENODET_C)
#define ACG_POLYLINENODET_TEMPLATES
#include "PolyLineNodeT.cc"
#endif
//=============================================================================
#endif // ACG_POLYLINENODET_HH defined
//=============================================================================

