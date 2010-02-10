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
 *   $Revision$                                                       *
 *   $Author$                                                      *
 *   $Date$                   *
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
  enableTexCoords_(true),
  lineIndexBuffer_(0),
  lineIndexBufferInitialized_(false),
  enabled_arrays_(0),
  updateVertexPickingList_(true),
  vertexPickingBaseIndex_(0),
  vertexPickingList_(0),
  updateEdgePickingList_(true),
  edgePickingBaseIndex_(0),
  edgePickingList_(0),
  updateFacePickingList_(true),
  facePickingBaseIndex_(0),
  facePickingList_(0),
  updateAnyPickingList_(true),
  anyPickingBaseIndex_(0),
  anyPickingList_(0),
  perFaceTextureIndexAvailable_(false),
  perFaceTextureCoordsAvailable_(false),
  textureMap_(0)
{
  
  /// \todo : Handle vbo not supported
  if ( ! GLEW_ARB_vertex_buffer_object ) {
    std::cerr << "Error! Vertex buffer objects are not supported! The meshNode will not work without them!" << std::endl;
  }
  
  
  vertexPickingList_ = glGenLists(1);
  edgePickingList_   = glGenLists(1);
  facePickingList_   = glGenLists(1);
  anyPickingList_    = glGenLists(3);

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
  
  if (vertexPickingList_)
    glDeleteLists (vertexPickingList_, 1);
  
  if (edgePickingList_)
    glDeleteLists (edgePickingList_, 1);
  
  if (facePickingList_)
    glDeleteLists (facePickingList_, 1);
  
  if (anyPickingList_)
    glDeleteLists (anyPickingList_, 3);
  
}

