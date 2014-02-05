/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2014 by Computer Graphics Group, RWTH Aachen       *
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
*   $Date$                     *
*                                                                            *
\*===========================================================================*/




//=============================================================================
//
//  IMPLEMENTATION
//
//=============================================================================

#define CURVATURE_C

//== INCLUDES =================================================================

#include <ACG/Geometry/Algorithms.hh>
#include "Math_Tools/Math_Tools.hh"

#include <iostream>
#include <OpenMesh/Core/Geometry/MathDefs.hh>

#include <cmath>

//== NAMESPACES ===============================================================

namespace curvature {

//== IMPLEMENTATION ==========================================================

/*! compute consistent discrete gaussian curvature (vertex is a small sphere patch, edges are small cylinders)
*/
template< typename MeshT >
double
gauss_curvature(MeshT& _mesh, const typename MeshT::VertexHandle& _vh) {
   if (_mesh.status(_vh).deleted())
     return 0.0;
  
  double gauss_curv = 2.0 * M_PI;

  /*

  TODO: Check the boundary case.

  If the vertex is a boundary vertex
  if ( _mesh.is_boundary(_vh) )
    gauss_curv = M_PI;

  */

  const typename MeshT::Point p0 = _mesh.point(_vh);

  typename MeshT::CVOHIter voh_it( _mesh.cvoh_iter(_vh));
  typename MeshT::CVOHIter n_voh_it = voh_it;

  if ( ! voh_it->is_valid() )
     return 0.0;
   
  // move to next
  ++n_voh_it;

  for(; voh_it.is_valid(); ++voh_it, ++n_voh_it)
  {
    typename MeshT::Point p1 = _mesh.point(_mesh.to_vertex_handle(   *voh_it));
    typename MeshT::Point p2 = _mesh.point(_mesh.to_vertex_handle( *n_voh_it));

    gauss_curv -= acos(OpenMesh::sane_aarg( ((p1-p0).normalize() | (p2-p0).normalize()) ));
  }

  return gauss_curv;
}


template<class MeshT, class VectorT, class REALT>
void discrete_mean_curv_op( const MeshT&                        _m,
                        	  const typename MeshT::VertexHandle& _vh,
			                      VectorT&                            _n,
			                      REALT&                              _area )
{
  _n = VectorT(0,0,0);
  _area = 0.0;

  typename MeshT::ConstVertexOHalfedgeIter voh_it = _m.cvoh_iter(_vh);
  
   if ( ! voh_it->is_valid() )
     return;
   
  for(; voh_it.is_valid(); ++voh_it)
  {
    if ( _m.is_boundary( _m.edge_handle( *voh_it ) ) )
      continue;
    
    const typename MeshT::Point p0 = _m.point( _vh );
    const typename MeshT::Point p1 = _m.point( _m.to_vertex_handle( *voh_it));
//     const typename MeshT::Point p2 = _m.point( _m.to_vertex_handle( _m.next_halfedge_handle( voh_it.handle())));
    const typename MeshT::Point p2 = _m.point( _m.from_vertex_handle( _m.prev_halfedge_handle( *voh_it)));
    const typename MeshT::Point p3 = _m.point( _m.to_vertex_handle( _m.next_halfedge_handle( _m.opposite_halfedge_handle(*voh_it))));

    const REALT alpha = acos( OpenMesh::sane_aarg((p0-p2).normalize() | (p1-p2).normalize()) );
    const REALT beta  = acos( OpenMesh::sane_aarg((p0-p3).normalize() | (p1-p3).normalize()) );
      
    REALT cotw = 0.0;
    
    if ( !OpenMesh::is_eq(alpha,M_PI/2.0) )
      cotw += (REALT(1.0))/tan(alpha);
    
    if ( !OpenMesh::is_eq(beta,M_PI/2.0) )
      cotw += (REALT(1.0))/tan(beta);

#ifdef WIN32
	if ( OpenMesh::is_zero(cotw) )
		continue;
#else
    if ( OpenMesh::is_zero(cotw) || std::isinf(cotw) )
		continue;
#endif

    // calculate area
    const int obt = ACG::Geometry::isObtuse(p0,p1,p2);
    if(obt == 0)
    {
      REALT gamma = acos( OpenMesh::sane_aarg((p0-p1).normalize() | (p2-p1).normalize()) );
      
      REALT tmp = 0.0;
      if ( !OpenMesh::is_eq(alpha,M_PI/2.0) )
        tmp += (p0-p1).sqrnorm()*1.0/tan(alpha);
    
      if ( !OpenMesh::is_eq(gamma,M_PI/2.0) )
        tmp += (p0-p2).sqrnorm()*1.0/tan(gamma);
      
#ifdef WIN32
	  if ( OpenMesh::is_zero(tmp) )
		continue;
#else
      if ( OpenMesh::is_zero(tmp) || std::isinf(tmp) )
		continue;
#endif

      _area += 0.125*( tmp );
    }
    else
    {
      if(obt == 1)
      {
	     _area += ((p1-p0) % (p2-p0)).norm() * 0.5 * 0.5;
      }
      else
      {
	     _area += ((p1-p0) % (p2-p0)).norm() * 0.5 * 0.25;
      }
    }
    
    _n += ((p0-p1)*cotw);

    // error handling
    //if(_area < 0) std::cerr << "error: triangle area < 0\n";
//    if(isnan(_area))
//    {
//      REALT gamma = acos( ((p0-p1).normalize() | (p2-p1).normalize()) );

/*
      std::cerr << "***************************\n";
      std::cerr << "error : trianlge area = nan\n";
      std::cerr << "alpha : " << alpha << std::endl;
      std::cerr << "beta  : " << beta  << std::endl;
      std::cerr << "gamma : " << gamma << std::endl;
      std::cerr << "cotw  : " << cotw  << std::endl;
      std::cerr << "normal: " << _n << std::endl;
      std::cerr << "p0    : " << p0 << std::endl;
      std::cerr << "p1    : " << p1 << std::endl;
      std::cerr << "p2    : " << p2 << std::endl;
      std::cerr << "p3    : " << p3 << std::endl;
      std::cerr << "***************************\n";
*/
//    }
  }

  _n /= 4.0*_area;
}



//=============================================================================
} // curvature Namespace 
//=============================================================================
