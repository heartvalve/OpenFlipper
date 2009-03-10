//=============================================================================
//
//                               OpenFlipper
//        Copyright (C) 2008 by Computer Graphics Group, RWTH Aachen
//                           www.openflipper.org
//
//-----------------------------------------------------------------------------
//
//                                License
//
//  OpenFlipper is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  OpenFlipper is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License
//  along with OpenFlipper.  If not, see <http://www.gnu.org/licenses/>.
//
//-----------------------------------------------------------------------------
//
//   $Revision$
//   $Author$
//   $Date$
//
//=============================================================================




//=============================================================================
//
//  CLASS MeshNodeT
//
//=============================================================================

#ifndef ACG_MESHNODE_HH
#define ACG_MESHNODE_HH

//== INCLUDES =================================================================

#include "BaseNode.hh"
#include <vector>
#include <string>
#include "../ShaderUtils/GLSLShader.hh"


//== NAMESPACES ===============================================================


namespace ACG {
namespace SceneGraph {


//== CLASS DEFINITION =========================================================


/** \class MeshNodeT MeshNodeT.hh <ACG/Scenegraph/MeshNodeT.hh>

    This node is used to draw an ACG mesh.

    It uses compile-time-tags for recognizing whether
    the mesh is a TriMeshT or a PolyMeshT.

    The available draw modes are collected based on these mesh
    properties like normal or color for faces or vertices.
*/

template <class Mesh>
class MeshNodeT : public BaseNode
{

public:

  /** Default constructor.
   * @param _textureMap if the mesh has a face_texture_index and a map is provided textures are switched based on the given ids
   */
  MeshNodeT(const Mesh&  _mesh,
	    BaseNode*    _parent=0,
	    std::string  _name="<MeshNode>");

  /// Destructor
  virtual ~MeshNodeT();


  ACG_CLASSNAME(MeshNode);


  /// return available draw modes
  unsigned int availableDrawModes() const;
  /// update bounding box
  void boundingBox(Vec3f& _bbMin, Vec3f& _bbMax);
  /// drawing
  void draw(GLState& _state, unsigned int _drawMode);
  /// picking
  void pick(GLState& _state, PickTarget _target);


  /// get mesh
  const Mesh& mesh() const { return mesh_; }


  /// update geometry buffers (vertex buffer objects)
  void update_geometry();
  /** update face indices (only for triangle meshes).  will be
      overridden by TriStripNodeT. */
  virtual void update_topology();

  /** \brief Setup a mapping between internal texture ids on the esh and the ids for the loaded textures in opengl
   */
  void set_texture_map( std::vector< GLuint >* _map){ textureMap_ = _map; };

protected:


  // types
  enum FaceMode { FACE_NORMALS, FACE_COLORS, PER_VERTEX, FACE_NORMALS_COLORS, FACE_HALFEDGE_TEXTURED };

  // draw vertices
  void draw_vertices();
  // draw polygons. to be overridden by TriStripNodeT
  virtual void draw_faces(FaceMode _mode);

  // pick vertices
  void pick_vertices(GLState& _state, bool _front = false);
  // pick polygons. to be overridden by TriMeshNodeT
  void pick_faces(GLState& _state);
  // pick edges
  void pick_edges(GLState& _state, bool _front = false);


  // opengl vertex array stuff
  enum ArrayType
  {
    NONE                    = 0,
    VERTEX_ARRAY            = 1,
    NORMAL_ARRAY            = 2,
    COLOR_ARRAY             = 4,
    TEXTURE_COORD_1D_ARRAY  = 8,
    TEXTURE_COORD_2D_ARRAY  = 16,
    TEXTURE_COORD_3D_ARRAY  = 32
  };

  // enable/disable vertex arrays according to the bits in _arrays
  void enable_arrays(unsigned int _arrays);



protected:

  // mesh reference
  const Mesh&  mesh_;

  // which arrays are currently enabled?
  unsigned int enabled_arrays_;

  // vertex buffer objects
  unsigned int  face_index_buffer_, vertex_buffer_, normal_buffer_;

  // index list for fast rendering (will be accessed by TriStripNodeT
  std::vector<unsigned int>  indices_;

private:

  bool vertexBufferInitialized_;
  bool normalBufferInitialized_;
  bool faceIndexBufferInitialized_;

  // Internal buffer used when rendering non float vertex coordinates
  std::vector< ACG::Vec3f > vertices_;

  // Internal buffer used when rendering non float normals
  std::vector< ACG::Vec3f > normals_;

  // Mapping of mesh face texture indeces to gltexture id ( has to be provided externally )
  std::vector< GLuint >* textureMap_;

  // display Lists used for picking
  GLuint faceList_;
  GLuint vertexList_;
  GLuint edgeList_;

  // do we need to update our display lists?
  bool updateFaceList_;
  bool updateVertexList_;
  bool updateEdgeList_;
  unsigned int faceBaseIndex_;
  unsigned int vertexBaseIndex_;
  unsigned int edgeBaseIndex_;
};


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
#if defined(INCLUDE_TEMPLATES) && !defined(ACG_MESHNODE_C)
#define ACG_MESHNODE_TEMPLATES
#include "MeshNodeT.cc"
#endif
//=============================================================================
#endif // ACG_MESHNODE_HH defined
//=============================================================================
