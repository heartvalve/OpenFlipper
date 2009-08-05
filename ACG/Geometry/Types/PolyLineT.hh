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
// CLASS PolyLineT
//
// Author:  David Bommes <bommes@cs.rwth-aachen.de>
//
//=============================================================================


#ifndef ACG_POLYLINET_HH
#define ACG_POLYLINET_HH


//== INCLUDES =================================================================
#include <vector>
#include <iostream>
#include "../../Config/ACGDefines.hh"

//== FORWARDDECLARATIONS ======================================================

//== NAMESPACES ===============================================================

namespace ACG {

//== CLASS DEFINITION =========================================================




/** \class PolyLineT PolyLineT.hh <ACG/.../PolyLineT.hh>

    Brief Description.

    A more elaborate description follows.
*/

template <class PointT>
class PolyLineT
{
public:

  // internal relevant Types
  typedef PointT                      Point;
  typedef typename Point::value_type  Scalar;

  /// Constructor
  PolyLineT(bool _closed = false );

  /// Copy Constructor
  PolyLineT(const PolyLineT& _line);

  /// Destructor
  ~PolyLineT() {}

  // acces functions for closed_
  bool is_closed() const         { return closed_;}
  void set_closed(const bool _c) { closed_ = _c;}

  // get number of vertices
  unsigned int n_vertices() const { return points_.size(); }

  // get number of edges
  unsigned int n_edges() const { if( n_vertices() == 0) return 0;
                                 else return points_.size()-1 + (unsigned int)closed_; }

  // clear the current polyline
  void clear();

  // resize current polyline
  void resize( unsigned int _n);

  // add a point
  void add_point(const Point& _p);

  // get a point of the polyline
        Point& point(unsigned int _i)       { return points_[_i];}
  const Point& point(unsigned int _i) const { return points_[_i];}

  // get the points of the polyline
        std::vector<Point>& points()       { return points_;}
  const std::vector<Point>& points() const { return points_;}

  // get first point of the polyline ( no range check!!!)
        Point& front()       { return points_[0];}
  const Point& front() const { return points_[0];}
  // get last point of the polyline ( no range check!!!)
        Point& back()       { return points_[n_vertices()-1];}
  const Point& back() const { return points_[n_vertices()-1];}

  // TODO
  // void push_back (const Point& _p)
  // void push_front(const Point& _p)
  // void pop_back (const Point& _p)
  // void pop_front(const Point& _p)

  // compute the length of the polyline (in future cached method)
  Scalar length() const;


  // controle interval lengths
  void subdivide( Scalar _largest  );
  void collapse ( Scalar _smallest );

  void subdivide_old( Scalar _largest  );
  void collapse_old ( Scalar _smallest );

  // invert polyline that first vertex becomes last
  void invert();

  // append _pl to polyline
  void append(const PolyLineT<PointT>& _pl);

  // prepend _pl to polyline
  void prepend(const PolyLineT<PointT>& _pl);

  // split polyline at vertex with index _split_idx
  void split_closed( unsigned int _split_idx);

  // split polyline at vertex with index _split_idx
  void split( unsigned int _split_idx, PolyLineT<PointT>& _new_pl);

  // smooth polyline
  void smooth_uniform_laplace();
  void smooth_uniform_laplace2();
  void smooth_uniform_laplace3();

  // project polyline points to nearest surface points (use spatial search!!!)
  template <class MeshT, class SpatialSearchT>
  void project_to_mesh( const MeshT& _mesh, SpatialSearchT * _ssearch = 0);

  // project polyline points to nearest surface points (use spatial search!!!)
  template <class MeshT, class SpatialSearchT>
  void project_to_mesh( const std::vector<MeshT*> _mesh,
			std::vector<SpatialSearchT*>* _ssearch = 0);

  // project polyline points to nearest surface points (use spatial search!!!)
  template <class MeshT, class SpatialSearchT>
  void resample_on_mesh_edges( MeshT& _mesh, SpatialSearchT * _ssearch = 0);

  // conversion methods PolyLine <-> LineNode
  template <class LineNodeT>
  LineNodeT* get_line_node( LineNodeT*& _line_node, int _mode = 0);

