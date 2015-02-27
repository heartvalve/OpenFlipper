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


#include "MeshCompiler.hh"

#include <map>
#include <list>
#include <cassert>
#include <iostream>
#include <sstream>
#include <algorithm>

#ifdef USE_OPENMP
#include <omp.h>
#endif

#ifdef ACG_MC_USE_STL_HASH
#include <unordered_map> // requires c++0x
#else
#include <QHash>  // alternative to unordered_map
#endif // ACG_MC_USE_STL_HASH

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


MeshCompilerVertexCompare MeshCompiler::defaultVertexCompare;

void MeshCompiler::AdjacencyList::init( int n )
{
  delete [] start;
  delete [] buf;
  delete [] count;

  num = n;
  start = new int[n];
  count = new unsigned char[n];

  buf   = 0; // unknown buffer length

  // invalidate start indices
  memset(start, -1, n * sizeof(int));

  // reset count
  memset(count, 0, n * sizeof(unsigned char));  
}

int MeshCompiler::AdjacencyList::getAdj( const int i, const int k ) const
{
  assert(k < count[i]);
  assert(k > -1);

  int st = start[i];
  assert(st > -1);

  return buf[st + k];
}

int MeshCompiler::AdjacencyList::getCount( const int i ) const
{
  return count[i];
}

void MeshCompiler::AdjacencyList::clear()
{
  num = 0;
  delete [] start; start = 0;
  delete [] buf; buf = 0;
  delete [] count; count = 0;
  bufSize = 0;
}



void MeshCompiler::computeAdjacency()
{
  const int numVerts = input_[inputIDPos_].count;

  // ==============================================================
  // compute vertex -> face adjacency

  // count total number of adjacency entries
  // store adj entries in a single tightly packed buffer

  // check if user provided adjacency information
  if (faceInput_->getVertexAdjCount(0) < 0 && adjacencyVert_.bufSize <= 0)
  {
    adjacencyVert_.init(numVerts);

    // count # adjacent faces per vertex
    for (int i = 0; i < numFaces_; ++i)
    {
      int nCorners = getFaceSize(i);

      for (int k = 0; k < nCorners; ++k)
      {
        //        int vertex = faceInput_->getSingleFaceAttr(i, k, inputIDPos_);
        int vertex = getInputIndex(i, k, inputIDPos_);

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
        //        int vertex = faceInput_->getSingleFaceAttr(i, k, inputIDPos_);
        int vertex = getInputIndex(i, k, inputIDPos_);
        int adjIdx = adjacencyVert_.start[vertex] + adjacencyVert_.count[vertex]++;

        adjacencyVert_.buf[ adjIdx ] = i;
      }
    }

    //////////////////////////////////////////////////////////////////////////
    // debug version:
    // dump computed and external adjacency for comparison
//     dbgdumpAdjList("dbg_adjacency_mc.txt");
// 
//     if (faceInput_->getVertexAdjCount(0) >= 0)
//     {
//       FILE* file = 0;
//       file = fopen("dbg_adjacency_ext.txt", "wt");
// 
//       if (file)
//       {
//         fprintf(file, "vertex-adjacency: \n");
//         for (int i = 0; i < input_[inputIDPos_].count; ++i)
//         {
//           // sorting the adjacency list for easy comparison of adjacency input
//           int count = faceInput_->getVertexAdjCount(i);
// 
//           std::vector<int> sortedList(count);
//           for (int k = 0; k < count; ++k)
//             sortedList[k] = faceInput_->getVertexAdjFace(i, k);
// 
//           std::sort(sortedList.begin(), sortedList.end());
// 
//           for (int k = 0; k < count; ++k)
//             fprintf(file, "adj[%d][%d] = %d\n", i, k, sortedList[k]);
//         }
// 
// 
//         fclose(file);
//       }
//     }
    
  }
    
}


void MeshCompiler::setVertices( int _num, const void* _data, int _stride, bool _internalCopy /*= false*/, GLuint _fmt, int _elementSize )
{
  setAttribVec(inputIDPos_, _num, _data, _stride, _internalCopy, _fmt, _elementSize);
}

void MeshCompiler::setNormals( int _num, const void* _data, int _stride, bool _internalCopy /*= false*/, GLuint _fmt, int _elementSize )
{
  setAttribVec(inputIDNorm_, _num, _data, _stride, _internalCopy, _fmt, _elementSize);
}

void MeshCompiler::setTexCoords( int _num, const void* _data, int _stride, bool _internalCopy /*= false*/, GLuint _fmt, int _elementSize )
{
  setAttribVec(inputIDTexC_, _num, _data, _stride, _internalCopy, _fmt, _elementSize);
}


void MeshCompiler::setAttribVec(int _attrIdx, int _num, const void* _data, int _stride, bool _internalCopy /*= false*/, GLuint _fmt, int _elementSize)
{
  // sets vertex data for each attribute individually
  // Example:
  // Format: float3 pos, float2 uv, float3 normal has 3 attribute vectors, that store all positions, texcoords etc. of the mesh
  // - positions: float3[n1]  n1: number of positions
  // - texcoords: float2[n2]  n2: number of texcoords
  // - normals: float3[n3]    n3: number of normals
  //  the vector sizes n1, n2, n3 do not have to be equal!

  if (_attrIdx < 0)
    return;

  assert(_attrIdx < (int)decl_.getNumElements());

  VertexElementInput* inbuf = input_ + _attrIdx;

  inbuf->count = _num;

  // size in bytes of one vertex element (eg. 12 for float3 element)
  int size = inbuf->attrSize = (int)VertexDeclaration::getElementSize(decl_.getElement(_attrIdx));


  // make an internal copy of the input if the user can not guarantee, that the data address is permanently valid
  if (_internalCopy)
  {
    delete [] inbuf->internalBuf;
    inbuf->internalBuf = new char[size * _num];
    inbuf->data = inbuf->internalBuf;

    inbuf->stride = size;

    if (_data)
    {
      // copy elementwise because of striding
      for (int i = 0; i < _num; ++i)
      {
        memcpy(inbuf->internalBuf + (size_t)(size * i),
          (const char*)_data + (size_t)(_stride * i),
          size);
      }
    }
  }
  else
  {
    // store data address only without making a copy
    inbuf->data = (const char*)_data;
    inbuf->stride = _stride ? _stride : size;

    delete [] inbuf->internalBuf;
    inbuf->internalBuf = 0;
  }


  inbuf->fmt = _fmt;
  inbuf->elementSize = _elementSize;
}

void MeshCompiler::WeldList::add(const int _face, const int _corner)
{
  const int stride = meshComp->getVertexDeclaration()->getVertexStride();

  // pointer address to vertex data
  char* vtx0 = workBuf + stride * list.size();
  char* vtx1 = workBuf;

  // query vertex data
  meshComp->getInputFaceVertexData(_face, _corner, vtx0);

  bool matched = false;

  // search for same vertex that is referenced already
  for (size_t i = 0; i < list.size() && !matched; ++i)
  {
    WeldListEntry* it = &list[i];
    // query referenced vertex data

    // compare vertices
    if (cmpFunc->equalVertex(vtx0, vtx1, meshComp->getVertexDeclaration()))
    {
      // found duplicate vertex
      // -> remap index data s.t. only one these two vertices is referenced

      WeldListEntry e;
      e.faceId = _face; e.cornerId = _corner;
      e.refFaceId = it->refFaceId; e.refCornerId = it->refCornerId;

      list.push_back(e);

      matched = true;
    }

    vtx1 += stride;
  }

  // unreferenced vertex
  if (!matched)
  {
    WeldListEntry e;
    e.faceId = _face; e.cornerId = _corner;
    e.refFaceId = _face; e.refCornerId = _corner;

    list.push_back(e);
  }

}

void MeshCompiler::weldVertices()
{
  const int numVerts = input_[inputIDPos_].count;

  // clear weld map
  vertexWeldMapFace_.resize(numIndices_, -1);
  vertexWeldMapCorner_.resize(numIndices_, 0xff);

  // alloc buffer to query vertex data
  int maxAdjCount = 0;
  for (int i = 0; i < numVerts; ++i)
  {
    const int n = getAdjVertexFaceCount(i);
    maxAdjCount = std::max(n, maxAdjCount);
  }

  // OPTIMIZATION: Now the vertex compare buffer works as a vertex cache, storing interpreted vertex data.
  //  Thus, each vertex has to be interpreted only once ( when it gets inserted into the welding list )
  char* vtxCompBuf = new char[decl_.getVertexStride() * (maxAdjCount + 1)];

  WeldList weldList;
  weldList.meshComp = this;
  weldList.cmpFunc = vertexCompare_;
  weldList.workBuf = vtxCompBuf;

  weldList.list.reserve(maxAdjCount);

  for (int i = 0; i < numVerts; ++i)
  {
    // OPTIMIZATION: Moved constructor/destructor of WeldList out of for-loop
    // Create welding list for each vertex.
    weldList.list.clear();

    // Search for candidates in adjacent faces
    for (int k = 0; k < getAdjVertexFaceCount(i); ++k)
    {
      const int adjFace = getAdjVertexFace(i, k);

      // find corner id of adj face
      int adjCornerId = -1;
      for (int m = 0; m < getFaceSize(adjFace); ++m)
      {
        const int adjVertex = getInputIndex(adjFace, m, inputIDPos_);
        
        if (adjVertex == i)
        {
          adjCornerId = m;
          break;
        }
      }

      assert(adjCornerId != -1);


      // check for existing entry
      const int weldMapOffset = getInputFaceOffset(adjFace) + adjCornerId;
      
      if (vertexWeldMapFace_[weldMapOffset] >= 0)
        continue; // skip

      weldList.add(adjFace, adjCornerId);
    }


    // apply local WeldList of a vertex to global weld map

    for (size_t e = 0; e < weldList.list.size(); ++e)
    {
      const WeldListEntry* it = &weldList.list[e];
      const int weldMapOffset = getInputFaceOffset(it->faceId) + it->cornerId;

      if (vertexWeldMapFace_[weldMapOffset] >= 0)
        continue; // skip

      // store in map
      vertexWeldMapFace_[weldMapOffset] = it->refFaceId;
      vertexWeldMapCorner_[weldMapOffset] = it->refCornerId;
    }
  }


  // -------------------------------------------------------------
  // Alternative method that avoids iterating over adjacency list at cost of higher memory load
  // Could not measure any noticeable difference in run-time performance.
// 
//   std::vector< std::vector< std::pair<int,int> > > VertexColMap;
//   VertexColMap.resize(numVerts);
// 
//   for (int i = 0; i < numVerts; ++i)
//     VertexColMap[i].reserve( getAdjVertexFaceCount(i) );
// 
//   for (int i = 0; i < numFaces_; ++i)
//   {
//     for (int k = 0; k < getFaceSize(i); ++k)
//     {
//       int v = faceInput_->getSingleFaceAttr(i, k, inputIDPos_);
// 
//       VertexColMap[v].push_back( std::pair<int,int>(i,k) );
//     }
//   }
// 
//   for (int i = 0; i < numVerts; ++i)
//   {
//     // Create welding list for each vertex.
//     WeldList weldList;
//     weldList.meshComp = this;
//     weldList.cmpFunc = vertexCompare_;
//     weldList.workBuf = vtxCompBuf;
// 
//     for (int k = 0; k < VertexColMap[i].size(); ++k)
//       weldList.add(VertexColMap[i][k].first, VertexColMap[i][k].second);
// 
//     // apply local WeldList of a vertex to global weld map
// 
// //     for (std::list< WeldListEntry >::iterator it = weldList.list.begin();
// //       it != weldList.list.end();  ++ it)
//     for (size_t e = 0; e < weldList.list.size(); ++e)
//     {
//       const WeldListEntry* it = &weldList.list[e];
//       const int weldMapOffset = getInputFaceOffset(it->faceId) + it->cornerId;
// 
// //      if (vertexWeldMap_[weldMapOffset].first >= 0)
//       if (vertexWeldMapFace_[weldMapOffset] >= 0)
//         continue; // skip
// 
//       // store in map
// //      vertexWeldMap_[weldMapOffset] = std::pair<int, int> ( it->refFaceId, it->refCornerId );
//       vertexWeldMapFace_[weldMapOffset] = it->refFaceId;
//       vertexWeldMapCorner_[weldMapOffset] = it->refCornerId;
//     }
//   }
//   // -------------------------------------------------------------


  // fix incomplete welding map (isolated vertices)
  fixWeldMap();

  delete [] vtxCompBuf;
}

