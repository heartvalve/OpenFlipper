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
#include <ACG/Config/ACGDefines.hh>

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

  /** \brief Check if the polyline is marked as closed
   *
   * If the polyline is closed, the end vertices are connected.
   *
   * @return Closed polyline
   */
  bool is_closed() const         { return closed_;}

  /** \brief Set if the polyline should be closed and therefore forms a loop
   *
   * @param _c Set polyline closed or not?
   */
  void set_closed(const bool _c) { closed_ = _c;}

  ///  \brief Get number of vertices
  size_t n_vertices() const { return points_.size(); }

  ///  \brief Get number of edges
  size_t n_edges() const;

  /// \brief Clear the current polyline
  void clear();

  /// \brief Resize current polyline
  void resize( unsigned int _n);

  /// \brief Append a point to the polyline
  void add_point(const Point& _p);

  /** \brief insert _p at _idx into polyline
   *
   * @param _idx Where to insert
   * @param _p   New point
   */
  void insert_point(int _idx, const Point& _p);

  /// \brief Delete point at _idx
  void delete_point(int _idx);


  /// \brief Get a point of the polyline
  Point& point(unsigned int _i)       { return points_[_i];}

  /// \brief Get a point of the polyline
  const Point& point(unsigned int _i) const { return points_[_i];}

  /// \brief Get all points of the polyline
  std::vector<Point>& points()       { return points_;}

  /// \brief Get all points of the polyline
  const std::vector<Point>& points() const { return points_;}

  /// \brief Get first point of the polyline ( no range check!!!)
  Point& front()       { return points_[0];}

  /// \brief Get first point of the polyline ( no range check!!!)
  const Point& front() const { return points_[0];}

  /// \brief Get last point of the polyline ( no range check!!!)
  Point& back()       { return points_[n_vertices()-1];}

  /// \brief Get last point of the polyline ( no range check!!!)
  const Point& back() const { return points_[n_vertices()-1];}

  /// \brief get the i-th oriented edge vector
  Point edge_vector(unsigned int _i) const 
  { return(point((_i+1)%n_vertices())-point(_i));}

  /** \brief Compute the length of the polyline (in future cached method)
   *
   * @return Length of the polyline
   */
  Scalar length() const;

  /** \brief Subdivide polyline
   *
   * Subdivides segments until the longest interval in the line is smaller than the given value
   *
   * @param _largest largest interval length after subdivision
   */
  void subdivide( Scalar _largest  );

  /** \brief Collapse polyline
   *
   * Collapses segments until the smallest interval in the line is larger than the given value
   *
   * @param _smallest smallest interval length after collapsing
   */
  void collapse ( Scalar _smallest );

  /// \brief Invert polyline that first vertex becomes last
  void invert();

  /// \brief Append second polyline _pl to this one
  void append(const PolyLineT<PointT>& _pl);

  /// \brief Prepend second polyline _pl to polyline
  void prepend(const PolyLineT<PointT>& _pl);

  /** \brief Split closed polyline at vertex with index _split_idx
   *
   * @param _split_idx Where to split
   */
  void split_closed( unsigned int _split_idx);

  /** \brief Split closed polyline at vertex with index _split_idx
   *
   * @param _split_idx  Where to split
   * @param _new_pl     The new polyline is returned in this variable
   */
  void split( unsigned int _split_idx, PolyLineT<PointT>& _new_pl);

  /** \brief remove points which are subsequent and lie at the same position
   *
   * @param _keep_edge_vertices  Keep the corresponding edge vertices
   * @param _epsilon             Vertices which are closer than this distance are considered at same position
   */
  void remove_subsequent_identical_points( const bool _keep_edge_vertices = false, const double _epsilon = 1e-6 );

  /// \brief Laplacian smoothing
  void smooth_uniform_laplace();

  /// \brief Squared laplacian smoothing
  void smooth_uniform_laplace2();

  /// \brief Cubic laplacian smoothing
  void smooth_uniform_laplace3();


  /// \brief creating a circle with center _center and radius _radius lying in tangent plane specified by _normal
  void set_to_circle(const PointT _center, const PointT _normal, double _radius, unsigned int _n_samples = 100);

  /** \brief Project polyline points to nearest surface points (use spatial search!!!)
   *
   * \note The points of the polyline are updated in place!
   *
   * @param _mesh    The mesh to project to
   * @param _ssearch A spatial search to improve speed
   */
  template <class MeshT, class SpatialSearchT>
  void project_to_mesh( const MeshT&     _mesh,
                        SpatialSearchT*  _ssearch = 0);

  /** \brief Project polyline points to nearest surface points (use spatial search!!!)
   *
   * This function projects the polyline points to the closest of the given meshes.
   *
   * \note The points of the polyline are updated in place!
   *
   *
   * @param _mesh    vector of meshes that should be used
   * @param _ssearch A spatial search to improve speed
   */
  template <class MeshT, class SpatialSearchT>
  void project_to_mesh( const std::vector<MeshT*>&    _mesh,
			                  std::vector<SpatialSearchT*>* _ssearch = 0);

  // This Block is used to pull in extended PolyLine features such as integration of
  // a PolyLine into a Mesh. Currently this code is not included in the free version.
  #ifdef EXTENDED_POLY_LINE
    #include <libs/MeshIntegration/PolyLineIntegrationT.hh>
  #endif


  /** \brief  compute total gaussian curvature of enclosed surface (via Gauss-Bonnet)
   *
   * The enclosed surface is defined to be on the left side of the curve given
   * through N x E
   * This function requires Edge-Normals!!!
   */
  Scalar total_gaussian_curvature();

  /** \brief Conversion PolyLine <-> LineNode
   *
   * Creates a line node out of the polylline
   *
   * @param _line_node The newly created line node
   * @param _mode  0 : LineSegments mode, 1: polygon mode
   * @return
   */
  template <class LineNodeT>
  LineNodeT* get_line_node( LineNodeT*& _line_node, int _mode = 0);

  /** \brief Takes a line node and sets the points of this polyline to the points of the node
   *
   * @param _line_node The line node with the data
   * @param _mode      0 : LineSegments mode, 1: polygon mode
   */
  template <class LineNodeT>
  void set_line_node( LineNodeT*& _line_node, int _mode = 0);

  /// \brief Print information string to cerr
  void print() const;

  /// \brief Load polyline from a file
  void load( const char* _filename);

  /// \brief Save polyline to a file
  void save( const char* _filename) const;

  //  ############################### Standard Property Handling #############################

  // request properties
  void request_vertex_normals()    { request_prop( ref_count_vnormals_   , vnormals_);}
  void request_vertex_binormals()  { request_prop( ref_count_vbinormals_ , vbinormals_);}
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
  void release_vertex_binormals()  { release_prop( ref_count_vbinormals_ , vbinormals_);}
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
  bool vertex_binormals_available()  const {return bool(ref_count_vbinormals_);}
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
  
        Point& vertex_binormal(unsigned int _i)       { return vbinormals_[_i];}
  const Point& vertex_binormal(unsigned int _i) const { return vbinormals_[_i];}

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

  // ############################### SelectionWrappers  ############################
  
  bool vertex_selected(unsigned int _i) { return (_i < vselections_.size() ? vertex_selection(_i) == 1 : false); }
  bool edge_selected(unsigned int _i) { return (_i < eselections_.size() ? edge_selection(_i) == 1 : false); }
  
  void select_vertex(unsigned int _i) { if(_i < vselections_.size()) vertex_selection(_i) = 1; }
  void select_edge(unsigned int _i) { if(_i < eselections_.size()) edge_selection(_i) = 1; }
  
  void deselect_vertex(unsigned int _i) { if(_i < vselections_.size()) vertex_selection(_i) = 0; }
  void deselect_edge(unsigned int _i) { if(_i < eselections_.size()) edge_selection(_i) = 0; }


  // ############################### Helper Functions  #############################

  // copy this[_j] = _pl[_i] WITH all vertex properties
  void copy_vertex_complete(const PolyLineT<PointT>& _pl, unsigned int _i, unsigned int _j);


  // copy this[_j] = _pl[_i] WITH all edge properties
  void copy_edge_complete(const PolyLineT<PointT>& _pl, unsigned int _i, unsigned int _j);

