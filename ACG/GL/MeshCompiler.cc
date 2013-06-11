
#include "MeshCompiler.hh"

#include <map>
#include <list>
#include <assert.h>
#include <iostream>


#include <ACG/Geometry/GPUCacheOptimizer.hh>


namespace ACG{

/*
use case


mesh with float3 pos, float2 uv, float3 n:

VertexElement elements[] = {{..}, ..};
VertexDeclaration decl;

decl.addElement(..float3 position..)
decl.addElement(..float2 uv..)
decl.addElement(..float3 normal..)

MeshCompiler mesh(decl);

mesh.setVertices(..)
mesh.setNormals(..)
mesh.setTexCoords(..)


mesh.setNumFaces(333);
mesh.setTriangles(indexBuffer);
for each face i
  mesh.setFaceMaterial(i, matID)

mesh.compile(FLAG_OPTIMIZE | FLAG_COMPUTE_TANGENTS | FLAG_STATIC)
*/


/* TODO
- default declaration (declaration can not be changed afterwards)
   float3 pos, float2 uv, float3 normal

- option to set triangle input buffer

- compile function with options
   FLAG_OPTIMIZE         : optimize for better gpu cache usage
   FLAG_COMPUTE_TANGENTS : compute tangent vectors if tangent slots are available in buffer layout
   FLAG_STATIC           : no update needed in future
*/


MeshCompiler::AdjacencyList::AdjacencyList()
: start(0), count(0), buf(0), bufSize(0), num(0)
{}

MeshCompiler::AdjacencyList::~AdjacencyList()
{
  delete [] start;
  delete [] buf;
}

void MeshCompiler::AdjacencyList::init( int n )
{
  delete [] start;
  delete [] buf;

  num = n;
  start = new int[2 * n];
  count = start + n;

  buf   = 0; // unknown buffer length

  // invalidate start indices
  memset(start, -1, n * sizeof(int));

  // reset count
  memset(count, 0, n * sizeof(int));  
}

int MeshCompiler::AdjacencyList::getAdj( int i, int k ) const
{
  int cnt = count[i];
  assert(k < cnt);
  assert(k > -1);

  int st = start[i];
  assert(st > -1);

  return buf[st + k];
}

int MeshCompiler::AdjacencyList::getCount( int i ) const
{
  return count[i];
}


void MeshCompiler::computeAdjacency()
{
  // count total number of adjacency entries
  // to pack everthing in a single tightly packed buffer

  const int numVerts = input_[inputIDPos_].count;

  adjacencyVert_.init(numVerts);

  // count # adjacent faces per vertex
  for (int i = 0; i < numFaces_; ++i)
  {
    int nCorners = getFaceSize(i);

    for (int k = 0; k < nCorners; ++k)
    {
      int vertex = faceInput_->getSingleFaceAttr(i, k, inputIDPos_);

      adjacencyVert_.count[vertex]++;
    }
  }


  // count num of needed entries
  int nCounter = 0;

  for (int i = 0; i < numVerts; ++i)
  {
    adjacencyVert_.start[i] = nCounter; // save start indices

    nCounter += adjacencyVert_.count[i];

    adjacencyVert_.count[i] = 0; // count gets recomputed in next step
  }

  // alloc memory
  adjacencyVert_.buf = new int[nCounter];
  adjacencyVert_.bufSize = nCounter;

  // build adjacency list
  for (int i = 0; i < numFaces_; ++i)
  {
    int nCorners = getFaceSize(i);

    for (int k = 0; k < nCorners; ++k)
    {
      int vertex = faceInput_->getSingleFaceAttr(i, k, inputIDPos_);
      int adjIdx = adjacencyVert_.start[vertex] + adjacencyVert_.count[vertex]++;

      adjacencyVert_.buf[ adjIdx ] = i;
    }
  }


  // ==============================================================
  // compute face -> face adjacency

  adjacencyFace_.init(numFaces_);

  // std::map too slow for complete face adjacency computation
//  std::map<int, int> faceMap;

  // OPtimization: use per face-flag instead of std::map to do redundancy check
  std::vector<int> faceMap2(numFaces_, -1);

  std::list<int> faceAdjTmp;

  // 1st - count # adjacent faces per face
  // make use of vertex adjacency list
  
  nCounter = 0; // counts # adjacency entries needed

  for (int i = 0; i < numFaces_; ++i)
  {
    const int nCorners = getFaceSize(i);

    int faceAdjCount = 0;

    for (int k = 0; k < nCorners; ++k)
    {
      int vertex = faceInput_->getSingleFaceAttr(i, k, inputIDPos_);

      // walk adjacency list to find neighboring faces
      int adjCount = adjacencyVert_.count[vertex];
      for (int q = 0; q < adjCount; ++q)
      {
        int adjFace = adjacencyVert_.getAdj(vertex, q);

        // add to map, taking care of duplicate entries
        if (adjFace != i)
        {
          //          faceMap[adjFace] = 1; // optimized code below
          if (faceMap2[adjFace] != 1)
          {
            faceAdjCount++;
            faceAdjTmp.push_back(adjFace);
            faceMap2[adjFace] = 1;
          }
        }

      }
    }

    // save count and start index
    adjacencyFace_.start[i] = nCounter;
    adjacencyFace_.count[i] = faceAdjCount;

    nCounter += faceAdjCount;


    for (std::list<int>::iterator it = faceAdjTmp.begin(); it != faceAdjTmp.end(); ++it)
    {
      faceMap2[*it] = -1;
    }

    faceAdjTmp.clear();

//    faceMap.clear();
  }



  // 2nd - build adjacency list

  // alloc
  adjacencyFace_.buf = new int[nCounter];
  adjacencyFace_.bufSize = nCounter;

  nCounter = 0;

  for (int i = 0; i < numFaces_; ++i)
  {
    int nCorners = getFaceSize(i);

    nCounter = 0;

    for (int k = 0; k < nCorners; ++k)
    {
      int vertex = faceInput_->getSingleFaceAttr(i, k, inputIDPos_);

      // walk adjacency list to find neighboring faces
      const int adjCount = adjacencyVert_.count[vertex];
      for (int q = 0; q < adjCount; ++q)
      {
        int adjFace = adjacencyVert_.getAdj(vertex, q);

        // add to map, taking care of duplicate entries
        if (adjFace != i)
        {
//          faceMap[adjFace] = 1; // optimized code below
          if (faceMap2[adjFace] != 1)
          {
            int adjIdx = adjacencyFace_.start[i];
            adjIdx += nCounter++;

            adjacencyFace_.buf[adjIdx] = adjFace;
          }

          faceMap2[adjFace] = 1;
        }
      }
    }

    /// unoptimized code
// 
//     for (std::map<int, int>::iterator it = faceMap.begin(); it != faceMap.end(); ++it)
//     {
//       assert(nCounter < adjacencyFace_.count[i]);
//       assert(adjacencyFace_.start[i] >= 0);
// 
//       int adjIdx = adjacencyFace_.start[i];
//       adjIdx += nCounter++;
// 
//       adjacencyFace_.buf[adjIdx] = it->first;
//     }

//    faceMap.clear();

    // optimized code, replacing faceMap.clear() :
    for (int k = 0; k < adjacencyFace_.getCount(i); ++k)
    {
      faceMap2[adjacencyFace_.getAdj(i, k)] = -1;
    }

  }
}


void MeshCompiler::setVertices( int _num, const void* _data, int _stride, bool _internalCopy /*= false*/ )
{
  setAttribVec(inputIDPos_, _num, _data, _stride, _internalCopy);
}

void MeshCompiler::setNormals( int _num, const void* _data, int _stride, bool _internalCopy /*= false*/ )
{
  setAttribVec(inputIDNorm_, _num, _data, _stride, _internalCopy);
}

void MeshCompiler::setTexCoords( int _num, const void* _data, int _stride, bool _internalCopy /*= false*/ )
{
  setAttribVec(inputIDTexC_, _num, _data, _stride, _internalCopy);
}


void MeshCompiler::setAttribVec(int _attrIdx, int _num, const void* _data, int _stride, bool _internalCopy /*= false*/)
{
  if (_attrIdx < 0)
    return;

  assert(_attrIdx < (int)decl_.getNumElements());

  VertexElementInput* inbuf = input_ + _attrIdx;

  inbuf->count = _num;

  int size = inbuf->attrSize = (int)VertexDeclaration::getElementSize(decl_.getElement(_attrIdx));

  if (_internalCopy)
  {
    inbuf->internalBuf = new char[size * _num];
    inbuf->data = inbuf->internalBuf;

    inbuf->stride = size;

    if (_data)
    {
      // copy elementwise because of striding
      for (int i = 0; i < _num; ++i)
      {
        memcpy(inbuf->internalBuf + size * i,
          (const char*)_data + _stride * i,
          size);
      }
    }
  }
  else
  {
    inbuf->data = (const char*)_data;
    inbuf->stride = _stride ? _stride : size;

    delete [] inbuf->internalBuf;
    inbuf->internalBuf = 0;
  }
}




void MeshCompiler::splitVertices()
{
  /* algorithm overview

  we split by indices only,
  actual vertex data will not be taken into account.

  thus if the input contains two vertices with the same value,
  they still are treated as different vertices since they have different indices


  a shared vertex gets split whenever at least one attribute
  changes with respect to the face.
  i.e. one face wants to combine vertex i with attribute j
  while another face needs it with attribute k

  example:

  face 0 and 5 share vertex v3

  face 0 uses v3 combined with normal n0,
  but face 5 wants to have v3 with normal n1


  we look up if v3 has been split up already and search
  for a v3-n0 combination
  eventually this combination is added to the splitting list

  repeat for the v3-n1 combination

  */

  // build internal face-offset table for storing interleaved index buffer
  int curOffset = 0;
  for (int i = 0; i < numFaces_; ++i)
  {
    faceStart_[i] = curOffset;

    int fsize = getFaceSize(i);
    curOffset += fsize;

    maxFaceCorners_ = std::max(maxFaceCorners_, fsize);
  }

  delete splitter_;
  splitter_ = new VertexSplitter(decl_.getNumElements(),
                                 input_[inputIDPos_].count,
                                 adjacencyVert_.bufSize);

  numIndices_ = faceInput_->getNumIndices();
  faceBufSplit_.resize(numIndices_, -1);

  // count # vertices after splitting
  numDrawVerts_ = 0;

  for (int i = 0; i < numFaces_; ++i)
  {
    const int fsize = getFaceSize(i);
    for (int k = 0; k < fsize; ++k)
    {
      int vertex[16];

      getInputFaceVertex(i, k, vertex);

      // split vertices by index data only
      // value of position, normal etc. are not considered
      int idx = splitter_->split(vertex);

      // handle index storage
      setInputIndexSplit(i, k, idx);
    }
  }
}


void MeshCompiler::forceUnsharedFaceVertex()
{
  // ==============================================
  //  face normal fix
  //  make sure that each triangle has at least one unique unshared vertex
  //  this vertex can store per face attributes when needed

  // the used algorithm is optimal (min split count) for any n-poly mesh
  //  however, this step should be done before triangulation
  //  and faces have to be triangulated in fans to ensure
  //  that each triangle makes use of the face-specific vertex


  std::vector<int> sharedVertices;
  sharedVertices.resize(maxFaceCorners_);

  std::vector<int> tmpFaceVerts; // used for face rotation-swap
  tmpFaceVerts.resize(maxFaceCorners_);

  faceRotCount_.resize(numFaces_, 0);

  for (int faceID = 0; faceID < numFaces_; ++faceID)
  {
    const int numCorners = getFaceSize(faceID);
    const int numAdj = adjacencyFace_.getCount(faceID);

    // reset shared list
    memset(&sharedVertices[0], 0, sizeof(int) * maxFaceCorners_);
    int numShared = 0;

    for (int k = 0; k < numAdj; ++k)
    {
      int adjFaceID = adjacencyFace_.getAdj(faceID, k);

      assert(adjFaceID >= 0);
      assert(adjFaceID < numFaces_);

      for (int v0 = 0; v0 < numCorners && numShared < numCorners; ++v0)
      {
        if (sharedVertices[v0])
          continue;

        int vertexID0 = getInputIndexSplit(faceID, v0);

        for (int v1 = 0; v1 < getFaceSize(adjFaceID); ++v1)
        {
          int vertexID1 = getInputIndexSplit(adjFaceID, v1);

          if (vertexID0 == vertexID1)
          {
            if (!sharedVertices[v0])
            {
              sharedVertices[v0] = true;
              ++numShared;
            }
          }
        }
      }

    }


    if (numShared == numCorners)
    {
      // worst-case: all vertices shared with neighbors

      // add split vertex to end of vertex buffer, which is used exclusively by the current face
      // current vertex count is stored in splitter_->numVerts

      setInputIndexSplit(faceID, 0, splitter_->numVerts++);
    }
    else if (sharedVertices[0])
    {
      // validation code
      int x = 0;
      for (int i = 0; i < numCorners; ++i)
        x += sharedVertices[i];
      assert(x < numCorners);

      // we have to make sure that an unshared vertex is the first referenced face vertex
      // this is currently not the case, so rotate the face indices until this is true

      // make copy of current face splitVertexID
      for (int i = 0; i < numCorners; ++i)
        tmpFaceVerts[i] = getInputIndexSplit(faceID, i);

      // rotation order: i -> i+1
      // find # rotations needed
      int rotCount = 1;
      for (; rotCount < numCorners; ++rotCount)
      {
        if (!sharedVertices[rotCount % numCorners])
          break;
      }

      assert(rotCount < numCorners);

      // rotate:  i -> i+rotCount
      rotCount = numCorners - rotCount;

      faceRotCount_[faceID] = rotCount;
   
      for (int i = 0; i < numCorners; ++i)
        setInputIndexSplit(faceID, (i + rotCount)%numCorners, tmpFaceVerts[i]);
    }
    // best-case: unshared vertex at corner 0

  }

}

void MeshCompiler::getInputFaceVertex( int i, int j, int* _out )
{
  for (unsigned int k = 0; k < decl_.getNumElements(); ++k)
    _out[k] = getInputIndex(i, j, k);
}





MeshCompiler::MeshCompiler(const VertexDeclaration& _decl)
: decl_(_decl)
{
  faceInput_ = 0;
  deleteFaceInputeData_ = false;

  splitter_ = 0;
  numSubsets_ = 0;
  numIndices_ = 0;
  numTris_ = 0;

  numFaces_ = 0;
  interleavedInput_ = 0;
  curFaceInputPos_ = 0;

  indices_ = 0;

  numDrawVerts_ = 0;

  maxFaceCorners_ = 0;

  // search for convenient attribute indices
  numAttributes_ = decl_.getNumElements();
  inputIDNorm_ = inputIDPos_ = inputIDTexC_ = -1;

  for (int i = 0; i < (int)decl_.getNumElements(); ++i)
  {
    const VertexElement* e = decl_.getElement(i);

    switch (e->usage_)
    {
    case VERTEX_USAGE_POSITION: inputIDPos_ = i; break;
    case VERTEX_USAGE_NORMAL: inputIDNorm_ = i; break;
    case VERTEX_USAGE_TEXCOORD: inputIDTexC_ = i; break;
    default: break;
    }
  }
}

MeshCompiler::~MeshCompiler()
{
  if (!triIndexBuffer_.empty() && indices_ != &triIndexBuffer_[0])
    delete [] indices_;


  if (deleteFaceInputeData_)
    delete faceInput_;

  delete splitter_;
}


int MeshCompiler::getInputIndex( const int _face, const int _corner, const int _attrId ) const
{
  return faceInput_->getSingleFaceAttr(_face, _corner, _attrId);
}

int MeshCompiler::getInputIndexOffset( const int _face, const int _corner, const bool _rotation ) const
{
  assert(_face >= 0);
  assert(_face < numFaces_);

  int baseIdx = faceStart_.empty() ? maxFaceCorners_ * _face : faceStart_[_face];
//  int fsize = faceCorners_.empty() ? maxFaceCorners_ : faceCorners_[_face];
  int fsize = getFaceSize(_face);

  int rotCount = faceRotCount_.empty() && _rotation ? 0 : faceRotCount_[_face];


  assert(baseIdx >= 0);
  assert(baseIdx <= numIndices_);

  assert(fsize > 0);

  return baseIdx + (_corner + rotCount) % fsize;
}


void MeshCompiler::setInputIndexSplit( const int _face, const int _corner, const int _val )
{
  int offset = getInputIndexOffset(_face, _corner);

  // keep track of number of vertices after splitting process
  if (_val >= numDrawVerts_)
    numDrawVerts_ = _val + 1;

  faceBufSplit_[offset] = _val;
}

int MeshCompiler::getInputIndexSplit( const int _face, const int _corner ) const
{
  int offset = getInputIndexOffset(_face, _corner);

  return faceBufSplit_[offset];
}




MeshCompilerDefaultFaceInput::MeshCompilerDefaultFaceInput(int _numFaces, int _numIndices)
: numFaces_(_numFaces), numIndices_(_numIndices)
{
  faceOffset_.resize(numFaces_, -1);
  faceSize_.resize(numFaces_, 0);
  faceData_[0].reserve(_numIndices);
}


void MeshCompiler::setNumFaces( const int _numFaces, const int _numIndices )
{
  if (faceInput_)
    return;

  MeshCompilerDefaultFaceInput* internalInput = new MeshCompilerDefaultFaceInput(_numFaces, _numIndices);

  numFaces_ = _numFaces;

  faceInput_ = internalInput;
  deleteFaceInputeData_ = true;

  // internal face-offset buffer required for mesh processing
  faceStart_.resize(_numFaces, -1);
  faceSize_.resize(_numFaces, 0);

}


void MeshCompiler::setFaceAttrib( int _i, int _numEdges, int* _v, int _attrID )
{
  if (!_v || _attrID < 0) return;

  if (!faceInput_)
    faceInput_ = new MeshCompilerDefaultFaceInput(0, 0);

  MeshCompilerDefaultFaceInput* input = dynamic_cast<MeshCompilerDefaultFaceInput*>(faceInput_);
  if (input)
  {
    input->setFaceData(_i, _numEdges, _v, _attrID);
  }

  if (faceSize_.empty())
    faceSize_.resize(numFaces_, 0);

  faceSize_[_i] = (short)_numEdges;

}

void MeshCompiler::setFaceAttrib( int _i, int _v0, int _v1, int _v2, int _attrID )
{
  int tmp[3] = {_v0, _v1, _v2};
  setFaceAttrib(_i, 3, tmp, _attrID);
}



MeshCompiler::VertexSplitter::VertexSplitter(int _numAttribs,
                                         int _numVerts,
                                         int _numWorstCase,
                                         float _estBufferIncrease)
: numAttribs(_numAttribs), numVerts(_numVerts)
{
  if (_numWorstCase <= 0)
    _numWorstCase = int(float(_numVerts) * (_estBufferIncrease + 1.0f));

  int maxCount = (_numAttribs + 1) * _numWorstCase;

  // alloc split list and invalidate
//   splits = new int[maxCount];
//   memset(splits, -1, maxCount * sizeof(int) );
  splits.resize(maxCount, -1);
}



MeshCompiler::VertexSplitter::~VertexSplitter()
{
//  delete [] splits;
}


int MeshCompiler::VertexSplitter::split(int* vertex)
{
  int pos = vertex[0];
  int next = getNext(pos);

  if (next < 0)
  {
    // 1st time reference

    // store attributes
    setAttribs(pos, vertex);

    // mark as referenced (next = this)
    setNext(pos, pos);
  }
  else
  {
    // already referenced

    int bSearchSplit = 1;

    // search vertex in split list
    while (pos >= 0&& bSearchSplit)
    {
      // is vertex already in split list?
      if (!memcmp(vertex, getAttribs(pos), numAttribs * sizeof(int)))
      {
        // found! reuse index
        return pos;
      }
      else
      {
        next = getNext(pos);
        
        if (next == pos) break; // avoid loop

        if (next < 0) break;    // end of list
        pos = next;             // go to next entry
      }
    }

    // combination not found -> add new vertex

    int newID = numVerts++;

    setNext(pos, newID);
    setAttribs(newID, vertex);

    pos = newID;
  }

  return pos;
}



int MeshCompiler::VertexSplitter::getNext(int id)
{
  assert(id >= 0);

  int entryIdx = id * (1 + numAttribs);

  // need more space?
  if (entryIdx >= (int)splits.size())
    splits.resize(entryIdx + numAttribs * 100, -1);

  return splits[entryIdx];
}

void MeshCompiler::VertexSplitter::setNext(int id, int next)
{
  assert(id >= 0);
  
  int entryIdx = id * (1 + numAttribs);

  // need more space?
  if (entryIdx >= (int)splits.size())
    splits.resize(entryIdx + numAttribs * 100, -1);

  splits[entryIdx] = next;
}

int* MeshCompiler::VertexSplitter::getAttribs(int id)
{
  assert(id >= 0);

  int entryIdx = id * (1 + numAttribs) + 1;

  // need more space?
  if (entryIdx + numAttribs >= (int)splits.size())
    splits.resize(entryIdx + numAttribs * 100, -1);

  return &splits[0] + entryIdx;
}

void MeshCompiler::VertexSplitter::setAttribs( int id, int* attr )
{
  memcpy(getAttribs(id), attr, numAttribs * sizeof(int));
}



MeshCompiler::VertexElementInput::VertexElementInput()
: internalBuf(0), data(0),
  count(0), stride(0), attrSize(0)
{
}

MeshCompiler::VertexElementInput::~VertexElementInput()
{
  delete [] internalBuf;
}








void MeshCompiler::triangulate()
{
  // count no. of triangles

  int numTris = 0;

  for (int i = 0; i < numFaces_; ++i)
    numTris += getFaceSize(i) - 2;

  numTris_ = numTris;

  triIndexBuffer_.resize(numTris * 3);
  triToSortFaceMap_.resize(numTris);

  // - find mapping (triToFaceMap):  triangle id -> sorted face id
  // - build triangle index buffer:  triIndexBuffer_

  int triCounter = 0;
  int indexCounter = 0;

  for (int sortFaceID = 0; sortFaceID < numFaces_; ++sortFaceID)
  {
    // get original face id
    int faceID = faceSortMap_.empty() ? sortFaceID : faceSortMap_[sortFaceID];

    int faceSize = getFaceSize(faceID);

    // save face index mapping
    triToSortFaceMap_[triCounter++] = sortFaceID;

    // convert polygon into triangle fan
    // NOTE: all triangles must use the first face-vertex here!
    for (int k = 0; k < 3; ++k) 
      triIndexBuffer_[indexCounter++] = getInputIndexSplit(faceID, k);

    for (int k = 3; k < faceSize; ++k)
    {
      // added tri belongs to current face
      triToSortFaceMap_[triCounter++] = sortFaceID;

      triIndexBuffer_[indexCounter++] = getInputIndexSplit(faceID, 0);
      triIndexBuffer_[indexCounter++] = getInputIndexSplit(faceID, k-1);
      triIndexBuffer_[indexCounter++] = getInputIndexSplit(faceID, k);
    }
  }


  // ---------------
  // fill out missing subset info:

  for (int i = 0; i < numSubsets_; ++i)
  {
    subsets_[i].startIndex = 0xffffffff;
    subsets_[i].numTris = 0;
  }

  // triangle count per subset

  for (int i = 0; i < numTris; ++i)
  {
    int faceID = mapTriToInputFace(i);

    int faceGroup = getFaceGroup(faceID);
    Subset* sub = &subsets_[findGroupSubset(faceGroup)];

    ++sub->numTris;
  }

  // start index

  for (int i = 0; i < numSubsets_; ++i)
  {
    if (i > 0)
      subsets_[i].startIndex = subsets_[i-1].startIndex + subsets_[i-1].numTris * 3;
    else
      subsets_[i].startIndex = 0;
  }

}


void MeshCompiler::sortFacesByGroup()
{
  // sort faces based on their group id
  // faces within the same group can be rendered in one batch

  numSubsets_ = 0;

  std::map<int, unsigned int> GroupIDs; // map[groupID] = first face

  // initialize GroupIDs map
  for (int face = 0; face < numFaces_; ++face)
  {
    int texID = getFaceGroup(face);

    if (GroupIDs.find(texID) == GroupIDs.end())
      GroupIDs[texID] = face;
  }

  // alloc subset array
  numSubsets_ = GroupIDs.size();
  subsets_.resize(numSubsets_);

  if (numSubsets_ > 1)
    faceSortMap_.resize(numFaces_, -1);

  // initialize subsets and face sorting map

  std::map<int, unsigned int>::const_iterator it = GroupIDs.begin();
  unsigned int numSortedFaces = 0;

  for (unsigned int i = 0; it != GroupIDs.end(); ++i, ++it)
  {
    // subset id = group id
    subsets_[i].id = it->first;

    // store id mapping (optimization)
    subsetIDMap_[it->first] = i;

    // rearrange by subset chunks, face offset = # processed faces
    subsets_[i].numFaces = 0;
    subsets_[i].startFace = numSortedFaces;

    // triangle information is computed later
    subsets_[i].startIndex = 0;
    subsets_[i].numTris = 0;

    if (numSubsets_ == 1)
    {
      subsets_[i].numFaces = numFaces_;
    }
    else
    {
      // - find # faces in this subset
      // - create face sorting map:  map[sortFaceID] = faceID
      for (int k = it->second; k < numFaces_; ++k)
      {
        int texID = getFaceGroup(k);

        if (texID == subsets_[i].id)
        {
          subsets_[i].numFaces++;

          faceSortMap_[numSortedFaces++] = k;
        }
      }
    }
  }

}


void MeshCompiler::optimize()
{
  indices_ = new int[numTris_ * 3];

  triOptMap_.resize(numTris_, -1);

  for (int i = 0; i < numSubsets_; ++i)
  {
    Subset* pSubset = &subsets_[i];

    GPUCacheOptimizerTipsify copt(24, pSubset->numTris, numDrawVerts_, 4, &triIndexBuffer_[0] + pSubset->startIndex);
    copt.WriteIndexBuffer(4, indices_ + pSubset->startIndex);

    // apply changes to trimap
    const unsigned int StartTri = pSubset->startIndex/3;
    for (unsigned int k = 0; k < pSubset->numTris; ++k)
    {
      unsigned int SrcTri = copt.GetTriangleMap()[k];
      triOptMap_[k + StartTri] = SrcTri + StartTri;
    }

  }

  // call to GPUCacheOptimizer::OptimizeVertices!

  unsigned int* vertexOptMap = new unsigned int[numDrawVerts_];

  GPUCacheOptimizer::OptimizeVertices(numTris_, numDrawVerts_, 4, indices_, vertexOptMap);

  // apply vertexOptMap on index buffer

  for (int i = 0; i < numTris_ * 3; ++i)
    indices_[i] = vertexOptMap[indices_[i]];


  // apply opt-map on current vertex-map

  for (int i = 0; i < numFaces_; ++i)
  {
    const int frot = faceRotCount_.empty() ? 0 : faceRotCount_[i];
    const int fsize = getFaceSize(i);

    for (int k = 0;  k < fsize; ++k)
    {
      // undo face rotation
      int rotIdx = k - frot;
      if (rotIdx < 0) rotIdx += fsize;

      int oldVertex = getInputIndexSplit(i, rotIdx);

      int newVertex = vertexOptMap[oldVertex];

      setInputIndexSplit(i, rotIdx, newVertex);
    }
  }

  delete [] vertexOptMap;
}



void MeshCompiler::build(bool _optimizeVCache, bool _needPerFaceAttribute)
{
  /*
  1. step
  Split vertices s.t. we can create an interleaved vertex buffer.
  Neighboring faces may share a vertex with different normals,
  which gets resolved here.

  output:
   faceBufSplit_

  faceBufSplit_ reroutes the original index buffer to their unique
  vertex indices. So the orig. indices (faceBuf_) are still needed
  to know how to compose each vertex and faceBufSplit_ gives them
  a new index.
 
  faceBufSplit_ should not be used directly,
  use get/setInputIndexSplit for the mapping between interleaved indices and face vertices.
  */
  
  // Adjacency info can be used to predict the split count [optional]
  if (_needPerFaceAttribute)
    computeAdjacency();
 
  splitVertices();

  if (_needPerFaceAttribute)
  {
    // The first vertex of each face shall not be referenced by any other face.
    // This vertex can then be used to store per-face data
    
    // Adjacency info needed here
    forceUnsharedFaceVertex();
  }

  
  /*
  2. step

  Sort faces by materials, grouping faces with the same material together.

  output:
   subsets_           (tri group offsets for each mat-id)
   triIndexBuffer_  (updated)
   triToSortFaceMap_      (updated)
  */
  sortFacesByGroup();


  /*
  3. step

  Triangulate any n-polys and unfold triangles to a new index buffer.

  output:
   numTris_
   triIndexBuffer_  (tri index buffer)
   triToSortFaceMap_      (map triangle id -> face id)

  From this point on we have to keep track of triToSortFaceMap_
  to compose vertices. 
  
  Assembling the vertex for corner "k" of triangle "tri" works as followed.
    int dstID = triIndexBuffer_[tri * 3 + k];
    int srcFace = triToSortFaceMap_[tri];

    int vertexAttribs[16];
    getInputFaceVertex(srcFace, k, vertexAttribs);

    vertexBuffer[dstID].pos = inputVerts_[vertexAttribs[POS]];
    vertexBuffer[dstID].normal = inputVerts_[vertexAttribs[NORMAL]];
    ...
  */
  triangulate();


  /*
  4. step

  Optimize indices for efficient gpu vcache usage.
  This is done within the boundaries of each subset.
  Also, this operates on triangles instead of single indices,
  which means only complete triangles are reordered and
  will not be rotated afterwards.

  Additionally reorders vertex indices in increasing order.

  output:
   triIndexBuffer_  (updated)
   triOptMap
  */
  if (_optimizeVCache)
    optimize();
  else if (!triIndexBuffer_.empty())
    indices_ = &triIndexBuffer_[0];

  createVertexMap();

}






void MeshCompiler::setAttrib( int _attrIdx, int _v, const void* _data )
{
  VertexElementInput* inbuf = input_ + _attrIdx;

  if (!inbuf->internalBuf)
  {
    std::cerr << "setAttrib failed: please allocate internal input buffer before using setAttrib" << std::endl;
    return;
  }

  assert(inbuf->count > _v);

  memcpy(inbuf->internalBuf + _v * inbuf->stride, _data, inbuf->attrSize);
}



int MeshCompiler::mapTriToInputFace( int _tri )
{
  assert(_tri >= 0);
  assert(_tri < numTris_);

  int sortFaceID = triToSortFaceMap_[_tri];
  assert(sortFaceID >= 0);
  assert(sortFaceID < numFaces_);

  int faceID = faceSortMap_.empty() ? sortFaceID : faceSortMap_[sortFaceID];
  assert(faceID >= 0);
  assert(faceID < numFaces_);

  return faceID;
}

int MeshCompiler::getFaceGroup( int _faceID ) const
{
  if (faceGroupIDs_.empty())
    return -1;

  return faceGroupIDs_[_faceID];
}

int MeshCompiler::findGroupSubset( int _groupID )
{
  return subsetIDMap_[_groupID];
}

const MeshCompiler::Subset* MeshCompiler::getSubset( int _i ) const
{
  return &subsets_[_i];
}



void MeshCompiler::dbgdump(const char* _filename) const
{
  FILE* file = 0;
  
  file = fopen(_filename, "wt");

  if (file)
  {
    for (int i = 0; i < numAttributes_; ++i)
    {
      const VertexElementInput* inp = input_ + i;
      fprintf(file, "attribute[%d]: internalbuf 0x%p, data 0x%p, count %d, stride %d, attrSize %d\n",
        i,  inp->internalBuf, inp->data, inp->count, inp->stride, inp->attrSize);
    }

    fprintf(file, "\n\n");

    fprintf(file, "faces %d\nindices %d\n", numFaces_, numIndices_);

    fprintf(file, "interleavedInput %d\n\n", interleavedInput_);

    for (size_t i = 0; i < faceBufSplit_.size(); ++i)
      fprintf(file, "faceBufSplit_[%d] = %d\n", (int)i, faceBufSplit_[i]);

    fprintf(file, "\n\n");

    for (size_t i = 0; i < faceGroupIDs_.size(); ++i)
      fprintf(file, "faceGroupIDs_[%d] = %d\n", (int)i, faceGroupIDs_[i]);

    fprintf(file, "\n\n");

    for (size_t i = 0; i < faceSortMap_.size(); ++i)
      fprintf(file, "faceSortMap_[%d] = %d\n", (int)i, faceSortMap_[i]);

    fprintf(file, "\n\n");
    
    for (size_t i = 0; i < faceSortMap_.size(); ++i)
      fprintf(file, "faceSortMap_[%d] = %d\n", (int)i, faceSortMap_[i]);


    fprintf(file, "\n\n");

    for (size_t i = 0; i < triIndexBuffer_.size()/3; ++i)
      fprintf(file, "tri[%d] = %d %d %d\n", (int)i, triIndexBuffer_[i*3], triIndexBuffer_[i*3+1], triIndexBuffer_[i*3+2]);


    fprintf(file, "\n\n");

    for (size_t i = 0; i < subsets_.size(); ++i)
    {
      const Subset* sub = &subsets_[i];
      fprintf(file, "subset[%d]: id %d, startIndex %d, numTris %d, numFaces %d, startFace %d\n", 
        (int)i, sub->id, sub->startIndex, sub->numTris, sub->numFaces, sub->startFace);
    }

    fprintf(file, "\n\n");

    for (std::map<int, int>::const_iterator it = subsetIDMap_.begin();
      it != subsetIDMap_.end(); ++it)
      fprintf(file, "subsetIDMap[%d] = %d\n", it->first, it->second);

    fprintf(file, "\n\n");

    for (std::map<int, int>::const_iterator it = subsetIDMap_.begin();
      it != subsetIDMap_.end(); ++it)
      fprintf(file, "subsetIDMap[%d] = %d\n", it->first, it->second);

    fprintf(file, "\n\n");

    fprintf(file, "adjacencyVert\n");
    adjacencyVert_.dbgdump(file);

    fprintf(file, "\n\n");

    fprintf(file, "adjacencyFace\n");
    adjacencyFace_.dbgdump(file);

    fprintf(file, "\n\n");

    fclose(file);
  }
}


void MeshCompiler::AdjacencyList::dbgdump(FILE* file) const
{
  if (file)
  {
    for (int i = 0; i < num; ++i)
    {
      for (int k = 0; k < getCount(i); ++k)
        fprintf(file, "adj[%d][%d] = %d\n", i, k, getAdj(i, k));
    }
  }
}

void MeshCompiler::VertexElementInput::getElementData(int _idx, void* _dst) const
{
  assert(_idx >= 0);
  assert(_idx < count);

  memcpy(_dst, data + _idx * stride, attrSize);
}


void MeshCompiler::dbgdumpObj(const char* _filename) const
{
  FILE* file = 0;

  file = fopen(_filename, "wt");

  if (file)
  {
    for (int i = 0; i < numDrawVerts_; ++i)
    {
      float vertex[64];

      getVertex(i, vertex);

      if (inputIDPos_ >= 0)
      {
        float* pos = vertex + (size_t)decl_.getElement(inputIDPos_)->pointer_ / 4;
        fprintf(file, "v %f %f %f\n", pos[0], pos[1], pos[2]);
      }

      if (inputIDNorm_ >= 0)
      {
        float* pos = vertex + (size_t)decl_.getElement(inputIDNorm_)->pointer_ / 4;
        fprintf(file, "vn %f %f %f\n", pos[0], pos[1], pos[2]);
      }

      if (inputIDTexC_ >= 0)
      {
        float* pos = vertex  + (size_t)decl_.getElement(inputIDTexC_)->pointer_ / 4;
        fprintf(file, "vt %f %f\n", pos[0], pos[1]);
      }
    }


    for (int i = 0; i < numTris_; ++i)
    {
      if (!inputIDTexC_ && !inputIDNorm_)
        fprintf(file, "f %d %d %d\n", indices_[i*3]+1, indices_[i*3+1]+1, indices_[i*3+2]+1);

      else if (!inputIDTexC_)
        fprintf(file, "f %d/%d %d/%d %d/%d\n", indices_[i*3]+1,indices_[i*3]+1, indices_[i*3+1]+1,indices_[i*3+1]+1, indices_[i*3+2]+1,indices_[i*3+2]+1);

      else
        fprintf(file, "f %d/%d/%d %d/%d/%d %d/%d/%d\n",
                indices_[i*3]+1,indices_[i*3]+1,indices_[i*3]+1,
                indices_[i*3+1]+1,indices_[i*3+1]+1,indices_[i*3+1]+1, 
                indices_[i*3+2]+1,indices_[i*3+2]+1,indices_[i*3+2]+1);
    }


    fclose(file);
  }
}

void MeshCompiler::setFaceVerts( int _i, int _numEdges, int* _v )
{
  setFaceAttrib(_i, _numEdges, _v, inputIDPos_);
}

void MeshCompiler::setFaceVerts( int _i, int _v0, int _v1, int _v2 )
{
  int tmp[] = {_v0, _v1, _v2};
  setFaceAttrib(_i, 3, tmp, inputIDPos_);
}

void MeshCompiler::setFaceNormals( int _i, int _numEdges, int* _v )
{
  setFaceAttrib(_i, _numEdges, _v, inputIDNorm_);
}

void MeshCompiler::setFaceNormals( int _i, int _v0, int _v1, int _v2 )
{
  int tmp[] = {_v0, _v1, _v2};
  setFaceAttrib(_i, 3, tmp, inputIDNorm_);
}


void MeshCompiler::setFaceTexCoords( int _i, int _numEdges, int* _v )
{
  setFaceAttrib(_i, _numEdges, _v, inputIDTexC_);
}

void MeshCompiler::setFaceTexCoords( int _i, int _v0, int _v1, int _v2 )
{
  int tmp[] = {_v0, _v1, _v2};
  setFaceAttrib(_i, 3, tmp, inputIDTexC_);
}


int MeshCompiler::getNumTriangles() const
{
  return numTris_;
}

void MeshCompiler::getVertex( int _id, void* _out ) const
{
  int faceID = vertexMap_[_id].first;

  // revert face rotation here to get the correct input vertex
  int frot = faceRotCount_.empty() ? 0 : faceRotCount_[faceID];
  int cornerID = vertexMap_[_id].second - frot;
  if (cornerID < 0) cornerID += getFaceSize(faceID);

  for (int i = 0; i < numAttributes_; ++i)
  {
    const VertexElement* el = decl_.getElement(i);

    int idx = faceInput_->getSingleFaceAttr(faceID, cornerID, i);

    input_[i].getElementData(idx, (char*)_out + (size_t)el->pointer_);
  }
}

int MeshCompiler::getIndex( int _i ) const
{
  assert(_i >= 0);
  assert(_i < numTris_ * 3);
  return indices_[_i];
}

void MeshCompiler::createVertexMap()
{
  vertexMap_.resize(numDrawVerts_, std::pair<int, int>(-1, -1));

  for (int i = 0; i < numFaces_; ++i)
  {
    for (int k = 0; k < getFaceSize(i); ++k)
    {
      int vertexID = getInputIndexSplit(i, k);
      vertexMap_[vertexID] = std::pair<int, int>(i, k);
    }
  }

  // validate
#ifdef _DEBUG
  for (int i = 0; i < numDrawVerts_; ++i)
  {
    if (vertexMap_[i].first < 0 ||
      vertexMap_[i].second < 0)
      std::cerr << "mesh-assembler: vertexMap error at index: " << i << std::endl;
  }
#endif // _DEBUG

}


void MeshCompiler::createFaceMap()
{
  faceToTriMap_.resize(numTris_, -1); 

  for (int i = 0; i < numTris_; ++i)
  {
    int faceID = mapToOriginalFaceID(i);

    // offset into lookup table
    int offset = faceStart_.empty() ? faceID * maxFaceCorners_ : faceStart_[faceID];

    int triNo = 0;

    // search for free entry
    while (faceToTriMap_[offset + triNo] == -1)
      ++triNo;

    assert(triNo < getFaceSize(faceID) - 2);

    faceToTriMap_[offset + triNo] = i;
  }
}



void MeshCompiler::dbgdumpAdjList( const char* _filename ) const
{
  FILE* file = 0;
  
  file = fopen(_filename, "wt");

  if (file)
  {
    fprintf(file, "vertex-adjacency: \n");
    adjacencyVert_.dbgdump(file);

    fprintf(file, "face-adjacency: \n");
    adjacencyFace_.dbgdump(file);

    fclose(file);
  }
}

void MeshCompiler::setFaceGroup( int _i, int _groupID )
{
  if ((int)faceGroupIDs_.size() < numFaces_)
    faceGroupIDs_.resize(numFaces_, -1);

  faceGroupIDs_[_i] = _groupID;
}

void MeshCompiler::getVertexBuffer( void* _dst, const int _offset /*= 0*/, const int _range /*= -1*/ )
{
  int batchSize = _range < 0 ? numDrawVerts_ - _offset : _range;

  char* bdst = (char*)_dst;

  for (int i = 0; i < batchSize; ++i)
  {
    getVertex(i, bdst + decl_.getVertexStride() * i);
  }
}

int MeshCompiler::mapToOriginalFaceID( const int _i ) const
{
  int faceID = _i;
  if (!triOptMap_.empty())
    faceID = triOptMap_[faceID];

  if (!triToSortFaceMap_.empty())
    faceID = triToSortFaceMap_[faceID];

  if (!faceSortMap_.empty())
    faceID = faceSortMap_[faceID];

  return faceID;
}

void MeshCompiler::mapToOriginalVertexID( const int _i, int& _faceID, int& _cornerID ) const
{
  _faceID = vertexMap_[_i].first;
  _cornerID = vertexMap_[_i].second;
}

int MeshCompiler::mapToDrawVertexID( const int _faceID, const int _cornerID ) const
{
  return getInputIndexSplit(_faceID, _cornerID);
}

int MeshCompiler::mapToDrawTriID( const int _faceID, const int _k /*= 0*/, int* _numTrisOut /*= 0*/ ) const
{
  const int fsize = getFaceSize(_faceID);
  assert(_k < fsize - 2);

  if (_numTrisOut)
    *_numTrisOut = fsize;

  int offset = faceStart_.empty() ? _faceID * maxFaceCorners_ : faceStart_[_faceID];

  return faceToTriMap_[offset + _k];
}

int MeshCompiler::getMemoryUsage() const
{
  size_t usage = faceStart_.size() * 4;
  usage += faceSize_.size() * 2;

  usage += faceGroupIDs_.size() * 4;
  
  usage += faceBufSplit_.size() * 4;
  usage += faceRotCount_.size() * 4;
  usage += faceSortMap_.size() * 4;
  usage += triIndexBuffer_.size() * 4;
  
  usage += subsetIDMap_.size() * sizeof(Subset);


  usage += adjacencyVert_.bufSize;
  usage += adjacencyFace_.bufSize;

  if (splitter_)
    usage += splitter_->splits.size() * 4;

  usage += triToSortFaceMap_.size() * 4;
  usage += triOptMap_.size() * 4;
  usage += vertexMap_.size() * 8;
  usage += faceToTriMap_.size() * 4;

  usage += numTris_ * 3 * 4; // indices_

  return usage;
}


void MeshCompilerDefaultFaceInput::dbgWriteToObjFile(FILE* _file, int _posAttrID, int _normalAttrID, int _texcAttrID)
{
  for (int i = 0; i < numFaces_; ++i)
  {
    std::string strLine = "f ";

    int offset = faceOffset_[i];
    int size = faceSize_[i];

    char tmp[0xff];
    for (int k = 0; k < size; ++k)
    {
      if (_normalAttrID>=0 && _texcAttrID>=0)
        sprintf(tmp, "%d/%d/%d ", faceData_[_posAttrID][offset+k]+1, 
                                    faceData_[_texcAttrID][offset+k]+1,
                                    faceData_[_normalAttrID][offset+k]+1);
      else if (_normalAttrID >= 0)
        sprintf(tmp, "%d//%d ", faceData_[_posAttrID][offset + k]+1, faceData_[_normalAttrID][offset+k]+1);
      else if (_texcAttrID >= 0)
        sprintf(tmp, "%d/%d ", faceData_[_posAttrID][offset + k]+1, faceData_[_texcAttrID][offset+k]+1);
      else
        sprintf(tmp, "%d ", faceData_[_posAttrID][offset + k]+1);

      strLine += tmp;
    }

    fprintf(_file, "%s\n", strLine.c_str());
  }
}

bool MeshCompilerDefaultFaceInput::getFaceAttr( int _faceID, int _attrID, int* _out )
{
  int offset = faceOffset_[_faceID];
  int fsize = faceSize_[_faceID];

  for (int i = 0; i < fsize; ++i)
    _out[i] = faceData_[_attrID][offset+i];

  return true;
}

int MeshCompilerDefaultFaceInput::getSingleFaceAttr( int _faceID, int _faceCorner, int _attrID )
{
  int offset = faceOffset_[_faceID];
  assert(_faceCorner < getFaceSize(_faceID));

  return faceData_[_attrID][offset + _faceCorner];
}

void MeshCompilerDefaultFaceInput::setFaceData( int _faceID, int _size, int* _data, int _attrID /*= 0*/ )
{
  if (faceData_[_attrID].capacity() == 0)
    faceData_[_attrID].reserve(numIndices_);

  if ((int)faceOffset_.size() <= _faceID)
    faceOffset_.resize(_faceID+1, -1);

  if ((int)faceSize_.size() <= _faceID)
    faceSize_.resize(_faceID+1, -1);

  faceSize_[_faceID] = _size;

  if (faceOffset_[_faceID] < 0)
  {
    // append new face data to stream
    faceOffset_[_faceID] = faceData_[_attrID].size();
    for (int i = 0; i < _size; ++i)
      faceData_[_attrID].push_back(_data[i]);
  }
  else
  {
    int offs = faceOffset_[_faceID];

    if ((int)faceData_[_attrID].size() == offs)
    {
      for (int i = 0; i < _size; ++i)
        faceData_[_attrID].push_back(_data[i]);
    }
    else
    {
      // resize array
      if ((int)faceData_[_attrID].size() < offs + _size + 1)
        faceData_[_attrID].resize(offs + _size + 1, -1);

      // update existing face data
      for (int i = 0; i < _size; ++i)
        faceData_[_attrID][offs + i] = _data[i];
    }
  }
}

int MeshCompilerFaceInput::getSingleFaceAttr( int _faceID, int _faceCorner, int _attrID )
{
  const int fsize = getFaceSize(_faceID);

  assert(_faceID < getNumFaces());
  assert(_faceID >= 0);
  assert(_faceCorner >= 0);
  assert(_faceCorner < fsize);

  int retVal = -1;

  int* tmpPtr = getFaceAttr(_faceID, _attrID);

  if (tmpPtr)
    retVal = tmpPtr[_faceCorner];
  else
  {
    tmpPtr = new int[fsize];
    if (getFaceAttr(_faceID, _attrID, tmpPtr))
      retVal = tmpPtr[_faceCorner];
    delete [] tmpPtr;
  }

  return retVal;
}



}
