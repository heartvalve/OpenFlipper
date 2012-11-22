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

#define ACG_DRAW_MESH_C

//=============================================================================

#include "DrawMesh.hh"
#include <ACG/Geometry/GPUCacheOptimizer.hh>
#include <ACG/GL/VertexDeclaration.hh>
#include <assert.h>
#include <math.h>
#include <vector>
#include <map>
#include <string.h>
#include <fstream>


//=============================================================================

#define DRAW_MESH_VERTEX_CMP_EPSILON 1e-4f

// print a memory usage report each draw call
//#define DEBUG_MEM_USAGE


namespace ACG
{

template <class Mesh>
DrawMeshT<Mesh>::Vertex::Vertex()
{
  for (int i = 0; i < 3; ++i)
  {
    pos[i] = 0.0f;
    n[i] = 0.0f;
//    tan[i] = 0.0f;

    if (i < 2)
      tex[i] = 0.0f;
  }

//  tan[3] = 0.0f;

  fcol = vcol = 0xFFFFFFFF;
}

template <class Mesh>
int DrawMeshT<Mesh>::Vertex::equals(const Vertex& r)
{
  float f = 0.0f; // difference
  for (int i = 0; i < 3; ++i)
  {
    f += fabsf(r.pos[i] - pos[i]);
    f += fabsf(r.n[i] - n[i]);
//    f += fabsf(r.tan[i] - tan[i]);

    if (i < 2)
      f += fabsf(r.tex[i] - tex[i]);
  }

//  f += fabsf(r.tan[3] - tan[3]);

  return (vcol == r.vcol && fcol == r.fcol) && (f < DRAW_MESH_VERTEX_CMP_EPSILON);
}

inline int intNextPowerOfTwo(int x)
{
  x--;
  x = (x >> 1) | x;
  x = (x >> 2) | x;
  x = (x >> 4) | x;
  x = (x >> 8) | x;
  x = (x >> 16) | x;
  x++;
  return x;
}

template <class Mesh>
DrawMeshT<Mesh>::DrawMeshT(Mesh& _mesh)
:  mesh_(_mesh),
   numTris_(0), numVerts_(0),
   indices_(0),
   vertices_(0),
   rebuild_(REBUILD_NONE),
   prevNumFaces_(0), prevNumVerts_(0),
   numSubsets_(0),
   subsets_(0),
   vbo_(0), ibo_(0),
   lineIBO_(0),
   indexType_(0),
   colorMode_(1),
   flatMode_(0), bVBOinFlatMode_(0),
   textureMode_(1), bVBOinHalfedgeTexMode_(1),
   halfedgeNormalMode_(0), bVBOinHalfedgeNormalMode_(0),
   triToFaceMap_(0),
   vertexMap_(0),
   invVertexMap_(0),
   indicesTmp_(0),
   verticesTmp_(0),
   textureIndexPropertyName_("Not Set"),
   perFaceTextureCoordinatePropertyName_("Not Set"),
   updatePerEdgeBuffers_(1),
  updatePerHalfedgeBuffers_(1)
{
  vertexDeclVCol_ = new VertexDeclaration;
  vertexDeclFCol_ = new VertexDeclaration;

  vertexDeclEdgeCol_ = new VertexDeclaration;
  vertexDeclHalfedgeCol_ = new VertexDeclaration;
  vertexDeclHalfedgePos_ = new VertexDeclaration;

  createVertexDeclarations();
}


template <class Mesh>
void
DrawMeshT<Mesh>::rebuild()
{
  if (rebuild_ == REBUILD_NONE) return;

  if (!mesh_.n_vertices()) return;

  // support for point clouds:
  if (mesh_.n_vertices() && mesh_.n_faces() == 0)
  {
    if (mesh_.n_vertices() > numVerts_)
    {
      delete [] vertexMap_; vertexMap_ = 0;
      delete [] invVertexMap_; invVertexMap_ = 0;
      delete [] vertices_; vertices_ = 0;
      delete [] verticesTmp_; verticesTmp_ = 0;

      numVerts_ = mesh_.n_vertices();
      vertices_ = new Vertex[numVerts_];
      verticesTmp_ = new Vertex[numVerts_];
    }

    numVerts_ = mesh_.n_vertices();

    // read all vertices
    for (unsigned int i = 0; i < numVerts_; ++i)
      readVertex(vertices_ + i,
                 mesh_.vertex_handle(i), 
                 (typename Mesh::HalfedgeHandle)(-1), 
                 (typename Mesh::FaceHandle)(-1));

    createVBO();

    return;
  }



  unsigned int maxFaceVertCount = 0;
  unsigned int newTriCount = countTris(&maxFaceVertCount);
  
  int bTriangleRebuild = 0; // what should be rebuild?
  int bVertexRebuild = 0;

  if (newTriCount > numTris_)
  {
    // index buffers too small
    // resize here
    delete [] indices_;
    delete [] indicesTmp_;
    delete [] triToFaceMap_;

    if (ibo_) glDeleteBuffers(1, &ibo_);
    ibo_ = 0;

    numTris_ = newTriCount;

    triToFaceMap_ = new unsigned int[numTris_];
    indices_ = new unsigned int[numTris_ * 3];
    indicesTmp_ = new unsigned int[numTris_ * 3];

    bTriangleRebuild = 1;
  }

  if (prevNumFaces_ != mesh_.n_faces())
  {
    bTriangleRebuild = 1;
    prevNumFaces_ = mesh_.n_faces();
  }

  if (prevNumVerts_ != mesh_.n_vertices())
  {
    if (prevNumVerts_ < mesh_.n_vertices())
    {
      // resize inverse vertex map
      delete [] invVertexMap_;
      invVertexMap_ = new unsigned int[mesh_.n_vertices()];
    }

    bVertexRebuild = 1;
    bTriangleRebuild = 1; // this may have caused changes in the topology!
    prevNumVerts_ = mesh_.n_vertices();
  }

  // support faster update by only updating vertices
  if (!bTriangleRebuild && !bVertexRebuild && rebuild_ == REBUILD_GEOMETRY)
  {
    // only update vertices, i.e. update values of vertices
    
    for (unsigned int i = 0; i < numVerts_; ++i)
    {
      unsigned int MeshHalfedge = vertexMap_[i];

//      Mesh::VertexHandle vh = mesh_.vertex_handle(MeshVertex);

      // just pick one face, srews up face colors here so color updates need a full rebuild
      typename Mesh::HalfedgeHandle hh = mesh_.halfedge_handle(MeshHalfedge);
      typename Mesh::VertexHandle vh = mesh_.to_vertex_handle(hh);
      typename Mesh::FaceHandle fh = mesh_.face_handle(hh);

      readVertex(vertices_ + i, vh, hh, fh);
    }

    createVBO();

    rebuild_ = REBUILD_NONE;
    return;
  }


  std::vector<unsigned int> tris(numTris_ * 3);


  //////////////////////////////////////////////////////////////////////////
  // convert from half_edge data structure to triangle index list


  numTris_ = convertToTriangleMesh(&tris[0], maxFaceVertCount);

  //////////////////////////////////////////////////////////////////////////
  // process vertices
  //  1. one vertex per triangle corner
  //  2. weld equal vertices

  unsigned int newNumVerts = 0;
  std::vector<Vertex> tmpVertexBuffer(3 * numTris_); // pre vertex welding
  std::vector<Vertex> dstVertexBuffer(3 * numTris_); // post vertex welding

  unsigned int* pVertexRemap = new unsigned int[3 * numTris_]; // keeps track of vertex origin (per-vertex attributes)

  createBigVertexBuf(&tmpVertexBuffer[0], pVertexRemap, &tris[0]);


  // minimize vertex buffer
  unsigned int* pTmpVertexRemap = new unsigned int[3 * numTris_];

  std::list< std::pair<unsigned int, unsigned int> > duplicatesMap;

  newNumVerts = weldVertices(&dstVertexBuffer[0],
    &tmpVertexBuffer[0], &tris[0], &pTmpVertexRemap[0], pVertexRemap, duplicatesMap);

  std::swap(pTmpVertexRemap, pVertexRemap);
  delete [] pTmpVertexRemap; pTmpVertexRemap = 0;


  // end-of mesh extraction

  if (numVerts_ < newNumVerts)
  {
    // per vertex buffers too small
    delete [] vertices_;
    delete [] verticesTmp_;
    delete [] vertexMap_;
//    delete [] m_pVerticesC;

    numVerts_ = newNumVerts;

    vertices_ = new Vertex[numVerts_];
    verticesTmp_ = new Vertex[numVerts_];
    vertexMap_ = new unsigned int[numVerts_];
  }

 

  //////////////////////////////////////////////////////////////////////////
  // sort triangles by material

  sortTrisByMaterial(indices_, &tris[0]);

  //////////////////////////////////////////////////////////////////////////
  // optimize vcache
  //  by material subset

  // index buffer first

  optimizeTris(indices_, indices_);
  optimizeVerts(vertices_, &dstVertexBuffer[0], indices_, pVertexRemap);


  delete [] pVertexRemap;


  //////////////////////////////////////////////////////////////////////////
  // create the inverse vertex map

  for (unsigned int i = 0; i < numVerts_; ++i)
  {
    OpenMesh::HalfedgeHandle hh = mesh_.halfedge_handle(vertexMap_[i]);
    invVertexMap_[mesh_.to_vertex_handle(hh).idx()] = i;
  }

  std::list< std::pair<unsigned int, unsigned int> >::iterator duplIt;
  for (duplIt = duplicatesMap.begin(); duplIt != duplicatesMap.end(); ++duplIt)
  {
    unsigned int idx = invVertexMap_[duplIt->second]; // final vertex index in DrawMesh

    invVertexMap_[duplIt->first] = idx;
  }

  //////////////////////////////////////////////////////////////////////////
  // copy to GPU

  createVBO();
  createIBO();

  bVBOinHalfedgeNormalMode_ = halfedgeNormalMode_;

  rebuild_ = REBUILD_NONE;

  // picking buffers update
}


template <class Mesh>
unsigned int
DrawMeshT<Mesh>::convertToTriangleMesh(unsigned int* _dstIndexBuf, unsigned int _maxFaceVertexCount)
{
  unsigned int triCounter = 0;
  unsigned int indexCounter = 0;

  unsigned int* pFaceVerts = new unsigned int[_maxFaceVertexCount];

  for (unsigned int i = 0; i < mesh_.n_faces(); ++i)
  {
    typename Mesh::FaceHandle fh =  mesh_.face_handle(i);
    // convert this face to triangles

	  typename Mesh::HalfedgeHandle hh_start = mesh_.halfedge_handle(fh);
 
  	// count vertices
  	unsigned int nPolyVerts = 0;

    // read all vertex indices of this face
    for (typename Mesh::FaceHalfedgeIter hh_it = mesh_.fh_iter(fh); hh_it; ++hh_it )
      pFaceVerts[nPolyVerts++] = mesh_.to_vertex_handle(hh_it).idx();

    //////////////////////////////////////////////////////////////////////////
    // convert to triangles

    triToFaceMap_[triCounter++] = i;
    for (int k = 0; k < 3; ++k) _dstIndexBuf[indexCounter++] = pFaceVerts[k];

    //  trivial fanning

    for (unsigned int k = 3; k < nPolyVerts; ++k)
    {
      triToFaceMap_[triCounter++] = i;

      _dstIndexBuf[indexCounter++] = pFaceVerts[0];
      _dstIndexBuf[indexCounter++] = pFaceVerts[k-1];
      _dstIndexBuf[indexCounter++] = pFaceVerts[k];
    }
  }

  delete [] pFaceVerts;

  return triCounter;
}

template <class Mesh>
void
DrawMeshT<Mesh>::readVertex(Vertex* _pV,
                            typename Mesh::VertexHandle _vh,
                            typename Mesh::HalfedgeHandle _hh,
                            typename Mesh::FaceHandle _fh)
{
  for (int m = 0; m < 3; ++m)
  {
    _pV->pos[m] = mesh_.point(_vh)[m];

    if (halfedgeNormalMode_ == 0 && mesh_.has_vertex_normals())
      _pV->n[m] = mesh_.normal(_vh)[m];
    else if (halfedgeNormalMode_ && 
      mesh_.has_halfedge_normals() && _hh != (typename Mesh::HalfedgeHandle)(-1))
      _pV->n[m] = mesh_.normal(_hh)[m];
    else _pV->n[m] = 0.0f;

    if (m < 2)
    {
      if (mesh_.has_halfedge_texcoords2D())
      {
        if (_hh != (typename Mesh::HalfedgeHandle)(-1))
          _pV->tex[m] = mesh_.texcoord2D(_hh)[m];
      }
      else _pV->tex[m] = 0.0f;
    }
  }
  
  // per face & per vertex color
  unsigned int byteCol[2];
  for (int col = 0; col < 2; ++col)
  {
    typename Mesh::Color vecCol(255, 255, 255, 255);

    if (col == 0 && mesh_.has_vertex_colors()) vecCol = OpenMesh::color_cast<Vec4uc,typename Mesh::Color>(mesh_.color(_vh));
    if ((_fh != (typename Mesh::FaceHandle)(-1)))
    {
      if (col == 1 && mesh_.has_face_colors() && _fh.idx() >= 0) 
        vecCol = OpenMesh::color_cast<Vec4uc,typename Mesh::Color>(mesh_.color(_fh));
    }

    // OpenGL color format: A8B8G8R8
    byteCol[col] = (unsigned char)(vecCol[0]);
    byteCol[col] |= ((unsigned char)(vecCol[1])) << 8;
    byteCol[col] |= ((unsigned char)(vecCol[2])) << 16;
    byteCol[col] |= ((unsigned char)(vecCol[3])) << 24;
    //byteCol[col] |= 0xFF << 24; // if no alpha channel
  }

 _pV->vcol = byteCol[0];
//  _pV->vcol = 0xFFFF0000; // blue,  debug
 _pV->fcol = byteCol[1];
//  pV->fcol = 0xFF00FF00; // green, debug
}

template <class Mesh>
void
DrawMeshT<Mesh>::createBigVertexBuf(Vertex* _dstVertexBuf,
                                    unsigned int* _dstVertexMap,
                                    const unsigned int* _indexBuf)
{
  // fill 3 * NumTris vertex buffer
  for (unsigned int i = 0; i < numTris_; ++i)
  {
    typename Mesh::FaceHandle fh = mesh_.face_handle(triToFaceMap_[i]);

    for (int k = 0; k < 3; ++k)
    {
      typename Mesh::VertexHandle vh = mesh_.vertex_handle(_indexBuf[i * 3 + k]);

      // find corresponding halfedge and get per face attributes
      typename Mesh::HalfedgeHandle hh;

      for (typename Mesh::FaceHalfedgeIter hh_it = mesh_.fh_iter(fh); hh_it; ++hh_it )
      {
        if (mesh_.to_vertex_handle(hh_it).idx() == vh.idx())
        {
          hh = hh_it;
          break;
        }
      }

      _dstVertexMap[i * 3 + k] = hh.idx();


      Vertex* pV = _dstVertexBuf + i * 3 + k;

      readVertex(pV, vh, hh, fh);
    }
  }
}

template <class Mesh>
unsigned int 
DrawMeshT<Mesh>::weldVertices(Vertex* _dstVertexBuf,
                              const Vertex* _srcVertexBuf,
                              unsigned int* _dstIndexBuf, 
                              unsigned int* _dstVertexMap,
                              const unsigned int* _srcVertexMap,
                              std::list< std::pair<unsigned int, unsigned int> >& _duplicatesMap)
{
  unsigned int newCount = 0;
  unsigned int hashSize = intNextPowerOfTwo(3 * numTris_);

  // hash table + linked list for collision check
  std::vector<unsigned int> hashTable(hashSize + 3 * numTris_);
  unsigned int* pNext = &hashTable[hashSize];

  memset(&hashTable[0], 0xFFFFFFFF, hashSize * sizeof(unsigned int));

  for (unsigned int i = 0; i < 3 * numTris_; ++i)
  {
    const Vertex* pV = _srcVertexBuf + i;

    // float3 hash function, hash by position only
    unsigned int* ui = (unsigned int*)pV->pos;
    unsigned int f = (ui[0] - ui[1]*13 + ui[2]*23)&0x7ffffE00; // make unsigned, discard some high precision mantissa bits
    unsigned int hashVal = ((f>>22)^(f>>12)^(f)) & (hashSize - 1); //  modulo hashSize

    // check previous entries

    unsigned int offset = hashTable[hashVal];

    while ((offset != 0xFFFFFFFF) && (!_dstVertexBuf[offset].equals(*pV)))
      offset = pNext[offset];

    if (offset == 0xFFFFFFFF)
    {
      // create new vertex entry here:
      _dstIndexBuf[i] = newCount;

      _dstVertexMap[newCount] = _srcVertexMap[i];

      memcpy(&_dstVertexBuf[newCount], pV, sizeof(Vertex));

      pNext[newCount] = hashTable[hashVal];

      hashTable[hashVal] = newCount++;
    }
    else // duplicate vertex found, use previous index
    {
      _dstIndexBuf[i] = offset;


      // INCOMPLETE INVERSE VERTEX MAP FIX
      // create a multimap to get all pairs of duplicates
      typename Mesh::HalfedgeHandle h1 = mesh_.halfedge_handle(_dstVertexMap[offset]);
      typename Mesh::HalfedgeHandle h2 = mesh_.halfedge_handle(_srcVertexMap[i]);

      unsigned int idx1 = mesh_.to_vertex_handle(h1).idx();
      unsigned int idx2 = mesh_.to_vertex_handle(h2).idx();

      if (idx1 != idx2)
        _duplicatesMap.push_front(std::pair<unsigned int, unsigned int>(idx2, idx1));
    }
  }

  // nearly degenerate triangle fix
  if (newCount < 3)
  {
    // copy input buffer
    newCount = 3 * numTris_;
    memcpy(_dstVertexBuf, _srcVertexBuf, sizeof(Vertex) * newCount);
    for (unsigned int i = 0; i < newCount; ++i)
    {
      _dstIndexBuf[i] = i;
      _dstVertexMap[i] = _srcVertexMap[i];
    }
  }


  // PER-FACE-NORMAL FIX
  // welding is finished here, but we have to ensure that each
  // triangle has at least one unshared vertex
  //  this vertex must to be the last referenced vertex of the triangle
  //  when switching to flat-shading, this vertex holds the face normal

  // count # adjacent tris for each vertex
  unsigned int* pNumAdjTris = new unsigned int[newCount];
  memset(pNumAdjTris, 0, newCount * sizeof(unsigned int));

  unsigned int totalNumAdjTris = 0;

  for (unsigned int i = 0; i < numTris_; ++i)
  {
    for (unsigned int k = 0; k < 3; ++k)
    {
      ++pNumAdjTris[_dstIndexBuf[i * 3 + k]];
      ++totalNumAdjTris;
    }
  }

  // detect tris not owning a distinct vertex

  for (unsigned int t = 0; t < numTris_; ++t)
  {
    unsigned int unsharedVertex = 0xFFFFFFFF; // which vertex of this tri is unshared

    // count # shared vertices
    for (unsigned int i = 0; i < 3; ++i)
    {
      unsigned int V = _dstIndexBuf[t * 3 + i];

      if (pNumAdjTris[V] == 1)
      {
        unsharedVertex = V;
        break;
      }
    }

    if (unsharedVertex == 0xFFFFFFFF)
    {
      // this tri does not own a distinct vertex
      // add a seperate vertex for this triangle

      _dstVertexMap[newCount] = _dstVertexMap[_dstIndexBuf[t*3+2]];

      // optimization: only split when necessary, (no double split if NumAdjTris[V] = 2)
      --pNumAdjTris[_dstIndexBuf[t*3+2]];

      memcpy(&_dstVertexBuf[newCount], &_dstVertexBuf[_dstIndexBuf[t*3+2]], sizeof(Vertex));

      _dstIndexBuf[t*3+2] = newCount++;
    }
    else
    {
      // rotate references until the unshared vertex is the first one
      while (unsharedVertex != _dstIndexBuf[t * 3 + 2])
      {
        unsigned int tri[] = {_dstIndexBuf[t*3], _dstIndexBuf[t*3+1], _dstIndexBuf[t*3+2]};

        _dstIndexBuf[t*3] = tri[1];
        _dstIndexBuf[t*3+1] = tri[2];
        _dstIndexBuf[t*3+2] = tri[0];
      }
    }
  }

  delete [] pNumAdjTris;


  return newCount;
}


template <class Mesh>
int
DrawMeshT<Mesh>::getTextureIDofTri(unsigned int _tri)
{
  OpenMesh::FPropHandleT< int > textureIndexProperty;
  if (mesh_.get_property_handle(textureIndexProperty, textureIndexPropertyName_))
    return mesh_.property(textureIndexProperty, mesh_.face_handle(triToFaceMap_[_tri]));
  
  if (mesh_.has_face_texture_index())
    return mesh_.texture_index(mesh_.face_handle(triToFaceMap_[_tri]));
  
  return 0;
}

template<class Mesh>
void
DrawMeshT<Mesh>::sortTrisByMaterial(unsigned int* _dstIndexBuf, const unsigned int* _srcIndexBuf)
{
  delete [] subsets_;
  subsets_ = 0;
  numSubsets_ = 0;

  unsigned int* pTmpTriToFaceMap = new unsigned int[numTris_];

  std::map<int, unsigned int> MatIDs; // map[matid] = first tri

  for (unsigned int i = 0; i < numTris_; ++i)
  {
    int texID = getTextureIDofTri(i);
    if (MatIDs.find(texID) == MatIDs.end())
      MatIDs[texID] = i;
  }

  numSubsets_ = MatIDs.size();
  subsets_ = new Subset[numSubsets_];

  std::map<int, unsigned int>::const_iterator it = MatIDs.begin();

  unsigned int lastIndexOffset = 0;

  for (unsigned int i = 0; it != MatIDs.end(); ++i, ++it)
  {
    subsets_[i].materialID = it->first;

    // rearrange by subset chunks

    subsets_[i].startIndex = lastIndexOffset;
    subsets_[i].numTris = 0;

    for (unsigned int k = it->second; k < numTris_; ++k)
    {
      if (getTextureIDofTri(k) == subsets_[i].materialID)
      {
        pTmpTriToFaceMap[lastIndexOffset / 3] = triToFaceMap_[k];

        for (int v = 0; v < 3; ++v)
          _dstIndexBuf[lastIndexOffset++] = _srcIndexBuf[k * 3 + v];
      }
    }

    subsets_[i].numTris = (lastIndexOffset - subsets_[i].startIndex) / 3;

  }

  std::swap(triToFaceMap_, pTmpTriToFaceMap);

  delete [] pTmpTriToFaceMap;
}

template<class Mesh>
void
DrawMeshT<Mesh>::optimizeTris(unsigned int* _dstIndexBuf, unsigned int* _srcIndexBuf)
{
  unsigned int* pTmpTriToFaceMap = new unsigned int[numTris_];
  for (unsigned int i = 0; i < numSubsets_; ++i)
  {
    Subset* pSubset = subsets_ + i;

    GPUCacheOptimizerTipsify copt(24, pSubset->numTris, numVerts_, 4, _srcIndexBuf + pSubset->startIndex);
    copt.WriteIndexBuffer(4, _dstIndexBuf + pSubset->startIndex);

    // apply changes to trimap
    const unsigned int StartTri = pSubset->startIndex/3;
    for (unsigned int k = 0; k < pSubset->numTris; ++k)
    {
      unsigned int SrcTri = copt.GetTriangleMap()[k];
      pTmpTriToFaceMap[k + StartTri] = triToFaceMap_[SrcTri + StartTri];
    }
  }
  std::swap(triToFaceMap_, pTmpTriToFaceMap);
  delete [] pTmpTriToFaceMap;
}


template <class Mesh>
void
DrawMeshT<Mesh>::optimizeVerts(Vertex* _dstVertexBuf,
                               const Vertex* _srcVertexBuf,
                               unsigned int* _inOutIndexBuf, 
                               const unsigned int* _srcVertexMap)
{
  // vertices
  std::vector<unsigned int> vertexRemap(numVerts_);
  GPUCacheOptimizer::OptimizeVertices(numTris_, numVerts_, 4, _inOutIndexBuf, &vertexRemap[0]);

  for (unsigned int i = 0; i < numTris_ * 3; ++i) indices_[i] = vertexRemap[indices_[i]];

  // isolated vertices get sorted out here
  // it's crucial to set numVerts_ to the # referenced verts, to avoid problems 
  // with picking functions

  unsigned int numIsolates = 0;

  for (unsigned int i = 0; i < numVerts_; ++i)
  {
    if (vertexRemap[i] < numVerts_)
    {
      memcpy(_dstVertexBuf + vertexRemap[i], _srcVertexBuf + i, sizeof(Vertex));

//      vertexMap_[i] = _srcVertexMap[i];
      vertexMap_[vertexRemap[i]] = _srcVertexMap[i];
    }
    else
      ++numIsolates; // isolated vertex
  }

  numVerts_ -= numIsolates;
}


template <class Mesh>
void
DrawMeshT<Mesh>::createVBO()
{
  // data read from vertices_
  if (!vbo_)
    glGenBuffersARB(1, &vbo_);

  ACG::GLState::bindBufferARB(GL_ARRAY_BUFFER_ARB, vbo_);

  if (!flatMode_ && textureMode_)
  {
    glBufferDataARB(GL_ARRAY_BUFFER_ARB, numVerts_ * sizeof(Vertex), vertices_, GL_STATIC_DRAW_ARB);
    bVBOinFlatMode_ = 0;
  }
  else
  {
    // use per face normals
    memcpy(verticesTmp_, vertices_, sizeof(Vertex) * numVerts_);

    if (flatMode_)
    {
      for (unsigned int i = 0; i < numTris_; ++i)
      {
        // get face normal
        ACG::Vec3d n = mesh_.normal(mesh_.face_handle(triToFaceMap_[i]));

        // store face normal in last tri vertex
        for (unsigned int k = 0; k < 3; ++k)
        {
          verticesTmp_[indices_[i*3+2]].n[k] = n[k];
        }
      }
      bVBOinFlatMode_ = 1;
    }
    if (!textureMode_)
    {
      for (unsigned int i = 0; i < numVerts_; ++i)
      {
        for (int k = 0; k < 2; ++k)
        {
          verticesTmp_[i].tex[k] = mesh_.texcoord2D(
            mesh_.to_vertex_handle(mesh_.halfedge_handle(vertexMap_[i])))[k];
        }
      }
      bVBOinHalfedgeTexMode_ = 0;
    }

    glBufferDataARB(GL_ARRAY_BUFFER_ARB, numVerts_ * sizeof(Vertex), verticesTmp_, GL_STATIC_DRAW_ARB);

  }

  ACG::GLState::bindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
}

template <class Mesh>
void
DrawMeshT<Mesh>::createIBO()
{
  // data read from indices_

  // create triangle index buffer

  if (!ibo_)
    glGenBuffersARB(1, &ibo_);

  ACG::GLState::bindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, ibo_);

