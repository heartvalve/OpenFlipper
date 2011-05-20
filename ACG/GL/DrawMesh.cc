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

#define ACG_DRAW_MESH_C

//=============================================================================

#include "DrawMesh.hh"
#include <ACG/Geometry/GPUCacheOptimizer.hh>
#include <assert.h>
#include <math.h>
#include <vector>
#include <map>
#include <string.h>
#include <fstream>

//=============================================================================

#define DRAW_MESH_VERTEX_CMP_EPSILON 1e-4f


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
:  m_Mesh(_mesh),
   m_NumTris(0), m_NumVerts(0),
   m_pIndices(0),
   m_pVertices(0),
   m_Rebuild(REBUILD_NONE),
   m_PrevNumFaces(0), m_PrevNumVerts(0),
   m_NumSubsets(0),
   m_pSubsets(0),
   m_VBO(0), m_IBO(0),
   m_LineIBO(0),
   m_IndexType(0),
   m_iColorMode(1),
   m_bFlatMode(0), m_bVBOinFlatMode(0),
   m_pTriToFaceMap(0),
   m_pVertexMap(0),
   m_pInvVertexMap(0),
   m_pIndicesTmp(0),
   m_pVerticesTmp(0),
   textureIndexPropertyName_("Not Set"),
   perFaceTextureCoordinatePropertyName_("Not Set"),
   m_bUpdatePerEdgeBuffers(1),
  m_bUpdatePerHalfedgeBuffers(1)
{
}


template <class Mesh>
void
DrawMeshT<Mesh>::rebuild()
{
  if (m_Rebuild == REBUILD_NONE) return;

  unsigned int MaxFaceVertCount = 0;
  unsigned int NewTriCount = countTris(&MaxFaceVertCount);
  
  int bTriangleRebuild = 0; // what should be rebuild?
  int bVertexRebuild = 0;

//  m_Mesh.request_face_normals();


  if (NewTriCount > m_NumTris)
  {
    // index buffers too small
    // resize here
    delete [] m_pIndices;
    delete [] m_pIndicesTmp;
    delete [] m_pTriToFaceMap;

    glDeleteBuffers(1, &m_IBO);
    m_IBO = 0;

    m_NumTris = NewTriCount;

    m_pTriToFaceMap = new unsigned int[m_NumTris];
    m_pIndices = new unsigned int[m_NumTris * 3];
    m_pIndicesTmp = new unsigned int[m_NumTris * 3];

    bTriangleRebuild = 1;
  }

  if (m_PrevNumFaces != m_Mesh.n_faces())
  {
    bTriangleRebuild = 1;
    m_PrevNumFaces = m_Mesh.n_faces();
  }

  if (m_PrevNumVerts != m_Mesh.n_vertices())
  {
    if (m_PrevNumVerts < m_Mesh.n_vertices())
    {
      // resize inverse vertex map
      delete [] m_pInvVertexMap;
      m_pInvVertexMap = new unsigned int[m_Mesh.n_vertices()];
    }

    bVertexRebuild = 1;
    bTriangleRebuild = 1; // this may have caused changes in the topology!
    m_PrevNumVerts = m_Mesh.n_vertices();
  }

  // support faster update by only updating vertices
  if (!bTriangleRebuild && !bVertexRebuild && m_Rebuild == REBUILD_GEOMETRY)
  {
    // only update vertices, i.e. update values of vertices
    
    for (unsigned int i = 0; i < m_NumVerts; ++i)
    {
      unsigned int MeshHalfedge = m_pVertexMap[i];

//      Mesh::VertexHandle vh = m_Mesh.vertex_handle(MeshVertex);

      // just pick one face, srews up face colors here so color updates need a full rebuild
      typename Mesh::HalfedgeHandle hh = m_Mesh.halfedge_handle(MeshHalfedge);
      typename Mesh::VertexHandle vh = m_Mesh.to_vertex_handle(hh);
      typename Mesh::FaceHandle fh = m_Mesh.face_handle(hh);

      readVertex(m_pVertices + i, vh, hh, fh);
    }

    createVBO();

    m_Rebuild = REBUILD_NONE;
    return;
  }


  std::vector<unsigned int> Tris(m_NumTris * 3);


  //////////////////////////////////////////////////////////////////////////
  // convert from half_edge data structure to triangle index list


  m_NumTris = convertToTriangleMesh(&Tris[0], MaxFaceVertCount);

  //////////////////////////////////////////////////////////////////////////
  // process vertices
  //  1. one vertex per triangle corner
  //  2. weld equal vertices

  unsigned int NewNumVerts = 0;
  std::vector<Vertex> TmpVertexBuffer(3 * m_NumTris); // pre vertex welding
  std::vector<Vertex> DstVertexBuffer(3 * m_NumTris); // post vertex welding

  unsigned int* pVertexRemap = new unsigned int[3 * m_NumTris]; // keeps track of vertex origin (per-vertex attributes)

  createBigVertexBuf(&TmpVertexBuffer[0], pVertexRemap, &Tris[0]);


  // minimize vertex buffer
  unsigned int* pTmpVertexRemap = new unsigned int[3 * m_NumTris];

  NewNumVerts = weldVertices(&DstVertexBuffer[0],
    &TmpVertexBuffer[0], &Tris[0], &pTmpVertexRemap[0], pVertexRemap);


  std::swap(pTmpVertexRemap, pVertexRemap);
  delete [] pTmpVertexRemap; pTmpVertexRemap = 0;


  // end-of mesh extraction

  if (m_NumVerts < NewNumVerts)
  {
    // per vertex buffers too small
    delete [] m_pVertices;
    delete [] m_pVerticesTmp;
    delete [] m_pVertexMap;
//    delete [] m_pVerticesC;

    m_NumVerts = NewNumVerts;

    m_pVertices = new Vertex[m_NumVerts];
    m_pVerticesTmp = new Vertex[m_NumVerts];
    m_pVertexMap = new unsigned int[m_NumVerts];
  }

 

  //////////////////////////////////////////////////////////////////////////
  // sort triangles by material

  sortTrisByMaterial(m_pIndices, &Tris[0]);
//  memcpy(m_pIndices, &Tris[0], 3 * m_NumTris * sizeof(UINT));

  //////////////////////////////////////////////////////////////////////////
  // optimize vcache
  //  by material subset

  // index buffer first

  optimizeTris(m_pIndices, m_pIndices);
  optimizeVerts(m_pVertices, &DstVertexBuffer[0], m_pIndices, pVertexRemap);


  delete [] pVertexRemap;


  //////////////////////////////////////////////////////////////////////////
  // create the inverse vertex map

  for (unsigned int i = 0; i < m_NumVerts; ++i)
  {
    OpenMesh::HalfedgeHandle hh = m_Mesh.halfedge_handle(m_pVertexMap[i]);
    m_pInvVertexMap[m_Mesh.to_vertex_handle(hh).idx()] = i;
  }


  //////////////////////////////////////////////////////////////////////////
  // copy to GPU

  createVBO();
  createIBO();


  m_Rebuild = REBUILD_NONE;

  // picking buffers update
}


