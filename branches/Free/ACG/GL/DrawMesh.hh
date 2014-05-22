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
#include <ACG/GL/MeshCompiler.hh>
#include <ACG/GL/globjects.hh>
#include <ACG/ShaderUtils/GLSLShader.hh>

//== FORWARDDECLARATIONS ======================================================


//== NAMESPACES ===============================================================

namespace ACG {

//== CLASS DEFINITION =========================================================


/** \brief Mesh Drawing Class
 *
 * This class creates a new mesh for efficient rendering based on an OpenMesh object.
 * DrawMesh also supports optimized picking and toggling between vertex/halfedge/face normals and vertex/halfedge texcoords.
 * 
 * It is not recommended to use this class if the input mesh does not implement an OpenMesh kernel.
 * Instead, MeshCompiler can be used directly to build the vertex and index buffer.
 */

template <class Mesh>
class DrawMeshT
{
private:

#pragma pack(push, 1)
  /// full precision vertex, 36 bytes w/o tangent
  struct Vertex
  {
    Vertex();

    float pos[3]; /*!< Position */
    float tex[2]; /*!< per halfedge texture coordinates */
    float n[3];   /*!< normal vector */
//    float tan[4]; /*!< tangent vector + parity */

    unsigned int col; /*!< color */

//     unsigned int vcol; /*!<  per vertex color */
//     unsigned int fcol; /*!<  per face color */
  };

  /// compressed vertex, 18 bytes
  struct VertexC
  {
    unsigned short pos[3];
    unsigned short u, v;
    unsigned int n;
//    unsigned int tan;

    unsigned int col;
  };
#pragma pack(pop)

  struct Subset
  {
    int  materialID;
    unsigned long startIndex;
    unsigned long numTris;
  };

  enum REBUILD_TYPE {REBUILD_NONE = 0, REBUILD_FULL = 1, REBUILD_GEOMETRY = 2, REBUILD_TOPOLOGY = 4, REBUILD_TEXTURES = 8};


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

  /** \brief get opengl vertex buffer id
  */
  GLuint getVBO();

  /** \brief get opengl index buffer id
  */
  GLuint getIBO();

  /** \brief get index type of index buffer
  */
  GLenum getIndexType() const {return indexType_;}
  

  /** \brief get mesh compiler used to create the draw mesh
  */
  MeshCompiler* getMeshCompiler() {return meshComp_;}

  /** \brief get vertex declaration used for per-vertex color rendering
  */
  VertexDeclaration* getVertexDeclaration();

  /** \brief map from vertex index of the original mesh point buffer to the corresponding vertex index inside the VBO.
  @param _v vertex index into mesh points
  @return vertex index into VBO
  */
  unsigned int mapVertexToVBOIndex(unsigned int _v);


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

  /** \brief adds RenderObjects to a deferred draw call renderer
  *
  *   @param _renderer renderobjects are added to this renderer
  *   @param _baseObj address of the base renderobject with information about shader generation, gl states, matrices ..
  *   @param _textureMap maps from internally texture-id to OpenGL texture id
  *   may be null to disable textured rendering
  */
  void addTriRenderObjects(IRenderer* _renderer, const RenderObject* _baseObj, std::map< int, GLuint>* _textureMap);

  /** \brief render the mesh in wireframe mode
  */
  void drawLines();

  /** \brief render the mesh in wireframe mode, deferred draw call
  */
  void addLineRenderObjects(IRenderer* _renderer, const RenderObject* _baseObj);


  /** \brief render vertices only
  */
  void drawVertices();

  /** \brief render vertices only, deferred draw call
  */
  void addPointRenderObjects(IRenderer* _renderer, const RenderObject* _baseObj);


  unsigned int getNumTris() const {return numTris_;}
  unsigned int getNumVerts() const {return numVerts_;}
  unsigned int getNumSubsets() const {return meshComp_->getNumSubsets();}


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


  /** \brief reads a vertex from mesh_ and write it to _pDst
   *
   * @param _pDst [out] pointer to the resulting vertex
   * @param _vh mesh vertex handle to read from
   * @param _hh corresponding halfedge handle of this vertex
   * @param _fh corresponding face handle of this vertex
   */
  void readVertex(Vertex*                             _pDst,
                  const typename Mesh::VertexHandle   _vh,
                  const typename Mesh::HalfedgeHandle _hh,
                  const typename Mesh::FaceHandle     _fh);

  /** \brief return a vertex color from mesh
   *
   * @param _vh mesh vertex handle
   */
  unsigned int getVertexColor(const typename Mesh::VertexHandle   _vh);

  /** \brief return a face color from mesh
   *
   * @param _fh mesh face handle
   */
  unsigned int getFaceColor(const typename Mesh::FaceHandle   _fh);

  /** \brief  eventually update vertex and index buffers
   *
   */
  void updateGPUBuffers();

  /** \brief  stores the vertex buffer on the gpu
   *
   */
  void createVBO();

  /** \brief stores the index buffer on the gpu
   *
   */
  void createIBO();

  /** \brief creates all vertex declarations needed for deferred draw call renderer
   *
   */
  void createVertexDeclarations();

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