  template <class LineNodeT>
  void set_line_node( LineNodeT*& _line_node, int _mode = 0);

  // print information string
  void print() const;

  // storage
  void load( const char* _filename);
  void save( const char* _filename) const;

  // ############################### Standard Property Handling #############################

  // request properties
  void request_vertex_normals()    { request_prop( ref_count_vnormals_   , vnormals_);}
  void request_vertex_colors()     { request_prop( ref_count_vcolors_    , vcolors_ );}
  void request_vertex_scalars()    { request_prop( ref_count_vscalars_   , vscalars_);}
  void request_vertex_selections() { request_prop( ref_count_vselections_, vselections_);}
  void request_vertex_vhandles()   { request_prop( ref_count_vvhandles_  , vvhandles_);}
  void request_vertex_ehandles()   { request_prop( ref_count_vehandles_  , vehandles_);}
  void request_vertex_fhandles()   { request_prop( ref_count_vfhandles_  , vfhandles_);}

  void request_edge_normals()      { request_prop( ref_count_enormals_   , enormals_);}
  void request_edge_colors()       { request_prop( ref_count_ecolors_    , ecolors_ );}
  void request_edge_scalars()      { request_prop( ref_count_escalars_   , escalars_);}
  void request_edge_selections()   { request_prop( ref_count_eselections_, eselections_);}

  // release properties
  void release_vertex_normals()    { release_prop( ref_count_vnormals_   , vnormals_);}
  void release_vertex_colors()     { release_prop( ref_count_vcolors_    , vcolors_ );}
  void release_vertex_scalars()    { release_prop( ref_count_vscalars_   , vscalars_);}
  void release_vertex_selections() { release_prop( ref_count_vselections_, vselections_);}
  void release_vertex_vhandles()   { release_prop( ref_count_vvhandles_  , vvhandles_);}
  void release_vertex_ehandles()   { release_prop( ref_count_vehandles_  , vehandles_);}
  void release_vertex_fhandles()   { release_prop( ref_count_vfhandles_  , vfhandles_);}

  void release_edge_normals()      { release_prop( ref_count_enormals_   , enormals_);}
  void release_edge_colors()       { release_prop( ref_count_ecolors_    , ecolors_ );}
  void release_edge_scalars()      { release_prop( ref_count_escalars_   , escalars_);}
  void release_edge_selections()   { release_prop( ref_count_eselections_, eselections_);}

  // property availability
  bool vertex_normals_available()    const {return bool(ref_count_vnormals_);}
  bool vertex_colors_available()     const {return bool(ref_count_vcolors_);}
  bool vertex_scalars_available()    const {return bool(ref_count_vscalars_);}
  bool vertex_selections_available() const {return bool(ref_count_vselections_);}
  bool vertex_vhandles_available() const {return bool(ref_count_vvhandles_);}
  bool vertex_ehandles_available() const {return bool(ref_count_vehandles_);}
  bool vertex_fhandles_available() const {return bool(ref_count_vfhandles_);}

  bool edge_normals_available()    const {return bool(ref_count_enormals_);}
  bool edge_colors_available()     const {return bool(ref_count_ecolors_);}
  bool edge_scalars_available()    const {return bool(ref_count_escalars_);}
  bool edge_selections_available() const {return bool(ref_count_eselections_);}

  // property access ( no range or availability check! )
        Point& vertex_normal(unsigned int _i)       { return vnormals_[_i];}
  const Point& vertex_normal(unsigned int _i) const { return vnormals_[_i];}

        Point & vertex_color(unsigned int _i)       { return vcolors_[_i];}
  const Point & vertex_color(unsigned int _i) const { return vcolors_[_i];}

        Scalar& vertex_scalar(unsigned int _i)       { return vscalars_[_i];}
  const Scalar& vertex_scalar(unsigned int _i) const { return vscalars_[_i];}

        unsigned char& vertex_selection(unsigned int _i)       {return vselections_[_i];}
  const unsigned char& vertex_selection(unsigned int _i) const {return vselections_[_i];}