template <class Mesh>
unsigned int
DrawMeshT<Mesh>::convertToTriangleMesh(unsigned int* DstIndexBuf, unsigned int MaxFaceVertexCount)
{
  unsigned int TriCounter = 0;
  unsigned int IndexCounter = 0;

  unsigned int* pFaceVerts = new unsigned int[MaxFaceVertexCount];

  for (unsigned int i = 0; i < m_Mesh.n_faces(); ++i)
  {
    typename Mesh::FaceHandle fh =  m_Mesh.face_handle(i);
    // convert this face to triangles

	  typename Mesh::HalfedgeHandle hh_start = m_Mesh.halfedge_handle(fh);
 
  	// count vertices
  	unsigned int nPolyVerts = 0;

    // read all vertex indices of this face
    for (typename Mesh::FaceHalfedgeIter hh_it = m_Mesh.fh_iter(fh); hh_it; ++hh_it )
      pFaceVerts[nPolyVerts++] = m_Mesh.to_vertex_handle(hh_it).idx();

    //////////////////////////////////////////////////////////////////////////
    // convert to triangles

    m_pTriToFaceMap[TriCounter++] = i;
    for (int k = 0; k < 3; ++k) DstIndexBuf[IndexCounter++] = pFaceVerts[k];

    //  trivial fanning

    for (unsigned int k = 3; k < nPolyVerts; ++k)
    {
      m_pTriToFaceMap[TriCounter++] = i;

      DstIndexBuf[IndexCounter++] = pFaceVerts[0];
      DstIndexBuf[IndexCounter++] = pFaceVerts[k-1];
      DstIndexBuf[IndexCounter++] = pFaceVerts[k];
    }
  }

  delete [] pFaceVerts;

  return TriCounter;
}

template <class Mesh>
void
DrawMeshT<Mesh>::readVertex(Vertex* pV,
                            typename Mesh::VertexHandle vh,
                            typename Mesh::HalfedgeHandle hh,
                            typename Mesh::FaceHandle fh)
{
  for (int m = 0; m < 3; ++m)
  {
    pV->pos[m] = m_Mesh.point(vh)[m];

    if (m_Mesh.has_vertex_normals())
      pV->n[m] = m_Mesh.normal(vh)[m];
    else pV->n[m] = 0.0f;

    if (m < 2)
    {
      if (m_Mesh.has_vertex_texcoords2D())
         pV->tex[m] = m_Mesh.texcoord2D(hh)[m];
      else pV->tex[m] = 0.0f;
    }
  }
  
  // per face & per vertex color
  unsigned int byteCol[2];
  for (int col = 0; col < 2; ++col)
  {
    typename Mesh::Color vecCol(255, 255, 255);

    if (col == 0 && m_Mesh.has_vertex_colors()) vecCol = m_Mesh.color(vh);
    if (col == 1 && m_Mesh.has_face_colors() && fh.idx() >= 0) vecCol = m_Mesh.color(fh);

    // OpenGL color format: A8B8G8R8
    byteCol[col] = (unsigned char)(vecCol[0]);
    byteCol[col] |= ((unsigned char)(vecCol[1])) << 8;
    byteCol[col] |= ((unsigned char)(vecCol[2])) << 16;
//          byteCol[col] |= ((unsigned char)(floatCol[3])) << 24;   // no alpha channel in OpenMesh
    byteCol[col] |= 0xFF << 24;
  }

 pV->vcol = byteCol[0];
//  pV->vcol = 0xFFFF0000; // blue,  debug
 pV->fcol = byteCol[1];
//  pV->fcol = 0xFF00FF00; // green, debug
}

