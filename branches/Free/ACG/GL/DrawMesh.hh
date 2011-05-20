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
//  CLASS DrawMeshT
//
//=============================================================================


#ifndef ACG_DRAW_MESH_HH
#define ACG_DRAW_MESH_HH


//== INCLUDES =================================================================

#include <vector>
#include <OpenMesh/Core/Utils/Property.hh>
#include <OpenMesh/Core/IO/MeshIO.hh>

#include <ACG/Config/ACGDefines.hh>

#include <ACG/GL/GLState.hh>

//== FORWARDDECLARATIONS ======================================================


//== NAMESPACES ===============================================================

namespace ACG {

//== CLASS DEFINITION =========================================================

template <class Mesh>
class DrawMeshT
{
private:

#pragma pack(push, 1)
  /// full precision vertex, 40 bytes w/o tangent
  struct Vertex
  {
    Vertex();

    float pos[3]; // position
    float tex[2]; // tex coords
    float n[3]; // normal vector
//    float tan[4]; // tangent vector + parity

     unsigned int vcol; // per vertex color
     unsigned int fcol;   // per face color

    int equals(const Vertex& r);
  };

  /// compressed vertex, 22 bytes
  struct VertexC
  {
    unsigned short pos[3];
    unsigned short u, v;
    unsigned int n;
//    unsigned int tan;

    unsigned int vcol;
    unsigned int fcol;
  };
#pragma pack(pop)

  struct Subset
  {
    int  MaterialID;
    unsigned int StartIndex;
    unsigned int NumTris;
  };

  enum REBUILD_TYPE {REBUILD_NONE = 0, REBUILD_FULL = 1, REBUILD_GEOMETRY = 2, REBUILD_TOPOLOGY = 4};


public:

  DrawMeshT(Mesh& _mesh);
  virtual ~DrawMeshT();

  void disableColors()      {m_iColorMode = 0;}
  void usePerVertexColors() {m_iColorMode = 1;}
  void usePerFaceColors()   {m_iColorMode = 2;}

  void setFlatShading()     {m_bFlatMode = 1;}
  void setSmoothShading()   {m_bFlatMode = 0;}

  /** \brief eventually rebuilds buffers used for rendering and binds index and vertex buffer
  */
  void bindBuffers();
  
  /** \brief disables vertex, normal, texcoord and color pointers in OpenGL
  */
  void unbindBuffers();

  /** \brief binds index and vertex buffer and executes draw calls
  *
  *   @param textureMap maps from internally texture-id to OpenGL texture id
  *   may be null to disable textured rendering
  */
  void draw(std::map< int, GLuint>* _textureMap);
  

  /** \brief render the mesh in wireframe mode
  */
  void drawLines();


  /** \brief render vertices only
  */
  void drawVertices();


  unsigned int getNumTris() const {return m_NumTris;}
  unsigned int getNumVerts() const {return m_NumVerts;}
  unsigned int getNumSubsets() const {return m_NumSubsets;}

  // The UpdateX functions give a hint on what to update.
  //  may perform a full rebuild internally!

  /** \brief request an update for the mesh topology
   */
  void updateTopology() {m_Rebuild |= REBUILD_TOPOLOGY;}

  /** \brief request an update for the mesh vertices
   */
  void updateGeometry() {m_Rebuild |= REBUILD_GEOMETRY;}

  /** \brief request a full rebuild of the mesh
   *
   */
  void updateFull() {m_Rebuild |= REBUILD_FULL;}

  /** \brief returns the number of used textured of this mesh
   *
   * @return Number of different textures used in the mesh
   */
  unsigned int getNumTextures();

  /** \brief set the name of the property used for texture index specification
  *
  * The given property name will define a texture index. The strip processor checks this
  * property and generates strips which contain only the same index. If the property is not
  * found, strips will be independent of this property
  *
  * @param _indexPropertyName
  */
  void setTextureIndexPropertyName( std::string _indexPropertyName );

  /** \brief get the name of the texture index property
   *
   * @return
   */
  const std::string& getTextureIndexPropertyName() const { return textureIndexPropertyName_; };

