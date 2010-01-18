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
  BaseNode(_parent, _name),
  mesh_(_mesh),
  stripProcessor_(_mesh),
  vertexBuffer_(0),
  vertexBufferInitialized_(false),
  enableNormals_(true),
  normalVertexBuffer_(0),
  normalVertexBufferInitialized_(false),
  enableColors_(true),
  colorVertexbuffer_(0),
  colorVertexBufferInitialized_(false),
  lineIndexBuffer_(0),
  lineIndexBufferInitialized_(false),
  enabled_arrays_(0),
  updateVertexPickingList_(true),
  vertexPickingBaseIndex_(0),
  vertexList_(0)
  
{
  /// \todo : Handle vbo not supported
  if ( ! GLEW_ARB_vertex_buffer_object ) {
    std::cerr << "Error! Vertex buffer objects are not supported! The meshNode will not work without them!" << std::endl;
  }
  
  vertexList_ = glGenLists (1);

}  

template<class Mesh>
TriStripNodeT<Mesh>::
~MeshNodeT()
{
  // Delete all allocated buffers
  if (vertexBuffer_)
    glDeleteBuffersARB(1, &vertexBuffer_);
  
  if (normalVertexBuffer_)
    glDeleteBuffersARB(1, &normalVertexBuffer_);  
  
  if (colorVertexbuffer_)
    glDeleteBuffersARB(1, &colorVertexbuffer_);    
  
  if (lineIndexBuffer_)
    glDeleteBuffersARB(1, &lineIndexBuffer_);     
  
}

template<class Mesh>
unsigned int
TriStripNodeT<Mesh>::
availableDrawModes() const {
  unsigned int drawModes(0);
  
  // We can always render points and a wireframe.
  drawModes |= DrawModes::POINTS;
  drawModes |= DrawModes::WIREFRAME;
  
  if (mesh_.has_vertex_normals())
  {
    drawModes |= DrawModes::POINTS_SHADED;  
    drawModes |= DrawModes::SOLID_SMOOTH_SHADED;
  }
  
  if (mesh_.has_vertex_colors())
  {
    drawModes |= DrawModes::POINTS_COLORED;
  }
  
  return drawModes;
}

template<class Mesh>
void
TriStripNodeT<Mesh>::
boundingBox(Vec3d& _bbMin, Vec3d& _bbMax) {
  _bbMin.minimize(bbMin_);
  _bbMax.maximize(bbMax_);
}

template<class Mesh>
void
TriStripNodeT<Mesh>::
draw(GLState& _state, unsigned int _drawMode) {
  
  /// \todo Whats this? Why is this set here
  glDepthFunc(depthFunc());

  unsigned int arrays = VERTEX_ARRAY;
  
  glPushAttrib(GL_ENABLE_BIT);
 
  if ( (_drawMode & DrawModes::POINTS) || (_drawMode & DrawModes::POINTS_COLORED) || (_drawMode & DrawModes::POINTS_SHADED )  ) {
    
    glShadeModel(GL_FLAT);
    
    if ( _drawMode & DrawModes::POINTS_SHADED  ) {
      glEnable(GL_LIGHTING);
    } else
      glDisable(GL_LIGHTING);
  
    // Use Colors in this mode if allowed
    if ( enableColors_ && (_drawMode & DrawModes::POINTS_COLORED) )
      arrays |= COLOR_VERTEX_ARRAY;
    
    // Use Normals in this mode if allowed
    if ( enableNormals_ && (_drawMode & DrawModes::POINTS_SHADED ) ) {
      arrays |= NORMAL_VERTEX_ARRAY;
      
      // If we have colors and lighting with normals, we have to use colormaterial
      if ( (arrays & COLOR_VERTEX_ARRAY) )
        glEnable(GL_COLOR_MATERIAL);
      else 
        glDisable(GL_COLOR_MATERIAL);
    }
    
    // Bring the arrays online
    enable_arrays(arrays);
    
    // Draw vertices
    draw_vertices();
  }
  
  
  /// \todo We can render also wireframe shaded and with vertex colors
  if (_drawMode & DrawModes::WIREFRAME)
  {
    enable_arrays( VERTEX_ARRAY | LINE_INDEX_ARRAY );
    glDisable(GL_LIGHTING);
    glShadeModel(GL_FLAT);
    draw_lines();
  }
  
  if ( ( _drawMode & DrawModes::SOLID_SMOOTH_SHADED ) && mesh_.has_vertex_normals() )
  {
    enable_arrays( VERTEX_ARRAY | NORMAL_VERTEX_ARRAY );
    glEnable(GL_LIGHTING);
    glShadeModel(GL_SMOOTH);
    glDepthRange(0.01, 1.0);
    draw_faces(PER_VERTEX);
    glDepthRange(0.0, 1.0);
  }
  
  enable_arrays(0);
  
  // Unbind all remaining buffers
  glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB , 0 );
  glBindBufferARB(GL_ARRAY_BUFFER_ARB , 0 );
  
  /// \todo Whats this? Why is this set here and why isnt it set to the one before entering the function?
  glDepthFunc(GL_LESS);
  
  glPopAttrib();
}

