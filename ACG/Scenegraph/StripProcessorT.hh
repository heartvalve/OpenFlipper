/*===========================================================================*\
 *                                                                           *
 *                               OpenMesh                                    *
 *      Copyright (C) 2001-2014 by Computer Graphics Group, RWTH Aachen      *
 *                           www.openmesh.org                                *
 *                                                                           *
 *---------------------------------------------------------------------------* 
 *  This file is part of OpenMesh.                                           *
 *                                                                           *
 *  OpenMesh is free software: you can redistribute it and/or modify         * 
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
 *  OpenMesh is distributed in the hope that it will be useful,              *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU Lesser General Public License for more details.                      *
 *                                                                           *
 *  You should have received a copy of the GNU LesserGeneral Public          *
 *  License along with OpenMesh.  If not,                                    *
 *  see <http://www.gnu.org/licenses/>.                                      *
 *                                                                           *
\*===========================================================================*/ 

/*===========================================================================*\
 *                                                                           *             
 *   $Revision$                                                         *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/

//=============================================================================
//
//  CLASS StripProcessorT
//
//=============================================================================


#ifndef ACG_STRIP_PROCESSOR_HH
#define ACG_STRIP_PROCESSOR_HH


//== INCLUDES =================================================================

#include <vector>
#include <OpenMesh/Core/Utils/Property.hh>
#include <OpenMesh/Core/IO/MeshIO.hh>

#include "../Config/ACGDefines.hh"

#include <ACG/GL/GLState.hh>

//== FORWARDDECLARATIONS ======================================================


//== NAMESPACES ===============================================================

namespace ACG {
namespace SceneGraph {


//== CLASS DEFINITION =========================================================

/** \brief class for managing a single triangle strip.
*
*/
class ACGDLLEXPORT Strip {
  
public:
    
  /** \brief This array cotains the actual triangle strip used for rendering
  *
  * Render this index array as a triangle strip!
  */
  std::vector< unsigned int > indexArray;
  
  /// This map contains for each vertex in the strips a handle to the face it closes
  std::vector< OpenMesh::FaceHandle > faceMap;
  
  /// This contains the texture index used for rendering this strip
  int textureIndex;
  
};

bool ACGDLLEXPORT stripTextureCompare(const Strip& i,const Strip& j);

class ACGDLLEXPORT TextureRenderInfo {
  public:
  TextureRenderInfo(int _textureId,int _faceCount,int _startOffset):
  textureId(_textureId),
  faceCount(_faceCount),
  startOffset(_startOffset)
  {};
  
  /// Id of the texture to be rendered
  int textureId;
  /// Number of faces in the render step for this texture
  int faceCount;
  /// Start offset in the face buffer
  int startOffset;
};



/** \class StripProcessorT StripProcessorT.hh <ACG/SceneGraph/StripProcessorT.hh>
    This class decomposes a mesh into several strips
*/

template <class Mesh>
class StripProcessorT
{
public:
    
  typedef unsigned int                      Index;
  typedef std::vector<Strip>                Strips;
  typedef typename Strips::const_iterator   StripsIterator;
  typedef typename Mesh::FaceHandle         FaceHandle;

  /// Default constructor
  StripProcessorT(Mesh& _mesh);

  /// Destructor
  ~StripProcessorT();

  /// Compute triangle strips, returns number of strips
  unsigned int stripify();

  /// delete all strips
  void clear() { Strips().swap(strips_); }

  /// returns number of strips
  unsigned int nStrips() const { return strips_.size(); }

  /// Access strips
  StripsIterator begin() const { return strips_.begin(); }
  /// Access strips
  StripsIterator end()   const { return strips_.end(); }


private:

  

  /// Test whether face is convex
  void convexityTest(FaceHandle _fh);

  OpenMesh::FPropHandleT<bool>::reference  processed(typename Mesh::FaceHandle _fh) {
    return mesh_.property(processed_, _fh);
  }
  OpenMesh::FPropHandleT<bool>::reference  used(typename Mesh::FaceHandle _fh) {
    return mesh_.property(used_, _fh);
  }



private:

  Mesh&                          mesh_;
  Strips                         strips_;
  OpenMesh::FPropHandleT<bool>   processed_, used_;
  
//===========================================================================
/** @name Strip generation
* @{ */
//===========================================================================    
public:
  
/** \brief Force a strip update 
*
*
*/
void invalidateStrips() { stripsValid_ = false; };
  
private:  
  
typedef std::vector<typename Mesh::FaceHandle>  FaceHandles;

/// This flag shows if the strips have to be regenerated
bool stripsValid_;  

/// this method does the main work
void buildStrips();

/// This method generates strips for triangle meshes
void buildStripsTriMesh();

/** This method generates strips for polyMeshes meshes
*
* The strips generated in this function are triangle strips. The function
* takes arbitrary polygons as input and triangulates them.
*/
void buildStripsPolyMesh();


/// build a strip from a given halfedge (in both directions) of a triangle mesh
void buildStripTriMesh(typename Mesh::HalfedgeHandle _start_hh,
                        Strip& _strip,
                        FaceHandles& _faces);
                        
/// build a strip from a given halfedge (in both directions) of a polymesh
void buildStripPolyMesh(typename Mesh::HalfedgeHandle _start_hh,
                        Strip& _strip,
                        FaceHandles& _faces);
                                                 
/** @} */      
  
//===========================================================================
/** @name Per edge drawing arrays handling
* @{ */
//===========================================================================    
public:  
  /** \brief get a pointer to the per edge vertex buffer
  *
  * This function will return a pointer to the first element of the vertex buffer.
  */
  ACG::Vec3f * perEdgeVertexBuffer();   

  /** \brief get a pointer to the per edge color buffer
  *
  * This function will return a pointer to the first element of the color buffer.
  */
  ACG::Vec4f * perEdgeColorBuffer();   
  
  /** \brief Update of the buffers
  *
  * This function will set all per edge buffers to invalid and will force an update
  * whe they are requested
  */
  void invalidatePerEdgeBuffers(){ updatePerEdgeBuffers_ = true; };
  
private:
  /** \brief Update all per edge drawing buffer
  *n
  * The updated buffers are: per edge vertex buffer ( 2 vertices per edge )
  */
  void updatePerEdgeBuffers();  
  
  /// Per Edge vertex buffer (glLines)
  std::vector< ACG::Vec3f >  perEdgeVertexBuffer_;
  
  /// Per Edge color buffer 
  std::vector< ACG::Vec4f >  perEdgeColorBuffer_;
  
  /// This flag controls if an update is really necessary
  bool updatePerEdgeBuffers_;

/** @} */    


//===========================================================================
/** @name Per halfedge drawing arrays handling
* @{ */
//===========================================================================    
public:  
  /** \brief get a pointer to the per edge vertex buffer
  *
  * This function will return a pointer to the first element of the vertex buffer.
  */
  ACG::Vec3f * perHalfedgeVertexBuffer();   

  /** \brief get a pointer to the per edge color buffer
  *
  * This function will return a pointer to the first element of the color buffer.
  */
  ACG::Vec4f * perHalfedgeColorBuffer();   
  
  /** \brief Update of the buffers
  *
  * This function will set all per edge buffers to invalid and will force an update
  * whe they are requested
  */
  void invalidatePerHalfedgeBuffers(){ updatePerHalfedgeBuffers_ = true; };
  
private:
  /** \brief Update all per edge drawing buffer
  *n
  * The updated buffers are: per edge vertex buffer ( 2 vertices per edge )
  */
  void updatePerHalfedgeBuffers();  
  
  
  /** \brief compute halfedge point
  * compute visulization point for halfedge (shifted to interior of face)
  */
  typename Mesh::Point halfedge_point(const typename Mesh::HalfedgeHandle _heh);

  /// Per Edge vertex buffer (glLines)
  std::vector< ACG::Vec3f >  perHalfedgeVertexBuffer_;
  
  /// Per Edge color buffer 
  std::vector< ACG::Vec4f >  perHalfedgeColorBuffer_;
  
  /// This flag controls if an update is really necessary
  bool updatePerHalfedgeBuffers_;

/** @} */    
  
//===========================================================================
/** @name Per face drawing arrays handling
* @{ */
//===========================================================================      

public:
  /** \brief get a pointer to the per face vertex buffer
  *
  * This function will return a pointer to the first element of the per face vertex buffer.
  * Use updatePerFaceBuffers to update the buffer before you render it via
  * ACG::GLState::colorPointer. 
  */
  ACG::Vec3f * perFaceVertexBuffer();
  