  indexType_ = GL_UNSIGNED_INT;
  if (numVerts_ <= 0xFFFF)
  {
    // use 2 byte indices
    unsigned short* pwIndices = (unsigned short*)indicesTmp_;
    indexType_ = GL_UNSIGNED_SHORT;

    for (unsigned int i = 0; i < numTris_ * 3; ++i)
      pwIndices[i] = (unsigned short)indices_[i];

    glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, numTris_ * 3 * sizeof(unsigned short), pwIndices, GL_STATIC_DRAW_ARB);
  }
  else
    glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, numTris_ * 3 * sizeof(unsigned int), indices_, GL_STATIC_DRAW_ARB);


  // line index buffer:
  if (mesh_.n_edges())
  {
    unsigned int* pLineBuffer = new unsigned int[mesh_.n_edges() * 2];

    if (!lineIBO_)
      glGenBuffersARB(1, &lineIBO_);

    for (unsigned int i = 0; i < mesh_.n_edges(); ++i)
    {
      OpenMesh::HalfedgeHandle hh = mesh_.halfedge_handle(mesh_.edge_handle(i), 0);

      if (indexType_ == GL_UNSIGNED_SHORT)
      {
        ((unsigned short*)pLineBuffer)[2*i] = (unsigned short)invVertexMap_[mesh_.from_vertex_handle(hh).idx()];
        ((unsigned short*)pLineBuffer)[2*i+1] = (unsigned short)invVertexMap_[mesh_.to_vertex_handle(hh).idx()];
      }
      else
      {
        pLineBuffer[2*i] = invVertexMap_[mesh_.from_vertex_handle(hh).idx()];
        pLineBuffer[2*i+1] = invVertexMap_[mesh_.to_vertex_handle(hh).idx()];
      }
    }

    ACG::GLState::bindBufferARB(GL_ELEMENT_ARRAY_BUFFER, lineIBO_);

    // 2 or 4 byte indices:
    if (indexType_ == GL_UNSIGNED_SHORT)
      glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 
                   mesh_.n_edges() * 2 * sizeof(unsigned short), 
                   pLineBuffer, GL_STATIC_DRAW_ARB);
    else
      glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 
                   mesh_.n_edges() * 2 * sizeof(unsigned int),
                   pLineBuffer, GL_STATIC_DRAW_ARB);

    delete [] pLineBuffer;
  }

  ACG::GLState::bindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
}