template<class Mesh>
void
TriStripNodeT<Mesh>::
draw_vertices() {
  std::cerr << "Draw vertices" << std::endl;
  if ( !vertexBufferInitialized_ )
    std::cerr << "Error! Uninitialized vertex buffer in draw call! " << std::endl;

  glDrawArrays(GL_POINTS, 0, mesh_.n_vertices());
}

template<class Mesh>
void
TriStripNodeT<Mesh>::
draw_lines() {
  if ( !lineIndexBufferInitialized_ )
    std::cerr << "Error! Uninitialized Line buffer in draw call! " << std::endl;
  
  glDrawElements(GL_LINES, mesh_.n_edges() * 2, GL_UNSIGNED_INT, 0 );
}

template<class Mesh>
void
TriStripNodeT<Mesh>::
draw_faces(FaceMode _mode) {
  std::cerr << "Draw Faces" << std::endl;
  
  if ( stripProcessor_.is_valid() ) {
    if ( _mode == PER_VERTEX ) {
      typename StripProcessorT<Mesh>::StripsIterator strip_it   = stripProcessor_.begin();
      typename StripProcessorT<Mesh>::StripsIterator strip_last = stripProcessor_.end();
      
      for (; strip_it!=strip_last; ++strip_it) {
        glDrawElements(GL_TRIANGLE_STRIP,
                       strip_it->indexArray.size(),
                       GL_UNSIGNED_INT,
                        &(strip_it->indexArray)[0]  );
      }
      
    }
  } else {
    std::cerr << "Error in draw Faces! No strip data available!" << std::endl;
  }
}