void MeshCompiler::fixWeldMap()
{
  for (int i = 0; i < numFaces_; ++i)
  {
    const int fsize = getFaceSize(i);
    for (int k = 0; k < fsize; ++k)
    {
      const int weldMapOffset = getInputFaceOffset(i) + k;

      if (vertexWeldMapFace_[weldMapOffset] < 0)
      {
        vertexWeldMapFace_[weldMapOffset] = i;
        vertexWeldMapCorner_[weldMapOffset] = k;
      }
    }
  }
}

void MeshCompiler::findIsolatedVertices()
{
  const int nVerts = input_[inputIDPos_].count;

  numIsolatedVerts_ = 0;
  // For each vertex check if there exists a reference in the splitting list. We have found an isolated vertex if there is no reference.
  //  Checking the vertex-face adjacency count is also possible to detect isolated vertices.

  for (int i = 0; i < nVerts; ++i)
  {
    if (splitter_->isIsolated(i))
      ++numIsolatedVerts_;
  }

  isolatedVertices_.clear();
  isolatedVertices_.reserve(numIsolatedVerts_);
  for (int i = 0; i < nVerts; ++i)
  {
    if (splitter_->isIsolated(i))
      isolatedVertices_.push_back(i);
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

  const int numPositions = input_[inputIDPos_].count;

  // estimate number of splits to avoid resizing too often
  int estimatedSplitCount = 0;
  int numDifferentInputCounts = 0;

  // simple heuristic:
  // if the number of elements of an arbitrary attribute is larger than the number of positions, add the difference to the number of splits
  //  the actual number of splits may be larger, in which case the array has to be resized.
  for (int i = 0; i < numAttributes_; ++i)
  {
    if (i != inputIDPos_)
    {
      if (input_[i].count > numPositions)
      {
        const int diff = input_[i].count - numPositions;

        if (diff > 0)
        {
          estimatedSplitCount = std::max(diff, estimatedSplitCount);
          ++numDifferentInputCounts;
        }
      }
    }
  }

  if (numDifferentInputCounts > 1)
  {
    // estimation probably too small, increase by 20 %
    estimatedSplitCount = int(float(estimatedSplitCount) * 1.2f);
  }

  assert(estimatedSplitCount >= 0);

  // worst case: each vertex can be used by only one face
  //  clamp estimation-count accordingly

  int maxSplitCount = 0;

  if (numIndices_ > 0)
  {
    if (numIndices_ > numPositions)
      maxSplitCount = numIndices_ - numPositions;
  }
  else
  {
    // numIndices_ is unknown
    int sumFaceSize = 0;

    if (constantFaceSize_)
      sumFaceSize = numFaces_ * maxFaceSize_;
    else
    {
      for (int i = 0; i < numFaces_; ++i)
        sumFaceSize += getFaceSize(i);
    }

    if (sumFaceSize > numPositions)
      maxSplitCount = sumFaceSize - numPositions;
  }

  estimatedSplitCount = std::min(estimatedSplitCount, maxSplitCount);

//  std::cout << "estimated split count: " << estimatedSplitCount << std::endl;

  // split vertices such that each index combination of a face corner (i_pos, i_uv, i_attr..) has a unique vertex id and the number of vertices is minimal
  delete splitter_;
  splitter_ = new VertexSplitter(decl_.getNumElements(),
                                 numPositions,
                                 numPositions + estimatedSplitCount,
                                 0.0f);

  faceBufSplit_.resize(numIndices_, -1);

  // count # vertices after splitting
  numDrawVerts_ = 0;

  for (int i = 0; i < numFaces_; ++i)
  {
    const int fsize = getFaceSize(i);
    for (int k = 0; k < fsize; ++k)
    {
      // indices of the face vertex into the attribute vectors
      int vertex[16];  // {i_pos, i_attr1, i_attr2 ..}


      // get indices of a face corner after welding
      getInputFaceVertex_Welded(i, k, vertex);

      // split vertices by index data only
      // value of position, normal etc. are not considered
      const int idx = splitter_->split(vertex);

      // handle index storage
      setInputIndexSplit(i, k, idx);
    }
  }


//  std::cout << "actual split count: " << (numDrawVerts_ - numPositions) << std::endl;



  // Fix splitting list if there are isolated vertices in between.
  // Isolated vertices currently occupy spots in in the interleaved vertex buffer.
  // -> Remove them from the vbo.
  findIsolatedVertices();

  if (numIsolatedVerts_ > 0)
  {
    // create table that stores how many isolated vertices have been encountered up to each vertex
    std::vector<int> IsoFix(numPositions, 0);

    int fixIndex = 0;
    for (int i = 0; i < numPositions; ++i)
    {
      if (splitter_->isIsolated(i))
        fixIndex--;

      IsoFix[i] = fixIndex;
    }

    numDrawVerts_ = 0;

    // apply index fixing table
    for (int i = 0; i < numFaces_; ++i)
    {
      const int fsize = getFaceSize(i);
      for (int k = 0; k < fsize; ++k)
      {
        // get vertex position id
        int vertex[16];
        getInputFaceVertex_Welded(i, k, vertex);

        // fix interleaved index
        int idx = getInputIndexSplit(i, k);
        idx += IsoFix[vertex[0]];
        setInputIndexSplit(i, k, idx);
      }
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
  sharedVertices.resize(maxFaceSize_);

  std::vector<int> tmpFaceVerts; // used for face rotation-swap
  tmpFaceVerts.resize(maxFaceSize_);

  faceRotCount_.resize(numFaces_, 0);

  int numInitialVerts = numDrawVerts_;
  char* VertexUsed = new char[numDrawVerts_];
  memset(VertexUsed, 0, sizeof(char) * numDrawVerts_);

  for (int faceID = 0; faceID < numFaces_; ++faceID)
  {
    const int numCorners = getFaceSize(faceID);
    
    // reset shared list
    memset(&sharedVertices[0], 0, sizeof(int) * maxFaceSize_);
    int numShared = 0;

    for (int v0 = 0; v0 < numCorners && numShared < numCorners; ++v0)
    {
      if (sharedVertices[v0])
        continue;

      const int vertexID0 = getInputIndexSplit(faceID, v0);

      if (vertexID0 >= numInitialVerts || VertexUsed[vertexID0])
      {
        sharedVertices[v0] = true;
        ++numShared;
      }
    }


    if (numShared == numCorners)
    {
      // worst-case: all vertices shared with neighbors

      // add split vertex to end of vertex buffer, which is used exclusively by the current face
      // current vertex count is stored in numDrawVerts_

      setInputIndexSplit(faceID, 0, numDrawVerts_);
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
        {
          if (tmpFaceVerts[rotCount] < numInitialVerts)
            VertexUsed[tmpFaceVerts[rotCount]] = 1;
          break;
        }
      }

      assert(rotCount < numCorners);

      // rotate:  i -> i+rotCount
      rotCount = numCorners - rotCount;

      faceRotCount_[faceID] = rotCount;
   
      for (int i = 0; i < numCorners; ++i)
        setInputIndexSplit(faceID, (i + rotCount)%numCorners, tmpFaceVerts[i]);
    }
    else
    {
      // best-case: unshared vertex at corner 0
      const int idx = getInputIndexSplit(faceID, 0);
      if (idx < numInitialVerts)
        VertexUsed[idx] = 1;
    }

  }

  delete [] VertexUsed;

}

void MeshCompiler::getInputFaceVertex( const int _face, const int _corner, int* _out ) const
{
  for (unsigned int k = 0; k < decl_.getNumElements(); ++k)
    _out[k] = getInputIndex(_face, _corner, k);
}

void MeshCompiler::getInputFaceVertex_Welded( const int i, const int j, int* _out ) const
{
  int face = i;
  int corner = j;

  // apply welding map if available
  if (!vertexWeldMapFace_.empty())
  {
    const int offset = getInputFaceOffset(i);

    face = vertexWeldMapFace_[offset + j];
    corner = vertexWeldMapCorner_[offset + j];
  }

  for (unsigned int k = 0; k < decl_.getNumElements(); ++k)
    _out[k] = getInputIndex(face, corner, k);
}

void MeshCompiler::getInputFaceVertexData( const int _faceId, const int _corner, void* _out ) const
{
  for (int i = 0; i < numAttributes_; ++i)
  {
    const VertexElement* el = decl_.getElement(i);

    const int idx = getInputIndex(_faceId, _corner, i);

    input_[i].getElementData(idx, (char*)_out + (size_t)el->pointer_, el);
  }
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
  curFaceInputPos_ = 0;

  indices_ = 0;

  numDrawVerts_ = 0;
  numIsolatedVerts_ = 0;

  maxFaceSize_ = 0;
  constantFaceSize_ = false;

  provokingVertex_ = -1;
  provokingVertexSetByUser_ = false;

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


  vertexCompare_ = &defaultVertexCompare;

}

MeshCompiler::~MeshCompiler()
{
  if (!triIndexBuffer_.empty() && indices_ != &triIndexBuffer_[0])
    delete [] indices_;


  if (deleteFaceInputeData_)
    delete faceInput_;

  delete splitter_;
}


int MeshCompiler::getInputIndexOffset( const int _face, const int _corner, const bool _rotation ) const
{
  assert(_face >= 0);
  assert(_face < numFaces_);

  const int baseIdx = int(faceStart_.empty() ? maxFaceSize_ * _face : faceStart_[_face]);
  const int fsize = getFaceSize(_face);

  const int rotCount = faceRotCount_.empty() && _rotation ? 0 : faceRotCount_[_face];


  assert(baseIdx >= 0);
  assert(baseIdx <= numIndices_);

  assert(fsize > 0);

  return baseIdx + (_corner + rotCount) % fsize;
}


void MeshCompiler::setInputIndexSplit( const int _face, const int _corner, const int _val )
{
  const int offset = getInputIndexOffset(_face, _corner);

  // keep track of number of vertices after splitting process
  if (_val >= numDrawVerts_)
    numDrawVerts_ = _val + 1;

  faceBufSplit_[offset] = _val;
}

int MeshCompiler::getInputIndexSplit( const int _face, const int _corner ) const
{
  const int offset = getInputIndexOffset(_face, _corner);

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

  const int maxCount = (_numAttribs + 1) * (_numWorstCase + 1);

  // alloc split list and invalidate
  splits.resize(maxCount, -1);

  dbg_numResizes = 0;
  dbg_numSplits = 0;
}



MeshCompiler::VertexSplitter::~VertexSplitter()
{
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
    while (pos >= 0 && bSearchSplit)
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
        
        if (next < 0) break;    // end of list
        if (next == pos) break; // avoid loop

        pos = next;             // go to next entry
      }
    }

    // combination not found -> add new vertex

    int newID = numVerts++;

    setNext(pos, newID);
    setAttribs(newID, vertex);

    pos = newID;

    ++dbg_numSplits;
  }

  return pos;
}



int MeshCompiler::VertexSplitter::getNext(const int id)
{
  assert(id >= 0);

  const int entryIdx = id * (1 + numAttribs);

  // need more space?
  if (entryIdx >= (int)splits.size())
  {
    splits.resize(entryIdx + numAttribs * 100, -1);
    ++dbg_numResizes;
  }

  return splits[entryIdx];
}

void MeshCompiler::VertexSplitter::setNext(const int id, const int next)
{
  assert(id >= 0);
  
  const int entryIdx = id * (1 + numAttribs);

  // need more space?
  if (entryIdx >= (int)splits.size())
  {
    splits.resize(entryIdx + numAttribs * 100, -1);
    ++dbg_numResizes;
  }

  splits[entryIdx] = next;
}

