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




//=============================================================================
//
//  CLASS StatusNodeT - IMPLEMENTATION
//
//=============================================================================


#define ACG_STATUS_NODES_C


//== INCLUDES =================================================================


#include "StatusNodesT.hh"
#include "../GL/gl.hh"

//== NAMESPACES ===============================================================


namespace ACG {
namespace SceneGraph {


//== IMPLEMENTATION ==========================================================


template <class Mesh, class Mod>
StatusNodeT<Mesh, Mod>::
StatusNodeT( const Mesh&         _mesh,
             BaseNode*           _parent,
             const std::string&  _name )
  : MaterialNode(_parent, _name), mesh_(_mesh),
  bbMin_(FLT_MAX,  FLT_MAX,  FLT_MAX),
  bbMax_(-FLT_MAX, -FLT_MAX, -FLT_MAX),
  invalidGeometry_(true),
  vertexIndexInvalid_(true),
  halfedgeCacheInvalid_(true),
  edgeIndexInvalid_(true),
  faceIndexInvalid_(true)
{
  set_line_width(3);
  set_point_size(5);
}


//----------------------------------------------------------------------------


template <class Mesh, class Mod>
void
StatusNodeT<Mesh, Mod>::
boundingBox(Vec3d& _bbMin, Vec3d& _bbMax)
{
  _bbMin.minimize(bbMin_);
  _bbMax.maximize(bbMax_);
}


//----------------------------------------------------------------------------


template <class Mesh, class Mod>
DrawModes::DrawMode
StatusNodeT<Mesh, Mod>::
availableDrawModes() const
{
  return (DrawModes::POINTS |
	  DrawModes::WIREFRAME |
	  DrawModes::SOLID_FLAT_SHADED);
}


//----------------------------------------------------------------------------


template <class Mesh, class Mod>
void
StatusNodeT<Mesh, Mod>::
update_cache()
{
  if ( invalidGeometry_ ) {
    bbMin_ = Vec3d(FLT_MAX,  FLT_MAX,  FLT_MAX);
    bbMax_ = Vec3d(-FLT_MAX, -FLT_MAX, -FLT_MAX);

    typename Mesh::ConstVertexIter  v_it(mesh_.vertices_sbegin()),
        v_end(mesh_.vertices_end());

    for (; v_it!=v_end; ++v_it) {
      bbMin_.minimize(mesh_.point(v_it));
      bbMax_.maximize(mesh_.point(v_it));
    }

    invalidGeometry_  = false;
  }


  // Update the indices for selected vertices
  if ( vertexIndexInvalid_ ) {

    typename Mesh::ConstVertexIter  v_it(mesh_.vertices_sbegin()),
      v_end(mesh_.vertices_end());

    v_cache_.clear();
    for (; v_it!=v_end; ++v_it) {
      if (Mod::is_vertex_selected(mesh_, v_it.handle())) {
        v_cache_.push_back(v_it.handle().idx());
      }
    }

    vertexIndexInvalid_ = false;
  }

  // Update index list of selected edges
  if ( edgeIndexInvalid_ ) {

    typename Mesh::ConstEdgeIter  e_it(mesh_.edges_sbegin()),
      e_end(mesh_.edges_end());
    typename Mesh::VertexHandle   vh;

    e_cache_.clear();
    for (; e_it!=e_end; ++e_it) {
      if (Mod::is_edge_selected(mesh_, e_it)) {
        vh = mesh_.to_vertex_handle(mesh_.halfedge_handle(e_it, 0));
        e_cache_.push_back(vh.idx());
        vh = mesh_.to_vertex_handle(mesh_.halfedge_handle(e_it, 1));
        e_cache_.push_back(vh.idx());
      }
    }

    edgeIndexInvalid_ = false;
  }


  // Update index list of selected halfedges
  if ( halfedgeCacheInvalid_) {

    typename Mesh::ConstHalfedgeIter  he_it(mesh_.halfedges_sbegin()),
      he_end(mesh_.halfedges_end());

    he_points_.clear();
    he_normals_.clear();
    for (; he_it!=he_end; ++he_it) {
      if (Mod::is_halfedge_selected(mesh_, he_it))  {
        // add vertices
        he_points_.push_back( halfedge_point(he_it));
        he_points_.push_back( halfedge_point(mesh_.prev_halfedge_handle(he_it)));

        // add normals
        FaceHandle fh;
        if(!mesh_.is_boundary(he_it))
          fh = mesh_.face_handle(he_it);
        else
          fh = mesh_.face_handle(mesh_.opposite_halfedge_handle(he_it));

        he_normals_.push_back( mesh_.normal(fh));
        he_normals_.push_back( mesh_.normal(fh));
      }
    }

    halfedgeCacheInvalid_ = false;
  }


  // update index list of selected faces
  if ( faceIndexInvalid_ ) {

    typename Mesh::ConstFaceIter  f_it(mesh_.faces_sbegin()),
      f_end(mesh_.faces_end());
    typename Mesh::ConstFaceVertexIter   fv_it;

    f_cache_.clear();
    fh_cache_.clear();
    for (; f_it!=f_end; ++f_it) {
      if (Mod::is_face_selected(mesh_, f_it)) {
        fv_it = mesh_.cfv_iter(f_it);
        f_cache_.push_back(fv_it.handle().idx()); ++fv_it;
        f_cache_.push_back(fv_it.handle().idx()); ++fv_it;
        f_cache_.push_back(fv_it.handle().idx());
        fh_cache_.push_back(f_it);
      }
    }


    if (!mesh_.is_trimesh())
    {
      // triangulate poly-list
      poly_cache.resize(0);

      typename std::vector<FaceHandle>::const_iterator  fh_it(fh_cache_.begin()),
        fh_end(fh_cache_.end());
      typename Mesh::CFVIter                            fv_it;

      for (; fh_it!=fh_end; ++fh_it) 
      {
        fv_it = mesh_.cfv_iter(*fh_it);

        // 1. polygon vertex
        unsigned int v0 = fv_it.handle().idx();

        // go to next vertex
        ++fv_it;
        unsigned int vPrev = fv_it.handle().idx();

        // create triangle fans pointing towards v0
        for (; fv_it; ++fv_it) 
        {
          poly_cache.push_back(v0);
          poly_cache.push_back(vPrev);

          vPrev = fv_it.handle().idx();
          poly_cache.push_back(vPrev);
        }
      }
    }


    faceIndexInvalid_ = false;
  }

}


//----------------------------------------------------------------------------


template <class Mesh, class Mod>
void
StatusNodeT<Mesh, Mod>::
draw(GLState& _state, const DrawModes::DrawMode& _drawMode)
{
  // Call updater function before doing anything
  update_cache();

  bool shaded = (_drawMode & ( DrawModes::SOLID_FLAT_SHADED |
			       DrawModes::SOLID_SMOOTH_SHADED |
			       DrawModes::SOLID_PHONG_SHADED |
			       DrawModes::SOLID_TEXTURED_SHADED |
			       DrawModes::POINTS_SHADED ));

  bool wires  = (_drawMode & ( DrawModes::WIREFRAME |
			       DrawModes::HIDDENLINE ));

  bool smooth = (_drawMode & ( DrawModes::SOLID_SMOOTH_SHADED |
			       DrawModes::SOLID_PHONG_SHADED |
			       DrawModes::SOLID_TEXTURED_SHADED ));

  bool points = ((drawMode() == DrawModes::DEFAULT) |
		 (_drawMode & DrawModes::POINTS));

  bool edges = ((drawMode() == DrawModes::DEFAULT) |
		 (_drawMode & DrawModes::WIREFRAME));

  bool halfedges = ((drawMode() == DrawModes::DEFAULT) |
		    (_drawMode & DrawModes::WIREFRAME));

  bool faces = ((drawMode() == DrawModes::DEFAULT) |
		(_drawMode & DrawModes::SOLID_FLAT_SHADED));


  GLenum prev_depth = _state.depthFunc();
  
  ACG::GLState::depthFunc(GL_LEQUAL);

  if (shaded)  ACG::GLState::enable(GL_LIGHTING);
  else         ACG::GLState::disable(GL_LIGHTING);

  if (smooth)  ACG::GLState::shadeModel(GL_SMOOTH);
  else         ACG::GLState::shadeModel(GL_FLAT);


  if (shaded && mesh_.has_vertex_normals()) {
    ACG::GLState::enableClientState(GL_NORMAL_ARRAY);
    ACG::GLState::normalPointer(mesh_.vertex_normals());
  }


  ACG::GLState::enableClientState(GL_VERTEX_ARRAY);
  ACG::GLState::vertexPointer(mesh_.points());
  

  // points
  if (points)
    draw_points();


  // edges
  if (edges)
    draw_edges();


  if (shaded && !smooth)
    ACG::GLState::disableClientState(GL_NORMAL_ARRAY);


  // faces
  if (faces)  {
    if (wires) {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      draw_faces(smooth);
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    } else {
      
      glPushAttrib( GL_ENABLE_BIT );

      ACG::GLState::enable(GL_POLYGON_OFFSET_FILL);

      glPolygonOffset(0.001, 0.0);
      draw_faces(smooth);

      glPopAttrib();
    }
  }

  // half edges
  if (halfedges)
    draw_halfedges();

  ACG::GLState::disableClientState(GL_NORMAL_ARRAY);
  ACG::GLState::disableClientState(GL_VERTEX_ARRAY);
  
  ACG::GLState::depthFunc(prev_depth);
}


//----------------------------------------------------------------------------


template <class Mesh, class Mod>
void
StatusNodeT<Mesh, Mod>::
draw_points()
{
  if ( !v_cache_.empty() )
    glDrawElements(GL_POINTS,
  		             v_cache_.size(),
		               GL_UNSIGNED_INT,
		               &v_cache_[0]);
}


//----------------------------------------------------------------------------


template <class Mesh, class Mod>
void
StatusNodeT<Mesh, Mod>::
draw_edges()
{
  if ( !e_cache_.empty() )
    glDrawElements(GL_LINES,
  		             e_cache_.size(),
		               GL_UNSIGNED_INT,
		               &e_cache_[0]);
}


//----------------------------------------------------------------------------


template <class Mesh, class Mod>
void
StatusNodeT<Mesh, Mod>::
draw_halfedges() {
  if ( !he_points_.empty()) {
    ACG::GLState::enableClientState(GL_NORMAL_ARRAY);

    ACG::GLState::vertexPointer(&he_points_[0]);

    if ( !he_normals_.empty())
      ACG::GLState::normalPointer(&he_normals_[0]);

    glDrawArrays(GL_LINES, 0, he_points_.size() );
  }

}


//----------------------------------------------------------------------------


template <class Mesh, class Mod>
void
StatusNodeT<Mesh, Mod>::
draw_faces(bool _per_vertex)
{
  typename std::vector<FaceHandle>::const_iterator  fh_it(fh_cache_.begin()),
                                                    fh_end(fh_cache_.end());
  typename Mesh::CFVIter                            fv_it;


  // TRIANGLES
  if (mesh_.is_trimesh()) {

    if (!_per_vertex) {
      glBegin(GL_TRIANGLES);
      for (; fh_it!=fh_end; ++fh_it) {
        glNormal(mesh_.normal(*fh_it));
        glVertex(mesh_.point(fv_it=mesh_.cfv_iter(*fh_it)));
        glVertex(mesh_.point(++fv_it));
        glVertex(mesh_.point(++fv_it));
      }

      glEnd();
    } else {

      if ( !f_cache_.empty() )
        glDrawElements(GL_TRIANGLES,
                       f_cache_.size(),
                       GL_UNSIGNED_INT,
                       &f_cache_[0]);
    }

    // POLYGONS
  } else {

    if (!_per_vertex) {

      for (; fh_it!=fh_end; ++fh_it) {
        glBegin(GL_POLYGON);
        glNormal(mesh_.normal(*fh_it));
        for (fv_it=mesh_.cfv_iter(*fh_it); fv_it; ++fv_it)
          glVertex(mesh_.point(fv_it));
        glEnd();
      }

    } else {

      for (; fh_it!=fh_end; ++fh_it) {
        glBegin(GL_POLYGON);
        for (fv_it=mesh_.cfv_iter(*fh_it); fv_it; ++fv_it) {
          glNormal(mesh_.normal(fv_it));
          glArrayElement(fv_it.handle().idx());
        }
        glEnd();
      }

    }
  }
}


//----------------------------------------------------------------------------


template <class Mesh, class Mod>
typename Mesh::Point
StatusNodeT<Mesh, Mod>::
halfedge_point(const HalfedgeHandle _heh) 
{
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

//----------------------------------------------------------------------------

template <class Mesh, class Mod>
void StatusNodeT<Mesh, Mod>::getRenderObjects(IRenderer* _renderer,
                                              GLState& _state,
                                              const DrawModes::DrawMode& _drawMode,
                                              const class Material* _mat)
{
  // Call updater function before doing anything
  update_cache();

  bool shaded = (_drawMode & ( DrawModes::SOLID_FLAT_SHADED |
			       DrawModes::SOLID_SMOOTH_SHADED |
			       DrawModes::SOLID_PHONG_SHADED |
			       DrawModes::SOLID_TEXTURED_SHADED |
			       DrawModes::POINTS_SHADED ));

  bool points = ((drawMode() == DrawModes::DEFAULT) |
    (_drawMode & DrawModes::POINTS));

  bool edges = ((drawMode() == DrawModes::DEFAULT) |
    (_drawMode & DrawModes::WIREFRAME));

  bool halfedges = ((drawMode() == DrawModes::DEFAULT) |
    (_drawMode & DrawModes::WIREFRAME));

  bool faces = ((drawMode() == DrawModes::DEFAULT) |
    (_drawMode & DrawModes::SOLID_FLAT_SHADED));

  RenderObject ro;
  ro.debugName = "StatusNode";
  ro.initFromState(&_state);

  ro.depthTest = true;
  ro.depthFunc = GL_LEQUAL;

  // Use the material from the underlying materialnode
  ro.setMaterial(_mat);

  pointVertexDecl_.clear();
  pointVertexDecl_.addElement(GL_DOUBLE, 3, VERTEX_USAGE_POSITION, mesh_.points());

  if (shaded && mesh_.has_vertex_normals()) 
    pointVertexDecl_.addElement(GL_DOUBLE, 3, VERTEX_USAGE_NORMAL, mesh_.vertex_normals());

  pointVertexDecl_.setVertexStride(24); // separated buffers, double3:  24 bytes


  // draw status later than scene
  ro.priority = 1;


  // point list
  if (points && !v_cache_.empty())
  {
    ro.vertexDecl = &pointVertexDecl_;
    
    ro.glDrawElements(GL_POINTS, v_cache_.size(), GL_UNSIGNED_INT, &v_cache_[0]);
    _renderer->addRenderObject(&ro);
  }

  // edge list
  if (edges && !e_cache_.empty())
  {
    ro.vertexDecl = &pointVertexDecl_;

    ro.glDrawElements(GL_LINES, e_cache_.size(), GL_UNSIGNED_INT, &e_cache_[0]);
    _renderer->addRenderObject(&ro);
  }


  if (faces && !f_cache_.empty())
  {
    ro.vertexDecl = &pointVertexDecl_;

    if (mesh_.is_trimesh()) 
    {
      ro.glDrawElements(GL_TRIANGLES,  f_cache_.size(), GL_UNSIGNED_INT,  &f_cache_[0]);
      _renderer->addRenderObject(&ro);
    }
    else
    {
      ro.glDrawElements(GL_TRIANGLES,  poly_cache.size(), GL_UNSIGNED_INT,  &poly_cache[0]);
      _renderer->addRenderObject(&ro);
    }
  }


  // halfedge list
  if (halfedges && !he_points_.empty())
  {
    ro.shaderDesc.shadeMode = SG_SHADE_UNLIT;

    halfedgeVertexDecl_.clear();
    halfedgeVertexDecl_.addElement(GL_DOUBLE, 3, VERTEX_USAGE_POSITION, &he_points_[0]);

    ro.vertexDecl = &halfedgeVertexDecl_;

    ro.glDrawArrays(GL_LINES, 0, he_points_.size() );
    _renderer->addRenderObject(&ro);
  }


}


//----------------------------------------------------------------------------

template <class Mesh, class Mod>
void StatusNodeT<Mesh, Mod>::updateGeometry() {
  invalidGeometry_ = true;
}


template <class Mesh, class Mod>
void StatusNodeT<Mesh, Mod>::updateTopology() {
  vertexIndexInvalid_   = true;
  halfedgeCacheInvalid_ = true;
  edgeIndexInvalid_     = true;
  faceIndexInvalid_     = true;

}

template <class Mesh, class Mod>
void StatusNodeT<Mesh, Mod>::updateSelection() {
  vertexIndexInvalid_   = true;
  halfedgeCacheInvalid_ = true;
  edgeIndexInvalid_     = true;
  faceIndexInvalid_     = true;

}


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
