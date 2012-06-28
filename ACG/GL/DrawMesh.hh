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
*                                                                            *
*   $Revision$                                                       *
*   $LastChangedBy$                                                *
*   $Date$                     *
*                                                                            *
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
#include <list>
#include <OpenMesh/Core/Utils/Property.hh>
#include <OpenMesh/Core/IO/MeshIO.hh>

#include <ACG/Config/ACGDefines.hh>

#include <ACG/GL/GLState.hh>

#include <ACG/GL/IRenderer.hh>

//== FORWARDDECLARATIONS ======================================================


//== NAMESPACES ===============================================================

namespace ACG {

//== CLASS DEFINITION =========================================================


/** \brief Mesh Drawing Class
 *
 * Functional description:
 *
 * This class prepares the OpenMesh object for efficient rendering.
 * This is done in the function Rebuild() :
 *   1. Triangulation to guarantee a triangle mesh  (ConvertToTriangleMesh)
 *   2. Create a big 3*NumTris vertex buffer, so that each triangle has it's own 3 vertices
 *   3. Minimize this Vertex Buffer so that each vertex is unique
 *   4. Fix for Flat-Shading mode:
 *       OpenGL does not support Face-Normal generation on the fly, so we have to guarantee
 *       that the last vertex of each triangle is not shared with any other triangle.
 *       If flat shading is enabled, we need to update the vertex buffer and store the face normal
 *       for each triangle in this unshared vertex.
 *   5. Sort triangles by material:
 *       To minimize state OpenGL state changes and draw calls, triangles are sorted by material.
 *       This information is stored in Subsets
 *   6. GPU-Cache-Optimization:
 *       Optimize triangles for each subset for efficient gpu vertex cache usage.
 *       After that reorder vertices to avoid big gpu-memory jumps when reading in vertices
 *   while processing maintain the following maps:
 *     vertex index in the final vertex buffer -> halfedge index in OpenMesh  (vertexMap_)
 *     triangle index in the final index buffer -> face index in OpenMesh  (triToFaceMap_)
 */
template <class Mesh>
class DrawMeshT
{
private:

#pragma pack(push, 1)
  /// full precision vertex, 40 bytes w/o tangent
  struct Vertex
  {
    Vertex();

    float pos[3]; /*!< Position */
    float tex[2]; /*!< per halfedge texture coordinates */
    float n[3];   /*!< normal vector */
//    float tan[4]; /*!< tangent vector + parity */

    unsigned int vcol; /*!<  per vertex color */
    unsigned int fcol; /*!<  per face color */

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
    int  materialID;
    unsigned long startIndex;
    unsigned long numTris;
  };

  enum REBUILD_TYPE {REBUILD_NONE = 0, REBUILD_FULL = 1, REBUILD_GEOMETRY = 2, REBUILD_TOPOLOGY = 4, REBUILD_TEXTURES = 5};


public:

  DrawMeshT(Mesh& _mesh);
  virtual ~DrawMeshT();

  void disableColors()      {colorMode_ = 0;}
  void usePerVertexColors() {colorMode_ = 1;}
  void usePerFaceColors()   {colorMode_ = 2;}

  void setFlatShading()     {flatMode_ = 1;}
  void setSmoothShading()   {flatMode_ = 0;}

  void usePerVertexTexcoords()     {textureMode_ = 0;}
  void usePerHalfedgeTexcoords()   {textureMode_ = 1;}
  void usePerVertexNormals()   {halfedgeNormalMode_ = 0;}
  void usePerHalfedgeNormals()   {halfedgeNormalMode_ = 1;}

  /** \brief eventually rebuilds buffers used for rendering and binds index and vertex buffer
  */
  void bindBuffers();

  /** \brief eventually rebuilds buffers used for rendering and binds index and vertex buffer
  */
  void bindBuffersToRenderObject(RenderObject* _obj);

  /** \brief disables vertex, normal, texcoord and color pointers in OpenGL
  */
  void unbindBuffers();

  /** \brief binds index and vertex buffer and executes draw calls
  *
  *   @param _textureMap maps from internally texture-id to OpenGL texture id
  *   may be null to disable textured rendering
  */
  void draw(std::map< int, GLuint>* _textureMap);
  