int* MeshCompiler::VertexSplitter::getAttribs(const int id)
{
  assert(id >= 0);

  const int entryIdx = id * (1 + numAttribs) + 1;

  // need more space?
  if (entryIdx + numAttribs >= (int)splits.size())
  {
    splits.resize(entryIdx + numAttribs * 100, -1);
    ++dbg_numResizes;
  }

  return &splits[0] + entryIdx;
}

void MeshCompiler::VertexSplitter::setAttribs( const int id, int* attr )
{
  memcpy(getAttribs(id), attr, numAttribs * sizeof(int));
}

bool MeshCompiler::VertexSplitter::isIsolated( const int vertexPosID )
{
  return getNext(vertexPosID) < 0;
}



MeshCompiler::VertexElementInput::VertexElementInput()
: internalBuf(0), data(0),
  count(0), stride(0), attrSize(0),
  fmt(0), elementSize(-1)
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
    const int faceID = faceSortMap_.empty() ? sortFaceID : faceSortMap_[sortFaceID];

    const int faceSize = getFaceSize(faceID);

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


  // rotate tris such that the unshared face vertex is at the wanted provoking position of each triangle

  if (provokingVertex_ >= 0)
  {
    for (int i  = 0; i < numTris_; ++i)
    {
      for (int k = 0; k < 3 - provokingVertex_; ++k)
      {
        const int tmp =  triIndexBuffer_[i*3];
        triIndexBuffer_[i*3] = triIndexBuffer_[i*3 + 1];
        triIndexBuffer_[i*3 + 1] = triIndexBuffer_[i*3 + 2];
        triIndexBuffer_[i*3 + 2] = tmp;
      }
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
    const int faceID = mapTriToInputFace(i);

    const int faceGroup = getFaceGroup(faceID);
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
    const int texID = getFaceGroup(face);

    if (GroupIDs.find(texID) == GroupIDs.end())
      GroupIDs[texID] = face;
  }

  // alloc subset array
  numSubsets_ = int(GroupIDs.size());
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
        const int texID = getFaceGroup(k);

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
      const unsigned int SrcTri = copt.GetTriangleMap()[k];
      triOptMap_[k + StartTri] = SrcTri + StartTri;
    }

  }

  // call to GPUCacheOptimizer::OptimizeVertices!

  unsigned int* vertexOptMap = new unsigned int[numDrawVerts_];

  GPUCacheOptimizer::OptimizeVertices(numTris_, numDrawVerts_, 4, indices_, vertexOptMap);

  // apply vertexOptMap to index buffer

  for (int i = 0; i < numTris_ * 3; ++i)
    indices_[i] = vertexOptMap[indices_[i]];


  // apply opt-map to current vertex-map

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



void MeshCompiler::build(bool _weldVertices, bool _optimizeVCache, bool _needPerFaceAttribute, bool _keepIsolatedVertices)
{
  // track memory report for profiling/debugging
  const bool dbg_MemProfiling = false;

  // array allocation/copy data/validation checks etc.
  prepareData();

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

  if (_weldVertices)
  {
    if (dbg_MemProfiling)
      std::cout << "computing adjacency.., memusage = " << (getMemoryUsage() /(1024 * 1024)) << std::endl;

    computeAdjacency();

    if (dbg_MemProfiling)
      std::cout << "vertex welding.., memusage = " << (getMemoryUsage() /(1024 * 1024)) << std::endl;

    weldVertices();

    // delete adjacency list (high memory cost)
    adjacencyVert_.clear();
  }
 
  if (dbg_MemProfiling)
    std::cout << "vertex splitting.., memusage = " << (getMemoryUsage() /(1024 * 1024)) << std::endl;

  splitVertices();

  if (dbg_MemProfiling)
    std::cout << "splitting done.., memusage = " << (getMemoryUsage() /(1024 * 1024)) << std::endl;

  // delete splitting and welding map ( high mem cost and is no longer needed after splitting )
  {
    delete splitter_; splitter_ = 0;

    {
      std::vector< int > emptyVec;
      vertexWeldMapFace_.swap(emptyVec);
    } {
      std::vector< unsigned char > emptyVec;
      vertexWeldMapCorner_.swap(emptyVec);
    }
  }

  if (_needPerFaceAttribute)
  {
    if (dbg_MemProfiling)
      std::cout << "force unshared vertex.., memusage = " << (getMemoryUsage() /(1024 * 1024)) << std::endl;

    // The provoking vertex of each face shall not be referenced by any other face.
    // This vertex can then be used to store per-face data
    
    // default provoking position 2
    if (provokingVertex_ < 0)
      provokingVertex_ = 2;
    
    provokingVertex_ = provokingVertex_ % 3;

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
  if (dbg_MemProfiling)
    std::cout << "sorting by mat.., memusage = " << (getMemoryUsage() /(1024 * 1024)) << std::endl;
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
  if (dbg_MemProfiling)
    std::cout << "triangulate.., memusage = " << (getMemoryUsage() /(1024 * 1024)) << std::endl;
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
  {
    if (dbg_MemProfiling)
      std::cout << "optimizing.., memusage = " << (getMemoryUsage() /(1024 * 1024)) << std::endl;

    optimize();
  }
  else if (!triIndexBuffer_.empty())
  {
    indices_ = new int[triIndexBuffer_.size()];
    memcpy(indices_, &triIndexBuffer_[0], triIndexBuffer_.size() * sizeof(int));
  }

  if (dbg_MemProfiling)
    std::cout << "creating maps.., memusage = " << (getMemoryUsage() /(1024 * 1024)) << std::endl;

  // delete temporary tri index buffer (use indices_ instead)
  {
    std::vector< int > emptyVec;
    triIndexBuffer_.swap(emptyVec);
  }

  createVertexMap(_keepIsolatedVertices);
  createFaceMap();

  // delete intermediate mappings
  {
    std::vector< int > emptyVec;
    triToSortFaceMap_.swap(emptyVec);
  } {
    std::vector< int > emptyVec;
    triOptMap_.swap(emptyVec);
  } {
    std::vector< int > emptyVec;
    faceSortMap_.swap(emptyVec);
  }

  if (dbg_MemProfiling)
    std::cout << "finished.., memusage = " << (getMemoryUsage() /(1024 * 1024)) << std::endl;


  // debugging
//   dbgdump("../../../dbg_meshcomp.txt");
//   dbgdumpObj("../../../dbg_meshcomp.obj");
//   dbgVerify("../../../dbg_maps.txt");
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

  memcpy(inbuf->internalBuf + (size_t)(_v * inbuf->stride), _data, inbuf->attrSize);
}

int MeshCompiler::getNumInputAttributes( int _attrIdx ) const
{
  assert (_attrIdx >= 0);
  assert(_attrIdx < int(decl_.getNumElements()));

  return input_[_attrIdx].count;
}


int MeshCompiler::mapTriToInputFace( const int _tri ) const
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

  return (int)faceGroupIDs_[_faceID];
}

int MeshCompiler::findGroupSubset( int _groupID )
{
  return subsetIDMap_[_groupID];
}

const MeshCompiler::Subset* MeshCompiler::getSubset( int _i ) const
{
  return &subsets_[_i];
}

std::string MeshCompiler::vertexToString( const void* v ) const
{
  std::stringstream str;

  for (int i = 0; i < (int)decl_.getNumElements(); ++i)
  {
    const VertexElement* el = decl_.getElement(i);

    str << el->shaderInputName_ << " [";

    const void* data = static_cast<const char*>(v) + (size_t)el->pointer_;

    switch ( el->type_ )
    {
    case GL_DOUBLE:
      {
        const double* d0 = static_cast<const double*>(data);

        for (int k = 0; k < (int)el->numElements_; ++k)
           str << d0[k] << ", ";
      } break;


    case GL_FLOAT:
      {
        const float* f0 = static_cast<const float*>(data);

        for (int k = 0; k < (int)el->numElements_; ++k)
          str << f0[k] << ", ";
      } break;

    case GL_INT:
    case GL_UNSIGNED_INT:
      {
        const int* i0 = static_cast<const int*>(data);

        for (int k = 0; k < (int)el->numElements_; ++k)
          str << i0[k] << ", ";
      } break;

    case GL_SHORT:
    case GL_UNSIGNED_SHORT:
      {
        const short* i0 = static_cast<const short*>(data);

        for (int k = 0; k < (int)el->numElements_; ++k)
          str << i0[k] << ", ";
      } break;

    case GL_BYTE:
    case GL_UNSIGNED_BYTE:
      {
        const char* i0 = static_cast<const char*>(data);

        for (int k = 0; k < (int)el->numElements_; ++k)
          str << ((int)i0[k]) << ", ";
      } break;

    default: std::cerr << "MeshCompiler: vertexToString() unknown type: " << el->type_ << std::endl;
    }


    str << "] ";

  }

  return str.str();
}

bool MeshCompiler::dbgVerify(const char* _filename) const
{
  int numTotalErrors = 0;

  std::ofstream file;

  if (_filename)
    file.open(_filename);

  if (file || !_filename)
  {

    // ---------------------------------------------------------------------------------------

    int numErrors = 0;

    // check draw_tri <-> face
    if (file.is_open())
      file <<  "checking draw_tri <-> face mapping ..\n";

    for (int face = 0; face < getNumFaces(); ++face)
    {
      int numTrisOfFace = 0;
      mapToDrawTriID(face, 0, &numTrisOfFace);

      for (int k = 0; k < numTrisOfFace; ++k)
      {
        int tri = mapToDrawTriID(face, k, 0);

        int dbg_face = mapToOriginalFaceID(tri);
        if (face != dbg_face)
        {
          if (file.is_open())
            file << "error: face " << face << " -> (numTris: " << numTrisOfFace << ",  tri " << tri << " -> face " << dbg_face << ")\n";
          ++numErrors;
        }
      }
    }

    if (file.is_open())
      file << numErrors << " errors found\n\n";
    numTotalErrors += numErrors;

    // ---------------------------------------------------------------------------------------

    // check input (face, corner) -> vertex id
    if (file.is_open())
      file <<  "checking (face, corner) -> vbo by comparing vertex data ..\n";
    numErrors = 0;

    char* vtxCmpData = new char[decl_.getVertexStride() * 2];

    for (int face = 0; face < getNumFaces(); ++face)
    {
      int fsize = getFaceSize(face);

      for (int k = 0; k < fsize; ++k)
      {
        char* v0 = vtxCmpData;
        char* v1 = vtxCmpData + decl_.getVertexStride();

        memset(v0, 0, decl_.getVertexStride());
        memset(v1, 0, decl_.getVertexStride());

        // get input vertex
        getInputFaceVertexData(face, k, v0);

        // get output vertex
        int vertex = mapToDrawVertexID(face, k);
        getVertex(vertex, v1);

        if (!vertexCompare_->equalVertex(v0, v1, &decl_))
        {
          std::string vertexData0 = vertexToString(v0);
          std::string vertexData1 = vertexToString(v1);

          if (file)
            file << "error: (face " << face << ", corner " << k << ") -> vertex " << vertex << ": " << vertexData0.c_str() << " != "<< vertexData1.c_str() << "\n";

          ++numErrors;
        }
      }
    }

    if (file.is_open())
      file << numErrors << " errors found\n\n";
    numTotalErrors += numErrors;

    numErrors = 0;

    // ---------------------------------------------------------------------------------------

    // check vertex id -> input (face, corner)
    if (file.is_open())
      file <<  "checking vbo -> (face, corner) by comparing vertex data ..\n";

    for (int vertex = 0; vertex < getNumVertices(); ++vertex)
    {
      int face = 0, corner = 0;
      int posID = mapToOriginalVertexID(vertex, face, corner);

      char* v0 = vtxCmpData;
      char* v1 = vtxCmpData + decl_.getVertexStride();

      memset(v0, 0, decl_.getVertexStride());
      memset(v1, 0, decl_.getVertexStride());

      // get output vertex
      getVertex(vertex, v0);

      // get input vertex
      if (face >= 0)
        getInputFaceVertexData(face, corner, v1);
      else
      {
        // isolated vertex
        for (unsigned int i = 0; i < decl_.getNumElements(); ++i)
        {
          const VertexElement* el = decl_.getElement(i);
          input_[i].getElementData(posID, (char*)v1 + (size_t)el->pointer_, el);
        }
      }

      if (!vertexCompare_->equalVertex(v0, v1, &decl_))
      {
        std::string vertexData0 = vertexToString(v0);
        std::string vertexData1 = vertexToString(v1);

        if (file)
          file << "error: vertex " << vertex << " -> (face " << face << ", corner " << corner << "): " << vertexData0.c_str() << " != " << vertexData1.c_str() << "\n";
        ++numErrors;
      }
    }

    if (file.is_open())
      file << numErrors << " errors found\n\n";
    numTotalErrors += numErrors;

    numErrors = 0;

    delete [] vtxCmpData;


    // -----------------------------------------------------------

    // check unshared vertex
    if (provokingVertex_ >= 0)
    {
      if (file.is_open())
        file <<  "checking unshared per face vertices ..\n";

      std::vector< std::map<int, int> > VertexRefs;
      VertexRefs.resize(getNumVertices());

      for (int face = 0; face < numFaces_; ++face)
      {
        int nTris = 0;
        mapToDrawTriID(face, 0, &nTris);

        for (int k = 0; k < nTris; ++k)
        {
          int tri = mapToDrawTriID(face, k, 0);

          int faceVertex = getIndex(tri * 3 + provokingVertex_);

          VertexRefs[faceVertex][face] = 1;
        }

      }


      for (int i = 0; i < getNumVertices(); ++i)
      {
        if (VertexRefs[i].size() > 1)
        {
          if (file)
          {
            file << "error : vertex " << i << " is referenced by " << VertexRefs[i].size() << " faces: ";

            for (std::map<int, int>::iterator it = VertexRefs[i].begin(); it != VertexRefs[i].end(); ++it)
              file << it->first << ", ";

            file << "\n";
          }
          ++numErrors;
        }
      }

      if (file.is_open())
        file << numErrors << " errors found\n\n";
      numTotalErrors += numErrors;

      numErrors = 0;


      // -----------------------------------------------------------
      // check face group sorting

      if (file.is_open())
        file << "checking face group sorting ..\n";

      for (int i = 0; i < getNumSubsets(); ++i)
      {
        const ACG::MeshCompiler::Subset* sub = getSubset(i);

        for (int k = 0; k < (int)sub->numTris; ++k)
        {
          int faceID = mapToOriginalFaceID(sub->startIndex/3 + k);
          int grID = getFaceGroup(faceID);

          if (grID != sub->id)
          {
            if (file.is_open())
              file <<  "error: face " << faceID << " with group-id " << grID << " was mapped to subset-group " << sub->id << "\n";

            ++numErrors;
          }

        }
      }

      if (file.is_open())
        file << numErrors << " errors found\n\n";
      numTotalErrors += numErrors;

      numErrors = 0;

    }
    

    if (file.is_open())
      file.close();
  }

  return numTotalErrors == 0;
}