private:

  /** Compute the closest point on a mesh
   *
   * @param _mesh     The corresponding mesh
   * @param _point    Point that should be checked
   * @param _fh       Returns closest face to the given point
   * @param _ssearch  Spatial search structure to speedup search
   * @param _dbest    Returns the distance
   * @return The closest point on the mesh (Not necessarily a vertex in the mesh!)
   */
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

  /** Intersect point with line
   *
   * @param _p_plane Plane point
   * @param _n_plane Plane normal
   * @param _p0      Startpoint of line
   * @param _p1      Endpoint of line
   * @param _p_int   Intersection point
   * @return Intersecting?
   */
  template <class IPoint>
  bool plane_line_intersection( const IPoint& _p_plane,
				const IPoint& _n_plane,
				const IPoint& _p0,
				const IPoint& _p1,
				      IPoint& _p_int);

  template<class MeshT>
  void edge_points_in_segment( const MeshT&                             _mesh,
			                         const Point&                             _p0,
			                         const Point&                             _p1,
			                         const typename MeshT::FaceHandle         _fh0,
			                         const typename MeshT::FaceHandle         _fh1,
			                         std::vector<Point> &                     _points,
			                         std::vector<typename MeshT::EdgeHandle>& _ehandles );


private:

  /// \brief List of points in the polyline
  std::vector<Point> points_;

  /// \brief Connect first and last point?
  bool closed_;

  // ############################### Standard Property Handling #############################

  // list of vertex properties
  std::vector<Point>  vnormals_;
  std::vector<Point>  vbinormals_;
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
  unsigned int ref_count_vbinormals_;
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

#ifdef EXTENDED_POLY_LINE
  #if defined(INCLUDE_TEMPLATES) && !defined(ACG_POLYLINE_INTEGRATIONT_C)
  #define ACG_POLYLINE_INTEGRATIONT_TEMPLATES
  #include <libs/MeshIntegration/PolyLineIntegrationT.cc>
  #endif
#endif


//=============================================================================
#endif // ACG_POLYLINET_HH defined
//=============================================================================

