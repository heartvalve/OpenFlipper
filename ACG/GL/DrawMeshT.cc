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

#define ACG_DRAW_MESH_TCC

//=============================================================================

#include "DrawMesh.hh"

#include <ACG/GL/gl.hh>
#include <ACG/Geometry/GPUCacheOptimizer.hh>
#include <ACG/GL/VertexDeclaration.hh>
#include <ACG/GL/ShaderCache.hh>
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

// print a memory usage report each draw call
//#define DEBUG_MEM_USAGE


namespace ACG
{

template <class Mesh>
DrawMeshT<Mesh>::DrawMeshT(Mesh& _mesh)
:  mesh_(_mesh),
   rebuild_(REBUILD_NONE),
   prevNumFaces_(0), prevNumVerts_(0),
   colorMode_(1),
   curVBOColorMode_(1),
   flatMode_(0), bVBOinFlatMode_(0),
   textureMode_(1), bVBOinHalfedgeTexMode_(1),
   halfedgeNormalMode_(0), bVBOinHalfedgeNormalMode_(0),
   invVertexMap_(0),
   offsetPos_(0), offsetNormal_(20), offsetTexc_(12), offsetColor_(32),
   updateFullVBO_(true),
   textureIndexPropertyName_("Not Set"),
   perFaceTextureCoordinatePropertyName_("Not Set"),
   updatePerEdgeBuffers_(1),
  updatePerHalfedgeBuffers_(1)
{

  pickVertexMethod_ = 1;
  pickVertexShader_ = 0;

  pickFaceShader_ = 0;
  pickEdgeShader_ = 0;

  createVertexDeclaration();
}

template<class Mesh>
template<class T>
const void* DrawMeshT<Mesh>::testMeshPropertyTypeT( const OpenMesh::BaseProperty* _prop, unsigned int* _outSize ) const
{
  if (_outSize)
    *_outSize = 0;
  const void* dataPtr = 0;

  // rtti - detect type of property from openmesh via dynamic_cast
  typedef OpenMesh::PropertyT< T > Prop1;
  typedef OpenMesh::PropertyT< OpenMesh::VectorT<T, 1> > PropVec1;
  typedef OpenMesh::PropertyT< OpenMesh::VectorT<T, 2> > PropVec2;
  typedef OpenMesh::PropertyT< OpenMesh::VectorT<T, 3> > PropVec3;
  typedef OpenMesh::PropertyT< OpenMesh::VectorT<T, 4> > PropVec4;

  const Prop1* p1 = dynamic_cast<const Prop1*>(_prop);
  const PropVec1* pv1 = dynamic_cast<const PropVec1*>(_prop);
  const PropVec2* pv2 = dynamic_cast<const PropVec2*>(_prop);
  const PropVec3* pv3 = dynamic_cast<const PropVec3*>(_prop);
  const PropVec4* pv4 = dynamic_cast<const PropVec4*>(_prop);
  
  if (p1 || pv1)
  {
    if (_outSize)
      *_outSize = 1;
    if (p1)
      dataPtr = p1->data();
    else
      dataPtr = pv1->data();
  }
  else if (pv2)
  {
    if (_outSize)
      *_outSize = 2;
    dataPtr = pv2->data();
  }
  else if (pv3)
  {
    if (_outSize)
      *_outSize = 3;
    dataPtr = pv3->data();
  }
  else if (pv4)
  {
    if (_outSize)
      *_outSize = 4;
    dataPtr = pv4->data();
  }

  return dataPtr;
}


template<class Mesh>
const void* DrawMeshT<Mesh>::getMeshPropertyType( OpenMesh::BaseProperty* _prop, GLuint* _outType, unsigned int* _outSize ) const
{
  const void* dataPtr = 0;

  // try float
  dataPtr = testMeshPropertyTypeT<float>(_prop, _outSize);
  if (dataPtr)
  {
    if (_outType) *_outType = GL_FLOAT;
    return dataPtr;
  }

  // try byte
  dataPtr = testMeshPropertyTypeT<char>(_prop, _outSize);
  if (dataPtr)
  {
    if (_outType) *_outType = GL_BYTE;
    return dataPtr;
  }

  // try ubyte
  dataPtr = testMeshPropertyTypeT<unsigned char>(_prop, _outSize);
  if (dataPtr)
  {
    if (_outType) *_outType = GL_UNSIGNED_BYTE;
    return dataPtr;
  }

  // try double
  dataPtr = testMeshPropertyTypeT<double>(_prop, _outSize);

  if (dataPtr)
  {
    if (_outType) *_outType = GL_DOUBLE;
    return dataPtr;
  }

  // try int
  dataPtr = testMeshPropertyTypeT<int>(_prop, _outSize);

  if (dataPtr)
  {
    if (_outType) *_outType = GL_INT;
    return dataPtr;
  }

  // try uint
  dataPtr = testMeshPropertyTypeT<unsigned int>(_prop, _outSize);

  if (dataPtr)
  {
    if (_outType) *_outType = GL_UNSIGNED_INT;
    return dataPtr;
  }

  // try short
  dataPtr = testMeshPropertyTypeT<short>(_prop, _outSize);

  if (dataPtr)
  {
    if (_outType) *_outType = GL_SHORT;
    return dataPtr;
  }

  // try ushort
  dataPtr = testMeshPropertyTypeT<unsigned short>(_prop, _outSize);

  if (dataPtr)
  {
    if (_outType) *_outType = GL_UNSIGNED_SHORT;
    return dataPtr;
  }


  // unknown data type
  if (_outSize)
    *_outSize = 0;

  if (_outType)
    *_outType = 0;

  return 0;
}



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



  // --------------------------------------------
  // debug - request properties
/*
  if (additionalElements_.empty() && (mesh_._get_hprop("inTangent") || mesh_._get_vprop("inTangent")))
  {
    //     VertexProperty tmp;
    //     tmp.name_ = "inTangent";
    //     tmp.source_ = 0;
    // 
    //     additionalElements_.push_back(tmp);

//    scanVertexShaderForInput( "c:/dbg/nm_VS.tpl" );
    scanVertexShaderForInput( "/home/tenter/dbg/nm_VS.tpl" );
  }
*/
  // --------------------------------------------

  // todo: check if vertex layout has been changed and eventually force a full rebuild


  // update layout declaration
  createVertexDeclaration();

  // support for point clouds:
  if (mesh_.n_vertices() && mesh_.n_faces() == 0)
  {
    if (mesh_.n_vertices() > numVerts_)
    {
      delete [] invVertexMap_; 
      invVertexMap_ = 0;
    }
    numVerts_ = mesh_.n_vertices();
    vertices_.resize(numVerts_ * vertexDecl_->getVertexStride());

    // read all vertices
    for (size_t i = 0; i < numVerts_; ++i)
      readVertex(i,
                 mesh_.vertex_handle(i), 
                 (typename Mesh::HalfedgeHandle)(-1), 
                 (typename Mesh::FaceHandle)(-1));

    createVBO();
    rebuild_ = REBUILD_NONE;
    return;
  }