template<class Mesh>
void
TriStripNodeT<Mesh>::
enable_arrays(unsigned int _arrays) {
  
  // Unbind everything to ensure sane settings
  glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
  glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
  
  //===================================================================
  // Vertex Array
  //===================================================================
  
  // Check if we should enable the vertex array
  if (_arrays & VERTEX_ARRAY) {
    
    if ( !vertexBufferInitialized_ )
      std::cerr << "Error! Uninitialized vertex buffer! " << std::endl;
    
    // Check if its already enabled
    if (!(enabled_arrays_ & VERTEX_ARRAY)) {
      
      // Enable the vertex buffer
      enabled_arrays_ |= VERTEX_ARRAY;
   
      // Bind the Vertex buffer
      glBindBufferARB(GL_ARRAY_BUFFER_ARB, vertexBuffer_);
      
      // As we ensure that buffers are converted to float before using them, use Float here
      glVertexPointer(3, GL_FLOAT, 0, 0);
      glEnableClientState(GL_VERTEX_ARRAY);
    }
  } else if (enabled_arrays_ & VERTEX_ARRAY) {
    // Disable the Vertex array
    enabled_arrays_ &= ~VERTEX_ARRAY;
    glDisableClientState(GL_VERTEX_ARRAY);
  }
  
  
  //===================================================================
  // Normal Array
  //===================================================================
  
  // Check if we should enable the normal array
  if (_arrays & NORMAL_VERTEX_ARRAY) {
    
    if ( !normalVertexBufferInitialized_ )
      std::cerr << "Error! Uninitialized normal buffer! " << std::endl;
    
    // Check if its already enabled
    if (!(enabled_arrays_ & NORMAL_VERTEX_ARRAY)) {
      enabled_arrays_ |= NORMAL_VERTEX_ARRAY;
      
      glBindBufferARB(GL_ARRAY_BUFFER_ARB, normalVertexBuffer_);
      
      // As we ensure that buffers are converted to float before using them, use Float here
      glNormalPointer(GL_FLOAT, 0 , 0);
      
      glEnableClientState(GL_NORMAL_ARRAY);
    }
  } else if (enabled_arrays_ & NORMAL_VERTEX_ARRAY) {
    // Disable Normal array
    enabled_arrays_ &= ~NORMAL_VERTEX_ARRAY;
    glDisableClientState(GL_NORMAL_ARRAY);
  } 
  
  //===================================================================
  // Color Array
  //===================================================================  
  
  /// \todo This is different to normal and vertex buffer since it uses openmesh colors directly! Check for different color representations in OpenMesh!
  // Check if we should enable the color array
  if (_arrays & COLOR_VERTEX_ARRAY)  {
    
    // Check if its already enabled
    if (!(enabled_arrays_ & COLOR_VERTEX_ARRAY)) {
      enabled_arrays_ |= COLOR_VERTEX_ARRAY;
      
      glBindBufferARB(GL_ARRAY_BUFFER_ARB, colorVertexbuffer_);
      
      glColorPointer(3, GL_UNSIGNED_BYTE , 0 , 0);
      
      glEnableClientState(GL_COLOR_ARRAY);
      
    }
  } else if (enabled_arrays_ & COLOR_VERTEX_ARRAY) {
    // Disable Color array
    enabled_arrays_ &= ~COLOR_VERTEX_ARRAY;
    glDisableClientState(GL_COLOR_ARRAY);
  } 
  
  //===================================================================
  // Line Index Array
  //===================================================================  
  
  // Check if we should enable the color array
  if (_arrays & LINE_INDEX_ARRAY)  {
    
    // Check if its already enabled
    if (!(enabled_arrays_ & LINE_INDEX_ARRAY)) {
      enabled_arrays_ |= LINE_INDEX_ARRAY;
      
      glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, lineIndexBuffer_);
    }
  } else if (enabled_arrays_ & LINE_INDEX_ARRAY) {
    // Disable Color array
    enabled_arrays_ &= ~LINE_INDEX_ARRAY;
    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
  } 
  
  //===================================================================
  // Check for OpenGL Errors
  //===================================================================    
  glCheckErrors();
}

template<class Mesh>
void
TriStripNodeT<Mesh>::
pick(GLState& _state, PickTarget _target) {
  switch (_target)
  {
    case PICK_VERTEX:
    {
      pick_vertices(_state);
      break;
    }
    case PICK_FRONT_VERTEX:
    {
      pick_vertices(_state, true);
      break;
    }
    
    case PICK_ANYTHING:
    {
      pick_any(_state);
      break;
    }
    case PICK_FACE:
    {
      pick_faces(_state);
      break;
    }
    
    case PICK_EDGE:
    {
      pick_edges(_state);
      break;
    }
    
    case PICK_FRONT_EDGE:
    {
      pick_edges(_state, true);
      break;
    }
    
    default:
      break;
  }
}