  /** \brief set the name of the property used for texture coordinate
  *
  * The given property name will define per face Texture coordinates. This property has to be a
  * halfedge property. The coordinate on each edge is the texture coordinate of the to vertex.
  * If this property is not available, textures will not be processed by the strip processor.
  *
  * @param _perFaceTextureCoordinatePropertyName
  */
  void setPerFaceTextureCoordinatePropertyName( std::string _perFaceTextureCoordinatePropertyName );
  
  /** \brief Check if per Face Texture coordinates are available
  *
  * If this function returns true, a per face per vertex texture array is available
  *
  * @return
  */
  int perFaceTextureCoordinateAvailable();

  /** \brief Check if texture indices are available
  *
  * If this function returns true, the strip processor will respect textures during strip generation.
  * Each returned strip has than an index that has to be used as a texture index during strip rendering.
  *
  * @return
  */
  int perFaceTextureIndexAvailable();

private:
  // processing pipeline:

  /** \brief  draw_mesh updater
   *
   */
  void rebuild();


  /** \brief convert from halfedge data structure to triangle index list
   *
   * (only operates on indices)
   *
   * @param DstIndexBuf
   * @param MaxFaceVertexCount
   *
   * @return # of triangles,  also fills m_pTriToFaceMap
   */
  unsigned int convertToTriangleMesh(unsigned int* DstIndexBuf, unsigned int MaxFaceVertexCount);

  /** \brief create the big 3 * NumTris vertex buffer
   *
   * @param DstVertexBuf
   * @param DstVertexMap
   * @param IndexBuf
   */
  void createBigVertexBuf(Vertex* DstVertexBuf, unsigned int* DstVertexMap, const unsigned int* IndexBuf);

  /** \brief
   *
   * @param pDst
   * @param vh
   * @param hh
   * @param fh
   */
  void readVertex(Vertex*                       pDst,
                  typename Mesh::VertexHandle   vh,
                  typename Mesh::HalfedgeHandle hh,
                  typename Mesh::FaceHandle      fh);

  /** \brief
   *
   */
  void removeIsolatedVerts();

  /** \brief minimize the big vertex buffer
   *
   * @param DstVertexBuf
   * @param SrcVertexBuf
   * @param DstIndexBuf
   * @param DstVertexMap
   * @param SrcVertexMap
   * @return new #vertices
   */
  unsigned int weldVertices(Vertex*             DstVertexBuf,
                            const Vertex*       SrcVertexBuf,
                            unsigned int*       DstIndexBuf,
                            unsigned int*       DstVertexMap,
                            const unsigned int* SrcVertexMap);

  /** \brief sort triangles by material id
   *
   *
   * also creates subsets: m_pSubsets
   *
   * DstIndexBuf and SrcIndexBuf must be different!
   *
   * @param DstIndexBuf
   * @param SrcIndexBuf
   */
  void sortTrisByMaterial(unsigned int* DstIndexBuf, const unsigned int* SrcIndexBuf);

  /** \brief GPU cache optimization
   *
   * DstIndexBuf == SrcIndexBuf allowed (inplace operation)
   * tris are optimized based on subsets
   * SortTrisByMaterial must be called prior!!
   * also maintains m_pTriToFaceMap
   *
   * @param DstIndexBuf
   * @param SrcIndexBuf
   */
  void optimizeTris(unsigned int* DstIndexBuf, unsigned int* SrcIndexBuf);

  /** \brief optimize vertex layout
   *
   * for best results, call this after OptimizeTris
   * also maintains m_pVertexMap,
   * NOTE: SrcVertexMap is invalid after this call
   * SrcVertexBuf != DstVertexBuf!!
   *
   * @param DstVertexBuf
   * @param SrcVertexBuf
   * @param InOutIndexBuf
   * @param SrcVertexMap
   */
  void optimizeVerts(Vertex*             DstVertexBuf,
                     const Vertex*       SrcVertexBuf,
                     unsigned int*       InOutIndexBuf,
                     const unsigned int* SrcVertexMap);


  /** \brief  stores the vertex buffer on the gpu
   *
   */
  void createVBO();

  /** \brief stores the index buffer on the gpu
   *
   */
  void createIBO();

public:
  // color picking