  /** \brief initializes a RenderObject for a deferred draw call
  *
  *   @param _objOut address of the renderobject
  *   @param _textureMap maps from internally texture-id to OpenGL texture id
  *   may be null to disable textured rendering
  */
  void getTriRenderObjects(RenderObject* _objOut, std::map< int, GLuint>* _textureMap);

  /** \brief render the mesh in wireframe mode
  */
  void drawLines();

  /** \brief render the mesh in wireframe mode, deferred draw call
  */
  void getLineRenderObjects(RenderObject* _objOut);


  /** \brief render vertices only
  */
  void drawVertices();

  /** \brief render vertices only, deferred draw call
  */
  void getPointRenderObjects(RenderObject* _objOut);


  unsigned int getNumTris() const {return numTris_;}
  unsigned int getNumVerts() const {return numVerts_;}
  unsigned int getNumSubsets() const {return numSubsets_;}


  /** \brief measures the size in bytes of allocated memory.
  eventually prints a report to std::cout
  */
  unsigned int getMemoryUsage(bool _printReport = false);

  // The updateX functions give a hint on what to update.
  //  may perform a full rebuild internally!

  /** \brief request an update for the mesh topology
   */
  void updateTopology() {rebuild_ |= REBUILD_TOPOLOGY;}

  /** \brief request an update for the mesh vertices
   */
  void updateGeometry() {rebuild_ |= REBUILD_GEOMETRY;}

  /** \brief request an update for the textures
     */
  void updateTextures() {rebuild_ |= REBUILD_TEXTURES;}

  /** \brief request a full rebuild of the mesh
   *
   */
  void updateFull() {rebuild_ |= REBUILD_FULL;}

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
   * @return name of the texture index property
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
  * @return zero if not available, nonzero otherwise
  */
  int perFaceTextureCoordinateAvailable();

  /** \brief Check if texture indices are available
  *
  * If this function returns true, the strip processor will respect textures during strip generation.
  * Each returned strip has than an index that has to be used as a texture index during strip rendering.
  *
  * @return zero if not available, nonzero otherwise
  */
  int perFaceTextureIndexAvailable();

private:
  // processing pipeline:

  /** \brief  draw_mesh updater
   *
   */
  void rebuild();


  /** \brief Convert from halfedge data structure to triangle index list
   *
   * Only operates on indices
   *
   * @param _dstIndexBuf  pointer to the resulting index buffer
   * @param _maxFaceVertexCount maximum number of vertices per face seen in mesh_
   * @return number of triangles,  also fills triToFaceMap_
   */
  unsigned int convertToTriangleMesh(unsigned int* _dstIndexBuf, unsigned int _maxFaceVertexCount);

  /** \brief create the big 3 * NumTris vertex buffer
   *
   * @param _dstVertexBuf [out] pointer to the resulting vertex buffer
   * @param _dstVertexMap [out] pointer to the resulting vertex map
   * @param _indexBuf [in] pointer to the original index buffer, which is not needed anymore after this call
   */
  void createBigVertexBuf(Vertex* _dstVertexBuf, unsigned int* _dstVertexMap, const unsigned int* _indexBuf);

  /** \brief reads a vertex from mesh_ and write it to _pDst
   *
   * @param _pDst [out] pointer to the resulting vertex
   * @param _vh mesh vertex handle to read from
   * @param _hh corresponding halfedge handle of this vertex
   * @param _fh corresponding face handle of this vertex
   */
  void readVertex(Vertex*                       _pDst,
                  typename Mesh::VertexHandle   _vh,
                  typename Mesh::HalfedgeHandle _hh,
                  typename Mesh::FaceHandle     _fh);

  /** \brief
   *
   */
  void removeIsolatedVerts();

  /** \brief minimize the big vertex buffer
   *
   * @param _dstVertexBuf [out] pointer to the resulting vertex buffer
   * @param _srcVertexBuf [in] pointer to the big vertex buffer
   * @param _dstIndexBuf [out] pointer to the resutling index buffer
   * @param _dstVertexMap [out] pointer to the resulting vertex map (new -> old vertex)
   * @param _srcVertexMap [in] pointer to the previously used vertex map (new -> old vertex)
   * @param _duplicatesMap [out] maps from a duplicate vertex to it's first occurrence (OpenMesh vertex indices)
   * @return new number of vertices
   */
  unsigned int weldVertices(Vertex*             _dstVertexBuf,
                            const Vertex*       _srcVertexBuf,
                            unsigned int*       _dstIndexBuf,
                            unsigned int*       _dstVertexMap,
                            const unsigned int* _srcVertexMap,
                            std::list< std::pair<unsigned int, unsigned int> >& _duplicatesMap);