template <class Mesh>
void
DrawMeshT<Mesh>::createBigVertexBuf(Vertex* DstVertexBuf,
                                    unsigned int* DstVertexMap,
                                    const unsigned int* IndexBuf)
{
  // fill 3 * NumTris vertex buffer
  for (unsigned int i = 0; i < m_NumTris; ++i)
  {
    typename Mesh::FaceHandle fh = m_Mesh.face_handle(m_pTriToFaceMap[i]);

    for (int k = 0; k < 3; ++k)
    {
      typename Mesh::VertexHandle vh = m_Mesh.vertex_handle(IndexBuf[i * 3 + k]);

      // find corresponding halfedge and get per face attributes
      typename Mesh::HalfedgeHandle hh;

      for (typename Mesh::FaceHalfedgeIter hh_it = m_Mesh.fh_iter(fh); hh_it; ++hh_it )
      {
        if (m_Mesh.to_vertex_handle(hh_it).idx() == vh.idx())
        {
          hh = hh_it;
          break;
        }
      }

      DstVertexMap[i * 3 + k] = hh.idx();


      Vertex* pV = DstVertexBuf + i * 3 + k;

      readVertex(pV, vh, hh, fh);
    }
  }
}

template <class Mesh>
unsigned int 
DrawMeshT<Mesh>::weldVertices(Vertex* DstVertexBuf,
                              const Vertex* SrcVertexBuf,
                              unsigned int* DstIndexBuf, 
                              unsigned int* DstVertexMap,
                              const unsigned int* SrcVertexMap)
{
// 
//   for (unsigned int i = 0; i < m_Mesh.n_vertices(); ++i)
//   {
//     typename Mesh::VertexHandle vh = m_Mesh.vertex_handle(i);
// 
//     DstVertexMap[i] = i;
// 
//     ReadVertex(DstVertexBuf + i, vh, (OpenMesh::HalfedgeHandle)0, (OpenMesh::FaceHandle)0);
//   }
// 
//   return m_Mesh.n_vertices();


  unsigned int NewCount = 0;
  unsigned int HashSize = intNextPowerOfTwo(3 * m_NumTris);

  // hash table + linked list for collision check
  std::vector<unsigned int> HashTable(HashSize + 3 * m_NumTris);
  unsigned int* pNext = &HashTable[HashSize];

  memset(&HashTable[0], 0xFFFFFFFF, HashSize * sizeof(unsigned int));

  for (unsigned int i = 0; i < 3 * m_NumTris; ++i)
  {
    const Vertex* pV = SrcVertexBuf + i;

    // float3 hash function, hash by position only
    unsigned int* ui = (unsigned int*)pV->pos;
    unsigned int f = (ui[0] - ui[1]*13 + ui[2]*23)&0x7ffffE00; // make unsigned, discard some high precision mantissa bits
    unsigned int HashVal = ((f>>22)^(f>>12)^(f)) & (HashSize - 1); //  modulo HashSize

    // check previous entries

    unsigned int Offset = HashTable[HashVal];

    while ((Offset != 0xFFFFFFFF) && (!DstVertexBuf[Offset].equals(*pV)))
      Offset = pNext[Offset];

    if (Offset == 0xFFFFFFFF)
    {
      // create new vertex entry here:
      DstIndexBuf[i] = NewCount;

      DstVertexMap[NewCount] = SrcVertexMap[i];

      memcpy(&DstVertexBuf[NewCount], pV, sizeof(Vertex));

      pNext[NewCount] = HashTable[HashVal];

      HashTable[HashVal] = NewCount++;
    }
    else // duplicate vertex found, use previous index
      DstIndexBuf[i] = Offset;
  }


  // PER-FACE-NORMAL FIX
  // welding is finished here, but we have to ensure that each
  // triangle has at least one unshared vertex
  //  this vertex must to be the last referenced vertex of the triangle
  //  when switching to flat-shading, this vertex holds the face normal

  // count # adjacent tris for each vertex
  unsigned int* pNumAdjTris = new unsigned int[NewCount];
  memset(pNumAdjTris, 0, NewCount * sizeof(unsigned int));

  unsigned int TotalNumAdjTris = 0;

  for (unsigned int i = 0; i < m_NumTris; ++i)
  {
    for (unsigned int k = 0; k < 3; ++k)
    {
      ++pNumAdjTris[DstIndexBuf[i * 3 + k]];
      ++TotalNumAdjTris;
    }
  }

  // detect tris not owning a distinct vertex

  for (unsigned int t = 0; t < m_NumTris; ++t)
  {
    unsigned int UnsharedVertex = 0xFFFFFFFF; // which vertex of this tri is unshared

    // count # shared vertices
    for (unsigned int i = 0; i < 3; ++i)
    {
      unsigned int V = DstIndexBuf[t * 3 + i];

      if (pNumAdjTris[V] == 1)
      {
        UnsharedVertex = V;
        break;
      }
    }

    if (UnsharedVertex == 0xFFFFFFFF)
    {
      // this tri does not own a distinct vertex
      // add a seperate vertex for this triangle

      DstVertexMap[NewCount] = DstVertexMap[DstIndexBuf[t*3+2]];

      // optimization: only split when necessary, (no double split if NumAdjTris[V] = 2)
      --pNumAdjTris[DstIndexBuf[t*3+2]];

      memcpy(&DstVertexBuf[NewCount], &DstVertexBuf[DstIndexBuf[t*3+2]], sizeof(Vertex));

      DstIndexBuf[t*3+2] = NewCount++;
    }
    else
    {
      // rotate references until the unshared vertex is the first one
      while (UnsharedVertex != DstIndexBuf[t * 3 + 2])
      {
        unsigned int tri[] = {DstIndexBuf[t*3], DstIndexBuf[t*3+1], DstIndexBuf[t*3+2]};

        DstIndexBuf[t*3] = tri[1];
        DstIndexBuf[t*3+1] = tri[2];
        DstIndexBuf[t*3+2] = tri[0];
      }
    }
  }

  delete [] pNumAdjTris;


  return NewCount;
}


