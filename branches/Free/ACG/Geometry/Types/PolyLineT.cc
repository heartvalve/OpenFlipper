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
//  CLASS PolyLineT - IMPLEMENTATION
//
//=============================================================================

#define ACG_POLYLINET_C

//== INCLUDES =================================================================

#include <OpenMesh/Core/Geometry/VectorT.hh>

#include <iostream>
#include <fstream>
#include <map>

#include "PolyLineT.hh"
#include "MeshEdgeSamplerT.hh"

#include <float.h>
#include <ACG/Geometry/Algorithms.hh>

#ifdef USE_PHYSIM
#include <PhySim/Meshes/ConstrainedDelaunayT.hh>
#endif

#ifndef WIN32
#include <stdlib.h>
#endif

#ifdef USE_OPENMP
#include <omp.h>
#endif

//== NAMESPACES ===============================================================

namespace ACG {

//== IMPLEMENTATION ==========================================================


// comparison class for polyline_mesh_integration
template <class MT>
class Ecmp
{
public:
  Ecmp( MT& _mesh, typename MT::Point _v) : mesh_(_mesh), v_(_v) {}
  
  bool operator()(const int& _vidx0, const int& _vidx1)
  {
    typename MT::Scalar d0 = (mesh_.point( mesh_.vertex_handle(_vidx0)) | v_);
    typename MT::Scalar d1 = (mesh_.point( mesh_.vertex_handle(_vidx1)) | v_);
    
    return d0 < d1;
  }
  
  
  MT&                 mesh_;
  typename MT::Point  v_;
};


//-----------------------------------------------------------------------------


/// Constructor
template <class PointT>
PolyLineT<PointT>::
  PolyLineT( bool _closed )
    : closed_(_closed),
      ref_count_vnormals_(0),
      ref_count_vcolors_(0),
      ref_count_vscalars_(0),
      ref_count_vselections_(0),
      ref_count_vvhandles_(0),
      ref_count_vehandles_(0),
      ref_count_vfhandles_(0),
      ref_count_enormals_(0),
      ref_count_ecolors_(0),
      ref_count_escalars_(0),
      ref_count_eselections_(0)
{
}

//-----------------------------------------------------------------------------

/// Copy Constructor
template <class PointT>
PolyLineT<PointT>::
  PolyLineT( const PolyLineT& _line )
{
  closed_ = _line.closed_;

  //copy points
  points_ = _line.points_;

  //copy vertex properties
  vnormals_    = _line.vnormals_;
  vcolors_     = _line.vcolors_;
  vscalars_    = _line.vscalars_;
  vselections_ = _line.vselections_;
  vvhandles_   = _line.vvhandles_;
  vehandles_   = _line.vehandles_;
  vfhandles_   = _line.vfhandles_;

  //copy edge properties
  enormals_    = _line.enormals_;
  ecolors_     = _line.ecolors_;
  escalars_    = _line.escalars_;
  eselections_ = _line.eselections_;

  // property reference counter
  ref_count_vnormals_    = _line.ref_count_vnormals_;
  ref_count_vcolors_     = _line.ref_count_vcolors_;
  ref_count_vscalars_    = _line.ref_count_vscalars_;
  ref_count_vselections_ = _line.ref_count_vselections_;
  ref_count_vvhandles_   = _line.ref_count_vvhandles_;
  ref_count_vehandles_   = _line.ref_count_vehandles_;
  ref_count_vfhandles_   = _line.ref_count_vfhandles_;

  ref_count_enormals_    = _line.ref_count_enormals_;
  ref_count_ecolors_     = _line.ref_count_ecolors_;
  ref_count_escalars_    = _line.ref_count_escalars_;
  ref_count_eselections_ = _line.ref_count_eselections_;
}

//-----------------------------------------------------------------------------


template <class PointT>
void
PolyLineT<PointT>::
clear()
{
  points_.clear();

  // clear propertiy vectors
  vnormals_.clear();
  vcolors_.clear();
  vscalars_.clear();
  vselections_.clear();
  vvhandles_.clear();
  vehandles_.clear();
  vfhandles_.clear();

  enormals_.clear();
  ecolors_.clear();
  escalars_.clear();
  eselections_.clear();
}


//-----------------------------------------------------------------------------


template <class PointT>
void
PolyLineT<PointT>::
resize( unsigned int _n)
{
  if( _n < n_vertices())
  {

    points_.resize( _n);

    // clear propertiy vectors
    if( vertex_normals_available() )
      vnormals_.resize( _n);
    if( vertex_colors_available())
      vcolors_.resize( _n);
    if( vertex_scalars_available())
      vscalars_.resize( _n);
    if( vertex_selections_available())
      vselections_.resize( _n);
    if( vertex_vhandles_available())
      vvhandles_.resize( _n);
    if( vertex_ehandles_available())
      vehandles_.resize( _n);
    if( vertex_fhandles_available())
      vfhandles_.resize( _n);

    if( edge_normals_available())
      enormals_.resize( _n);
    if( edge_colors_available())
      ecolors_.resize( _n);
    if( edge_scalars_available())
      escalars_.resize( _n);
    if( edge_selections_available())
      eselections_.resize( _n);
  }
  else
  {
    while( n_vertices() < _n)
      add_point( Point());
  }
}


//-----------------------------------------------------------------------------


template <class PointT>
void
PolyLineT<PointT>::
add_point(const Point& _p)
{
  // add new point
  points_.push_back( _p );

  // add available properties
  if( vertex_normals_available() )
    vnormals_.push_back( Point(0,0,0));

  if( vertex_colors_available())
    vcolors_.push_back( Point(1,0,1));

  if( vertex_scalars_available())
    vscalars_.push_back( 0.0 );

  if( vertex_selections_available())
    vselections_.push_back( false);

  if( vertex_vhandles_available())
    vvhandles_.push_back(-1);

  if( vertex_ehandles_available())
    vehandles_.push_back(-1);

  if( vertex_fhandles_available())
    vfhandles_.push_back(-1);

  if( edge_normals_available())
    enormals_.push_back( Point(0,0,0));

  if( edge_colors_available())
    ecolors_.push_back( Point(1,0,1));

  if( edge_scalars_available())
    escalars_.push_back( 0.0);

  if( edge_selections_available())
    eselections_.push_back(false);
}

//-----------------------------------------------------------------------------

template <class PointT>
void
PolyLineT<PointT>::
insert_point(int _idx, const Point& _p)
{
  assert(_idx < (int)n_vertices() );
  
  // insert new point
  points_.insert(points_.begin()+_idx, _p);

  // insert available properties
  if( vertex_normals_available() )
    vnormals_.insert(vnormals_.begin()+_idx, Point(0,0,0));

  if( vertex_colors_available())
    vcolors_.insert(vcolors_.begin()+_idx, Point(1,0,1));

  if( vertex_scalars_available())
    vscalars_.insert(vscalars_.begin()+_idx, 0.0 );

  if( vertex_selections_available())
    vselections_.insert(vselections_.begin()+_idx, false);

  if( vertex_vhandles_available())
    vvhandles_.insert(vvhandles_.begin()+_idx, -1);

  if( vertex_ehandles_available())
    vehandles_.insert(vehandles_.begin()+_idx, -1);

  if( vertex_fhandles_available())
    vfhandles_.insert(vfhandles_.begin()+_idx, -1);

  if( edge_normals_available())
    enormals_.insert(enormals_.begin()+_idx, Point(0,0,0));

  if( edge_colors_available())
    ecolors_.insert(ecolors_.begin()+_idx, Point(1,0,1));

  if( edge_scalars_available())
    escalars_.insert(escalars_.begin()+_idx, 0.0);

  if( edge_selections_available())
    eselections_.insert(eselections_.begin()+_idx, false);
}


//-----------------------------------------------------------------------------
  

template <class PointT>
void
PolyLineT<PointT>::
delete_point(int _idx)
{
  assert(_idx < (int)n_vertices() );

  // delete point at given index
  points_.erase(points_.begin()+_idx);


  // delete available properties
  if( vertex_normals_available() )
    vnormals_.erase(vnormals_.begin()+_idx);

  if( vertex_colors_available())
    vcolors_.erase(vcolors_.begin()+_idx);

  if( vertex_scalars_available())
    vscalars_.erase(vscalars_.begin()+_idx);

  if( vertex_selections_available())
    vselections_.erase(vselections_.begin()+_idx);

  if( vertex_vhandles_available())
    vvhandles_.erase(vvhandles_.begin()+_idx);

  if( vertex_ehandles_available())
    vehandles_.erase(vehandles_.begin()+_idx);

  if( vertex_fhandles_available())
    vfhandles_.erase(vfhandles_.begin()+_idx);

  if( edge_normals_available())
    enormals_.erase(enormals_.begin()+_idx);

  if( edge_colors_available())
    ecolors_.erase(ecolors_.begin()+_idx);

  if( edge_scalars_available())
    escalars_.erase(escalars_.begin()+_idx);

  if( edge_selections_available())
    eselections_.erase(eselections_.begin()+_idx);
}


//-----------------------------------------------------------------------------


template <class PointT>
typename PolyLineT<PointT>::Scalar
PolyLineT<PointT>::
length() const
{
  Scalar l = 0;

  unsigned int n = points_.size();

  if( !closed_) --n;

  for(unsigned int i=0; i<n; ++i)
  {
    l += (points_[(i+1)%n]-points_[i]).norm();
  }

  return l;
}

//-----------------------------------------------------------------------------


template <class PointT>
void
PolyLineT<PointT>::
subdivide_old(Scalar _largest)
{
  // check validity
  if(!points_.size()) return;

  unsigned int n_subdivisions = 1;

  while( n_subdivisions != 0)
  {

    n_subdivisions = 0;

    std::vector<Point> new_points;

    new_points.push_back( points_[0]);

    // squared maximal length
    Scalar l2 = _largest*_largest;

    for(unsigned int i=1; i<points_.size(); ++i)
    {
      if( (new_points.back() - points_[i]).sqrnorm() > l2)
      {
	Point mid_point = (new_points.back() + points_[i])*0.5;

	new_points.push_back( mid_point );
	++n_subdivisions;
      }

      new_points.push_back( points_[i]);
    }

    // last interval for closed polyline
    if( closed_)
    {
      if( (new_points.back() - points_[0]).sqrnorm() > l2)
      {
	Point mid_point = (new_points.back() + points_[0])*0.5;
	new_points.push_back( mid_point );
	++n_subdivisions;
      }
    }

    // update points
    resize( new_points.size() );
    points_ = new_points;
  }
}


//-----------------------------------------------------------------------------


template <class PointT>
void
PolyLineT<PointT>::
subdivide(Scalar _largest)
{
  // check validity
  if(!n_vertices()) return;

  unsigned int n_subdivisions = 1;

  while( n_subdivisions != 0)
  {

    n_subdivisions = 0;

    // add new polyline and add first point
    PolyLineT<PointT> new_pl = *this;
    new_pl.resize(1);
    new_pl.copy_vertex_complete(*this, 0, 0);

    // squared maximal length
    Scalar l2 = _largest*_largest;

    for(unsigned int i=1; i<points_.size(); ++i)
    {
      if( (new_pl.point(new_pl.n_vertices()-1) - points_[i]).sqrnorm() > l2)
      {
	Point mid_point = (new_pl.point(new_pl.n_vertices()-1) + points_[i])*0.5;

	new_pl.add_point( mid_point );
	++n_subdivisions;
      }

      // copy vertex
      new_pl.resize( new_pl.n_vertices()+1);
      new_pl.copy_vertex_complete( *this, i, new_pl.n_vertices()-1);
    }

    // last interval for closed polyline
    if( closed_)
    {
      if( (new_pl.point(new_pl.n_vertices()-1) - points_[0]).sqrnorm() > l2)
      {
	Point mid_point = (new_pl.point(new_pl.n_vertices()-1) + points_[0])*0.5;
	new_pl.add_point( mid_point );
	++n_subdivisions;
      }
    }

    // update points
    *this = new_pl;
  }
}

//-----------------------------------------------------------------------------


template <class PointT>
void
PolyLineT<PointT>::
collapse(Scalar _smallest)
{
  // check validity
  if(!n_vertices()) return;

  unsigned int n_collapses = 1;

  unsigned int n_iter = 0;

  while( n_collapses != 0 && n_iter < 5)
  {
    ++n_iter;
    n_collapses = 0;

    // create new PolyLine (with all properties) and insert first point
    PolyLineT<PointT> new_pl = *this;
    new_pl.resize(1);
    new_pl.copy_vertex_complete( *this, 0, 0);

    // squared maximal length
    Scalar l2 = _smallest*_smallest;

    for(unsigned int i=1; i<points_.size(); ++i)
    {
      // check whether vertex is selected
      bool vertex_selected = false;
      if( vertex_selections_available() && vertex_selection(i))
	vertex_selected = true;

      if( (new_pl.point(new_pl.n_vertices()-1) - points_[i]).sqrnorm() >= l2 ||
	  vertex_selected ||
	  (!closed_ && i==points_.size()-1) )
      {
	// copy next point
	new_pl.resize( new_pl.n_vertices()+1);
	new_pl.copy_vertex_complete( *this, i, new_pl.n_vertices()-1);
      }
      else ++n_collapses;
    }

    // last interval for closed polyline
    if( closed_)
    {
      // check whether vertex is selected
      bool vertex_selected = false;
      if( vertex_selections_available() && vertex_selection(points_.size()-1))
	vertex_selected = true;

      if( (new_pl.point(new_pl.n_vertices()-1) - points_[0]).sqrnorm() < l2 && !vertex_selected)
      {
	new_pl.resize( new_pl.n_vertices()-1);
      }
      else ++n_collapses;
    }

    // update points
    *this = new_pl;
  }
}


//-----------------------------------------------------------------------------


template <class PointT>
void
PolyLineT<PointT>::
collapse_old(Scalar _smallest)
{
  // check validity
  if(!points_.size()) return;

  unsigned int n_collapses = 1;

  unsigned int n_iter = 0;

  while( n_collapses != 0 && n_iter < 5)
  {
    ++n_iter;
    n_collapses = 0;

    std::vector<Point> new_points;

    new_points.push_back( points_[0]);

    // squared maximal length
    Scalar l2 = _smallest*_smallest;

    for(unsigned int i=1; i<points_.size(); ++i)
    {
      if( (new_points.back() - points_[i]).sqrnorm() >= l2)
      {
	new_points.push_back( points_[i]);
      }
      else ++n_collapses;
    }

    // last interval for closed polyline
    if( closed_)
    {
      if( (new_points.back() - points_[0]).sqrnorm() >= l2)
      {
	new_points.resize( new_points.size()-1);
      }
      else ++n_collapses;
    }
    else
    {
      // always add endpoint for open polylines
      if( points_.back() != new_points.back())
	new_points.back() = points_.back();
    }

    // update points
    resize( new_points.size() );
    points_ = new_points;
  }
}


//-----------------------------------------------------------------------------


template <class PointT>
void
PolyLineT<PointT>::
smooth_uniform_laplace()
{
  // copy point positions
  std::vector<Point> points_old( points_ );

  int n = points_.size();

  int is = 0;
  int ie = n;

  if( !closed_ )
  {
    ++is;
    --ie;
  }

  if( vertex_selections_available())
  {
    #ifdef USE_OPENMP
    #pragma omp parallel for
    #endif
    for( int i=is; i<ie; ++i)
    {
      // only smooth not selected vertices
      if( !vertex_selection(i))
      // laplace stencil 1,-2,1
	     points_[i] = (points_old[ (i-1+n)%n ] +
		                points_old[ (i+n  )%n ]*2.0 +
		                points_old[ (i+1  )%n ]      )*0.25;

    }
  }
  else
  {
    #ifdef USE_OPENMP
    #pragma omp parallel for
    #endif
    for( int i=is; i<ie; ++i)
    {
      // laplace stencil 1,-2,1
      points_[i] = (points_old[ (i-1+n)%n ]       +
		              points_old[ (i+n  )%n ] * 2.0 +
		              points_old[ (i+1  )%n ]         )*0.25;
    }
  }
}


//-----------------------------------------------------------------------------


template <class PointT>
void
PolyLineT<PointT>::
smooth_uniform_laplace2()
{
  // copy point positions
  std::vector<Point> points_old( points_ );

  int n = points_.size();

  int is = 0;
  int ie = n;

  if( !closed_ )
  {
    is+=2;
    ie-=2;
  }

  if( vertex_selections_available())
  {
    #ifdef USE_OPENMP
    #pragma omp parallel for
    #endif
    for(int i=is; i<ie; ++i)
    {
      // only smooth not selected vertices
      if( !vertex_selection(i))
        // laplace^2 stencil 1,-4,6,-4,1
        points_[i] -= (points_old[ (i-2+2*n)%n ]      +
                       points_old[ (i-1+2*n)%n ]*-4.0 +
                       points_old[ (i      )%n ]* 6.0 +
                       points_old[ (i+1    )%n ]*-4.0 +
                       points_old[ (i+2    )%n ]      )/(16.0*2.0);
    }
  }
  else
  {
    #ifdef USE_OPENMP
    #pragma omp parallel for
    #endif
    for(int i=is; i<ie; ++i)
    {
      // laplace^2 stencil 1,-4,6,-4,1
      points_[i] -= (points_old[ (i-2+2*n)%n ]      +
		               points_old[ (i-1+2*n)%n ]*-4.0 +
                     points_old[ (i      )%n ]* 6.0 +
                     points_old[ (i+1    )%n ]*-4.0 +
                     points_old[ (i+2    )%n ]      )/(16.0*2.0);
    }
  }
}


//-----------------------------------------------------------------------------


template <class PointT>
void
PolyLineT<PointT>::
smooth_uniform_laplace3()
{
  // copy point positions
  std::vector<Point> points_old( points_ );

  int n = points_.size();

  int is = 0;
  int ie = n;

  if( !closed_ )
  {
    is+=3;
    ie-=3;
  }

  if( vertex_selections_available())
  {
    #ifdef USE_OPENMP
    #pragma omp parallel for
    #endif
    for( int i=is; i<ie; ++i)
    {
      // only smooth not selected vertices
      if( !vertex_selection(i))
	     // laplace^3 stencil 1,-6,15,-20,15,-6,1
	     points_[i] = (points_old[ (i-3+3*n)%n ]        +
                      points_old[ (i-2+3*n)%n ]*(-6.0) +
                      points_old[ (i-1+3*n)%n ]*15.0   +
                      points_old[ (i      )   ]*(44.0) +
                      points_old[ (i+1    )%n ]*15.0   +
                      points_old[ (i+2    )%n ]*(-6.0) +
                      points_old[ (i+3    )%n ]        )/64.0;

    }
  }
  else
  {
    #ifdef USE_OPENMP
    #pragma omp parallel for
    #endif
    for( int i=is; i<ie; ++i)
    {
	   // laplace^3 stencil 1,-6,15,-20,15,-6,1
	   points_[i] = (points_old[ (i-3+3*n)%n ]        +
                    points_old[ (i-2+3*n)%n ]*(-6.0) +
                    points_old[ (i-1+3*n)%n ]*15.0   +
                    points_old[ (i      )   ]*(44.0) +
                    points_old[ (i+1    )%n ]*15.0   +
                    points_old[ (i+2    )%n ]*(-6.0) +
                    points_old[ (i+3    )%n ]        )/64.0;
    }
  }
}


//-----------------------------------------------------------------------------


template <class PointT>
template <class MeshT, class SpatialSearchT>
void
PolyLineT<PointT>::
project_to_mesh( const MeshT& _mesh, SpatialSearchT * _ssearch)
{
  typename MeshT::FaceHandle fh;

  #ifdef USE_OPENMP
  #pragma omp parallel for
  #endif
  for(unsigned int i=0; i<points_.size(); ++i)
  {
    points_[i] = find_nearest_point( _mesh, points_[i], fh, _ssearch);
  }
}


//-----------------------------------------------------------------------------


template <class PointT>
template <class MeshT, class SpatialSearchT>
void
PolyLineT<PointT>::
project_to_mesh( const std::vector<MeshT*>     _mesh,
		           std::vector<SpatialSearchT*>* _ssearch)
{
  typename MeshT::FaceHandle fh;

  #ifdef USE_OPENMP
  #pragma omp parallel for
  #endif
  for(int i=0; i< (int)points_.size(); ++i)
  {
    // init d_best
    typename MeshT::Scalar d_best = -1;

    // best point
    Point p_best(0,0,0);

    // iterate over all possible meshes
    for(unsigned int j=0; j<_mesh.size(); ++j)
    {
      double d_new(-1);

      Point p_new;
      if(_ssearch != 0)
	     p_new = find_nearest_point( *(_mesh[j]), points_[i], fh, ((*_ssearch)[j]), &d_new);
      else
	     p_new = find_nearest_point( *(_mesh[j]), points_[i], fh, (SpatialSearchT*)0, &d_new);

      // store best result
      if( d_new < d_best || d_best == -1)
      {
	     p_best = p_new;
	     d_best = d_new;
      }
    }

    if( d_best != -1)
      points_[i] = p_best;
  }
}


//-----------------------------------------------------------------------------


template <class PointT>
template <class MeshT, class SpatialSearchT>
typename PolyLineT<PointT>::Point
PolyLineT<PointT>::
find_nearest_point( const MeshT&                _mesh,
		    const Point&                _point,
		    typename MeshT::FaceHandle& _fh,
		    SpatialSearchT *            _ssearch,
		    double*                     _dbest)
{
  typename MeshT::Point p0 = (typename MeshT::Point) _point;

  typename MeshT::Point  p_best = _mesh.point(_mesh.vertex_handle(0));
  typename MeshT::Scalar d_best = (p0-p_best).sqrnorm();

  typename MeshT::FaceHandle fh_best;

  if( _ssearch == 0 )
  {
    // exhaustive search
    typename MeshT::ConstFaceIter cf_it  = _mesh.faces_begin();
    typename MeshT::ConstFaceIter cf_end = _mesh.faces_end();

    for(; cf_it != cf_end; ++cf_it)
    {
      typename MeshT::ConstFaceVertexIter cfv_it = _mesh.cfv_iter(cf_it);

      const typename MeshT::Point& pt0 = _mesh.point(   cfv_it);
      const typename MeshT::Point& pt1 = _mesh.point( ++cfv_it);
      const typename MeshT::Point& pt2 = _mesh.point( ++cfv_it);

      typename MeshT::Point ptn;

      typename MeshT::Scalar d = Geometry::distPointTriangleSquared( p0,
								     pt0,
								     pt1,
								     pt2,
								     ptn );

      if( d < d_best)
      {
	d_best = d;
	p_best = ptn;

	fh_best = cf_it.handle();
      }
    }

    // return facehandle
    _fh = fh_best;

    // return distance
    if(_dbest)
      *_dbest = sqrt(d_best);


    return (Point) p_best;
  }
  else
  {
    typename MeshT::FaceHandle     fh = _ssearch->nearest(p0).handle;
    typename MeshT::CFVIter        fv_it = _mesh.cfv_iter(fh);

    const typename MeshT::Point&   pt0 = _mesh.point(  fv_it);
    const typename MeshT::Point&   pt1 = _mesh.point(++fv_it);
    const typename MeshT::Point&   pt2 = _mesh.point(++fv_it);

    // project
    d_best = Geometry::distPointTriangleSquared(p0, pt0, pt1, pt2, p_best);

    // return facehandle
    _fh = fh;

    // return distance
    if(_dbest)
      *_dbest = sqrt(d_best);

    return (Point) p_best;
  }
}

//-----------------------------------------------------------------------------


template <class PointT>
template <class MeshT, class SpatialSearchT>
void
PolyLineT<PointT>::
resample_on_mesh_edges( MeshT& _mesh, SpatialSearchT * _ssearch)
{
  // init new empty polyline
  PolyLineT<PointT> new_pl = *this;

  // vectors for new segment data
  std::vector<Point> new_points;
  std::vector<typename MeshT::EdgeHandle> new_ehandles;

  // current segment data
  Point p_start, p_end;
  typename MeshT::FaceHandle fh_start, fh_end;

  // init first endpoint
  p_end = find_nearest_point( _mesh, point(0), fh_end, _ssearch);

  // add start point
  new_pl.resize(1);
  new_pl.copy_vertex_complete(*this, 0, 0);
  new_pl.point(0) = p_end;
  // update properties
  if( new_pl.vertex_fhandles_available())
    new_pl.vertex_fhandle(0) = fh_end.idx();
  if( new_pl.vertex_ehandles_available())
    new_pl.vertex_ehandle(0) = -1;

  MeshEdgeSamplerT<MeshT, Point> mesampler( _mesh );

  for(unsigned int i=1; i<n_vertices(); ++i)
  {
    // first segment point is last one from previous step!
    p_start  = p_end;
    fh_start = fh_end;
    // get next segment point
    p_end   = find_nearest_point( _mesh, point(i), fh_end  , _ssearch);

    // get points for first segment
    mesampler.edge_points_in_segment( p_start,
				      p_end,
				      fh_start,
				      fh_end,
				      new_points,
				      new_ehandles );

//     // get points for first segment
//     edge_points_in_segment( _mesh,
// 			    p_start,
// 			    p_end,
// 			    fh_start,
// 			    fh_end,
// 			    new_points,
// 			    new_ehandles );

    // add new edge points
    for( unsigned int j=0; j< new_points.size(); ++j)
    {
      new_pl.add_point( new_points[j]);
      // update properties
      if( new_pl.vertex_ehandles_available())
	new_pl.vertex_ehandle( new_pl.n_vertices()-1) = new_ehandles[j].idx();
      if( new_pl.vertex_fhandles_available())
	new_pl.vertex_fhandle( new_pl.n_vertices()-1) = -1;
    }

    // add new intermediate point
    new_pl.resize( new_pl.n_vertices() + 1);
    new_pl.copy_vertex_complete(*this, i, new_pl.n_vertices()-1);
    new_pl.point(new_pl.n_vertices()-1) = p_end;
    // update properties
    if( new_pl.vertex_fhandles_available())
      new_pl.vertex_fhandle( new_pl.n_vertices()-1) = fh_end.idx();
    if( new_pl.vertex_ehandles_available())
      new_pl.vertex_ehandle( new_pl.n_vertices()-1) = -1;
  }

  // add points in last segment
  if(is_closed())
  {
    // first segment point is last one from previous step!
    p_start  = p_end;
    fh_start = fh_end;
    // get next segment point
    p_end   = find_nearest_point( _mesh, point(0), fh_end  , _ssearch);

    // get points for first segment
    mesampler.edge_points_in_segment( p_start,
				      p_end,
				      fh_start,
				      fh_end,
				      new_points,
				      new_ehandles );

//     // get points for first segment
//     edge_points_in_segment( _mesh,
// 			    p_start,
// 			    p_end,
// 			    fh_start,
// 			    fh_end,
// 			    new_points,
// 			    new_ehandles );

    // add new edge points
    for( unsigned int j=0; j< new_points.size(); ++j)
    {
      new_pl.add_point( new_points[j]);
      // update properties
      if( new_pl.vertex_ehandles_available())
	new_pl.vertex_ehandle( new_pl.n_vertices()-1) = new_ehandles[j].idx();
      if( new_pl.vertex_fhandles_available())
	new_pl.vertex_fhandle( new_pl.n_vertices()-1) = -1;
    }
  }

  // overwrite polyline with result
  *this = new_pl;
}


//-----------------------------------------------------------------------------

#ifdef USE_PHYSIM

template <class PointT>
template <class MeshT, class SpatialSearchT>
void
PolyLineT<PointT>::
integrate_into_mesh( MeshT& _mesh, SpatialSearchT * _ssearch)
{
  if( is_closed() && points_.size() < 2)
  {
    std::cerr << "Warning: closed polyline with only one vertex!!!\n";
    return;
  }

  // resample on mesh edges
  resample_on_mesh_edges( _mesh, _ssearch);

  //ToDo: Cleanup

  if( !vertex_ehandles_available() ||
      !vertex_fhandles_available()    )
  {
    std::cerr << "ERROR: PolyLine::integrate_into_mesh needs ehandles and fhandles!!!\n";
    return;
  }

  //Add points, if polyline starts or ends near boundary
  add_boundary_points(_mesh);

  // build constrained delaunay problems
  typedef std::pair<int,int>                                     FSegment;
  typedef std::pair< std::vector< FSegment >, std::vector<int> > FData;
  typedef std::map< int, FData >                                 FSPMap;
  // for each face we need a list of segments(=constraints) and
  // a list of interior vertices
  FSPMap face_map;

  typedef std::vector<int>       EData;
  typedef std::map< int, EData > EVMap;
  // for each edge we need a list of vertices lying on this edge
  // this data has to be sorted !!!
  EVMap edge_map;

  // add vertices to mesh
  std::vector<int> plvhs;
  int last_eidx = -1;
  int last_fidx = -1;
  int last_vidx = -1;
  int boundary_closed = is_closed();

  for(unsigned int i=0; i<points_.size()+boundary_closed; ++i)
  {
    int vidx = 0;
    int eidx = vertex_ehandle(i%points_.size());
    int fidx = vertex_fhandle(i%points_.size());

    // not last segment?
    if( i < points_.size())
    {
      plvhs.push_back( _mesh.add_vertex( (typename MeshT::Point) points_[i]).idx());

      vidx = plvhs.back();

      // error checking
      if( eidx != -1 && fidx != -1)
	std::cerr << "ERROR: eidx and fidx are valid simultaneously, this shouldn't happen!!!!\n";

      // add edge vertex reference
      if( eidx != -1)
	edge_map[eidx].push_back(vidx);
      
      // add inner face vertex reference
      if( fidx != -1)
	face_map[fidx].second.push_back( vidx );
    }
    else vidx = plvhs[0]; // take first vertex

    // add segments as delaunay constraints
    if( i>0 )
    {
      // inner vertex case
      if( ( fidx != -1 && last_eidx != -1) ||
	  ( eidx != -1 && last_fidx != -1) ||
	  ( fidx != -1 && last_fidx != -1 && fidx == last_fidx) )
      {
	int valid_fidx = std::max(fidx, last_fidx);

	face_map[valid_fidx].first.push_back( FSegment(vidx, last_vidx) );
      }
      else
	if( eidx != -1 && last_eidx != -1)
	{
	  typename MeshT::EdgeHandle eh      = _mesh.edge_handle(      eidx );
	  typename MeshT::HalfedgeHandle heh0= _mesh.halfedge_handle( eh, 0);
	  typename MeshT::HalfedgeHandle heh1= _mesh.halfedge_handle( eh, 1);

	  typename MeshT::FaceHandle fh0, fh1;
	  // edge might be at boundary !
	  if( !_mesh.is_boundary( heh0)) fh0 = _mesh.face_handle(heh0);
	  else                           fh0 = _mesh.face_handle(heh1);
	  if( !_mesh.is_boundary( heh1)) fh1 = _mesh.face_handle(heh1);
	  else                           fh1 = _mesh.face_handle(heh0);

	  std::pair<int,int> fhs( fh0.idx(), fh1.idx());

	  typename MeshT::EdgeHandle     last_eh  = _mesh.edge_handle( last_eidx );
	  typename MeshT::HalfedgeHandle last_heh0= _mesh.halfedge_handle( last_eh, 0);
	  typename MeshT::HalfedgeHandle last_heh1= _mesh.halfedge_handle( last_eh, 1);

	  typename MeshT::FaceHandle last_fh0, last_fh1;
	  // edge might be at boundary !
	  if( !_mesh.is_boundary( last_heh0)) last_fh0 = _mesh.face_handle(last_heh0);
	  else                                last_fh0 = _mesh.face_handle(last_heh1);
	  if( !_mesh.is_boundary( last_heh1)) last_fh1 = _mesh.face_handle(last_heh1);
	  else                                last_fh1 = _mesh.face_handle(last_heh0);

	  std::pair<int,int> last_fhs( last_fh0.idx(), last_fh1.idx());

	  int common_fh = -1;

	  if( fhs.first == last_fhs.first ||
	      fhs.first == last_fhs.second   )
	    common_fh = fhs.first;
	  else
	    if( fhs.second == last_fhs.first ||
		fhs.second == last_fhs.second   )
	      common_fh = fhs.second;
	    else
	    {
	      std::cerr << "ERROR: Polyline::integrate_into_mesh no common facehandle!!!\n";
	    }

	  face_map[common_fh].first.push_back( FSegment(vidx, last_vidx));
	}
	else
	{
	  std::cerr << "ERROR: unhandled segment case!!!\n";
	  std::cerr << "vidx: " << vidx << " / last " << last_vidx << std::endl;
	  std::cerr << "fidx: " << fidx << " / last " << last_fidx << std::endl;
	  std::cerr << "eidx: " << eidx << " / last " << last_eidx << std::endl;

	}
    }

    // store data for next segment
    last_eidx = eidx;
    last_fidx = fidx;
    last_vidx = vidx;
  }

  // sort edge vertices for halfedge_handle0
  EVMap::iterator ev_it  = edge_map.begin();
  for(; ev_it != edge_map.end(); ++ev_it)
  {
    typename MeshT::EdgeHandle eh = _mesh.edge_handle( ev_it->first );
    typename MeshT::Point      p0 = _mesh.point( _mesh.to_vertex_handle( _mesh.halfedge_handle(eh,0)));
    typename MeshT::Point      p1 = _mesh.point( _mesh.to_vertex_handle( _mesh.halfedge_handle(eh,1)));

    std::sort( ev_it->second.begin(), ev_it->second.end(), Ecmp<MeshT>(_mesh, p0-p1 ));
  }
  
  // setup edge constraints for faces
  FSPMap::iterator fsp_it  = face_map.begin();
  for(; fsp_it != face_map.end(); ++fsp_it)
  {
    typename MeshT::FaceHandle fh = _mesh.face_handle( fsp_it->first );
    typename MeshT::FHIter  fh_it = _mesh.fh_iter(fh);
    for(; fh_it; ++fh_it)
    {
      // get endpoints
      typename MeshT::VertexHandle vh_start = _mesh.from_vertex_handle(fh_it.handle());
      typename MeshT::VertexHandle vh_end   = _mesh.to_vertex_handle  (fh_it.handle());

      typename MeshT::EdgeHandle eh = _mesh.edge_handle( fh_it.handle());

      // add start vertex to face
      fsp_it->second.second.push_back( vh_start.idx() );

      // correct ordering? (for first halfedge)
      std::vector<int> edge_ordered ( edge_map[ eh.idx()]);
      if( fh_it.handle() != _mesh.halfedge_handle(eh, 0) )
	std::reverse(edge_ordered.begin(), edge_ordered.end());
      
      int prev_idx = vh_start.idx();

      std::vector<int>::iterator ev_it  = edge_ordered.begin();
      std::vector<int>::iterator ev_end = edge_ordered.end();

      for(; ev_it != ev_end; ++ev_it)
      {
	// add segment
	fsp_it->second.first.push_back( FSegment(prev_idx, *ev_it) );

	// add edge vertex to face
	fsp_it->second.second.push_back( *ev_it );

	prev_idx = *ev_it;
      }
      // add last segment
      fsp_it->second.first.push_back( FSegment(prev_idx, vh_end.idx()) );
    }
  }

  // update edge map for triangle consistency check
  ev_it  = edge_map.begin();
  for(; ev_it != edge_map.end(); ++ev_it)
  {
    typename MeshT::EdgeHandle     eh   = _mesh.edge_handle( ev_it->first );
    typename MeshT::HalfedgeHandle heh0 = _mesh.halfedge_handle( eh, 0 );
    typename MeshT::HalfedgeHandle heh1 = _mesh.halfedge_handle( eh, 1 );

    edge_map[eh.idx()].push_back( _mesh.to_vertex_handle(heh0).idx());
    edge_map[eh.idx()].push_back( _mesh.to_vertex_handle(heh1).idx());
  }


  // remove faces, solve cdelaunay, and insert new faces
  ACG::ConstrainedDelaunayT<double> cdelaunay;
  std::vector<int>                  orig_idx;
  std::map<int,int>                 new_idx;
  fsp_it  = face_map.begin();
  for(; fsp_it != face_map.end(); ++fsp_it)
  {
    typename MeshT::FaceHandle fh = _mesh.face_handle( fsp_it->first );
    int fidx = fh.idx();

    // get edge handles for later consistency check
    std::vector<int> ehs;
    typename MeshT::FEIter fe_it = _mesh.fe_iter( fh );
    for(; fe_it; ++fe_it)
      ehs.push_back( fe_it.handle().idx());
    
    // set normal for delaunay problem
    cdelaunay.clear();
    cdelaunay.set_normal( _mesh.normal(fh) );

    // add vertices of triangle cdelaunay problem
    orig_idx.clear();
    new_idx.clear();
    std::vector<int>::iterator fv_it  = fsp_it->second.second.begin();
    std::vector<int>::iterator fv_end = fsp_it->second.second.end();
    for(; fv_it != fv_end; ++fv_it)
    {
      new_idx[*fv_it] = orig_idx.size();
      orig_idx.push_back( *fv_it );
      bool valid = cdelaunay.add_point((ACG::ConstrainedDelaunayT<double>::Vec3r) _mesh.point(_mesh.vertex_handle(*fv_it)));

      if( !valid )
      {
	std::cerr << "ERROR: CGAL could not add point, aborting polyline integration...\n";
	// remove valence 0 vertices
	typename MeshT::VertexIter v_it  = _mesh.vertices_begin();
	for(; v_it != _mesh.vertices_end(); ++v_it)
	{
	  if( !_mesh.valence(v_it.handle()))
	    _mesh.delete_vertex(v_it.handle());
	}
	_mesh.garbage_collection();
	_mesh.update_normals();

	return;
      }
    }

    // add constraints
    std::vector<FSegment>::iterator fc_it  = fsp_it->second.first.begin();
    std::vector<FSegment>::iterator fc_end = fsp_it->second.first.end();
    for(; fc_it != fc_end; ++fc_it)
    {
      cdelaunay.add_constraint( new_idx[fc_it->first], new_idx[fc_it->second] );
    }

    // get new triangles and add them to _mesh
    std::vector< std::vector< int> > tri_list;
    cdelaunay.get_triangles( tri_list);

    // delete old face
    _mesh.delete_face(fh, false);

    //    if( fsp_it == face_map.begin())
    for(unsigned int i=0; i<tri_list.size(); ++i)
    {
      std::vector<typename MeshT::VertexHandle > tvhs;
      for(unsigned int j=0; j<tri_list[i].size(); ++j)
	tvhs.push_back( _mesh.vertex_handle( orig_idx[tri_list[i][j]]) );

      // check if face has is valid = not all vertices on the same input triangle edge
      bool face_valid = true;
      for(unsigned int j=0; j<ehs.size(); ++j)
      {
	if( tvhs.size() == 3)
	{
	  // ede_map iterators
	  std::vector<int>::iterator em_begin = edge_map[ehs[j]].begin();
	  std::vector<int>::iterator em_end   = edge_map[ehs[j]].end();

	  // all three vertices on same edge?
	  if( std::find( em_begin, em_end, tvhs[0].idx()) != em_end &&
	      std::find( em_begin, em_end, tvhs[1].idx()) != em_end &&
	      std::find( em_begin, em_end, tvhs[2].idx()) != em_end   )
	  {
	    face_valid = false;
	  }
	}
	else std::cerr << "ERROR: integrate polyline found triangle which has not 3 vertices...\n";
      }


      if( face_valid)
	_mesh.add_face( tvhs );

    }
  }

  //select polyline edges
  for(unsigned int i=1; i<points_.size()+boundary_closed; ++i)
  {
    // get vertex handles of segment
    typename MeshT::VertexHandle vh0 = _mesh.vertex_handle( plvhs[i-1] );
    typename MeshT::VertexHandle vh1 = _mesh.vertex_handle( plvhs[i%points_.size()] );

    // find connecting edge
    bool found_edge = false;
    typename MeshT::VOHIter voh_it = _mesh.voh_iter(vh0);
    for(; voh_it; ++voh_it)
      if( _mesh.to_vertex_handle(voh_it.handle()) == vh1)
      {
	_mesh.status( _mesh.edge_handle( voh_it.handle())).set_selected(true);
	found_edge = true;
      }

    if( !found_edge )
      std::cerr << "Warning: could not find polyline edge: " 
		<< i-1 << " to " << i%points_.size() 
		<< std::endl;
  }

  // clean up mesh
  _mesh.garbage_collection();
  _mesh.update_normals();
}

#endif

//-----------------------------------------------------------------------------


template <class PointT>
template <class MeshT>
void
PolyLineT<PointT>::
add_boundary_points(MeshT& _mesh)
{
  // face handles should be available
  if( ! vertex_fhandles_available() ) return;

  // polyline should have at least one point
  if( points_.empty()) return;

  if( !is_closed() )
  {
    // check first vertex
    int fidx = vertex_fhandle(0);
    if( fidx != -1)
    {
      typename MeshT::Point p = (typename MeshT::Point) (points_[0]);
      
      double                closest_dist = FLT_MAX;
      typename MeshT::Point closest_boundary(0,0,0), pb(0,0,0);
      int                   closest_eidx = -1;
      typename MeshT::FHIter fh_it = _mesh.fh_iter( _mesh.face_handle(fidx));

      for(; fh_it; ++fh_it)
      {
	if( _mesh.is_boundary( _mesh.opposite_halfedge_handle(fh_it.handle())))
	{
	  typename MeshT::Point v0 = _mesh.point(_mesh.to_vertex_handle  (fh_it.handle()));
	  typename MeshT::Point v1 = _mesh.point(_mesh.from_vertex_handle(fh_it.handle()));

	  double d = ACG::Geometry::distPointLineSquared(p, v0, v1, &pb);

	  if( d < closest_dist)
	  {
	    closest_dist     = d;
	    closest_boundary = pb;
	    closest_eidx     = _mesh.edge_handle( fh_it.handle()).idx();
	  }
	}
      }
      
      // if boundary point found, add it
      if( closest_eidx != -1 )
      {
	insert_point( 0, closest_boundary);
	vertex_ehandle(0) = closest_eidx;
      }
      
    }

    // check last vertex
    int pvidx = points_.size()-1;
    fidx  = vertex_fhandle(pvidx);
    if( fidx != -1)
    {
      typename MeshT::Point p = (typename MeshT::Point) (points_[pvidx]);
      
      double                closest_dist = FLT_MAX;
      typename MeshT::Point closest_boundary(0,0,0), pb(0,0,0);
      int                   closest_eidx = -1;
      typename MeshT::FHIter fh_it = _mesh.fh_iter( _mesh.face_handle(fidx));

      for(; fh_it; ++fh_it)
      {
	if( _mesh.is_boundary( _mesh.opposite_halfedge_handle(fh_it.handle())))
	{
	  typename MeshT::Point v0 = _mesh.point(_mesh.to_vertex_handle  (fh_it.handle()));
	  typename MeshT::Point v1 = _mesh.point(_mesh.from_vertex_handle(fh_it.handle()));

	  double d = ACG::Geometry::distPointLineSquared(p, v0, v1, &pb);

	  if( d < closest_dist)
	  {
	    closest_dist     = d;
	    closest_boundary = pb;
	    closest_eidx     = _mesh.edge_handle( fh_it.handle()).idx();
	  }
	}
      }
      
      // if boundary point found, add it
      if( closest_eidx != -1 )
      {
	add_point( closest_boundary);
	vertex_ehandle( points_.size()-1 ) = closest_eidx;
      }
    }
  }
}


//-----------------------------------------------------------------------------


template <class PointT>
template <class LineNodeT>
LineNodeT*
PolyLineT<PointT>::
get_line_node(LineNodeT*& _line_node, int _mode)
{
  typedef typename LineNodeT::value_type Vec3fL;

  if( _mode == 0)
  {
    // LineSegmentsMode

    // create LineNode
    _line_node = new LineNodeT(LineNodeT::LineSegmentsMode, 0, "PolyLine");
    _line_node->set_line_width(5.0);
    //   _line_node->set_base_color(Vec4f(0,1,0,0));
    _line_node->set_base_color(OpenMesh::Vec4f(0.2 + double(rand())/double(RAND_MAX)*0.8,
					       0.2 + double(rand())/double(RAND_MAX)*0.8,
					       0.2 + double(rand())/double(RAND_MAX)*0.8,
					       1.0));

    _line_node->show();

    // add line node
    for( unsigned int i=0; i<this->points().size()-1; ++i)
    {
      _line_node->add_line( (Vec3fL) this->points()[i  ],
			    (Vec3fL) this->points()[i+1] );
    }

    // close loop
    if(closed_)
      if( !this->points().empty() )
      {
	_line_node->add_point( (Vec3fL) this->points()[0]);
      }

    return _line_node;
  }
  else
  {
    // create LineNode
    _line_node = new LineNodeT(LineNodeT::PolygonMode, 0, "PolyLine");
    _line_node->set_line_width(5.0);
    //   _line_node->set_base_color(Vec4f(0,1,0,0));
    _line_node->set_base_color(OpenMesh::Vec4f(0.2 + double(rand())/double(RAND_MAX)*0.8,
					       0.2 + double(rand())/double(RAND_MAX)*0.8,
					       0.2 + double(rand())/double(RAND_MAX)*0.8,
					       1.0));

    _line_node->show();

    // add line node
    for( unsigned int i=0; i<this->points().size(); ++i)
    {
      _line_node->add_point( (Vec3fL) this->points()[i]);
    }

    // close loop
    if(closed_)
      if( !this->points().empty() )
      {
	_line_node->add_point( (Vec3fL) this->points()[0]);
      }

    return _line_node;
  }
}


//-----------------------------------------------------------------------------


template <class PointT>
template <class LineNodeT>
void
PolyLineT<PointT>::
set_line_node(LineNodeT*& _line_node, int _mode)
{
  //  typedef typename LineNodeT::value_type Vec3fL;

  // clear old values
  clear();

  if( _mode == 0)
  {
    // assume LineSegmentsMode

    const typename LineNodeT::PointVector& ln_points = _line_node->points();

    for(unsigned int i=0; i<ln_points.size();)
    {
      if( i != ln_points.size()-1)
	add_point( (Point) ln_points[i] );
      else
      {
	// last point
	if( (ln_points[ln_points.size()-1]-ln_points[0]).sqrnorm() == 0)
	{
	  closed_ = true;
	}
	else
	{
	  closed_ = false;
	  add_point( (Point) ln_points[i] );
	}
      }


      // increase counter
      if( i == 0) i+=1;
      else        i+=2;
    }
  }
  else
  {
    // assume PolygonMode
    closed_ = true;

    const typename LineNodeT::PointVector& ln_points( _line_node->points());

    for(unsigned int i=0; i<ln_points.size(); ++i)
    {
      add_point( (Point) ln_points[i] );
    }
  }
}


//-----------------------------------------------------------------------------


template <class PointT>
void
PolyLineT<PointT>::
print() const
{
  std::cerr << "****** PolyInfo ******\n";
  std::cerr << "closed : " << closed_ << std::endl;
  std::cerr << "#points: " << points_.size() << std::endl;
  for(unsigned int i=0; i<points_.size(); ++i)
    std::cerr << points_[i] << std::endl;
}


//-----------------------------------------------------------------------------


template <class PointT>
void
PolyLineT<PointT>::
load(const char* _filename)
{
  // clear old polyline
  clear();

  std::ifstream fin(_filename, std::ios::in);

  // closed ?
  fin >> closed_;

  // number of points
  int num_points;
  fin >> num_points;

//   std::cerr << "read " << _filename << std::endl;
//   std::cerr << "#points: " << num_points << std::endl;

  // read points
  for(int i=0; i<num_points; ++i)
  {
    Scalar x,y,z;
    fin >> x;
    fin >> y;
    fin >> z;
    Point p(x,y,z);
    add_point(p);
  }

  // ###########################
  // READ properties

  std::string token;

  fin >> token;

  if(vertex_vhandles_available())
  {
    if( token == "VVHANDLES")
    {
//       std::cerr << "read VVHANDLES...\n";
      for( unsigned int i=0; i<n_vertices(); ++i)
	fin >> vertex_vhandle(i);

      fin >> token;
    }
  }

  if(vertex_ehandles_available())
  {
    if( token == "VEHANDLES")
    {
//       std::cerr << "read VEHANDLES...\n";
      for( unsigned int i=0; i<n_vertices(); ++i)
      {
	fin >> vertex_ehandle(i);
      }

      fin >> token;
    }
  }

  if(vertex_fhandles_available())
  {
    if( token == "VFHANDLES")
    {
//       std::cerr << "read VFHANDLES...\n";

      for( unsigned int i=0; i<n_vertices(); ++i)
	fin >> vertex_fhandle(i);

      fin >> token;
    }
  }

  fin.close();
}


//-----------------------------------------------------------------------------


template <class PointT>
void
PolyLineT<PointT>::
save(const char* _filename) const
{
  std::ofstream fout(_filename, std::ios::out);

  //  is polyline closed?
  fout << closed_ << std::endl;

  // number of points
  fout << points_.size() << std::endl;

  std::cerr << "write " << _filename << std::endl;
  std::cerr << "#points: " << points_.size() << std::endl;


  // write each point
  for(unsigned int i=0; i<points_.size(); ++i)
  {

    fout << points_[i][0] << " ";
    fout << points_[i][1] << " ";
    fout << points_[i][2] << std::endl;
  }

  // ###########################
  // Write properties

  if(vertex_vhandles_available())
  {
    fout << "VVHANDLES" << std::endl;
    for( unsigned int i=0; i<n_vertices(); ++i)
      fout << vertex_vhandle(i) << std::endl;
  }

  if(vertex_ehandles_available())
  {
    fout << "VEHANDLES" << std::endl;
    for( unsigned int i=0; i<n_vertices(); ++i)
    {
      fout << vertex_ehandle(i) << std::endl;
    }
  }

  if(vertex_fhandles_available())
  {
    fout << "VFHANDLES" << std::endl;
    for( unsigned int i=0; i<n_vertices(); ++i)
      fout << vertex_fhandle(i) << std::endl;
  }

  fout.close();
}


//-----------------------------------------------------------------------------


template <class PointT>
template <class PropT>
void
PolyLineT<PointT>::
request_prop( unsigned int& _ref_count, PropT& _prop)
{
  if(_ref_count == 0)
  {
    _ref_count = 1;
    // always use vertex size!!!
    _prop.resize(n_vertices());
  }
  else ++_ref_count;
}


//-----------------------------------------------------------------------------


template <class PointT>
template <class PropT>
void
PolyLineT<PointT>::
release_prop( unsigned int& _ref_count, PropT& _prop)
{
  if( _ref_count <= 1)
  {
    _ref_count = 0;
    _prop.clear();
  }
  else --_ref_count;
}


//-----------------------------------------------------------------------------


template <class PointT>
void
PolyLineT<PointT>::
copy_vertex_complete(const PolyLineT<PointT>& _pl, unsigned int _i, unsigned int _j)
{
  // check range
  if( n_vertices() <= _j || _pl.n_vertices() <= _i)
  {
    std::cerr << "Warning: invalid range in PolyLine::copy_vertex_complete ( "
	      << _i << " " << _j << " ) " << std::endl;
    return;
  }

  // copy point position
  point(_j) = _pl.point(_i);

  // copy properties if available

  // vertex normal
  if( _pl.vertex_normals_available())
    if(   vertex_normals_available())
      vertex_normal(_j) = _pl.vertex_normal(_i);

  // vertex colors
  if( _pl.vertex_colors_available())
    if(   vertex_colors_available())
      vertex_color(_j) = _pl.vertex_color(_i);

  // vertex scalar
  if( _pl.vertex_scalars_available())
    if(   vertex_scalars_available())
      vertex_scalar(_j) = _pl.vertex_scalar(_i);

  // vertex selection
  if( _pl.vertex_selections_available())
    if(   vertex_selections_available())
      vertex_selection(_j) = _pl.vertex_selection(_i);

  // vertex vhandle
  if( _pl.vertex_vhandles_available())
    if(   vertex_vhandles_available())
      vertex_vhandle(_j) = _pl.vertex_vhandle(_i);

  // vertex ehandle
  if( _pl.vertex_ehandles_available())
    if(   vertex_ehandles_available())
      vertex_ehandle(_j) = _pl.vertex_ehandle(_i);

  // vertex vhandle
  if( _pl.vertex_ehandles_available())
    if(   vertex_ehandles_available())
      vertex_ehandle(_j) = _pl.vertex_ehandle(_i);
}


//-----------------------------------------------------------------------------


template <class PointT>
void
PolyLineT<PointT>::
copy_edge_complete(const PolyLineT<PointT>& _pl, unsigned int _i, unsigned int _j)
{
  // check range
  if( n_edges() <= _j || _pl.n_edges() <= _i)
  {
    std::cerr << "Warning: invalid range in PolyLine::copy_edge_complete ( "
	      << _i << " " << _j << " ) " << std::endl;
    return;
  }

  // edge normal
  if( _pl.edge_selections_available())
    if(   edge_selections_available())
      edge_selection(_j) = _pl.edge_selection(_i);


  // edge normal
  if( _pl.edge_normals_available())
    if(   edge_normals_available())
      edge_normal(_j) = _pl.edge_normal(_i);

  // edge color
  if( _pl.edge_colors_available())
    if(   edge_colors_available())
      edge_color(_j) = _pl.edge_color(_i);

  // edge scalar
  if( _pl.edge_scalars_available())
    if(   edge_scalars_available())
      edge_scalar(_j) = _pl.edge_scalar(_i);

  // edge normal
  if( _pl.edge_selections_available())
    if(   edge_selections_available())
      edge_selection(_j) = _pl.edge_selection(_i);

  // edge selection
  if( _pl.edge_selections_available())
    if(   edge_selections_available())
      edge_selection(_j) = _pl.edge_selection(_i);
}


//-----------------------------------------------------------------------------


template <class PointT>
void
PolyLineT<PointT>::
invert()
{
  // inversion is only supported for open polylines
  if(is_closed())
  {
    std::cerr << "Warning: inversion for closed polylines is not supported!!!\n";
    return;
  }

  PolyLineT<PointT> pl_temp = *this;

  // copy vertices in reverse order
  for(unsigned int i=0; i<n_vertices(); ++i)
    pl_temp.copy_vertex_complete( *this, n_vertices()-1-i, i);

  // copy edges in reverse order
  for(unsigned int i=0; i<n_edges(); ++i)
    pl_temp.copy_edge_complete( *this, n_edges()-1-i, i);

  // save inverted polyline
  *this = pl_temp;
}


//-----------------------------------------------------------------------------


template <class PointT>
void
PolyLineT<PointT>::
append(const PolyLineT<PointT>& _pl)
{
  // operation not supported for closed polylines
  if( is_closed() || _pl.is_closed())
  {
    std::cerr << is_closed() << " " << _pl.is_closed() << std::endl;
    std::cerr << "Warning: appending not supported for closed polylines!!!\n";
    return;
  }

  unsigned int old_nv = n_vertices();
  unsigned int old_ne = n_edges();

  resize( n_vertices() + _pl.n_vertices());

  for( unsigned int i=0; i<_pl.n_vertices(); ++i)
    copy_vertex_complete( _pl, i, i+old_nv);

  for( unsigned int i=0; i<_pl.n_edges(); ++i)
    copy_edge_complete( _pl, i, i+old_ne+1);
}


//-----------------------------------------------------------------------------


template <class PointT>
void
PolyLineT<PointT>::
prepend(const PolyLineT<PointT>& _pl)
{
  PolyLineT<PointT> pl_temp = _pl;

  pl_temp.append(*this);
  *this = pl_temp;
}


//-----------------------------------------------------------------------------


template <class PointT>
void
PolyLineT<PointT>::
split_closed( unsigned int _split_idx)
{
  if(!is_closed())
  {
    std::cerr << "Warning: split_closed was called for open polyline!\n";
    return;
  }

  // prepare new polyline
  PolyLineT<PointT> pl_temp = *this;
  pl_temp.resize(n_vertices()+1);
  pl_temp.set_closed( false);


  // splitted polyline has n+1 vertices
  for(unsigned int i=0; i<n_vertices()+1; ++i)
    pl_temp.copy_vertex_complete( *this, (i+_split_idx)%n_vertices(),i);

  for(unsigned int i=0; i<n_edges(); ++i)
    pl_temp.copy_edge_complete( *this, (i+_split_idx)%n_edges(),i);

  // copy updated polyline
  *this = pl_temp;
}


//-----------------------------------------------------------------------------


template <class PointT>
void
PolyLineT<PointT>::
split( unsigned int _split_idx, PolyLineT<PointT>& _new_pl)
{
  if( is_closed() ) split_closed( _split_idx);
  else
  {
    // copy properties
    _new_pl = *this;

    _new_pl.resize( n_vertices() - _split_idx);

    // copy vertex data
    for(unsigned int i=_split_idx; i<n_vertices(); ++i)
      _new_pl.copy_vertex_complete(*this, i, i-_split_idx);
    // copy edge data
    for(unsigned int i=_split_idx; i<n_edges(); ++i)
      _new_pl.copy_edge_complete(*this, i, i-_split_idx);

    // cut copied part
    resize(_split_idx+1);
  }
}


//-----------------------------------------------------------------------------


template <class PointT>
template <class IPoint>
bool
PolyLineT<PointT>::
plane_line_intersection( const IPoint& _p_plane,
			 const IPoint& _n_plane,
			 const IPoint& _p0,
			 const IPoint& _p1,
			 IPoint& _p_int)
{
  double a = (_n_plane | (_p_plane-_p0));
  double b = (_n_plane | (_p1-_p0));

  if( fabs(b) > 1e-9)
  {
    double s = a/b;

    if( s >= 0.0 && s<= 1.0)
    {
      _p_int = _p0 + (_p1-_p0)*s;

      // TEST Intersection Point
      if( fabs((_n_plane| (_p_int-_p_plane)) ) > 1e-9)
	std::cerr << "WARNING: wrong intersection point!!!\n";

      return true;
    }
    else
      return false;
  }
  else return false;
}


//-----------------------------------------------------------------------------


template <class PointT>
template<class MeshT>
void
PolyLineT<PointT>::
edge_points_in_segment( const MeshT& _mesh,
			const Point& _p0,
			const Point& _p1,
			const typename MeshT::FaceHandle _fh0,
			const typename MeshT::FaceHandle _fh1,
			std::vector<Point> &                     _points,
			std::vector<typename MeshT::EdgeHandle>& _ehandles )
{
  // initialize
  _points.clear();
  _ehandles.clear();

  Point p_start = _p0;
  Point p_end   = _p1;

  typename MeshT::FaceHandle fh_start = _fh0;
  typename MeshT::FaceHandle fh_end   = _fh1;

  if( fh_start == fh_end)
    return;

  // vectors for new points
  std::vector<Point> new_points0, new_points1;

  // vector for new edgehandle
  std::vector<typename MeshT::EdgeHandle> new_eh0, new_eh1;

//   // insert first point
//   new_points0.push_back( p_start);
//   new_points1.push_back( p_start);

  // construct cut plane
  // get first normal
  typename MeshT::HalfedgeHandle heh = _mesh.halfedge_handle(fh_start);
  Point p0      = (Point)_mesh.point(_mesh.to_vertex_handle( heh ));
  Point p1      = (Point)_mesh.point(_mesh.to_vertex_handle( heh = _mesh.next_halfedge_handle(heh) ));
  Point p2      = (Point)_mesh.point(_mesh.to_vertex_handle( heh = _mesh.next_halfedge_handle(heh) ));
  Point n_start = ((p1-p0)%(p2-p0)).normalize();

  // get second normal
  heh   = _mesh.halfedge_handle(fh_end);
  p0    = _mesh.point(_mesh.to_vertex_handle( heh ));
  p1    = _mesh.point(_mesh.to_vertex_handle( heh = _mesh.next_halfedge_handle(heh) ));
  p2    = _mesh.point(_mesh.to_vertex_handle( heh = _mesh.next_halfedge_handle(heh) ));
  Point n_end = ((p1-p0)%(p2-p0)).normalize();

  // get average normal
  Point n_avg = n_start + n_end;
  if( n_avg.sqrnorm() < 1e-7) n_avg = n_start - n_end;
  n_avg.normalize();

  // get plane data
  Point n_plane = n_avg % (p_start-p_end).normalize();
  if( n_plane.sqrnorm() > 1e-9) n_plane.normalize();
  else std::cerr << "WARNING: Edge Resampling -> not possible to construct stable cut plane!!!\n";
  Point p_plane = (p_start + p_end)*0.5;

  // get intersection halfedges of start triangle
  std::vector<typename MeshT::HalfedgeHandle > start_hehs;
  std::vector<Point>                          start_ps;
  heh = _mesh.halfedge_handle(fh_start);
  Point p_int;
  for(unsigned int i=0; i<3; ++i)
  {
    // edge endpoints
    p0 = (Point) _mesh.point(_mesh.from_vertex_handle(heh));
    p1 = (Point) _mesh.point(_mesh.to_vertex_handle  (heh));

    // intersection ?
    if( plane_line_intersection( p_plane, n_plane, p0, p1, p_int))
    {
      start_hehs.push_back( heh );
      start_ps.push_back(p_int);
    }
    // move to next halfedge handle
    heh = _mesh.next_halfedge_handle(heh);
  }

  // DEBUG
  // get intersection halfedges of end triangle
  std::vector<typename MeshT::HalfedgeHandle > end_hehs;
  std::vector<Point>                          end_ps;
  heh = _mesh.halfedge_handle(fh_end);
  for(unsigned int i=0; i<3; ++i)
  {
    // edge endpoints
    p0 = (Point) _mesh.point(_mesh.from_vertex_handle(heh));
    p1 = (Point) _mesh.point(_mesh.to_vertex_handle  (heh));

    // intersection ?
    if( plane_line_intersection( p_plane, n_plane, p0, p1, p_int))
    {
      end_hehs.push_back( heh );
      end_ps.push_back(p_int);
    }
    // move to next halfedge handle
    heh = _mesh.next_halfedge_handle(heh);
  }
  // END DEBUG

  // hack: debug hints
  if( start_hehs.size() != 2 || end_hehs.size() != 2)
  {
    std::cerr << "PolyLineResampling ERROR: wrong number of intersections... ";
    std::cerr << start_hehs.size() << " ";
    std::cerr << end_hehs.size()   << std::endl;
  }
  //  else std::cerr << "SUPI!!!\n";
  // end hack


  if( start_hehs.size() == 2 && end_hehs.size() == 2)
  {
    // initialize start points
    typename MeshT::HalfedgeHandle cur_heh0 = start_hehs[0];
    typename MeshT::HalfedgeHandle cur_heh1 = start_hehs[1];

    // store points and edge handles
    new_points0.push_back( start_ps[0]);
    new_eh0.push_back( _mesh.edge_handle( cur_heh0));

    new_points1.push_back( start_ps[1]);
    new_eh1.push_back( _mesh.edge_handle( cur_heh1));

    unsigned int count = 0;

    while( _mesh.face_handle(_mesh.opposite_halfedge_handle(cur_heh0)) != fh_end &&
	   _mesh.face_handle(_mesh.opposite_halfedge_handle(cur_heh1)) != fh_end &&
	   count < 1000 )
    {
      ++count;

      // move into first direction
      cur_heh0 = _mesh.opposite_halfedge_handle( cur_heh0);

      // test for boundary
      if( _mesh.is_boundary(cur_heh0))
      {
        std::cerr << "ERROR: found boundary in traversal!!!\n";
        cur_heh0 = _mesh.opposite_halfedge_handle( cur_heh0);
      } else {

        unsigned int old_size = new_points0.size();

        for(unsigned int i=0; i<2; ++i)
        {
          // move to next halfedge handle
          cur_heh0 = _mesh.next_halfedge_handle(cur_heh0);

          // edge endpoints
          p0 = (Point) _mesh.point(_mesh.from_vertex_handle(cur_heh0));
          p1 = (Point) _mesh.point(_mesh.to_vertex_handle  (cur_heh0));

          // intersection ?
          if( plane_line_intersection( p_plane, n_plane, p0, p1, p_int))
          {
            new_points0.push_back(p_int);
            new_eh0.push_back( _mesh.edge_handle( cur_heh0));
            break;
          }
        }

        // debug helper
        if( new_points0.size() != old_size + 1)
          std::cerr << "WARNING: could not find new point!!!\n";
      }

      // move into second direction
      cur_heh1 = _mesh.opposite_halfedge_handle( cur_heh1);

      // test for boundary
      if( _mesh.is_boundary(cur_heh1))
      {
        std::cerr << "ERROR: found boundary in traversal!!!\n";
        cur_heh1 = _mesh.opposite_halfedge_handle( cur_heh1);
      } else {

        unsigned int old_size = new_points1.size();

        for(unsigned int i=0; i<2; ++i)
        {
          // move to next halfedge handle
          cur_heh1 = _mesh.next_halfedge_handle(cur_heh1);

          // edge endpoints
          p0 = (Point) _mesh.point(_mesh.from_vertex_handle(cur_heh1));
          p1 = (Point) _mesh.point(_mesh.to_vertex_handle  (cur_heh1));

          // intersection ?
          if( plane_line_intersection( p_plane, n_plane, p0, p1, p_int))
          {
            new_points1.push_back(p_int);
            new_eh1.push_back( _mesh.edge_handle( cur_heh1));
            break;
          }
        }

        // debug helper
        if( new_points1.size() != old_size + 1)
          std::cerr << "WARNING: could not find new point!!!\n";
      }

    }

//     // add end points
//     new_points0.push_back( p_end );
//     new_points1.push_back( p_start );

    // set new points, test which direction converged first
    if(_mesh.face_handle(_mesh.opposite_halfedge_handle(cur_heh0)) == fh_end )
    {
      // return values
      _points   = new_points0;
      _ehandles = new_eh0;
    }
    else
      if (_mesh.face_handle(_mesh.opposite_halfedge_handle(cur_heh1)) == fh_end)
      {
        // return values
        _points   = new_points1;
        _ehandles = new_eh1;
      }
  }
}


//=============================================================================
} // namespace ACG
//=============================================================================