void MeshCompiler::dbgdump(const char* _filename) const
{
  std::ofstream file;
  file.open(_filename);

  if (file.is_open())
  {
    for (int i = 0; i < numAttributes_; ++i)
    {
      const VertexElementInput* inp = input_ + i;
      file << "attribute[" << i << "]: internalbuf " << inp->internalBuf << ", data " << inp->data << ", count " << inp->count << ", stride " << inp->stride << ", attrSize " << inp->attrSize << "\n";
    }

    file << "\n\n";
    file << "faces " << numFaces_ << "\nindices " << numIndices_ << "\n";

    if (!vertexWeldMapFace_.empty())
    {
      for (int i = 0; i < numFaces_; ++i)
      {
        for (int k = 0; k < getFaceSize(i); ++k)
        {
          int face = vertexWeldMapFace_[getInputFaceOffset(i) + k];
          int corner = vertexWeldMapCorner_[getInputFaceOffset(i) + k];
          file << "vertexWeldMap_[" << i <<", "<<k<<"] = ["<<face<<", "<<corner<<"]\n";
        }
      }
    }

    for (size_t i = 0; i < faceBufSplit_.size(); ++i)
      file << "faceBufSplit_["<<i<<"] = "<<faceBufSplit_[i]<<"\n";

    file << "\n\n";

    for (size_t i = 0; i < faceRotCount_.size(); ++i)
      file << "faceRotCount_["<<i<<"] = "<<int(faceRotCount_[i])<<"\n";

    file << "\n\n";

    for (size_t i = 0; i < faceGroupIDs_.size(); ++i)
      file << "faceGroupIDs_["<<i<<"] = "<<faceGroupIDs_[i]<<"\n";

    file << "\n\n";

    for (size_t i = 0; i < faceSortMap_.size(); ++i)
      file << "faceSortMap_["<<i<<"] = "<<faceSortMap_[i]<<"\n";

    file << "\n\n";

    for (size_t i = 0; i < triIndexBuffer_.size()/3; ++i)
      file << "tri["<<i<<"] = "<<triIndexBuffer_[i*3+1]<<" "<<triIndexBuffer_[i*3+1]<<" "<<triIndexBuffer_[i*3+2]<<"\n";

    file << "\n\n";

    for (size_t i = 0; i < subsets_.size(); ++i)
    {
      const Subset* sub = &subsets_[i];
      file <<"subset["<<i<<"]: id "<<sub->id<<", startIndex "<<sub->startIndex<<", numTris "<<sub->numTris<<", numFaces "<<sub->numFaces<<", startFace "<<sub->startFace<<"\n";
    }

    file << "\n\n";

    for (std::map<int, int>::const_iterator it = subsetIDMap_.begin();
      it != subsetIDMap_.end(); ++it)
      file << "subsetIDMap["<<it->first<<"] = "<<it->second<<"\n";

    file << "\n\n";

    for (int i = 0; i < numFaces_; ++i)
    {
      for (int k = 0; k < getFaceSize(i); ++k)
        file <<"mapToDrawVertexID["<<i<<", "<<k<<"] = "<<mapToDrawVertexID(i, k)<<"\n";
    }

    file << "\n\n";

    for (int i = 0; i < getNumVertices(); ++i)
    {
      int f, c;
      mapToOriginalVertexID(i, f, c);
      file <<"mapToOriginalVertexID["<<i<<"] = ["<<f<<", "<<c<<"]\n";
    }

    file << "\n\nadjacencyVert\n";
    adjacencyVert_.dbgdump(file);

    file << "\n\n";

    file << "\n\n";


    for (int i = 0; i < numFaces_; ++i)
    {
        for (int k = 0; k < getFaceSize(i); ++k)
        {
          float vtx[32];
          getInputFaceVertexData(i, k, vtx);

          //                                    pos[0]       pos[1]       pos[2]          uv[0]        uv[1]          n[0]         n[1]         n[2]
          file << "data["<<i<<", "<<k<<"] = ("<<vtx[0]<<" "<<vtx[1]<<" "<<vtx[2]<<") ("<<vtx[3]<<" "<<vtx[4]<<") ("<<vtx[5]<<" "<<vtx[6]<<" "<<vtx[7]<<")\n";
        }
    }

    file.close();
  }
}


void MeshCompiler::AdjacencyList::dbgdump(std::ofstream& file) const
{
  if (file.is_open())
  {
    for (int i = 0; i < num; ++i)
    {
      for (int k = 0; k < getCount(i); ++k)
        file << "adj["<<i<<"]["<<k<<"] = "<<getAdj(i,k)<<"\n";
    }
  }
}

void MeshCompiler::VertexElementInput::getElementData(int _idx, void* _dst, const VertexElement* _desc) const
{
  // attribute data not set by user, skip
  if (count == 0 || !data)
  {
    memset(_dst, 0, attrSize);
    return;
  }

  //   assert(_idx >= 0);
  //   assert(_idx < count);

  if (_idx < 0 || _idx >= count)
  {
    memset(_dst, 0, attrSize);
    return;
  }

  const void* dataAdr = static_cast<const char*>(data) + (size_t)(_idx * stride);

  if (fmt == 0 || elementSize == -1 || fmt == _desc->type_)
    memcpy(_dst, dataAdr, attrSize);
  else
  {
    // typecast data to format in vertex buffer
    int data_i[4] = {0,0,0,0};    // data in integer fmt
    double data_d[4] = {0.0, 0.0, 0.0, 0.0}; // data in floating point

    // read data
    for (int i = 0; i < elementSize; ++i)
    {
      switch (fmt)
      {
      case GL_FLOAT:  data_d[i] = (static_cast<const float*>(dataAdr))[i];  break;
      case GL_DOUBLE: data_d[i] = (static_cast<const double*>(dataAdr))[i];  break;
      
      case GL_UNSIGNED_INT:
      case GL_INT:    data_i[i] = (static_cast<const int*>(dataAdr))[i];  break;
      
      case GL_UNSIGNED_SHORT:
      case GL_SHORT:  data_i[i] = (static_cast<const short*>(dataAdr))[i];  break;

      case GL_UNSIGNED_BYTE:
      case GL_BYTE:  data_i[i] = (static_cast<const short*>(dataAdr))[i];  break;

      default: std::cerr << "MeshCompiler: unknown data format - " << fmt << std::endl; break;
      }
    }


    // zero mem
    memset(_dst, 0, attrSize);

    // typecast data
    if (fmt == GL_FLOAT || fmt == GL_DOUBLE)
    {
      for (int i = 0; i < (int)_desc->numElements_ && i < (int)elementSize; ++i)
      {
        switch (_desc->type_)
        {
        case GL_FLOAT: ((float*)_dst)[i] = (float)data_d[i]; break;
        case GL_DOUBLE: ((double*)_dst)[i] = (double)data_d[i]; break;
        
        case GL_UNSIGNED_INT:
        case GL_INT: ((int*)_dst)[i] = (int)data_d[i]; break;

        case GL_UNSIGNED_BYTE:
        case GL_BYTE: ((char*)_dst)[i] = (char)data_d[i]; break;

        case GL_UNSIGNED_SHORT:
        case GL_SHORT: ((short*)_dst)[i] = (short)data_d[i]; break;

        default: break;
        }
      }
    }
    else
    {
      for (int i = 0; i < (int)_desc->numElements_ && i < (int)elementSize; ++i)
      {
        switch (_desc->type_)
        {
        case GL_FLOAT: ((float*)_dst)[i] = (float)data_i[i]; break;
        case GL_DOUBLE: ((double*)_dst)[i] = (double)data_i[i]; break;

        case GL_UNSIGNED_INT:
        case GL_INT: ((int*)_dst)[i] = (int)data_i[i]; break;

        case GL_UNSIGNED_BYTE:
        case GL_BYTE: ((char*)_dst)[i] = (char)data_i[i]; break;

        case GL_UNSIGNED_SHORT:
        case GL_SHORT: ((short*)_dst)[i] = (short)data_i[i]; break;

        default: break;
        }
      }
    }


  }
}


void MeshCompiler::dbgdumpObj(const char* _filename) const
{
  std::ofstream file;
  file.open(_filename);

  if (file.is_open())
  {
    for (int i = 0; i < numDrawVerts_; ++i)
    {
      float vertex[64];

      getVertex(i, vertex);

      if (inputIDPos_ >= 0)
      {
        float* pos = vertex + (size_t)decl_.getElement(inputIDPos_)->pointer_ / 4;
        file << "v "<<pos[0]<<" "<<pos[1]<<" "<<pos[2]<<"\n";
      }

      if (inputIDNorm_ >= 0)
      {
        float* pos = vertex + (size_t)decl_.getElement(inputIDNorm_)->pointer_ / 4;
        file << "vn "<<pos[0]<<" "<<pos[1]<<" "<<pos[2]<<"\n";
      }

      if (inputIDTexC_ >= 0)
      {
        float* pos = vertex  + (size_t)decl_.getElement(inputIDTexC_)->pointer_ / 4;
        file << "vt "<<pos[0]<<" "<<pos[1]<<"\n";
      }
    }


    for (int i = 0; i < numTris_; ++i)
    {
      if (!inputIDTexC_ && !inputIDNorm_)
        file << "f "<<indices_[i*3]+1<<" "<<indices_[i*3+1]+1<<" "<<indices_[i*3+2]+1<<"\n";

      else if (!inputIDTexC_)
        file << "f "<<indices_[i*3]+1<<"/"<<indices_[i*3]+1<<" "<<indices_[i*3+1]+1<<"/"<<indices_[i*3+1]+1<<" "<<indices_[i*3+2]+1<<"/"<<indices_[i*3+2]+1<<"\n";

      else
        file << "f "<<indices_[i*3]+1<<"/"<<indices_[i*3]+1<<"/"<<indices_[i*3]+1<<" "<<indices_[i*3+1]+1<<"/"<<indices_[i*3+1]+1<<"/"<<indices_[i*3+1]+1<<" "<<indices_[i*3+2]+1<<"/"<<indices_[i*3+2]+1<<"/"<<indices_[i*3+2]+1<<"\n";
    }


    file.close();
  }
}


