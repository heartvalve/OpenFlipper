#pragma once

#include "VertexDeclaration.hh"

#include <map>
#include <vector>
#include <cstdio>
#include <string>

#include <ACG/GL/gl.hh>

/*

Mesh buffer assembler:

Builds a pair of vertex and index buffer based on a poly mesh.


- flexible processing pipeline
- uses index mapping only -> lower memory consumption
- independent of OpenMesh, OpenGL
- 



usage

1. Create a vertex declaration to specify your wanted
   vertex format, such as float3 pos, float3 normal..

2. Set your vertex data.
   Example in (float3 pos, float3 normal, float2 texc) format:
   
   float VertexPositions[100*3] = {..};
   float VertexNormals[120*3] = {..};
   float VertexUV[80*2] = {..};

   drawMesh->setVertices(100, VertexPositions, 12);
   drawMesh->setNormals(120, VertexNormals, 12);
   drawMesh->setTexCoords(80, VertexUV, 8);

   Note that different indices for vertices, normals and texcoords are allowed,
   hence the various element numbers 100, 120 and 80.


   Example 2 (interleaved input)

   float Vertices[100] = {
                            x0, y0, z0,   u0, v0,   nx0, ny0, nz0,
                            x1, y1, z1,   u1, v1,   nx1, ny1, nz1,
                            ...
                          };

   The stride is 8*4 = 32 bytes.
   We use parameters as follows.

   drawMesh->setVertices(100, Vertices, 32);
   drawMesh->setNormals(100, (char*)Vertices + 20, 32);
   drawMesh->setTexCoords(100, (char*)Vertices + 12, 32);
   
3. Set index data.

   Two methods are supported for this.

   You can either specify one index set for all vertex attributes
   or use another index buffer for each vertex attribute.
   The latter means having different indices for vertex and texcoords for example.


   drawMesh->setNumFaces(32, 96);


   for each face i
     int* faceVertexIndices = {v0, v1, v2, ...};
     setFaceVerts(i, 3, faceVertexIndices);

4. finish the initialization by calling the build() function

*/

namespace ACG{

class ACGDLLEXPORT MeshCompilerFaceInput
{
  // face data input interface
  // allows flexible and memory efficient face data input

public:
  MeshCompilerFaceInput(){}
  virtual ~MeshCompilerFaceInput(){}

  virtual int getNumFaces() const = 0;

  /** Get total number of indices in one attribute channel.
   *
   * i.e. total number of position indices of the whole mesh
  */
  virtual int getNumIndices() const = 0;

  /** Get number of vertices per face.
   * @param _faceID face index
  */
  virtual int getFaceSize(const int _faceID) const = 0;

  /** Get a single vertex-index entry of a face.
   *
   * @param _faceID face index
   * @param _faceCorner vertex corner of the face
   * @param _attrID attribute channel 
   * @return index-data if successful, -1 otherwise
  */
  virtual int getSingleFaceAttr(const int _faceID, const int _faceCorner, const int _attrID) const;

  /** Get an index buffer of a face for a specific attribute channel.
   * @param _faceID face index
   * @param _attrID attribute channel
   * @param _out pointer to output buffer, use getFaceSize(_faceID) to get the size needed to store face data
   * @return true if successful, false otherwise
  */
  virtual bool getFaceAttr(const int _faceID, const int _attrID, int* _out) const {return false;}

  /** Get an index buffer of a face for a specific attribute channel.
   * @param _faceID face index
   * @param _attrID attribute channel
   * @return array data of size "getFaceSize(_faceID)", allowed to return 0 when array data not permanently available in memory
  */
  virtual int* getFaceAttr(const int _faceID, const int _attrID) const {return 0;}


  // Adjacency information can be provided if it has been generated already.
  // Otherwise it will be generated on the fly when needed, which might be time consuming.

  /** Get the number of adjacent faces for a vertex.
   * @param _vertexID vertex index
   * @return number of adjacent faces, return -1 if adjacency information unavailable
  */
  virtual int getVertexAdjCount(const int _vertexID) const {return -1;}