template<class Mesh>
void
TriStripNodeT<Mesh>::
pick_vertices(GLState& _state, bool _front)
{
  typename Mesh::ConstVertexIter v_it(mesh_.vertices_begin()),
  v_end(mesh_.vertices_end());
  
  
  if (!_state.pick_set_maximum (mesh_.n_vertices())) {
    omerr() << "MeshNode::pick_vertices: color range too small, "
    << "picking failed\n";
    return;
  }
  
  if (_front) {
    std::cerr << "Draw mesh to hide non-front vertices" << std::endl;
    enable_arrays(VERTEX_ARRAY);
    
    Vec4f  clear_color = _state.clear_color();
    Vec4f  base_color  = _state.base_color();
    clear_color[3] = 1.0;
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    _state.set_base_color(clear_color);
    
    glDepthRange(0.01, 1.0);
    draw_faces(PER_VERTEX);
    glDepthRange(0.0, 1.0);
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDepthFunc(GL_LEQUAL);
    _state.set_base_color(base_color);
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
    enable_arrays(0);
  }
  
  
  if (vertexList_ && !updateVertexPickingList_ && _state.pick_current_index () == vertexPickingBaseIndex_) {
    std::cerr << "Call list" << std::endl;
    glCallList (vertexList_);
    if (_front)
      glDepthFunc(depthFunc());
    return;
  }
  
  if (vertexList_) {
    std::cerr << "Generate list" << std::endl;
    glNewList (vertexList_, GL_COMPILE);
    updateVertexPickingList_ = false;
    vertexPickingBaseIndex_ = _state.pick_current_index ();
  }
  
  if (_state.color_picking ()) {
    std::cerr << "Do color picking" << std::endl;
    
    stripProcessor_.update_picking_vertices(_state);
    
    enable_arrays(VERTEX_ARRAY);
    
    // For this version we load the colors directly not from vbo
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
    glColorPointer( stripProcessor_.pickColorBuffer() );   
    glEnableClientState(GL_COLOR_ARRAY);    
    
    // Draw color picking
    glDrawArrays(GL_POINTS, 0, mesh_.n_vertices());
    
    // Disable color array
    glDisableClientState(GL_COLOR_ARRAY);
    
    // disable all other arrays
    enable_arrays(0);
    
  } else 
    std::cerr << "Fallback not available!" << std::endl;
  
  if (vertexList_) {
    std::cerr << "Finish and render list" << std::endl;
    glEndList ();
    glCallList (vertexList_);
  }
  
  if (_front)
    glDepthFunc(depthFunc());
}

template<class Mesh>
void
TriStripNodeT<Mesh>::
pick_faces(GLState& _state)
{
}

template<class Mesh>
void
TriStripNodeT<Mesh>::
pick_edges(GLState& _state, bool _front)
{
}

template<class Mesh>
void
TriStripNodeT<Mesh>::
pick_any(GLState& _state)
{
}

