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

#define ACG_DRAW_MESH_C

//=============================================================================

#include "DrawMesh.hh"
#include <ACG/Geometry/GPUCacheOptimizer.hh>
#include <ACG/GL/VertexDeclaration.hh>
#include <cassert>
#include <cmath>
#include <vector>
#include <map>
#include <cstring>
#include <fstream>

#ifdef USE_OPENMP
#include <omp.h>
#endif

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

template <class Mesh>
DrawMeshT<Mesh>::DrawMeshT(Mesh& _mesh)
:  mesh_(_mesh),
   meshComp_(0),
   numTris_(0), numVerts_(0),
   vertices_(0),
   rebuild_(REBUILD_NONE),
   prevNumFaces_(0), prevNumVerts_(0),
   vbo_(0), ibo_(0),
   lineIBO_(0),
   indexType_(0),
   colorMode_(1),
   flatMode_(0), bVBOinFlatMode_(0),
   textureMode_(1), bVBOinHalfedgeTexMode_(1),
   halfedgeNormalMode_(0), bVBOinHalfedgeNormalMode_(0),
   invVertexMap_(0),
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


/*
template<class Mesh>
template<class Prop>
void DrawMeshT<Mesh>::getMeshPropertyType( Prop _propData, GLuint* _outType, int* _outSize, int* _outStride )
{
  std::string tid = typeid( _propData ).name();

  GLuint fmt = 0;
  int size = 0;

  if (sscanf(tid.c_str(), "class OpenMesh::VectorT<float,%i> const *", &size) == 1)
    fmt = GL_FLOAT;
  else if (sscanf(tid.c_str(), "class OpenMesh::VectorT<double,%i> const *", &size) == 1)
    fmt = GL_DOUBLE;
  else if (sscanf(tid.c_str(), "class OpenMesh::VectorT<int,%i> const *", &size) == 1)
    fmt = GL_INT;
  else if (sscanf(tid.c_str(), "class OpenMesh::VectorT<unsigned int,%i> const *", &size) == 1)
    fmt = GL_UNSIGNED_INT;
  else if (!strcmp(tid.c_str(), "int const *"))
  {
    fmt = GL_INT;
    size = 1;
  }
  else
  {
    std::cerr << "DrawMesh: unknown typeid of property: " << tid << std::endl;

    // random guess
    fmt = GL_DOUBLE;
    size = 3;
  }

  if (_outType)
    *_outType = fmt;

  if (_outSize)
    *_outSize = size;

  if (_outStride)
    *_outStride = (fmt == GL_DOUBLE ? 8 : 4) * size;
}
*/


template<class Mesh>
class DrawMeshFaceInput : public MeshCompilerFaceInput
{

public:

  DrawMeshFaceInput(Mesh& _mesh) :
  mesh_(_mesh)
  {
  };

  // map[attrID] -> use per halfedge/vertex attribute (true/false)
  std::vector<int> attributeStoredPerHalfedge_;


  int getNumFaces() const { return mesh_.n_faces(); }

  // compute number of indices later automatically
  int getNumIndices() const { return 0; };

  /** Get number of vertices per face.
   * @param _faceID face index
  */
  int getFaceSize(const int _faceID) const
  {
    return mesh_.valence( mesh_.face_handle(_faceID) );
  }

  /** Get a single vertex-index entry of a face.
   *
   * @param _faceID face index
   * @param _faceCorner vertex corner of the face
   * @param _attrID attribute channel 
   * @return index-data if successful, -1 otherwise
  */
  int getSingleFaceAttr(const int _faceID, const int _faceCorner, const int _attrID) const
  {
    const typename Mesh::FaceHandle fh =  mesh_.face_handle(_faceID);

    typename Mesh::ConstFaceHalfedgeIter fh_it = mesh_.cfh_iter(fh);

    // ========================================================================================
    // Optimization for triangular meshes (use previous halfedge to skip part of the iteration)
    // ========================================================================================
    if ( mesh_.is_trimesh()) {
      if ( attributeStoredPerHalfedge_[_attrID] != 0 ) {
        switch (_faceCorner) {
          case 0 : return fh_it->idx();
          break;
          case 1 : return (mesh_.next_halfedge_handle(*fh_it)).idx();
          break;
          case 2 : return (mesh_.prev_halfedge_handle(*fh_it)).idx();
          break;
          default :   std::cerr << " Index error!" << _faceCorner << std::endl; return -1;
          break;
        }
      } else {
        switch (_faceCorner) {
          case 0 : return mesh_.to_vertex_handle(*fh_it).idx();
          break;
          case 1 : return (mesh_.to_vertex_handle(mesh_.next_halfedge_handle(*fh_it))).idx();
          break;
          case 2 : return (mesh_.to_vertex_handle(mesh_.prev_halfedge_handle(*fh_it))).idx();
          break;
          default :   std::cerr << " Index error!" << _faceCorner << std::endl; return -1;
          break;
        }
      }
    }

    // ========================================================================================
    // Poly Meshes
    // ========================================================================================
    if ( attributeStoredPerHalfedge_[_attrID] != 0 ) {

      for (int i = 0; fh_it.is_valid() && i <= _faceCorner; ++fh_it, ++i )
        if (i == _faceCorner)
          return fh_it->idx();

    } else {

      for (int i = 0; fh_it.is_valid() && i <= _faceCorner; ++fh_it, ++i )
        if (i == _faceCorner)
          return mesh_.to_vertex_handle(*fh_it).idx();

    }

    // Not found -> return -1 as error
    return -1;
  }