  /** Get the index of an adjacent face for a vertex.
   * @param _vertexID vertex index
   * @param _k adjacency list entry in range [0, .., adjCount - 1]
   * @return face id of adjacent face, return -1 if adjacency information is available
  */
  virtual int getVertexAdjFace(const int _vertexID, const int _k) const {return -1;}

};

class ACGDLLEXPORT MeshCompilerDefaultFaceInput : public MeshCompilerFaceInput
{
public:
  MeshCompilerDefaultFaceInput(int _numFaces, int _numIndices);
  virtual ~MeshCompilerDefaultFaceInput(){}

  int getNumFaces() const  {return numFaces_;}
  int getNumIndices() const {return numIndices_;}

  int getFaceSize(const int _faceID) const {return faceSize_[_faceID];}

  int getSingleFaceAttr(const int _faceID, const int _faceCorner, const int _attrID) const;

  bool getFaceAttr(const int _faceID, const int _attrID, int* _out);

  void dbgWriteToObjFile(FILE* _file, int _posAttrID = 0, int _normalAttrID = -1, int _texcAttrID = -1);


  void setFaceData(int _faceID, int _size, int* _data, int _attrID = 0);

protected:

  int numFaces_,
    numIndices_;

  // input data is stored in a sequence stream
  //  face offsets may not be in sequence
  std::vector<int> faceOffset_;
  std::vector<int> faceSize_;

  // face index buffer for each vertex attribute
  std::vector<int> faceData_[16];

};


class ACGDLLEXPORT MeshCompilerVertexCompare
{
public:

  virtual bool equalVertex(const void* v0, const void* v1, const VertexDeclaration* _decl);
};

class ACGDLLEXPORT MeshCompiler
{
public:

  MeshCompiler(const VertexDeclaration& _decl);

  virtual ~MeshCompiler();

//===========================================================================
/** @name Vertex Data Input
* @{ */
//===========================================================================  

  /** set input vertex positions
   *
   * @param _num Number of vertex positions
   * @param _data Pointer to vertex data
   * @param _stride Difference in bytes between two vertex positions in _data. Default value 0 indicates a tight float3 position array without any other data or memory alignment.
   * @param _internalCopy Memory optimization flag: select true if the provided data address is only temporarily valid. Otherwise an internal copy must be made.
   * @param _fmt data format of one element (must be set if input data does not match vertex declaration)
   * @param _elementSize number of elements per attribute (i.e. 3 for vec3 ..,  -1 if unknown)
  */
  void setVertices(int _num, const void* _data, int _stride = 0, bool _internalCopy = false, GLuint _fmt = 0, int _elementSize = -1);

  /** set input normals
   *
   * @param _num Number of normals
   * @param _data Pointer to normals data
   * @param _stride Difference in bytes between two normals positions in _data. Default value 0 indicates a tight float3 position array without any other data or memory alignment.
   * @param _internalCopy Memory optimization flag: select true if the provided data address is only temporarily valid. Otherwise an internal copy must be made.
   * @param _fmt data format of one element (must be set if input data does not match vertex declaration)
   * @param _elementSize number of elements per attribute (i.e. 3 for vec3 ..,  -1 if unknown)
  */
  void setNormals(int _num, const void* _data, int _stride = 0, bool _internalCopy = false, GLuint _fmt = 0, int _elementSize = -1);

  /** set input texture coords
   *
   * @param _num Number of texture coords
   * @param _data Pointer to texture coord data
   * @param _stride Difference in bytes between two texture coordinate positions in _data. Default value 0 indicates a tight float3 position array without any other data or memory alignment.
   * @param _internalCopy Memory optimization flag: select true if the provided data address is only temporarily valid. Otherwise an internal copy must be made.
   * @param _fmt data format of one element (must be set if input data does not match vertex declaration)
   * @param _elementSize number of elements per attribute (i.e. 3 for vec3 ..,  -1 if unknown)
  */
  void setTexCoords(int _num, const void* _data, int _stride = 0, bool _internalCopy = false, GLuint _fmt = 0, int _elementSize = -1);