        int& vertex_vhandle(unsigned int _i)       {return vvhandles_[_i];}
  const int& vertex_vhandle(unsigned int _i) const {return vvhandles_[_i];}

        int& vertex_ehandle(unsigned int _i)       {return vehandles_[_i];}
  const int& vertex_ehandle(unsigned int _i) const {return vehandles_[_i];}

        int& vertex_fhandle(unsigned int _i)       {return vfhandles_[_i];}
  const int& vertex_fhandle(unsigned int _i) const {return vfhandles_[_i];}

        Point& edge_normal(unsigned int _i)       { return enormals_[_i];}
  const Point& edge_normal(unsigned int _i) const { return enormals_[_i];}

        Point & edge_color(unsigned int _i)       { return ecolors_[_i];}
  const Point & edge_color(unsigned int _i) const { return ecolors_[_i];}

        Scalar& edge_scalar(unsigned int _i)       { return escalars_[_i];}
  const Scalar& edge_scalar(unsigned int _i) const { return escalars_[_i];}

        unsigned char& edge_selection(unsigned int _i)       {return eselections_[_i];}
  const unsigned char& edge_selection(unsigned int _i) const {return eselections_[_i];}


  // ############################### Helper Functions  #############################

  // copy this[_j] = _pl[_i] WITH all vertex properties
  void copy_vertex_complete(const PolyLineT<PointT>& _pl, unsigned int _i, unsigned int _j);


  // copy this[_j] = _pl[_i] WITH all edge properties
  void copy_edge_complete(const PolyLineT<PointT>& _pl, unsigned int _i, unsigned int _j);

private:

  template <class MeshT, class SpatialSearchT>
  Point find_nearest_point( const MeshT&                _mesh,
			    const Point&                _point,
			    typename MeshT::FaceHandle &_fh,
			    SpatialSearchT *            _ssearch = 0,
			    double*                     _dbest   = 0);


  template <class PropT>
  void request_prop( unsigned int& _ref_count, PropT& _prop);
  template <class PropT>
  void release_prop( unsigned int& _ref_count, PropT& _prop);

  template <class IPoint>
  bool plane_line_intersection( const IPoint& _p_plane,
				const IPoint& _n_plane,
				const IPoint& _p0,
				const IPoint& _p1,
				      IPoint& _p_int);

  template<class MeshT>
  void edge_points_in_segment( const MeshT& _mesh,
			       const Point& _p0,
			       const Point& _p1,
			       const typename MeshT::FaceHandle _fh0,
			       const typename MeshT::FaceHandle _fh1,
			       std::vector<Point> &                     _points,
			       std::vector<typename MeshT::EdgeHandle>& _ehandles );


private:

  // list of points
  std::vector<Point> points_;

  // connect first and last point?
  bool closed_;

  // ############################### Standard Property Handling #############################

  // list of vertex properties
  std::vector<Point>  vnormals_;
  std::vector<Point>  vcolors_;
  std::vector<Scalar> vscalars_;
  std::vector<unsigned char>   vselections_;
  std::vector<int>    vvhandles_;
  std::vector<int>    vehandles_;
  std::vector<int>    vfhandles_;

  std::vector<Point>  enormals_;
  std::vector<Point>  ecolors_;
  std::vector<Scalar> escalars_;
  std::vector<unsigned char>   eselections_;


  // property reference counter
  unsigned int ref_count_vnormals_;
  unsigned int ref_count_vcolors_;
  unsigned int ref_count_vscalars_;
  unsigned int ref_count_vselections_;
  unsigned int ref_count_vvhandles_;
  unsigned int ref_count_vehandles_;
  unsigned int ref_count_vfhandles_;

  unsigned int ref_count_enormals_;
  unsigned int ref_count_ecolors_;
  unsigned int ref_count_escalars_;
  unsigned int ref_count_eselections_;
};


//=============================================================================
} // namespace ACG
//=============================================================================
#if defined(INCLUDE_TEMPLATES) && !defined(ACG_POLYLINET_C)
#define ACG_POLYLINET_TEMPLATES
#include "PolyLineT.cc"
#endif
//=============================================================================
#endif // ACG_POLYLINET_HH defined
//=============================================================================

