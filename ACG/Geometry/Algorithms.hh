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




#ifndef GEO_ALGORITHMS_HH
#define GEO_ALGORITHMS_HH


//== INCLUDES =================================================================

#include <float.h>
#include "../Math/VectorT.hh"


namespace ACG {
namespace Geometry {


//== 3D STUFF =================================================================

  
/** barycentric coord of _p w.r.t. (_u,_v,_w) in 3D
    _p has to lie in plane (_u,_v,_w) **/
template<typename Scalar>
bool
baryCoord( const VectorT<Scalar,3>&  _p,
	   const VectorT<Scalar,3>&  _u,
	   const VectorT<Scalar,3>&  _v,
	   const VectorT<Scalar,3>&  _w,
	   VectorT<Scalar,3>&        _result );


/// return area of triangle (_v0, _v1, _v2)
template <class Vec>
typename Vec::value_type
triangleArea( const Vec& _v0,
	      const Vec& _v1,
	      const Vec& _v2 )
{
  return sqrt(triangleAreaSquared(_v0,_v1,_v2));
}
  

/// return squared area of triangle (_v0, _v1, _v2)
template <class Vec>
typename Vec::value_type
triangleAreaSquared( const Vec& _v0,
		     const Vec& _v1,
		     const Vec& _v2 );


/// distance from point _p to line (_v0,_v1)
template<class Vec>
typename Vec::value_type
distPointLine( const Vec& _p,
	       const Vec& _v0,
	       const Vec& _v1,
	       Vec*       _min_v=0 )
{ return sqrt(distPointLineSquared(_p, _v0, _v1, _min_v)); }


/// squared distance from point _p to line (_v0,_v1)
template<class Vec>
typename Vec::value_type
distPointLineSquared( const Vec& _p,
		      const Vec& _v0,
		      const Vec& _v1,
		      Vec*       _min_v=0);

/// distance from point _p to triangle (_v0, _v1, _v2)
template <class Vec>
typename Vec::value_type
distPointTriangle( const Vec& _p,
		   const Vec& _v0,
		   const Vec& _v1,
		   const Vec& _v2,
		   Vec& _nearestPoint )
{ return sqrt(distPointTriangleSquared(_p, _v0, _v1, _v2, _nearestPoint)); }


/// squared distance from point _p to triangle (_v0, _v1, _v2)
template <class Vec>
typename Vec::value_type
distPointTriangleSquared( const Vec& _p,
			  const Vec& _v0,
			  const Vec& _v1,
			  const Vec& _v2,
			  Vec& _nearestPoint );


/// squared distance of lines (_v00, _v01) and (_v10, _v11)
template<typename Scalar>
Scalar
distLineLineSquared( const VectorT<Scalar,3>& _v00,
		     const VectorT<Scalar,3>& _v01,
		     const VectorT<Scalar,3>& _v10,
		     const VectorT<Scalar,3>& _v11,
		     VectorT<Scalar,3>*       _min_v0=0,
		     VectorT<Scalar,3>*       _min_v1=0,
		     bool                          _fastApprox=false );


/// distance of lines (_v00, _v01) and (_v10, _v11)
template<typename Scalar>
Scalar
distLineLine( const VectorT<Scalar,3>& _v00,
	      const VectorT<Scalar,3>& _v01,
	      const VectorT<Scalar,3>& _v10,
	      const VectorT<Scalar,3>& _v11,
	      VectorT<Scalar,3>*       _min_v0=0,
	      VectorT<Scalar,3>*       _min_v1=0 )
{
  return sqrt(distLineLineSquared(_v00, _v01, _v10, _v11,
				  _min_v0, _min_v1));
}


/// construct min. enclosing sphere
template<typename Scalar>
bool
minSphere(const VectorT<Scalar,3>&  _v0,
	  const VectorT<Scalar,3>&  _v1,
	  const VectorT<Scalar,3>&  _v2,
	  VectorT<Scalar,3>&        _center,
	  Scalar&                   _radius);


/// return squared radius of min. enclosing circle of triangle (_v0,_v1,_v2)
template<typename Scalar>
Scalar
minRadiusSquared( const VectorT<Scalar,3>&  _v0,
		  const VectorT<Scalar,3>&  _v1,
		  const VectorT<Scalar,3>&  _v2 );

  
/// return radius of min. enclosing circle of triangle (_v0,_v1,_v2)
template<typename Scalar>
Scalar
minRadius( const VectorT<Scalar,3>&  _v0,
	   const VectorT<Scalar,3>&  _v1,
	   const VectorT<Scalar,3>&  _v2 )
{
  return sqrt(minRadiusSquared(_v0, _v1, _v2));
}


/// return circumcenter of triangle (_v0,_v1,_v2)
template<typename Scalar>
bool
circumCenter( const VectorT<Scalar,3>&  _v0,
	      const VectorT<Scalar,3>&  _v1,
	      const VectorT<Scalar,3>&  _v2,
	      VectorT<Scalar,3>&        _result );


/// return squared radius of circumcircle of triangle (_v0,_v1,_v2)
template<typename Scalar>
Scalar
circumRadiusSquared( const VectorT<Scalar,3>&  _v0,
		     const VectorT<Scalar,3>&  _v1,
		     const VectorT<Scalar,3>&  _v2 );


/// return radius of circumcircle of triangle (_v0,_v1,_v2)
template<typename Scalar>
Scalar
circumRadius( const VectorT<Scalar,3>&  _v0,
	      const VectorT<Scalar,3>&  _v1,
	      const VectorT<Scalar,3>&  _v2 )
{
  return sqrt(circumRadiusSquared(_v0, _v1, _v2));
}



/// return circumcenter of tetrahedron (_v0,_v1,_v2,_v3)
template<typename Scalar>
bool
circumCenter( const VectorT<Scalar,3>&  _v0,
	      const VectorT<Scalar,3>&  _v1,
	      const VectorT<Scalar,3>&  _v2,
	      const VectorT<Scalar,3>&  _v3,
	      VectorT<Scalar,3>&        _result );


/// return squared radius of circumcircle of tetrahedron (_v0,_v1,_v2,_v3)
template<typename Scalar>
Scalar
circumRadiusSquared( const VectorT<Scalar,3>&  _v0,
		     const VectorT<Scalar,3>&  _v1,
		     const VectorT<Scalar,3>&  _v2,
		     const VectorT<Scalar,3>&  _v3 )
{
  VectorT<Scalar,3> cc;
  return circumCenter(_v0, _v1, _v2, _v3, cc) ? (cc-_v0).sqrnorm() : FLT_MAX;
}


/// return radius of circumcircle of tetrahedron (_v0,_v1,_v2,_v3)
template<typename Scalar>
Scalar
circumRadius( const VectorT<Scalar,3>&  _v0,
	      const VectorT<Scalar,3>&  _v1,
	      const VectorT<Scalar,3>&  _v2,
	      const VectorT<Scalar,3>&  _v3 )
{
  return sqrt(circumRadiusSquared(_v0, _v1, _v2, _v3));
}


/// find a vector that's perpendicular to _v
template <typename Scalar>
VectorT<Scalar,3>
perpendicular( const VectorT<Scalar,3>&  _v );



//== 2D STUFF =================================================================



/// barycentric coord of _p w.r.t. (_u,_v,_w) in 2D
template<typename Scalar>
bool
baryCoord( const VectorT<Scalar,2>&  _p,
	   const VectorT<Scalar,2>&  _u,
	   const VectorT<Scalar,2>&  _v,
	   const VectorT<Scalar,2>&  _w,
	   VectorT<Scalar,3>&        _result );


/// is point _p in triangle (_v0,_v1,_v2)?
template<typename Scalar>
bool
isInTriangle( const VectorT<Scalar,2>&  _p,
	      const VectorT<Scalar,2>&  _u,
	      const VectorT<Scalar,2>&  _v,
	      const VectorT<Scalar,2>&  _w )
{
  VectorT<Scalar,3> bary;
  if (baryCoord(_p, _u, _v, _w, bary)) 
    return ((bary[0]>0.0) && (bary[1]>0.0) && (bary[2]>0.0));
  else {
    std::cerr << "point in triangle error\n";
    return false;
  }
}


/// orientation of point _p w.r.t. line through _v0,_v1
template<typename Scalar>
Scalar
pointLineOrientation( const VectorT<Scalar,2>&  _p,
		      const VectorT<Scalar,2>&  _v0,
		      const VectorT<Scalar,2>&  _v1 )
{
  VectorT<Scalar,2> d1(_p-_v0), d2(_v1-_v0);
  return (d1[0]*d2[1]-d1[1]*d2[0]);
}




/// are 3 vertices in counterclockwise order?
template<typename Scalar>
bool
isCCW( const VectorT<Scalar,2>&  _v0,
       const VectorT<Scalar,2>&  _v1,
       const VectorT<Scalar,2>&  _v2 )
{
  return ( pointLineOrientation(_v0, _v1, _v2) < Scalar(0.0) );
}


/// are 3 vertices in clockwise order?
template<typename Scalar>
bool
isCW( const VectorT<Scalar,2>&  _v0,
      const VectorT<Scalar,2>&  _v1,
      const VectorT<Scalar,2>&  _v2 )
{
  return ( pointLineOrientation(_v0, _v1, _v2) > Scalar(0.0) );
}


/// intersect two line segments (_v0,_v1) and (_v2,_v3)
template<typename Scalar>
bool
lineIntersection( const VectorT<Scalar,2>&  _v0,
		  const VectorT<Scalar,2>&  _v1,
		  const VectorT<Scalar,2>&  _v2,
		  const VectorT<Scalar,2>&  _v3,
		  Scalar& _t1,
		  Scalar& _t2 );


/// return circumcenter of triangle (_v0,_v1,_v2)
template<typename Scalar>
bool
circumCenter( const VectorT<Scalar,2>&  _v0,
	      const VectorT<Scalar,2>&  _v1,
	      const VectorT<Scalar,2>&  _v2,
	      VectorT<Scalar,2>&        _result );


//== N-DIM STUFF ==============================================================


/// return aspect ratio of triangle
template <typename Scalar, int N>
Scalar
aspectRatio( const VectorT<Scalar, N>& _v0,
	     const VectorT<Scalar, N>& _v1,
	     const VectorT<Scalar, N>& _v2 );

/// return roundness of triangle: 1=equilateral, 0=colinear
template <typename Scalar, int N>
Scalar
roundness( const VectorT<Scalar, N>& _v0,
	   const VectorT<Scalar, N>& _v1,
	   const VectorT<Scalar, N>& _v2 );


//=============================================================================
} // namespace Geometry
} // namespace ACG
//=============================================================================
#if defined(INCLUDE_TEMPLATES) && !defined(GEO_ALGORITHMS_C)
#define GEO_ALGORITHMS_TEMPLATES
#include "Algorithms.cc"
#endif
//=============================================================================
#endif // GEO_ALGORITHMS_HH defined
//=============================================================================