  /** Set custom input attribute.
  *
  * Alternatively allocates an internal buffer only, such that data can be provided via setAttrib().
  * @param _attrIdx      Attribute id from VertexDeclaration
  * @param _num          Number of attributes
  * @param _data         Input data buffer, may be null to only
  * @param _stride       Offset difference in bytes to the next attribute in _data. Default value 0 indicates no data alignment/memory packing.
  * @param _internalCopy Create an internal buffer and make a copy _data
  * @param _fmt data format of one element (must be set if input data does not match vertex declaration)
  * @param _elementSize number of elements per attribute (i.e. 3 for vec3 ..,  -1 if unknown)
  */
  void setAttribVec(int _attrIdx, int _num, const void* _data, int _stride = 0, bool _internalCopy = false, GLuint _fmt = 0, int _elementSize = -1);

  /** set one single vertex
      setVertices with internalCopy = true must be called before
      to have an effect
  */
  void setVertex(int _v, float* _f);
  void setVertex(int _v, float _x, float _y, float _z, float _w);

  void setAttrib(int _attrIdx, int _v, const void* _data);

/** @} */  


//===========================================================================
/** @name Flexible Face Data Input
* @{ */
//===========================================================================  

  /** Set Face data input
   *
   * Making use of the MeshCompilerFaceInput interface completly overrides the default input behavior.
   * Any subsequent call to default input data functions such as setNumFaces(), setFaceVerts() etc. will be ignored
   *
   * @param _faceInput user defined face input (no internal copy made, do not delete while using MeshCompiler)
   */
  void setFaceInput(MeshCompilerFaceInput* _faceInput);

/** @} */  

//===========================================================================
/** @name Default Face Data Input
* @{ */
//===========================================================================  

  /** \brief Set number of faces and indices if known by user
   *
   * User may give a rough estimate of face/index count. 
   * A more accurate estimation improves efficiency: too low numbers result in performance hit, too high numbers in memory consumption
   * @param _numFaces Number of faces. Value 0 accepted at cost of performance
   * @param _numIndices Number of indices, i.e. 3 * numFaces for triangle meshes. Value 0 accepted at cost of performance
   */
  void setNumFaces(const int _numFaces, const int _numIndices);


  /** \brief Set index buffer for a triangle mesh.
   *
   * This should only be used if the input vertex buffer is interleaved already.
   * @param _numTris Number of triangles.
   * @param _indexSize Size in bytes of one index.
   * @param _indices Pointer to a buffer containing the index data.
   */
  void setIndexBufferInterleaved(int _numTris, int _indexSize, const void* _indices);

  /** \brief Set vertex ids per triangle.
   *
   * @param _i Face ID
   * @param _v0 1st vertex id
   * @param _v1 2nd vertex id
   * @param _v2 3rd vertex id
   */
  void setFaceVerts(int _i, int _v0, int _v1, int _v2);

  /** \brief Set vertex ids per face.
   *
   * @param _i Face id
   * @param _faceSize Size of face, ie. number of vertices of face
   * @param _v Vertex ids
   */
  void setFaceVerts(int _i, int _faceSize, int* _v);

  /** \brief Set normal ids per triangle.
   *
   * @param _i Face ID
   * @param _v0 1st normal id
   * @param _v1 2nd normal id
   * @param _v2 3rd normal id
   */
  void setFaceNormals(int _i, int _v0, int _v1, int _v2);
  
  /** \brief Set normal ids per face.
   *
   * @param _i Face id
   * @param _faceSize Size of face, ie. number of vertices of face
   * @param _v Normal ids
   */
  void setFaceNormals(int _i, int _faceSize, int* _v);

  /** \brief Set texcoord ids per triangle.
   *
   * @param _i Face ID
   * @param _v0 1st texcoord id
   * @param _v1 2nd texcoord id
   * @param _v2 3rd texcoord id
   */
  void setFaceTexCoords(int _i, int _v0, int _v1, int _v2);

  /** \brief Set texcoord ids per face.
   *
   * @param _i Face id
   * @param _faceSize Size of face, ie. number of vertices of face
   * @param _v Texcoord ids
   */
  void setFaceTexCoords(int _i, int _faceSize, int* _v);


