/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2011 by Computer Graphics Group, RWTH Aachen       *
*                           www.openflipper.org                              *
*                                                                            *
*--------------------------------------------------------------------------- *
*  This file is part of OpenFlipper.                                         *
*                                                                            *
*  OpenFlipper is free software: you can redistribute it and/or modify       *
*  it under the terms of the GNU Lesser General Public License as            *
*  published by the Free Software Foundation, either version 3 of            *
*  the License, or (at your option) any later version with the               *
*  following exceptions:                                                     *
*                                                                            *
*  If other files instantiate templates or use macros                        *
*  or inline functions from this file, or you compile this file and          *
*  link it with other files to produce an executable, this file does         *
*  not by itself cause the resulting executable to be covered by the         *
*  GNU Lesser General Public License. This exception does not however        *
*  invalidate any other reasons why the executable file might be             *
*  covered by the GNU Lesser General Public License.                         *
*                                                                            *
*  OpenFlipper is distributed in the hope that it will be useful,            *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
*  GNU Lesser General Public License for more details.                       *
*                                                                            *
*  You should have received a copy of the GNU LesserGeneral Public           *
*  License along with OpenFlipper. If not,                                   *
*  see <http://www.gnu.org/licenses/>.                                       *
*                                                                            *
\*===========================================================================*/

/*===========================================================================*\
*                                                                            *
*   $Revision$                                                       *
*   $LastChangedBy$                                                *
*   $Date$                    *
*                                                                            *
\*===========================================================================*/

//=============================================================================
//
//  CLASS AdaptiveRemesherT - IMPLEMENTATION
//
//=============================================================================

#define ADAPTIVE_REMESHERT_C


//== INCLUDES =================================================================

#include "AdaptiveRemesherT.hh"
#include <ACG/Geometry/Algorithms.hh>
#include "DiffGeoT.hh"


//== NAMESPACES ===============================================================

namespace Remeshing {


//== IMPLEMENTATION ==========================================================


template <class Mesh>
void
AdaptiveRemesherT<Mesh>::
init_reference()
{
  Base::init_reference();

  // compute max curvature on both meshes
  Base::refmesh_->add_property(refcurv_);
  compute_curvature(*Base::refmesh_, refcurv_);
  Base::mesh_.add_property(curvature_);
  compute_curvature(Base::mesh_, curvature_);
}


//-----------------------------------------------------------------------------


template <class Mesh>
void
AdaptiveRemesherT<Mesh>::
project_to_reference(typename Mesh::VertexHandle _vh) const
{
  typename Mesh::Point          p = Base::mesh_.point(_vh);
  typename Mesh::FaceHandle     fh = Base::bsp_->nearest(p).handle;

  if ( ! fh.is_valid() ) {
    std::cerr << "AdaptiveRemesherT: Projection, invalid face handle" << std::endl;
    return;
  }

  typename Mesh::CFVIter        fv_it = Base::refmesh_->cfv_iter(fh);
	  

  const typename Mesh::Point&   p0 = Base::refmesh_->point(*fv_it);
  typename Mesh::Normal         n0 = Base::refmesh_->normal(*fv_it);
  typename Mesh::Scalar         c0 = Base::refmesh_->property(refcurv_, *fv_it);
  const typename Mesh::Point&   p1 = Base::refmesh_->point(*(++fv_it));
  typename Mesh::Normal         n1 = Base::refmesh_->normal(*fv_it);
  typename Mesh::Scalar         c1 = Base::refmesh_->property(refcurv_, *fv_it);
  const typename Mesh::Point&   p2 = Base::refmesh_->point(*(++fv_it));
  typename Mesh::Normal         n2 = Base::refmesh_->normal(*fv_it);
  typename Mesh::Scalar         c2 = Base::refmesh_->property(refcurv_, *fv_it);


  // project
  //Geometry::dist_point_triangle(p, p0, p1, p2, p);
  ACG::Geometry::distPointTriangle<typename Mesh::Point>(p, p0, p1, p2, p);
  Base::mesh_.set_point(_vh, p);


  // get barycentric coordinates
  if (!ACG::Geometry::baryCoord(p, p0, p1, p2, p))
    p[0] = p[1] = p[2] = 1.0/3.0;


  // interpolate normal
  typename Mesh::Normal n;
  n  = (n0 *= p[0]);
  n += (n1 *= p[1]);
  n += (n2 *= p[2]);
  n.normalize();
  Base::mesh_.set_normal(_vh, n);


  // interpolate curvature
  typename Mesh::Scalar c;
  c  = (c0 *= p[0]);
  c += (c1 *= p[1]);
  c += (c2 *= p[2]);
  Base::mesh_.property(curvature_, _vh) = c;
}


//-----------------------------------------------------------------------------


template <class Mesh>
void
AdaptiveRemesherT<Mesh>::
compute_curvature(Mesh& _mesh, OpenMesh::VPropHandleT<Scalar> _ph)
{
  DiffGeoT<Mesh> diffgeo(_mesh);
  diffgeo.compute(2);

  typename Mesh::VIter  v_it, v_end(_mesh.vertices_end());

  for (v_it=_mesh.vertices_begin(); v_it!=v_end; ++v_it)
    _mesh.property(_ph, *v_it) = diffgeo.max_curvature(*v_it);
}


//-----------------------------------------------------------------------------


template <class Mesh>
void
AdaptiveRemesherT<Mesh>::
remesh(Scalar        _error,
       Scalar        _emin,
       Scalar        _emax,
       unsigned int  _iters,
       bool          _use_projection)
{
  // set thesholds
  error_ = _error;
  emin_  = _emin;
  emax_  = _emax;

  // do it
  Base::remesh(_iters, 0, _use_projection);

  // free curvature property (refmesh has been deleted already)
  Base::mesh_.remove_property(curvature_);
}


//-----------------------------------------------------------------------------


template <class Mesh>
bool
AdaptiveRemesherT<Mesh>::
is_too_long(VertexHandle v0, VertexHandle v1) const
{
  const Point& p0 = Base::mesh_.point(v0);
  const Point& p1 = Base::mesh_.point(v1);

  Scalar c = 0.5 * ( Base::mesh_.property(curvature_, v0) +
		     Base::mesh_.property(curvature_, v1) );

  Scalar e = 2.0/c*error_ - error_*error_;
  e = (e <= 0.0 ? error_ : 2.0*sqrt(e));

  if (e<emin_) e=emin_; else if (e>emax_) e=emax_;
  Scalar emax = 4.0/3.0 * e;

  return ((p0 - p1).sqrnorm() > emax*emax);
}


//-----------------------------------------------------------------------------


template <class Mesh>
bool
AdaptiveRemesherT<Mesh>::
is_too_short(VertexHandle v0, VertexHandle v1) const
{
  const Point& p0 = Base::mesh_.point(v0);
  const Point& p1 = Base::mesh_.point(v1);

  Scalar c = 0.5 * ( Base::mesh_.property(curvature_, v0) +
		     Base::mesh_.property(curvature_, v1) );

  Scalar e = 2.0/c*error_ - error_*error_;
  e = (e <= 0.0 ? error_ : 2.0*sqrt(e));

  if (e<emin_) e=emin_; else if (e>emax_) e=emax_;
  Scalar emin = 4.0/5.0 * e;

  return ((p0 - p1).sqrnorm() < emin*emin);
}


//=============================================================================
} // namespace Remeshing
//=============================================================================