void MeshCompiler::dbgdumpInputObj( const char* _filename ) const
{
  const int nVerts = (inputIDPos_ >= 0) ? input_[inputIDPos_].count : 0;
//  const int nNormals = (inputIDNorm_ >= 0) ? input_[inputIDNorm_].count : 0;
//  const int nTexC = (inputIDTexC_ >= 0) ? input_[inputIDTexC_].count : 0;

  
  std::ofstream file;
  file.open(_filename);

  if (file.is_open())
  {
    // write vertex data

    for (int i = 0; i < nVerts; ++i)
    {
      float pos[16];
      const VertexElement* el = decl_.getElement(inputIDPos_);
      input_[inputIDPos_].getElementData(i, pos, el);

      file << "v "<<pos[0]<<" "<<pos[1]<<" "<<pos[2]<<"\n";
    }

    if (inputIDTexC_ >= 0)
    {
      float pos[16];
      const VertexElementInput* vei = &input_[inputIDTexC_];
      const VertexElement* el = decl_.getElement(inputIDTexC_);

      for (int i = 0; i < vei->count; ++i)
      {
        vei->getElementData(i, pos, el);
        file << "vt "<<pos[0]<<" "<<pos[1]<<"\n";
      }
    }


    if (inputIDNorm_ >= 0)
    {
      float pos[16];
      const VertexElementInput* vei = &input_[inputIDNorm_];
      const VertexElement* el = decl_.getElement(inputIDNorm_);

      for (int i = 0; i < vei->count; ++i)
      {
        vei->getElementData(i, pos, el);
        file << "vn "<<pos[0]<<" "<<pos[1]<<" "<<pos[2]<<"\n";
      }
    }


    // write face data

    for (int i = 0; i < faceInput_->getNumFaces(); ++i)
    {
      std::string strLine = "f ";

      int size = faceInput_->getFaceSize(i);

      char tmp[0xff];
      for (int k = 0; k < size; ++k)
      {
        if (inputIDNorm_>=0 && inputIDTexC_>=0)
          sprintf(tmp, "%d/%d/%d ", faceInput_->getSingleFaceAttr(i, k, inputIDPos_) + 1,
          faceInput_->getSingleFaceAttr(i, k, inputIDTexC_) + 1,
          faceInput_->getSingleFaceAttr(i, k, inputIDNorm_) + 1);
        else if (inputIDNorm_ >= 0)
          sprintf(tmp, "%d//%d ", faceInput_->getSingleFaceAttr(i, k, inputIDPos_) + 1, faceInput_->getSingleFaceAttr(i, k, inputIDNorm_) + 1);
        else if (inputIDTexC_ >= 0)
          sprintf(tmp, "%d/%d ", faceInput_->getSingleFaceAttr(i, k, inputIDPos_) + 1, faceInput_->getSingleFaceAttr(i, k, inputIDTexC_) + 1);
        else
          sprintf(tmp, "%d ", faceInput_->getSingleFaceAttr(i, k, inputIDPos_) + 1);

        strLine += tmp;
      }

      file << strLine.c_str() << "\n";
    }

    file.close();
  }

}