  invalidateFullVBO();


  unsigned int maxFaceVertCount = 0;
  unsigned int numIndices = 0;
  unsigned int newTriCount = countTris(&maxFaceVertCount, &numIndices);
  
  int bTriangleRebuild = 0; // what should be rebuild?
  int bVertexRebuild = 0;

  if (newTriCount > numTris_)
  {
    // index buffer too small
    deleteIbo();

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

      readVertex(i, vh, hh, fh);
    }

    createVBO();

    rebuild_ = REBUILD_NONE;
    return;
  }


  // full rebuild:
  delete meshComp_;
  meshComp_ = new MeshCompiler(*vertexDecl_);


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

  // index source for custom attributes
  for (size_t i = 0; i < additionalElements_.size(); ++i)
  {
    const VertexProperty* prop = &additionalElements_[i];

    if (prop->declElementID_ >= 0)
      faceInput->attributeStoredPerHalfedge_[prop->declElementID_] = (prop->source_ == PROPERTY_SOURCE_HALFEDGE) ? 1 : 0;
  }

  meshComp_->setFaceInput(faceInput);

  // set textures
  for (unsigned int i = 0; i < mesh_.n_faces(); ++i)
    meshComp_->setFaceGroup(i, getTextureIDofFace(i));

  // pass vertex data to mesh compiler


  // points
  meshComp_->setVertices(mesh_.n_vertices(), mesh_.points(), 24, false, GL_DOUBLE, 3);
  
  // normals
  if (mesh_.has_halfedge_normals())
    meshComp_->setNormals(mesh_.n_halfedges(), mesh_.property(mesh_.halfedge_normals_pph()).data(), 24, false, GL_DOUBLE, 3);
  else if (mesh_.has_vertex_normals())
    meshComp_->setNormals(mesh_.n_vertices(), mesh_.vertex_normals(), 24, false, GL_DOUBLE, 3);

  if (mesh_.has_halfedge_texcoords2D())
    meshComp_->setTexCoords(mesh_.n_halfedges(), mesh_.htexcoords2D(), 8, false, GL_FLOAT, 2);

  // add more requested custom attribtues to mesh compiler here..

  for (size_t i = 0; i < additionalElements_.size(); ++i)
  {
    VertexProperty* propDesc = &additionalElements_[i];

    if (propDesc->declElementID_ >= 0)
    {
      const VertexElement* el = vertexDecl_->getElement((unsigned int)propDesc->declElementID_);

      if (el->usage_ == VERTEX_USAGE_SHADER_INPUT)
      {
        // get openmesh property handle
        OpenMesh::BaseProperty* baseProp = 0;
        
        switch (propDesc->source_)
        {
        case PROPERTY_SOURCE_VERTEX: baseProp = mesh_._get_vprop(propDesc->name_); break;
        case PROPERTY_SOURCE_FACE: baseProp = mesh_._get_fprop(propDesc->name_); break;
        case PROPERTY_SOURCE_HALFEDGE: baseProp = mesh_._get_hprop(propDesc->name_); break;
        default: baseProp = mesh_._get_vprop(propDesc->name_); break;
        }

        if (baseProp)
        {
          int numAttribs = baseProp->n_elements();
          const void* attribData = propDesc->propDataPtr_; 

          meshComp_->setAttribVec( propDesc->declElementID_, numAttribs, attribData );
        }
        

      }
    }

    
  }


  // compile draw buffers
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

  vertices_.resize(numVerts_ * vertexDecl_->getVertexStride());
  meshComp_->getVertexBuffer(&vertices_[0]);

  // copy colors
  for (int i = 0; i < (int)numVerts_; ++i)
  {
    typename Mesh::HalfedgeHandle hh = mapToHalfedgeHandle(i);

    unsigned int col = 0;

    if (hh.is_valid())
      col = getVertexColor(mesh_.to_vertex_handle(hh));
    else
    {
      // isolated vertex
      int f_id, c_id;
      int posID = meshComp_->mapToOriginalVertexID(i, f_id, c_id);
      col = getVertexColor( mesh_.vertex_handle(posID) );
    }

    writeColor(i, col);
  }

  // vbo stores per vertex colors
  curVBOColorMode_ = 1; 

  // copy face colors to provoking id
  if (colorMode_ == 2)
  {
    const int provokingId = meshComp_->getProvokingVertex();
    assert(provokingId >= 0 && provokingId < 3);

    for (int i = 0; i < (int)numTris_; ++i)
    {
      int idx = meshComp_->getIndex(i*3+provokingId);

      int faceId = meshComp_->mapToOriginalFaceID(i);
      unsigned int fcolor = getFaceColor(mesh_.face_handle(faceId));

      writeColor(idx, fcolor);
    }

#ifdef _DEBUG
    // debug check

    for (int i = 0; i < (int)numTris_; ++i)
    {
      int idx = meshComp_->getIndex(i*3+provokingId);

      int faceId = meshComp_->mapToOriginalFaceID(i);
      unsigned int fcolor = getFaceColor(mesh_.face_handle(faceId));

      unsigned int storedColor = *(unsigned int*)(&vertices_[idx * vertexDecl_->getVertexStride() + offsetColor_]);

      if (storedColor != fcolor)
      {
        std::cout << "warning: possibly found provoking vertex shared by more than one face, writing report to ../../meshcomp_provoking.txt" << std::endl;

        // could also be caused by multi-threading, where one thread calls rebuild() 
        // and the other thread updates face colors between previous for-loop and debug-check

        // check for errors
        meshComp_->dbgVerify("../../meshcomp_provoking.txt");
        
        break; // verify and dump report only once
      }
    }
#endif // _DEBUG

    curVBOColorMode_ = colorMode_;
  }


 

  //////////////////////////////////////////////////////////////////////////
  // copy to GPU

  createVBO();
  createIBO();

  bVBOinHalfedgeNormalMode_ = halfedgeNormalMode_;

  rebuild_ = REBUILD_NONE;
}


