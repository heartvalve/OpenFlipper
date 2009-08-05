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
//  CLASS MeshEdgeSamplerT
//
//=============================================================================


#ifndef ACG_MESHEDGESAMPLERT_HH
#define ACG_MESHEDGESAMPLERT_HH


//== INCLUDES =================================================================

#include <iostream>
#include <vector>
#include "../../Config/ACGDefines.hh"

//== FORWARDDECLARATIONS ======================================================

//== NAMESPACES ===============================================================

namespace ACG {

//== CLASS DEFINITION =========================================================




/** \class MeshEdgeSamplerT MeshEdgeSamplerT.hh <ACG/.../MeshEdgeSamplerT.hh>

    Brief Description.

    A more elaborate description follows.
*/

template <class MeshT, class PointT>
class MeshEdgeSamplerT
{
public:

  // typedefs
  typedef MeshT                         Mesh;
  typedef typename Mesh::VertexHandle   VH;
  typedef typename Mesh::FaceHandle     FH;
  typedef typename Mesh::EdgeHandle     EH;
  typedef typename Mesh::HalfedgeHandle HEH;
  typedef PointT                        Point;


  /// Constructor
  MeshEdgeSamplerT( Mesh& _mesh) : mesh_(_mesh){}

  /// Destructor
  ~MeshEdgeSamplerT() {}


  // compute points on _mesh edges on intersection plane spanned by
  // (_p_start - _p_end) and (normal(_fh_start) + normal(_fh_end)
  // Finally the shorter possibility of the closed cut is chosen
  void edge_points_in_segment( const Point&        _p_start,
			       const Point&        _p_end,
			       const FH            _fh_start,
			       const FH            _fh_end,
			       std::vector<Point>& _points,
			       std::vector<EH>&    _ehandles );
protected:

  // compute plane spanned by(_p_start - _p_end) and
  // (normal(_fh_start) + normal(_fh_end)
  void compute_plane( const Point&        _p_start,
		      const Point&        _p_end,
		      const FH            _fh_start,
		      const FH            _fh_end);

  // get two start HaledgeHandles
  void get_start_hehs( const Point& /*_p_start*/ ,
		       const FH    _fh_start,
		             HEH&  _sheh0,
		             HEH&  _sheh1 );

  void get_end_fhs_set( const Point&     _p_end,
			const FH         _fh_end,
			std::vector<FH>& _endfhs);



  // intersect line _p0,_p1 with stored plane and return intersection point _p_int
  // and distance != 0.0 if the intersection point is not in range [_p0, _p1]
  double plane_line_intersection( const Point& _p0,
				  const Point& _p1,
				  Point& _p_int);

  double plane_line_intersection( const HEH _heh,
				  Point&    _p_int);

  bool line_in_plane( const Point& _p0,
		      const Point& _p1,
		      double       _eps);

  bool line_in_plane( const HEH& _heh );

  double plane_dist( const Point& _p0 );

  double line_line_dist(const Point& _p_start,
			const Point& _p_end,
			const HEH&   _heh,
			      Point& _p_best);

  HEH get_next_edge_intersection( const HEH& _heh, const Point& _pold, Point& _pint);


  void sort_intersections( HEH& _heh0, double& _d0, Point& _ip0,
			   HEH& _heh1, double& _d1, Point& _ip1,
			   HEH& _heh2, double& _d2, Point& _ip2 );

  double epsilon( const FH& _fh);
  double epsilon( const HEH& _heh);

  void finish_list( std::vector<Point>& _points,
		    std::vector<EH>&    _ehs,
		    const HEH           _cur_heh,
		    const FH            _fh_end,
		    const Point&        _p_end,
		    const int           _type);

  void finish_list_old( std::vector<Point>& _points,
			std::vector<EH>&    _ehs,
			const HEH           _cur_heh,
			const FH            _fh_end,
			const Point&        _p_end,
			const int           _t);


  void circulate_CW(HEH _heh, FH _fh_end, std::vector<EH>& _cw_ehs);

  void circulate_CCW(HEH _heh, FH _fh_end, std::vector<EH>& _ccw_ehs);

private:

  // reference to triangle mesh
  Mesh& mesh_;

  // plane
  Point p_plane_;
  Point n_plane_;
};


//=============================================================================
} // namespace ACG
//=============================================================================
#if defined(INCLUDE_TEMPLATES) && !defined(ACG_MESHEDGESAMPLERT_C)
#define ACG_MESHEDGESAMPLERT_TEMPLATES
#include "MeshEdgeSamplerT.cc"
#endif
//=============================================================================
#endif // ACG_MESHEDGESAMPLERT_HH defined
//=============================================================================