template <class Mesh>
int
DrawMeshT<Mesh>::getTextureIDofTri(unsigned int tri)
{
  OpenMesh::FPropHandleT< int > textureIndexProperty;
  if (m_Mesh.get_property_handle(textureIndexProperty, textureIndexPropertyName_))
    return m_Mesh.property(textureIndexProperty, m_Mesh.face_handle(m_pTriToFaceMap[tri]));
  
  if (m_Mesh.has_face_texture_index())
    return m_Mesh.texture_index(m_Mesh.face_handle(m_pTriToFaceMap[tri]));
  
  return 0;
}

template<class Mesh>
void
DrawMeshT<Mesh>::sortTrisByMaterial(unsigned int* DstIndexBuf, const unsigned int* SrcIndexBuf)
{
  delete [] m_pSubsets;
  m_pSubsets = 0;
  m_NumSubsets = 0;

  unsigned int* pTmpTriToFaceMap = new unsigned int[m_NumTris];

  std::map<int, unsigned int> MatIDs; // map[matid] = first tri

  for (unsigned int i = 0; i < m_NumTris; ++i)
  {
    int texID = getTextureIDofTri(i);
    if (MatIDs.find(texID) == MatIDs.end())
      MatIDs[texID] = i;
  }

  m_NumSubsets = MatIDs.size();
  m_pSubsets = new Subset[m_NumSubsets];

  std::map<int, unsigned int>::const_iterator it = MatIDs.begin();

  unsigned int LastIndexOffset = 0;

  for (unsigned int i = 0; it != MatIDs.end(); ++i, ++it)
  {
    m_pSubsets[i].MaterialID = it->first;

    // rearrange by subset chunks

    m_pSubsets[i].StartIndex = LastIndexOffset;
    m_pSubsets[i].NumTris = 0;

    for (unsigned int k = it->second; k < m_NumTris; ++k)
    {
      if (getTextureIDofTri(k) == m_pSubsets[i].MaterialID)
      {
        pTmpTriToFaceMap[LastIndexOffset / 3] = m_pTriToFaceMap[k];

        for (int v = 0; v < 3; ++v)
          DstIndexBuf[LastIndexOffset++] = SrcIndexBuf[k * 3 + v];
      }
    }

    m_pSubsets[i].NumTris = (LastIndexOffset - m_pSubsets[i].StartIndex) / 3;

  }

  std::swap(m_pTriToFaceMap, pTmpTriToFaceMap);

  delete [] pTmpTriToFaceMap;
}

template<class Mesh>
void
DrawMeshT<Mesh>::optimizeTris(unsigned int* DstIndexBuf, unsigned int* SrcIndexBuf)
{
  unsigned int* pTmpTriToFaceMap = new unsigned int[m_NumTris];
  for (unsigned int i = 0; i < m_NumSubsets; ++i)
  {
    Subset* pSubset = m_pSubsets + i;

    GPUCacheOptimizerTipsify copt(24, pSubset->NumTris, m_NumVerts, 4, SrcIndexBuf + pSubset->StartIndex);
    copt.WriteIndexBuffer(4, DstIndexBuf + pSubset->StartIndex);

    // apply changes to trimap
    const unsigned int StartTri = pSubset->StartIndex/3;
    for (unsigned int k = 0; k < pSubset->NumTris; ++k)
    {
      unsigned int SrcTri = copt.GetTriangleMap()[k];
      pTmpTriToFaceMap[k + StartTri] = m_pTriToFaceMap[SrcTri + StartTri];
    }
  }
  std::swap(m_pTriToFaceMap, pTmpTriToFaceMap);
  delete [] pTmpTriToFaceMap;
}


template <class Mesh>
void
DrawMeshT<Mesh>::optimizeVerts(Vertex* DstVertexBuf,
                               const Vertex* SrcVertexBuf,
                               unsigned int* InOutIndexBuf, 
                               const unsigned int* SrcVertexMap)
{
  // vertices
  std::vector<unsigned int> VertexRemap(m_NumVerts);
  GPUCacheOptimizer::OptimizeVertices(m_NumTris, m_NumVerts, 4, InOutIndexBuf, &VertexRemap[0]);

  for (unsigned int i = 0; i < m_NumTris * 3; ++i) m_pIndices[i] = VertexRemap[m_pIndices[i]];

  // isolated vertices get sorted out here
  // it's crucial to set m_NumVerts to the # referenced verts, to avoid problems 
  // with picking functions

  unsigned int NumIsolates = 0;

  for (unsigned int i = 0; i < m_NumVerts; ++i)
  {
    if (VertexRemap[i] < m_NumVerts)
    {
      memcpy(DstVertexBuf + VertexRemap[i], SrcVertexBuf + i, sizeof(Vertex));

//      m_pVertexMap[i] = SrcVertexMap[i];
      m_pVertexMap[VertexRemap[i]] = SrcVertexMap[i];
    }
    else
      ++NumIsolates; // isolated vertex
  }

  m_NumVerts -= NumIsolates;
}