template<class Mesh>
unsigned int
TriStripNodeT<Mesh>::
availableDrawModes() const {
  unsigned int drawModes(0);
  
  // We can always render points and a wireframe.
  drawModes |= DrawModes::POINTS;
  drawModes |= DrawModes::HIDDENLINE;
  drawModes |= DrawModes::WIREFRAME;
  
  if (mesh_.has_vertex_normals())
  {
    drawModes |= DrawModes::POINTS_SHADED;  
    drawModes |= DrawModes::SOLID_SMOOTH_SHADED;
    drawModes |= DrawModes::SOLID_PHONG_SHADED;
  }
  
  if (mesh_.has_face_normals())
    drawModes |= DrawModes::SOLID_FLAT_SHADED;
  
  if (mesh_.has_vertex_colors())
  {
    drawModes |= DrawModes::POINTS_COLORED;
  }
  
  if (mesh_.has_face_colors()) {
    drawModes |= DrawModes::SOLID_FACES_COLORED;
    
    if( mesh_.has_face_normals() )
      drawModes |= DrawModes::SOLID_FACES_COLORED_FLAT_SHADED;
  }
  
  if ( mesh_.has_vertex_texcoords2D() ) {
    drawModes |= DrawModes::SOLID_TEXTURED;
    
    if (mesh_.has_vertex_normals())
      drawModes |= DrawModes::SOLID_TEXTURED_SHADED; 
  }
  
  if ( perFaceTextureCoordsAvailable_ ) {
    drawModes |= DrawModes::SOLID_2DTEXTURED_FACE;
    
    if (mesh_.has_face_normals())
      drawModes |= DrawModes::SOLID_2DTEXTURED_FACE_SHADED;
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
  
  if (_drawMode & DrawModes::HIDDENLINE)
  {
    enable_arrays(VERTEX_ARRAY);
    
    // First:
    // Render all faces in background color to initialize z-buffer
    Vec4f  clear_color = _state.clear_color();
    Vec4f  base_color  = _state.base_color();
    clear_color[3] = 1.0;
    
    glDisable(GL_LIGHTING);
    glShadeModel(GL_FLAT);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    _state.set_base_color(clear_color);
    
    glDepthRange(0.01, 1.0);
    draw_faces(PER_VERTEX);
    glDepthRange(0.0, 1.0);
    
    // Second
    // Render the lines. All lines not on the front will be skipped in z-test
    enable_arrays(VERTEX_ARRAY|LINE_INDEX_ARRAY);
    glDepthFunc(GL_LEQUAL);
    _state.set_base_color(base_color);
    draw_lines();
    glDepthFunc(depthFunc());
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }
  
  if ( ( _drawMode & DrawModes::SOLID_FLAT_SHADED ) && mesh_.has_face_normals())
  {
    glEnable(GL_LIGHTING);
    glShadeModel(GL_FLAT);
    glDepthRange(0.01, 1.0);
    enable_arrays(PER_FACE_VERTEX_ARRAY | PER_FACE_NORMAL_ARRAY);
    draw_faces(PER_FACE);
    glDepthRange(0.0, 1.0);
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
  
  if ( ( _drawMode & DrawModes::SOLID_PHONG_SHADED ) && mesh_.has_vertex_normals() )
  {
    ///\todo Integrate shader here! 
    //     if ( parent() != 0 ) {
    //       if ( parent()->className() == "ShaderNode" ) {
    //
    //         ShaderNode* node = dynamic_cast< ShaderNode* > ( parent() );
    //
    //         GLSL::PtrProgram program = node->getShader( DrawModes::SOLID_PHONG_SHADED );
    //
    //         // Enable own Phong shader
    //         program->use();
    
    enable_arrays(VERTEX_ARRAY | NORMAL_VERTEX_ARRAY );
    glEnable(GL_LIGHTING);
    glShadeModel(GL_SMOOTH);
    glDepthRange(0.01, 1.0);
    draw_faces(PER_VERTEX);
    glDepthRange(0.0, 1.0);
    
    //disable own Phong shader
    //         program->disable();
    //       }
    //     }
  }
  
  
  if ( ( _drawMode & DrawModes::SOLID_FACES_COLORED )&& mesh_.has_face_colors())
  {
    Vec4f base_color_backup = _state.base_color();
    
    glDisable(GL_LIGHTING);
    glShadeModel(GL_FLAT);
    glDepthRange(0.01, 1.0);
    enable_arrays(PER_FACE_VERTEX_ARRAY | PER_FACE_COLOR_ARRAY);    
    draw_faces(PER_FACE);
    glDepthRange(0.0, 1.0);
    
    _state.set_base_color(base_color_backup);
  }
  
  
  if ( ( _drawMode & DrawModes::SOLID_FACES_COLORED_FLAT_SHADED ) && mesh_.has_face_colors() && mesh_.has_face_normals())
  {
    Vec4f base_color_backup = _state.base_color();
    glEnable(GL_LIGHTING);
    
    glShadeModel(GL_FLAT);
    glDepthRange(0.01, 1.0);
    enable_arrays(PER_FACE_VERTEX_ARRAY | PER_FACE_COLOR_ARRAY | PER_FACE_NORMAL_ARRAY );
    draw_faces(PER_FACE);
    glDepthRange(0.0, 1.0);
    
    _state.set_base_color(base_color_backup);
  }
  
  
  if ( ( _drawMode & DrawModes::SOLID_TEXTURED )  && mesh_.has_vertex_texcoords2D())
  {
    ///\todo enableTexCoords_
    enable_arrays(VERTEX_ARRAY | TEXCOORD_VERTEX_ARRAY );
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glShadeModel(GL_FLAT);
    glDepthRange(0.01, 1.0);
    draw_faces(PER_VERTEX);
    glDepthRange(0.0, 1.0);
    glDisable(GL_TEXTURE_2D);
  }
  
  if ( ( _drawMode & DrawModes::SOLID_TEXTURED_SHADED ) && mesh_.has_vertex_texcoords2D() && mesh_.has_vertex_normals())
  {
    enable_arrays(VERTEX_ARRAY | NORMAL_VERTEX_ARRAY | TEXCOORD_VERTEX_ARRAY);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
    glShadeModel(GL_SMOOTH);
    glDepthRange(0.01, 1.0);
    draw_faces(PER_VERTEX);
    glDepthRange(0.0, 1.0);
    glDisable(GL_TEXTURE_2D);
  }
  
  
  
  // Textured by using coordinates stored in halfedges ... arrays generated by stripprocessor
  if ( _drawMode & DrawModes::SOLID_2DTEXTURED_FACE )
  {
    glEnable(GL_TEXTURE_2D);
    
    enable_arrays( PER_FACE_VERTEX_ARRAY | PER_FACE_TEXCOORD_ARRAY );
    
    glDisable(GL_LIGHTING);
    glShadeModel(GL_FLAT);
    glDepthRange(0.01, 1.0);
    draw_faces(PER_FACE);
    glDepthRange(0.0, 1.0);
    
    glDisable(GL_TEXTURE_2D);
  }
  
  // Textured by using coordinates stored in halfedges
  if ( ( _drawMode & DrawModes::SOLID_2DTEXTURED_FACE_SHADED ) && mesh_.has_face_normals())
  {
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
    
    enable_arrays( PER_FACE_VERTEX_ARRAY | PER_FACE_TEXCOORD_ARRAY | PER_FACE_NORMAL_ARRAY );
    
    glShadeModel(GL_FLAT);
    glDepthRange(0.01, 1.0);
    draw_faces(PER_FACE);
    glDepthRange(0.0, 1.0);
    glDisable(GL_TEXTURE_2D);
    
  }
  
  enable_arrays(0);
  
  // Unbind all remaining buffers
  glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB , 0 );
  
  /// \todo Whats this? Why is this set here and why isnt it set to the one before entering the function?
  glDepthFunc(GL_LESS);
  
  glPopAttrib();
}

template<class Mesh>
void
TriStripNodeT<Mesh>::
draw_vertices() {
  if ( !vertexBufferInitialized_ )
    std::cerr << "Error! Uninitialized vertex buffer in draw call! " << std::endl;

  glDrawArrays(GL_POINTS, 0, mesh_.n_vertices());
}

template<class Mesh>
void
TriStripNodeT<Mesh>::
draw_lines() {

  if ( enabled_arrays_ & LINE_INDEX_ARRAY )
    
    // Check if array is set up correctly
    if ( lineIndexBufferInitialized_ )
      glDrawElements(GL_LINES, mesh_.n_edges() * 2, GL_UNSIGNED_INT, 0 );
    else
      std::cerr << "Error! Uninitialized Line buffer in draw call! " << std::endl;
    
  // If we are rendering per edge per vertex attributes, we need to use a seperated vertex buffer!
  else if ( enabled_arrays_ & PER_EDGE_VERTEX_ARRAY )
    glDrawArrays(GL_LINES, 0, mesh_.n_edges() * 2);
  
  // Something went wrong here!
  else
    std::cerr << "Unable to Draw! array configuration is invalid!!" << std::endl;
}

template<class Mesh>
void
TriStripNodeT<Mesh>::
draw_faces(FaceMode _mode) {
  
  // If we have all properties per Vertex, we can render with index array from triangle strips!
  if (_mode == PER_VERTEX) {
    
    // Update strips if necessary
    stripProcessor_.stripify();
    
    typename StripProcessorT<Mesh>::StripsIterator strip_it   = stripProcessor_.begin();
    typename StripProcessorT<Mesh>::StripsIterator strip_last = stripProcessor_.end();
    
    
    
    for (; strip_it!=strip_last; ++strip_it) {
      glDrawElements(GL_TRIANGLE_STRIP,
                    strip_it->indexArray.size(),
                    GL_UNSIGNED_INT,
                      &(strip_it->indexArray)[0]  );
    }
  } else if ( _mode ==  PER_FACE ) {
    
    if ( stripProcessor_.perFaceTextureIndexAvailable()  && (textureMap_ != 0) ) {

      int lastTexture = -1;
      int texture = 0;
      
      for ( uint i = 0 ; i < stripProcessor_.textureRenderData()->size() ; ++i ) {
        int texture =  (*stripProcessor_.textureRenderData())[i].textureId;
        if ( texture != lastTexture) {

          if ( textureMap_->find(texture) == textureMap_->end() ) {
            std::cerr << "Illegal texture index ... trying to access " << texture << std::endl;
            lastTexture = -1;
            continue;
          }
          
          glBindTexture( GL_TEXTURE_2D, (*textureMap_)[texture] );          
          
          lastTexture = texture;
        }
        
        // We need per face attributes so we have to use seperate vertices per face
        glDrawArrays(GL_TRIANGLES, (*stripProcessor_.textureRenderData())[i].startOffset , (*stripProcessor_.textureRenderData())[i].faceCount * 3 );
      }
      
    } else {
      
      // We need per face attributes so we have to use seperate vertices per face
      glDrawArrays(GL_TRIANGLES, 0, stripProcessor_.perFaceVertexBufferSize() );
    }
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
  if ( mesh_.has_vertex_normals() && ( _arrays & NORMAL_VERTEX_ARRAY ) ) {
    
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
  // per Vertex Color Array
  //===================================================================  
  
  /// \todo This is different to normal and vertex buffer since it uses openmesh colors directly! Check for different color representations in OpenMesh!
  // Check if we should enable the color array
  if ( mesh_.has_vertex_colors() && ( _arrays & COLOR_VERTEX_ARRAY ))  {
    
    // Check if its already enabled
    if (!(enabled_arrays_ & COLOR_VERTEX_ARRAY)) {
      enabled_arrays_ |= COLOR_VERTEX_ARRAY;
      
      glBindBufferARB(GL_ARRAY_BUFFER_ARB, colorVertexbuffer_);
      
      // Explicitly give the pointer as we uploaded the data ourself!
      glColorPointer(4, GL_FLOAT , 0 , 0);
      
      glEnableClientState(GL_COLOR_ARRAY);
      
    }
  } else if (enabled_arrays_ & COLOR_VERTEX_ARRAY) {
    // Disable Color array
    enabled_arrays_ &= ~COLOR_VERTEX_ARRAY;
    glDisableClientState(GL_COLOR_ARRAY);
  } 
  
  //===================================================================
  // per Vertex Texture coordinate Array
  //===================================================================  
  
  // Check if we should enable the color array
  if ( mesh_.has_vertex_texcoords2D() && ( _arrays & TEXCOORD_VERTEX_ARRAY ))  {
    
    // Check if its already enabled
    if (!(enabled_arrays_ & TEXCOORD_VERTEX_ARRAY)) {
      enabled_arrays_ |= TEXCOORD_VERTEX_ARRAY;
      
      glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
      
      // Explicitly give the pointer as we uploaded the data ourself!
      glTexCoordPointer(2, GL_FLOAT , 0 , mesh_.texcoords2D() );
      
      glEnableClientState(GL_TEXTURE_COORD_ARRAY);
      
    }
  } else if (enabled_arrays_ & TEXCOORD_VERTEX_ARRAY) {
    // Disable TexCoord array
    enabled_arrays_ &= ~TEXCOORD_VERTEX_ARRAY;
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  }   
  
  //===================================================================
  // per Edge Vertex Array
  //===================================================================  
  
  // Check if we should enable the per face vertex array
  if (_arrays & PER_EDGE_VERTEX_ARRAY)  {
    
    // Check if its already enabled
    if (!(enabled_arrays_ & PER_EDGE_VERTEX_ARRAY)) {
      enabled_arrays_ |= PER_EDGE_VERTEX_ARRAY;
      
      // For this version we load the colors directly not from vbo
      glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
      glVertexPointer( stripProcessor_.perEdgeVertexBuffer() );   
      
      glEnableClientState(GL_VERTEX_ARRAY);
      
    }
  } else if (enabled_arrays_ & PER_EDGE_VERTEX_ARRAY) {
    // Disable Vertex array
    enabled_arrays_ &= ~PER_EDGE_VERTEX_ARRAY;
    glDisableClientState(GL_VERTEX_ARRAY);
  } 
  
  //===================================================================
  // per Edge Color Array
  //===================================================================  
  
  // Check if we should enable the per face vertex array
  if ( mesh_.has_edge_colors()  && ( _arrays & PER_EDGE_COLOR_ARRAY) )  {
    
    // Check if its already enabled
    if (!(enabled_arrays_ & PER_EDGE_COLOR_ARRAY)) {
      enabled_arrays_ |= PER_EDGE_COLOR_ARRAY;
      
      // For this version we load the colors directly not from vbo
      glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
      glColorPointer( stripProcessor_.perEdgeColorBuffer() );   
      
      glEnableClientState(GL_COLOR_ARRAY);
      
    }
  } else if (enabled_arrays_ & PER_EDGE_COLOR_ARRAY) {
    // Disable Vertex array
    enabled_arrays_ &= ~PER_EDGE_COLOR_ARRAY;
    glDisableClientState(GL_COLOR_ARRAY);
  }   
  
  //===================================================================
  // per Face Vertex Array
  //===================================================================  
  
  // Check if we should enable the per face vertex array
  if (_arrays & PER_FACE_VERTEX_ARRAY)  {
    
    // Check if its already enabled
    if (!(enabled_arrays_ & PER_FACE_VERTEX_ARRAY)) {
      enabled_arrays_ |= PER_FACE_VERTEX_ARRAY;
      
      // For this version we load the colors directly not from vbo
      glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
      glVertexPointer( stripProcessor_.perFaceVertexBuffer() );   
      
      glEnableClientState(GL_VERTEX_ARRAY);
      
    }
  } else if (enabled_arrays_ & PER_FACE_VERTEX_ARRAY) {
    // Disable Vertex array
    enabled_arrays_ &= ~PER_FACE_VERTEX_ARRAY;
    glDisableClientState(GL_VERTEX_ARRAY);
  } 

  //===================================================================
  // per Face Normal Array
  //===================================================================  

  // Check if we should enable the per face normal array
  if (mesh_.has_face_normals() && (_arrays & PER_FACE_NORMAL_ARRAY) )  {
    
    // Check if its already enabled
    if (!(enabled_arrays_ & PER_FACE_NORMAL_ARRAY)) {
      enabled_arrays_ |= PER_FACE_NORMAL_ARRAY;
      
      // For this version we load the colors directly not from vbo
      glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
      glNormalPointer( stripProcessor_.perFaceNormalBuffer() );   
      
      glEnableClientState(GL_NORMAL_ARRAY);
      
    }
  } else if (enabled_arrays_ & PER_FACE_NORMAL_ARRAY) {
    // Disable Normal array
    enabled_arrays_ &= ~PER_FACE_NORMAL_ARRAY;
    glDisableClientState(GL_NORMAL_ARRAY);
  } 
  
  //===================================================================
  // per Face Color Array
  //===================================================================  
  
  // Check if we should enable the per face color array
  if (mesh_.has_face_colors() && (_arrays & PER_FACE_COLOR_ARRAY) )  {
    
    // Check if its already enabled
    if (!(enabled_arrays_ & PER_FACE_COLOR_ARRAY)) {
      enabled_arrays_ |= PER_FACE_COLOR_ARRAY;
      
      // For this version we load the colors directly not from vbo
      glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
      glColorPointer( stripProcessor_.perFaceColorBuffer() );   
      
      glEnableClientState(GL_COLOR_ARRAY);
      
    }
  } else if (enabled_arrays_ & PER_FACE_COLOR_ARRAY) {
    // Disable Normal array
    enabled_arrays_ &= ~PER_FACE_COLOR_ARRAY;
    glDisableClientState(GL_COLOR_ARRAY);
  }   
  
  //===================================================================
  // per Face TexCoord Array
  //===================================================================  
  
  // Check if we should enable the per face color array
  if ( stripProcessor_.perFaceTextureCoordinateAvailable() && (_arrays & PER_FACE_TEXCOORD_ARRAY ) )  {
    
    // Check if its already enabled
    if (!(enabled_arrays_ & PER_FACE_TEXCOORD_ARRAY)) {
      enabled_arrays_ |= PER_FACE_TEXCOORD_ARRAY;
      
      // For this version we load the colors directly not from vbo
      glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
      glTexCoordPointer( stripProcessor_.perFacePerVertexTextureCoordBuffer() );   
      
      glEnableClientState(GL_TEXTURE_COORD_ARRAY );
    }
  } else if (enabled_arrays_ & PER_FACE_TEXCOORD_ARRAY) {
    // Disable Texture Coordinate array
    enabled_arrays_ &= ~PER_FACE_TEXCOORD_ARRAY;
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
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
  
  if ( _arrays == 0 ) 
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
  
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
    omerr() << "MeshNode::pick_vertices: color range too small, " << "picking failed\n";
    return;
  }
  
  if (_front) {
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
  
  
  if (vertexPickingList_ && !updateVertexPickingList_ && _state.pick_current_index () == vertexPickingBaseIndex_) {
    glCallList (vertexPickingList_);
    if (_front)
      glDepthFunc(depthFunc());
    return;
  }
  
  if (vertexPickingList_) {
    glNewList (vertexPickingList_, GL_COMPILE);
    updateVertexPickingList_ = false;
    vertexPickingBaseIndex_ = _state.pick_current_index ();
  }
  
  if (_state.color_picking ()) {
    stripProcessor_.updatePickingVertices(_state);
    
    enable_arrays(VERTEX_ARRAY);
    
    // For this version we load the colors directly not from vbo
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
    glColorPointer( stripProcessor_.pickVertexColorBuffer() );   
    glEnableClientState(GL_COLOR_ARRAY);    
    
    // Draw color picking
    glDrawArrays(GL_POINTS, 0, mesh_.n_vertices());
    
    // Disable color array
    glDisableClientState(GL_COLOR_ARRAY);
    
    // disable all other arrays
    enable_arrays(0);
    
  } else 
    std::cerr << "Fallback not available pick_vertices!" << std::endl;
  
  if (vertexPickingList_) {
    glEndList ();
    glCallList (vertexPickingList_);
  }
  
  if (_front)
    glDepthFunc(depthFunc());
}

template<class Mesh>
void
TriStripNodeT<Mesh>::
pick_edges(GLState& _state, bool _front)
{

  if (!_state.pick_set_maximum (mesh_.n_edges())) {
    omerr() << "MeshNode::pick_edges: color range too small, " << "picking failed\n";
    return;
  }
  
  if (_front) {
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
    
    // disable all other arrays
    enable_arrays(0);
  }
  
  if (edgePickingList_ && !updateEdgePickingList_ && _state.pick_current_index () == edgePickingBaseIndex_) {
    glCallList (edgePickingList_);
    if (_front)
      glDepthFunc(depthFunc());
    return;
  }
  
  if (edgePickingList_) {
    glNewList (edgePickingList_, GL_COMPILE);
    updateEdgePickingList_ = false;
    edgePickingBaseIndex_ = _state.pick_current_index ();
  }
  
  if (_state.color_picking ()) {
    stripProcessor_.updatePickingEdges(_state);
    
    // For this version we load the colors directly not from vbo
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
    
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    
    glVertexPointer (stripProcessor_.perEdgeVertexBuffer());
    glColorPointer(stripProcessor_.pickEdgeColorBuffer());
    
    glDrawArrays(GL_LINES, 0, mesh_.n_edges() * 2);
    
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);

    // disable all other arrays
    enable_arrays(0);
    
  } else {
    std::cerr << "No fallback pick_edges!" << std::endl;
  }
  
  if (edgePickingList_) {
    glEndList ();
    glCallList (edgePickingList_);
  }
  
  if (_front)
    glDepthFunc(depthFunc());
}

template<class Mesh>
void
TriStripNodeT<Mesh>::
pick_faces(GLState& _state)
{
  typename Mesh::ConstFaceIter        f_it(mesh_.faces_sbegin()),
  f_end(mesh_.faces_end());
  typename Mesh::ConstFaceVertexIter  fv_it;
  
  
  if ( mesh_.n_faces() > 0 ) {
    if (!_state.pick_set_maximum (mesh_.n_faces())) {
      omerr() << "MeshNode::pick_faces: color range too small, " << "picking failed\n";
      return;
    }
  } else {
    if (!_state.pick_set_maximum (1)) {
      omerr() << "Strange pickSetMAximum failed for index 1 in MeshNode\n";
      return;
    }
  }
  
  if (facePickingList_ && !updateFacePickingList_ && _state.pick_current_index () == facePickingBaseIndex_) {
    glCallList (facePickingList_);
    return;
  }
  
  if (facePickingList_) {
    glNewList (facePickingList_, GL_COMPILE);
    updateFacePickingList_ = false;
    facePickingBaseIndex_ = _state.pick_current_index();
  }
  
  if (_state.color_picking ()) {

    stripProcessor_.updatePickingFaces(_state);
    
    // For this version we load the colors directly not from vbo
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
    
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    
    glVertexPointer( stripProcessor_.perFaceVertexBuffer() );
    glColorPointer(  stripProcessor_.pickFaceColorBuffer() );
    
    glDrawArrays(GL_TRIANGLES, 0, stripProcessor_.perFaceVertexBufferSize() );
    
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    
    // disable all other arrays
    enable_arrays(0);
    
  } else
    std::cerr << "No fallback pick_faces!" << std::endl;
  
  if (facePickingList_) {
    glEndList ();
    glCallList (facePickingList_);
  }
}

template<class Mesh>
void
TriStripNodeT<Mesh>::
pick_any(GLState& _state)
{
  
  unsigned int numElements = mesh_.n_faces() + mesh_.n_edges() + mesh_.n_vertices();
  
  // nothing to pick ?
  if (numElements <= 0) {
    std::cerr << "Number of elements : 0 " << std::endl;
    return;
  }
  
  if (!_state.pick_set_maximum (numElements))
  {
    omerr() << "MeshNode::pick_any: color range too small, " << "picking failed\n";
    return;
  }
  
  if (anyPickingList_ && !updateAnyPickingList_ && _state.pick_current_index () == anyPickingBaseIndex_)
  {
    glCallList (anyPickingList_);
    glCallList (anyPickingList_+1);
    glCallList (anyPickingList_+2);
    return;
  }
  
  if (anyPickingList_) {
    glNewList (anyPickingList_, GL_COMPILE);
    updateAnyPickingList_ = false;
    anyPickingBaseIndex_ = _state.pick_current_index ();
  }
  
  if (_state.color_picking ()) {
    
    stripProcessor_.updatePickingAny(_state);
    
    // For this version we load the colors directly not from vbo
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
    
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    
    glVertexPointer( stripProcessor_.perFaceVertexBuffer() );
    glColorPointer(  stripProcessor_.pickFaceColorBuffer() );
    
    glDrawArrays(GL_TRIANGLES, 0, stripProcessor_.perFaceVertexBufferSize() );
    
    
    if (anyPickingList_)
    {
      glEndList ();
      glNewList (anyPickingList_+1, GL_COMPILE);
    }
    
    glDepthFunc(GL_LEQUAL);
    
    glVertexPointer (stripProcessor_.perEdgeVertexBuffer());
    glColorPointer(stripProcessor_.pickEdgeColorBuffer());
    
    glDrawArrays(GL_LINES, 0, mesh_.n_edges() * 2);
    
    if (anyPickingList_)
    {
      glEndList ();
      glNewList (anyPickingList_+2, GL_COMPILE);
    }
    
    enable_arrays(VERTEX_ARRAY);
    
    // For this version we load the colors directly not from vbo
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
    glColorPointer( stripProcessor_.pickVertexColorBuffer() );   
    glEnableClientState(GL_COLOR_ARRAY);    
    
    // Draw color picking
    glDrawArrays(GL_POINTS, 0, mesh_.n_vertices());
    
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    
    glDepthFunc(depthFunc());
    
    // disable all other arrays
    enable_arrays(0);
  }
  
  
  if (anyPickingList_)
  {
    glEndList ();
    glCallList (anyPickingList_);
    glCallList (anyPickingList_+1);
    glCallList (anyPickingList_+2);
  }

glCheckErrors();
}

template<class Mesh>
void
TriStripNodeT<Mesh>::
update_geometry() {
  std::cerr << "Update geometry" << std::endl;
  
  /// \todo check the following statements. If only geometry changed, the normals,vertices have to be updated nothing else!
  /*
  updateFaceList_ = true;
  updateAnyList_ = true;
  */
  
  updateVertexPickingList_ = true;
  updateEdgePickingList_   = true;
  updateFacePickingList_   = true;
  updateAnyPickingList_    = true;
  
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
    // Clear local conversion array
    normals_.clear();

    // Preallocate memory for efficiency
    normals_.reserve( mesh_.n_vertices() );

    // Convert data to the desired format
    typename Mesh::ConstVertexIter v_end(mesh_.vertices_end());
    for ( typename Mesh::ConstVertexIter v_it(mesh_.vertices_begin()) ; v_it != v_end ; ++v_it )
      normals_.push_back( ACG::Vec3f(mesh_.normal(v_it)) );
    
    if ( !normals_.empty() ) {
      
      // Upload to graphics card
      glBufferDataARB(GL_ARRAY_BUFFER_ARB,
                      3 * mesh_.n_vertices() * sizeof(float),
                      &normals_[0],
                      GL_STATIC_DRAW_ARB);
                      normalVertexBufferInitialized_ = true;
                      
      // As we uploaded the data to the graphics card, we can clear it in the main memory                      
      normals_.clear();
      std::vector< ACG::Vec3f >().swap(normals_);
    }
    
  }

  // ==========================================================================
  // Generate color buffer
  // ==========================================================================

  // Allocate it if required
  if (!colorVertexbuffer_)  glGenBuffersARB(1,  (GLuint*)  &colorVertexbuffer_);

  glBindBufferARB(GL_ARRAY_BUFFER_ARB, colorVertexbuffer_);
  colorVertexBufferInitialized_ = false;

  // Colors consist of 4 scalars (RGBA) with type float -> direct upload!
  if ( sizeof(ColorScalar) == 4 && mesh_.vertex_colors()->dim() == 4 ) {
    glBufferDataARB(GL_ARRAY_BUFFER_ARB,
                    4 * mesh_.n_vertices() * sizeof(ColorScalar),
                    mesh_.vertex_colors(),
                    GL_STATIC_DRAW_ARB);
                    
    colorVertexBufferInitialized_ = true;
  } else {
    // Format mismatch -> conversion
    
    // Clear the local color conversion array
    colors_.clear();
    
    // Preallocate memory for efficiency
    colors_.reserve( mesh_.n_vertices() );
    
    // Convert data to the desired format
    typename Mesh::ConstVertexIter v_end(mesh_.vertices_end());
    for ( typename Mesh::ConstVertexIter v_it(mesh_.vertices_begin()) ; v_it != v_end ; ++v_it )
      colors_.push_back( OpenMesh::color_cast<Vec4f>(mesh_.color(v_it)) );
    
    if ( !colors_.empty() ) {
      // Upload to graphics card
      glBufferDataARB(GL_ARRAY_BUFFER_ARB,
                      4 * mesh_.n_vertices() * sizeof(float),
                      &colors_[0],
                      GL_STATIC_DRAW_ARB);
                      colorVertexBufferInitialized_ = true;
      
      // As we uploaded the data to the graphics card, we can clear it in the main memory                      
      colors_.clear();
      std::vector< ACG::Vec4f >().swap(colors_);
    }
    
  }
  
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
  // Mark strips as invalid to force an update
  stripProcessor_.invalidateStrips();
  
  // Set per face arrays to invalid as they have to be regenerated
  stripProcessor_.invalidatePerFaceBuffers();
  
  // Set per edge arrays to invalid as they have to be regenerated
  stripProcessor_.invalidatePerEdgeBuffers();
  
//   std::cerr << "Created " << stripProcessor_.nStrips() << " strips\n" << std::endl;
  
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
  
  if ( !lineIndices_.empty() ) {
    // Copy the buffer to the graphics card
    glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB,
                    mesh_.n_edges() * sizeof(unsigned int) * 2,
                    &lineIndices_[0],
                    GL_STATIC_DRAW_ARB);

    /// \todo: clear lineIndices (swap vector!)
    
    lineIndexBufferInitialized_ = true;
  }
 
  // Unbind the buffer after the work has been done
  glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
}

template<class Mesh>
void
TriStripNodeT<Mesh>::
setIndexPropertyName( std::string _indexPropertyName ) { 
  stripProcessor_.setIndexPropertyName(_indexPropertyName);
  
  perFaceTextureIndexAvailable_ =  stripProcessor_.perFaceTextureIndexAvailable();
    
};

template<class Mesh>
void
TriStripNodeT<Mesh>::
setHalfedgeTextcoordPropertyName( std::string _halfedgeTextcoordPropertyName ){ 
  stripProcessor_.setPerFaceTextureCoordinatePropertyName(_halfedgeTextcoordPropertyName);
  
  perFaceTextureCoordsAvailable_ = stripProcessor_.perFaceTextureCoordinateAvailable();
  
  if (perFaceTextureCoordsAvailable_ )
    std::cerr << "perFaceTextureCoordsAvailable_" << std::endl;
  else
    std::cerr << "perFaceTextureCoordsAvailable_ not available" << std::endl;
};

//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
