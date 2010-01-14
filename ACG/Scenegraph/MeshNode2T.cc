/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2009 by Computer Graphics Group, RWTH Aachen      *
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
 *   $Revision: 7994 $                                                       *
 *   $Author: moebius $                                                      *
 *   $Date: 2009-12-18 12:18:13 +0100 (Fr, 18. Dez 2009) $                   *
 *                                                                           *
\*===========================================================================*/




//=============================================================================
//
//  CLASS MeshNodeT - IMPLEMENTATION
//
//=============================================================================

#define ACG_MESHNODE_C

//== NAMESPACES ===============================================================


namespace ACG {
namespace SceneGraph {
    

//== INCLUDES =================================================================


#include "MeshNode2T.hh"

template<class Mesh>
TriStripNodeT<Mesh>::
TriStripNodeT(Mesh&        _mesh,
              BaseNode*    _parent,
              std::string  _name ):
  mesh_(_mesh),
  vertex_buffer_(0),
  vertexBufferInitialized_(false),
  enabled_arrays_(0)
{
  /// \todo : Handle vbo not supported
  if ( ! GLEW_ARB_vertex_buffer_object ) {
    std::cerr << "Error! Vertex buffer objects are not supported! The meshNode will not work without them!" << std::endl;
  }

}  

template<class Mesh>
TriStripNodeT<Mesh>::
~MeshNodeT()
{
  // Delete all allocated buffers
  if (vertex_buffer_)
    glDeleteBuffersARB(1, (GLuint*) &vertex_buffer_);
  
}

template<class Mesh>
unsigned int
TriStripNodeT<Mesh>::
availableDrawModes() const {
  unsigned int drawModes(0);
  
  drawModes |= DrawModes::POINTS;
  
  return drawModes;
}

template<class Mesh>
void
TriStripNodeT<Mesh>::
boundingBox(Vec3d& _bbMin, Vec3d& _bbMax) {
  
}

template<class Mesh>
void
TriStripNodeT<Mesh>::
draw(GLState& _state, unsigned int _drawMode) {
  glDepthFunc(depthFunc());


  if (_drawMode & DrawModes::POINTS)
  {
    enable_arrays(VERTEX_ARRAY);
    glDisable(GL_LIGHTING);
    glShadeModel(GL_FLAT);
    draw_vertices();
  }
  
}

template<class Mesh>
void
TriStripNodeT<Mesh>::
draw_vertices() {
  glDrawArrays(GL_POINTS, 0, mesh_.n_vertices());
}

template<class Mesh>
void
TriStripNodeT<Mesh>::
enable_arrays(unsigned int _arrays) {

  if ( !vertexBufferInitialized_ )
    std::cerr << "Error! Uninitialized vertex buffer! " << std::endl;
  
  // Check if we should enable the vertex array
  if (_arrays & VERTEX_ARRAY) {
    
    // Check if its already enabled
    if (!(enabled_arrays_ & VERTEX_ARRAY)) {
      
      // Enable the vertex buffer
      enabled_arrays_ |= VERTEX_ARRAY;
      glEnableClientState(GL_VERTEX_ARRAY);
   
      // Bind the Vertex buffer
      glBindBufferARB(GL_ARRAY_BUFFER_ARB, vertex_buffer_);
      
      // As we ensure that buffers are converted to float before using them, use Float here
      glVertexPointer(3, GL_FLOAT, 0, 0);
    }
  }  else if (enabled_arrays_ & VERTEX_ARRAY) {
    // Disable the Vertex array
    enabled_arrays_ &= ~VERTEX_ARRAY;
    glDisableClientState(GL_VERTEX_ARRAY);
  }
  
}

template<class Mesh>
void
TriStripNodeT<Mesh>::
pick(GLState& _state, PickTarget _target) {
  
}

template<class Mesh>
void
TriStripNodeT<Mesh>::
update_geometry() {
  
  // First of all, we update the bounding box:
  bbMin_ = Vec3d(FLT_MAX,  FLT_MAX,  FLT_MAX);
  bbMax_ = Vec3d(-FLT_MAX, -FLT_MAX, -FLT_MAX);
  typename Mesh::ConstVertexIter  v_it(mesh_.vertices_begin()),
  v_end(mesh_.vertices_end());
  
  for (; v_it!=v_end; ++v_it)
  {
    bbMin_.minimize(mesh_.point(v_it));
    bbMax_.maximize(mesh_.point(v_it));
  }
  
  //===================================================================
  // Generate a vertex buffer on the GPU
  //===================================================================
  
  // Allocate it if required
  if (!vertex_buffer_)  glGenBuffersARB(1,  (GLuint*) &vertex_buffer_);
  
  glBindBufferARB(GL_ARRAY_BUFFER_ARB, vertex_buffer_);
  vertexBufferInitialized_ = false;
  
  //Check if using floats otherwise convert to internal float array
  if ( sizeof(PointScalar) == 4 ) {
    
    glBufferDataARB(GL_ARRAY_BUFFER_ARB,
                    3 * mesh_.n_vertices() * sizeof(PointScalar),
                    mesh_.points(),
                    GL_STATIC_DRAW_ARB);
                    
                    vertexBufferInitialized_ = true;
                    
  } else {
    
    vertices_.clear();
    typename Mesh::ConstVertexIter v_it(mesh_.vertices_begin()),
    v_end(mesh_.vertices_end());
    
    for ( ; v_it != v_end ; ++v_it )
      vertices_.push_back( ACG::Vec3f(mesh_.point(v_it)) );
    
    if ( !vertices_.empty() ) {
      
      glBufferDataARB(GL_ARRAY_BUFFER_ARB,
                      3 * mesh_.n_vertices() * sizeof(float),
                      &vertices_[0],
                      GL_STATIC_DRAW_ARB);
                      vertexBufferInitialized_ = true;
                      
    }
  }
  
  // ==========================================================================
  // unbind all buffers
  glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
  
}

template<class Mesh>
void
TriStripNodeT<Mesh>::
update_topology() {
  
}
              
//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
