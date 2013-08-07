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

#include <cfloat>
#include <ACG/Geometry/Algorithms.hh>

#ifndef WIN32
#include <cstdlib>
#endif

#ifdef USE_OPENMP
#include <omp.h>
#endif

//== NAMESPACES ===============================================================

namespace ACG {

//== IMPLEMENTATION ==========================================================

//-----------------------------------------------------------------------------


/// Constructor
template <class PointT>
PolyLineT<PointT>::
  PolyLineT( bool _closed )
    : closed_(_closed),
      ref_count_vnormals_(0),
      ref_count_vbinormals_(0),
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
  vbinormals_  = _line.vbinormals_;
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
  ref_count_vbinormals_  = _line.ref_count_vbinormals_;
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
  vbinormals_.clear();
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
size_t
PolyLineT<PointT>::n_edges() const
{
  if (n_vertices() == 0)
    return 0;
  else
    return points_.size() - 1 + (unsigned int) closed_;
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
    if( vertex_binormals_available() )
      vbinormals_.resize( _n);
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
  
  if( vertex_binormals_available() )
    vbinormals_.push_back( Point(0,0,0));

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
  
  if( vertex_binormals_available() )
    vbinormals_.insert(vbinormals_.begin()+_idx, Point(0,0,0));

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
  
  if( vertex_binormals_available() )
    vbinormals_.erase(vbinormals_.begin()+_idx);

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

  if(!closed_)
  {
    for(unsigned int i=0; i<n-1; ++i)
    {
      l += (points_[(i+1)]-points_[i]).norm();
    }
  }
  else
  {
    for(unsigned int i=0; i<n; ++i)
    {
      l += (points_[(i+1)%n]-points_[i]).norm();
    }
  }

  return l;
}

//-----------------------------------------------------------------------------


template <class PointT>
void
PolyLineT<PointT>::
subdivide(Scalar _largest)
{
  // check validity
  if (!n_vertices())
    return;

  unsigned int n_subdivisions = 1;

  while (n_subdivisions != 0) {

    n_subdivisions = 0;

    // add new polyline and add first point
    PolyLineT<PointT> new_pl = *this;
    new_pl.resize(1);
    new_pl.copy_vertex_complete(*this, 0, 0);

    // squared maximal length
    Scalar l2 = _largest * _largest;

    for (unsigned int i = 1; i < points_.size(); ++i) {
      if ((new_pl.point(new_pl.n_vertices() - 1) - points_[i]).sqrnorm() > l2) {
        Point mid_point = (new_pl.point(new_pl.n_vertices() - 1) + points_[i]) * 0.5;

        new_pl.add_point(mid_point);
        ++n_subdivisions;
      }

      // copy vertex
      new_pl.resize(new_pl.n_vertices() + 1);
      new_pl.copy_vertex_complete(*this, i, new_pl.n_vertices() - 1);
    }

    // last interval for closed polyline
    if (closed_) {
      if ((new_pl.point(new_pl.n_vertices() - 1) - points_[0]).sqrnorm() > l2) {
        Point mid_point = (new_pl.point(new_pl.n_vertices() - 1) + points_[0]) * 0.5;
        new_pl.add_point(mid_point);
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
void
PolyLineT<PointT>::
set_to_circle(const PointT _center, const PointT _normal, double _radius, unsigned int _n_samples)
{
  this->clear();
  this->set_closed(true);

  // get local basis vectors
  PointT n = _normal; n.normalize();
  PointT u = ACG::Geometry::perpendicular(_normal); u*=_radius/u.norm();
  PointT v = n % u;

  for(unsigned int i=0; i<_n_samples; ++i)
  {
    double alpha = double(i)*2.0*M_PI/double(_n_samples);

    this->add_point(_center + u*cos(alpha) + v*sin(alpha));
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

  typename MeshT::Point p_best = _mesh.point(_mesh.vertex_handle(0));
  typename MeshT::Scalar d_best = (p0 - p_best).sqrnorm();

  typename MeshT::FaceHandle fh_best;

  if (_ssearch == 0) {
    // exhaustive search
    typename MeshT::ConstFaceIter cf_it = _mesh.faces_begin();
    typename MeshT::ConstFaceIter cf_end = _mesh.faces_end();

    for (; cf_it != cf_end; ++cf_it) {
      typename MeshT::ConstFaceVertexIter cfv_it = _mesh.cfv_iter(*cf_it);

      const typename MeshT::Point& pt0 = _mesh.point(*cfv_it);
      const typename MeshT::Point& pt1 = _mesh.point(*(++cfv_it));
      const typename MeshT::Point& pt2 = _mesh.point(*(++cfv_it));

      typename MeshT::Point ptn;

      typename MeshT::Scalar d = Geometry::distPointTriangleSquared(p0, pt0, pt1, pt2, ptn);

      if (d < d_best) {
        d_best = d;
        p_best = ptn;

        fh_best = *cf_it;
      }
    }

    // return face handle
    _fh = fh_best;

    // return distance
    if (_dbest)
      *_dbest = sqrt(d_best);

    return (Point) p_best;
  } else {
    typename MeshT::FaceHandle fh = _ssearch->nearest(p0).handle;
    typename MeshT::CFVIter fv_it = _mesh.cfv_iter(fh);

    const typename MeshT::Point& pt0 = _mesh.point(*fv_it);
    const typename MeshT::Point& pt1 = _mesh.point(*(++fv_it));
    const typename MeshT::Point& pt2 = _mesh.point(*(++fv_it));

    // project
    d_best = Geometry::distPointTriangleSquared(p0, pt0, pt1, pt2, p_best);

    // return facehandle
    _fh = fh;

    // return distance
    if (_dbest)
      *_dbest = sqrt(d_best);

    return (Point) p_best;
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

  if (_mode == 0) {
    // LineSegmentsMode

    // create LineNode
    _line_node = new LineNodeT(LineNodeT::LineSegmentsMode, 0, "PolyLine");
    _line_node->set_line_width(5.0);
    //   _line_node->set_base_color(Vec4f(0,1,0,0));
    _line_node->set_base_color(
        OpenMesh::Vec4f(0.2 + double(rand()) / double(RAND_MAX) * 0.8, 0.2 + double(rand()) / double(RAND_MAX) * 0.8,
            0.2 + double(rand()) / double(RAND_MAX) * 0.8, 1.0));

    _line_node->show();

    // add line node
    for (unsigned int i = 0; i < this->points().size() - 1; ++i) {
      _line_node->add_line((Vec3fL) this->points()[i], (Vec3fL) this->points()[i + 1]);
    }

    // close loop
    if (closed_)
      if (!this->points().empty()) {
        _line_node->add_point((Vec3fL) this->points()[0]);
      }

    return _line_node;
  } else {
    // create LineNode
    _line_node = new LineNodeT(LineNodeT::PolygonMode, 0, "PolyLine");
    _line_node->set_line_width(5.0);
    //   _line_node->set_base_color(Vec4f(0,1,0,0));
    _line_node->set_base_color(
        OpenMesh::Vec4f(0.2 + double(rand()) / double(RAND_MAX) * 0.8, 0.2 + double(rand()) / double(RAND_MAX) * 0.8,
            0.2 + double(rand()) / double(RAND_MAX) * 0.8, 1.0));

    _line_node->show();

    // add line node
    for (unsigned int i = 0; i < this->points().size(); ++i) {
      _line_node->add_point((Vec3fL) this->points()[i]);
    }

    // close loop
    if (closed_)
      if (!this->points().empty()) {
        _line_node->add_point((Vec3fL) this->points()[0]);
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

  if (_mode == 0) {
    // assume LineSegmentsMode

    const typename LineNodeT::PointVector& ln_points = _line_node->points();

    for (unsigned int i = 0; i < ln_points.size();) {
      if (i != ln_points.size() - 1)
        add_point((Point) ln_points[i]);
      else {
        // last point
        if ((ln_points[ln_points.size() - 1] - ln_points[0]).sqrnorm() == 0) {
          closed_ = true;
        } else {
          closed_ = false;
          add_point((Point) ln_points[i]);
        }
      }

      // increase counter
      if (i == 0)
        i += 1;
      else
        i += 2;
    }
  } else {
    // assume PolygonMode
    closed_ = true;

    const typename LineNodeT::PointVector& ln_points(_line_node->points());

    for (unsigned int i = 0; i < ln_points.size(); ++i) {
      add_point((Point) ln_points[i]);
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
  while(true)
  {
    token = "-";
    fin >> token;
    if(token == "VVHANDLES")
    {
	if(!vertex_vhandles_available()) request_vertex_vhandles();
	for(unsigned int i=0; i<n_vertices(); ++i)
	  fin >> vertex_vhandle(i);
    }
    else if(token == "VEHANDLES")
    {
	if(!vertex_ehandles_available()) request_vertex_ehandles(); 
	for(unsigned int i=0; i<n_vertices(); ++i)
	  fin >> vertex_ehandle(i);
    }
    else if(token == "VFHANDLES")
    {
	if(!vertex_fhandles_available()) request_vertex_fhandles();
	for(unsigned int i=0; i<n_vertices(); ++i)
	  fin >> vertex_fhandle(i);
    }
    else if(token == "VNORMALS")
    {
	if(!vertex_normals_available()) request_vertex_normals();
	for(unsigned int i=0; i<n_vertices(); ++i)
	{
	  fin >> vertex_normal(i)[0];
	  fin >> vertex_normal(i)[1];
	  fin >> vertex_normal(i)[2];
	}
    }
    else if(token == "VBINORMALS")
    {
	if(!vertex_binormals_available()) request_vertex_binormals();
	for(unsigned int i=0; i<n_vertices(); ++i)
	{
	  fin >> vertex_binormal(i)[0];
	  fin >> vertex_binormal(i)[1];
	  fin >> vertex_binormal(i)[2];
	}
    }
    else if(token != "") break; //eat up empty lines
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
      fout << vertex_ehandle(i) << std::endl;
  }

  if(vertex_fhandles_available())
  {
    fout << "VFHANDLES" << std::endl;
    for( unsigned int i=0; i<n_vertices(); ++i)
      fout << vertex_fhandle(i) << std::endl;
  }
  
  if(vertex_normals_available())
  {
    fout << "VNORMALS" << std::endl;
    for( unsigned int i=0; i<n_vertices(); ++i)
    {
      fout << vnormals_[i][0] << " ";
      fout << vnormals_[i][1] << " ";
      fout << vnormals_[i][2] << std::endl;
    }
  }
  
  if(vertex_binormals_available())
  {
    fout << "VBINORMALS" << std::endl;
    for( unsigned int i=0; i<n_vertices(); ++i)
    {
      fout << vbinormals_[i][0] << " ";
      fout << vbinormals_[i][1] << " ";
      fout << vbinormals_[i][2] << std::endl;
    }
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
    
  if( _pl.vertex_binormals_available())
    if(   vertex_binormals_available())
      vertex_binormal(_j) = _pl.vertex_binormal(_i);

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
  double a = (_n_plane | (_p_plane - _p0));
  double b = (_n_plane | (_p1 - _p0));

  if (fabs(b) > 1e-9) {
    double s = a / b;

    if (s >= 0.0 && s <= 1.0) {
      _p_int = _p0 + (_p1 - _p0) * s;

      // TEST Intersection Point
      if (fabs((_n_plane | (_p_int - _p_plane))) > 1e-9)
        std::cerr << "WARNING: wrong intersection point!!!\n";

      return true;
    } else
      return false;
  } else
    return false;
}


//-----------------------------------------------------------------------------


template <class PointT>
template<class MeshT>
void
PolyLineT<PointT>::
edge_points_in_segment( const MeshT&                             _mesh,
		                  	const Point&                             _p0,
			                  const Point&                             _p1,
			                  const typename MeshT::FaceHandle         _fh0,
			                  const typename MeshT::FaceHandle         _fh1,
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


//-----------------------------------------------------------------------------


template <class PointT>
void
PolyLineT<PointT>::
remove_subsequent_identical_points(const bool _keep_edge_vertices, const double _epsilon )
{
	if(is_closed())
	{
		for( int i=0; i<int(n_vertices()); ++i)
		{
			int i_next = (i+1) % int(n_vertices());

			if( (point(i) - point(i_next)).norm() < _epsilon)
			{
				if( vertex_ehandles_available())
				{
					if( !_keep_edge_vertices || vertex_ehandle(i_next) == -1)
					{
						delete_point(i_next);
						--i;
					}
					else
						if( vertex_ehandle(i) == -1)
						{
							delete_point(i);
							--i;
						}
				}
				else // simply delete i_next
				{
					delete_point(i_next);
					--i;
				}
			}
		}
	}
	else
	{
		for( int i=0; i<int(n_vertices())-1; ++i)
		{
			int i_next = (i+1) % int(n_vertices());

			if( (point(i) - point(i_next)).norm() < _epsilon)
			{
				if( vertex_ehandles_available())
				{
					if( !_keep_edge_vertices || vertex_ehandle(i_next) == -1)
					{
						if(i_next != int(n_vertices())-1)
						{
							delete_point(i_next);
							--i;
						}
						else delete_point(i);
					}
					else
						if( vertex_ehandle(i) == -1)
						{
							if(i != 0)
							{
								delete_point(i);
								--i;
							}
						}
				}
				else // simply delete i_next
				{
					if(i_next != int(n_vertices())-1)
					{
						delete_point(i_next);
						--i;
					}
					else delete_point(i);
				}
			}
		}
	}
}


//-----------------------------------------------------------------------------


template <class PointT>
typename PolyLineT<PointT>::Scalar
PolyLineT<PointT>::
total_gaussian_curvature()
{
  // total gaussian curvature
  Scalar gcurv = 0.0;

  if (!edge_normals_available() || !is_closed()) {
    std::cerr << "Warning: PolyLineT::total_gaussian_curvature requires a closed loop and edge normals!!!\n";
  } else // closed and edge normals available
  {
    // sum over boundary
    for (unsigned int i = 0; i < n_edges(); ++i) {
      // get two consecutive edge vectors and corresponding normals
      Point e0 = edge_vector(i);
      Point n0 = edge_normal(i);
      Point e1 = edge_vector((i + 1) % n_edges());
      Point n1 = edge_normal((i + 1) % n_edges());

      // normalize edges
      if (e0.norm() > 1e-8)
        e0.normalize();
      else {
        std::cerr << "Warning: e0 is undefined...\n";
        continue;
      }
      if (e1.norm() > 1e-8)
        e1.normalize();
      else {
        std::cerr << "Warning: e1 is undefined...\n";
        continue;
      }

      // normalize normals
      if (n0.norm() > 1e-4)
        n0.normalize();
      else {
        std::cerr << "Warning: n0 is undefined...\n";
        continue;
      }
      if (n1.norm() > 1e-4)
        n1.normalize();
      else {
        std::cerr << "Warning: n1 is undefined...\n";
        continue;
      }

      // compute binormals
      Point b0 = n0 % e0;
      // Point b1 = n1%e0;

      // normalize binormals
      if (b0.norm() > 1e-8)
        b0.normalize();
      else {
        std::cerr << "Warning: b0 is undefined...\n";
        continue;
      }
      // if( b1.norm() > 1e-4) b1.normalize();
      // else std::cerr << "Warning: b1 is undefined...\n";

      // compute intersection of tangent planes
      Point d = n0 % n1;
      if ((d | b0) < 0) // reorientation necessary?
        d *= -1.0;

      // both in same tangent plane?
      if (d.norm() < 1e-3)
        d = b0;
      else
        d.normalize();

      // add both angles
      double dp = (-e0 | d);
      dp = std::max(-1.0, dp);
      dp = std::min(1.0, dp);

      // compute first boundary angle
      double boundary_angle = acos(dp);

      dp = (e1 | d);
      dp = std::max(-1.0, dp);
      dp = std::min(1.0, dp);

      // add second angle
      boundary_angle += acos(dp);

      gcurv += M_PI - boundary_angle;
    }
  }
  // return total gaussian curvature
  return (2.0 * M_PI - gcurv);
}

//=============================================================================
} // namespace ACG
//=============================================================================
