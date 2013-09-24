#pragma once

#include "VertexDeclaration.hh"

#include <map>
#include <vector>
#include <cstdio>
#include <string>

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

class MeshCompilerFaceInput
{
  // face data input interface
  // allows flexible and memory efficient face data input

public:
  MeshCompilerFaceInput(){}
  virtual ~MeshCompilerFaceInput(){}

  virtual int getNumFaces() = 0;

  /** Get total number of indices in one attribute channel.
   *
   * i.e. total number of position indices of the whole mesh
  */
  virtual int getNumIndices() = 0;

  /** Get number of vertices per face.
   * @param _faceID face index
  */
  virtual int getFaceSize(int _faceID) const = 0;

  /** Get a single vertex-index entry of a face.
   *
   * @param _faceID face index
   * @param _faceCorner vertex corner of the face
   * @param _attrID attribute channel 
   * @return index-data if successful, -1 otherwise
  */
  virtual int getSingleFaceAttr(int _faceID, int _faceCorner, int _attrID);

  /** Get an index buffer of a face for a specific attribute channel.
   * @param _faceID face index
   * @param _attrID attribute channel
   * @param _out pointer to output buffer, use getFaceSize(_faceID) to get the size needed to store face data
   * @return true if successful, false otherwise
  */
  virtual bool getFaceAttr(int _faceID, int _attrID, int* _out) {return false;}

  /** Get an index buffer of a face for a specific attribute channel.
   * @param _faceID face index
   * @param _attrID attribute channel
   * @return array data of size "getFaceSize(_faceID)", allowed to return 0 when array data not permanently available in memory
  */
  virtual int* getFaceAttr(int _faceID, int _attrID) {return 0;}
};

class MeshCompilerDefaultFaceInput : public MeshCompilerFaceInput
{
public:
  MeshCompilerDefaultFaceInput(int _numFaces, int _numIndices);
  virtual ~MeshCompilerDefaultFaceInput(){}

  int getNumFaces() {return numFaces_;}
  int getNumIndices() {return numIndices_;}

  int getFaceSize(int _faceID) const {return faceSize_[_faceID];}

  int getSingleFaceAttr(int _faceID, int _faceCorner, int _attrID);

  bool getFaceAttr(int _faceID, int _attrID, int* _out);

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

class MeshCompiler
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
  */
  void setVertices(int _num, const void* _data, int _stride = 0, bool _internalCopy = false);

  /** set input normals
   *
   * @param _num Number of normals
   * @param _data Pointer to normals data
   * @param _stride Difference in bytes between two normals positions in _data. Default value 0 indicates a tight float3 position array without any other data or memory alignment.
   * @param _internalCopy Memory optimization flag: select true if the provided data address is only temporarily valid. Otherwise an internal copy must be made.
  */
  void setNormals(int _num, const void* _data, int _stride = 0, bool _internalCopy = false);

  /** set input texture coords
   *
   * @param _num Number of texture coords
   * @param _data Pointer to texture coord data
   * @param _stride Difference in bytes between two texture coordinate positions in _data. Default value 0 indicates a tight float3 position array without any other data or memory alignment.
   * @param _internalCopy Memory optimization flag: select true if the provided data address is only temporarily valid. Otherwise an internal copy must be made.
  */
  void setTexCoords(int _num, const void* _data, int _stride = 0, bool _internalCopy = false);

  /** Set custom input attribute.
  *
  * Alternatively allocates an internal buffer only, such that data can be provided via setAttrib().
  * @param _attrIdx      Attribute id from VertexDeclaration
  * @param _num          Number of attributes
  * @param _data         Input data buffer, may be null to only
  * @param _stride       Offset difference in bytes to the next attribute in _data. Default value 0 indicates no data alignment/memory packing.
  * @param _internalCopy Create an internal buffer and make a copy _data
  */
  void setAttribVec(int _attrIdx, int _num, const void* _data, int _stride = 0, bool _internalCopy = false);

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
  void setFaceGroup(int _i, int _groupID);

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
   * @param _optimizeVCache Reorder faces for optimized vcache usage. Low performance hit on build() execution time
   * @param _needPerFaceAttribute User wants to set per-face attributes in draw vertex buffer. The first referenced vertex of each face can be used to store per-face data. High performance hit on execution time
  */
  void build(bool _optimizeVCache = true, bool _needPerFaceAttribute = false);


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
  int getNumTriangles() const;

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
    return faceSize_[_i];
  }

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
  */
  void mapToOriginalVertexID(const int _i, int& _faceID, int& _cornerID) const;

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

  // compute adjacency information: vertex -> neighboring faces, face -> neighboring faces
  void computeAdjacency();

  // convert per-face vertices to unique ids
  void splitVertices();

