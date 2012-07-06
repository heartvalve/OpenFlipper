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
#include <ACG/GL/DrawMesh.hh>

//== NAMESPACES ===============================================================


namespace ACG {
namespace SceneGraph {


//== CLASS DEFINITION =========================================================



/** \class MeshNodeT MeshNodeT.hh <ACG/Scenegraph/MeshNodeT.hh>

    This node draws a mesh using triangle strips.
*/

template <class Mesh>
class MeshNodeT  : public BaseNode
{
public:
  ACG_CLASSNAME(MeshNode);

  /** \brief Default constructor
  *     
  * The constructor needs a mesh on which this node will work.
  */
  MeshNodeT(Mesh&        _mesh,
            BaseNode*    _parent=0,
            std::string  _name="<MeshNode>" );

  /// Destructor
  virtual ~MeshNodeT();
  
  
  /** \brief the geometry of the mesh has changed
  *
  * call this function if you changed the geometry of the mesh.
  * All buffers related to the geometry will be updated.
  */
  void update_geometry();
  
  /** \brief the topology of the mesh has changed
  *
  * call this function if you changed the topology of the mesh.
  * All buffers related to the topology will be updated.
  *
  * this also triggers an update for the colors
  */
  void update_topology();  

  /** \brief the colors of the mesh have changed
  *
  * call this function if you changed the colors of the mesh.
  * All buffers related to the color will be updated.
  *
  * if you also updated the topology, the color is updated automatically
  */
  void update_color();
  
  /** \brief force an texture update
   *
   * This function has to be called, when the textures have changed
   */
  void update_textures();

private:

  /** Typedefs of the mesh representation
  * 
  * These typedefs are used to specifiy and convert all input to float for rendering
  */
  typedef typename Mesh::Point         Point;
  typedef typename Point::value_type   PointScalar;
  typedef typename Mesh::Normal        Normal;
  typedef typename Normal::value_type  NormalScalar;
  typedef typename Mesh::Color         Color;
  typedef typename Color::value_type   ColorScalar;
  
//===========================================================================
/** @name Mesh Handling
* @{ */
//===========================================================================  
  
public:
  
  /** \brief get the internal mesh
  */
  const Mesh& mesh() const { return mesh_; }  
  
private:  
  /// The mesh this node works on
  Mesh& mesh_;  
  
/** @} */  
  
//===========================================================================
/** @name Strip generation and handling
* @{ */
//===========================================================================  
public:
//   void update_strips();

private:
  DrawMeshT<Mesh>* drawMesh_;

/** @} */  

//===========================================================================
/** @name Bounding Box
* @{ */
//===========================================================================  

public:  
  
  /** \brief Current bounding box
  *
  * This function returns the bounding box of the node.
  */
  void boundingBox(Vec3d& _bbMin, Vec3d& _bbMax);  
  
private:
  
  /// bounding box lower left corner
  Vec3d bbMin_;
  
  /// bounding box upper right corner
  Vec3d bbMax_;
  
/** @} */
  
//===========================================================================
/** @name Normal Buffer
* @{ */
//===========================================================================

public:
  /// Returns if the normal array is currently activated
  bool normalsEnabled() { return enableNormals_; };
  
  /// Enable or disable the use of the normal array
  void enableNormals(bool _enable) { enableNormals_ = _enable; };

private:
  
  /// Flag if normals should be used
  bool enableNormals_;

/** @} */
  
//===========================================================================
/** @name Color buffer
* @{ */
//===========================================================================  
public:
  /// Returns if the color array is currently activated
  bool colorsEnabled() { return enableColors_; };
  
  /// Enable or disable the use of color array
  void enableColors(bool _enable) { enableColors_ = _enable; };

private:
  
  bool enableColors_;

  /** @} */
  
//===========================================================================
/** @name Array control functions
* @{ */
//===========================================================================

public:
  
  /** \brief enable/disable vertex arrays according to the bits in _arrays
  *
  * Use this function to enable or disable the appropriate array for rendering. Currently
  * the arrays in ArrayType are supported
  */
  void enable_arrays(unsigned int _arrays);    

private:
  
  /// Enum controlling which array should be used for rendering
  enum ArrayType
  {
    NONE                             = 0,
    PER_EDGE_VERTEX_ARRAY            = 1,
    PER_EDGE_COLOR_ARRAY             = 2,
    PER_HALFEDGE_VERTEX_ARRAY        = 4,
    PER_HALFEDGE_COLOR_ARRAY         = 8
  };
  
  /// which arrays are currently enabled?
  unsigned int enabled_arrays_;
  

//===========================================================================
/** @name Rendering functions
* @{ */
//===========================================================================
  
public:
  /** \brief Draws the object
  *
  */
  void draw(GLState& _state, const DrawModes::DrawMode& _drawMode);

  /** \brief Draws the object deferred
  *
  */
  void getRenderObjects(IRenderer* _renderer, GLState& _state, const DrawModes::DrawMode& _drawMode);

  /** \brief return available draw modes 
  *
  * The drawmodes are constructed based on the mesh properties and the hardware capabilities
  * of the system.
  */
  ACG::SceneGraph::DrawModes::DrawMode  availableDrawModes() const;
  
private:
  