template<class Mesh>
void
TriStripNodeT<Mesh>::
update_geometry() {
  std::cerr << "Update geometry" << std::endl;
  
  /// \todo check the following statements. If only geometry changed, the normals,vertices have to be updated nothing else!
  /*
  updateFaceList_ = true;
  updateVertexList_ = true;
  updateEdgeList_ = true;
  updateAnyList_ = true;
  */
  
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
  if (!vertexBuffer_)  glGenBuffersARB(1,  (GLuint*) &vertexBuffer_);
  
  glBindBufferARB(GL_ARRAY_BUFFER_ARB, vertexBuffer_);
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
  // Generate normal buffer
  // ==========================================================================
  
  // Allocate it if required
  if (!normalVertexBuffer_)  glGenBuffersARB(1,  (GLuint*)  &normalVertexBuffer_);
  
  glBindBufferARB(GL_ARRAY_BUFFER_ARB, normalVertexBuffer_);
  normalVertexBufferInitialized_ = false;
  
  // Check if using floats otherwise convert to internal float array
  if ( sizeof(NormalScalar) == 4) {
    
    glBufferDataARB(GL_ARRAY_BUFFER_ARB,
                    3 * mesh_.n_vertices() * sizeof(NormalScalar),
                    mesh_.vertex_normals(),
                    GL_STATIC_DRAW_ARB);
                    
                    normalVertexBufferInitialized_ = true;
                    
  } else {
    normals_.clear();
    typename Mesh::ConstVertexIter v_it(mesh_.vertices_begin()),
    v_end(mesh_.vertices_end());
    
    for ( ; v_it != v_end ; ++v_it )
      normals_.push_back( ACG::Vec3f(mesh_.normal(v_it)) );
    
    if ( !normals_.empty() ) {
      
      glBufferDataARB(GL_ARRAY_BUFFER_ARB,
                      3 * mesh_.n_vertices() * sizeof(float),
                      &normals_[0],
                      GL_STATIC_DRAW_ARB);
                      normalVertexBufferInitialized_ = true;
    }
    
  }

  // ==========================================================================
  // Generate color buffer
  // ==========================================================================

  // Allocate it if required
  if (!colorVertexbuffer_)  glGenBuffersARB(1,  (GLuint*)  &colorVertexbuffer_);

  glBindBufferARB(GL_ARRAY_BUFFER_ARB, colorVertexbuffer_);
  colorVertexBufferInitialized_ = false;

  /// \todo Convert from other types in openmesh to the right representation in the node
  // Check if using floats otherwise convert to internal float array
  if ( sizeof(ColorScalar) == 1) {
    
    glBufferDataARB(GL_ARRAY_BUFFER_ARB,
                    3 * mesh_.n_vertices() * sizeof(ColorScalar),
                    mesh_.vertex_colors(),
                    GL_STATIC_DRAW_ARB);
                    
                    colorVertexBufferInitialized_ = true;
                    
  } else {
   std::cerr << "Invalid color format!" << std::endl; 
  }/*else {
    normals_.clear();
    typename Mesh::ConstVertexIter v_it(mesh_.vertices_begin()),
    v_end(mesh_.vertices_end());
    
    for ( ; v_it != v_end ; ++v_it )
      normals_.push_back( ACG::Vec3f(mesh_.normal(v_it)) );
    
    if ( !normals_.empty() ) {
      
      glBufferDataARB(GL_ARRAY_BUFFER_ARB,
                      3 * mesh_.n_vertices() * sizeof(float),
                      &normals_[0],
                      GL_STATIC_DRAW_ARB);
                      normalVertexBufferInitialized_ = true;
    }
    
  }*/

  // ==========================================================================
  // unbind all buffers
  // ==========================================================================
  glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
  
}

template<class Mesh>
void
TriStripNodeT<Mesh>::
update_topology() {
  std::cerr << "Update topology" << std::endl;
  
  // ==========================================================================
  // Clear the strips and regenerate them!
  // ==========================================================================
  stripProcessor_.clear();
  stripProcessor_.stripify();
  
  std::cerr << "Created " << stripProcessor_.n_strips() << " strips\n" << std::endl;
  
  // ==========================================================================
  // Generate a buffer for rendering all lines
  // ==========================================================================
  lineIndices_.clear();
  
  typename Mesh::ConstEdgeIter e_it(mesh_.edges_begin()),
  e_end(mesh_.edges_end());
  
  for ( ; e_it != e_end ; ++e_it ) {
    lineIndices_.push_back( mesh_.from_vertex_handle(mesh_.halfedge_handle(e_it,0)).idx() );
    lineIndices_.push_back( mesh_.to_vertex_handle(mesh_.halfedge_handle(e_it,0)).idx()  );
  }
  
  // Allocate it if required
  if (!lineIndexBuffer_)  glGenBuffersARB(1,  (GLuint*) &lineIndexBuffer_);
  
  glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, lineIndexBuffer_);
  lineIndexBufferInitialized_ = false;
  
  // Copy the buffer to the graphics card
  glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB,
                  mesh_.n_edges() * sizeof(unsigned int) * 2,
                  &lineIndices_[0],
                  GL_STATIC_DRAW_ARB);

  /// \todo: clear lineIndeices (swap vector!)
  
  lineIndexBufferInitialized_ = true;
 
  // Unbind the buffer after the work has been done
  glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
}
              
//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
