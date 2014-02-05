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
//  CLASS StripProcessorT - IMPLEMENTATION
//
//=============================================================================

#define ACG_STRIP_PROCESSOR_C

//== INCLUDES =================================================================

#include <list>
#include <iostream>

#include <OpenMesh/Core/Utils/color_cast.hh>

//== NAMESPACES ===============================================================


namespace ACG {
namespace SceneGraph {
    
  //== IMPLEMENTATION ==========================================================

template <class Mesh>
StripProcessorT<Mesh>::
StripProcessorT(Mesh& _mesh) :
mesh_(_mesh),
stripsValid_(false),
updatePerEdgeBuffers_(true),
updatePerHalfedgeBuffers_(true),
updatePerFaceBuffers_(true),
textureIndexPropertyName_("Not Set"),
perFaceTextureCoordinatePropertyName_("Not Set")
{
  mesh_.request_face_normals();
}

template <class Mesh>
StripProcessorT<Mesh>::
~StripProcessorT() {

}

/*template <class Mesh>
uint
StripProcessorT<Mesh>::getPolyDim() {
    
    typename Mesh::FaceIter f_it, f_end = mesh_.faces_end();
    typename Mesh::FaceVertexIter fv_it;
    uint n = 0;
    
    for(f_it = mesh_.faces_begin(); f_it != f_end; ++f_it) {
        uint count = 0;
        for ( fv_it = mesh_.fv_iter(f_it); fv_it; ++fv_it ) {
            ++count;
        }
        
        if(count > n) n = count;
    }
    
    return n;
}

template <class Mesh>
uint
StripProcessorT<Mesh>::getFaceDimension(FaceHandle _fh) {
    uint count = 0;
    for ( typename Mesh::FaceVertexIter fv_it = mesh_.fv_iter(_fh); fv_it; ++fv_it )
        ++count;
    return count;
}*/

template <class Mesh>
void
StripProcessorT<Mesh>::convexityTest(FaceHandle /*_fh*/) {
}

template <class Mesh>
unsigned int
StripProcessorT<Mesh>::
stripify()
{
  if ( stripsValid_)
    return nStrips();
  
  // preprocess:  add new properties
  mesh_.add_property( processed_ );
  mesh_.add_property( used_ );
  mesh_.request_face_status();

  // build strips
  clear();
  buildStrips();

  // postprocess:  remove properties
  mesh_.remove_property(processed_);
  mesh_.remove_property(used_);
  mesh_.release_face_status();

  stripsValid_ = true;
  
  return nStrips();
}


//-----------------------------------------------------------------------------

template <class Mesh>
void
StripProcessorT<Mesh>::
buildStrips()
{
  // init faces to be un-processed and un-used
  // deleted or hidden faces are marked processed
  if (mesh_.has_face_status()) {
    for (typename Mesh::FaceIter f_it=mesh_.faces_begin(); f_it != mesh_.faces_end(); ++f_it)
      if (mesh_.status(f_it).hidden() || mesh_.status(f_it).deleted())
        processed(f_it) = used(f_it) = true;
      else
        processed(f_it) = used(f_it) = false;
  } else {
    for (typename Mesh::FaceIter f_it=mesh_.faces_begin(); f_it != mesh_.faces_end() ; ++f_it)
      processed(f_it) = used(f_it) = false;
  }
  
  if ( mesh_.is_trimesh() ) {
    buildStripsTriMesh();
  } else {
    buildStripsPolyMesh();
  }
  
  // In this case, we have to sort the array to have all strips with the same texture in one block (reduce texture switching)
  if ( perFaceTextureIndexAvailable() ) {
    std::sort(strips_.begin(),strips_.end(),&stripTextureCompare);
  }
  
}

template <class Mesh>
void
StripProcessorT<Mesh>::
buildStripsPolyMesh() {
    
    // Note: Limiting number of strips to be built to three, even for n-gons.
    
    std::vector<Strip>                            experiments; // Three strips to be built. Take longest one after all.
    std::vector< typename Mesh::HalfedgeHandle >  h;           // Three halfedges to start from.
    std::vector< FaceHandles >                    faces;       // Lists of faces.
    typename FaceHandles::iterator                fh_it, fh_end;
    typename Mesh::FaceIter                       f_it, f_end=mesh_.faces_end();
    
    for (f_it=mesh_.faces_begin(); true; ) {

      // find start face
      for (; f_it != f_end; ++f_it)
          if (!processed(f_it)) break;
      
      // stop if all have been processed
      if (f_it == f_end) 
        break; 
              
      // Number of experiments equals number of edges of the face
      experiments.clear();
      experiments.resize( mesh_.valence(f_it) );
      faces.clear();
      faces.resize(mesh_.valence(f_it));
      
      // Store all adjacent halfedges
      h.clear();
      for ( typename Mesh::FaceHalfedgeIter fhalfedge_it(mesh_,f_it); fhalfedge_it; ++fhalfedge_it ) 
        h.push_back( fhalfedge_it.handle() );
      
      // build all strips, take best one
      unsigned int best_length = 0;
      unsigned int best_idx    = 0;
      for (unsigned int i = 0; i < mesh_.valence(f_it) ; ++i)
      {
        buildStripPolyMesh(h[i], experiments[i], faces[i]);
        const unsigned int length = experiments[i].indexArray.size();
        if (length  > best_length) {
            best_length = length;
            best_idx    = i;
        }
        
        // unset the used flag for the following experiments
        for (fh_it=faces[i].begin(), fh_end=faces[i].end(); fh_it!=fh_end; ++fh_it)
            used(*fh_it) = false;
      }
      
      // update processed status
      // The processed flag indicates that the whole face has been rendered
      fh_it  = faces[best_idx].begin();
      fh_end = faces[best_idx].end();
      for (; fh_it!=fh_end; ++fh_it)
          processed(*fh_it) = true;
      
      // add best strip to strip-list
      strips_.push_back(experiments[best_idx]);
    }
}

template <class Mesh>
void
StripProcessorT<Mesh>::
buildStripsTriMesh()
{    
  Strip                           experiments[3];
  typename Mesh::HalfedgeHandle   h[3];

  FaceHandles                     faces[3];
  typename FaceHandles::iterator  fh_it, fh_end;
  typename Mesh::FaceIter         f_it, f_end=mesh_.faces_end();
  
  for (f_it=mesh_.faces_begin(); true; )
  {
    // find start face
    for (; f_it!=f_end; ++f_it)
      if (!processed(f_it))
        break;
    if (f_it==f_end) break; // stop if all have been processed


    // collect starting halfedges
    h[0] = mesh_.halfedge_handle(f_it.handle());
    h[1] = mesh_.next_halfedge_handle(h[0]);
    h[2] = mesh_.next_halfedge_handle(h[1]);

    // build 3 strips, take best one
    unsigned int best_length = 0;
    unsigned int best_idx    = 0;

    for (unsigned int i=0; i<3; ++i)
    {
      buildStripTriMesh(h[i], experiments[i], faces[i]);

      const unsigned int length = experiments[i].indexArray.size();
      if ( length  > best_length)
      {
        best_length = length;
        best_idx    = i;
      }

      for (fh_it=faces[i].begin(), fh_end=faces[i].end();
           fh_it!=fh_end; ++fh_it)
        used(*fh_it) = false;
    }


    // update processed status
    fh_it  = faces[best_idx].begin();
    fh_end = faces[best_idx].end();
    for (; fh_it!=fh_end; ++fh_it)
      processed(*fh_it) = true;

    // add best strip to strip-list
    strips_.push_back(experiments[best_idx]);
  }
}

//-----------------------------------------------------------------------------


template <class Mesh>
void
StripProcessorT<Mesh>::
buildStripPolyMesh(typename Mesh::HalfedgeHandle _start_hh,
                  Strip& _strip,
                  FaceHandles& _faces) {
    
    std::list<unsigned int>               strip;
    std::list<typename Mesh::FaceHandle > faceMap;
    typename Mesh::FaceHandle             fh;
    typename Mesh::HalfedgeHandle         hh_left, hh_right; // Keep the hh of the halfedge where we started
                      
    // reset list
    _faces.clear();

    // Init strip
    strip.push_back(mesh_.from_vertex_handle(_start_hh).idx());
    strip.push_back(mesh_.to_vertex_handle(_start_hh).idx());
    
    // Check if we have to take care of textures
    // If this property is not available, we do not have texture info and will therefore 
    // skip texture handling in strip generation
    OpenMesh::FPropHandleT< int > textureIndexProperty;
    bool textureHandling = false;
    if ( mesh_.get_property_handle(textureIndexProperty, textureIndexPropertyName_)  ) {
      textureHandling = true;
      _strip.textureIndex = mesh_.property(textureIndexProperty,mesh_.face_handle(_start_hh));
    } 
    
    /// \todo Implement texture processing here

    // Walk along the strip: 1st direction
    // We construct the strip by using alternating vertices
    // of each side.
    hh_left = hh_right = _start_hh;
    
    while(true) {
      
      // Go right
      hh_right = mesh_.prev_halfedge_handle(hh_right);
      
      // Add vertex to triangle strip
      strip.push_back(mesh_.from_vertex_handle(hh_right).idx());
      faceMap.push_back(mesh_.face_handle(hh_right));
      
      // Test if we're at the very last halfedge of the polygon
      if(mesh_.to_vertex_handle(mesh_.next_halfedge_handle(hh_left)) == mesh_.from_vertex_handle(hh_right)) {
          
          // Mark face as processed and used
          fh = mesh_.face_handle(hh_left);
          _faces.push_back(fh);
          used(fh) = true;
      
          // Go over to next face via the exit halfedge
          hh_left = hh_right = mesh_.opposite_halfedge_handle(mesh_.next_halfedge_handle(hh_left));
          
          // Test if polygon is convex (only for testing purposes a.t.m.)
          convexityTest(fh);
          
          if(mesh_.is_boundary(hh_left)) break;
          fh = mesh_.face_handle(hh_left);
          if (processed(fh) || used(fh)) break;
          
          // texture check
          if ( textureHandling && ( _strip.textureIndex != mesh_.property(textureIndexProperty,fh ) ) )  break;

//           continue;
      } 
      
      // Go left
      hh_left = mesh_.next_halfedge_handle(hh_left);
      
      // Add vertex to triangle strip
      strip.push_back(mesh_.to_vertex_handle(hh_left).idx());
      faceMap.push_back(mesh_.face_handle(hh_left));
      
      // Test if we're at the very last halfedge of the polygon
      if(mesh_.to_vertex_handle(mesh_.next_halfedge_handle(hh_left)) == mesh_.from_vertex_handle(hh_right)) {
        // Mark face as processed and used
        fh = mesh_.face_handle(hh_left);
        _faces.push_back(fh);
        used(fh) = true;
        
        // Go over to next face via the exit halfedge
        hh_left = hh_right = mesh_.opposite_halfedge_handle(mesh_.next_halfedge_handle(hh_left));
        
        // Test if polygon is convex (only for testing purposes a.t.m.)
        convexityTest(fh);
        
        if(mesh_.is_boundary(hh_left)) break;
        fh = mesh_.face_handle(hh_left);
        if (processed(fh) || used(fh)) break;
        
        // texture check
        if ( textureHandling && ( _strip.textureIndex != mesh_.property(textureIndexProperty,fh ) ) ) break;

      }
       
    }
    
    // Be carefull with the map. As we run in the opposite direction now,
    // we have to finish the map at the end of the strip
    
    // Walk along the strip: 2nd direction
    // We construct the strip by using alternating vertices
    // of each side.
    hh_left = hh_right = mesh_.opposite_halfedge_handle(_start_hh);
    
    bool flip(false);
    while(true) {
      // Boundary check as the first might be at the boundary
      if(mesh_.is_boundary(hh_left)) break;

      // Go right
      hh_right = mesh_.prev_halfedge_handle(hh_right);
      
      // Add vertex to triangle strip
      strip.push_front(mesh_.from_vertex_handle(hh_right).idx());
      faceMap.push_front(mesh_.face_handle(hh_right));
      flip = !flip;
      
      // Test if we're at the very last halfedge of the polygon
      if(mesh_.to_vertex_handle(mesh_.next_halfedge_handle(hh_left)) == mesh_.from_vertex_handle(hh_right)) {
          
        // Mark face as processed and used
        fh = mesh_.face_handle(hh_right);
        _faces.push_back(fh);
        used(fh) = true;
    
        // Go over to next face via the exit halfedge
        hh_left = hh_right = mesh_.opposite_halfedge_handle(mesh_.next_halfedge_handle(hh_left));
    
        if(mesh_.is_boundary(hh_left)) break;
        fh = mesh_.face_handle(hh_left);
        if (processed(fh) || used(fh)) break;
        
        // texture check
        if ( textureHandling && ( _strip.textureIndex != mesh_.property(textureIndexProperty,fh ) ) ) break;
        
        // Test if polygon is convex (only for testing purposes a.t.m.)
        convexityTest(fh);
        
//         continue;
        
      }
      
      // Go left
      hh_left = mesh_.next_halfedge_handle(hh_left);
      
      // Add vertex to triangle strip
      strip.push_front( mesh_.to_vertex_handle(hh_left).idx() );
      faceMap.push_front( mesh_.face_handle(hh_left) );
      flip = !flip;
      
      // Test if we're at the very last halfedge of the polygon
      if(mesh_.to_vertex_handle(mesh_.next_halfedge_handle(hh_left)) == mesh_.from_vertex_handle(hh_right)) {
        
        // Mark face as processed and used
        fh = mesh_.face_handle(hh_right);
        _faces.push_back(fh);
        used(fh) = true;
        
        // Go over to next face via the exit halfedge
        hh_left = hh_right = mesh_.opposite_halfedge_handle(mesh_.next_halfedge_handle(hh_left));
        
        if(mesh_.is_boundary(hh_left)) break;
        fh = mesh_.face_handle(hh_left);
        if (processed(fh) || used(fh)) break;
        
        // texture check
        if ( textureHandling && ( _strip.textureIndex != mesh_.property(textureIndexProperty,fh ) ) ) break;
        
        // Test if polygon is convex (only for testing purposes a.t.m.)
        convexityTest(fh);
      }
     
    }
    
    if ( flip ) {
      strip.push_front(strip.front());
      faceMap.push_front(mesh_.face_handle(0));
    }
    
    // Finish the map ( As this is the start of the strip now, this data will be ignored!
    faceMap.push_front(mesh_.face_handle(0));
    faceMap.push_front(mesh_.face_handle(0));    
    
    // copy final strip to _strip
    _strip.indexArray.clear();
    _strip.indexArray.reserve(strip.size());
    std::copy(strip.begin(), strip.end(), std::back_inserter(_strip.indexArray));
    
    _strip.faceMap.clear();
    _strip.faceMap.reserve(strip.size());
    std::copy(faceMap.begin(), faceMap.end(), std::back_inserter(_strip.faceMap));
}


//-----------------------------------------------------------------------------


template <class Mesh>
void
StripProcessorT<Mesh>::
buildStripTriMesh(typename Mesh::HalfedgeHandle _start_hh,
            Strip& _strip,
            FaceHandles& _faces)
{   
  std::list<unsigned int>  strip;
  typename Mesh::HalfedgeHandle   hh;
  typename Mesh::FaceHandle       fh;
  
  std::list<typename Mesh::FaceHandle > faceMap;


  // reset face list
  _faces.clear();


  // init strip
  strip.push_back(mesh_.from_vertex_handle(_start_hh).idx());
  strip.push_back(mesh_.to_vertex_handle(_start_hh).idx());

  // Check if we have to take care of textures
  // If this property is not available, we do not have texture info and will therefore 
  // skip texture handling in strip generation
  bool textureHandling = false;
  OpenMesh::FPropHandleT< int > textureIndexProperty;
  if ( mesh_.get_property_handle(textureIndexProperty, textureIndexPropertyName_) ) {
    textureHandling = true;
    _strip.textureIndex = mesh_.property(textureIndexProperty,mesh_.face_handle(_start_hh));
  } else {
    // Set to no texture!
    // This is not really necessary but cleans up for debugging
    _strip.textureIndex = 0; 
  }
  
  /// \todo Implement texture processing here

  // walk along the strip: 1st direction
  hh = mesh_.prev_halfedge_handle(mesh_.opposite_halfedge_handle(_start_hh));
  while (1)
  {
    // go right
    hh = mesh_.next_halfedge_handle(hh);
    hh = mesh_.opposite_halfedge_handle(hh);
    hh = mesh_.next_halfedge_handle(hh);
    if (mesh_.is_boundary(hh)) break;
    fh = mesh_.face_handle(hh);
    if (processed(fh) || used(fh)) break;
    
    // texture check
    if ( textureHandling && ( _strip.textureIndex != mesh_.property(textureIndexProperty,fh ) ) ) break;
    
    _faces.push_back(fh);
    used(fh) = true;
    strip.push_back(mesh_.to_vertex_handle(hh).idx());
    faceMap.push_back(mesh_.face_handle(hh));

    // go left
    hh = mesh_.opposite_halfedge_handle(hh);
    hh = mesh_.next_halfedge_handle(hh);
    if (mesh_.is_boundary(hh)) break;
    fh = mesh_.face_handle(hh);
    if (processed(fh) || used(fh)) break;
    
    // texture check
    if ( textureHandling && ( _strip.textureIndex != mesh_.property(textureIndexProperty,fh ) ) ) break;
    
    _faces.push_back(fh);
    used(fh) = true;
    strip.push_back(mesh_.to_vertex_handle(hh).idx());
    faceMap.push_back(mesh_.face_handle(hh));
  }


  // walk along the strip: 2nd direction
  bool flip(false);
  hh = mesh_.prev_halfedge_handle(_start_hh);
  while (1)
  {
    // go right
    hh = mesh_.next_halfedge_handle(hh);
    hh = mesh_.opposite_halfedge_handle(hh);
    hh = mesh_.next_halfedge_handle(hh);
    if (mesh_.is_boundary(hh)) break;
    fh = mesh_.face_handle(hh);
    if (processed(fh) || used(fh)) break;
    
    // texture check
    if ( textureHandling && ( _strip.textureIndex != mesh_.property(textureIndexProperty,fh ) ) ) break;
    
    _faces.push_back(fh);
    used(fh) = true;
    strip.push_front(mesh_.to_vertex_handle(hh).idx());
    faceMap.push_front(mesh_.face_handle(hh));
    flip = true;

    // go left
    hh = mesh_.opposite_halfedge_handle(hh);
    hh = mesh_.next_halfedge_handle(hh);
    if (mesh_.is_boundary(hh)) break;
    fh = mesh_.face_handle(hh);
    if (processed(fh) || used(fh)) break;
    
    // texture check
    if ( textureHandling && ( _strip.textureIndex != mesh_.property(textureIndexProperty,fh ) ) ) break;
    
    _faces.push_back(fh);
    used(fh) = true;
    strip.push_front(mesh_.to_vertex_handle(hh).idx());
    faceMap.push_front(mesh_.face_handle(hh));
    flip = false;
  }

  if (flip) {
    strip.push_front(strip.front());
    faceMap.push_front(mesh_.face_handle(0));
  }
  
  // Add two empty faces at the front. They will not be used to render anything as only the third one defines the 
  // face properties in the strip.
  faceMap.push_front(mesh_.face_handle(0));
  faceMap.push_front(mesh_.face_handle(0));

  // copy final strip to _strip
  _strip.indexArray.clear();
  _strip.indexArray.reserve(strip.size());
  std::copy(strip.begin(), strip.end(), std::back_inserter(_strip.indexArray));
  
  _strip.faceMap.clear();
  _strip.faceMap.reserve(strip.size());
  std::copy(faceMap.begin(), faceMap.end(), std::back_inserter(_strip.faceMap));
}

template <class Mesh>
void
StripProcessorT<Mesh>::
updatePickingVertices(ACG::GLState&  _state, uint _offset) {
  
  unsigned int idx = 0;
  
  // Adjust size of the color buffer to the number of vertices in the mesh
  pickVertexColorBuf_.resize( mesh_.n_vertices() );
  
  // Get the right picking colors from the gl state and add them per vertex to the color buffer
  typename Mesh::ConstVertexIter v_it(mesh_.vertices_begin()), v_end(mesh_.vertices_end());
  for (; v_it!=v_end; ++v_it, ++idx) 
    pickVertexColorBuf_[idx] = _state.pick_get_name_color(idx + _offset);
}

template <class Mesh>
void
StripProcessorT<Mesh>::
updatePickingEdges(ACG::GLState& _state,  uint _offset ) {
  
  updatePerEdgeBuffers();
  
  pickEdgeColorBuf_.resize(mesh_.n_edges() * 2);
  
  
  int idx = 0;
  
  typename Mesh::ConstEdgeIter  e_it(mesh_.edges_sbegin()), e_end(mesh_.edges_end());
  for (; e_it!=e_end; ++e_it) {
    
    const Vec4uc pickColor =  _state.pick_get_name_color (e_it.handle().idx() + _offset);
    
    pickEdgeColorBuf_[idx]    = pickColor;
    pickEdgeColorBuf_[idx+1]  = pickColor;

    idx += 2;
  }
}
    
template <class Mesh>
void
StripProcessorT<Mesh>::
updatePickingFaces(ACG::GLState& _state ) {
  
  // Make sure, the face buffers are up to date before generating the picking data!
  updatePerFaceBuffers();
   
  // Get total number of triangles
  // Each strip has two vertices more than triangles
  unsigned int n_faces = 0;
  for(StripsIterator it = strips_.begin(); it != strips_.end(); ++it) 
    n_faces += (*it).indexArray.size() - 2;
  
  // 3 vertices per face.
  pickFaceColorBuf_.resize(n_faces * 3);
  
  // Index to the current buffer position
  unsigned int bufferIndex = 0;
  
  // Process all strips
  for ( unsigned int i = 0 ; i < strips_.size() ; ++i ) {
    
    // The order of the vertices in the strip is alternating but as the last vertex still defines
    // points to the associated face, we dont need to swap here!
    
    // process all faces in the strip
    // The strip contains 2 faces less then number of vertices in the strip.
    // As we need seperate faces during rendering, the strips are splitted into triangles
    // The last vertex of each triangle defines the picking color for the last face.
    // The handles and indices are collected during the strip generation.
    for (unsigned int stripIndex = 2 ; stripIndex <  strips_[ i ].indexArray.size() ; ++stripIndex) {
      
      // We have to provide a vertex color for each of the vertices as we need flat shading!
      const Vec4uc pickColor = _state.pick_get_name_color ( strips_[ i ].faceMap[ stripIndex ].idx() );
      pickFaceColorBuf_[ bufferIndex + 0 ] = pickColor;
      pickFaceColorBuf_[ bufferIndex + 1 ] = pickColor;
      pickFaceColorBuf_[ bufferIndex + 2 ] = pickColor;
      
      bufferIndex += 3;
    }
  }
  
}

template <class Mesh>
void
StripProcessorT<Mesh>::
updatePickingAny(ACG::GLState& _state ) {
  // Update strip information if necessary
  stripify();
  
  updatePickingFaces(_state);
  updatePickingEdges(_state,mesh_.n_faces());
  updatePickingVertices(_state,mesh_.n_faces() + mesh_.n_edges());
}

template <class Mesh>
void
StripProcessorT<Mesh>::
updatePerEdgeBuffers() {
  // Only update buffers if they are invalid
  if (!updatePerEdgeBuffers_) 
    return;
  
  perEdgeVertexBuffer_.resize(mesh_.n_edges() * 2);
  
  if ( mesh_.has_edge_colors() ) {
    perEdgeColorBuffer_.resize(mesh_.n_edges() * 2);
  } else
    perEdgeColorBuffer_.clear();    
  
  unsigned int idx = 0;
  
  typename Mesh::ConstEdgeIter  e_it(mesh_.edges_sbegin()), e_end(mesh_.edges_end());
  for (; e_it!=e_end; ++e_it) {
    
    perEdgeVertexBuffer_[idx]   = mesh_.point(mesh_.to_vertex_handle(mesh_.halfedge_handle(e_it, 0)));
    perEdgeVertexBuffer_[idx+1] = mesh_.point(mesh_.to_vertex_handle(mesh_.halfedge_handle(e_it, 1)));
    
    if (  mesh_.has_edge_colors() ) {
      const Vec4f color = OpenMesh::color_cast<Vec4f>( mesh_.color(e_it) ) ;
      perEdgeColorBuffer_[ idx ]     = color;
      perEdgeColorBuffer_[ idx + 1 ] = color;
    }
    
    idx += 2;
  }
  
  updatePerEdgeBuffers_ = false;
  
}

template <class Mesh>
ACG::Vec3f * 
StripProcessorT<Mesh>::
perEdgeVertexBuffer() { 
  // Force update of the buffers if required
  if (updatePerEdgeBuffers_)
    updatePerEdgeBuffers();
  return &(perEdgeVertexBuffer_)[0]; 
}

template <class Mesh>
ACG::Vec4f * 
StripProcessorT<Mesh>::
perEdgeColorBuffer() { 
  // Force update of the buffers if required
  if (updatePerEdgeBuffers_)
    updatePerEdgeBuffers();
  return &(perEdgeColorBuffer_)[0]; 
}


template <class Mesh>
void
StripProcessorT<Mesh>::
updatePerHalfedgeBuffers() {
  // Only update buffers if they are invalid
  if (!updatePerHalfedgeBuffers_) 
    return;
  
  perHalfedgeVertexBuffer_.resize(mesh_.n_halfedges() * 2);
  
  if ( mesh_.has_halfedge_colors() ) {
    perHalfedgeColorBuffer_.resize(mesh_.n_halfedges() * 2);
  } else
    perHalfedgeColorBuffer_.clear();    
  
  unsigned int idx = 0;
  
  typename Mesh::ConstHalfedgeIter  he_it(mesh_.halfedges_sbegin()), he_end(mesh_.halfedges_end());
  for (; he_it!=he_end; ++he_it) {
    
    perHalfedgeVertexBuffer_[idx]   = halfedge_point(he_it);
    perHalfedgeVertexBuffer_[idx+1] = halfedge_point(mesh_.prev_halfedge_handle(he_it));
    
    if (  mesh_.has_halfedge_colors() ) {
      const Vec4f color = OpenMesh::color_cast<Vec4f>( mesh_.color(he_it) ) ;
      perHalfedgeColorBuffer_[ idx ]     = color;
      perHalfedgeColorBuffer_[ idx + 1 ] = color;
    }
    
    idx += 2;
  }
  
  updatePerHalfedgeBuffers_ = false;
  
}

template <class Mesh>
typename Mesh::Point
StripProcessorT<Mesh>::
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
ACG::Vec3f * 
StripProcessorT<Mesh>::
perHalfedgeVertexBuffer() { 
  // Force update of the buffers if required
  if (updatePerHalfedgeBuffers_)
    updatePerHalfedgeBuffers();
  return &(perHalfedgeVertexBuffer_)[0]; 
}

template <class Mesh>
ACG::Vec4f * 
StripProcessorT<Mesh>::
perHalfedgeColorBuffer() { 
  // Force update of the buffers if required
  if (updatePerHalfedgeBuffers_)
    updatePerHalfedgeBuffers();
  return &(perHalfedgeColorBuffer_)[0]; 
}

template <class Mesh>
void
StripProcessorT<Mesh>::
updatePerFaceBuffers() {
  
  stripify();

  // Only update buffers if they are invalid
  if (!updatePerFaceBuffers_) 
    return;
  

  // Get and Recheck property
  OpenMesh::HPropHandleT< typename Mesh::TexCoord2D >  perFaceTextureCoordinateProperty;  
  const bool usePerFaceTextureCoordinateProperty = mesh_.get_property_handle(perFaceTextureCoordinateProperty,perFaceTextureCoordinatePropertyName_); 
  
  const bool usePerFaceTextureIndex = perFaceTextureIndexAvailable();
  
  unsigned int n_faces = 0;

  // For the polyMeshes we have to count the faces in all strips
  // Each strip has two vertices more than triangles
  for(StripsIterator it = strips_.begin(); it != strips_.end(); ++it) 
    n_faces += (*it).indexArray.size() - 2;
  
  // 3 vertices per face.
  perFaceVertexBuffer_.resize(n_faces * 3);

  // Index to the current buffer position
  unsigned int bufferIndex = 0;

  if ( mesh_.has_face_normals() ) 
    perFaceNormalBuffer_.resize(n_faces * 3);
  else
    perFaceNormalBuffer_.clear();
  
  if ( mesh_.has_vertex_normals() ) 
    perFacePerVertexNormalBuffer_.resize(n_faces * 3);
  else
    perFacePerVertexNormalBuffer_.clear();
  
  if ( mesh_.has_face_colors() ) 
    perFaceColorBuffer_.resize(n_faces * 3);
  else
    perFaceColorBuffer_.clear();      
  
  if ( usePerFaceTextureCoordinateProperty ) 
    perFaceTextureCoordArray_.resize(n_faces * 3);
  else
    perFaceTextureCoordArray_.clear();   

  textureRenderData_.clear();
  
  if ( usePerFaceTextureIndex )
    textureRenderData_.reserve( strips_.size() );
  
  // Process all strips
  for ( unsigned int i = 0 ; i < strips_.size() ; ++i ) {
    
    // Record strip information
    // Or store a simple strip info with texture 0
    if ( usePerFaceTextureIndex ) {
      textureRenderData_.push_back( TextureRenderInfo(strips_[i].textureIndex , strips_[ i ].indexArray.size() -2 ,bufferIndex) );
    }
    
    // The order of the vertices in the strip is alternating so we have to alter the directions as well
    // or we get backfacing triangles although they are frontfacing
    bool swap = true;
    
    // process all faces in the strip
    // The strip contains 2 faces less then number of vertices in the strip.
    // As we need seperate faces during rendering, the strips are splitted into triangles
    // The last vertex of each triangle defines the color and the normal for the last face.
    // Handles and indices are collected during the strip generation.
    for (unsigned int stripIndex = 2 ; stripIndex <  strips_[ i ].indexArray.size() ; ++stripIndex) {
      
      if (  mesh_.has_face_normals() ) {
        const Vec3d normal = mesh_.normal( strips_[ i ].faceMap[ stripIndex ] );
        perFaceNormalBuffer_[ bufferIndex + 0 ] = normal;
        perFaceNormalBuffer_[ bufferIndex + 1 ] = normal;
        perFaceNormalBuffer_[ bufferIndex + 2 ] = normal;
      }
    
      if (  mesh_.has_face_colors() ) {
        const Vec4f color = OpenMesh::color_cast<Vec4f>( mesh_.color( strips_[ i ].faceMap[ stripIndex ] ) );
        perFaceColorBuffer_[ bufferIndex + 0 ] = color;
        perFaceColorBuffer_[ bufferIndex + 1 ] = color;
        perFaceColorBuffer_[ bufferIndex + 2 ] = color;
      }
      
      if ( swap ) {
        // Cant render triangle strips as we need one color per face and this means duplicating vertices
        perFaceVertexBuffer_[ bufferIndex + 0 ] = mesh_.point(mesh_.vertex_handle( strips_[ i ].indexArray[ stripIndex - 2 ] ));
        perFaceVertexBuffer_[ bufferIndex + 1 ] = mesh_.point(mesh_.vertex_handle( strips_[ i ].indexArray[ stripIndex - 1 ] ));
        perFaceVertexBuffer_[ bufferIndex + 2 ] = mesh_.point(mesh_.vertex_handle( strips_[ i ].indexArray[ stripIndex - 0 ] ));
        
        // Get the normals of all vertices at this face
        if (  mesh_.has_vertex_normals() ) {
          perFacePerVertexNormalBuffer_[ bufferIndex + 0 ] = mesh_.normal(mesh_.vertex_handle( strips_[ i ].indexArray[ stripIndex - 2 ] ));
          perFacePerVertexNormalBuffer_[ bufferIndex + 1 ] = mesh_.normal(mesh_.vertex_handle( strips_[ i ].indexArray[ stripIndex - 1 ] ));
          perFacePerVertexNormalBuffer_[ bufferIndex + 2 ] = mesh_.normal(mesh_.vertex_handle( strips_[ i ].indexArray[ stripIndex - 0 ] ));
        }
        
        
        if ( usePerFaceTextureCoordinateProperty ) {
          typename Mesh::ConstFaceHalfedgeIter fhe_it(mesh_.cfh_iter(strips_[ i ].faceMap[ stripIndex ]));
          
          for ( ; fhe_it ; ++fhe_it ) {
            typename Mesh::VertexHandle cvh = mesh_.to_vertex_handle(fhe_it);
            Vec2f texcoord = mesh_.property(perFaceTextureCoordinateProperty,fhe_it);
	    
            if ( mesh_.vertex_handle( strips_[ i ].indexArray[ stripIndex - 2 ] ) == cvh ) {
              perFaceTextureCoordArray_[ bufferIndex + 0 ]  = texcoord; 
              continue;
            } else if ( mesh_.vertex_handle( strips_[ i ].indexArray[ stripIndex - 1 ] ) == cvh ) {
              perFaceTextureCoordArray_[ bufferIndex + 1 ]  = texcoord; 
              continue;
            } else if ( mesh_.vertex_handle( strips_[ i ].indexArray[ stripIndex - 0 ] ) == cvh ) {
              perFaceTextureCoordArray_[ bufferIndex + 2 ]  = texcoord; 
              continue;
            }
          }
          
        }
        
        swap = false;
      } else {
        // Cant render triangle strips as we need one color per face and this means duplicating vertices
        perFaceVertexBuffer_[ bufferIndex + 2 ] = mesh_.point(mesh_.vertex_handle( strips_[ i ].indexArray[ stripIndex - 2 ] ));
        perFaceVertexBuffer_[ bufferIndex + 1 ] = mesh_.point(mesh_.vertex_handle( strips_[ i ].indexArray[ stripIndex - 1 ] ));
        perFaceVertexBuffer_[ bufferIndex + 0 ] = mesh_.point(mesh_.vertex_handle( strips_[ i ].indexArray[ stripIndex - 0 ] ));
        
        // Get the normals of all vertices at this face
        if (  mesh_.has_vertex_normals() ) {
          perFacePerVertexNormalBuffer_[ bufferIndex + 2 ] = mesh_.normal(mesh_.vertex_handle( strips_[ i ].indexArray[ stripIndex - 2 ] ));
          perFacePerVertexNormalBuffer_[ bufferIndex + 1 ] = mesh_.normal(mesh_.vertex_handle( strips_[ i ].indexArray[ stripIndex - 1 ] ));
          perFacePerVertexNormalBuffer_[ bufferIndex + 0 ] = mesh_.normal(mesh_.vertex_handle( strips_[ i ].indexArray[ stripIndex - 0 ] ));
        }
        
        if ( usePerFaceTextureCoordinateProperty ) {
          typename Mesh::ConstFaceHalfedgeIter fhe_it(mesh_.cfh_iter(strips_[ i ].faceMap[ stripIndex ]));
          
          for ( ; fhe_it ; ++fhe_it ) {
            typename Mesh::VertexHandle cvh = mesh_.to_vertex_handle(fhe_it);
            const Vec2f texcoord = mesh_.property(perFaceTextureCoordinateProperty,fhe_it);
            
            if ( mesh_.vertex_handle( strips_[ i ].indexArray[ stripIndex - 2 ] ) == cvh ) {
              perFaceTextureCoordArray_[ bufferIndex + 2 ]  = texcoord; 
              continue;
            } else if ( mesh_.vertex_handle( strips_[ i ].indexArray[ stripIndex - 1 ] ) == cvh ) {
              perFaceTextureCoordArray_[ bufferIndex + 1 ]  = texcoord; 
              continue;
            } else if ( mesh_.vertex_handle( strips_[ i ].indexArray[ stripIndex - 0 ] ) == cvh ) {
              perFaceTextureCoordArray_[ bufferIndex + 0 ]  = texcoord; 
              continue;
            }
          }
          
        }
        
        swap = true;
      }
      
      bufferIndex += 3;
    }
  } 
  
  updatePerFaceBuffers_ = false;
}


template <class Mesh>
ACG::Vec3f * 
StripProcessorT<Mesh>::
perFaceVertexBuffer() { 
  
  // Force update of the buffers if required
  if (updatePerFaceBuffers_)
    updatePerFaceBuffers();
  
  return &(perFaceVertexBuffer_)[0]; 
}

template <class Mesh>
ACG::Vec3f * 
StripProcessorT<Mesh>::
perFaceNormalBuffer() { 
  // Force update of the buffers if required
  if (updatePerFaceBuffers_)
    updatePerFaceBuffers();
  
  return &(perFaceNormalBuffer_)[0]; 
}

template <class Mesh>
ACG::Vec3f * 
StripProcessorT<Mesh>::
perFacePerVertexNormalBuffer() { 
  // Force update of the buffers if required
  if (updatePerFaceBuffers_)
    updatePerFaceBuffers();
  
  return &(perFacePerVertexNormalBuffer_)[0]; 
}

template <class Mesh>
ACG::Vec4f * 
StripProcessorT<Mesh>::
perFaceColorBuffer() { 
  // Force update of the buffers if required
  if (updatePerFaceBuffers_)
    updatePerFaceBuffers();
  
  return &(perFaceColorBuffer_)[0]; 
}

template <class Mesh>
void
StripProcessorT<Mesh>::
setPerFaceTextureCoordinatePropertyName( std::string _perFaceTextureCoordinatePropertyName ) {
  
  // Check if the given property exists
  OpenMesh::HPropHandleT< typename Mesh::TexCoord2D >  perFaceTextureCoordinateProperty;
  if ( !mesh_.get_property_handle(perFaceTextureCoordinateProperty,_perFaceTextureCoordinatePropertyName) )  {
    std::cerr << "StripProcessor: Unable to get per face texture coordinate property named " << _perFaceTextureCoordinatePropertyName << std::endl;
    return;
  }
  
  // Remember the property name
  perFaceTextureCoordinatePropertyName_ = _perFaceTextureCoordinatePropertyName;
  
  // mark the buffers as invalid as we have a new per face index array
  invalidatePerFaceBuffers();
}

template <class Mesh>
bool 
StripProcessorT<Mesh>::perFaceTextureCoordinateAvailable()  {
  
  // We really have to recheck, as the property might get lost externally (e.g. on restores of the mesh)
  OpenMesh::HPropHandleT< typename Mesh::TexCoord2D >  perFaceTextureCoordinateProperty;
  if ( !mesh_.get_property_handle(perFaceTextureCoordinateProperty, perFaceTextureCoordinatePropertyName_) )  {
    return false;
  }
  
  // Property available
  return true;
  
}  

template <class Mesh>
bool 
StripProcessorT<Mesh>::perFaceTextureIndexAvailable() {
  
  // We really have to recheck, as the property might get lost externally (e.g. on restores of the mesh)
  OpenMesh::FPropHandleT< int > textureIndexProperty;
  if ( !mesh_.get_property_handle(textureIndexProperty, textureIndexPropertyName_) )  {
    return false;
  }
  
  // Property available
  return true;
}

template <class Mesh>
void
StripProcessorT<Mesh>::
setIndexPropertyName( std::string _indexPropertyName ) { 
  
  // Check if the given property exists
  OpenMesh::FPropHandleT< int > textureIndexProperty;
  if ( !mesh_.get_property_handle(textureIndexProperty,_indexPropertyName) )  {
    std::cerr << "StripProcessor: Unable to get per face texture Index property named " << _indexPropertyName << std::endl;
    return;
  }
  
  // Remember the property name
  textureIndexPropertyName_ = _indexPropertyName;
  
  // mark strips as invalid ( have to be regenerated to collect texture index information)
  stripsValid_ = false;
  
  // mark the buffers as invalid as we have a new per face index array
  invalidatePerFaceBuffers();
}

//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