template <class Mesh>
void
DrawMeshT<Mesh>::createVBO()
{
  // data read from m_pVertices
  if (!m_VBO)
    glGenBuffersARB(1, &m_VBO);

  glGenBuffersARB(1, &m_VBO);

  glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_VBO);

  if (!m_bFlatMode)
  {
    glBufferDataARB(GL_ARRAY_BUFFER_ARB, m_NumVerts * sizeof(Vertex), m_pVertices, GL_STATIC_DRAW_ARB);
    m_bVBOinFlatMode = 0;
  }
  else
  {
    // use per face normals
    memcpy(m_pVerticesTmp, m_pVertices, sizeof(Vertex) * m_NumVerts);

    for (unsigned int i = 0; i < m_NumTris; ++i)
    {
      // calculate face normal
      ACG::Vec3f V[3];

      for (unsigned int k = 0; k < 3; ++k)
      {
        Vertex* p = m_pVerticesTmp + m_pIndices[i * 3 + k];
        V[k] = ACG::Vec3f(p->pos[0], p->pos[1], p->pos[2]);
      }

      ACG::Vec3f u = V[1] - V[0];
      ACG::Vec3f v = V[2] - V[0];
      ACG::Vec3f n = OpenMesh::cross(u, v).normalize_cond();

      // store face normal in first vertex
      for (unsigned int k = 0; k < 3; ++k)
      {
        m_pVerticesTmp[m_pIndices[i*3+2]].n[k] = n[k];
      }
    }

    glBufferDataARB(GL_ARRAY_BUFFER_ARB, m_NumVerts * sizeof(Vertex), m_pVerticesTmp, GL_STATIC_DRAW_ARB);

    m_bVBOinFlatMode = 1;
  }

  glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
}

template <class Mesh>
void
DrawMeshT<Mesh>::createIBO()
{
  // data read from m_pIndices

  // create triangle index buffer

  if (!m_IBO)
    glGenBuffersARB(1, &m_IBO);
  
if(0)  {
    FILE* pFile = fopen("ibo_dm.dat", "wb");
    fwrite(m_pIndices, m_NumTris * 4 * 3, 1, pFile);
    fclose(pFile);

    pFile = fopen("mesh_info_dm.txt", "wt");
    fprintf(pFile, "#faces: %d\n#tris: %d\n#verts: %d\n#dmverts: %d", m_Mesh.n_faces(), m_NumTris, m_Mesh.n_vertices(), m_NumVerts);
    fclose(pFile);
  }
  glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, m_IBO);

if(1){

  m_IndexType = GL_UNSIGNED_INT;
  if (m_NumVerts <= 0xFFFF)
  {
    // use 2 byte indices
    unsigned short* pwIndices = (unsigned short*)m_pIndicesTmp;
    m_IndexType = GL_UNSIGNED_SHORT;

    for (unsigned int i = 0; i < m_NumTris * 3; ++i)
      pwIndices[i] = (unsigned short)m_pIndices[i];

    glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, m_NumTris * 3 * sizeof(unsigned short), pwIndices, GL_STATIC_DRAW_ARB);
  }
  else
    glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, m_NumTris * 3 * sizeof(unsigned int), m_pIndices, GL_STATIC_DRAW_ARB);
}
else
{
  FILE* pFile = fopen("ibo.dat", "rb");
  fseek(pFile, 0, SEEK_END);
  m_NumTris = ftell(pFile) / (3 * 4);
  fseek(pFile, 0, SEEK_SET);

  UINT* pTmp = new UINT[m_NumTris * 3];

  fread(pTmp, m_NumTris * 3 * 4, 1, pFile);

  m_IndexType = GL_UNSIGNED_INT;

  glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, m_NumTris * 3 * 4, pTmp, GL_STATIC_DRAW_ARB);

  delete [] pTmp;

}


  // line index buffer:
  if (m_Mesh.n_edges())
  {
    unsigned int* pLineBuffer = new unsigned int[m_Mesh.n_edges() * 2];

    if (!m_LineIBO)
      glGenBuffersARB(1, &m_LineIBO);

    for (unsigned int i = 0; i < m_Mesh.n_edges(); ++i)
    {
      OpenMesh::HalfedgeHandle hh = m_Mesh.halfedge_handle(m_Mesh.edge_handle(i), 0);

      if (m_IndexType == GL_UNSIGNED_SHORT)
      {
        ((unsigned short*)pLineBuffer)[2*i] = (unsigned short)m_pInvVertexMap[m_Mesh.from_vertex_handle(hh).idx()];
        ((unsigned short*)pLineBuffer)[2*i+1] = (unsigned short)m_pInvVertexMap[m_Mesh.to_vertex_handle(hh).idx()];
      }
      else
      {
        pLineBuffer[2*i] = m_pInvVertexMap[m_Mesh.from_vertex_handle(hh).idx()];
        pLineBuffer[2*i+1] = m_pInvVertexMap[m_Mesh.to_vertex_handle(hh).idx()];
      }
    }

    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, m_LineIBO);

    // 2 or 4 byte indices:
    if (m_IndexType == GL_UNSIGNED_SHORT)
      glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 
                   m_Mesh.n_edges() * 2 * sizeof(unsigned short), 
                   pLineBuffer, GL_STATIC_DRAW_ARB);
    else
      glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 
                   m_Mesh.n_edges() * 2 * sizeof(unsigned int),
                   pLineBuffer, GL_STATIC_DRAW_ARB);

    delete [] pLineBuffer;
  }

  glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
}