template <class Mesh>
void
DrawMeshT<Mesh>::readVertex(unsigned int _vertex,
                            const typename Mesh::VertexHandle _vh,
                            const typename Mesh::HalfedgeHandle _hh,
                            const typename Mesh::FaceHandle _fh)
{
  static const typename Mesh::HalfedgeHandle invalidHEH(-1);
  static const typename Mesh::FaceHandle     invalidFH(-1);


  ACG::Vec3d n(0.0, 0.0, 1.0);
  ACG::Vec2f texc(0.0f, 0.0f);
  unsigned int col(0);

  // read normal
  if (halfedgeNormalMode_ == 0 && mesh_.has_vertex_normals())
    n = mesh_.normal(_vh);
  else if (halfedgeNormalMode_ &&  mesh_.has_halfedge_normals() && _hh != invalidHEH)
    n = mesh_.normal(_hh);

  // read texcoord
  if (mesh_.has_halfedge_texcoords2D())
  {
    if (_hh != invalidHEH && textureMode_ == 1)
      texc = mesh_.texcoord2D(_hh);
    else if (mesh_.has_vertex_texcoords2D())
      texc = mesh_.texcoord2D(_vh);
  }
  else if (mesh_.has_vertex_texcoords2D())
    texc = mesh_.texcoord2D(_vh);

  // read per face or per vertex color
  unsigned int byteCol[2];
  for (int col = 0; col < 2; ++col)
  {
    Vec4uc vecCol(255, 255, 255, 255);

    if (col == 0 && mesh_.has_vertex_colors()) 
      vecCol = OpenMesh::color_cast<Vec4uc, typename Mesh::Color>(mesh_.color(_vh));
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

  if (colorMode_ != 2)
    col = byteCol[0]; // vertex colors
  else
    col = byteCol[1]; // face colors


  // store vertex attributes in vbo
  writePosition(_vertex, mesh_.point(_vh));
  writeNormal(_vertex, n);
  writeTexcoord(_vertex, texc);
  writeColor(_vertex, col);
  

  // read/write custom attributes

  for (size_t i = 0; i < additionalElements_.size(); ++i)
  {
    std::cout << "not implemented!" << std::endl;


  }

  
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
  bindVbo();

  // toggle between normal source and texcoord source
  // (per vertex, per halfedge, per face)

  if (flatMode_ && meshComp_)
  {
    for (unsigned int i = 0; i < numTris_; ++i)
    {
      int faceId = meshComp_->mapToOriginalFaceID(i);

      // get face normal
      ACG::Vec3d n = mesh_.normal(mesh_.face_handle(faceId));

      // store face normal in last tri vertex
//      for (unsigned int k = 0; k < 3; ++k)
      {
        int idx = meshComp_->getIndex(i*3 + meshComp_->getProvokingVertex());
//        vertices_[idx].n[k] = n[k];
        writeNormal(idx, n);
      }

    }
    bVBOinFlatMode_ = 1;
  }
  else
  {
    for (unsigned int i = 0; i < numVerts_; ++i)
    {
      typename Mesh::HalfedgeHandle hh = mapToHalfedgeHandle(i);

      // get halfedge normal

      if (hh.is_valid())
      {
        ACG::Vec3d n;
        if (halfedgeNormalMode_ == 1 && mesh_.has_halfedge_normals())
          n = mesh_.normal( hh );
        else
          n = mesh_.normal( mesh_.to_vertex_handle(hh) );

//         for (int k = 0; k < 3; ++k)
//           vertices_[i].n[k] = n[k];
        writeNormal(i, n);
      }
      else
      {
        // isolated vertex
        int posID = i;


        if (meshComp_) {
          int f_id, c_id;
          posID = meshComp_->mapToOriginalVertexID(i, f_id, c_id);
        }

//         for (int k = 0; k < 3; ++k)
//           vertices_[i].tex[k] = mesh_.normal( mesh_.vertex_handle(posID) )[k];
        writeNormal(i, mesh_.normal( mesh_.vertex_handle(posID) ));
      }
    }

    bVBOinFlatMode_ = 0;
  }

  if (textureMode_ == 0)
  {
    // per vertex texcoords
    if (mesh_.has_vertex_texcoords2D())
    {
      for (unsigned int i = 0; i < numVerts_; ++i)
      {
        typename Mesh::HalfedgeHandle hh = mapToHalfedgeHandle(i);

        if (hh.is_valid())
        {
          // copy texcoord
//           for (int k = 0; k < 2; ++k)
//             vertices_[i].tex[k] = mesh_.texcoord2D( mesh_.to_vertex_handle(hh) )[k];

          writeTexcoord(i, mesh_.texcoord2D( mesh_.to_vertex_handle(hh) ) );
        }
        else
        {
          // isolated vertex
          int posID = i;

          if (meshComp_) {
            int f_id, c_id;
            posID = meshComp_->mapToOriginalVertexID(i, f_id, c_id);
          }

//           for (int k = 0; k < 2; ++k)
//             vertices_[i].tex[k] = mesh_.texcoord2D( mesh_.vertex_handle(posID) )[k];

          writeTexcoord(i, mesh_.texcoord2D( mesh_.vertex_handle(posID) ) );

        }
      }
    }
    
    bVBOinHalfedgeTexMode_ = 0;
  }
  else
  {
    if (mesh_.has_vertex_texcoords2D() || mesh_.has_halfedge_texcoords2D())
    {
      // per halfedge texcoords
      for (unsigned int i = 0; i < numVerts_; ++i)
      {
        typename Mesh::HalfedgeHandle hh = mapToHalfedgeHandle(i);

        if (hh.is_valid())
        {
          // copy texcoord
          if (mesh_.has_halfedge_texcoords2D())
          {
//             for (int k = 0; k < 2; ++k)
//               vertices_[i].tex[k] = mesh_.texcoord2D( hh )[k];

            writeTexcoord(i, mesh_.texcoord2D( hh ) );
          }

        }
        else if (mesh_.has_vertex_texcoords2D())
        {
          // isolated vertex
          int posID = i;


          if (meshComp_) {
            int f_id, c_id;
            posID = meshComp_->mapToOriginalVertexID(i, f_id, c_id);
          }

//           for (int k = 0; k < 2; ++k)
//             vertices_[i].tex[k] = mesh_.texcoord2D( mesh_.vertex_handle(posID) )[k];

          writeTexcoord(i, mesh_.texcoord2D( mesh_.vertex_handle(posID) ) );

        }
      }
    }
    
    bVBOinHalfedgeTexMode_ = 1;
  }

  if (colorMode_ && colorMode_ != curVBOColorMode_)
  {
    if (colorMode_ == 1)
    {
      // use vertex colors

      for (int i = 0; i < (int)numVerts_; ++i)
      {
        typename Mesh::HalfedgeHandle hh = mapToHalfedgeHandle(i);

        unsigned int col;

        if (hh.is_valid())
          col = getVertexColor(mesh_.to_vertex_handle(hh));
        else
        {
          // isolated vertex
          int f_id, c_id;
          int posID = meshComp_->mapToOriginalVertexID(i, f_id, c_id);
          col = getVertexColor( mesh_.vertex_handle(posID) );
        }

        writeColor(i, col);
      }
    }
    else if (colorMode_ == 2)
    {
      // use face colors

      const int provokingId = meshComp_->getProvokingVertex();
      assert(provokingId >= 0 && provokingId < 3);

      for (int i = 0; i < (int)numTris_; ++i)
      {
        int idx = meshComp_->getIndex(i*3+provokingId);

        int faceId = meshComp_->mapToOriginalFaceID(i);
        unsigned int fcolor = getFaceColor(mesh_.face_handle(faceId));

//        vertices_[idx].col = fcolor;
        writeColor(idx, fcolor);
      }
    }

    // vbo colors updated
    curVBOColorMode_ = colorMode_;
  }

  fillVertexBuffer();

  ACG::GLState::bindBufferARB(GL_ARRAY_BUFFER_ARB, 0);

  // non indexed vbo needs updating now
  invalidateFullVBO();
}

template <class Mesh>
void
DrawMeshT<Mesh>::createIBO()
{
  // data read from indices_

  bindIbo();

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
  createIndexBuffer();


  // line index buffer:
  if (mesh_.n_edges())
  {
    std::vector<unsigned int> lineBuffer(mesh_.n_edges() * 2);

    for (unsigned int i = 0; i < mesh_.n_edges(); ++i)
    {
      OpenMesh::HalfedgeHandle hh = mesh_.halfedge_handle(mesh_.edge_handle(i), 0);

      if (indexType_ == GL_UNSIGNED_SHORT)
      {
        // put two words in a dword
        unsigned int combinedIdx = invVertexMap_[mesh_.from_vertex_handle(hh).idx()] | (invVertexMap_[mesh_.to_vertex_handle(hh).idx()] << 16);
        lineBuffer[i] = combinedIdx;
      }
      else
      {
        lineBuffer[2 * i] = invVertexMap_[mesh_.from_vertex_handle(hh).idx()];
        lineBuffer[2 * i + 1] = invVertexMap_[mesh_.to_vertex_handle(hh).idx()];
      }
    }

    bindLineIbo();

    fillLineBuffer(mesh_.n_edges(), &lineBuffer[0]);
  }

  ACG::GLState::bindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
}

template <class Mesh>
DrawMeshT<Mesh>::~DrawMeshT(void)
{
  delete [] invVertexMap_;
}



template <class Mesh>
unsigned int DrawMeshT<Mesh>::getMemoryUsage(bool _printReport)
{
  unsigned int res = 0;
  unsigned int sysBufSize = 0;

  sysBufSize += meshComp_->getMemoryUsage();

  // vertex buffer
  if (!vertices_.empty())
    sysBufSize += vertexDecl_->getVertexStride() * numVerts_;

  
  
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
    gpuBufSize += numVerts_ * vertexDecl_->getVertexStride();

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
    if (bVBOinFlatMode_ != flatMode_ || bVBOinHalfedgeTexMode_ != textureMode_ || (colorMode_ && curVBOColorMode_ != colorMode_))
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
  {
    if (invVertexMap_)
      return invVertexMap_[_v];
    else //if mesh is a point cloud
      return _v;
  }

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
    ACG::GLState::colorPointer(4, GL_UNSIGNED_BYTE, vertexDecl_->getVertexStride(), (char*)offsetColor_);
    ACG::GLState::enableClientState(GL_COLOR_ARRAY);
  }

  // vertex decl
  ACG::GLState::vertexPointer(3, GL_FLOAT, vertexDecl_->getVertexStride(), (char*)offsetPos_);
  ACG::GLState::enableClientState(GL_VERTEX_ARRAY);

  glClientActiveTexture(GL_TEXTURE0);
  ACG::GLState::texcoordPointer(2, GL_FLOAT, vertexDecl_->getVertexStride(), (char*)offsetTexc_);
  ACG::GLState::enableClientState(GL_TEXTURE_COORD_ARRAY);

  ACG::GLState::normalPointer(GL_FLOAT, vertexDecl_->getVertexStride(), (char*)offsetNormal_);
  ACG::GLState::enableClientState(GL_NORMAL_ARRAY);

//  ACG::GLState::normalPointerEXT(3, GL_FLOAT, sizeof(Vertex), (char*)(20));  // ACG::GLState::normalPointerEXT crashes sth. in OpenGL

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
  _obj->vertexDecl = vertexDecl_;
}