  /** \brief draws all vertices of the mesh
  *
  */
  inline void draw_vertices();

  inline void add_point_RenderObjects(IRenderer* _renderer, const RenderObject* _baseObj);

  /** \brief draws all edges of the mesh
  *
  */
  inline void draw_lines();

  inline void add_line_RenderObjects(IRenderer* _renderer, const RenderObject* _baseObj);


  /** \brief draws all halfedges of the mesh
  *
  */
  inline void draw_halfedges();

  
  /** \brief draws all faces of the mesh 
  *
  */
  void draw_faces();

  void add_face_RenderObjects(IRenderer* _renderer, const RenderObject* _baseObj);
  
private:
  

/** @} */

//===========================================================================
/** @name general picking functions
* @{ */
//===========================================================================
public:
  /** \brief Draws the object in picking mode
  *
  */
  void pick(GLState& _state, PickTarget _target);

/** @} */

//===========================================================================
/** @name vertex picking functions
* @{ */
//===========================================================================  
  
private:

  /** \brief Renders picking for vertices
  * _front: Only render front vertices (not occluded by geometry)
  */
  void pick_vertices(GLState& _state, bool _front = false);
  
  /// Flag indicating if the vertex picking has to be updated
  bool updateVertexPicking_;

  /// Index of the first vertex in vertexpicking
  unsigned int vertexPickingBaseIndex_;
  
/** @} */
  

//===========================================================================
/** @name edge picking functions
* @{ */
//=========================================================================== 


private:
  /** \brief Renders picking for edges
  * _front: Only render front edges (not occluded by geometry)
  */
  void pick_edges(GLState& _state, bool _front = false);  
  
  /// Flag indicating if the edge picking has to be updated
  bool updateEdgePicking_;
  
  /// Index of the first edge in edgepicking
  unsigned int edgePickingBaseIndex_;


/** @} */

//===========================================================================
/** @name face picking functions
* @{ */
//=========================================================================== 


private:  
  /** \brief Renders picking for faces
  * _front: Only render front faces (not occluded by geometry)
  */
  void pick_faces(GLState& _state);
  
  /// Flag indicating if the edge picking has to be updated
  bool updateFacePicking_;
  
  /// Index of the first face in facepicking
  unsigned int facePickingBaseIndex_;  

/** @} */

//===========================================================================
/** @name anything picking functions
* @{ */
//=========================================================================== 
  
private:  
  
  /** \brief Renders picking for all primitives
  *
  */
  void pick_any(GLState& _state);  
  
  /// Flag indicating if the any picking has to be updated
  bool updateAnyPicking_;
  
  /// Index of the first face in anypicking
  unsigned int anyPickingBaseIndex_;    
  
/** @} */
  

//===========================================================================
/** @name Texture handling
* @{ */
//=========================================================================== 
public:
  /** \brief set the name of the property used for texture index specification
  *
  * The given property name will define a texture index. This index is used to make
  * a lookup in the texture correspondence map containing for each index the gluint
  * for the texture to be used. A zero in the property means, that no texture will be bound for the 
  * face.
  * If you define a non existing name here, texture switching will be disabled and it
  * is assumed that a texture is bound already.
  *
  *\todo Remove the external texture loading and do it here.
  *
  */
  void setIndexPropertyName( std::string _indexPropertyName );
  
  /// \brief Get current texture index property name
  const std::string& indexPropertyName() const;
  
  
  /** \brief Set the name of the per face texture coordinate property
  *
  * Set this property for per face per vertex texture coordinates. Additionally you have to set
  * the IndexPropertyName to make texturing with multiple textures work.
  */
  void setHalfedgeTextcoordPropertyName( std::string _halfedgeTextcoordPropertyName );
  
  public:
    
    /** \brief Setup a mapping between internal texture ids on the mesh and the ids for the loaded textures in opengl
    *
    * @param _map maps between an int index stored in the Mesh describing which texture to use for a face,
    *             and the GluInt name of the texture bound by the TextureNode. \n
    *             If such a map is not available ( =0 ), assume TextureNode has already bound a texture
    *             And render without switching textures
    */
    void setTextureMap( std::map< int, GLuint>* _map){ textureMap_ = _map; };
  
private:
  
  /// This flag indicates if we have a per Face texture index property
  bool perFaceTextureIndexAvailable_;
  
  /// This flag indicates if we have a per Face texture coordinate property
  bool perFaceTextureCoordsAvailable_;
  
  /// Mapping of mesh face texture indices to gltexture id ( has to be provided externally )
  std::map< int, GLuint>* textureMap_;

/** @} */
  
  /// \todo Remove all these functions afterwards!
  
public:  
  void set_property_map( std::map< int, std::string>* _map){ };  
    

  /** \brief measures the size in bytes of allocated memory
  */
  unsigned int getMemoryUsage();
    

};


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
#if defined(INCLUDE_TEMPLATES) && !defined(ACG_MESHNODE_C)
#define ACG_MESHNODE_TEMPLATES
#include "MeshNode2T.cc"
#endif
//=============================================================================
#endif // ACG_MESHNODE_HH defined
//=============================================================================