template <class Mesh>
DrawMeshT<Mesh>::~DrawMeshT(void)
{
  delete [] indices_;
  delete [] indicesTmp_;
  delete [] vertices_;
  delete [] verticesTmp_;
//  delete [] m_pVerticesC;

  delete [] subsets_;

  delete [] vertexMap_;

  delete [] triToFaceMap_;

  delete [] invVertexMap_;

  delete vertexDeclFCol_;
  delete vertexDeclVCol_;
  delete vertexDeclEdgeCol_;
  delete vertexDeclHalfedgeCol_;
  delete vertexDeclHalfedgePos_;

  if (vbo_) glDeleteBuffersARB(1, &vbo_);
  if (ibo_) glDeleteBuffersARB(1, &ibo_);
  if (lineIBO_) glDeleteBuffersARB(1, &lineIBO_);
}



template <class Mesh>
unsigned int DrawMeshT<Mesh>::getMemoryUsage(bool _printReport)
{
  unsigned int res = 0;
  unsigned int sysBufSize = 0;

  // index buffer
  if (indices_)
    sysBufSize += numTris_ * 3 * 4;

  // vertex buffer
  if (vertices_)
    sysBufSize += sizeof(Vertex) * numVerts_;

  // temp buffers
  if (indicesTmp_)
    sysBufSize += numTris_ * 3 * 4;

  if (verticesTmp_)
    sysBufSize += sizeof(Vertex) * numVerts_;

  
  
  res += sysBufSize;

  // mappings
  unsigned int mapsSize = 0;
 
  if (vertexMap_)
    mapsSize += numVerts_ * 4;

  if (triToFaceMap_)
    res += numTris_ * 4;

  if (invVertexMap_)
    res += mesh_.n_vertices() * 4;

  res += mapsSize;


  // picking buffers
  unsigned int pickBufSize = 0;

  pickBufSize += pickVertBuf_.capacity() * sizeof(ACG::Vec3f);
  pickBufSize += pickVertColBuf_.capacity() * sizeof(ACG::Vec4uc);

  pickBufSize += pickEdgeBuf_.capacity() * sizeof(ACG::Vec4uc);

  pickBufSize += pickFaceVertexBuf_.capacity() * sizeof(ACG::Vec3f);
  pickBufSize += pickFaceColBuf_.capacity() * sizeof(ACG::Vec4uc);


  pickBufSize += pickAnyFaceColBuf_.capacity() * sizeof(ACG::Vec4uc);
  pickBufSize += pickAnyEdgeColBuf_.capacity() * sizeof(ACG::Vec4uc);
  pickBufSize += pickAnyVertexColBuf_.capacity() * sizeof(ACG::Vec4uc);

  res += pickBufSize;


  // edge and halfedge vertex buffers (glDraw from sysmem)
  unsigned int edgeBufSize = 0;

  edgeBufSize += perEdgeVertexBuf_.capacity() * sizeof(ACG::Vec3f);
  edgeBufSize += perEdgeColorBuf_.capacity() * sizeof(ACG::Vec4uc);

  edgeBufSize += perHalfedgeVertexBuf_.capacity() * sizeof(ACG::Vec3f);
  edgeBufSize += perHalfedgeColorBuf_.capacity() * sizeof(ACG::Vec4uc);


  res += edgeBufSize;


  unsigned int gpuBufSize = 0;

  if (ibo_)
    gpuBufSize += numTris_ * 3 * (indexType_ == GL_UNSIGNED_INT ? 4 : 2);

  if (vbo_)
    gpuBufSize += numVerts_ * sizeof(Vertex);

  if (_printReport)
  {
    std::cout << "\nDrawMesh memory usage in MB:\n";
    std::cout << "Vertex+IndexBuffer (SYSMEM only): " << float(sysBufSize) / (1024 * 1024);
    std::cout << "\nMappings: " << float(mapsSize) / (1024 * 1024);
    std::cout << "\nPicking Buffers: " << float(pickBufSize) / (1024 * 1024);
    std::cout << "\nEdge Buffers: " << float(edgeBufSize) / (1024 * 1024);
    std::cout << "\nTotal SYSMEM: " << float(res) / (1024 * 1024);
    std::cout << "\nTotal GPU: " << float(gpuBufSize) / (1024 * 1024) << std::endl;
  }
  
  return res;
}