template <class Mesh>
void DrawMeshT<Mesh>::unbindBuffers()
{
  ACG::GLState::disableClientState(GL_VERTEX_ARRAY);
  ACG::GLState::disableClientState(GL_TEXTURE_COORD_ARRAY);
  ACG::GLState::disableClientState(GL_NORMAL_ARRAY);

  if (colorMode_)
    ACG::GLState::disableClientState(GL_COLOR_ARRAY);

  ACG::GLState::bindBuffer(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
  ACG::GLState::bindBuffer(GL_ARRAY_BUFFER_ARB, 0);
}

template <class Mesh>
void DrawMeshT<Mesh>::draw(std::map< int, GLuint>* _textureMap, bool _nonindexed)
{
  if (!_nonindexed)
    bindBuffers();
  else
  {
    updateFullVBO();
    vboFull_.bind();
    vertexDecl_->activateFixedFunction();
  }

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

        if (!_nonindexed)
          glDrawElements(GL_TRIANGLES, sub->numTris * 3, indexType_,
            (GLvoid*)( (size_t)sub->startIndex * (indexType_ == GL_UNSIGNED_INT ? 4 : 2))); // offset in bytes
        else
          glDrawArrays(GL_TRIANGLES, sub->startIndex, sub->numTris * 3);
      }
    }
    else
    {
      if (!_nonindexed)
        glDrawElements(GL_TRIANGLES, numTris_ * 3, indexType_, 0);
      else
        glDrawArrays(GL_TRIANGLES, 0, numTris_ * 3);
    }
  }

  unbindBuffers();
}