template <class Mesh>
DrawMeshT<Mesh>::~DrawMeshT(void)
{
  delete [] m_pIndices;
  delete [] m_pIndicesTmp;
  delete [] m_pVertices;
  delete [] m_pVerticesTmp;
//  delete [] m_pVerticesC;

  delete [] m_pSubsets;

  delete [] m_pVertexMap;

  delete [] m_pTriToFaceMap;

  delete [] m_pInvVertexMap;

  if (m_VBO) glDeleteBuffersARB(1, &m_VBO);
  if (m_IBO) glDeleteBuffersARB(1, &m_IBO);
  if (m_LineIBO) glDeleteBuffersARB(1, &m_LineIBO);
}

template <class Mesh>
void DrawMeshT<Mesh>::bindBuffers()
{
  // rebuild if necessary
  if (!m_NumTris || ! m_NumVerts || !m_pSubsets) m_Rebuild = REBUILD_FULL;

  // if no rebuild necessary, check for smooth / flat shading switch 
  // to update normals
  if (m_Rebuild == REBUILD_NONE)
  {
    if (m_bVBOinFlatMode != m_bFlatMode)
      createVBO();
  }
  else
    rebuild();

  glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_VBO);

  // prepare color mode
  if (m_iColorMode)
  {
    if (m_iColorMode == 1)
      glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex), (char*)offsetof(Vertex, vcol));
    else
      glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex), (char*)offsetof(Vertex, fcol));

    glEnableClientState(GL_COLOR_ARRAY);
  }

  // vertex decl
  glVertexPointer(3, GL_FLOAT, sizeof(Vertex), (char*)offsetof(Vertex, pos));
  glEnableClientState(GL_VERTEX_ARRAY);

  glClientActiveTexture(GL_TEXTURE0);
  glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), (char*)offsetof(Vertex, tex));
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);

  glNormalPointer(GL_FLOAT, sizeof(Vertex), (char*)offsetof(Vertex, n));
  glEnableClientState(GL_NORMAL_ARRAY);

//  glNormalPointerEXT(3, GL_FLOAT, sizeof(Vertex), (char*)(20));  // glNormalPointerEXT crashes sth. in OpenGL
//  glTangentPointerEXT(4, GL_FLOAT, sizeof(Vertex), (void*)(32));

  //  glEnableClientState(GL_TANGENT_ARRAY_EXT);

  glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, m_IBO);
}

template <class Mesh>
void DrawMeshT<Mesh>::unbindBuffers()
{
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);
  //  glDisableClientState(GL_TANGENT_ARRAY_EXT);

  if (m_iColorMode)
    glDisableClientState(GL_COLOR_ARRAY);

  glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
  glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
}

template <class Mesh>
void DrawMeshT<Mesh>::draw(std::map< int, GLuint>* _textureMap)
{
  bindBuffers();

  if (_textureMap)
  {
    // textured mode

    for (unsigned int i = 0; i < m_NumSubsets; ++i)
    {
      Subset* pSubset = m_pSubsets + i;

      if ( _textureMap->find(pSubset->MaterialID) == _textureMap->end() ) {
        std::cerr << "Illegal texture index ... trying to access " << pSubset->MaterialID << std::endl;
        glBindTexture(GL_TEXTURE_2D, 0);
      }
      else
        glBindTexture(GL_TEXTURE_2D, (*_textureMap)[pSubset->MaterialID]);

      glDrawElements(GL_TRIANGLES, pSubset->NumTris * 3, m_IndexType,
        (char*)( pSubset->StartIndex * (m_IndexType == GL_UNSIGNED_INT ? 4 : 2))); // offset in bytes
    }
  }
  else
    glDrawElements(GL_TRIANGLES, m_NumTris * 3, m_IndexType, 0);

  unbindBuffers();
}

template <class Mesh>
void DrawMeshT<Mesh>::drawLines()
{
  bindBuffers();

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_LineIBO);

  glDrawElements(GL_LINES, m_Mesh.n_edges() * 2, m_IndexType, 0);

  unbindBuffers();
}

template <class Mesh>
void DrawMeshT<Mesh>::drawVertices()
{
  bindBuffers();

  glDrawArrays(GL_POINTS, 0, m_NumVerts);

  unbindBuffers();
}

template <class Mesh>
void DrawMeshT<Mesh>::removeIsolatedVerts()
{
  if (!m_NumVerts || !m_NumTris) return;

  unsigned char* pVertexUsed = new unsigned char[m_NumVerts];
  memset(pVertexUsed, 0, m_NumVerts);

  for (unsigned int i = 0; i < m_NumTris * 3; ++i)
    pVertexUsed[m_pIndices[i]] = 1;

  unsigned int NewNumVerts = 0;

  for (unsigned int i = 0; i < m_NumVerts; ++i)
  {
    if (pVertexUsed[i])
      ++NewNumVerts;
  }


  if (NewNumVerts == m_NumVerts) // early out
  {
    delete [] pVertexUsed;
    return;
  }


  memcpy(m_pVerticesTmp, m_pVertices, m_NumVerts * sizeof(Vertex));

  // remap vertices, track changes

  unsigned int* pRemap = new unsigned int[m_NumVerts]; // remap[old] = new index

  unsigned int Counter = 0;
  for (unsigned int i = 0; i < m_NumVerts; ++i)
  {
    if (pVertexUsed[i])
      pRemap[i] = Counter++;
    else
      pRemap[i] = NewNumVerts;
  }


  delete [] pRemap;
  delete [] pVertexUsed;
}

//////////////////////////////////////////////////////////////////////////