   /** Call this function to update the color picking array
  * The _offsett value can be used to shift the color in the resulting arrays.
  * pick Any uses the offset to generate arrays for picking everything.
  *
  * @param _state
  * @param _offset
  */
  void updatePickingVertices(ACG::GLState& _state , uint _offset = 0);    

  /** \brief get a pointer to the per vertex picking color buffer
  *
  * This function will return a pointer to the first element of the picking buffer.
  * Use updatePickingVertices to update the buffer before you render it via
  * glColorPointer.
  *
  * @return pointer to the first element of the picking buffer
  */
  ACG::Vec4uc * pickVertexColorBuffer(){ return &(m_PickVertBuf)[0]; };
  
private:  
  
  /// The color buffer used for vertex picking
  std::vector< ACG::Vec4uc > m_PickVertBuf;



public:
    
  /**  \brief Update color picking array for edges
   *
   * Call this function to update the color picking array
   * The _offsett value can be used to shift the color in the resulting arrays.
   * pick Any uses the offset to generate arrays for picking everything.
   *
   * @param _state
   * @param _offset
   */
  void updatePickingEdges(ACG::GLState& _state , uint _offset = 0 );    
  
  /** \brief get a pointer to the per edge picking color buffer
  *
  * This function will return a pointer to the first element of the picking buffer.
  * Use updatePickingEdges to update the buffer before you render it via
  * glColorPointer.
  *
  * @return pointer to the first element of the picking buffer
  */
  ACG::Vec4uc * pickEdgeColorBuffer(){ return &(m_PickEdgeBuf)[0]; };
  

  
private:  
  
  std::vector< ACG::Vec4uc > m_PickEdgeBuf;



public:
  
  /**  \brief Update color picking array for faces
   *
   * @param _state
   */
  void updatePickingFaces(ACG::GLState& _state );    
  
  /** \brief get a pointer to the per face picking color buffer
  *
  * This function will return a pointer to the first element of the picking buffer.
  * Use updatePickingFaces to update the buffer before you render it via
  * glColorPointer.
  *
  * @return
  */
  ACG::Vec4uc * pickFaceColorBuffer(){ return &(m_PickFaceColBuf)[0]; };

  /** \brief get a pointer to the per vertex picking color buffer
    *
    * This function will return a pointer to the first element of the picking buffer.
    * Use updatePickingFaces to update the buffer before you render it via
    * glColorPointer.
    *
    * @return pointer to the first element of the picking buffer
    */
  ACG::Vec3f * pickFaceVertexBuffer(){ return &(m_PickFaceVertexBuf)[0]; };
  
private:  

  std::vector< ACG::Vec3f > m_PickFaceVertexBuf;
  std::vector< ACG::Vec4uc > m_PickFaceColBuf;


public:
  /** \brief Call this function to update the color picking array
  *
  * This function calls the updatePickingVertices, updatePickingEdges, updatePickingVertices
  * functions with an appropriate offset so that the standard arrays will be updated.
  *
  * @param _state
  */
  void updatePickingAny(ACG::GLState& _state );    
  
  /** \brief get a pointer to the any picking color buffer
  *
  * This function will return a pointer to the first element of the picking buffer.
  * Use updatePickingAny to update the buffer before you render it via
  * glColorPointer.
  *
  * @return Pointer to the first element of the picking buffer
  */
  ACG::Vec4uc * pickAnyColorBuffer(){ return &(m_PickAnyBuf)[0]; };
  
private:  
  
  std::vector< ACG::Vec4uc > m_PickAnyBuf;
  

private:

  // small helper functions

  /** \brief Number of triangles after triangulation of the mesh
   *
   * returns the number of tris after triangulation of this mesh
   * if needed, also returns the highest number of vertices of a face
   *
   * @param pOutMaxPolyVerts
   *
   * @return
  */
  unsigned int countTris(unsigned int* pOutMaxPolyVerts = 0);

  /** \brief get the texture index of a triangle
   *
   *
   *  @param tri Triangle index (-1 if not available)
   *
   *  @return Texture index of a triangle
   */
  int getTextureIDofTri(unsigned int tri);

private:

