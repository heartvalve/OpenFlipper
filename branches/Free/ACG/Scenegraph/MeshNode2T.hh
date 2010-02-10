/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2009 by Computer Graphics Group, RWTH Aachen      *
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
//  CLASS TriStripNodeT
//
//=============================================================================


#ifndef ACG_MESHNODE_HH
#define ACG_MESHNODE_HH


//== INCLUDES =================================================================



#include "BaseNode.hh"
#include <vector>
#include <ACG/Scenegraph/StripProcessorT.hh>

//== NAMESPACES ===============================================================


namespace ACG {
namespace SceneGraph {


//== CLASS DEFINITION =========================================================



/** \class TriStripNodeT TriStripNodeT.hh <ACG/Scenegraph/TriStripNodeT.hh>

    This node draws a mesh using triangle strips.
*/

template <class Mesh>
class TriStripNodeT  : public BaseNode
{
public:
  ACG_CLASSNAME(MeshNode);

  /** \brief Default constructor
  *     
  * The constructor needs a mesh on which this node will work.
  */
  TriStripNodeT(Mesh&        _mesh,
             BaseNode*    _parent=0,
             std::string  _name="<MeshNode>" );

  /// Destructor
  virtual ~TriStripNodeT();
  
  
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
  */
  void update_topology();  

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
  StripProcessorT<Mesh> stripProcessor_;

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
/** @name Vertex Buffer
* @{ */
//===========================================================================
  
private:
  
  /// Vertex buffer
  GLuint vertexBuffer_;
  
  /// Vertex buffer initialization flag
  bool   vertexBufferInitialized_;  
  
  /// Internal buffer used when rendering non float vertex coordinates
  std::vector< ACG::Vec3f > vertices_;
  
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
  
  /// Normal buffer
  GLuint normalVertexBuffer_;
  
  /// normal buffer initialization flag
  bool normalVertexBufferInitialized_;
  
  /// Internal buffer used when rendering non float normals
  std::vector< ACG::Vec3f > normals_;

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
  
  /// color buffer
  GLuint colorVertexbuffer_;
  
  /// normal buffer initialization flag
  bool colorVertexBufferInitialized_;
  
  /// Internal buffer used when rendering non Vec4f colors
  std::vector< ACG::Vec4f > colors_;
  
  /** @} */
  
//===========================================================================
/** @name Texcoord buffer
* @{ */
//===========================================================================  
public:
  /// Returns if the per vertex Texture coordinates array is currently activated
  bool texCoordEnabled() { return enableTexCoords_; };
  
  /// Enable or disable the use of the  per vertex Texture coordinates array
  void enableTexCoords(bool _enable) { enableTexCoords_ = _enable; };
  
private:
  
  bool enableTexCoords_;
  
  /** @} */  
    
//===========================================================================
/** @name Line buffer
* @{ */
//===========================================================================    
private:
  /// Vector storing vertices for rendering all edges in the mesh
  std::vector< unsigned int > lineIndices_;
  
  /// Index buffer for lines
  GLuint lineIndexBuffer_;
  
  /// lineIndexBuffer initialization flag
  bool lineIndexBufferInitialized_;
  
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
    NONE                      = 0,
    VERTEX_ARRAY              = 1,
    NORMAL_VERTEX_ARRAY       = 2,
    COLOR_VERTEX_ARRAY        = 4,
    TEXCOORD_VERTEX_ARRAY     = 8,
    LINE_INDEX_ARRAY          = 16,
    PER_EDGE_VERTEX_ARRAY     = 32,
    PER_EDGE_COLOR_ARRAY      = 64,
    PER_FACE_VERTEX_ARRAY     = 128,
    PER_FACE_NORMAL_ARRAY     = 256,
    PER_FACE_COLOR_ARRAY      = 512,
    PER_FACE_TEXCOORD_ARRAY   = 1024
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
  void draw(GLState& _state, unsigned int _drawMode);
  
private:
  // types
  enum FaceMode {  PER_VERTEX, PER_FACE };
  
  /** \brief draws all vertices of the mesh
  *
  */
  inline void draw_vertices();
  
  /** \brief draws all edges of the mesh
  *
  */
  inline void draw_lines();
  
  /** \brief draws all faces of the mesh 
  *
  */
  void draw_faces(FaceMode _mode);
  
  /** \brief return available draw modes 
  *
  * The drawmodes are constructed based on the mesh properties and the hardware capabilities
  * of the system.
  */
  unsigned int availableDrawModes() const;

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
  *
  */
  void pick_vertices(GLState& _state, bool _front = false);
  
  /// Flag indicating if the vertex picking display list has to be updates
  bool updateVertexPickingList_;  

  /// Index of the first vertex in vertexpicking
  unsigned int vertexPickingBaseIndex_;
  
  /// Dispplay list for rendering vertex picking
  GLuint vertexPickingList_;
  
/** @} */
  

//===========================================================================
/** @name edge picking functions
* @{ */
//=========================================================================== 


private:
  /** \brief Renders picking for edges
  *
  */
  void pick_edges(GLState& _state, bool _front = false);  
  
  /// Flag indicating if the edge picking display list has to be updates
  bool updateEdgePickingList_;
  
  /// Index of the first edge in edgepicking
  unsigned int edgePickingBaseIndex_;
  
  /// Dispplay list for rendering edge picking
  GLuint edgePickingList_;

/** @} */

//===========================================================================
/** @name face picking functions
* @{ */
//=========================================================================== 


private:  
  /** \brief Renders picking for faces
  *
  */
  void pick_faces(GLState& _state);
  
  /// Flag indicating if the edge picking display list has to be updates
  bool updateFacePickingList_;
  
  /// Index of the first face in facepicking
  unsigned int facePickingBaseIndex_;  
  
  /// Dispplay list for rendering edge picking
  GLuint facePickingList_;

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
  
  /// Flag indicating if the any picking display list has to be updates
  bool updateAnyPickingList_;
  
  /// Index of the first face in anypicking
  unsigned int anyPickingBaseIndex_;    
  
  /// Dispplay list for rendering general picking
  GLuint anyPickingList_;  
  
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
  
  /// Mapping of mesh face texture indices to gltexture id ( has to be provided externally )
  std::map< int, GLuint>* textureMap_;

/** @} */
  
  /// \todo Remove all these functions afterwards!
  
public:  
  void set_property_map( std::map< int, std::string>* _map){ };  
    
    
    

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