template <class Mesh>
void DrawMeshT<Mesh>::bindBuffers()
{
  // rebuild if necessary
  if (!numTris_ || ! numVerts_ || !subsets_) rebuild_ = REBUILD_FULL;

  if (bVBOinHalfedgeNormalMode_ != halfedgeNormalMode_) rebuild_ = REBUILD_FULL;

  // if no rebuild necessary, check for smooth / flat shading switch 
  // to update normals
  if (rebuild_ == REBUILD_NONE)
  {
    if (bVBOinFlatMode_ != flatMode_ || bVBOinHalfedgeTexMode_ != textureMode_)
      createVBO();
  }
  else
    rebuild();

  ACG::GLState::bindBuffer(GL_ARRAY_BUFFER_ARB, vbo_);

  // prepare color mode
  if (colorMode_)
  {
    if (colorMode_ == 1)
      ACG::GLState::colorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex), (char*)32);
    else
      ACG::GLState::colorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex), (char*)36);

    ACG::GLState::enableClientState(GL_COLOR_ARRAY);
  }

  // vertex decl
  ACG::GLState::vertexPointer(3, GL_FLOAT, sizeof(Vertex), 0);
  ACG::GLState::enableClientState(GL_VERTEX_ARRAY);

  glClientActiveTexture(GL_TEXTURE0);
  ACG::GLState::texcoordPointer(2, GL_FLOAT, sizeof(Vertex), (char*)12);
  ACG::GLState::enableClientState(GL_TEXTURE_COORD_ARRAY);

  ACG::GLState::normalPointer(GL_FLOAT, sizeof(Vertex), (char*)20);
  ACG::GLState::enableClientState(GL_NORMAL_ARRAY);

