//=============================================================================
//
//                               OpenFlipper
//        Copyright (C) 2008 by Computer Graphics Group, RWTH Aachen
//                           www.openflipper.org
//
//-----------------------------------------------------------------------------
//
//                                License
//
//  OpenFlipper is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  OpenFlipper is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License
//  along with OpenFlipper.  If not, see <http://www.gnu.org/licenses/>.
//
//-----------------------------------------------------------------------------
//
//   $Revision$
//   $Author$
//   $Date$
//
//=============================================================================




//=============================================================================
//
//  IMPLEMENTATION
//
//=============================================================================

#define GEOMETRYFUNCTIONS_C

//== INCLUDES =================================================================

#include "GeometryFunctions.hh"

#include <OpenMesh/Core/Geometry/MathDefs.hh>

#include <math.h>
#include <iostream>


//== NAMESPACES ===============================================================

namespace GeometryFunctions {

//== IMPLEMENTATION ==========================================================

/// Return false if x is not a number
inline bool isNan(double x) {
   return (x != x);
}


template < typename VectorT >
VectorT project_to_edge(const VectorT& _start , const VectorT& _end , const VectorT& _point )
{
  VectorT direction = ( _end - _start );
  assert( fabs(direction.norm()) > 0.0000000001) ;
  const VectorT projected_point = ( ( _point - _start ) | direction ) * direction + _start;

  if ( ( ( projected_point - _start ) | direction ) > ( ( _end - _start ) | direction ) )
     return _end;

  if ( ( ( projected_point - _start ) | direction ) < 0 )
     return _start;

  return projected_point;
}

template < typename VectorT , typename ValueT >
inline
ValueT
dist_plane(const VectorT& _porigin,
                 const VectorT& _pnormal,
                 const VectorT& _point)
{
  assert( fabs(_pnormal.norm()) > 0.0000000001) ;
  return( ((_point-_porigin) | _pnormal));
}

template < typename VectorT >
inline
VectorT
project_to_plane(const VectorT& _porigin,
                           const VectorT& _pnormal,
                           const VectorT& _point)
{
   return (_point - _pnormal * dist_plane< VectorT , double >( _porigin , _pnormal , _point ) );
}

template < typename VectorT , typename ValueT >
ValueT
get_fullangle( VectorT _vector1 , VectorT _vector2 , const VectorT& _normal , bool& skip )
{
   //Project vectors into tangent plane defined by _normal
   _vector1 = _vector1 - _normal * ( _vector1 | _normal );
   _vector2 = _vector2 - _normal * ( _vector2 | _normal );
   _vector1.normalize();
   _vector2.normalize();

   //calculate projection onto right Vector (used to decide if vector2 is left or right of vector1
   const double right = ( ( _normal % _vector1 ) | _vector2 ) ;

   double sp    = ( _vector1 | _vector2 );
   //Catch some errors with scalar product and the following acos
   if (sp < -1.0) {
     sp = -1.0;
   }

   if (sp > 1.0) {
      sp = 1.0;
   }

   double angle = acos(sp);

   // catch some possible nans
   skip = ( isNan(right) || isNan(angle) ) ;

   if ( right < 0 ) {
      angle = 2 * M_PI - angle;
   }

   return angle;
}

template < typename ValueT >
inline
ValueT
angle_dist( const ValueT& angle0 , const ValueT& angle1 ) {
  ValueT dist = fabs( angle1 - angle0 );
  return ( std::min( dist , 2 * M_PI - dist) );
}

template < typename ValueT >
inline
ValueT
get_angle( const ValueT& _cos ,
                  const ValueT& _sin )
{
   const double angle_asin   = asin( OpenMesh::sane_aarg(_sin) );
   const double angle_acos  = acos( OpenMesh::sane_aarg(_cos) );

   if ( angle_asin >= 0 ) { //Quadrant 1,2
      if ( angle_acos >= 0 ) { // Quadrant 1
          return angle_asin;
      } else { //Quadrant 2
         return (M_PI - angle_asin);
      }
   } else {  //Quadrant 3,4
      if ( angle_acos >= 0 ) { // Quadrant 4
         return (2 * M_PI + angle_asin);
      } else { //Quadrant 3
         return (M_PI - angle_asin);
      }
   }
}

template < typename ValueT >
inline
ValueT
rad_to_deg( const ValueT& _angle ) {
  return ( _angle / M_PI * 180);
}

template < typename ValueT >
inline
ValueT
deg_to_rad( const ValueT& _angle ) {
   return ( _angle / 180 * M_PI );
}

template<class VectorT>
int is_obtuse(const VectorT& _p0,
                     const VectorT& _p1,
	             const VectorT& _p2 )
{
  const double a0 = ((_p1-_p0)|(_p2-_p0));

  if ( a0<0.0) return 1;
  else
  {
    const double a1 = ((_p0-_p1)|(_p2-_p1));

    if (a1 < 0.0 ) return 2;
    else
    {
      const double a2 = ((_p0-_p2)|(_p1-_p2));

      if (a2 < 0.0 ) return 3;
      else return 0;
    }
  }
}

//=============================================================================
} // GeometryFunctions Namespace
//=============================================================================