  /** \brief get a pointer to the per face normal buffer
  *
  * This function will return a pointer to the first element of the per face normal buffer.
  * Use updatePerFaceBuffers to update the buffer before you render it via
  * ACG::GLState::normalPointer. The buffer contains 3 equal normals per face.
  */  
  ACG::Vec3f * perFaceNormalBuffer();
  
  /** \brief get a pointer to the per face per vertex normal buffer
  *
  * This function will return a pointer to the first element of the per face per vertex normal buffer.
  * Use updatePerFaceBuffers to update the buffer before you render it via
  * ACG::GLState::normalPointer. The buffer contains 3 equal normals per face.
  */  
  ACG::Vec3f * perFacePerVertexNormalBuffer();
  
  /** \brief get a pointer to the per face color buffer
  *
  * This function will return a pointer to the first element of the per face color buffer.
  * Use updatePerFaceBuffers to update the buffer before you render it via
  * ACG::GLState::colorPointer. The buffer contains 3 equal normals per face.
  */  
  ACG::Vec4f * perFaceColorBuffer();  
  
  /** \brief Get the number of elements in the face picking buffers
  */
  unsigned int perFaceVertexBufferSize(){ return perFaceVertexBuffer_.size(); };

  /** \brief Update of the buffers
  *
  * This function will set all per face buffers to invalid and will force an update
  * whe they are requested
  */
  void invalidatePerFaceBuffers(){ updatePerFaceBuffers_ = true; };
  
private:
  /** \brief Update all per face buffers 
  *
  * The updated buffers are: per face vertex buffer, per face normal buffer (3 equal normals per face), per face color buffer (3 equal colors per face)
  */
  void updatePerFaceBuffers();
  
  /// Buffer holding vertices for per face rendering
  std::vector< ACG::Vec3f >  perFaceVertexBuffer_;
  std::vector< ACG::Vec4f >  perFaceColorBuffer_;
  std::vector< ACG::Vec3f >  perFaceNormalBuffer_;
  std::vector< ACG::Vec3f >  perFacePerVertexNormalBuffer_;

  /// This flag controls if an update is really necessary
  bool updatePerFaceBuffers_;
  
/** @} */  

//===========================================================================
/** @name vertex picking functions
* @{ */
//===========================================================================    
  
public:
  
  /** Call this function to update the color picking array
  * The _offsett value can be used to shift the color in the resulting arrays.
  * pick Any uses the offset to generate arrays for picking everything.
  */
  void updatePickingVertices(ACG::GLState& _state , uint _offset = 0);    
  
  /** \brief get a pointer to the per vertex picking color buffer
  *
  * This function will return a pointer to the first element of the picking buffer.
  * Use updatePickingVertices to update the buffer before you render it via
  * ACG::GLState::colorPointer.
  */
  ACG::Vec4uc * pickVertexColorBuffer(){ return &(pickVertexColorBuf_)[0]; };
  
private:  
  
  /// The color buffer used for vertex picking
  std::vector< ACG::Vec4uc > pickVertexColorBuf_;
  

/** @} */  

//===========================================================================
/** @name Edge picking functions
* @{ */
//===========================================================================    

  public:
    
    /** Call this function to update the color picking array
    * The _offsett value can be used to shift the color in the resulting arrays.
    * pick Any uses the offset to generate arrays for picking everything.
    */
    void updatePickingEdges(ACG::GLState& _state , uint _offset = 0 );    
    
    /** \brief get a pointer to the per edge picking color buffer
    *
    * This function will return a pointer to the first element of the picking buffer.
    * Use updatePickingEdges to update the buffer before you render it via
    * ACG::GLState::colorPointer.
    */
    ACG::Vec4uc * pickEdgeColorBuffer(){ return &(pickEdgeColorBuf_)[0]; };
    
 
    
  private:  
    
    std::vector< ACG::Vec4uc > pickEdgeColorBuf_;
    
    
/** @} */  

//===========================================================================
/** @name Face picking functions
* @{ */
//===========================================================================    

public:
  