  /** \brief sort triangles by material id
   *
   *
   * also creates subsets: subsets_
   *
   * _dstIndexBuf and _srcIndexBuf must be different!
   *
   * @param _dstIndexBuf [out] pointer to the resulting index buffer
   * @param _srcIndexBuf [in] pointer to the index buffer about to be sorted
   */
  void sortTrisByMaterial(unsigned int* _dstIndexBuf, const unsigned int* _srcIndexBuf);

  /** \brief GPU cache optimization
   *
   * _dstIndexBuf == _srcIndexBuf allowed (inplace operation)
   * tris are optimized based on subsets
   * SortTrisByMaterial must be called prior!!
   * also maintains triToFaceMap_
   *
   * @param _dstIndexBuf [out] pointer to the resulting index buffer
   * @param _srcIndexBuf [in] pointer to the unoptimized index buffer
   */
  void optimizeTris(unsigned int* _dstIndexBuf, unsigned int* _srcIndexBuf);

  /** \brief optimize vertex layout
   *
   * for best results, call this after optimizeTris
   * also maintains vertexMap_,
   * NOTE: _srcVertexMap is invalid after this call
   * _srcVertexBuf != _dstVertexBuf!!
   *
   * @param _dstVertexBuf [out] pointer to the resulting vertex buffer
   * @param _srcVertexBuf [in] pointer to the source vertex buffer
   * @param _inOutIndexBuf [in, out] pointer to the index buffer, altered by this function
   * @param _srcVertexMap [in] pointer to the vertex map (new -> old vertex)
   */
  void optimizeVerts(Vertex*             _dstVertexBuf,
                     const Vertex*       _srcVertexBuf,
                     unsigned int*       _inOutIndexBuf,
                     const unsigned int* _srcVertexMap);


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
  * ACG::GLState::colorPointer.
  *
  * @return pointer to the first element of the picking buffer
  */
  ACG::Vec4uc * pickVertexColorBuffer(){
    if ( !pickVertColBuf_.empty() )
      return &(pickVertColBuf_)[0];
    else {
      std::cerr << "Illegal request to pickVertexColorBuffer when buffer is empty!" << std::endl;
      return 0;
    }
  };

  /** \brief get a pointer to the per vertex picking vertex buffer
  *
  * This function will return a pointer to the first element of the picking buffer.
  * Use updatePickingVertices to update the buffer before you render it via
  * ACG::GLState::vertexPointer.
  *
  * @return pointer to the first element of the picking buffer
  */
  ACG::Vec3f * pickVertexBuffer(){
    if ( !pickVertBuf_.empty() )
      return &(pickVertBuf_)[0];
    else {
      std::cerr << "Illegal request to pickVertexBuffer when buffer is empty!" << std::endl;
      return 0;
    }
  };
  
private:  
  
  /// The vertex buffer used for vertex picking
  std::vector< ACG::Vec3f > pickVertBuf_;
  /// The color buffer used for vertex picking
  std::vector< ACG::Vec4uc > pickVertColBuf_;



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
  * ACG::GLState::colorPointer.
  *
  * @return pointer to the first element of the picking buffer
  */
  ACG::Vec4uc * pickEdgeColorBuffer(){
    if ( !pickEdgeBuf_.empty() )
      return &(pickEdgeBuf_)[0];
    else {
      std::cerr << "Illegal request to pickEdgeColorBuffer when buffer is empty!" << std::endl;
      return 0;
    }
  }
  
private:  
  
  std::vector< ACG::Vec4uc > pickEdgeBuf_;



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
  * ACG::GLState::colorPointer.
  *
  * @return pointer to the per face picking color buffer
  */
  ACG::Vec4uc * pickFaceColorBuffer(){
    if ( !pickFaceColBuf_.empty() )
      return &(pickFaceColBuf_)[0];
    else {
      std::cerr << "Illegal request to pickFaceColorBuffer when buffer is empty!" << std::endl;
      return 0;
    }
  }