template <class Mesh>
void ACG::DrawMeshT<Mesh>::addTriRenderObjects(IRenderer* _renderer, const RenderObject* _baseObj, std::map< int, GLuint>* _textureMap, bool _nonindexed)
{
  if (numTris_)
  {
    RenderObject ro = *_baseObj;
    if (!_nonindexed)
      bindBuffersToRenderObject(&ro);
    else
    {
      updateFullVBO();

      ro.vertexBuffer = vboFull_.id();
      ro.vertexDecl = vertexDecl_;
    }

    if (_baseObj->shaderDesc.textured())
    {
      // textured mode

      for (int i = 0; i < meshComp_->getNumSubsets(); ++i)
      {
        const MeshCompiler::Subset* sub = meshComp_->getSubset(i);

        if ( _textureMap )
        {
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
        }
        else // no texture map specified, use whatever texture is currently bound to the first texture stage
        {
          glActiveTextureARB(GL_TEXTURE0);
          GLint textureID = 0;
          glGetIntegerv(GL_TEXTURE_BINDING_2D, &textureID);

          RenderObject::Texture tex;
          tex.type = GL_TEXTURE_2D;
          tex.id = textureID;
          ro.addTexture(tex,0);
        }

        

        if (!_nonindexed)
          ro.glDrawElements(GL_TRIANGLES, sub->numTris * 3, indexType_, 
            (GLvoid*)((size_t)sub->startIndex * (indexType_ == GL_UNSIGNED_INT ? 4 : 2))); // offset in bytes
        else
          ro.glDrawArrays(GL_TRIANGLES, sub->startIndex, sub->numTris * 3);
        
        _renderer->addRenderObject(&ro);
      }
    }
    else
    {
      if (!_nonindexed)
        ro.glDrawElements(GL_TRIANGLES, numTris_ * 3, indexType_, 0);
      else
        ro.glDrawArrays(GL_TRIANGLES,0,  numTris_ * 3);
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
void DrawMeshT<Mesh>::updatePickingVertices_opt(ACG::GLState& _state)
{
  // Make sure, the face buffers are up to date before generating the picking data!
  if (!numVerts_ && mesh_.n_vertices())
  {
    rebuild_ = REBUILD_FULL;
    rebuild();
  }

#ifndef GL_ARB_texture_buffer_object
  pickVertexMethod_ = 1; // no texture buffers supported during compilation
#endif

  if (numVerts_)
  {
    // upload vbo id->openmesh id lookup-table to texture buffer

    if (pickVertexMethod_ == 0)
    {
#ifdef GL_ARB_texture_buffer_object

      std::vector<int> forwardMap(numVerts_, 0);

      for (int i = 0; i < (int)numVerts_; ++i)
      {
        int vboIdx = mapVertexToVBOIndex(i);
        if (vboIdx >= 0)
          forwardMap[vboIdx] = i;
      }
      pickVertexMapTBO_.setBufferData(sizeof(int) * numVerts_, &forwardMap[0], GL_R32I, GL_STATIC_DRAW);

#endif // GL_ARB_texture_buffer_object

    }
    else
    {
      // Another method: draw with index buffer, which contains the mapping from openmesh vertex id to drawmesh vbo vertex.
      // problem with this: gl_VertexID is affected by index buffer and thus represents the drawmesh vbo ids
      // -> use gl_PrimitiveID instead, which represents the openmesh vertex id
      if (invVertexMap_)
      {
        bindPickVertexIbo();
        fillInvVertexMap(mesh_.n_vertices(), invVertexMap_);
        ACG::GLState::bindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
      }

    }
  }
}


template <class Mesh>
bool ACG::DrawMeshT<Mesh>::supportsPickingVertices_opt()
{
  // load and compile picking shader

  // load once directly from files:
//   if (!pickVertexShader_)
//   {
//     if (pickVertexMethod_ == 0)
//       pickVertexShader_ = GLSL::loadProgram("Picking/pick_vertices_vs.glsl", "Picking/pick_vertices_fs.glsl");
//     else
//       pickVertexShader_ = GLSL::loadProgram("Picking/vertex.glsl", "Picking/pick_vertices_fs2.glsl");
//   }
  
  // load from cache
  if (pickVertexMethod_ == 0)
    pickVertexShader_ = ShaderCache::getInstance()->getProgram("Picking/pick_vertices_vs.glsl", "Picking/pick_vertices_fs.glsl", 0, false);
  else
    pickVertexShader_ = ShaderCache::getInstance()->getProgram("Picking/vertex.glsl", "Picking/pick_vertices_fs2.glsl", 0, false);

  // check link status
  return pickVertexShader_ && pickVertexShader_->isLinked();
}


template <class Mesh>
void ACG::DrawMeshT<Mesh>::drawPickingVertices_opt( const GLMatrixf& _mvp, int _pickOffset )
{
  // optimized version which computes picking ids in the shader

  /*
  pickVertexMethod_
   0: - create a textureBuffer containing the mapping from vbo id to openmesh vertex id
      - draw point list of the main vbo and read texture map in the vertex shader
      - computation in vertex shader via gl_VertexID
      -> required mem: 4 bytes per vertex in draw vbo
      -> # vertex transforms: vertex count in draw vbo

   1: - create index buffer containing the mapping from openmesh vertex id to vbo id
         (not required for point-clouds)
      - draw point list with index buffer
      - computation in fragment shader via gl_PrimitiveID
      -> required mem: nothing for point-clouds, otherwise 4 bytes per vertex in openmesh
      -> # vertex transforms: vertex count in openmesh

      method 1 is probably more efficient overall
  */

  // test support by loading and compiling picking shader
  if (!supportsPickingVertices_opt())
    return;

  ACG::GLState::bindBufferARB(GL_ARRAY_BUFFER_ARB, getVBO());
  
  if (pickVertexMethod_ == 1 && invVertexMap_)
    bindPickVertexIbo();

  // setup picking shader
  pickVertexShader_->use();
  getVertexDeclaration()->activateShaderPipeline(pickVertexShader_);

  pickVertexShader_->setUniform("pickVertexOffset", _pickOffset);

  if (pickVertexMethod_ == 0)
  {
    pickVertexShader_->setUniform("vboToInputMap", 0);
#ifdef GL_ARB_texture_buffer_object
    pickVertexMap_opt()->bind(GL_TEXTURE0);
#endif
  }

  pickVertexShader_->setUniform("mWVP", _mvp);

  // draw call
  if (pickVertexMethod_ == 0)
    glDrawArrays(GL_POINTS, 0, getNumVerts());
  else
  {
    if (pickVertexIBO_opt() && invVertexMap_)
      glDrawElements(GL_POINTS, mesh_.n_vertices(), GL_UNSIGNED_INT, 0);
    else
      glDrawArrays(GL_POINTS, 0, mesh_.n_vertices());
  }

  // restore gl state      
  getVertexDeclaration()->deactivateShaderPipeline(pickVertexShader_);
  pickVertexShader_->disable();

  // unbind draw buffers
  ACG::GLState::bindBufferARB(GL_ARRAY_BUFFER_ARB, 0);

  if (pickVertexMethod_ == 1)
      ACG::GLState::bindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
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
void DrawMeshT<Mesh>::updatePickingEdges_opt(ACG::GLState& _state )
{
  // Make sure, the face buffers are up to date before generating the picking data!
  if (!numTris_ && mesh_.n_faces())
  {
    rebuild_ = REBUILD_FULL;
    rebuild();
  }

  // nothing else to do, optimized edge picking method has no memory overhead
}


template <class Mesh>
bool ACG::DrawMeshT<Mesh>::supportsPickingEdges_opt()
{
  // fetch picking shader from cache (edge picking uses same shader as vertex picking)
  pickEdgeShader_ = ShaderCache::getInstance()->getProgram("Picking/vertex.glsl", "Picking/pick_vertices_fs2.glsl", 0, false);

  // check link status
  return pickEdgeShader_ && pickEdgeShader_->isLinked();
}


template <class Mesh>
void ACG::DrawMeshT<Mesh>::drawPickingEdges_opt( const GLMatrixf& _mvp, int _pickOffset )
{
  // optimized version which computes picking ids in the shader

  /* optimization :
  - reuse draw vbo of drawmesh and line index buffer (which is used for wireframe mode)
     the line index buffer stores all edges in the same order as they appear in openmesh
  - use edge id of openmesh as gl_PrimitiveID in fragment shader to compute the picking id
  
  -> no rendering from sysmem buffers
  -> no maintenance/update of picking colors required
  -> no additional memory allocation
  */

  if (!numTris_)
    return;

  // test support by loading and compiling picking shader
  if (!supportsPickingEdges_opt())
    return;

  ACG::GLState::bindBufferARB(GL_ARRAY_BUFFER_ARB, getVBO());
  bindLineIbo();

  // setup picking shader (same shader as in vertex picking)
  pickEdgeShader_->use();
  getVertexDeclaration()->activateShaderPipeline(pickEdgeShader_);

  pickEdgeShader_->setUniform("pickVertexOffset", _pickOffset);
  pickEdgeShader_->setUniform("mWVP", _mvp);

  // draw call
  glDrawElements(GL_LINES, mesh_.n_edges() * 2, indexType_, 0);

  // restore gl state      
  getVertexDeclaration()->deactivateShaderPipeline(pickEdgeShader_);
  pickEdgeShader_->disable();

  // unbind draw buffers
  ACG::GLState::bindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
  ACG::GLState::bindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
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
void DrawMeshT<Mesh>::updatePickingFaces_opt(ACG::GLState& _state )
{
  // Make sure, the face buffers are up to date before generating the picking data!
  if (!numTris_ && mesh_.n_faces())
  {
    rebuild_ = REBUILD_FULL;
    rebuild();
  }

#ifdef GL_ARB_texture_buffer_object
  if (meshComp_ && meshComp_->getNumTriangles())
  {
    // upload tri->face lookup-table to texture buffer
    pickFaceTriToFaceMapTBO_.setBufferData(sizeof(int) * meshComp_->getNumTriangles(), meshComp_->mapToOriginalFaceIDPtr(), GL_R32I, GL_STATIC_DRAW);
  }
#endif // GL_ARB_texture_buffer_object

}


template <class Mesh>
bool ACG::DrawMeshT<Mesh>::supportsPickingFaces_opt()
{
  if (!ACG::Texture::supportsTextureBuffer())
    return false;
    
  // fetch picking shader from cache
  pickFaceShader_ = ShaderCache::getInstance()->getProgram("Picking/vertex.glsl", "Picking/pick_face.glsl", 0, false);

  // check link status
  return pickFaceShader_ && pickFaceShader_->isLinked();
}


template <class Mesh>
void ACG::DrawMeshT<Mesh>::drawPickingFaces_opt( const GLMatrixf& _mvp, int _pickOffset )
{
  // optimized version which computes picking ids in the shader

  /* optimization idea:
  - reuse draw buffers of drawmesh
  - create lookup table which maps from draw triangle id to openmesh face id (stored in textureBuffer)
  - get base offset of face picking: pickFaceOffset = _state.pick_get_name_color(0)
  - render with following fragment shader, which computes face picking ids on the fly:

  uniform int pickFaceOffset;
  uniform isamplerBuffer triangleToFaceMap;

  out int outPickID; // if possible to write integer. otherwise, convert to ubyte4 or sth.

  void main()
  {
    // map from triangle id to face id
    int faceID = texelFetch(triangleToFaceMap, gl_PrimitiveID);

    outPickID = pickFaceOffset + faceID;
    // maybe the integer id has to be converted to a vec4 color here, not sure
  }

  -> no rendering from sysmem buffers
  -> no maintenance/update of picking colors required
  -> lower memory footprint: 4 bytes per triangle

  keep old approach to stay compatible on systems which do not support texture buffers or integer arithmetic in shaders
  */

  if (!numTris_)
    return;

  // test support by loading and compiling picking shader
  if (!supportsPickingFaces_opt())
    return;

  // reuse cache optimized draw buffers
  ACG::GLState::bindBufferARB(GL_ARRAY_BUFFER_ARB, getVBO());
  ACG::GLState::bindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, getIBO());

  // setup picking shader
  pickFaceShader_->use();
  getVertexDeclaration()->activateShaderPipeline(pickFaceShader_);

  pickFaceShader_->setUniform("pickFaceOffset", _pickOffset);
  pickFaceShader_->setUniform("triToFaceMap", 0);

#ifdef GL_ARB_texture_buffer_object
  pickFaceTriangleMap_opt()->bind(GL_TEXTURE0);
#endif

  pickFaceShader_->setUniform("mWVP", _mvp);

  // draw call
  glDrawElements(GL_TRIANGLES, getNumTris() * 3, getIndexType(), 0);

  // restore gl state      
  getVertexDeclaration()->deactivateShaderPipeline(pickFaceShader_);
  pickFaceShader_->disable();

  // unbind draw buffers
  ACG::GLState::bindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
  ACG::GLState::bindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
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
void DrawMeshT<Mesh>::updatePickingAny_opt(ACG::GLState& _state )
{
  updatePickingFaces_opt(_state);
  updatePickingEdges_opt(_state);
  updatePickingVertices_opt(_state);
  
  // optimized any picking does not require separate picking buffers
}

template <class Mesh>
bool ACG::DrawMeshT<Mesh>::supportsPickingAny_opt()
{
  return supportsPickingFaces_opt() && supportsPickingEdges_opt() && supportsPickingVertices_opt();
}


template <class Mesh>
void ACG::DrawMeshT<Mesh>::drawPickingAny_opt( const GLMatrixf& _mvp, int _pickOffset )
{
  // optimized version which computes picking ids in the shader

  /* optimization:
    draw picking ids of faces, edges and vertices with appropriate offsets
  */

  // test support by loading and compiling picking shader
  if (!supportsPickingAny_opt())
    return;

  drawPickingFaces_opt(_mvp, _pickOffset);

  ACG::GLState::depthFunc(GL_LEQUAL);

  drawPickingEdges_opt(_mvp, _pickOffset + mesh_.n_faces());
  
  drawPickingVertices_opt(_mvp, _pickOffset + mesh_.n_faces() + mesh_.n_edges());
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
void ACG::DrawMeshT<Mesh>::createVertexDeclaration()
{
  vertexDecl_->clear();

  vertexDecl_->addElement(GL_FLOAT, 3, VERTEX_USAGE_POSITION);
  vertexDecl_->addElement(GL_FLOAT, 2, VERTEX_USAGE_TEXCOORD);
  vertexDecl_->addElement(GL_FLOAT, 3, VERTEX_USAGE_NORMAL);
  vertexDecl_->addElement(GL_UNSIGNED_BYTE, 4, VERTEX_USAGE_COLOR);

  for (size_t i = 0; i < additionalElements_.size(); ++i)
  {
    VertexProperty* prop = &additionalElements_[i];

    // invalidate detected type
    prop->sourceType_.numElements_ = 0;
    prop->sourceType_.pointer_ = 0;
    prop->sourceType_.type_ = 0;
    prop->sourceType_.shaderInputName_ = 0;
    prop->sourceType_.usage_ = VERTEX_USAGE_SHADER_INPUT;
    prop->propDataPtr_ = 0;
    prop->declElementID_ = -1;

    // get property handle in openmesh by name
    OpenMesh::BaseProperty* baseProp = 0;

    switch (prop->source_)
    {
    case PROPERTY_SOURCE_VERTEX: baseProp = mesh_._get_vprop(prop->name_); break;
    case PROPERTY_SOURCE_FACE: baseProp = mesh_._get_fprop(prop->name_); break;
    case PROPERTY_SOURCE_HALFEDGE: baseProp = mesh_._get_hprop(prop->name_); break;
    default: baseProp = mesh_._get_vprop(prop->name_); break;
    }

    // detect data type of property
    prop->propDataPtr_ = getMeshPropertyType(baseProp, &prop->sourceType_.type_, &prop->sourceType_.numElements_);


    if (prop->propDataPtr_)
    {
      prop->sourceType_.shaderInputName_ = prop->name_.c_str();

      // should have same type in vbo
      prop->destType_ = prop->sourceType_;

      prop->destType_.shaderInputName_ = prop->vertexShaderInputName_.c_str();

      prop->declElementID_ = int(vertexDecl_->getNumElements());

      vertexDecl_->addElement(&prop->destType_);
    }
    else
      std::cerr << "Could not detect data type of property " << prop->name_ << std::endl;
  }
}


template <class Mesh>
void ACG::DrawMeshT<Mesh>::updateEdgeHalfedgeVertexDeclarations()
{
  vertexDeclEdgeCol_->clear();
  vertexDeclEdgeCol_->addElement(GL_FLOAT, 3, VERTEX_USAGE_POSITION, perEdgeVertexBuffer());
  vertexDeclEdgeCol_->addElement(GL_UNSIGNED_BYTE, 4, VERTEX_USAGE_COLOR, perEdgeColorBuffer());

  vertexDeclHalfedgeCol_->clear();
  vertexDeclHalfedgeCol_->addElement(GL_FLOAT, 3, VERTEX_USAGE_POSITION, perHalfedgeVertexBuffer());
  vertexDeclHalfedgeCol_->addElement(GL_UNSIGNED_BYTE, 4, VERTEX_USAGE_COLOR, perHalfedgeColorBuffer());
  
  vertexDeclHalfedgePos_->clear();
  vertexDeclHalfedgePos_->addElement(GL_FLOAT, 3, VERTEX_USAGE_POSITION, perHalfedgeVertexBuffer());

  vertexDeclEdgeCol_->setVertexStride(0);
  vertexDeclHalfedgeCol_->setVertexStride(0);
  vertexDeclHalfedgePos_->setVertexStride(0);
}

template <class Mesh>
VertexDeclaration* ACG::DrawMeshT<Mesh>::getVertexDeclaration()
{
  return vertexDecl_;
}


template<class Mesh>
typename Mesh::HalfedgeHandle ACG::DrawMeshT<Mesh>::mapToHalfedgeHandle(int _vertexId)
{
  int faceId = -1, cornerId = -1;

  // map to halfedge handle
  if (meshComp_)
    meshComp_->mapToOriginalVertexID(_vertexId, faceId, cornerId);

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


template <class Mesh>
void ACG::DrawMeshT<Mesh>::writeVertexElement( void* _dstBuf, unsigned int _vertex, unsigned int _stride, unsigned int _elementOffset, unsigned int _elementSize, const void* _elementData )
{
  // byte offset
  unsigned int offset = _vertex * _stride + _elementOffset;

  // write address
  char* dst = static_cast<char*>(_dstBuf) + offset;

  // copy
  memcpy(dst, _elementData, _elementSize);
}

template <class Mesh>
void ACG::DrawMeshT<Mesh>::writePosition( unsigned int _vertex, const ACG::Vec3d& _n )
{
  // store float3 position
  float f3[3] = {float(_n[0]), float(_n[1]), float(_n[2])};

  writeVertexElement(&vertices_[0], _vertex, vertexDecl_->getVertexStride(), 0, 12, f3);
}

template <class Mesh>
void ACG::DrawMeshT<Mesh>::writeNormal( unsigned int _vertex, const ACG::Vec3d& _n )
{
  // store float3 normal
  float f3[3] = {float(_n[0]), float(_n[1]), float(_n[2])};

  writeVertexElement(&vertices_[0], _vertex, vertexDecl_->getVertexStride(), offsetNormal_, 12, f3);
}


template <class Mesh>
void ACG::DrawMeshT<Mesh>::writeTexcoord( unsigned int _vertex, const ACG::Vec2f& _uv )
{
  writeVertexElement(&vertices_[0], _vertex, vertexDecl_->getVertexStride(), offsetTexc_, 8, _uv.data());
}

template <class Mesh>
void ACG::DrawMeshT<Mesh>::writeColor( unsigned int _vertex, unsigned int _color )
{
  writeVertexElement(&vertices_[0], _vertex, vertexDecl_->getVertexStride(), offsetColor_, 4, &_color);
}


template <class Mesh>
void ACG::DrawMeshT<Mesh>::writeVertexProperty( unsigned int _vertex, const VertexElement* _elementDesc, const ACG::Vec4f& _propf )
{
  unsigned int elementSize = VertexDeclaration::getElementSize(_elementDesc);

  writeVertexElement(&vertices_[0], _vertex, vertexDecl_->getVertexStride(), _elementDesc->getByteOffset(), elementSize, _propf.data());
}



template <class Mesh>
void ACG::DrawMeshT<Mesh>::addVertexElement( const std::string& _propertyName, PropertySource _source )
{
  // check if element has already been requested before

  for (size_t i = 0; i < additionalElements_.size(); ++i)
  {
    if (additionalElements_[i].name_ == _propertyName)
    {
      additionalElements_[i].source_ = _source;

      return;
    }
  }


  // request new property
  VertexProperty prop;

  prop.name_ = _propertyName;
  prop.source_ = _source;
  prop.vertexShaderInputName_ = _propertyName;
  // rest of property desc is initialized later in createVertexDeclaration()

  additionalElements_.push_back(prop);

  updateFull();
}


template <class Mesh>
bool ACG::DrawMeshT<Mesh>::scanVertexShaderForInput( const std::string& _vertexShaderFile )
{
  bool success = true;

  std::ifstream file;

  file.open(_vertexShaderFile.c_str(), std::ios_base::in);

  if (file.is_open())
  {
    while (!file.eof())
    {
      char line[0xff];

      file.getline(line, 0xff);

      // get rid of whitespaces at begin/end, and internal padding
      QString strLine = line;
      strLine = strLine.simplified();

      // pattern matching for vertex input attributes
      if (!strLine.startsWith("//") && !strLine.startsWith("*/"))
      {

        if (strLine.startsWith("in ") || strLine.contains(" in "))
        {
          // extract 
          int semIdx = strLine.indexOf(';');


          if (semIdx >= 0)
          {
            // property name = string before semicolon without whitespace
            
            // remove parts after semicolon
            QString strName = strLine;
            strName.remove(semIdx, strName.length());

            strName = strName.simplified();

            // property name = string between last whitespace and last character
            int lastWhite = strName.lastIndexOf(' ');
            
            if (lastWhite >= 0)
            {
              strName.remove(0, lastWhite);

              strName = strName.simplified();

              // check for reserved input attributes
              if (strName != "inPosition" &&
                  strName != "inTexCoord" &&
                  strName != "inNormal" &&
                  strName != "inColor")
              {
                // custom property

                std::string propName = strName.toStdString();

                // choose property source
                PropertySource src = PROPERTY_SOURCE_VERTEX;

                if (strLine.contains("flat "))
                {
                  // per face attribute
                  src = PROPERTY_SOURCE_FACE;

                  if (!mesh_._get_fprop(propName))
                    src = PROPERTY_SOURCE_VERTEX; // face source not available, try vertex next..
                }

                if (src == PROPERTY_SOURCE_VERTEX)
                {
                  if (!mesh_._get_vprop(propName))
                    src = PROPERTY_SOURCE_HALFEDGE; // vertex source not available, try halfedge next..
                }

                // prefer halfedge props over vertex props
                if (src == PROPERTY_SOURCE_VERTEX)
                {
                  if ( mesh_._get_hprop(propName) && src == PROPERTY_SOURCE_VERTEX)
                    src = PROPERTY_SOURCE_HALFEDGE;
                }

                // error output if property does not exist
                if (src == PROPERTY_SOURCE_HALFEDGE && !mesh_._get_hprop(propName))
                {
                  std::cerr << "DrawMesh error - requested property " << propName << " does not exist" << std::endl;
                  success = false;
                }
                else
                {
                  addVertexElement(propName, src);
                }

              }

            }
          }


        }

      }

    }


  }

  return success;
}

template<class Mesh>
void DrawMeshT<Mesh>::dumpObj(const char* _filename) const
{
  std::ofstream file;
  file.open(_filename);

  if (file.is_open())
  {
    for (int attrId = 0; attrId < 3; ++attrId)
    {
      for (int i = 0; i < numVerts_; ++i)
      {
        // ptr to vertex
        const char* v = &vertices_[i * vertexDecl_->getVertexStride()];

        const float* pos = reinterpret_cast<const float*>((const void*)(v + offsetPos_));
        const float* n = reinterpret_cast<const float*>((const void*)(v + offsetNormal_));
        const float* texc = reinterpret_cast<const float*>((const void*)(v + offsetTexc_));
//        unsigned int col = *reinterpret_cast<const unsigned int*>(v + offsetColor_);

        switch (attrId)
        {
        case 0: file << "v "<<pos[0]<<" "<<pos[1]<<" "<<pos[2]<<"\n"; break;
        case 1: file << "vt "<<texc[0]<<" "<<texc[1]<<"\n"; break;
        case 2: file << "vn "<<n[0]<<" "<<n[1]<<" "<<n[2]<<"\n"; break;
        default: break;
        }
      }
    }


    for (int i = 0; i < numTris_; ++i)
    {
      // ptr to triangle
      const int* tri = meshComp_->getIndexBuffer() + i*3;

      file << "f "<<tri[0]+1<<"/"<<tri[0]+1<<"/"<<tri[0]+1<<" "<<tri[1]+1<<"/"<<tri[1]+1<<"/"<<tri[1]+1<<" "<<tri[2]+1<<"/"<<tri[2]+1<<"/"<<tri[2]+1<<"\n";
    }

    file.close();
  }
}



template <class Mesh>
void ACG::DrawMeshT<Mesh>::readVertexFromVBO(unsigned int _vertex, void* _dst)
{
  assert(_dst != 0);

  unsigned int stride = vertexDecl_->getVertexStride();

  // byte offset
  unsigned int offset = _vertex * stride;

  // copy
  memcpy(_dst, &vertices_[offset], stride);
}





template <class Mesh>
void ACG::DrawMeshT<Mesh>::invalidateFullVBO()
{
  updateFullVBO_ = true;
}


template <class Mesh>
void ACG::DrawMeshT<Mesh>::updateFullVBO()
{
  // update indexed vbo first, in the next step this vbo is resolved into non-indexed
  updateGPUBuffers();

  if (updateFullVBO_)
  {
    MeshCompiler* mc = getMeshCompiler();

    if (mc)
    {
      int numTris = mc->getNumTriangles();

      // alloc buffer
      int numVerts = 3 * numTris;
      int stride = mc->getVertexDeclaration()->getVertexStride();
      std::vector<char> fullBuf(numVerts * stride);

      // fill buffer
      for (int i = 0; i < numTris; ++i)
      {
        for (int k = 0; k < 3; ++k)
        {
          int idx = i * 3 + k;
          int vertexID = mc->getIndex(idx);
          readVertexFromVBO(vertexID, &fullBuf[idx * stride]);

          if (colorMode_ == 2)
          {
            // read face color

            int faceId = meshComp_->mapToOriginalFaceID(i);
            unsigned int fcolor = getFaceColor(mesh_.face_handle(faceId));

            // store face color
            writeVertexElement(&fullBuf[0], idx, vertexDecl_->getVertexStride(), offsetColor_, 4, &fcolor);
          }

        }
      }

      if (!fullBuf.empty())
        vboFull_.upload(fullBuf.size(), &fullBuf[0], GL_STATIC_DRAW);

      // clean update flag
      updateFullVBO_ = false;
    }
  }
}





}