private:

  // small helper functions

  /** i: face index
      j: corner index
      _out: output vertex (index for each attribute)
  */
  void getInputFaceVertex(int _face, int _corner, int* _out) const;

  int getInputIndex(const int _face, const int _corner, const int _attrId) const;

private:

  // ====================================================
  // input data

  // vertex buffer input

  struct VertexElementInput 
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

    /// read a vertex element
    void getElementData(int _idx, void* _dst) const;
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
  std::vector<short> faceSize_;    // face size, copy of faceInput_->getFaceSize() for better performance
  size_t   maxFaceCorners_;           // max(faceCorners_)
  std::vector<int>  faceGroupIDs_; // group id for each face (optional input)
  int   curFaceInputPos_;          // current # indices set by user

  MeshCompilerFaceInput* faceInput_;   // face data input interface
  bool deleteFaceInputeData_;       // delete if face input data internally created


  std::vector<int>  faceBufSplit_; // index buffer for the interleaved vertex buffer
  std::vector<int>  faceRotCount_; // # rotation ccw face rotation applied, handled internally by getInputIndexOffset
  std::vector<int>  faceSortMap_;  // face IDs sorted by group; maps sortFaceID -> FaceID

  int   numTris_;
  std::vector<int>  triIndexBuffer_; // triangulated index buffer with interleaved vertices



  // face grouping with subsets for per-face materials
  int     numSubsets_;
  std::vector<Subset> subsets_;
  std::map<int, int> subsetIDMap_; // maps groupId -> subsetID

  // =====================================================

  struct AdjacencyList 
  {
    AdjacencyList();
    ~AdjacencyList();

    void init(int n);
    int  getAdj(int i, int k) const;
    int  getCount(int i) const;

    int* start;   // index to adjacency buffer
    int* count;   // # of adjacent faces
    int* buf;     // adjacency data
    int  bufSize; // size of buf
    int  num;     // # adjacency entries

    void dbgdump(FILE* file) const;
  };

  // adjacency list: vertex -> faces
  AdjacencyList adjacencyVert_;

  // adjacency: face -> faces
  AdjacencyList adjacencyFace_;



  struct VertexSplitter 
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
    int  getNext(int id);
    int* getAttribs(int id);
    void setNext(int id, int next);
    void setAttribs(int id, int* attr);
  };

  VertexSplitter*  splitter_;

  // =====================================================

  // mappings

  /// maps from triangle ID to sorted face ID
  std::vector<int> triToSortFaceMap_;

  /// maps from optimized tri ID to unoptimized tri ID
  std::vector<int> triOptMap_;

  /// vertex index in vbo -> input (face id, corner id) pair
  std::vector<std::pair<int, int> > vertexMap_;

  /// input face index -> output tri index
  std::vector<int> faceToTriMap_;

  // =====================================================

  // final buffers used for drawing

  /// # vertices in vbo
  int numDrawVerts_;

  /// index buffer
  int*  indices_;

private:

  // return interleaved vertex id for input buffers
  int getInputIndexSplit(const int _face, const int _corner) const;

  void setInputIndexSplit(const int _face, const int _corner, const int _val);

  int mapTriToInputFace(int _tri);

  int getInputIndexOffset(const int _face, const int _corner, const bool _rotation = true) const;


  /// build() preparation
  void prepareData();

  // make sure each face has one vertex id without any references by neighboring faces
  // split vertices when necessary
  void forceUnsharedFaceVertex();

  // convert n-poly -> tris (triangle fans)
  void triangulate();

  // sort input faces by group ids
  void sortFacesByGroup();

  // v-cache optimization + vertex reorder
  void optimize();

  // create vertex mapping: input id <-> final buffer id
  void createVertexMap();

  // create face mapping: input id <-> final tri id
  void createFaceMap();

public:
  // debugging tools

  /// dump mesh info to text file
  void dbgdump(const char* _filename) const;

  /// dump mesh in wavefront obj format
  void dbgdumpObj(const char* _filename) const;

  /// dump adjacency list to text file
  void dbgdumpAdjList(const char* _filename) const;

  /// return memory consumption in bytes
  size_t getMemoryUsage() const;

  /// check for errors in input data
  std::string checkInputData() const;
};



}