void MeshCompiler::dbgdumpInputBin( const char* _filename, bool _seperateFiles ) const
{
  const int nVerts = (inputIDPos_ >= 0) ? input_[inputIDPos_].count : 0;
  const int nNormals = (inputIDNorm_ >= 0) ? input_[inputIDNorm_].count : 0;
  const int nTexC = (inputIDTexC_ >= 0) ? input_[inputIDTexC_].count : 0;

  if (!_seperateFiles)
  {
    /*
    simple binary format (can be used for profiling and testing with large meshes):

    int numFaces,
      numVerts,
      numNormals,
      numTexCoords,
      faceBufSize

    -------------------------

    int faceSize[numFaces];
    int faceData[attribute][faceBufSize];

    -------------------------

    float3 verts[numVerts];
    float2 texc[numTexCoords];
    float3 normals[numNormals];

    -------------------------

    adjacency chunks: vertex + face
  
    int num, bufsize;
    int* offset[num];
    uchar* count[num];
    int* buf[bufsize];

    */


    FILE* file = 0;

    file = fopen(_filename, "wb");

    if (file)
    {
      const int nFaces = faceInput_->getNumFaces();
      fwrite(&nFaces, 4, 1, file);

      fwrite(&nVerts, 4, 1, file);
      fwrite(&nNormals, 4, 1, file);
      fwrite(&nTexC, 4, 1, file);

      int faceBufSize = 0;

      for (int i = 0; i < faceInput_->getNumFaces(); ++i)
        faceBufSize += faceInput_->getFaceSize(i);

      fwrite(&faceBufSize, 4, 1, file);

      // write face data

      for (int i = 0; i < faceInput_->getNumFaces(); ++i)
      {
        int fsize = faceInput_->getFaceSize(i);
        fwrite(&fsize, 4, 1, file);
      }

      for (int i = 0; i < faceInput_->getNumFaces(); ++i)
      {
        for (int k = 0; k < faceInput_->getFaceSize(i); ++k)
        {
          int idx = faceInput_->getSingleFaceAttr(i, k, inputIDPos_);
          fwrite(&idx, 4, 1, file);
        }
      }

      if (inputIDTexC_ >= 0)
      {
        for (int i = 0; i < faceInput_->getNumFaces(); ++i)
        {
          for (int k = 0; k < faceInput_->getFaceSize(i); ++k)
          {
            int idx = faceInput_->getSingleFaceAttr(i, k, inputIDTexC_);
            fwrite(&idx, 4, 1, file);
          }
        }
      }

      if (inputIDNorm_ >= 0)
      {
        for (int i = 0; i < faceInput_->getNumFaces(); ++i)
        {
          for (int k = 0; k < faceInput_->getFaceSize(i); ++k)
          {
            int idx = faceInput_->getSingleFaceAttr(i, k, inputIDNorm_);
            fwrite(&idx, 4, 1, file);
          }
        }
      }

      // write vertex data

    
      for (int i = 0; i < nVerts; ++i)
      {
        float pos[16];
        const VertexElement* el = decl_.getElement(inputIDPos_);
        input_[inputIDPos_].getElementData(i, pos, el);

        fwrite(pos, 4, 3, file);
      }

      if (inputIDTexC_ >= 0)
      {
        float pos[16];
        const VertexElementInput* vei = &input_[inputIDTexC_];
        const VertexElement* el = decl_.getElement(inputIDTexC_);

        for (int i = 0; i < vei->count; ++i)
        {
          vei->getElementData(i, pos, el);
          fwrite(pos, 4, 2, file);
        }
      }


      if (inputIDNorm_ >= 0)
      {
        float pos[16];
        const VertexElementInput* vei = &input_[inputIDNorm_];
        const VertexElement* el = decl_.getElement(inputIDNorm_);

        for (int i = 0; i < vei->count; ++i)
        {
          vei->getElementData(i, pos, el);
          fwrite(pos, 4, 3, file);
        }
      }
    

      // write adjacency

      fwrite(&adjacencyVert_.num, 4, 1, file);
      fwrite(&adjacencyVert_.bufSize, 4, 1, file);

      fwrite(adjacencyVert_.start, 4, adjacencyVert_.num, file);
      fwrite(adjacencyVert_.count, 1, adjacencyVert_.num, file);
      fwrite(adjacencyVert_.buf, 4, adjacencyVert_.bufSize, file);

      fclose(file);
    }

  }
  else
  {
    // dump data to separate files
    FILE* file = fopen("../mesh_fsize.bin", "wb");

    for (int i = 0; i < numFaces_; ++i)
    {
      unsigned char fsize = (unsigned char)faceInput_->getFaceSize(i);
      fwrite(&fsize, 1, 1, file);
    }

    fclose(file);

    file = fopen("../mesh_fdata_pos.bin", "wb");

    for (int i = 0; i < numFaces_; ++i)
    {
      for (int k = 0; k < faceInput_->getFaceSize(i); ++k)
      {
        int idx = faceInput_->getSingleFaceAttr(i, k, inputIDPos_);
        fwrite(&idx, 4, 1, file);
      }
    }

    fclose(file);

    if (inputIDTexC_ >= 0)
    {
      file = fopen("../mesh_fdata_texc.bin", "wb");

      for (int i = 0; i < numFaces_; ++i)
      {
        for (int k = 0; k < faceInput_->getFaceSize(i); ++k)
        {
          int idx = faceInput_->getSingleFaceAttr(i, k, inputIDTexC_);
          fwrite(&idx, 4, 1, file);
        }
      }

      fclose(file);
    }

    if (inputIDNorm_ >= 0)
    {
      file = fopen("../mesh_fdata_norm.bin", "wb");

      for (int i = 0; i < numFaces_; ++i)
      {
        for (int k = 0; k < faceInput_->getFaceSize(i); ++k)
        {
          int idx = faceInput_->getSingleFaceAttr(i, k, inputIDNorm_);
          fwrite(&idx, 4, 1, file);
        }
      }

      fclose(file);
    }

    // write vertex data

    file = fopen("../mesh_vdata_pos.bin", "wb");
    for (int i = 0; i < nVerts; ++i)
    {
      float pos[16];
      const VertexElement* el = decl_.getElement(inputIDPos_);
      input_[inputIDPos_].getElementData(i, pos, el);

      fwrite(pos, 4, 3, file);
    }
    fclose(file);

    if (inputIDTexC_ >= 0)
    {
      file = fopen("../mesh_vdata_texc.bin", "wb");

      float pos[16];
      const VertexElementInput* vei = &input_[inputIDTexC_];
      const VertexElement* el = decl_.getElement(inputIDTexC_);

      for (int i = 0; i < vei->count; ++i)
      {
        vei->getElementData(i, pos, el);
        fwrite(pos, 4, 2, file);
      }

      fclose(file);
    }


    if (inputIDNorm_ >= 0)
    {
      file = fopen("../mesh_vdata_norm.bin", "wb");

      float pos[16];
      const VertexElementInput* vei = &input_[inputIDNorm_];
      const VertexElement* el = decl_.getElement(inputIDNorm_);

      for (int i = 0; i < vei->count; ++i)
      {
        vei->getElementData(i, pos, el);
        fwrite(pos, 4, 3, file);
      }

      fclose(file);
    }
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

void MeshCompiler::getVertex( int _id, void* _out ) const
{
  int faceID, cornerID;
  int posID = mapToOriginalVertexID(_id, faceID, cornerID);

  if (faceID >= 0)
  {
    // read connnected vertex
    for (int i = 0; i < numAttributes_; ++i)
    {
      const VertexElement* el = decl_.getElement(i);

      int idx = getInputIndex(faceID, cornerID, i);

      input_[i].getElementData(idx, (char*)_out + (size_t)el->pointer_, el);
    }
  }
  else
  {
    // isolated vertex

    for (int i = 0; i < numAttributes_; ++i)
    {
      const VertexElement* el = decl_.getElement(i);
      input_[i].getElementData(posID, (char*)_out + (size_t)el->pointer_, el);
    }
  }
}

int MeshCompiler::getIndex( int _i ) const
{
  assert(_i >= 0);
  assert(_i < numTris_ * 3);
  return indices_[_i];
}

void MeshCompiler::createVertexMap(bool _keepIsolatedVerts)
{
  // the current vertex buffer does not contain any isolated vertices
  // -> add them to end of buffer if required
  const int offsetIso = numDrawVerts_;
  if (_keepIsolatedVerts)
    numDrawVerts_ += numIsolatedVerts_; // keep
  else
    numIsolatedVerts_ = 0; // delete isolated vertices

  vertexMapFace_.resize(numDrawVerts_, -1);
  vertexMapCorner_.resize(numDrawVerts_, 0xff);

  for (int i = 0; i < numFaces_; ++i)
  {
    for (int k = 0; k < getFaceSize(i); ++k)
    {
      int vertexID = getInputIndexSplit(i, k);
      vertexMapFace_[vertexID] = i;
      vertexMapCorner_[vertexID] = (unsigned char)k;
    }
  }

  if (_keepIsolatedVerts)
  {
    // add isolated verts to end of map
    for (int i = 0; i < numIsolatedVerts_; ++i)
    {
      assert(vertexMapFace_[offsetIso + i] < 0);
      vertexMapFace_[offsetIso + i] = isolatedVertices_[i];
    }
  }

  // validate
#ifdef _DEBUG
  for (int i = 0; i < numDrawVerts_; ++i)
  {
    if (i < offsetIso)
    {
      if (vertexMapFace_[i] < 0 ||
        vertexMapCorner_[i] == 0xff)
        std::cerr << "mesh-assembler: vertexMap error at index: " << i << std::endl;
    }
    else if (vertexMapFace_[i] < 0)
      std::cerr << "mesh-assembler: vertexMap error at index: " << i << std::endl;
  }
#endif // _DEBUG

}


void MeshCompiler::createFaceMap()
{
  // -------------------------------
  // create tri -> face map

  triToFaceMap_.resize(numTris_, -1);
  for (int i = 0; i < numTris_; ++i)
  {
    int faceID = i;
    if (!triOptMap_.empty())
      faceID = triOptMap_[faceID];

    if (!triToSortFaceMap_.empty())
      faceID = triToSortFaceMap_[faceID];

    if (!faceSortMap_.empty())
      faceID = faceSortMap_[faceID];

    triToFaceMap_[i] = faceID;
  }


  // -------------------------------
  // create face -> tri map

  // create offset table
  if (!constantFaceSize_)
  {
    faceToTriMapOffset_.resize(numTris_, -1);

    int offset = 0;
    for (int i = 0; i < numFaces_; ++i)
    {
      faceToTriMapOffset_[i] = offset;

      // # tris of face
      int fsize = getFaceSize(i);
      offset += fsize - 2;
    }
  }
  else
    faceToTriMapOffset_.clear();

  // create face -> tri map
  faceToTriMap_.resize(numTris_, -1); 

  for (int i = 0; i < numTris_; ++i)
  {
    int faceID = mapToOriginalFaceID(i);

    // offset into lookup table
    int offset = constantFaceSize_ ? (faceID * (maxFaceSize_ - 2)) : faceToTriMapOffset_[faceID];

    int triNo = 0;

    // search for free entry
    while (faceToTriMap_[offset + triNo] != -1 && triNo + offset < numTris_)
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
    for (int i = 0; i < input_[inputIDPos_].count; ++i)
    {
      // sorting the adjacency list for easy comparison of adjacency input
      int count = getAdjVertexFaceCount(i);

      std::vector<int> sortedList(count);
      for (int k = 0; k < count; ++k)
        sortedList[k] = getAdjVertexFace(i, k);

      std::sort(sortedList.begin(), sortedList.end());

      for (int k = 0; k < count; ++k)
        fprintf(file, "adj[%d][%d] = %d\n", i, k, sortedList[k]);
    }


    fclose(file);
  }
}

void MeshCompiler::setFaceGroup( int _i, short _groupID )
{
  if ((int)faceGroupIDs_.size() <= std::max(numFaces_,_i))
    faceGroupIDs_.resize(std::max(numFaces_,_i+1), -1);

  faceGroupIDs_[_i] = _groupID;
}

void MeshCompiler::getVertexBuffer( void* _dst, const int _offset /*= 0*/, const int _range /*= -1*/ )
{
  int batchSize = _range;

  // clamp batch size
  if ((_range < 0) || (_offset + batchSize > numDrawVerts_))
    batchSize = numDrawVerts_ - _offset;

  char* bdst = (char*)_dst;

  for (int i = 0; i < batchSize; ++i)
  {
    getVertex(i + _offset, bdst + decl_.getVertexStride() * i);
  }
}

struct MeshCompiler_EdgeTriMapKey
{
  int e0, e1;

  MeshCompiler_EdgeTriMapKey(int a, int b)
    : e0(std::min(a,b)), e1(std::max(a,b)) {}

  bool operator ==(const MeshCompiler_EdgeTriMapKey& k) const
  {
    return e0 == k.e0 && e1 == k.e1;
  }

  bool operator <(const MeshCompiler_EdgeTriMapKey& k) const
  {
    if (e0 < k.e0)
      return true;

    if (e0 > k.e0)
      return false;

    return e1 < k.e1;
  }
};


#ifdef ACG_MC_USE_STL_HASH
// requires c++0x
struct MeshCompiler_EdgeTriMapKey_Hash
{
  std::size_t operator()(const MeshCompiler_EdgeTriMapKey& k) const
  {
    return ((std::hash<int>()(k.e0) << 1) ^ std::hash<int>()(k.e1)) >> 1;
  }
};
#else
uint qHash(const MeshCompiler_EdgeTriMapKey& k)
{
  return ((::qHash(k.e0) << 1) ^ ::qHash(k.e1)) >> 1;
}
#endif // ACG_MC_USE_STL_HASH


void MeshCompiler::getIndexAdjBuffer(void* _dst, const int _borderIndex /* = -1 */)
{
  int* idst = (int*)_dst;

  for (int i = 0; i < numTris_; ++i)
  {
    // initialize all triangles
    idst[i*6] = getIndex(i*3);
    idst[i*6+2] = getIndex(i*3+1);
    idst[i*6+4] = getIndex(i*3+2);
    
    idst[i*6+1] = _borderIndex;
    idst[i*6+3] = _borderIndex;
    idst[i*6+5] = _borderIndex;
  }

  // map from edge -> adjacent tris
  QHash<MeshCompiler_EdgeTriMapKey, std::pair<int, int> > EdgeAdjMap;

  for (int i = 0; i < numTris_; ++i)
  {
    // get input positions of triangle
    int PosIdx[3];

    for (int k = 0; k < 3; ++k)
    {
      int faceId, cornerId;
      PosIdx[k] = mapToOriginalVertexID(getIndex(i*3+k), faceId, cornerId);
    }

    // build edge->triangle adjacency map
    for (int e = 0; e < 3; ++e)
    {
      MeshCompiler_EdgeTriMapKey edge( PosIdx[e], PosIdx[(e+1)%3] );

      QHash< MeshCompiler_EdgeTriMapKey, std::pair<int, int> >::iterator itKey;

      itKey = EdgeAdjMap.find(edge);

      if (itKey == EdgeAdjMap.end())
        EdgeAdjMap[edge] = std::pair<int, int>(i, -1);
      else
        itKey.value().second = i;
    }

  }

  // use edge->triangle adjacency map to build index buffer with adjacency
  for (QHash< MeshCompiler_EdgeTriMapKey, std::pair<int, int> >::iterator it = EdgeAdjMap.begin(); it != EdgeAdjMap.end(); ++it)
  {
    // two adjacent tris of current edge
    int Tris[2] = {it.value().first, it.value().second};

    // get input positions of triangles
    int PosIdx[6];

    for (int k = 0; k < 3; ++k)
    {
      int faceId, cornerId;

      PosIdx[k] = mapToOriginalVertexID(getIndex(Tris[0]*3+k), faceId, cornerId);

      if (Tris[1] >= 0)
        PosIdx[3+k] = mapToOriginalVertexID(getIndex(Tris[1]*3+k), faceId, cornerId);
    }


    // find edge and opposite corner wrt. to adjacent tris

    int TriEdges[2] = {-1,-1};
    int TriOppositeVerts[2] = {-1,-1};

    for (int e = 0; e < 3; ++e)
    {
      MeshCompiler_EdgeTriMapKey edge0( PosIdx[e], PosIdx[(e+1)%3] );

      if (edge0 == it.key())
      {
        TriEdges[0] = e;
        TriOppositeVerts[1] = (e+2)%3;
      }
    }

    if (Tris[1] >= 0)
    {
      for (int e = 0; e < 3; ++e)
      {
        MeshCompiler_EdgeTriMapKey edge1( PosIdx[3 + e], PosIdx[3 + ((e+1)%3)] );

        if (edge1 == it.key())
        {
          TriEdges[1] = e;
          TriOppositeVerts[0] = (e+2)%3;
        }
      }
    }

    // store adjacency in buffer

    for (int i = 0; i < 2; ++i)
    {
      if (Tris[i] >= 0)
      {
        int e = TriEdges[i];

        // opposite vertex
        int ov = _borderIndex;

        if (TriOppositeVerts[i] >= 0 && Tris[(i+1)%2] >= 0)
          ov = getIndex(Tris[(i+1)%2]*3 + TriOppositeVerts[i]);

        idst[Tris[i] * 6 + e*2 + 1] = ov;
      }
    }

  }

  return;

  /*

  // alternative algorithm without hashing with multi threading
  // in case hashing method is too slow, the following unfinished implementation can be completed

  // requires vertex-face adjacency

  computeAdjacency();

#ifdef  USE_OPENMP
#pragma omp parallel for
#endif
  for (int i = 0; i < numFaces_; ++i)
  {
    const int fsize = getFaceSize(i);

    // for each edge of current face
    for (int e = 0; e < fsize; ++e)
    {
      int vertexId = getInputIndex(i, e, inputIDPos_);

      MeshCompiler_EdgeTriMapKey edge( getInputIndex(i, e, inputIDPos_), 
        getInputIndex(i, (e+1)%fsize, inputIDPos_) );


      int numAdjFaces = adjacencyVert_.getCount(vertexId);

      // search for adjacent tri with shared edge
      for (int a = 0; a < numAdjFaces; ++a)
      {
        int adjFaceID = adjacencyVert_.getAdj(vertexId, a);

        const int adjFaceSize = getFaceSize(adjFaceID);

        // for each edge of adjacent face
        for (int ae = 0; ae < adjFaceSize; ++ae)
        {
          // get start/end indices of adjacent half-edge
          MeshCompiler_EdgeTriMapKey adjEdge( getInputIndex(adjFaceID, ae, inputIDPos_), 
            getInputIndex(adjFaceID, (ae+1)%adjFaceSize, inputIDPos_) );

          // check whether this is the same edge
          if (adjEdge == edge)
          {
            // find these half-edges in output mesh




          }
        }
      }

    }


    // add adjacency for newly inserted edges (triangulation of n-polys)

  }

  // delete adjacency list since its no longer needed
  adjacencyVert_.clear();

*/
}

void MeshCompiler::getIndexAdjBuffer_MT(void* _dst, const int _borderIndex /* = -1 */)
{
  // alternative algorithm without hashing, but with multi-threading support

  // compute vertex-position -> triangle adjacency
  //  vertex-position: unsplit position IDs of input mesh
  //  triangles:  triangles in output mesh after build()

  const int numVerts = input_[inputIDPos_].count;
  AdjacencyList outputVertexTriAdj;

  outputVertexTriAdj.init(numVerts);

  // count # adjacent tris per vertex
  for (int i = 0; i < numTris_; ++i)
  {
    int faceID, cornerID;

    for (int k = 0; k < 3; ++k)
    {
      int vertex = getIndex(i * 3 + k);
      int posID = mapToOriginalVertexID(vertex, faceID, cornerID);

      outputVertexTriAdj.count[posID]++;
    }
  }

  // count num of needed entries
  int nCounter = 0;

  for (int i = 0; i < numVerts; ++i)
  {
    outputVertexTriAdj.start[i] = nCounter; // save start indices

    nCounter += outputVertexTriAdj.count[i];

    outputVertexTriAdj.count[i] = 0; // count gets recomputed in next step
  }

  // alloc memory
  outputVertexTriAdj.buf = new int[nCounter];
  outputVertexTriAdj.bufSize = nCounter;

  // build adjacency list
  for (int i = 0; i < numTris_; ++i)
  {
    int faceID, cornerID;

    for (int k = 0; k < 3; ++k)
    {
      int vertex = getIndex(i * 3 + k);
      int posID = mapToOriginalVertexID(vertex, faceID, cornerID);

      int adjIdx = outputVertexTriAdj.start[posID] + outputVertexTriAdj.count[posID]++;

      outputVertexTriAdj.buf[ adjIdx ] = i;
    }
  }


  // assemble index buffer

  int* idst = (int*)_dst;

  for (int i = 0; i < numTris_; ++i)
  {
    // initialize all triangles
    idst[i*6] = getIndex(i*3);
    idst[i*6+2] = getIndex(i*3+1);
    idst[i*6+4] = getIndex(i*3+2);

    idst[i*6+1] = _borderIndex;
    idst[i*6+3] = _borderIndex;
    idst[i*6+5] = _borderIndex;
  }

#ifdef USE_OPENMP
#pragma omp parallel for
#endif
  for (int vertexID = 0; vertexID < numVerts; ++vertexID)
  {
    const int numAdjTris = outputVertexTriAdj.getCount(vertexID);


    for (int adjID0 = 0; adjID0 < numAdjTris; ++adjID0)
    {
      const int triID0 = outputVertexTriAdj.getAdj(vertexID, adjID0);

      int TriPos0[3];
      for (int k = 0; k < 3; ++k)
      {
        int faceid, cornerid;
        TriPos0[k] = mapToOriginalVertexID( getIndex(triID0*3+k), faceid, cornerid );
      }

      for (int adjID1 = adjID0+1; adjID1 < numAdjTris; ++adjID1) //for (int triID1 = 0; triID1 < numTris_; ++triID1)
      {
        const int triID1 = outputVertexTriAdj.getAdj(vertexID, adjID1);

        int TriPos1[3];
        for (int k = 0; k < 3; ++k)
        {
          int faceid, cornerid;
          TriPos1[k] = mapToOriginalVertexID( getIndex(triID1*3+k), faceid, cornerid );
        }

        // find shared edge
        for (int e0 = 0; e0 < 3; ++e0)
        {
          MeshCompiler_EdgeTriMapKey edge0(TriPos0[e0], TriPos0[(e0+1)%3]);

          for (int e1 = 0; e1 < 3; ++e1)
          {
            MeshCompiler_EdgeTriMapKey edge1(TriPos1[e1], TriPos1[(e1+1)%3]);

            if (edge0 == edge1)
            {
              // found shared edge

              int oppVertex0 = getIndex( triID1*3 + ((e1+2)%3) );
              int oppVertex1 = getIndex( triID0*3 + ((e0+2)%3) );

              idst[triID0*6 + e0*2 +1] = oppVertex0;
              idst[triID1*6 + e1*2 + 1] = oppVertex1;
            }
          }

        }

      }

    }

  }
}

void MeshCompiler::getIndexAdjBuffer_BruteForce( void* _dst, const int _borderIndex /*= -1*/ )
{

  int* idst = (int*)_dst;

  for (int i = 0; i < numTris_; ++i)
  {
    // initialize all triangles
    idst[i*6] = getIndex(i*3);
    idst[i*6+2] = getIndex(i*3+1);
    idst[i*6+4] = getIndex(i*3+2);

    idst[i*6+1] = _borderIndex;
    idst[i*6+3] = _borderIndex;
    idst[i*6+5] = _borderIndex;
  }

  // build brute-force adjacency list and compare -- O(n^2)
#ifdef USE_OPENMP
#pragma omp parallel for
#endif
  for (int triID0 = 0; triID0 < numTris_; ++triID0)
  {
    int TriPos0[3];
    for (int k = 0; k < 3; ++k)
    {
      int faceid, cornerid;
      TriPos0[k] = mapToOriginalVertexID( getIndex(triID0*3+k), faceid, cornerid );
    }

    for (int triID1 = triID0 + 1; triID1 < numTris_; ++triID1)
    {
      int TriPos1[3];
      for (int k = 0; k < 3; ++k)
      {
        int faceid, cornerid;
        TriPos1[k] = mapToOriginalVertexID( getIndex(triID1*3+k), faceid, cornerid );
      }

      // find shared edge
      for (int e0 = 0; e0 < 3; ++e0)
      {
        MeshCompiler_EdgeTriMapKey edge0(TriPos0[e0], TriPos0[(e0+1)%3]);

        for (int e1 = 0; e1 < 3; ++e1)
        {
          MeshCompiler_EdgeTriMapKey edge1(TriPos1[e1], TriPos1[(e1+1)%3]);

          if (edge0 == edge1)
          {
            // found shared edge

            int oppVertex0 = getIndex( triID1*3 + ((e1+2)%3) );
            int oppVertex1 = getIndex( triID0*3 + ((e0+2)%3) );

            idst[triID0*6 + e0*2 +1] = oppVertex0;
            idst[triID1*6 + e1*2 + 1] = oppVertex1;
          }
        }

      }

    }

  }

}


int MeshCompiler::mapToOriginalFaceID( const int _i ) const
{
//   int faceID = _i;
//   if (!triOptMap_.empty())
//     faceID = triOptMap_[faceID];
// 
//   if (!triToSortFaceMap_.empty())
//     faceID = triToSortFaceMap_[faceID];
// 
//   if (!faceSortMap_.empty())
//     faceID = faceSortMap_[faceID];
// 
//   return faceID;

  return triToFaceMap_[_i];
}

const int* MeshCompiler::mapToOriginalFaceIDPtr() const
{
  if (triToFaceMap_.empty())
    return 0;
  else 
    return &triToFaceMap_[0];
}


int MeshCompiler::mapToOriginalVertexID( const int _i, int& _faceID, int& _cornerID ) const
{
  int positionID = -1;

  if (_i < numDrawVerts_ - numIsolatedVerts_)
  {
    // connected vertex
    _faceID = vertexMapFace_[_i];

    // revert face rotation here to get the correct input vertex
    int frot = faceRotCount_.empty() ? 0 : faceRotCount_[_faceID];

    _cornerID = vertexMapCorner_[_i] - frot;
    if (_cornerID < 0) _cornerID += getFaceSize(_faceID);

    positionID = getInputIndex(_faceID, _cornerID, inputIDPos_);
  }
  else
  {
    // isolated vertex: vertexMap stores input position id instead of face
    positionID = vertexMapFace_[_i];

    _faceID = -1;
    _cornerID = -1;
  }

  return positionID;
}

int MeshCompiler::mapToDrawVertexID( const int _faceID, const int _cornerID ) const
{
  // apply face rotation
  const int fsize = getFaceSize(_faceID);
  const int frot = faceRotCount_.empty() ? 0 : faceRotCount_[_faceID];

  int rotCorner = _cornerID + frot;
  if (rotCorner >= fsize) rotCorner -= fsize;

  return getInputIndexSplit(_faceID, rotCorner);
}

int MeshCompiler::mapToDrawTriID( const int _faceID, const int _k /*= 0*/, int* _numTrisOut /*= 0*/ ) const
{
  const int fsize = getFaceSize(_faceID);
  assert(_k < fsize - 2);

  if (_numTrisOut)
    *_numTrisOut = fsize - 2;

  int offset = constantFaceSize_ ? (_faceID * (maxFaceSize_ - 2)) : faceToTriMapOffset_[_faceID];

  return faceToTriMap_[offset + _k];
}

size_t MeshCompiler::getMemoryUsage(bool _printConsole) const
{
  /*
  Short evaluation of memory footprint (version at 27.12.13)

  Memory consumption was measured with xyzrgb_dragon.ply from the Stanford repository (7.2 mil tris, 3.6 mil verts).
   Options used for build(): welding - true, optimize - true, require per face attributes - true,
   Adjacency information was generated by MeshCompiler.
   Input data by user: 275 mb (indices, float3 positions, float3 normals)

  Breakdown of memory footprint in build():
  1. 467 mb - adjacency lists ( vertex-face list 98 mb, face-face list 369 mb)
  2. 103 mb - vertex weld map
  3.  82 mb - each of: faceBufSplit_, indices_
  4.  51 mb - vertexMap
  5.  41 mb - splitter_
  6.  27 mb - each of: faceToTriMap_, triToFaceMap_, faceSortMap_, ... ie. any per face map
  7.  13 mb - faceGroupIds
  8.   6 mb - faceRotCount

  Peak memory load: 563 mb after computing face-face adjacency for forceUnsharedVertex() (adjacency lists and faceBufSplit_ were allocated at that time)
  Final memory load: 292 mb measured after build(), mem allocations: [faceGroupIds, faceBufSplit, vertexMap, faceToTriMap, triToFaceMap, indices]

  Update: (12.01.14)
  forceUnsharedVertex() has been modified and no longer requires a face-face adjacency list, which was the biggest memory buffer allocated by build()
  */


  size_t usage = faceStart_.size() * 4;
  usage += faceSize_.size() * 1;
  usage += faceData_.size() * 4;
  usage += faceGroupIDs_.size() * 2;
  usage += faceBufSplit_.size() * 4;
  usage += faceRotCount_.size() * 1;
  usage += faceSortMap_.size() * 4;

  usage += triIndexBuffer_.size() * 4;
  
  usage += subsetIDMap_.size() * sizeof(Subset);

  usage += vertexWeldMapFace_.size() * 5;

  usage += adjacencyVert_.bufSize * 4; // buf
  usage += adjacencyVert_.num * 4; // start
  usage += adjacencyVert_.num * 1; // count


  if (splitter_)
    usage += splitter_->splits.size() * 4;

  usage += triToSortFaceMap_.size() * 4;
  usage += triOptMap_.size() * 4;
  usage += vertexMapFace_.size() * 5;
  usage += faceToTriMap_.size() * 4;
  usage += faceToTriMapOffset_.size() * 4;
  usage += triToFaceMap_.size() * 4;

  usage += numTris_ * 3 * 4; // indices_



  if (_printConsole)
  {
    const int byteToMB = 1024 * 1024;

    std::cout << "faceStart_: " << sizeof(std::pair<int, unsigned char>) << std::endl;

    std::cout << "faceStart_: " << faceStart_.size() * 4 / byteToMB << std::endl;
    std::cout << "faceSize_: " << faceSize_.size() * 1 / byteToMB << std::endl;
    std::cout << "faceData_: " << faceData_.size() * 4 / byteToMB << std::endl;
    std::cout << "faceGroupIDs_: " << faceGroupIDs_.size() * 2 / byteToMB << std::endl;
    std::cout << "faceBufSplit_: " << faceBufSplit_.size() * 4 / byteToMB << std::endl;
    std::cout << "faceRotCount_: " << faceRotCount_.size() * 1 / byteToMB << std::endl;

    std::cout << "faceSortMap_: " << faceSortMap_.size() * 4 / byteToMB << std::endl;

    std::cout << "triIndexBuffer_: " << triIndexBuffer_.size() * 4 / byteToMB << std::endl;

    std::cout << "vertexWeldMap_: " << vertexWeldMapFace_.size() * 5 / byteToMB << std::endl;

    std::cout << "adjacencyVert_: buf = " << adjacencyVert_.bufSize * 4 / byteToMB <<
      ", offset = " << adjacencyVert_.num * 4 / byteToMB <<
      ", count = "  << adjacencyVert_.num * 1 / byteToMB << std::endl;

    if (splitter_)
      std::cout << "splitter_: " << splitter_->splits.size() * 4 / byteToMB << std::endl;

    std::cout << "triToSortFaceMap_: " << triToSortFaceMap_.size() *  4 / byteToMB << std::endl;
    std::cout << "triOptMap_: " << triOptMap_.size() *  4 / byteToMB << std::endl;
    std::cout << "vertexMap_: " << vertexMapFace_.size() * 5 / byteToMB << std::endl;
    std::cout << "faceToTriMapOffset_: " << faceToTriMapOffset_.size() * 4 / byteToMB << std::endl;
    std::cout << "faceToTriMap_: " << faceToTriMap_.size() * 4 / byteToMB << std::endl;
    std::cout << "triToFaceMap_: " << triToFaceMap_.size() * 4 / byteToMB << std::endl;


    std::cout << "indices_: " << 3 * numTris_ * 4 / byteToMB << std::endl;

  }


  return usage;
}

std::string MeshCompiler::checkInputData() const
{
  if (!faceInput_)
    return "Error: no face input data present\n";

  std::stringstream strm;

  int faceV[16];

  for (int a = 0; a < numAttributes_; ++a)
  {
    if (input_[a].count <= 0)
      strm << "Warning: input buffer is not initialized: buffer id " << a << "\n";
  }

  for (int i = 0; i < faceInput_->getNumFaces(); ++i)
  {
    if (faceInput_->getFaceSize(i) > 0xff)
      strm << "Error: face size too big: face " << i << ", size " << faceInput_->getFaceSize(i) << " must not exceed 255\n";

    std::map<int, int> facePositions;

    for (int k = 0; k < faceInput_->getFaceSize(i); ++k)
    {
      getInputFaceVertex(i, k, faceV);

      for (int a = 0; a < numAttributes_; ++a)
      {
        if (input_[a].count > 0)
        {
          // index boundary check
          if (faceV[a] >= input_[a].count)
            strm << "Error: input index (face/corner/attribute: " << i << "/" << k << "/" << a << ") invalid: " << faceV[a] << " >= buffer size (" << input_[a].count << ")\n";
          if (faceV[a] < 0)
            strm << "Error: input index (face/corner/attribute: " << i << "/" << k << "/" << a << ") invalid: " << faceV[a] << "\n";
        }
      }

      if (numAttributes_)
        facePositions[faceV[0]] = k;
    }

    // degenerate check
    if ((int)facePositions.size() != faceInput_->getFaceSize(i))
      strm << "Warning: degenerate face " << i << "\n";

    // empty face
    if (!faceInput_->getFaceSize(i))
      strm << "Warning: empty face " << i << "\n";
  }

  return strm.str();
}

void MeshCompiler::prepareData()
{
  // update face count, in case not provided by user
  numFaces_ = faceInput_->getNumFaces();
  numIndices_ = faceInput_->getNumIndices();

  // clip empty faces from the end (user estimated too many faces)
  for (int i = numFaces_-1; i >= 0 && !faceInput_->getFaceSize(i); --i)
    --numFaces_;

  // update size of each attribute (necessary for correct handling of missing input buffers)
  for (unsigned int i = 0; i < decl_.getNumElements(); ++i)
  {
    int attrSize = (int)VertexDeclaration::getElementSize(decl_.getElement(i));
    assert(input_[i].attrSize <= 0 || input_[i].attrSize == attrSize);
    input_[i].attrSize = attrSize;
  }

  // build internal face-offset table for storing interleaved index buffer

  int minFaceSize = 99999999;
  for (int i = 0; i < numFaces_; ++i)
  {
    const int fsize = faceInput_->getFaceSize(i);

    maxFaceSize_ = std::max((int)maxFaceSize_, fsize);
    minFaceSize  = std::min(minFaceSize, fsize);
  }
  
  // faces with less than 3 vertices (lines or points) shouldn't be included in the first place
  // these parts should be handled separately, as they don't need all the costly preprocessing of MeshCompiler
  if (minFaceSize < 3)
    std::cout << "error: input contains faces with less than 3 vertices! MeshCompiler only works for pure surface meshes!" << std::endl;

  assert(minFaceSize >= 3);

  if (minFaceSize < (int)maxFaceSize_)
  {
    int curOffset = 0;

    // internal face-offset buffer required for mesh processing
    faceStart_.resize(numFaces_, -1);
    faceSize_.resize(numFaces_, 0);

    for (int i = 0; i < numFaces_; ++i)
    {
      faceSize_[i] = (unsigned char)faceInput_->getFaceSize(i);
      faceStart_[i] = curOffset;
      curOffset += faceSize_[i];
    }

    if (numIndices_ <= 0)
      numIndices_ = curOffset;
  }
  else
  {
    constantFaceSize_ = true;
    numIndices_ = maxFaceSize_ * numFaces_;
  }


  // reset mappings from previous builds()
  triOptMap_.clear();
  triToSortFaceMap_.clear();
  faceSortMap_.clear();

  faceToTriMap_.clear();
  faceToTriMapOffset_.clear();

  triToFaceMap_.clear();
  vertexMapFace_.clear();
  vertexMapCorner_.clear();

  faceBufSplit_.clear();
  faceRotCount_.clear();

  isolatedVertices_.clear();
  numIsolatedVerts_ = 0;

  if (!provokingVertexSetByUser_)
    provokingVertex_ = -1;


#ifdef _DEBUG
  for (int i = 0; i < numFaces_; ++i)
  {
    for (int k = 0; k < getFaceSize(i); ++k)
    {
      int v0 = faceInput_->getSingleFaceAttr(i, k, inputIDPos_);
      int v1 = getInputIndex(i, k, inputIDPos_);

      assert(v0 == v1);
    }
  }
#endif // _DEBUG
}

void MeshCompiler::setFaceInput( MeshCompilerFaceInput* _faceInput )
{
  faceInput_ = _faceInput;
}

void MeshCompiler::setIndexBufferInterleaved( int _numTris, int _indexSize, const void* _indices )
{
  assert(_indices);
  assert(_indexSize);

  setNumFaces(_numTris, _numTris * 3);

  for (int i = 0; i < _numTris; ++i)
  {
    int tri[3] = {-1, -1, -1};

    for (int k = 0; k < 3; ++k)
    {
      int offs = i * 3 + k;

      switch(_indexSize)
      {
      case 1: tri[k] = ((char*)_indices)[offs]; break;
      case 2: tri[k] = ((short*)_indices)[offs]; break;
      case 4: tri[k] = ((int*)_indices)[offs]; break;
      case 8: tri[k] = (int)((long long*)_indices)[offs]; break;
      default: break;
      }
    }

    setFaceVerts(i, 3, tri);
  }
}

int MeshCompiler::getNumFaces() const
{
  return numFaces_;
}

void MeshCompiler::setProvokingVertex( int _v )
{
  provokingVertex_ = _v;
  provokingVertexSetByUser_ = true;
}

int MeshCompiler::getAdjVertexFaceCount( int _vertexID ) const
{
  return adjacencyVert_.num ? adjacencyVert_.getCount(_vertexID) : faceInput_->getVertexAdjCount(_vertexID);
}

int MeshCompiler::getAdjVertexFace( int _vertexID, int _k ) const
{
  return adjacencyVert_.num ? adjacencyVert_.getAdj(_vertexID, _k) : faceInput_->getVertexAdjFace(_vertexID, _k);
}

bool MeshCompiler::isTriangleMesh() const
{
  return maxFaceSize_ == 3;
}

bool MeshCompiler::isFaceEdge( const int _triID, const int _edge ) const
{
  assert(_edge >= 0);
  assert(_edge < 3);

  if (maxFaceSize_ <= 3) return true;

  // brute force: search for triangle edge in input face

  const int faceID = mapToOriginalFaceID(_triID);
  const int fsize = getFaceSize(faceID);

  // get all draw vertices of face
  std::vector<int> FaceVerts(fsize);

  for (int i = 0; i < fsize; ++i)
    FaceVerts[i] = mapToDrawVertexID(faceID, i);

  int edgeStart = -1;
  int edgeEnd = -1;

  switch (_edge)
  {
  case 0: edgeStart = 0; edgeEnd = 1; break;
  case 1: edgeStart = 1; edgeEnd = 2; break;
  case 2: edgeStart = 2; edgeEnd = 0; break;
  default: break;
  }

  // access index buffer of triangle
  edgeStart = getIndex(_triID * 3 + edgeStart);
  edgeEnd = getIndex(_triID * 3 + edgeEnd);

  // search for edge in face vertices
  for (int e = 0; e < fsize; ++e)
  {
    if (FaceVerts[e] == edgeStart && FaceVerts[(e+1)%fsize] == edgeEnd)
      return true;
    if (FaceVerts[e] == edgeEnd && FaceVerts[(e+1)%fsize] == edgeStart)
      return true;
  }

  return false;
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

bool MeshCompilerDefaultFaceInput::getFaceAttr( int _faceID, int _attrID, int* _out ) const
{
  int offset = faceOffset_[_faceID];
  int fsize = faceSize_[_faceID];

  for (int i = 0; i < fsize; ++i)
    _out[i] = faceData_[_attrID][offset+i];

  return true;
}

int MeshCompilerDefaultFaceInput::getSingleFaceAttr( const int _faceID, const int _faceCorner, const int _attrID ) const
{
  // Offset + _faceCorner
  const int pos = faceOffset_[_faceID] + _faceCorner;
  assert(_faceCorner < getFaceSize(_faceID));


  if (faceData_[_attrID].empty())
    return -1;

  return faceData_[_attrID][pos];
}

void MeshCompilerDefaultFaceInput::setFaceData( int _faceID, int _size, int* _data, int _attrID /*= 0*/ )
{
  // update face count
  if (numFaces_ <= _faceID)
    numFaces_ = _faceID + 1;

  // reserve mem
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

int MeshCompilerFaceInput::getSingleFaceAttr( const int _faceID, const int _faceCorner, const int _attrID )  const
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


bool MeshCompilerVertexCompare::equalVertex( const void* v0, const void* v1, const VertexDeclaration* _decl )
{
  assert(_decl);
  assert(v0);
  assert(v1);

  const double d_eps = 1e-4;
  const float f_eps = 1e-4f;

  const int nElements = (int)_decl->getNumElements();
  for (int i = 0; i < nElements; ++i)
  {
    const VertexElement* el = _decl->getElement(i);


    const void* el_0 = static_cast<const char*>(v0) + (size_t)el->pointer_;
    const void* el_1 = static_cast<const char*>(v1) + (size_t)el->pointer_;

    switch ( el->type_ )
    {
    case GL_DOUBLE:
      {
        const double* d0 = static_cast<const double*>(el_0);
        const double* d1 = static_cast<const double*>(el_1);

        double diff = 0.0;

        for (int k = 0; k < (int)el->numElements_; ++k)
          diff += fabs(d0[k] - d1[k]);

        if (diff > d_eps)
          return false;

      } break;


    case GL_FLOAT:
      {
        const float* f0 = static_cast<const float*>(el_0);
        const float* f1 = static_cast<const float*>(el_1);

        float diff = 0.0;

        for (int k = 0; k < (int)el->numElements_; ++k)
        {
          // sign-bit manipulation vs. fabsf  - no significant performance difference
//           float e = f0[k] - f1[k];
//           int e_abs = *((int*)(&e)) & 0x7FFFFFFF;
//           diff += *((float*)&e_abs);

          diff += fabsf(f0[k] - f1[k]);
        }

        if (diff > f_eps)
          return false;
      } break;

    case GL_INT:
    case GL_UNSIGNED_INT:
      {
        const int* i0 = static_cast<const int*>(el_0);
        const int* i1 = static_cast<const int*>(el_1);

        for (int k = 0; k < (int)el->numElements_; ++k)
        {
          if (i0[k] != i1[k])
            return false;
        }
      } break;

    case GL_SHORT:
    case GL_UNSIGNED_SHORT:
      {
        const short* i0 = static_cast<const short*>(el_0);
        const short* i1 = static_cast<const short*>(el_1);

        for (int k = 0; k < (int)el->numElements_; ++k)
        {
          if (i0[k] != i1[k])
            return false;
        }
      } break;

    case GL_BYTE:
    case GL_UNSIGNED_BYTE:
      {
        const char* i0 = static_cast<const char*>(el_0);
        const char* i1 = static_cast<const char*>(el_1);

        for (int k = 0; k < (int)el->numElements_; ++k)
        {
          if (i0[k] != i1[k])
            return false;
        }
      } break;

    default: std::cerr << "MeshCompiler: equalVertex() comparision not implemented for type: " << el->type_ << std::endl;
    }
  }

  return true;
}




}