//  ACG::GLState::normalPointerEXT(3, GL_FLOAT, sizeof(Vertex), (char*)(20));  // ACG::GLState::normalPointerEXT crashes sth. in OpenGL
//  glTangentPointerEXT(4, GL_FLOAT, sizeof(Vertex), (void*)(32));

  //  ACG::GLState::enableClientState(GL_TANGENT_ARRAY_EXT);

  ACG::GLState::bindBuffer(GL_ELEMENT_ARRAY_BUFFER_ARB, ibo_);
}

template <class Mesh>
void DrawMeshT<Mesh>::bindBuffersToRenderObject(RenderObject* _obj)
{
  // rebuild if necessary
  if (!numTris_ || ! numVerts_ || !subsets_) rebuild_ = REBUILD_FULL;

  if (bVBOinHalfedgeNormalMode_ != halfedgeNormalMode_) rebuild_ = REBUILD_FULL;

  // if no rebuild necessary, check for smooth / flat shading switch 
  // to update normals
  if (rebuild_ == REBUILD_NONE)
  {
    if (bVBOinFlatMode_ != flatMode_ || bVBOinHalfedgeTexMode_ != textureMode_)
      createVBO();
  }
  else
    rebuild();

  _obj->vertexBuffer = vbo_;
  _obj->indexBuffer = ibo_;

  _obj->indexType = indexType_;

  // assign correct vertex declaration
  _obj->vertexDecl = vertexDeclVCol_;

  // prepare color mode
  if (colorMode_)
  {
    if (colorMode_ != 1)
      _obj->vertexDecl = vertexDeclFCol_;
  }
}

