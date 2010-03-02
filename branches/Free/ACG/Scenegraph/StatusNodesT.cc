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
  bbMax_(-FLT_MAX, -FLT_MAX, -FLT_MAX)
{
  depthFunc(GL_LEQUAL);

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
  bbMin_ = Vec3d(FLT_MAX,  FLT_MAX,  FLT_MAX);
  bbMax_ = Vec3d(-FLT_MAX, -FLT_MAX, -FLT_MAX);

  typename Mesh::ConstVertexIter  v_it(mesh_.vertices_sbegin()),
                                  v_end(mesh_.vertices_end());

  v_cache_.clear();
  for (; v_it!=v_end; ++v_it)
  {
    if (Mod::is_vertex_selected(mesh_, v_it.handle()))
    {
      v_cache_.push_back(v_it.handle().idx());
    }
    bbMin_.minimize(mesh_.point(v_it));
    bbMax_.maximize(mesh_.point(v_it));
  }



  typename Mesh::ConstEdgeIter  e_it(mesh_.edges_sbegin()),
                                e_end(mesh_.edges_end());
  typename Mesh::VertexHandle   vh;

  e_cache_.clear();
  for (; e_it!=e_end; ++e_it)
  {
    if (Mod::is_edge_selected(mesh_, e_it))
    {
      vh = mesh_.to_vertex_handle(mesh_.halfedge_handle(e_it, 0));
      e_cache_.push_back(vh.idx());
      vh = mesh_.to_vertex_handle(mesh_.halfedge_handle(e_it, 1));
      e_cache_.push_back(vh.idx());
    }
  }



  typename Mesh::ConstFaceIter  f_it(mesh_.faces_sbegin()),
                                f_end(mesh_.faces_end());
  typename Mesh::ConstFaceVertexIter   fv_it;

  f_cache_.clear();
  fh_cache_.clear();
  for (; f_it!=f_end; ++f_it)
  {
    if (Mod::is_face_selected(mesh_, f_it))
    {
      fv_it = mesh_.cfv_iter(f_it);
      f_cache_.push_back(fv_it.handle().idx()); ++fv_it;
      f_cache_.push_back(fv_it.handle().idx()); ++fv_it;
      f_cache_.push_back(fv_it.handle().idx());
      fh_cache_.push_back(f_it);
    }
  }
}


//----------------------------------------------------------------------------


template <class Mesh, class Mod>
void
StatusNodeT<Mesh, Mod>::
draw(GLState& /* _state */ , DrawModes::DrawMode _drawMode)
{
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

  bool faces = ((drawMode() == DrawModes::DEFAULT) |
		(_drawMode & DrawModes::SOLID_FLAT_SHADED));


  glDepthFunc(depthFunc());

  if (shaded)  glEnable(GL_LIGHTING);
  else         glDisable(GL_LIGHTING);

  if (smooth)  glShadeModel(GL_SMOOTH);
  else         glShadeModel(GL_FLAT);


  if (shaded && mesh_.has_vertex_normals())
  {
    glEnableClientState(GL_NORMAL_ARRAY);
    glNormalPointer(mesh_.vertex_normals());
  }


  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(mesh_.points());



  // points
  if (points)
    draw_points();


  // edges
  if (edges)
    draw_edges();


  if (shaded && !smooth)
    glDisableClientState(GL_NORMAL_ARRAY);


  // faces
  if (faces)
  {
    if (wires)
    {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      draw_faces(smooth);
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    else
    {
      
      glPushAttrib( GL_ENABLE_BIT );

      glEnable(GL_POLYGON_OFFSET_FILL);

      glPolygonOffset(0.001, 0.0);
      draw_faces(smooth);

      glPopAttrib();
    }
  }

  glDisableClientState(GL_NORMAL_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);

  glDepthFunc(GL_LESS);
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


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