  /** Get an index buffer of a face for a specific attribute channel.
   * @param _faceID face index
   * @param _attrID attribute channel
   * @param _out pointer to output buffer, use getFaceSize(_faceID) to get the size needed to store face data
   * @return true if successful, false otherwise
  */
  bool getFaceAttr(const int _faceID, const int _attrID, int* _out) const
  {
    const typename Mesh::FaceHandle fh =  mesh_.face_handle(_faceID);

    const bool usePerHalfedge = (attributeStoredPerHalfedge_[_attrID] != 0);

    // read all vertex indices of this face
    typename Mesh::ConstFaceHalfedgeIter hh_it = mesh_.cfh_iter(fh);
    for (int i = 0; hh_it.is_valid(); ++hh_it, ++i )
    {
      _out[i] = usePerHalfedge ? hh_it->idx() : mesh_.to_vertex_handle(*hh_it).idx();
    }

    return true;
  }

  /** Get an index buffer of a face for a specific attribute channel.
   * @param _faceID face index
   * @param _attrID attribute channel
   * @return array data of size "getFaceSize(_faceID)", allowed to return 0 when array data not permanently available in memory
  */
  int* getFaceAttr(const int _faceID, const int _attrID) const
  {
    // cannot be implemented with OpenMesh data structure
    return 0;
  }



  int getVertexAdjCount(const int _vertexID) const
  {
    const typename Mesh::VertexHandle vh =  mesh_.vertex_handle(_vertexID);

    int counter = 0;

    // read all vertex indices of this face
    typename Mesh::ConstVertexFaceIter adj_it = mesh_.cvf_iter(vh);
    for (;adj_it.is_valid(); ++adj_it)
      ++counter;

    return counter;
  }

  int getVertexAdjFace(const int _vertexID, const int _k) const
  {
    const typename Mesh::VertexHandle vh =  mesh_.vertex_handle(_vertexID);

    // read all vertex indices of this face
    typename Mesh::ConstVertexFaceIter adj_it = mesh_.cvf_iter(vh);
    for (int i = 0; adj_it.is_valid() && i < _k; ++adj_it, ++i);

    return adj_it->idx();
  }

private:
  Mesh& mesh_;
};