  /// OpenMesh object to be rendered
  Mesh& m_Mesh;

  unsigned int m_NumTris, m_NumVerts;

  /// final index buffer used for rendering
  unsigned int* m_pIndices;

  /// final vertex buffer used for rendering
  Vertex* m_pVertices;

  /// hint on what to rebuild
  unsigned int m_Rebuild;

  /** used to track mesh changes, that require a full rebuild
    * values directly taken from Mesh template
    */
  unsigned int m_PrevNumFaces,m_PrevNumVerts;


  // per material/texture subsets
  unsigned int m_NumSubsets;
  Subset* m_pSubsets;

  GLuint m_VBO,
         m_IBO;

  /// index buffer used in Wireframe / Hiddenline mode
  GLuint m_LineIBO;

  /// support for 2 and 4 byte unsigned integers
  GLenum m_IndexType;

  /// Color Mode: 0: none, 1: per vertex,  else: per face
  int m_iColorMode;
  /// flat / smooth shade mode toggle
  int m_bFlatMode;

  /// normals in VBO currently in flat / smooth mode
  int m_bVBOinFlatMode;


  /** remapping for faster mesh change updates
   *  maps from triangle index to Mesh::FaceHandle::idx
   */
  unsigned int* m_pTriToFaceMap;

  /// vertex index in vbo -> original OpenMesh halfedge index
  unsigned int* m_pVertexMap;

  /** inverse vertex map: original OpenMesh vertex index -> one vertex index in vbo
      this map is ambiguous and only useful for per vertex attributes rendering i.e. lines!
  */
  unsigned int* m_pInvVertexMap;


  //========================================================================
  // internal processing
  // temporal buffer allocations to avoid memory requests while updating
  //========================================================================

  unsigned int* m_pIndicesTmp;
  Vertex* m_pVerticesTmp;

  //========================================================================
  // texture handling
  //========================================================================

  /** \brief Property for the per face texture index.
  *
  * This property is used by the mesh for texture index specification.
  * If this is invalid, then it is assumed that there is one or no active
  * texture. This means that the generated strips will be independent of texture 
  * information.
  */  
  std::string textureIndexPropertyName_;

  /** \brief Property for the per face texture coordinates.
  *
  * This property is used by the mesh for texture coordinate specification.
  * If this is invalid, then the strip processor will ignore per face textures during processing.
  */  
  std::string perFaceTextureCoordinatePropertyName_;

  
public:
  //========================================================================
  // per edge buffers
  void invalidatePerEdgeBuffers() {m_bUpdatePerEdgeBuffers = 1;}
  void updatePerEdgeBuffers();
  ACG::Vec3f* perEdgeVertexBuffer();
  ACG::Vec4f* perEdgeColorBuffer();

  void invalidatePerHalfedgeBuffers() {m_bUpdatePerHalfedgeBuffers = 1;}
  void updatePerHalfedgeBuffers();
  ACG::Vec3f* perHalfedgeVertexBuffer();
  ACG::Vec4f* perHalfedgeColorBuffer();


private:
  int m_bUpdatePerEdgeBuffers;
  std::vector<ACG::Vec3f> m_PerEdgeVertexBuf;
  std::vector<ACG::Vec4f> m_PerEdgeColorBuf;

  int m_bUpdatePerHalfedgeBuffers;
  std::vector<ACG::Vec3f> m_PerHalfedgeVertexBuf;
  std::vector<ACG::Vec4f> m_PerHalfedgeColorBuf;

  /** \brief compute halfedge point
  * compute visualization point for halfedge (shifted to interior of face)
  *
  * @param _heh
  *
  * @return
  */
  typename Mesh::Point halfedge_point(const typename Mesh::HalfedgeHandle _heh);

};



//=============================================================================
} // namespace ACG
//=============================================================================
#if defined(INCLUDE_TEMPLATES) && !defined(ACG_DRAW_MESH_C)
#define ACG_DRAW_MESH_TEMPLATES
#include "DrawMesh.cc"
#endif
//=============================================================================
#endif // ACG_DRAW_MESH_HH defined
//=============================================================================