  /** \brief Set attribute ids per triangle.
   *
   * @param _i Face id
   * @param _v0 1st element id
   * @param _v1 2nd element id
   * @param _v2 3rd element id
   * @param _attrID Which attribute: index of VertexDeclaration element array
   */
  void setFaceAttrib(int _i, int _v0, int _v1, int _v2, int _attrID);

  /** \brief Set attribute ids per face.
   *
   * @param _i        Face id
   * @param _faceSize Size of face, ie. number of vertices of face
   * @param _v        Element ids
   * @param _attrID   Which attribute: index of VertexDeclaration element array
   */
  void setFaceAttrib(int _i, int _faceSize, int* _v, int _attrID);


/** @} */  

//===========================================================================
/** @name Face Grouping and Subsets
* @{ */
//===========================================================================  


  /** \brief Specify face groups.
   *
   * Faces with the same group ID will be chunked together in the sorting process.
   * This feature may be used for material/texture subsets.
   * @param _i       Face ID
   * @param _groupID Custom group ID
   */
  void setFaceGroup(int _i, short _groupID);

  // subset/group management
  struct Subset
  {
    int id; // subset id
    unsigned int startIndex; // 1st occurrence of group in index buffer
    unsigned int numTris; // number of tris belonging to subset in index buffer

    unsigned int numFaces; // number of faces belonging to subset
    unsigned int startFace; // index into sorted list of faces
  };

  /** \brief get subset ID of a group
   *
   * @param _groupID Id of the group
   * @return Subset Id
   */
  int findGroupSubset(int _groupID);

  /** \brief Get Face Group of the given face
   *
   * @param _faceID Id of the face
   * @return Group Id
   */
  int getFaceGroup(int _faceID) const;

  /** \brief Get Group Id of the triangle
   *
   * @param _triID Id of the triangle
   * @return Group of the triangle
   */
  int getTriGroup(int _triID) const;

  /** \brief Get the number of subsets
   *
   * @return Number of subsets
   */
  int getNumSubsets() const {return (int)subsets_.size();}

  /** \brief get a specific subset
   *
   * @param _i  Id of the subset
   * @return    The subset
   */
  const Subset* getSubset(int _i) const;


/** @} */  

//===========================================================================
/** @name Mesh Compilation
* @{ */
//===========================================================================  


  /** \brief Build vertex + index buffer.
   * 
   * @param _weldVertices Compare vertices and attempt to eliminate duplicates. High computation cost
   * @param _optimizeVCache Reorder faces for optimized vcache usage. High computation cost
   * @param _needPerFaceAttribute User wants to set per-face attributes in draw vertex buffer. The first referenced vertex of each face can be used to store per-face data. Low computation cost
   * @param _keepIsolatedVertices Isolated vertices should not be discarded in the output vertex buffer
  */
  void build(bool _weldVertices = false, bool _optimizeVCache = true, bool _needPerFaceAttribute = false, bool _keepIsolatedVertices = false);

  /** Get number of vertices in final buffer.
  */
  int getNumVertices() const {return numDrawVerts_;}

  /** See glProvokingVertex()
   *
   * Specifiy the vertex to be used as the source of data for flat shading.
   * The default value is 2, meaning that the last vertex of each triangle will be used.
   * setProvokingVertex() must be called prior to build(), if a different provoking vertex is desired.
   * Additionally build() has to set its _needPerFaceAttribute parameter to true to enable provoking vertices.
   *
   * @param _v triangle vertex where the provoking vertex should be stored [0, 1, 2]
   */
  void setProvokingVertex(int _v);


  /** \brief Get vertex buffer ready for rendering.
   * 
   * Query final vertex buffer data.
   * Support vertex buffer batch uploads.
   * @param _dst [out] Pointer to memory address where the vertex buffer should be copied to
   * @param _offset Begin of vertex buffer batch
   * @param _range Size of vertex buffer batch. Copies rest of buffer if _range < 0.
  */
  void getVertexBuffer(void* _dst, const int _offset = 0, const int _range = -1);

  /** Get index buffer ready for rendering.
  */
  int* getIndexBuffer() const {return indices_;}