  /** \brief get a pointer to the per vertex picking color buffer
    *
    * This function will return a pointer to the first element of the picking buffer.
    * Use updatePickingFaces to update the buffer before you render it via
    * ACG::GLState::colorPointer.
    *
    * @return pointer to the first element of the picking buffer
    */
  ACG::Vec3f * pickFaceVertexBuffer(){
    if ( !pickFaceVertexBuf_.empty() )
      return &(pickFaceVertexBuf_)[0];
    else {
      std::cerr << "Illegal request to pickFaceVertexBuffer when buffer is empty!" << std::endl;
      return 0;
    }
  }
  
private:  

  std::vector< ACG::Vec3f > pickFaceVertexBuf_;
  std::vector< ACG::Vec4uc > pickFaceColBuf_;


public:
  /** \brief Call this function to update the color picking array
  *
  * This function calls the updatePickingVertices, updatePickingEdges, updatePickingVertices
  * functions with an appropriate offset so that the standard arrays will be updated.
  *
  * @param _state OpenGL state
  */
  void updatePickingAny(ACG::GLState& _state );    
  
  /** \brief get a pointer to the any picking color buffer
  *
  * This function will return a pointer to the first element of the picking buffer.
  * Use updatePickingAny to update the buffer before you render it via
  * ACG::GLState::colorPointer.
  *
  * @return Pointer to the first element of the picking buffer
  */
  ACG::Vec4uc * pickAnyFaceColorBuffer(){
    if ( !pickAnyFaceColBuf_.empty() )
      return &(pickAnyFaceColBuf_)[0];
    else {
      std::cerr << "Illegal request to pickAnyFaceColorBuffer when buffer is empty!" << std::endl;
      return 0;
    }
  }

  /** \brief get a pointer to the any picking color buffer
  *
  * This function will return a pointer to the first element of the picking buffer.
  * Use updatePickingAny to update the buffer before you render it via
  * ACG::GLState::colorPointer.
  *
  * @return Pointer to the first element of the picking buffer
  */
  ACG::Vec4uc * pickAnyEdgeColorBuffer(){
    if ( !pickAnyEdgeColBuf_.empty() )
      return &(pickAnyEdgeColBuf_)[0];
    else {
      std::cerr << "Illegal request to pickAnyEdgeColorBuffer when buffer is empty!" << std::endl;
      return 0;
    }
  }

  /** \brief get a pointer to the any picking color buffer
  *
  * This function will return a pointer to the first element of the picking buffer.
  * Use updatePickingAny to update the buffer before you render it via
  * ACG::GLState::colorPointer.
  *
  * @return Pointer to the first element of the picking buffer
  */
  ACG::Vec4uc * pickAnyVertexColorBuffer(){
    if ( !pickAnyVertexColBuf_.empty() )
      return &(pickAnyVertexColBuf_)[0];
    else {
      std::cerr << "Illegal request to pickAnyVertexColorBuffer when buffer is empty!" << std::endl;
      return 0;
    }
  }
  
private:  
  
  std::vector< ACG::Vec4uc > pickAnyFaceColBuf_;
  std::vector< ACG::Vec4uc > pickAnyEdgeColBuf_;
  std::vector< ACG::Vec4uc > pickAnyVertexColBuf_;
  

private:

  // small helper functions

  /** \brief Number of triangles after triangulation of the mesh
   *
   * returns the number of tris after triangulation of this mesh
   * if needed, also returns the highest number of vertices of a face
   *
   * @param _pOutMaxPolyVerts
   *
   * @return  number of triangles
  */
  unsigned int countTris(unsigned int* _pOutMaxPolyVerts = 0);

  /** \brief get the texture index of a triangle
   *
   *
   *  @param _tri Triangle index (-1 if not available)
   *
   *  @return Texture index of a triangle
   */
  int getTextureIDofTri(unsigned int _tri);

private:

  /// OpenMesh object to be rendered
  Mesh& mesh_;

  unsigned int numTris_, numVerts_;

  /// final index buffer used for rendering
  unsigned int* indices_;

  /// final vertex buffer used for rendering
  Vertex* vertices_;

