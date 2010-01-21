/*===========================================================================*\
 *                                                                           *
 *                               OpenMesh                                    *
 *      Copyright (C) 2001-2009 by Computer Graphics Group, RWTH Aachen      *
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
 *   $Revision: 137 $                                                         *
 *   $Date: 2009-06-04 10:46:29 +0200 (Do, 04. Jun 2009) $                   *
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

#include <ACG/GL/GLState.hh>

//== FORWARDDECLARATIONS ======================================================


//== NAMESPACES ===============================================================

namespace ACG {
namespace SceneGraph {


//== CLASS DEFINITION =========================================================

/** \brief class for managing a single triangle strip.
*
*/
class Strip {
  
public:
    
  /** \brief This array cotains the actual triangle strip used for rendering
  *
  * Render this index array as a triangle strip!
  */
  std::vector< unsigned int > indexArray;
  
  
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
  typedef std::vector<FaceHandle>           FaceMap;
  

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

  /// are strips computed?
  bool isValid() const { return !strips_.empty(); }

  /// Access strips
  StripsIterator begin() const { return strips_.begin(); }
  /// Access strips
  StripsIterator end()   const { return strips_.end(); }


private:

  typedef std::vector<typename Mesh::FaceHandle>  FaceHandles;

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
                          FaceHandles& _faces,
                          FaceMap& _faceMap);
           
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
  
  // In case triangulation is needed
  bool                           triangulated_;
  std::vector<FaceMap>           faceMaps_;

//===========================================================================
/** @name Per face drawing arrays handling
* @{ */
//===========================================================================      

public:
  /** \brief Update the per face buffer. 
  *
  * The buffer contains triangles
  */
  void updatePerFaceBuffer();
  
  /** \brief get a pointer to the per face vertex buffer
  *
  * This function will return a pointer to the first element of the per face vertex buffer.
  * Use updatePerFaceBuffer to update the buffer before you render it via
  * glColorPointer. 
  */
  ACG::Vec3f * perFaceBuffer() { return &(perFaceBuffer_)[0]; };

private:
  /// Buffer holding vertices for per face rendering
  std::vector< ACG::Vec3f >  perFaceBuffer_;
  
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
  * glColorPointer.
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
    * glColorPointer.
    */
    ACG::Vec4uc * pickEdgeColorBuffer(){ return &(pickEdgeColorBuf_)[0]; };
    
    /** \brief get a pointer to the per edge picking vertex buffer
    *
    * This function will return a pointer to the first element of the picking buffer.
    * Use updatePickingEdges to update the buffer before you render it via
    * glColorPointer.
    */
    ACG::Vec3f * pickEdgeVertexBuffer(){ return &(pickEdgeVertexBuf_)[0]; };    
    
  private:  
    
    std::vector< ACG::Vec4uc > pickEdgeColorBuf_;
    std::vector< ACG::Vec3f >  pickEdgeVertexBuf_;
    
    
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
  * glColorPointer.
  */
  ACG::Vec4uc * pickFaceColorBuffer(){ return &(pickFaceColorBuf_)[0]; };
  
  /** \brief get a pointer to the per edge picking vertex buffer
  *
  * This function will return a pointer to the first element of the picking buffer.
  * Use updatePickingEdges to update the buffer before you render it via
  * glColorPointer.
  */
  ACG::Vec3f * pickFaceVertexBuffer(){ return &(pickFaceVertexBuf_)[0]; };    
  
  /** \brief Get the number of elements in the face picking buffers
  */
  unsigned int pickFaceBufferSize(){ return  pickFaceVertexBuf_.size(); };
  
private:  
  
  /// Call this function to update the color picking array
  void updatePickingFacesTrimesh(ACG::GLState& _state );
  
  /// Call this function to update the color picking array
  void updatePickingFacesPolymesh(ACG::GLState& _state );
  
  std::vector< ACG::Vec4uc > pickFaceColorBuf_;
  std::vector< ACG::Vec3f >  pickFaceVertexBuf_;
  
  
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
  * glColorPointer.
  */
  ACG::Vec4uc * pickAnyColorBuffer(){ return &(pickAnyColorBuf_)[0]; };
  
private:  
  
  std::vector< ACG::Vec4uc > pickAnyColorBuf_;
  
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