  /** Get number of triangles in final buffer.
  */
  int getNumTriangles() const {return numTris_;}

  /** Get number of input faces.
  */
  int getNumFaces() const;

  /** \brief Get size of input face
   *
   * @param _i Index
   * @return Size
   */
  inline int getFaceSize(const int _i) const
  {
    return faceSize_.empty() ? maxFaceSize_ : faceSize_[_i];
  }

  /** Get Vertex declaration.
  */
  const VertexDeclaration* getVertexDeclaration() const {return &decl_;}

  /** Get vertex in final draw vertex buffer.
  */
  void getVertex(int _id, void* _out) const;

  /** Get index in final draw index buffer.
  */
  int getIndex(int _i) const;


/** @} */  



//===========================================================================
/** @name Input/Output ID mapping
* @{ */
//===========================================================================  


  /** Mapping from draw vertex id -> input vertex id
   *
   * @param _i Vertex ID in draw buffer
   * @param _faceID [out] Face ID in face input buffer
   * @param _cornerID [out] Corner of face corresponding to vertex.
   * @return Position ID in input buffer
  */
  int mapToOriginalVertexID(const int _i, int& _faceID, int& _cornerID) const;

  /** Mapping from draw tri id -> input face id
   *
   * @param _triID Triangle ID in draw index buffer
   * @return Input Face ID
  */
  int mapToOriginalFaceID(const int _triID) const;



  /** Mapping from input vertex id -> draw vertex id
   *
   * @param _faceID Face ID in input data
   * @param _cornerID Corner of face
   * @return Draw Vertex ID in output vertex buffer
  */
  int mapToDrawVertexID(const int _faceID, const int _cornerID) const;

  /** Mapping from input Face id -> draw vertex id
   *
   * @param _faceID Face ID in input data
   * @param _k triangle no. associated to face, offset 0
   * @param _numTrisOut [out] Number of triangles associated to face (if input face was n-poly)
   * @return Draw Triangle ID in output vertex buffer
  */
  int mapToDrawTriID(const int _faceID, const int _k = 0, int* _numTrisOut = 0) const;


/** @} */  





private:

  // compute adjacency information: vertex -> neighboring faces (, face -> neighboring faces [removed] )
  void computeAdjacency();

  // convert per-face vertices to unique ids
  void splitVertices();

private:

  // small helper functions

  /** i: face index
      j: corner index
      _out: output vertex (index for each attribute)
  */
  void getInputFaceVertex(const int _face, const int _corner, int* _out) const;

  /** i: face index
      j: corner index
      _out: output vertex (index for each attribute) post welding operation
  */
  void getInputFaceVertex_Welded(int _face, int _corner, int* _out) const;

  /** i: face index
      j: corner index
      _out: output vertex address (vertex data)
  */
  void getInputFaceVertexData(int _face, int _corner, void* _out) const;


  inline int getInputIndex( const int& _face, const int& _corner, const int& _attrId ) const
  {
    return faceInput_->getSingleFaceAttr(_face, _corner, _attrId);

    // alternatively avoid virtual function call at cost of higher memory overhead ( could not confirm any run-time difference )
    // to use this, uncomment code in prepareData() that initializes faceData_ array as well
//    return faceData_[(getInputFaceOffset(_face) + _corner) * numAttributes_ + _attrId];
  }


private:

  // ====================================================
  // input data


  // vertex buffer input

  struct ACGDLLEXPORT VertexElementInput 
  {
    VertexElementInput();
    ~VertexElementInput();

     /// mem alloc if attribute buffer managed by this class
    char* internalBuf;

    /** address to data input, will not be released by MeshCompiler
        - may be an external address provided by user of class
    */
    const char* data;

    /// # elements in buffer
    int count;

    /// offset in bytes from one element to the next
    int stride;

    /// size in bytes of one attribute
    int attrSize;


    // vertex data access

    /// element data format
    GLuint fmt;

    /// number of ints/floats/bytes per element 
    int elementSize;

    /// read a vertex element
    void getElementData(int _idx, void* _dst, const VertexElement* _desc) const;
  };
  
  // input vertex data
  VertexElementInput  input_[16];