template <class Mesh>
unsigned int DrawMeshT<Mesh>::countTris(unsigned int* pMaxVertsOut)
{
  unsigned int TriCounter = 0;

  if (pMaxVertsOut) *pMaxVertsOut = 0;

  for (unsigned int i = 0; i < m_Mesh.n_faces(); ++i)
  {
    typename Mesh::FaceHandle fh =  m_Mesh.face_handle(i);

    // count vertices
  	unsigned int nPolyVerts = 0;

    for (typename Mesh::FaceHalfedgeIter hh_it = m_Mesh.fh_iter(fh); hh_it; ++hh_it )++nPolyVerts;

    TriCounter += (nPolyVerts - 2);

    if (pMaxVertsOut)
    {
      if (*pMaxVertsOut < nPolyVerts)
        *pMaxVertsOut = nPolyVerts;
    }
  }

  return TriCounter;
}



template <class Mesh>
ACG::Vec3f* DrawMeshT<Mesh>::perEdgeVertexBuffer()
{
  // Force update of the buffers if required
  if (m_bUpdatePerEdgeBuffers)
    updatePerEdgeBuffers();
  return &(m_PerEdgeVertexBuf)[0]; 
}

template <class Mesh>
ACG::Vec4f* DrawMeshT<Mesh>::perEdgeColorBuffer()
{
  // Force update of the buffers if required
  if (m_bUpdatePerEdgeBuffers)
    updatePerEdgeBuffers();
  return &(m_PerEdgeColorBuf[0]); 
}


template <class Mesh>
void DrawMeshT<Mesh>::updatePickingVertices(ACG::GLState& _state,
                                            uint _offset)
{
  unsigned int idx = 0;

  // Adjust size of the color buffer to the number of vertices in the mesh
  m_PickVertBuf.resize( m_Mesh.n_vertices() );

  // Get the right picking colors from the gl state and add them per vertex to the color buffer
  typename Mesh::ConstVertexIter v_it(m_Mesh.vertices_begin()), v_end(m_Mesh.vertices_end());
  for (; v_it!=v_end; ++v_it, ++idx) 
    m_PickVertBuf[idx] = _state.pick_get_name_color(idx + _offset);
}



template <class Mesh>
void DrawMeshT<Mesh>::updatePerEdgeBuffers()
{
  // Only update buffers if they are invalid
  if (!m_bUpdatePerEdgeBuffers) 
    return;

  m_PerEdgeVertexBuf.resize(m_Mesh.n_edges() * 2);

  if ( m_Mesh.has_edge_colors() ) {
    m_PerEdgeColorBuf.resize(m_Mesh.n_edges() * 2);
  } else
    m_PerEdgeColorBuf.clear();    

  unsigned int idx = 0;

  typename Mesh::ConstEdgeIter  e_it(m_Mesh.edges_sbegin()), e_end(m_Mesh.edges_end());
  for (; e_it!=e_end; ++e_it) {

    m_PerEdgeVertexBuf[idx]   = m_Mesh.point(m_Mesh.to_vertex_handle(m_Mesh.halfedge_handle(e_it, 0)));
    m_PerEdgeVertexBuf[idx+1] = m_Mesh.point(m_Mesh.to_vertex_handle(m_Mesh.halfedge_handle(e_it, 1)));

    if (  m_Mesh.has_edge_colors() ) {
      const Vec4f color = OpenMesh::color_cast<Vec4f>( m_Mesh.color(e_it) ) ;
      m_PerEdgeColorBuf[ idx ]     = color;
      m_PerEdgeColorBuf[ idx + 1 ] = color;
    }

    idx += 2;
  }

  m_bUpdatePerEdgeBuffers = 0;
}

template <class Mesh>
void DrawMeshT<Mesh>::updatePerHalfedgeBuffers()
{
  // Only update buffers if they are invalid
  if (!m_bUpdatePerHalfedgeBuffers) 
    return;

  m_PerHalfedgeVertexBuf.resize(m_Mesh.n_halfedges() * 2);

  if ( m_Mesh.has_halfedge_colors() ) {
    m_PerHalfedgeColorBuf.resize(m_Mesh.n_halfedges() * 2);
  } else
    m_PerHalfedgeColorBuf.clear();    

  unsigned int idx = 0;

  typename Mesh::ConstHalfedgeIter  he_it(m_Mesh.halfedges_sbegin()), he_end(m_Mesh.halfedges_end());
  for (; he_it!=he_end; ++he_it) {

    m_PerHalfedgeVertexBuf[idx]   = halfedge_point(he_it);
    m_PerHalfedgeVertexBuf[idx+1] = halfedge_point(m_Mesh.prev_halfedge_handle(he_it));

    if (  m_Mesh.has_halfedge_colors() ) {
      const Vec4f color = OpenMesh::color_cast<Vec4f>( m_Mesh.color(he_it) ) ;
      m_PerHalfedgeColorBuf[ idx ]     = color;
      m_PerHalfedgeColorBuf[ idx + 1 ] = color;
    }

    idx += 2;
  }

  m_bUpdatePerHalfedgeBuffers = 1;
}

template <class Mesh>
typename Mesh::Point
DrawMeshT<Mesh>::
halfedge_point(const typename Mesh::HalfedgeHandle _heh) {

  typename Mesh::Point p  = m_Mesh.point(m_Mesh.to_vertex_handle  (_heh));
  typename Mesh::Point pp = m_Mesh.point(m_Mesh.from_vertex_handle(_heh));
  typename Mesh::Point pn = m_Mesh.point(m_Mesh.to_vertex_handle(m_Mesh.next_halfedge_handle(_heh)));

  //  typename Mesh::Point n  = (p-pp)%(pn-p);
  typename Mesh::Point fn;
  if( !m_Mesh.is_boundary(_heh))
    fn = m_Mesh.normal(m_Mesh.face_handle(_heh));
  else
    fn = m_Mesh.normal(m_Mesh.face_handle(m_Mesh.opposite_halfedge_handle(_heh)));

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
  if (m_bUpdatePerHalfedgeBuffers)
    updatePerHalfedgeBuffers();
  return &(m_PerHalfedgeVertexBuf)[0]; 
}