template <class Mesh>
void
DrawMeshT<Mesh>::rebuild()
{
  if (rebuild_ == REBUILD_NONE) return;

  if (!mesh_.n_vertices())
  {
    numVerts_ = 0;
    numTris_ = 0;
    return;
  }

  // support for point clouds:
  if (mesh_.n_vertices() && mesh_.n_faces() == 0)
  {
    if (mesh_.n_vertices() > numVerts_)
    {
      delete [] invVertexMap_; invVertexMap_ = 0;
      delete [] vertices_; vertices_ = 0;
      delete [] verticesTmp_; verticesTmp_ = 0;

      numVerts_ = mesh_.n_vertices();
      vertices_ = new Vertex[numVerts_];
      verticesTmp_ = new Vertex[numVerts_];
    }
    numVerts_ = mesh_.n_vertices();

    // read all vertices
    for (size_t i = 0; i < numVerts_; ++i)
      readVertex(vertices_ + i,
                 mesh_.vertex_handle(i), 
                 (typename Mesh::HalfedgeHandle)(-1), 
                 (typename Mesh::FaceHandle)(-1));

    createVBO();
    rebuild_ = REBUILD_NONE;
    return;
  }



  unsigned int maxFaceVertCount = 0;
  unsigned int numIndices = 0;
  unsigned int newTriCount = countTris(&maxFaceVertCount, &numIndices);
  
  int bTriangleRebuild = 0; // what should be rebuild?
  int bVertexRebuild = 0;

  if (newTriCount > numTris_)
  {
    // index buffer too small
    if (ibo_) glDeleteBuffers(1, &ibo_);
    ibo_ = 0;

    numTris_ = newTriCount;

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

  // support faster update by only updating vertices (do a complete update if the textures have to be rebuild)
  if (!bTriangleRebuild && !bVertexRebuild && (rebuild_ & REBUILD_GEOMETRY) && !(rebuild_ & REBUILD_TEXTURES))
  {
    // only update vertices, i.e. update values of vertices

    #ifndef WIN32
      #ifdef USE_OPENMP
        #pragma omp parallel for
      #endif
    #endif
    for (unsigned int i = 0; i < numVerts_; ++i)
    {
      // just pick one face, srews up face colors here so color updates need a full rebuild
      const typename Mesh::HalfedgeHandle hh = mapToHalfedgeHandle(i);
      typename Mesh::VertexHandle   vh(-1);
      typename Mesh::FaceHandle     fh(-1);

      if (hh.is_valid())
      {
        vh = mesh_.to_vertex_handle(hh);
        fh = mesh_.face_handle(hh);
      }
      else
      {
        int f_id, c_id;
        int posID = meshComp_->mapToOriginalVertexID(i, f_id, c_id);
        vh = mesh_.vertex_handle(posID);
      }

      readVertex(vertices_ + i, vh, hh, fh);
    }

    createVBO();

    rebuild_ = REBUILD_NONE;
    return;
  }

  // full rebuild:
  delete meshComp_;
  meshComp_ = new MeshCompiler(*vertexDeclFCol_);


  // search for convenient attribute indices
  int attrIDNorm = -1, attrIDPos = -1, attrIDTexC = -1;

  for (int i = 0; i < (int)meshComp_->getVertexDeclaration()->getNumElements(); ++i)
  {
    const VertexElement* e = meshComp_->getVertexDeclaration()->getElement(i);

    switch (e->usage_)
    {
    case VERTEX_USAGE_POSITION: attrIDPos = i; break;
    case VERTEX_USAGE_NORMAL: attrIDNorm = i; break;
    case VERTEX_USAGE_TEXCOORD: attrIDTexC = i; break;
    default: break;
    }
  }


  // pass face data to mesh compiler
  DrawMeshFaceInput<Mesh>* faceInput = new DrawMeshFaceInput<Mesh>(mesh_);
  faceInput->attributeStoredPerHalfedge_.resize(meshComp_->getVertexDeclaration()->getNumElements(), 0);
  faceInput->attributeStoredPerHalfedge_[attrIDPos]  = 0;
  faceInput->attributeStoredPerHalfedge_[attrIDNorm] = ( (halfedgeNormalMode_ && mesh_.has_halfedge_normals()) ? 1 : 0 );
  faceInput->attributeStoredPerHalfedge_[attrIDTexC] = ( mesh_.has_halfedge_texcoords2D() ? 1 : 0);

  meshComp_->setFaceInput(faceInput);

  // set textures
  for (int i = 0; i < mesh_.n_faces(); ++i)
    meshComp_->setFaceGroup(i, getTextureIDofFace(i));

  // pass vertex data to mesh compiler


  // points
  meshComp_->setVertices(mesh_.n_vertices(), mesh_.points(), 24, false, GL_DOUBLE, 3);
  
  // normals
  if (halfedgeNormalMode_ == 0 && mesh_.has_vertex_normals())
    meshComp_->setNormals(mesh_.n_vertices(), mesh_.vertex_normals(), 24, false, GL_DOUBLE, 3);
  else if (halfedgeNormalMode_ &&  mesh_.has_halfedge_normals())
    meshComp_->setNormals(mesh_.n_halfedges(), mesh_.property(mesh_.halfedge_normals_pph()).data(), 24, false, GL_DOUBLE, 3);

  if (mesh_.has_halfedge_texcoords2D())
    meshComp_->setTexCoords(mesh_.n_halfedges(), mesh_.htexcoords2D(), 8, false, GL_FLOAT, 2);


/*
  setMeshCompilerInput(attrIDPos, mesh_.points(), mesh_.n_vertices());
  
  // normals
  if (halfedgeNormalMode_ == 0 && mesh_.has_vertex_normals())
    setMeshCompilerInput(attrIDNorm, mesh_.vertex_normals(), mesh_.n_vertices());
  else if (halfedgeNormalMode_ &&  mesh_.has_halfedge_normals())
    setMeshCompilerInput(attrIDNorm, mesh_.property(mesh_.halfedge_normals_pph()).data(), mesh_.n_halfedges());

  if (mesh_.has_halfedge_texcoords2D())
  {
    setMeshCompilerInput(attrIDTexC, mesh_.htexcoords2D(), mesh_.n_halfedges());
// 
//     GLuint fmtN = 0;
//     int sizeN = 0, strideN = 0;
//     getMeshPropertyType(mesh_.htexcoords2D(), &fmtN, &sizeN, &strideN);
// 
//     meshComp_->setTexCoords(mesh_.n_halfedges(), mesh_.htexcoords2D(), strideN, false, fmtN, sizeN);
  }
*/

  meshComp_->build(true, true, true, true);


  // create inverse vertex map
  for (int i = 0; i < (int)mesh_.n_faces(); ++i)
  {
    typename Mesh::FaceHandle fh =  mesh_.face_handle(i);

    int corner = 0;

    for (typename Mesh::FaceHalfedgeIter hh_it = mesh_.fh_iter(fh); hh_it.is_valid(); ++hh_it )
    {
      int vertexId = mesh_.to_vertex_handle(*hh_it).idx();
      invVertexMap_[vertexId] = meshComp_->mapToDrawVertexID(i, corner++);
    }
  }


  // get vertex buffer
  numTris_ = meshComp_->getNumTriangles();
  numVerts_ = meshComp_->getNumVertices();

  delete [] vertices_;
  vertices_ = new Vertex[numVerts_];
  meshComp_->getVertexBuffer(vertices_);

  // copy colors
  for (int i = 0; i < (int)numVerts_; ++i)
  {
    typename Mesh::HalfedgeHandle hh = mapToHalfedgeHandle(i);

    if (hh.is_valid())
    {
      vertices_[i].vcol = getVertexColor(mesh_.to_vertex_handle(hh));
      vertices_[i].fcol = getFaceColor(mesh_.face_handle(hh));
    }
    else
    {
      // isolated vertex
      int f_id, c_id;
      int posID = meshComp_->mapToOriginalVertexID(i, f_id, c_id);
      vertices_[i].vcol = getVertexColor( mesh_.vertex_handle(posID) );
      vertices_[i].fcol = 0;
    }
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
void
DrawMeshT<Mesh>::readVertex(Vertex* _pV,
                            const typename Mesh::VertexHandle _vh,
                            const typename Mesh::HalfedgeHandle _hh,
                            const typename Mesh::FaceHandle _fh)
{
  static const typename Mesh::HalfedgeHandle invalidHEH(-1);
  static const typename Mesh::FaceHandle     invalidFH(-1);

  for (int m = 0; m < 3; ++m)
  {
    _pV->pos[m] = mesh_.point(_vh)[m];

    if (halfedgeNormalMode_ == 0 && mesh_.has_vertex_normals())
      _pV->n[m] = mesh_.normal(_vh)[m];
    else if (halfedgeNormalMode_ && 
      mesh_.has_halfedge_normals() && _hh != invalidHEH)
      _pV->n[m] = mesh_.normal(_hh)[m];
    else _pV->n[m] = 0.0f;

    if (m < 2)
    {
      if (mesh_.has_halfedge_texcoords2D())
      {
        if (_hh != invalidHEH)
          _pV->tex[m] = mesh_.texcoord2D(_hh)[m];
      }
      else _pV->tex[m] = 0.0f;
    }
  }
  
  // per face & per vertex color
  unsigned int byteCol[2];
  for (int col = 0; col < 2; ++col)
  {
    Vec4uc vecCol(255, 255, 255, 255);

    if (col == 0 && mesh_.has_vertex_colors()) vecCol = OpenMesh::color_cast<Vec4uc, typename Mesh::Color>(mesh_.color(_vh));
    if (_fh != invalidFH)
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
unsigned int
DrawMeshT<Mesh>::getVertexColor(const typename Mesh::VertexHandle _vh)
{
  static const typename Mesh::VertexHandle     invalidVH(-1);

  unsigned int byteCol;
  
  Vec4uc vecCol(255, 255, 255, 255);

  if ( _vh != invalidVH && mesh_.has_vertex_colors() )
    vecCol = OpenMesh::color_cast<Vec4uc, typename Mesh::Color>(mesh_.color(_vh));

  // OpenGL color format: A8B8G8R8
  byteCol  = (unsigned char)(vecCol[0]);
  byteCol |= ((unsigned char)(vecCol[1])) << 8;
  byteCol |= ((unsigned char)(vecCol[2])) << 16;
  byteCol |= ((unsigned char)(vecCol[3])) << 24;

  return byteCol;
}

template <class Mesh>
unsigned int
DrawMeshT<Mesh>::getFaceColor(const typename Mesh::FaceHandle _fh)
{
  static const typename Mesh::FaceHandle     invalidFH(-1);

  unsigned int byteCol;
  Vec4uc vecCol(255, 255, 255, 255);

  if ( _fh != invalidFH && mesh_.has_face_colors() && _fh.idx() >= 0 ) 
    vecCol = OpenMesh::color_cast<Vec4uc,typename Mesh::Color>(mesh_.color(_fh));

  // OpenGL color format: A8B8G8R8
  byteCol  = (unsigned char)(vecCol[0]);
  byteCol |= ((unsigned char)(vecCol[1])) << 8;
  byteCol |= ((unsigned char)(vecCol[2])) << 16;
  byteCol |= ((unsigned char)(vecCol[3])) << 24;

  return byteCol;
}


template <class Mesh>
int
DrawMeshT<Mesh>::getTextureIDofFace(unsigned int _face)
{
  OpenMesh::FPropHandleT< int > textureIndexProperty;
  if (mesh_.get_property_handle(textureIndexProperty, textureIndexPropertyName_))
    return mesh_.property(textureIndexProperty, mesh_.face_handle(_face));

  if (mesh_.has_face_texture_index())
    return mesh_.texture_index(mesh_.face_handle(_face));

  return 0;
}

template <class Mesh>
int
DrawMeshT<Mesh>::getTextureIDofTri(unsigned int _tri)
{
  return getTextureIDofFace(meshComp_->mapToOriginalFaceID(_tri));
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

    delete [] verticesTmp_;
    verticesTmp_ = new Vertex[numVerts_];

    memcpy(verticesTmp_, vertices_, sizeof(Vertex) * numVerts_);

    if (flatMode_)
    {
      for (unsigned int i = 0; i < numTris_; ++i)
      {
        int faceId = meshComp_->mapToOriginalFaceID(i);

        // get face normal
        ACG::Vec3d n = mesh_.normal(mesh_.face_handle(faceId));

        // store face normal in last tri vertex
        for (unsigned int k = 0; k < 3; ++k)
        {
          int idx = meshComp_->getIndex(i*3 + 2);
          verticesTmp_[idx].n[k] = n[k];
        }
      }
      bVBOinFlatMode_ = 1;
    }
    if (!textureMode_)
    {
      for (unsigned int i = 0; i < numVerts_; ++i)
      {
        typename Mesh::HalfedgeHandle hh = mapToHalfedgeHandle(i);

        if (hh.is_valid())
        {
          // copy texcoord
          for (int k = 0; k < 2; ++k)
            verticesTmp_[i].tex[k] = mesh_.texcoord2D( mesh_.to_vertex_handle(hh) )[k];
        }
        else
        {
          // isolated vertex
          int f_id, c_id;
          int posID = meshComp_->mapToOriginalVertexID(i, f_id, c_id);

          for (int k = 0; k < 2; ++k)
            verticesTmp_[i].tex[k] = mesh_.texcoord2D( mesh_.vertex_handle(posID) )[k];
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
//   if (numVerts_ <= 0xFFFF)
//   {
//     // use 2 byte indices
//     unsigned short* pwIndices = (unsigned short*)indicesTmp_;
//     indexType_ = GL_UNSIGNED_SHORT;
// 
//     for (unsigned int i = 0; i < numTris_ * 3; ++i)
//       pwIndices[i] = (unsigned short)indices_[i];
// 
//     glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, numTris_ * 3 * sizeof(unsigned short), pwIndices, GL_STATIC_DRAW_ARB);
//   }
//   else
  glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, numTris_ * 3 * sizeof(unsigned int), meshComp_->getIndexBuffer(), GL_STATIC_DRAW_ARB);


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
  delete [] vertices_;
  delete [] verticesTmp_;
//  delete [] m_pVerticesC;

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

  sysBufSize += meshComp_->getMemoryUsage();

  // vertex buffer
  if (vertices_)
    sysBufSize += sizeof(Vertex) * numVerts_;

  if (verticesTmp_)
    sysBufSize += sizeof(Vertex) * numVerts_;

  
  
  res += sysBufSize;

  // mappings
  unsigned int mapsSize = 0;
 
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
void DrawMeshT<Mesh>::updateGPUBuffers()
{
  // rebuild if necessary
  if ((!numTris_ && mesh_.n_faces())|| ! numVerts_ || (!meshComp_ && mesh_.n_faces()))
  {
    rebuild_ = REBUILD_FULL;
  }

  if (bVBOinHalfedgeNormalMode_ != halfedgeNormalMode_) rebuild_ = REBUILD_FULL;

  // if no rebuild necessary, check for smooth / flat shading switch 
  // to update normals
  if (rebuild_ == REBUILD_NONE)
  {
    if (bVBOinFlatMode_ != flatMode_ || bVBOinHalfedgeTexMode_ != textureMode_)
      createVBO();
  }
  else
  {
    rebuild();
  }
}


template <class Mesh>
GLuint DrawMeshT<Mesh>::getVBO()
{
  updateGPUBuffers();
  return vbo_;
}

template <class Mesh>
GLuint DrawMeshT<Mesh>::getIBO()
{
  updateGPUBuffers();
  return ibo_;
}

template <class Mesh>
unsigned int DrawMeshT<Mesh>::mapVertexToVBOIndex(unsigned int _v)
{
  if (_v < mesh_.n_vertices())
    return invVertexMap_[_v];

  return (unsigned int)-1;
}

template <class Mesh>
void DrawMeshT<Mesh>::bindBuffers()
{
  updateGPUBuffers();

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
  updateGPUBuffers();

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

      for (int i = 0; i < meshComp_->getNumSubsets(); ++i)
      {
        const MeshCompiler::Subset* sub = meshComp_->getSubset(i);

        if ( _textureMap->find(sub->id) == _textureMap->end() ) {
          std::cerr << "Illegal texture index ... trying to access " << sub->id << std::endl;
          ACG::GLState::bindTexture(GL_TEXTURE_2D, 0);
        }
        else
          ACG::GLState::bindTexture(GL_TEXTURE_2D, (*_textureMap)[sub->id]);

        glDrawElements(GL_TRIANGLES, sub->numTris * 3, indexType_,
          (GLvoid*)( (size_t)sub->startIndex * (indexType_ == GL_UNSIGNED_INT ? 4 : 2))); // offset in bytes
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

    if (_textureMap && _baseObj->shaderDesc.textured())
    {
      // textured mode

      for (int i = 0; i < meshComp_->getNumSubsets(); ++i)
      {
        const MeshCompiler::Subset* sub = meshComp_->getSubset(i);

        if ( _textureMap->find(sub->id) == _textureMap->end() ) {
          std::cerr << "Illegal texture index ... trying to access " << sub->id << std::endl;
        }
        else
        {
          RenderObject::Texture tex;
          tex.type = GL_TEXTURE_2D;
          tex.id = (*_textureMap)[sub->id];
          ro.addTexture(tex,0);
        }

        

        ro.glDrawElements(GL_TRIANGLES, sub->numTris * 3, indexType_,
          (GLvoid*)( (size_t)sub->startIndex * (indexType_ == GL_UNSIGNED_INT ? 4 : 2))); // offset in bytes
        
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

//////////////////////////////////////////////////////////////////////////

template <class Mesh>
unsigned int DrawMeshT<Mesh>::countTris(unsigned int* pMaxVertsOut, unsigned int* _pOutNumIndices)
{
  unsigned int triCounter = 0;

  if (pMaxVertsOut) *pMaxVertsOut = 0;
  if (_pOutNumIndices) *_pOutNumIndices = 0;

  for (unsigned int i = 0; i < mesh_.n_faces(); ++i)
  {
    typename Mesh::FaceHandle fh =  mesh_.face_handle(i);

    // count vertices
  	unsigned int nPolyVerts = 0;

    for (typename Mesh::FaceHalfedgeIter hh_it = mesh_.fh_iter(fh); hh_it.is_valid(); ++hh_it ) ++nPolyVerts;

    triCounter += (nPolyVerts - 2);

    if (pMaxVertsOut)
    {
      if (*pMaxVertsOut < nPolyVerts)
        *pMaxVertsOut = nPolyVerts;
    }

    if (_pOutNumIndices) *_pOutNumIndices += nPolyVerts;
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

    perEdgeVertexBuf_[idx]   = mesh_.point(mesh_.to_vertex_handle(mesh_.halfedge_handle(*e_it, 0)));
    perEdgeVertexBuf_[idx+1] = mesh_.point(mesh_.to_vertex_handle(mesh_.halfedge_handle(*e_it, 1)));

    if (  mesh_.has_edge_colors() ) {
      const Vec4f color = OpenMesh::color_cast<Vec4f>( mesh_.color(*e_it) ) ;
      perEdgeColorBuf_[ idx ]     = color;
      perEdgeColorBuf_[ idx + 1 ] = color;
    }

    idx += 2;
  }


  updatePerEdgeBuffers_ = 0;

  updateEdgeHalfedgeVertexDeclarations();
}

template <class Mesh>
template<typename Mesh::Normal (DrawMeshT<Mesh>::*NormalLookup)(typename Mesh::FaceHandle)>
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

  for (typename Mesh::ConstHalfedgeIter he_it(mesh_.halfedges_sbegin()), he_end(mesh_.halfedges_end());
          he_it != he_end; ++he_it) {

    typename Mesh::HalfedgeHandle next_heh     = mesh_.next_halfedge_handle(*he_it);
    typename Mesh::HalfedgeHandle previous_heh = mesh_.prev_halfedge_handle(*he_it);

    if (mesh_.is_valid_handle(next_heh) && mesh_.is_valid_handle(previous_heh))
    {
        perHalfedgeVertexBuf_[idx]   = halfedge_point<NormalLookup>(*he_it);
        perHalfedgeVertexBuf_[idx+1] = halfedge_point<NormalLookup>(previous_heh);
    }
    else
    {
        // Cannot compute shifted vertex positions. Use original vertex positions instead.
        perHalfedgeVertexBuf_[idx  ] = mesh_.point(mesh_.to_vertex_handle(*he_it));
        perHalfedgeVertexBuf_[idx+1] = mesh_.point(mesh_.from_vertex_handle(*he_it));
    }

    if (  mesh_.has_halfedge_colors() ) {
      const Vec4f color = OpenMesh::color_cast<Vec4f>( mesh_.color(*he_it) ) ;
      perHalfedgeColorBuf_[ idx ]     = color;
      perHalfedgeColorBuf_[ idx + 1 ] = color;
    }

    idx += 2;
  }

  updatePerHalfedgeBuffers_ = 0;

  updateEdgeHalfedgeVertexDeclarations();
}

template<class Mesh>
template<typename Mesh::Normal (DrawMeshT<Mesh>::*NormalLookup)(typename Mesh::FaceHandle)>
typename Mesh::Point DrawMeshT<Mesh>::halfedge_point(const typename Mesh::HalfedgeHandle _heh) {

  typename Mesh::Point p  = mesh_.point(mesh_.to_vertex_handle  (_heh));
  typename Mesh::Point pp = mesh_.point(mesh_.from_vertex_handle(_heh));
  typename Mesh::Point pn = mesh_.point(mesh_.to_vertex_handle(mesh_.next_halfedge_handle(_heh)));

  //  typename Mesh::Point n  = (p-pp)%(pn-p);
  typename Mesh::Point fn;
  if( !mesh_.is_boundary(_heh))
    //fn = mesh_.normal(mesh_.face_handle(_heh));
      fn = (this->*NormalLookup)(mesh_.face_handle(_heh));
  else
    //fn = mesh_.normal(mesh_.face_handle(mesh_.opposite_halfedge_handle(_heh)));
      fn = (this->*NormalLookup)(mesh_.face_handle(mesh_.opposite_halfedge_handle(_heh)));

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
  if (updatePerHalfedgeBuffers_) {
      if (mesh_.has_face_normals())
          updatePerHalfedgeBuffers<&DrawMeshT::cachedNormalLookup>();
      else if (mesh_.is_trimesh())
          updatePerHalfedgeBuffers<&DrawMeshT::computedTriMeshNormal>();
      else
          updatePerHalfedgeBuffers<&DrawMeshT::computedNormal>();
  }
  return perHalfedgeVertexBuf_.empty() ? 0 : &(perHalfedgeVertexBuf_[0]); 
}

template <class Mesh>
ACG::Vec4f* DrawMeshT<Mesh>::perHalfedgeColorBuffer()
{
  // Force update of the buffers if required
  if (updatePerHalfedgeBuffers_) {
      if (mesh_.has_face_normals())
          updatePerHalfedgeBuffers<&DrawMeshT::cachedNormalLookup>();
      else if (mesh_.is_trimesh())
          updatePerHalfedgeBuffers<&DrawMeshT::computedTriMeshNormal>();
      else
          updatePerHalfedgeBuffers<&DrawMeshT::computedNormal>();
  }
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

    const Vec4uc pickColor =  _state.pick_get_name_color (e_it->idx() + _offset);

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

  for (unsigned int i = 0; i < numTris_; ++i)
  {
    unsigned int faceId = (unsigned int)meshComp_->mapToOriginalFaceID((int)i);

    const Vec4uc pickColor = _state.pick_get_name_color ( faceId );
    for (unsigned int k = 0; k < 3; ++k)
    {
      int idx = meshComp_->getIndex(i*3 + k);

      typename Mesh::HalfedgeHandle hh = mapToHalfedgeHandle(idx);
      typename Mesh::VertexHandle vh;

      if (hh.is_valid())
        vh = mesh_.to_vertex_handle( hh );
      else
      {
        int f_id, c_id;
        int posID = meshComp_->mapToOriginalVertexID(idx, f_id, c_id);
        vh = mesh_.vertex_handle(posID);
      }

      pickFaceVertexBuf_[i * 3 + k] = mesh_.point( vh );

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

  for (unsigned int i = 0; i < numTris_; ++i)
  {
    int faceId = meshComp_->mapToOriginalFaceID(i);
    const Vec4uc pickColor = _state.pick_get_name_color ( faceId );
    for (unsigned int k = 0; k < 3; ++k)
    {
      int idx = meshComp_->getIndex(i*3 + k);


      typename Mesh::HalfedgeHandle hh = mapToHalfedgeHandle(idx);
      typename Mesh::VertexHandle vh;

      if (hh.is_valid())
        vh = mesh_.to_vertex_handle( hh );
      else
      {
        int f_id, c_id;
        int posID = meshComp_->mapToOriginalVertexID(idx, f_id, c_id);
        vh = mesh_.vertex_handle(posID);
      }

      pickFaceVertexBuf_[i * 3 + k] = mesh_.point( vh );

      pickAnyFaceColBuf_[i * 3 + k] = pickColor;
    }
  }



  updatePerEdgeBuffers();

  pickAnyEdgeColBuf_.resize(mesh_.n_edges() * 2);

  unsigned int idx = 0;
  typename Mesh::ConstEdgeIter  e_it(mesh_.edges_sbegin()), e_end(mesh_.edges_end());
  for (; e_it!=e_end; ++e_it) {

    const Vec4uc pickColor =  _state.pick_get_name_color (e_it->idx() + mesh_.n_faces());

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
  for (unsigned int i = 0; i < meshComp_->getNumSubsets(); ++i)
  {
    if (meshComp_->getSubset(i)->id > 0) ++n;
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

template <class Mesh>
VertexDeclaration* ACG::DrawMeshT<Mesh>::getVertexDeclaration()
{
  return  vertexDeclVCol_;
}


template<class Mesh>
typename Mesh::HalfedgeHandle ACG::DrawMeshT<Mesh>::mapToHalfedgeHandle(int _vertexId)
{
  // map to halfedge handle+
  int faceId, cornerId;
  int posID = meshComp_->mapToOriginalVertexID(_vertexId, faceId, cornerId);

  if (faceId >= 0)
  {
    typename Mesh::FaceHandle fh = mesh_.face_handle(faceId);
    typename Mesh::FaceHalfedgeIter hh_it = mesh_.fh_iter(fh);

    // seek to halfedge
    for (int k = 0; k < cornerId && hh_it.is_valid(); ++k )
      ++hh_it;

    return *hh_it;
  }
  else
    return typename Mesh::HalfedgeHandle(-1);
}

template<class Mesh>
template<class Prop>
void ACG::DrawMeshT<Mesh>::setMeshCompilerInput( int _attrIdx, Prop _propData, int _num )
{
  GLuint fmt = 0;
  int size = 0, stride = 0;
  getMeshPropertyType(_propData, &fmt, &size, &stride);

  meshComp_->setAttribVec(_attrIdx, _num, _propData, stride, false, fmt, size);
}


}
