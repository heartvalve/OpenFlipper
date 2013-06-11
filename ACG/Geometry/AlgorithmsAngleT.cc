/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2011 by Computer Graphics Group, RWTH Aachen      *
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
//  IMPLEMENTATION
//
//=============================================================================

#define ALGORITHMSANGLE_C

//== INCLUDES =================================================================

#include "AlgorithmsAngleT.hh"

#include <OpenMesh/Core/Geometry/MathDefs.hh>

#include <cmath>
#include <iostream>


//== NAMESPACES ===============================================================

namespace ACG {
namespace Geometry {

//== IMPLEMENTATION ==========================================================

/// Return false if x is not a number
inline bool isNan(double x) {
   return (x != x);
}


template < typename VectorT , typename ValueT >
ValueT
getFullangle( VectorT _vector1 , VectorT _vector2 , const VectorT& _normal , bool& _skip )
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
   _skip = ( isNan(right) || isNan(angle) ) ;

   if ( right < 0 ) {
      angle = 2 * M_PI - angle;
   }

   return angle;
}

template < typename ValueT >
inline
ValueT
angleDist( const ValueT& angle0 , const ValueT& angle1 ) {
  ValueT dist = fabs( angle1 - angle0 );
  return ( std::min( dist , 2 * M_PI - dist) );
}

template < typename ValueT >
inline
ValueT
getAngle( const ValueT& _cos ,
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
radToDeg( const ValueT& _angle ) {
  return ( _angle / M_PI * 180);
}

template < typename ValueT >
inline
ValueT
degToRad( const ValueT& _angle ) {
   return ( _angle / 180 * M_PI );
}



//=============================================================================
} // Geometry Namespace
} // ACG Namespace
//=============================================================================