  // convenient attribute indices
  int inputIDPos_;  // index of positions into input_ array
  int inputIDNorm_; // index of normals into input_ array
  int inputIDTexC_; // index of texcoords into input_ array

  int                 numAttributes_;
  VertexDeclaration   decl_;


  // input face data

  int   numFaces_, 
        numIndices_;
  std::vector<int>  faceStart_;    // start position in buf for each face
  std::vector<unsigned char> faceSize_;    // face size, copy of faceInput_->getFaceSize() for better performance
  std::vector<int> faceData_;
  size_t   maxFaceSize_;           // max(faceSize_)
  bool  constantFaceSize_;
  std::vector<short>  faceGroupIDs_; // group id for each face (optional input)
  int   curFaceInputPos_;          // current # indices set by user

  MeshCompilerFaceInput* faceInput_;   // face data input interface
  bool deleteFaceInputeData_;       // delete if face input data internally created

  std::vector<int>  faceBufSplit_; // mapping from (faceID, cornerID) to interleaved vertex id after splitting
  std::vector<unsigned char>  faceRotCount_; // # ccw rotations per face, handled internally by getInputIndexOffset
  std::vector<int>  faceSortMap_;  // face IDs sorted by group; maps sortFaceID -> FaceID
  int               provokingVertex_; // provoking vertex of each triangle
  bool              provokingVertexSetByUser_; // was the provoking vertex selected by user or set to default?

  int   numTris_;
  std::vector<int>  triIndexBuffer_; // triangulated index buffer with interleaved vertices

  // IDs of isolated vertices: index into input position buffer
  std::vector<int>  isolatedVertices_;

  // face grouping with subsets for per-face materials
  int     numSubsets_;
  std::vector<Subset> subsets_;
  std::map<int, int> subsetIDMap_; // maps groupId -> subsetID

  // =====================================================

  struct ACGDLLEXPORT AdjacencyList 
  {
    AdjacencyList()
      : start(0), count(0), buf(0), bufSize(0), num(0) {}
    ~AdjacencyList()
    {
      delete [] start;
      delete [] buf;
      delete [] count;
    }

    void init(int n);
    int  getAdj(int i, int k) const;
    int  getCount(int i) const;

    void clear();

    int* start;   // index to adjacency buffer
    unsigned char* count;   // # of adjacent faces
    int* buf;     // adjacency data
    int  bufSize; // size of buf
    int  num;     // # adjacency entries

    void dbgdump(FILE* file) const;
  };

  // adjacency list: vertex -> faces
  AdjacencyList adjacencyVert_;

  // adjacency access interface (choosing between user input / self generated data)
  int getAdjVertexFaceCount(int _vertexID) const;
  int getAdjVertexFace(int _vertexID, int _k) const;


  struct WeldListEntry
  {
    int faceId;
    unsigned char cornerId;
    
    int refFaceId;
    unsigned char refCornerId;
  };

  struct ACGDLLEXPORT WeldList
  {
    void add(const int _face, const int _corner);

    std::vector< WeldListEntry > list;

    MeshCompiler* meshComp;
    MeshCompilerVertexCompare* cmpFunc;

    char* workBuf;
  };


  static MeshCompilerVertexCompare defaultVertexCompare;
  MeshCompilerVertexCompare* vertexCompare_;

  // mapping from <faceId, faceCornerId> -> <weldFaceId, weldFaceCorner>
//  std::vector< std::pair<int, unsigned char> > vertexWeldMap_;   // std::pair<int, unsigned char> gets padded to 8 bytes
  std::vector< int > vertexWeldMapFace_;
  std::vector< unsigned char > vertexWeldMapCorner_;


  struct ACGDLLEXPORT VertexSplitter 
  {
    // worst case split: num entries in vertex adj list
    // estBufferIncrease: if numWorstCase == 0, then we estimate an increase in vertex buffer by this percentage
    VertexSplitter(int numAttribs,
                   int numVerts,
                   int numWorstCase = 0,
                   float estBufferIncrease = 0.5f);

    ~VertexSplitter();

    /// returns a unique index for a vertex-attribute combination
    int split(int* vertex);