  /**  \brief Optimized rendering of vertex picking ids with a shader
   *
   * @param _mvp model view projection transformation
   * @param _pickOffset base picking id of the first vertex
   */
  void drawPickingVertices_opt(const GLMatrixf& _mvp, int _pickOffset);


  /**  \brief Check if optimized vertex picking is supported
   *
   */
  bool supportsPickingVertices_opt();

  /**  \brief Update color picking array for the shader implementation
   *
   * @param _state
   */
  void updatePickingVertices_opt(ACG::GLState& _state);

#ifdef GL_ARB_texture_buffer_object

  TextureBuffer* pickVertexMap_opt(){
    if ( pickVertexMapTBO_.is_valid() )
      return &pickVertexMapTBO_;
    else {
      std::cerr << "Illegal request to pickVertexMap_opt when buffer is empty!" << std::endl;
      return 0;
    }
  }

#endif // GL_ARB_texture_buffer_object


  /** \brief get an index buffer mapping from openmesh vertices to drawmesh vbo vertices
  *
  * @return index buffer object (UNSIGNED_INT), might be 0 if the openmesh object is a point cloud
  */
  GLuint pickVertexIBO_opt() {return pickVertexIBO_;} // does not work

private:

  /// The vertex buffer used for vertex picking
  std::vector< ACG::Vec3f > pickVertBuf_;
  /// The color buffer used for vertex picking
  std::vector< ACG::Vec4uc > pickVertColBuf_;


#ifdef GL_ARB_texture_buffer_object
  // map from vbo vertex id to openmesh vertex id
  TextureBuffer pickVertexMapTBO_;
#endif // GL_ARB_texture_buffer_object

  // map from openmesh vertex to vbo vertex id
  GLuint pickVertexIBO_;

  // vertex picking shader
  GLSL::Program* pickVertexShader_;


  // selected shader picking method:
  //  0 -> use texturebuffer which maps from vbo id to openmesh vertex id
  //  1 -> draw with indexbuffer mapping from openmesh vertex id to vbo vertex
  int pickVertexMethod_;

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


  /**  \brief Optimized rendering of edge picking ids with a shader
   *
   * @param _mvp model view projection transformation
   * @param _pickOffset base picking id of the first edge
   */
  void drawPickingEdges_opt(const GLMatrixf& _mvp, int _pickOffset);


  /**  \brief Check if optimized face picking is supported
   *
   */
  bool supportsPickingEdges_opt();

  /**  \brief Update color picking array for the shader implementation
   *
   * @param _state
   */
  void updatePickingEdges_opt(ACG::GLState& _state );

private:

  std::vector< ACG::Vec4uc > pickEdgeBuf_;

  // edge picking shader
  GLSL::Program* pickEdgeShader_;


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

  /**  \brief Optimized rendering of face picking ids with a shader
   *
   * @param _mvp model view projection transformation
   * @param _pickOffset base picking id of the first face
   */
  void drawPickingFaces_opt(const GLMatrixf& _mvp, int _pickOffset);


  /**  \brief Check if optimized face picking is supported
   *
   */
  bool supportsPickingFaces_opt();

  /**  \brief Update color picking array for the shader implementation
   *
   * @param _state
   */
  void updatePickingFaces_opt(ACG::GLState& _state );

#ifdef GL_ARB_texture_buffer_object

  TextureBuffer* pickFaceTriangleMap_opt(){
    if ( pickFaceTriToFaceMapTBO_.is_valid() )
      return &pickFaceTriToFaceMapTBO_;
    else {
      std::cerr << "Illegal request to pickFaceTriangleMap_opt when buffer is empty!" << std::endl;
      return 0;
    }
  }
#endif

private:

  // unoptimized picking buffers
  std::vector< ACG::Vec3f > pickFaceVertexBuf_;
  std::vector< ACG::Vec4uc > pickFaceColBuf_;

#ifdef GL_ARB_texture_buffer_object
  // optimized picking with shaders: maps from triangle id in draw vbo to face id in openmesh
  TextureBuffer pickFaceTriToFaceMapTBO_;
#endif // GL_ARB_texture_buffer_object

  /// optimized face picking shader
  GLSL::Program* pickFaceShader_;

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

  /**  \brief Optimized rendering of any picking ids with a shader
   *
   * @param _mvp model view projection transformation
   * @param _pickOffset base picking id of the first element
   */
  void drawPickingAny_opt(const GLMatrixf& _mvp, int _pickOffset);

  /**  \brief Check if optimized any picking is supported
   *
   */
  bool supportsPickingAny_opt();

  /**  \brief Update color picking array for the shader implementation
   *
   * @param _state
   */
  void updatePickingAny_opt(ACG::GLState& _state );

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
   * @param _pOutMaxPolyVerts max face size
   * @param _pOutNumIndices   total number of indices
   *
   * @return  number of triangles
  */
  unsigned int countTris(unsigned int* _pOutMaxPolyVerts = 0, unsigned int* _pOutNumIndices = 0);

  /** \brief get the texture index of a triangle
   *
   *
   *  @param _tri Triangle index (-1 if not available)
   *
   *  @return Texture index of a triangle
   */
  int getTextureIDofTri(unsigned int _tri);

