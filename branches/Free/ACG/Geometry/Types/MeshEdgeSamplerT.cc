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
 *   $Revision$                                                         *
 *   $Author$                                                      *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*//*===========================================================================*\
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
 *   $Revision$                                                         *
 *   $Author$                                                      *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/


//=============================================================================
//
//  CLASS MeshEdgeSamplerT - IMPLEMENTATION
//
//=============================================================================

#define ACG_MESHEDGESAMPLERT_C

//== INCLUDES =================================================================

#include "MeshEdgeSamplerT.hh"
#include <math.h>
#include <float.h>
#include <map>
#include "../Algorithms.hh"

// #define MES_DEBUG_HELPER
// #define MES_DEBUG_HELPER_MOVE_ON_EDGE
// #define MES_DEBUG_TEST

//== NAMESPACES ===============================================================

namespace ACG {

//== IMPLEMENTATION ==========================================================


template <class MeshT, class PointT>
void
MeshEdgeSamplerT<MeshT, PointT>::
edge_points_in_segment( const PointT&        _p_start,
			const PointT&        _p_end,
			const FH             _fh_start,
			const FH             _fh_end,
			std::vector<PointT>& _points,
			std::vector<EH>&     _ehandles )
{
  // clear vectors for new data
  _points.clear();
  _ehandles.clear();

#ifdef MES_DEBUG_HELPER
  // ############ DEBUG HELPER ###########################
  // debug helper: mark start and end triangles
  mesh_.status(_fh_start).set_selected(true);
  mesh_.status(_fh_end).set_selected(true);
  // ############ DEBUG HELPER ###########################
#endif

  // solve trivial case
  if(_fh_start == _fh_end)
    return;

  // possible end facehandles
  std::vector<FH> endfhs;
  get_end_fhs_set( _p_end, _fh_end, endfhs);

  // vector for new points and edgehandles
  std::vector<Point> new_points0;
  std::vector<Point> new_points1;
  std::vector<EH>    new_ehs0;
  std::vector<EH>    new_ehs1;

  // current values
  HEH cur_heh0, cur_heh1;
  FH  cur_fh0, cur_fh1;

  // special case _fh_start is in endfhs but not equal to _fh_end
  if( std::find(endfhs.begin(), endfhs.end(), _fh_start) != endfhs.end())
  {
    // find best candidate
    Point  p_best;
    double d_best = FLT_MAX;

    // find opposite face which is also in endfhs
    typename Mesh::FHIter fh_it = mesh_.fh_iter(_fh_start);
    for(; fh_it; ++fh_it)
    {
      FH cur_fh = mesh_.opposite_face_handle(fh_it.handle());
      if( std::find(endfhs.begin(), endfhs.end(), cur_fh) != endfhs.end())
      {
	Point  p_new;
	double d_new = line_line_dist(_p_start, _p_end, fh_it.handle(), p_new);
	if( d_new < d_best)
	{
	  d_best = d_new;
	  p_best = p_new;
	  cur_heh0 = fh_it.handle();
	  cur_heh1 = fh_it.handle();
	  cur_fh0  = mesh_.opposite_face_handle( cur_heh0);
	  cur_fh1  = mesh_.opposite_face_handle( cur_heh1);
	}
      }
    }

    // add new points and edge handles
    new_points0.push_back(p_best);
    new_points1.push_back(p_best);
    new_ehs0.push_back( mesh_.edge_handle( cur_heh0));
    new_ehs1.push_back( mesh_.edge_handle( cur_heh1));
  }
  else // regular case
  {
    // get current plane
    compute_plane( _p_start, _p_end, _fh_start, _fh_end);

    // get two start HalfedgeHandles
    get_start_hehs( _p_start, _fh_start, cur_heh0, cur_heh1);

    // hack only one direction
    //   cur_heh0 = cur_heh1;

    // add first intersection points
    Point p_int;
    // first direction
    plane_line_intersection( cur_heh0, p_int);
    new_points0.push_back( p_int);
    new_ehs0.push_back( mesh_.edge_handle( cur_heh0));
    // second direction
    plane_line_intersection( cur_heh1, p_int);
    new_points1.push_back( p_int);
    new_ehs1.push_back( mesh_.edge_handle( cur_heh1));

    // current face_handle
    cur_fh0 = mesh_.face_handle( mesh_.opposite_halfedge_handle(cur_heh0));
    cur_fh1 = mesh_.face_handle( mesh_.opposite_halfedge_handle(cur_heh1));

    // walk until end-facehandle found
    int steps = 0;
    while( std::find(endfhs.begin(), endfhs.end(), cur_fh0) == endfhs.end() &&
	   std::find(endfhs.begin(), endfhs.end(), cur_fh1) == endfhs.end() &&
	   steps < 2000 )
    {
      // increase steps
      ++steps;

      // get next intersection
      cur_heh0 = get_next_edge_intersection( cur_heh0, new_points0.back(), p_int);
      new_points0.push_back( p_int);
      new_ehs0.push_back( mesh_.edge_handle( cur_heh0));

      // get next intersection
      cur_heh1 = get_next_edge_intersection( cur_heh1, new_points1.back(), p_int);
      new_points1.push_back( p_int);
      new_ehs1.push_back( mesh_.edge_handle( cur_heh1));

      // update current face handles
      cur_fh0 = mesh_.face_handle(mesh_.opposite_halfedge_handle(cur_heh0));
      cur_fh1 = mesh_.face_handle(mesh_.opposite_halfedge_handle(cur_heh1));

      // error handling
      if(!cur_fh0.is_valid() || !cur_heh1.is_valid())
      {
	std::cerr << "ERROR (MeshEdgeSamplerT): face handle not valid (possibly hit boundary)\n";
	break;
      }
    }
  }

  // finalize and select
  // which case finished ?
  if( std::find(endfhs.begin(), endfhs.end(), cur_fh0) != endfhs.end())
  {
    finish_list( new_points0, new_ehs0, cur_heh0, _fh_end, _p_end, endfhs.size());

    _points = new_points0;
    _ehandles = new_ehs0;
  }
  else if( std::find(endfhs.begin(), endfhs.end(), cur_fh1) != endfhs.end())
  {
    finish_list( new_points1, new_ehs1, cur_heh1, _fh_end, _p_end, endfhs.size());

    _points = new_points1;
    _ehandles = new_ehs1;
  }
  else
  {
    std::cerr << "WARNING (MeshEdgeSamplerT): Did not find FH_END\n";

    // return nothing
    _points.clear();
    _ehandles.clear();
  }


#ifdef MES_DEBUG_HELPER
  // ############ DEBUG HELPER ###########################
  // debug helper (select edges
  for(unsigned int i=0; i<_ehandles.size(); ++i)
  {
    mesh_.status( _ehandles[i]).set_selected(true);

#ifdef MES_DEBUG_HELPER_MOVE_ON_EDGE
    HEH heh0 = mesh_.halfedge_handle( _ehandles[i], 0);
    HEH heh1 = mesh_.halfedge_handle( _ehandles[i], 1);

    Point p0 = mesh_.point( mesh_.to_vertex_handle(heh0));
    Point p1 = mesh_.point( mesh_.to_vertex_handle(heh1));

    if( (_points[i]-p0).norm() < (_points[i]-p1).norm())
      _points[i] += (p1-p0)*0.25;
    else
      _points[i] -= (p1-p0)*0.25;
#endif
  }
  // ############ DEBUG HELPER ###########################
#endif
}


//-----------------------------------------------------------------------------


template <class MeshT, class PointT>
void
MeshEdgeSamplerT<MeshT, PointT>::
compute_plane(const PointT&        _p_start,
	      const PointT&        _p_end,
	      const FH             _fh_start,
	      const FH             _fh_end   )
{
  // construct cut plane
  // get first normal
  HEH heh = mesh_.halfedge_handle(_fh_start);
  Point p0      = (Point)mesh_.point(mesh_.to_vertex_handle( heh ));
  Point p1      = (Point)mesh_.point(mesh_.to_vertex_handle( heh = mesh_.next_halfedge_handle(heh) ));
  Point p2      = (Point)mesh_.point(mesh_.to_vertex_handle( heh = mesh_.next_halfedge_handle(heh) ));
  Point n_start = ((p1-p0)%(p2-p0)).normalize();

  // get second normal
  heh   = mesh_.halfedge_handle(_fh_end);
  p0    = mesh_.point(mesh_.to_vertex_handle( heh ));
  p1    = mesh_.point(mesh_.to_vertex_handle( heh = mesh_.next_halfedge_handle(heh) ));
  p2    = mesh_.point(mesh_.to_vertex_handle( heh = mesh_.next_halfedge_handle(heh) ));
  Point n_end = ((p1-p0)%(p2-p0)).normalize();

  // get average normal
  Point n_avg = n_start + n_end;
  if( n_avg.sqrnorm() < 1e-7) n_avg = n_start - n_end;
  n_avg.normalize();

  // get plane data
  n_plane_ = n_avg % (_p_start-_p_end).normalize();
  if( n_plane_.sqrnorm() > 1e-8) n_plane_.normalize();
  else std::cerr << "WARNING: Edge Resampling -> not possible to construct stable cut plane!!!\n";
  p_plane_ = (_p_start + _p_end)*0.5;
}


//-----------------------------------------------------------------------------


template <class MeshT, class PointT>
void
MeshEdgeSamplerT<MeshT, PointT>::
get_start_hehs( const Point& /*_p_start*/ ,
		const FH    _fh_start,
		      HEH&  _sheh0,
		      HEH&  _sheh1 )
{
  // get triangle halfedgehandle
  HEH heh0 = mesh_.halfedge_handle( _fh_start);
  HEH heh1 = mesh_.next_halfedge_handle( heh0);
  HEH heh2 = mesh_.next_halfedge_handle( heh1);

  Point p0 = (Point) mesh_.point(mesh_.to_vertex_handle(heh0));
  Point p1 = (Point) mesh_.point(mesh_.to_vertex_handle(heh1));
  Point p2 = (Point) mesh_.point(mesh_.to_vertex_handle(heh2));

  double eps = 1e-5*((p1-p0).norm() + (p2-p1).norm() + (p2-p0).norm());

  // degenerate case?
  if( line_in_plane( p0, p1, eps))
  {
//     std::cerr << "start_hehs degenerate case 0\n";
    _sheh0 = heh2;
    _sheh1 = heh0;
  }
  else if( line_in_plane( p1, p2, eps))
  {
//     std::cerr << "start_hehs degenerate case 1\n";
    _sheh0 = heh0;
    _sheh1 = heh1;
  }
  else if( line_in_plane( p2, p0, eps))
  {
//     std::cerr << "start_hehs degenerate case 2\n";
    _sheh0 = heh1;
    _sheh1 = heh2;
  }
  // regular case
  else
  {
    // get intersection points
    Point ip0, ip1, ip2;
    double d0 = plane_line_intersection(p0,p2, ip0);
    double d1 = plane_line_intersection(p1,p0, ip1);
    double d2 = plane_line_intersection(p2,p1, ip2);

    // sort with increasing distance
    sort_intersections( heh0, d0, ip0,
			heh1, d1, ip1,
			heh2, d2, ip2 );

    //no valid case!
    if( d1 >= eps && d2 >= eps)
    {
      std::cerr << "ERROR (MeshEdgeSamplerT): Two intersections larger than epsilon...\n";
      _sheh0 = heh0;
      _sheh1 = heh1;
    }
    else if( d0 < eps && d1 < eps && d2 >= eps)
    {
      // regular case
      _sheh0 = heh0;
      _sheh1 = heh1;
    }
    else if( d0 < eps && d1 < eps && d2 < eps)
    {
      // intersection through vertex (which hehs should be merged?)
      if( (ip0-ip1).norm() < (ip0-ip2).norm())
      {
	_sheh0 = heh0;
	_sheh1 = heh2;
      }
      else
      {
	_sheh0 = heh0;
	_sheh1 = heh1;
      }
    }
    else
    {
      std::cerr << "ERROR (MeshEdgeSamplerT): this state cannot be reached, what went wrong?\n";
      std::cerr << d0 << " " << d1 << " " << d2 << std::endl;
      _sheh0 = heh0;
      _sheh1 = heh1;
    }
  }
}


//-----------------------------------------------------------------------------


template <class MeshT, class PointT>
void
MeshEdgeSamplerT<MeshT, PointT>::
get_end_fhs_set( const Point&     _p_end,
		 const FH         _fh_end,
		 std::vector<FH>& _endfhs)
{
  _endfhs.clear();
  _endfhs.push_back( _fh_end );

  std::map<int, bool> stored_fh;
  stored_fh[ _fh_end.idx()] = true;

  double eps = epsilon( _fh_end);

  typename Mesh::ConstFaceVertexIter fv_it = mesh_.cfv_iter(_fh_end);
  // for each triangle vertex
  for(; fv_it; ++fv_it)
  {
    typename Mesh::ConstVertexFaceIter vf_iter = mesh_.cvf_iter(fv_it.handle());
    for(; vf_iter; ++vf_iter)
    {
      FH cur_fh = vf_iter.handle();

      //not already stored?
      if( stored_fh.find(cur_fh.idx()) != stored_fh.end())
	continue;

      // get point of triangle
      HEH heh0 = mesh_.halfedge_handle( cur_fh);
      HEH heh1 = mesh_.next_halfedge_handle( heh0);
      HEH heh2 = mesh_.next_halfedge_handle( heh1);

      Point p0 = (Point)mesh_.point( mesh_.to_vertex_handle( heh0));
      Point p1 = (Point)mesh_.point( mesh_.to_vertex_handle( heh1));
      Point p2 = (Point)mesh_.point( mesh_.to_vertex_handle( heh2));

      Point pc;
      double d = ACG::Geometry::distPointTriangle( _p_end,
						   p0,
						   p1,
						   p2,
						   pc);
      // distance small enough?
      if( d < eps )
      {
	_endfhs.push_back( cur_fh);
	// tag as stored
	stored_fh[cur_fh.idx()] = true;
      }
    }
  }
}


//-----------------------------------------------------------------------------


template <class MeshT, class PointT>
double
MeshEdgeSamplerT<MeshT, PointT>::
plane_line_intersection( const Point& _p0,
			 const Point& _p1,
			 Point& _p_int)
{
  double a = (n_plane_ | (p_plane_-_p0));
  double b = (n_plane_ | (_p1-_p0));

  if( fabs(b) > 1e-8)
  {
    double s = a/b;

    _p_int = _p0 + (_p1-_p0)*s;

    // TEST Intersection Point
    if( fabs((n_plane_| (_p_int-p_plane_)) ) > 1e-8)
      std::cerr << "WARNING (MeshEdgeSamplerT): wrong intersection point!!!\n";


    // in range ?
    if( s >= 0.0 && s<= 1.0)
      return 0.0;
    else
      return std::min( (_p_int-_p1).norm(), (_p_int-_p0).norm());
  }
  else
  {
    std::cerr << "WARNING (MeshEdgeSamplerT): Line and Plane parallel!!!\n";
    return fabs((n_plane_| (_p0-p_plane_)));
  }
}

//-----------------------------------------------------------------------------


template <class MeshT, class PointT>
double
MeshEdgeSamplerT<MeshT, PointT>::
plane_line_intersection( const HEH _heh,
			 Point& _p_int)
{
  Point p0 = mesh_.point(mesh_.from_vertex_handle(_heh));
  Point p1 = mesh_.point(mesh_.  to_vertex_handle(_heh));

  return plane_line_intersection( p0, p1, _p_int);
}


//-----------------------------------------------------------------------------


template <class MeshT, class PointT>
bool
MeshEdgeSamplerT<MeshT, PointT>::
line_in_plane( const Point& _p0,
	       const Point& _p1,
	       double       _eps)
{
  if( plane_dist( (_p0+_p1)*0.5) < _eps &&
      ((_p1-_p0).normalize() | n_plane_) < _eps )
    return true;
  else
    return false;
}


//-----------------------------------------------------------------------------


template <class MeshT, class PointT>
bool
MeshEdgeSamplerT<MeshT, PointT>::
line_in_plane( const HEH& _heh )
{
  Point p0 = mesh_.point(mesh_.from_vertex_handle(_heh));
  Point p1 = mesh_.point(mesh_.  to_vertex_handle(_heh));

  double eps = (p1-p0).norm()*3e-5;

  return line_in_plane( p0, p1, eps);
}


//-----------------------------------------------------------------------------


template <class MeshT, class PointT>
double
MeshEdgeSamplerT<MeshT, PointT>::
plane_dist( const Point& _p0 )

{
  return fabs((n_plane_| (_p0-p_plane_)));
}


//-----------------------------------------------------------------------------

template <class MeshT, class PointT>
double
MeshEdgeSamplerT<MeshT, PointT>::
line_line_dist(const Point& _p_start,
	       const Point& _p_end,
	       const HEH&   _heh,
	             Point& _p_best)
{
  Point phe0 = mesh_.point(mesh_.to_vertex_handle(_heh));
  Point phe1 = mesh_.point(mesh_.to_vertex_handle(mesh_.opposite_halfedge_handle(_heh)));
  Point pb0;

  return ACG::Geometry::distLineLine(_p_start,
				     _p_end,
				     phe0,
				     phe1,
				     &pb0,
				     &_p_best);
}

//-----------------------------------------------------------------------------


template <class MeshT, class PointT>
typename MeshEdgeSamplerT<MeshT, PointT>::HEH
MeshEdgeSamplerT<MeshT, PointT>::
get_next_edge_intersection( const HEH& _heh, const Point& _pold, Point& _pint)
{
  HEH opp_heh = mesh_.opposite_halfedge_handle( _heh);

  if( mesh_.is_boundary( _heh ))
  {
    std::cerr << "ERROR (MeshEdgeSamplerT): next edge was boundary!!!\n";
    return _heh;
  }

  // candidate halfedges
  HEH cheh0 = mesh_.next_halfedge_handle( opp_heh);
  HEH cheh1 = mesh_.next_halfedge_handle( cheh0 );

  // degenerate case?
  if( line_in_plane( cheh0))
  {
    plane_line_intersection( cheh1, _pint);
    return cheh1;
  }
  else if( line_in_plane( cheh1))
  {
    plane_line_intersection( cheh0, _pint);
    return cheh0;
  }
  else // regular case
  {
    Point p_int0, p_int1;
    double d0 = plane_line_intersection( cheh0, p_int0);
    double d1 = plane_line_intersection( cheh1, p_int1);

    double eps = epsilon(_heh);

    // two intersections?
    if( d0 < eps && d1 < eps)
    {
      // take farther intesection
      if( (_pold-p_int0).norm() > (_pold-p_int1).norm())
      {
	_pint = p_int0;
	return cheh0;
      }
      else
      {
	_pint = p_int1;
	return cheh1;
      }
    }
    else
    {
      if( d0 <= d1 )  // take better candidate
      {
	_pint = p_int0;
	return cheh0;
      }
      else
      {
	_pint = p_int1;
	return cheh1;
      }
    }
  }
}


//-----------------------------------------------------------------------------


template <class MeshT, class PointT>
void
MeshEdgeSamplerT<MeshT, PointT>::
sort_intersections( HEH& _heh0, double& _d0, Point& _ip0,
		    HEH& _heh1, double& _d1, Point& _ip1,
		    HEH& _heh2, double& _d2, Point& _ip2 )
{
  if( _d0 > _d1)
  {
    std::swap( _heh0, _heh1);
    std::swap( _d0, _d1);
    std::swap( _ip0, _ip1);
  }

  if( _d1 > _d2)
  {
    std::swap( _heh1, _heh2);
    std::swap( _d1, _d2);
    std::swap( _ip1, _ip2);
  }

  if( _d0 > _d1)
  {
    std::swap( _heh0, _heh1);
    std::swap( _d0, _d1);
    std::swap( _ip0, _ip1);
  }
}


//-----------------------------------------------------------------------------


template <class MeshT, class PointT>
double
MeshEdgeSamplerT<MeshT, PointT>::
epsilon( const FH& _fh)
{
  // get point of triangle
  HEH heh0 = mesh_.halfedge_handle( _fh);
  HEH heh1 = mesh_.next_halfedge_handle( heh0);
  HEH heh2 = mesh_.next_halfedge_handle( heh1);

  Point p0 = (Point)mesh_.point( mesh_.to_vertex_handle( heh0));
  Point p1 = (Point)mesh_.point( mesh_.to_vertex_handle( heh1));
  Point p2 = (Point)mesh_.point( mesh_.to_vertex_handle( heh2));

  return 1e-5*((p1-p0).norm() + (p2-p1).norm() + (p0-p2).norm());
}

//-----------------------------------------------------------------------------


template <class MeshT, class PointT>
double
MeshEdgeSamplerT<MeshT, PointT>::
epsilon( const HEH& _heh)
{
  // get point of edge
  Point p0 = (Point)mesh_.point( mesh_.to_vertex_handle  ( _heh));
  Point p1 = (Point)mesh_.point( mesh_.from_vertex_handle( _heh));

  return 3e-5*((p1-p0).norm());
}


//-----------------------------------------------------------------------------


template <class MeshT, class PointT>
void
MeshEdgeSamplerT<MeshT, PointT>::
finish_list( std::vector<Point>& _points,
	     std::vector<EH>&    _ehs,
	     const HEH           _cur_heh,
	     const FH            _fh_end,
	     const Point&        _p_end,
	     const int           _type)

{
  // get heh in face and current face handle
  HEH cur_opp_heh = mesh_.opposite_halfedge_handle(_cur_heh);
  FH  cur_fh = mesh_.face_handle( cur_opp_heh);

  // get two other hehs of triangle
  HEH heh0 = mesh_.next_halfedge_handle( cur_opp_heh );
  HEH heh1 = mesh_.next_halfedge_handle( heh0);

  // already finished? (only one endfh)
  if( cur_fh == _fh_end) return;
  else if( _type == 2) // endpoint near edge -> opposite face handle
  {
    FH fhc0 = mesh_.face_handle(mesh_.opposite_halfedge_handle(heh0));
    FH fhc1 = mesh_.face_handle(mesh_.opposite_halfedge_handle(heh1));
    FH fhc2 = mesh_.face_handle(_cur_heh);

    // add endpoint
    _points.push_back(_p_end);

    if( fhc0 == _fh_end)
      _ehs.push_back( mesh_.edge_handle(heh0));
    else
      if( fhc1 == _fh_end)
	_ehs.push_back( mesh_.edge_handle(heh1));
      else
      {
	_ehs.push_back(mesh_.edge_handle(heh0));
	std::cerr << "ERROR: could not find end fh...\n";
	std::cerr << (fhc2.idx() == _fh_end.idx()) << std::endl;

// 	static bool first_error=true;
// 	if(first_error)
// 	{
// 	  first_error = false;
// 	  std::cerr << "size : " << _ehs.size() << std::endl;
// 	  std::cerr << "prev : " << fhc2.idx() << std::endl;
// 	  std::cerr << "cur : " << cur_fh.idx() << std::endl;
// 	  std::cerr << "next0: " << fhc0.idx() << std::endl;
// 	  std::cerr << "next1: " << fhc1.idx() << std::endl;
// 	  std::cerr << "end : " << _fh_end.idx() << std::endl;

// 	  mesh_.status(_fh_end).set_selected(true);
// 	  mesh_.status(fhc2).set_selected(true);
// 	  mesh_.status(cur_fh).set_selected(true);
// 	}

      }

//     // get triangle points
//     Point p0 = mesh_.point( mesh_.to_vertex_handle(heh0));
//     Point p1 = mesh_.point( mesh_.to_vertex_handle(heh1));
//     Point p2 = mesh_.point( mesh_.to_vertex_handle(cur_opp_heh));

//     // find corresponding edge
//     double ed0 = ACG::Geometry::distPointLine( _p_end, p2, p0);
//     double ed1 = ACG::Geometry::distPointLine( _p_end, p0, p1);

//     // add endpoint
//     _points.push_back(_p_end);

//     if( ed0 < ed1)
//     {
//       _ehs.push_back( mesh_.edge_handle( heh0));
//       // verify result
//       if( mesh_.face_handle( mesh_.opposite_halfedge_handle( heh0)) != _fh_end)
//       {
// 	std::cerr << "ERROR (MeshEdgeSamplerT): opposite edge handle is not correct (0) !!!\n";
// #ifdef MES_DEBUG_TEST
// 	// ############ DEBUG HELPER ###########################
// 	// debug helper (select edge)
// 	mesh_.status( mesh_.edge_handle( heh0)).set_selected(true);
// 	// ############ DEBUG HELPER ###########################
// #endif
//       }
//     }
//     else
//     {
//       _ehs.push_back( mesh_.edge_handle( heh1));
//       // verify result
//       if( mesh_.face_handle( mesh_.opposite_halfedge_handle( heh1)) != _fh_end)
//       {
// 	std::cerr << "ERROR (MeshEdgeSamplerT): opposite edge handle is not correct (1) !!!\n";
// #ifdef MES_DEBUG_TEST
// 	// ############ DEBUG HELPER ###########################
// 	// debug helper (select edge)
// 	mesh_.status( mesh_.edge_handle( heh1)).set_selected(true);
// 	// ############ DEBUG HELPER ###########################
// #endif
//       }
//     }
  }
  else if( _type > 2) // endpoint near triangle point -> circulate
  {
    std::vector<EH> cw_ehs;
    circulate_CW( heh1, _fh_end, cw_ehs);

    std::vector<EH> ccw_ehs;
    circulate_CCW(heh0, _fh_end, ccw_ehs);

    // select shorter path to _fh_end
    if( cw_ehs.size() < ccw_ehs.size())
    {
      for(unsigned int i=0; i<cw_ehs.size(); ++i)
      {
	_ehs.push_back( cw_ehs[i]);
	_points.push_back( _p_end);
      }
    }
    else
    {
      for(unsigned int i=0; i<ccw_ehs.size(); ++i)
      {
	_ehs.push_back( ccw_ehs[i]);
	_points.push_back( _p_end);
      }
    }

    if( cw_ehs.size() == 0 && ccw_ehs.size() == 0)
    {
      std::cerr << "circulation failed vidx: " << mesh_.to_vertex_handle( heh0).idx() << std::endl;
      mesh_.status( mesh_.to_vertex_handle( heh0)).set_selected(true);
    }
  }
}


//-----------------------------------------------------------------------------


template <class MeshT, class PointT>
void
MeshEdgeSamplerT<MeshT, PointT>::
finish_list_old( std::vector<Point>& _points,
	     std::vector<EH>&    _ehs,
	     const HEH           _cur_heh,
	     const FH            _fh_end,
 	     const Point&        _p_end,
	     const int           _t)
{
  // get heh in face and current face handle
  HEH cur_opp_heh = mesh_.opposite_halfedge_handle(_cur_heh);
  FH  cur_fh = mesh_.face_handle( cur_opp_heh);

  // get two other hehs of triangle
  HEH heh0 = mesh_.next_halfedge_handle( cur_opp_heh );
  HEH heh1 = mesh_.next_halfedge_handle( heh0);

  // get epsilon
  double eps = epsilon( cur_fh );

  VH circulate_vh(-1);

  // already finished?
  if( cur_fh == _fh_end) return;
  else
  {
    // get triangle points
    Point p0 = mesh_.point( mesh_.to_vertex_handle(heh0));
    Point p1 = mesh_.point( mesh_.to_vertex_handle(heh1));
    Point p2 = mesh_.point( mesh_.to_vertex_handle(cur_opp_heh));

    double d0 = (_p_end - p0).norm();
    double d1 = (_p_end - p1).norm();
    double d2 = (_p_end - p2).norm();

    std::cerr << "d0 d1 d2 " << d0 << " " << d1 << " " << d2 << std::endl;
    std::cerr << mesh_.to_vertex_handle(heh0).idx() << " "
	      << mesh_.to_vertex_handle(heh1).idx() << " "
	      << mesh_.to_vertex_handle(cur_opp_heh).idx() << std::endl;

    // find corresponding circulation vertex
    if( d0 < eps)      circulate_vh = mesh_.to_vertex_handle( heh0);
    else if( d1 < eps) circulate_vh = mesh_.to_vertex_handle( heh1);
    else if( d2 < eps) circulate_vh = mesh_.to_vertex_handle( cur_opp_heh);
    else
    {
      // else find corresponding edge
      double ed0 = ACG::Geometry::distPointLine( _p_end, p2, p0);
      double ed1 = ACG::Geometry::distPointLine( _p_end, p0, p1);

      // add endpoint
      _points.push_back(_p_end);

      if( ed0 < eps)
      {
	_ehs.push_back( mesh_.edge_handle( heh0));
	// verify result
	if( mesh_.face_handle( mesh_.opposite_halfedge_handle( heh0)) != _fh_end)
	  std::cerr << "ERROR (MeshEdgeSamplerT): opposite edge handle is not correct (0) !!!\n";
      }
      else if( ed1 < eps)
      {
	_ehs.push_back( mesh_.edge_handle( heh1));
	// verify result
	if( mesh_.face_handle( mesh_.opposite_halfedge_handle( heh1)) != _fh_end)
	  std::cerr << "ERROR (MeshEdgeSamplerT): opposite edge handle is not correct (1) !!!\n";
      }
      else
      {
	std::cerr << "ERROR (MeshEdgeSamplerT): endpoint should be near edge...\n";
	_ehs.push_back( mesh_.edge_handle( heh0));
      }
    }
  }

  if( circulate_vh == mesh_.to_vertex_handle(heh0))
  {
    std::vector<EH> cw_ehs;
    circulate_CW( heh1, _fh_end, cw_ehs);

    std::vector<EH> ccw_ehs;
    circulate_CCW(heh0, _fh_end, ccw_ehs);

    // select shorter path to _fh_end
    if( cw_ehs.size() < ccw_ehs.size())
    {
      for(unsigned int i=0; i<cw_ehs.size(); ++i)
      {
	_ehs.push_back( cw_ehs[i]);
	_points.push_back( _p_end);
      }
    }
    else
    {
      for(unsigned int i=0; i<ccw_ehs.size(); ++i)
      {
	_ehs.push_back( ccw_ehs[i]);
	_points.push_back( _p_end);
      }
    }
  }
  else if( circulate_vh == mesh_.to_vertex_handle(heh1))
  {
    std::vector<EH> ccw_ehs;
    circulate_CCW(heh1, _fh_end, ccw_ehs);

    for(unsigned int i=0; i<ccw_ehs.size(); ++i)
    {
      _ehs.push_back( ccw_ehs[i]);
      _points.push_back( _p_end);
    }
  }
  else if( circulate_vh == mesh_.to_vertex_handle(cur_opp_heh))
  {
    std::vector<EH> cw_ehs;
    circulate_CW(heh0, _fh_end, cw_ehs);

    for(unsigned int i=0; i<cw_ehs.size(); ++i)
    {
      _ehs.push_back( cw_ehs[i]);
      _points.push_back( _p_end);
    }
  }
}


//-----------------------------------------------------------------------------


template <class MeshT, class PointT>
void
MeshEdgeSamplerT<MeshT, PointT>::
circulate_CW(HEH _heh, FH _fh_end, std::vector<EH>& _cw_ehs)
{
  _cw_ehs.clear();

  // insert first heh
  HEH cur_heh = _heh;
  _cw_ehs.push_back( mesh_.edge_handle(cur_heh) );

  while( mesh_.face_handle(mesh_.opposite_halfedge_handle( cur_heh)) != _fh_end )
  {
    cur_heh = mesh_.opposite_halfedge_handle( cur_heh);
    cur_heh = mesh_.next_halfedge_handle(cur_heh);

    _cw_ehs.push_back( mesh_.edge_handle( cur_heh));

    // error check
    if( cur_heh == _heh)
    {
      std::cerr << "ERROR (MeshEdgeSamplerT): circulate_CW could not find _fh_end... "
		<< _cw_ehs.size() << std::endl;
      _cw_ehs.clear();
      break;
    }
  }
}


//-----------------------------------------------------------------------------


template <class MeshT, class PointT>
void
MeshEdgeSamplerT<MeshT, PointT>::
circulate_CCW(HEH _heh, FH _fh_end, std::vector<EH>& _ccw_ehs)
{
  _ccw_ehs.clear();

  // insert first heh
  HEH cur_heh = _heh;
  _ccw_ehs.push_back( mesh_.edge_handle(cur_heh) );

  while( mesh_.face_handle(mesh_.opposite_halfedge_handle( cur_heh)) != _fh_end )
  {
    cur_heh = mesh_.opposite_halfedge_handle( cur_heh);
    cur_heh = mesh_.next_halfedge_handle(cur_heh);
    cur_heh = mesh_.next_halfedge_handle(cur_heh);

    _ccw_ehs.push_back( mesh_.edge_handle( cur_heh));

    // error check
    if( cur_heh == _heh)
    {
      std::cerr << "ERROR (MeshEdgeSamplerT): circulate_CCW could not find _fh_end... "
		<< _ccw_ehs.size() << std::endl;
      _ccw_ehs.clear();
      break;
    }
  }
}

//=============================================================================
} // namespace ACG
//=============================================================================