template <class Mesh>
ACG::Vec4f* DrawMeshT<Mesh>::perHalfedgeColorBuffer()
{
  // Force update of the buffers if required
  if (m_bUpdatePerHalfedgeBuffers)
    updatePerHalfedgeBuffers();
  return &(m_PerHalfedgeColorBuf)[0]; 
}




template <class Mesh>
void DrawMeshT<Mesh>::updatePickingEdges(ACG::GLState& _state,
                                         unsigned int _offset)
{
  updatePerEdgeBuffers();

  m_PickEdgeBuf.resize(m_Mesh.n_edges() * 2);


  int idx = 0;

  typename Mesh::ConstEdgeIter  e_it(m_Mesh.edges_sbegin()), e_end(m_Mesh.edges_end());
  for (; e_it!=e_end; ++e_it) {

    const Vec4uc pickColor =  _state.pick_get_name_color (e_it.handle().idx() + _offset);

    m_PickEdgeBuf[idx]    = pickColor;
    m_PickEdgeBuf[idx+1]  = pickColor;

    idx += 2;
  }
}









template <class Mesh>
void DrawMeshT<Mesh>::updatePickingFaces(ACG::GLState& _state )
{
  // Make sure, the face buffers are up to date before generating the picking data!
//  updatePerFaceBuffers();

  m_PickFaceVertexBuf.resize(3 * m_NumTris);
  m_PickFaceColBuf.resize(3 * m_NumTris);

  // Index to the current buffer position
  unsigned int bufferIndex = 0;

  for (unsigned int i = 0; i < m_NumTris; ++i)
  {
    const Vec4uc pickColor = _state.pick_get_name_color ( m_pTriToFaceMap[i] );
    for (unsigned int k = 0; k < 3; ++k)
    {
      m_PickFaceVertexBuf[i * 3 + k] = m_Mesh.point(
        m_Mesh.to_vertex_handle(m_Mesh.halfedge_handle(m_pVertexMap[m_pIndices[i * 3 + k]])));

      m_PickFaceColBuf[i * 3 + k] = pickColor;
    }
  }


}


template <class Mesh>
void DrawMeshT<Mesh>::updatePickingAny(ACG::GLState& _state )
{
  // stripify()

  updatePickingFaces(_state);
  updatePickingEdges(_state, m_Mesh.n_faces());
  updatePickingVertices(_state, m_Mesh.n_faces() + m_Mesh.n_edges());
}


template <class Mesh>
void
DrawMeshT<Mesh>::
setTextureIndexPropertyName( std::string _indexPropertyName ) {

  // Check if the given property exists
  OpenMesh::FPropHandleT< int > textureIndexProperty;
  if ( !m_Mesh.get_property_handle(textureIndexProperty,_indexPropertyName) )  {
    std::cerr << "StripProcessor: Unable to get per face texture Index property named " << _indexPropertyName << std::endl;
    return;
  }

  // Remember the property name
  textureIndexPropertyName_ = _indexPropertyName;

  // mark strips as invalid ( have to be regenerated to collect texture index information)
//   stripsValid_ = false;
// 
//   // mark the buffers as invalid as we have a new per face index array
//   invalidatePerFaceBuffers();
  m_Rebuild |= REBUILD_TOPOLOGY;
};

template <class Mesh>
void
DrawMeshT<Mesh>::
setPerFaceTextureCoordinatePropertyName( std::string _perFaceTextureCoordinatePropertyName ) {

  // Check if the given property exists
  OpenMesh::HPropHandleT< typename Mesh::TexCoord2D >  perFaceTextureCoordinateProperty;
  if ( !m_Mesh.get_property_handle(perFaceTextureCoordinateProperty,_perFaceTextureCoordinatePropertyName) )  {
    std::cerr << "StripProcessor: Unable to get per face texture coordinate property named " << _perFaceTextureCoordinatePropertyName << std::endl;
    return;
  }

  // Remember the property name
  perFaceTextureCoordinatePropertyName_ = _perFaceTextureCoordinatePropertyName;

  // mark the buffers as invalid as we have a new per face index array
//  invalidatePerFaceBuffers();
  m_Rebuild |= REBUILD_GEOMETRY;
}


template <class Mesh>
unsigned int DrawMeshT<Mesh>::getNumTextures()
{
  unsigned int n = 0;
  for (unsigned int i = 0; i < m_NumSubsets; ++i)
  {
    if (m_pSubsets[i].MaterialID > 0) ++n;
  }
  return n;
}

template <class Mesh>
int 
DrawMeshT<Mesh>::perFaceTextureCoordinateAvailable()  {

  // We really have to recheck, as the property might get lost externally (e.g. on restores of the mesh)
  OpenMesh::HPropHandleT< typename Mesh::TexCoord2D >  perFaceTextureCoordinateProperty;
  if ( !m_Mesh.get_property_handle(perFaceTextureCoordinateProperty, perFaceTextureCoordinatePropertyName_) )  {
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
  if ( !m_Mesh.get_property_handle(textureIndexProperty, textureIndexPropertyName_) )  {
    return false;
  }

  // Property available
  return true;
}

}