    /// check if vertex is isolated with a complete splitting list
    bool isIsolated(const int vertexPosID);

    int  numAttribs;

    /// number of vertex combinations currently in use
    int  numVerts;

    /** split list format:
         for each vertex: [next split,  attribute ids]
          next split: -1 -> not split yet
                       i -> index into split list for next combination
                             with the same vertex position id (single linked list)

          attribute ids:
            array of attribute indices into input buffers,
            that makes up the complete vertex
            -1 -> vertex not used yet (relevant for split() only)
    */
//    int* splits;
    std::vector<int> splits;

    // split list access
    int  getNext(const int id);
    int* getAttribs(const int id);
    void setNext(const int id, const int next);
    void setAttribs(const int id, int* attr);

    // debugging metrics
    int dbg_numResizes;
    int dbg_numSplits;
  };

  VertexSplitter*  splitter_;

  // =====================================================

  // mappings

  /// maps from triangle ID to sorted face ID
  std::vector<int> triToSortFaceMap_;

  /// maps from optimized tri ID to unoptimized tri ID
  std::vector<int> triOptMap_;

  /// vertex index in vbo -> input (face id, corner id) pair , also inverse of faceBufSplit_
//  std::vector<std::pair<int, unsigned char> > vertexMap_; // sizeof( std::pair<int, unsigned char> ) = 8!!
  std::vector< int > vertexMapFace_;
  std::vector< unsigned char > vertexMapCorner_;

  /// input face index -> output tri index
  std::vector<int> faceToTriMap_;
  std::vector<int> faceToTriMapOffset_; 

  /// output tri index -> input face index
  std::vector<int> triToFaceMap_;

  // =====================================================

  // final buffers used for drawing

  /// # vertices in vbo
  int numDrawVerts_;

  /// # isolated vertices
  int numIsolatedVerts_;

  /// index buffer
  int*  indices_;

private:

  // return interleaved vertex id for input buffers
  int getInputIndexSplit(const int _face, const int _corner) const;

  void setInputIndexSplit(const int _face, const int _corner, const int _val);

  int mapTriToInputFace(const int _tri) const;

  int getInputIndexOffset(const int _face, const int _corner, const bool _rotation = true) const;

  inline int getInputFaceOffset(const int _face) const
  {
    return faceStart_.empty() ? maxFaceSize_ * _face : faceStart_[_face];
  }

  /// build() preparation
  void prepareData();

  // find isolated vertices
  void findIsolatedVertices();

  // make sure each face has one vertex id without any references by neighboring faces
  // split vertices when necessary
  void forceUnsharedFaceVertex();

  // eliminate duplicate attribute entries
  void weldVertices();

  // fix incomplete welding map if mesh contains isolated vertices
  void fixWeldMap();

  // convert n-poly -> tris (triangle fans)
  void triangulate();

  // sort input faces by group ids
  void sortFacesByGroup();

  // v-cache optimization + vertex reorder
  void optimize();

  // create vertex mapping: input id <-> final buffer id
  void createVertexMap(bool _keepIsolatedVerts);

  // create face mapping: input id <-> final tri id
  void createFaceMap();

public:
  // debugging tools

  /// dump mesh info to text file
  void dbgdump(const char* _filename) const;

  /// dump mesh in wavefront obj format
  void dbgdumpObj(const char* _filename) const;

  /// dump input mesh to binary file format
  void dbgdumpInputBin(const char* _filename, bool _seperateFiles = false) const;

  /// dump input mesh to wavefront obj format
  void dbgdumpInputObj(const char* _filename) const;

  /// dump adjacency list to text file
  void dbgdumpAdjList(const char* _filename) const;

  /// test correctness of input <-> output id mappings, unshared per face vertex.. logs errors in file
  bool dbgVerify(const char* _filename) const;

  /// interpret vertex data according declaration and write to string
  std::string vertexToString(const void* v) const;

  /// return memory consumption in bytes
  /// @param _printConsole print detailed memory costs to command console
  size_t getMemoryUsage(bool _printConsole = true) const;

  /// check for errors in input data
  std::string checkInputData() const;
};



}