  /// Call this function to update the color picking array
  void updatePickingFaces(ACG::GLState& _state );    
  
  /** \brief get a pointer to the per face picking color buffer
  *
  * This function will return a pointer to the first element of the picking buffer.
  * Use updatePickingFaces to update the buffer before you render it via
  * ACG::GLState::colorPointer.
  */
  ACG::Vec4uc * pickFaceColorBuffer(){ return &(pickFaceColorBuf_)[0]; };
  

  
private:  
  
  /// Call this function to update the color picking array
  void updatePickingFacesTrimesh(ACG::GLState& _state );
  
  /// Call this function to update the color picking array
  void updatePickingFacesPolymesh(ACG::GLState& _state );
  
  std::vector< ACG::Vec4uc > pickFaceColorBuf_;
  
/** @} */  
  
  //===========================================================================
  /** @name Any picking functions
  * @{ */
  //===========================================================================    
  
public:
  
  /** \brief Call this function to update the color picking array
  *
  * This function calls the updatePickingVertices, updatePickingEdges, updatePickingVertices
  * functions with an appropriate offset so that the standard arrays will be updated.
  */
  void updatePickingAny(ACG::GLState& _state );    
  
  /** \brief get a pointer to the any picking color buffer
  *
  * This function will return a pointer to the first element of the picking buffer.
  * Use updatePickingAny to update the buffer before you render it via
  * ACG::GLState::colorPointer.
  */
  ACG::Vec4uc * pickAnyColorBuffer(){ return &(pickAnyColorBuf_)[0]; };
  
private:  
  
  std::vector< ACG::Vec4uc > pickAnyColorBuf_;
  
/** @} */  

//===========================================================================
/** @name Per face Texture handling
* @{ */
//===========================================================================    

public:
  /** \brief set the name of the property used for texture index specification
  *
  * The given property name will define a texture index. The strip processor checks this
  * property and generates strips which contain only the same index. If the property is not
  * found, strips will be independend of this property
  */
  void setIndexPropertyName( std::string _indexPropertyName );
  
  /// \brief get the name of the texture index property
  const std::string& indexPropertyName() const { return textureIndexPropertyName_; };
  
  /** \brief set the name of the property used for texture coordinate
  *
  * The given property name will define per face Texture coordinates. This property has to be a
  * halfedge property. The coordinate on each edge is the texture coordinate of the to vertex.
  * If this property is not available, textures will not be processed by the strip processor.
  */
  void setPerFaceTextureCoordinatePropertyName( std::string _perFaceTextureCoordinatePropertyName );
  
  /** \brief Check if per Face Texture coordinates are available
  *
  * If this function returns true, a per face per vertex texture array is available
  */
  bool perFaceTextureCoordinateAvailable();
  
  /** \brief Check if textureindicies are available
  *
  * If this function returns true, the strip processor will respect textures during strip generation.
  * Each returned strip has than an index that has to be used as a texture index during strip rendering.
  */
  bool perFaceTextureIndexAvailable();
  
  /** \brief get a pointer to the per face per vertex texture coor buffer
  *
  * This function will return a pointer to the first element of the buffer.
  */
  ACG::Vec2f * perFacePerVertexTextureCoordBuffer(){ return &(perFaceTextureCoordArray_)[0]; };
  
  /** \brief get rendering information for texture data
  *
  * This function returns a pointer to the texture render data produced by the stripprocessor
  * each pair defines a texture index and te number of faces which have to be rendered for this 
  * texture before switching to the next one.
  *
  */
  std::vector< TextureRenderInfo >* textureRenderData(){ return &textureRenderData_; };

private:
  
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
  
  std::vector< ACG::Vec2f >  perFaceTextureCoordArray_;
  
  std::vector< TextureRenderInfo > textureRenderData_;

/** @} */  

};


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
#if defined(INCLUDE_TEMPLATES) && !defined(ACG_STRIP_PROCESSOR_C)
#define ACG_STRIP_PROCESSOR_TEMPLATES
#include "StripProcessorT.cc"
#endif
//=============================================================================
#endif // ACG_STRIP_PROCESSOR_HH defined
//=============================================================================