template <class Mesh>
void DrawMeshT<Mesh>::unbindBuffers()
{
  ACG::GLState::disableClientState(GL_VERTEX_ARRAY);
  ACG::GLState::disableClientState(GL_TEXTURE_COORD_ARRAY);
  ACG::GLState::disableClientState(GL_NORMAL_ARRAY);
  //  ACG::GLState::disableClientState(GL_TANGENT_ARRAY_EXT);

  if (colorMode_)
    ACG::GLState::disableClientState(GL_COLOR_ARRAY);

  ACG::GLState::bindBuffer(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
  ACG::GLState::bindBuffer(GL_ARRAY_BUFFER_ARB, 0);
}

template <class Mesh>
void DrawMeshT<Mesh>::draw(std::map< int, GLuint>* _textureMap)
{
  bindBuffers();

#ifdef DEBUG_MEM_USAGE
  getMemoryUsage(true);
#endif

  if (numTris_)
  {
    if (_textureMap)
    {
      // textured mode

      for (unsigned int i = 0; i < numSubsets_; ++i)
      {
        Subset* pSubset = subsets_ + i;

        if ( _textureMap->find(pSubset->materialID) == _textureMap->end() ) {
          std::cerr << "Illegal texture index ... trying to access " << pSubset->materialID << std::endl;
          ACG::GLState::bindTexture(GL_TEXTURE_2D, 0);
        }
        else
          ACG::GLState::bindTexture(GL_TEXTURE_2D, (*_textureMap)[pSubset->materialID]);

        glDrawElements(GL_TRIANGLES, pSubset->numTris * 3, indexType_,
          (GLvoid*)( pSubset->startIndex * (indexType_ == GL_UNSIGNED_INT ? 4 : 2))); // offset in bytes
      }
    }
    else
      glDrawElements(GL_TRIANGLES, numTris_ * 3, indexType_, 0);
  }

  unbindBuffers();
}


template <class Mesh>
void ACG::DrawMeshT<Mesh>::addTriRenderObjects(IRenderer* _renderer, const RenderObject* _baseObj, std::map< int, GLuint>* _textureMap)
{
  if (numTris_)
  {
    RenderObject ro = *_baseObj;
    bindBuffersToRenderObject(&ro);

    if (_textureMap && _baseObj->shaderDesc.textured)
    {
      // textured mode

      for (unsigned int i = 0; i < numSubsets_; ++i)
      {
        Subset* pSubset = subsets_ + i;

        if ( _textureMap->find(pSubset->materialID) == _textureMap->end() ) {
          std::cerr << "Illegal texture index ... trying to access " << pSubset->materialID << std::endl;
        }
        else
        {
//          ACG::GLState::bindTexture(GL_TEXTURE_2D, (*_textureMap)[pSubset->materialID]);
          ro.texture = (*_textureMap)[pSubset->materialID];
        }

        

        ro.glDrawElements(GL_TRIANGLES, pSubset->numTris * 3, indexType_,
          (GLvoid*)( pSubset->startIndex * (indexType_ == GL_UNSIGNED_INT ? 4 : 2))); // offset in bytes
        
        _renderer->addRenderObject(&ro);
      }
    }
    else
    {
      ro.glDrawElements(GL_TRIANGLES, numTris_ * 3, indexType_, 0);
      _renderer->addRenderObject(&ro);
    }
  }
}


template <class Mesh>
void DrawMeshT<Mesh>::drawLines()
{
  bindBuffers();

  if (mesh_.n_edges())
  {
    ACG::GLState::bindBuffer(GL_ELEMENT_ARRAY_BUFFER, lineIBO_);

    glDrawElements(GL_LINES, mesh_.n_edges() * 2, indexType_, 0);
  }

  unbindBuffers();
}


template <class Mesh>
void DrawMeshT<Mesh>::addLineRenderObjects(IRenderer* _renderer, const RenderObject* _baseObj)
{
  RenderObject ro = *_baseObj;
  bindBuffersToRenderObject(&ro);

  if (mesh_.n_edges())
  {
    ro.indexBuffer = lineIBO_;
    ro.glDrawElements(GL_LINES, mesh_.n_edges() * 2, indexType_, 0);
    
    _renderer->addRenderObject(&ro);
  }
}

template <class Mesh>
void DrawMeshT<Mesh>::drawVertices()
{

  bindBuffers();

  if (numVerts_)
    glDrawArrays(GL_POINTS, 0, numVerts_);

  unbindBuffers();
}

template <class Mesh>
void DrawMeshT<Mesh>::addPointRenderObjects(IRenderer* _renderer, const RenderObject* _baseObj)
{
  RenderObject ro = *_baseObj;
  bindBuffersToRenderObject(&ro);

  if (numVerts_)
  {
    ro.glDrawArrays(GL_POINTS, 0, numVerts_);

    _renderer->addRenderObject(&ro);
  }
}

template <class Mesh>
void DrawMeshT<Mesh>::removeIsolatedVerts()
{
  if (!numVerts_ || !numTris_) return;

  unsigned char* pVertexUsed = new unsigned char[numVerts_];
  memset(pVertexUsed, 0, numVerts_);

  for (unsigned int i = 0; i < numTris_ * 3; ++i)
    pVertexUsed[indices_[i]] = 1;

  unsigned int newNumVerts = 0;

  for (unsigned int i = 0; i < numVerts_; ++i)
  {
    if (pVertexUsed[i])
      ++newNumVerts;
  }


  if (newNumVerts == numVerts_) // early out
  {
    delete [] pVertexUsed;
    return;
  }


  memcpy(verticesTmp_, vertices_, numVerts_ * sizeof(Vertex));

  // remap vertices, track changes

  unsigned int* pRemap = new unsigned int[numVerts_]; // remap[old] = new index

  unsigned int counter = 0;
  for (unsigned int i = 0; i < numVerts_; ++i)
  {
    if (pVertexUsed[i])
      pRemap[i] = counter++;
    else
      pRemap[i] = newNumVerts;
  }


  delete [] pRemap;
  delete [] pVertexUsed;
}

//////////////////////////////////////////////////////////////////////////

template <class Mesh>
unsigned int DrawMeshT<Mesh>::countTris(unsigned int* pMaxVertsOut)
{
  unsigned int triCounter = 0;

  if (pMaxVertsOut) *pMaxVertsOut = 0;

  for (unsigned int i = 0; i < mesh_.n_faces(); ++i)
  {
    typename Mesh::FaceHandle fh =  mesh_.face_handle(i);

    // count vertices
  	unsigned int nPolyVerts = 0;

    for (typename Mesh::FaceHalfedgeIter hh_it = mesh_.fh_iter(fh); hh_it; ++hh_it )++nPolyVerts;

    triCounter += (nPolyVerts - 2);

    if (pMaxVertsOut)
    {
      if (*pMaxVertsOut < nPolyVerts)
        *pMaxVertsOut = nPolyVerts;
    }
  }

  return triCounter;
}



template <class Mesh>
ACG::Vec3f* DrawMeshT<Mesh>::perEdgeVertexBuffer()
{
  // Force update of the buffers if required
  if (updatePerEdgeBuffers_)
    updatePerEdgeBuffers();
  return perEdgeVertexBuf_.empty() ? 0 : &(perEdgeVertexBuf_[0]); 
}

template <class Mesh>
ACG::Vec4f* DrawMeshT<Mesh>::perEdgeColorBuffer()
{
  // Force update of the buffers if required
  if (updatePerEdgeBuffers_)
    updatePerEdgeBuffers();
  return perEdgeColorBuf_.empty() ? 0 : &(perEdgeColorBuf_[0]); 
}


template <class Mesh>
void DrawMeshT<Mesh>::updatePickingVertices(ACG::GLState& _state,
                                            uint _offset)
{
  unsigned int idx = 0;

  // Adjust size of the color buffer to the number of vertices in the mesh
  pickVertColBuf_.resize( mesh_.n_vertices() );
  pickVertBuf_.resize( mesh_.n_vertices() );

  // Get the right picking colors from the gl state and add them per vertex to the color buffer
  typename Mesh::ConstVertexIter v_it(mesh_.vertices_begin()), v_end(mesh_.vertices_end());
  for (; v_it!=v_end; ++v_it, ++idx) 
  {
    pickVertColBuf_[idx] = _state.pick_get_name_color(idx + _offset);
    pickVertBuf_[idx] = mesh_.point(mesh_.vertex_handle(idx));
  }
}



template <class Mesh>
void DrawMeshT<Mesh>::updatePerEdgeBuffers()
{
  // Only update buffers if they are invalid
  if (!updatePerEdgeBuffers_) 
    return;

  perEdgeVertexBuf_.resize(mesh_.n_edges() * 2);

  if ( mesh_.has_edge_colors() ) {
    perEdgeColorBuf_.resize(mesh_.n_edges() * 2);
  } else
    perEdgeColorBuf_.clear();    

  unsigned int idx = 0;

  typename Mesh::ConstEdgeIter  e_it(mesh_.edges_sbegin()), e_end(mesh_.edges_end());
  for (; e_it!=e_end; ++e_it) {

    perEdgeVertexBuf_[idx]   = mesh_.point(mesh_.to_vertex_handle(mesh_.halfedge_handle(e_it, 0)));
    perEdgeVertexBuf_[idx+1] = mesh_.point(mesh_.to_vertex_handle(mesh_.halfedge_handle(e_it, 1)));

    if (  mesh_.has_edge_colors() ) {
      const Vec4f color = OpenMesh::color_cast<Vec4f>( mesh_.color(e_it) ) ;
      perEdgeColorBuf_[ idx ]     = color;
      perEdgeColorBuf_[ idx + 1 ] = color;
    }

    idx += 2;
  }


  updatePerEdgeBuffers_ = 0;

  updateEdgeHalfedgeVertexDeclarations();
}

template <class Mesh>
void DrawMeshT<Mesh>::updatePerHalfedgeBuffers()
{
  // Only update buffers if they are invalid
  if (!updatePerHalfedgeBuffers_) 
    return;

  perHalfedgeVertexBuf_.resize(mesh_.n_halfedges() * 2);

  if ( mesh_.has_halfedge_colors() ) {
    perHalfedgeColorBuf_.resize(mesh_.n_halfedges() * 2);
  } else
    perHalfedgeColorBuf_.clear();    

  unsigned int idx = 0;

  typename Mesh::ConstHalfedgeIter  he_it(mesh_.halfedges_sbegin()), he_end(mesh_.halfedges_end());
  for (; he_it!=he_end; ++he_it) {

    perHalfedgeVertexBuf_[idx]   = halfedge_point(he_it);
    perHalfedgeVertexBuf_[idx+1] = halfedge_point(mesh_.prev_halfedge_handle(he_it));

    if (  mesh_.has_halfedge_colors() ) {
      const Vec4f color = OpenMesh::color_cast<Vec4f>( mesh_.color(he_it) ) ;
      perHalfedgeColorBuf_[ idx ]     = color;
      perHalfedgeColorBuf_[ idx + 1 ] = color;
    }

    idx += 2;
  }

  updatePerHalfedgeBuffers_ = 0;

  updateEdgeHalfedgeVertexDeclarations();
}

template <class Mesh>
typename Mesh::Point
DrawMeshT<Mesh>::
halfedge_point(const typename Mesh::HalfedgeHandle _heh) {

  typename Mesh::Point p  = mesh_.point(mesh_.to_vertex_handle  (_heh));
  typename Mesh::Point pp = mesh_.point(mesh_.from_vertex_handle(_heh));
  typename Mesh::Point pn = mesh_.point(mesh_.to_vertex_handle(mesh_.next_halfedge_handle(_heh)));

  //  typename Mesh::Point n  = (p-pp)%(pn-p);
  typename Mesh::Point fn;
  if( !mesh_.is_boundary(_heh))
    fn = mesh_.normal(mesh_.face_handle(_heh));
  else
    fn = mesh_.normal(mesh_.face_handle(mesh_.opposite_halfedge_handle(_heh)));

  typename Mesh::Point upd = ((fn%(pn-p)).normalize() + (fn%(p-pp)).normalize()).normalize();

  upd *= ((pn-p).norm()+(p-pp).norm())*0.08;

  return (p+upd);

  // double alpha = 0.1;
  // // correct weighting for concave triangles (or at concave boundaries)
  // if( (fn | n)  < 0.0) alpha *=-1.0;

  // return (p*(1.0-2.0*alpha) + pp*alpha + pn*alpha);
}

template <class Mesh>
ACG::Vec3f* DrawMeshT<Mesh>::perHalfedgeVertexBuffer()
{
  // Force update of the buffers if required
  if (updatePerHalfedgeBuffers_)
    updatePerHalfedgeBuffers();
  return perHalfedgeVertexBuf_.empty() ? 0 : &(perHalfedgeVertexBuf_[0]); 
}

template <class Mesh>
ACG::Vec4f* DrawMeshT<Mesh>::perHalfedgeColorBuffer()
{
  // Force update of the buffers if required
  if (updatePerHalfedgeBuffers_)
    updatePerHalfedgeBuffers();
  return perHalfedgeColorBuf_.empty() ? 0 : &(perHalfedgeColorBuf_[0]); 
}




template <class Mesh>
void DrawMeshT<Mesh>::updatePickingEdges(ACG::GLState& _state,
                                         unsigned int _offset)
{
  updatePerEdgeBuffers();

  pickEdgeBuf_.resize(mesh_.n_edges() * 2);


  int idx = 0;

  typename Mesh::ConstEdgeIter  e_it(mesh_.edges_sbegin()), e_end(mesh_.edges_end());
  for (; e_it!=e_end; ++e_it) {

    const Vec4uc pickColor =  _state.pick_get_name_color (e_it.handle().idx() + _offset);

    pickEdgeBuf_[idx]    = pickColor;
    pickEdgeBuf_[idx+1]  = pickColor;

    idx += 2;
  }
}









template <class Mesh>
void DrawMeshT<Mesh>::updatePickingFaces(ACG::GLState& _state )
{
  // Make sure, the face buffers are up to date before generating the picking data!
  if (!numTris_ && mesh_.n_faces())
  {
    rebuild_ = REBUILD_FULL;
    rebuild();
  }

  pickFaceVertexBuf_.resize(3 * numTris_);
  pickFaceColBuf_.resize(3 * numTris_);

  // Index to the current buffer position
  unsigned int bufferIndex = 0;

  for (unsigned int i = 0; i < numTris_; ++i)
  {
    const Vec4uc pickColor = _state.pick_get_name_color ( triToFaceMap_[i] );
    for (unsigned int k = 0; k < 3; ++k)
    {
      pickFaceVertexBuf_[i * 3 + k] = mesh_.point(
        mesh_.to_vertex_handle(mesh_.halfedge_handle(vertexMap_[indices_[i * 3 + k]])));

      pickFaceColBuf_[i * 3 + k] = pickColor;
    }
  }


}


template <class Mesh>
void DrawMeshT<Mesh>::updatePickingAny(ACG::GLState& _state )
{
  if (!numTris_ && mesh_.n_faces())
  {
    rebuild_ = REBUILD_FULL;
    rebuild();
  }
  
  pickFaceVertexBuf_.resize(3 * numTris_);
  pickAnyFaceColBuf_.resize(3 * numTris_);

  // Index to the current buffer position
  unsigned int bufferIndex = 0;

  for (unsigned int i = 0; i < numTris_; ++i)
  {
    const Vec4uc pickColor = _state.pick_get_name_color ( triToFaceMap_[i] );
    for (unsigned int k = 0; k < 3; ++k)
    {
      pickFaceVertexBuf_[i * 3 + k] = mesh_.point(
        mesh_.to_vertex_handle(mesh_.halfedge_handle(vertexMap_[indices_[i * 3 + k]])));

      pickAnyFaceColBuf_[i * 3 + k] = pickColor;
    }
  }



  updatePerEdgeBuffers();

  pickAnyEdgeColBuf_.resize(mesh_.n_edges() * 2);

  unsigned int idx = 0;
  typename Mesh::ConstEdgeIter  e_it(mesh_.edges_sbegin()), e_end(mesh_.edges_end());
  for (; e_it!=e_end; ++e_it) {

    const Vec4uc pickColor =  _state.pick_get_name_color (e_it.handle().idx() + mesh_.n_faces());

    pickAnyEdgeColBuf_[idx]    = pickColor;
    pickAnyEdgeColBuf_[idx+1]  = pickColor;

    idx += 2;
  }



  idx = 0;

  // Adjust size of the color buffer to the number of vertices in the mesh
  pickAnyVertexColBuf_.resize( mesh_.n_vertices() );
  pickVertBuf_.resize( mesh_.n_vertices() );

  // Get the right picking colors from the gl state and add them per vertex to the color buffer
  typename Mesh::ConstVertexIter v_it(mesh_.vertices_begin()), v_end(mesh_.vertices_end());
  for (; v_it!=v_end; ++v_it, ++idx) 
  {
    pickAnyVertexColBuf_[idx] = _state.pick_get_name_color(idx + mesh_.n_faces() + mesh_.n_edges());
    pickVertBuf_[idx] = mesh_.point(mesh_.vertex_handle(idx));
  }
}


template <class Mesh>
void
DrawMeshT<Mesh>::
setTextureIndexPropertyName( std::string _indexPropertyName ) {

  // Check if the given property exists
  OpenMesh::FPropHandleT< int > textureIndexProperty;
  if ( !mesh_.get_property_handle(textureIndexProperty,_indexPropertyName) )  {
    if ( _indexPropertyName != "No Texture Index" )
      std::cerr << "DrawMeshT: Unable to get per face texture Index property named " << _indexPropertyName << std::endl;
    return;
  }

  // Remember the property name
  textureIndexPropertyName_ = _indexPropertyName;

  // mark strips as invalid ( have to be regenerated to collect texture index information)
//   stripsValid_ = false;
// 
//   // mark the buffers as invalid as we have a new per face index array
//   invalidatePerFaceBuffers();
  rebuild_ |= REBUILD_TOPOLOGY;
}

template <class Mesh>
void
DrawMeshT<Mesh>::
setPerFaceTextureCoordinatePropertyName( std::string _perFaceTextureCoordinatePropertyName ) {

  // Check if the given property exists
  OpenMesh::HPropHandleT< typename Mesh::TexCoord2D >  perFaceTextureCoordinateProperty;
  if ( !mesh_.get_property_handle(perFaceTextureCoordinateProperty,_perFaceTextureCoordinatePropertyName) )  {
    if ( _perFaceTextureCoordinatePropertyName != "No Texture" )
      std::cerr << "DrawMeshT: Unable to get per face texture coordinate property named " << _perFaceTextureCoordinatePropertyName << std::endl;
    return;
  }

  // Remember the property name
  perFaceTextureCoordinatePropertyName_ = _perFaceTextureCoordinatePropertyName;

  // mark the buffers as invalid as we have a new per face index array
//  invalidatePerFaceBuffers();
  rebuild_ |= REBUILD_GEOMETRY;
}


template <class Mesh>
unsigned int DrawMeshT<Mesh>::getNumTextures()
{
  unsigned int n = 0;
  for (unsigned int i = 0; i < numSubsets_; ++i)
  {
    if (subsets_[i].materialID > 0) ++n;
  }
  return n;
}

template <class Mesh>
int 
DrawMeshT<Mesh>::perFaceTextureCoordinateAvailable()  {

  // We really have to recheck, as the property might get lost externally (e.g. on restores of the mesh)
  OpenMesh::HPropHandleT< typename Mesh::TexCoord2D >  perFaceTextureCoordinateProperty;
  if ( !mesh_.get_property_handle(perFaceTextureCoordinateProperty, perFaceTextureCoordinatePropertyName_) )  {
    return false;
  }

  // Property available
  return true;

}  


template <class Mesh>
int 
DrawMeshT<Mesh>::perFaceTextureIndexAvailable() {

  // We really have to recheck, as the property might get lost externally (e.g. on restores of the mesh)
  OpenMesh::FPropHandleT< int > textureIndexProperty;
  if ( !mesh_.get_property_handle(textureIndexProperty, textureIndexPropertyName_) )  {
    return false;
  }

  // Property available
  return true;
}






template <class Mesh>
void ACG::DrawMeshT<Mesh>::createVertexDeclarations()
{
  vertexDeclVCol_->addElement(GL_FLOAT, 3, VERTEX_USAGE_POSITION);
  vertexDeclVCol_->addElement(GL_FLOAT, 2, VERTEX_USAGE_TEXCOORD);
  vertexDeclVCol_->addElement(GL_FLOAT, 3, VERTEX_USAGE_NORMAL);
  vertexDeclVCol_->addElement(GL_UNSIGNED_BYTE, 4, VERTEX_USAGE_COLOR);

  // VertexDeclaration computes a stride of 36 automatically
  // force 40 bytes instead!
  vertexDeclVCol_->setVertexStride(sizeof(Vertex)); // pad for Vertex::fcol


  vertexDeclFCol_->addElement(GL_FLOAT, 3, VERTEX_USAGE_POSITION);
  vertexDeclFCol_->addElement(GL_FLOAT, 2, VERTEX_USAGE_TEXCOORD, 12);
  vertexDeclFCol_->addElement(GL_FLOAT, 3, VERTEX_USAGE_NORMAL, 20);
  vertexDeclFCol_->addElement(GL_UNSIGNED_BYTE, 4, VERTEX_USAGE_COLOR, 36);
}


template <class Mesh>
void ACG::DrawMeshT<Mesh>::updateEdgeHalfedgeVertexDeclarations()
{
  VertexElement elemArrayEC[] = { {GL_FLOAT, 3, VERTEX_USAGE_POSITION, 0, 0 },
                                 {GL_UNSIGNED_BYTE, 4, VERTEX_USAGE_COLOR, 0, 0 } };

  elemArrayEC[0].pointer_ = perEdgeVertexBuffer();
  elemArrayEC[1].pointer_ = perEdgeColorBuffer();

  VertexElement elemArrayH =  {GL_FLOAT, 3, VERTEX_USAGE_POSITION, 0, 0 };
  elemArrayH.pointer_ = perHalfedgeVertexBuffer();

  VertexElement elemArrayHC[] =  { {GL_FLOAT, 3, VERTEX_USAGE_POSITION, 0, 0 },
                                 {GL_UNSIGNED_BYTE, 4, VERTEX_USAGE_COLOR, 0, 0 } };
  elemArrayHC[0].pointer_ = perHalfedgeVertexBuffer();
  elemArrayHC[1].pointer_ = perHalfedgeColorBuffer();

  vertexDeclEdgeCol_->clear();
  vertexDeclHalfedgeCol_->clear();
  vertexDeclHalfedgePos_->clear();

  vertexDeclEdgeCol_->addElements(2, elemArrayEC);
  vertexDeclHalfedgeCol_->addElements(2, elemArrayHC);
  vertexDeclHalfedgePos_->addElement(&elemArrayH);

  vertexDeclEdgeCol_->setVertexStride(0);
  vertexDeclHalfedgeCol_->setVertexStride(0);
  vertexDeclHalfedgePos_->setVertexStride(0);
}


}