  /** \brief get the texture index of a face
   *
   *  @param _face Face index
   *  @return Face Texture id of face
   */
  int getTextureIDofFace(unsigned int _face);

  /** \brief get the data type of a mesh property
   *
   *
   *  @param _propData  mesh property data
   *  @param _outType [out] data type i.e. GL_FLOAT, GL_DOUBLE
   *  @param _outSize [out] number of atoms in range 1..4
   *  @param _outStride [out] size in bytes
   */
  template<class Prop>
  void getMeshPropertyType(Prop _propData, GLuint* _outType, int* _outSize, int* _outStride);

private:

  /// OpenMesh object to be rendered
  Mesh& mesh_;

  MeshCompiler* meshComp_;

  size_t numTris_, numVerts_;

  /// final index buffer used for rendering
  unsigned int* indices_;

  /// final vertex buffer used for rendering
  Vertex* vertices_;

  /// hint on what to rebuild
  unsigned int rebuild_;

  /** used to track mesh changes, that require a full rebuild
    * values directly taken from Mesh template
    */
  size_t prevNumFaces_,prevNumVerts_;


  GLuint vbo_,
         ibo_;

  /// index buffer used in Wireframe / Hiddenline mode
  GLuint lineIBO_;

  /// support for 2 and 4 byte unsigned integers
  GLenum indexType_;

  /// Color Mode: 0: none, 1: per vertex,  else: per face
  int colorMode_;

  /// Color Mode of vbo
  int curVBOColorMode_;

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


  /** inverse vertex map: original OpenMesh vertex index -> one vertex index in vbo
      this map is ambiguous and only useful for per vertex attributes rendering i.e. lines!
  */
  unsigned int* invVertexMap_;


  /// vertex buffer layout declaration with per vertex colors
  VertexDeclaration* vertexDecl_;

  /// vertex buffer layout declaration with per edge colors
  VertexDeclaration* vertexDeclEdgeCol_;

  /// vertex buffer layout declaration with per halfedge colors
  VertexDeclaration* vertexDeclHalfedgeCol_;

  /// vertex buffer layout declaration with halfedge positions only
  VertexDeclaration* vertexDeclHalfedgePos_;


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


private:
  //========================================================================
  // write functions for flexible vertex format

  void writeVertexElement(void* _dstBuf, unsigned int _vertex, unsigned int _stride, unsigned int _elementOffset, unsigned int _elementSize, const void* _elementData);

  void writeNormal(void* _dstBuf, unsigned int _vertex, unsigned int _stride, const ACG::Vec3d& _n);

  void writeTexcoord(void* _dstBuf, unsigned int _vertex, unsigned int _stride, const ACG::Vec2f& _uv);


  void writeVertexProperty(void* _dstBuf, unsigned int _vertex, unsigned int _stride, const VertexElement* _elementDesc, const ACG::Vec3f& _propf3);


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
  template<typename Mesh::Normal (DrawMeshT::*NormalLookup)(typename Mesh::FaceHandle)>
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


  /** \brief updates per edge and halfedge vertex declarations
  */
  void updateEdgeHalfedgeVertexDeclarations();


  /** \brief getter for vertex declarations
  */
  const VertexDeclaration* getEdgeColoredVertexDeclaration() const {return vertexDeclEdgeCol_;}

  /** \brief getter for vertex declarations
  */
  const VertexDeclaration* getHalfedgeVertexDeclaration() const {return vertexDeclHalfedgePos_;}

  /** \brief getter for vertex declarations
  */
  const VertexDeclaration* getHalfedgeColoredVertexDeclaration() const {return vertexDeclHalfedgeCol_;}


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
  template<typename Mesh::Normal (DrawMeshT::*NormalLookup)(typename Mesh::FaceHandle)>
  typename Mesh::Point halfedge_point(const typename Mesh::HalfedgeHandle _heh);

  inline
  typename Mesh::Normal cachedNormalLookup(typename Mesh::FaceHandle fh) {
      return mesh_.normal(fh);
  }

  inline
  typename Mesh::Normal computedTriMeshNormal(typename Mesh::FaceHandle fh) {
      typename Mesh::FVIter fv_iter = mesh_.fv_begin(fh);
      return (mesh_.point(*fv_iter) + mesh_.point(*(++fv_iter)) + mesh_.point(*(++fv_iter))) / 3.0;
  }

  inline
  typename Mesh::Normal computedNormal(typename Mesh::FaceHandle fh) {
      unsigned int count = 0;
      typename Mesh::Normal normal(0, 0, 0);
      for (typename Mesh::FVIter fv_it = mesh_.fv_begin(fh), fv_end = mesh_.fv_end(fh); fv_it != fv_end; ++fv_it) {
          normal += mesh_.point(*fv_it);
          ++count;
      }
      normal /= count;
      return normal;
  }

  typename Mesh::HalfedgeHandle mapToHalfedgeHandle(int _vertexId);

  void initMeshCompiler();

  template<class Prop>
  void setMeshCompilerInput(int _attrIdx, Prop _propData, int _num);

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