  /// hint on what to rebuild
  unsigned int rebuild_;

  /** used to track mesh changes, that require a full rebuild
    * values directly taken from Mesh template
    */
  unsigned int prevNumFaces_,prevNumVerts_;


  // per material/texture subsets
  unsigned int numSubsets_;
  Subset* subsets_;

  GLuint vbo_,
         ibo_;

  /// index buffer used in Wireframe / Hiddenline mode
  GLuint lineIBO_;

  /// support for 2 and 4 byte unsigned integers
  GLenum indexType_;

  /// Color Mode: 0: none, 1: per vertex,  else: per face
  int colorMode_;
  /// flat / smooth shade mode toggle
  int flatMode_;
  
  /// normals in VBO currently in flat / smooth mode
  int bVBOinFlatMode_;

  /// per vertex / halfedge texture mode toggle:  0: per vertex,  1: per halfedge
  int textureMode_;

  /// texcoords in VBO currently in per vertex / halfedge mode toggle
  int bVBOinHalfedgeTexMode_;

  /// per vertex / halfedge normals mode toggle:  0: per vertex,  1: per halfedge
  int halfedgeNormalMode_;

  /// normals in VBO currently in per vertex / halfedge mode toggle
  int bVBOinHalfedgeNormalMode_;


  /** remapping for faster mesh change updates
   *  maps from triangle index to Mesh::FaceHandle::idx
   */
  unsigned int* triToFaceMap_;

  /// vertex index in vbo -> original OpenMesh halfedge index
  unsigned int* vertexMap_;

  /** inverse vertex map: original OpenMesh vertex index -> one vertex index in vbo
      this map is ambiguous and only useful for per vertex attributes rendering i.e. lines!
  */
  unsigned int* invVertexMap_;


  /// vertex buffer layout declaration with per vertex colors
  VertexDeclaration* vertexDeclVCol_;

  /// vertex buffer layout declaration with per face colors
  VertexDeclaration* vertexDeclFCol_;


  //========================================================================
  // internal processing
  // temporal buffer allocations to avoid memory requests while updating
  //========================================================================

  unsigned int* indicesTmp_;
  Vertex* verticesTmp_;

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

  /** \brief Update of the buffers
  *
  * This function will set all per edge buffers to invalid and will force an update
  * whe they are requested
  */
  void invalidatePerEdgeBuffers() {updatePerEdgeBuffers_ = 1;}

  /** \brief Update all per edge drawing buffers
  *
  * The updated buffers are: vertex buffer ( 2 vertices per edge ), color buffer
  */
  void updatePerEdgeBuffers();
  
  /** \brief get a pointer to the per edge vertex buffer
  *
  * This function will return a pointer to the first element of the vertex buffer.
  */
  ACG::Vec3f* perEdgeVertexBuffer();
  
  /** \brief get a pointer to the per edge color buffer
  *
  * This function will return a pointer to the first element of the color buffer.
  */
  ACG::Vec4f* perEdgeColorBuffer();

  /** \brief Update of the buffers
  *
  * This function will set all per edge buffers to invalid and will force an update
  * whe they are requested
  */
  void invalidatePerHalfedgeBuffers() {updatePerHalfedgeBuffers_ = 1;}

  /** \brief Update all per edge drawing buffer
  *n
  * The updated buffers are: per edge vertex buffer ( 2 vertices per edge )
  */
  void updatePerHalfedgeBuffers();
   
  /** \brief get a pointer to the per edge vertex buffer
  *
  * This function will return a pointer to the first element of the vertex buffer.
  */
  ACG::Vec3f* perHalfedgeVertexBuffer();
  
  /** \brief get a pointer to the per edge color buffer
  *
  * This function will return a pointer to the first element of the color buffer.
  */
  ACG::Vec4f* perHalfedgeColorBuffer();


private:
  int updatePerEdgeBuffers_;
  std::vector<ACG::Vec3f> perEdgeVertexBuf_;
  std::vector<ACG::Vec4f> perEdgeColorBuf_;

  int updatePerHalfedgeBuffers_;
  std::vector<ACG::Vec3f> perHalfedgeVertexBuf_;
  std::vector<ACG::Vec4f> perHalfedgeColorBuf_;

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
