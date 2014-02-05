/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2014 by Computer Graphics Group, RWTH Aachen      *
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
//
//=============================================================================


#ifndef ALGORITHMSANGLE_HH
#define ALGORITHMSANGLE_HH


/*! \file AlgorithmsAngleT.hh
    \brief Functions for geometric operations related to angles
    
    General file with template functions handling angles
*/

//== INCLUDES =================================================================

//== FORWARDDECLARATIONS ======================================================

//== NAMESPACES ===============================================================

/// Namespace providing different geometric functions concerning angles
namespace ACG {
namespace Geometry {
   
//== CLASS DEFINITION =========================================================   
                       
/** Return a fully parametrized angle
   @param _vector1 vector pointing away from center, angle = 0
   @param _vector2 vector for which the angle should be calculated                    
   @param _normal  the normal used to compute if vector2 is left or right of vecor1
   @param _skip    Flag to catch nan. If true nan occurred and value should not be used
   @return the angle (0 .. 2 * PI)
*/
template < typename VectorT , typename ValueT >
ValueT
getFullangle( VectorT _vector1, 
              VectorT _vector2, 
              const   VectorT& _normal, 
              bool&   _skip );


/** Calculate the difference between two angles ( minimum distance )
   @param angle0 angle1
   @param angle1 angle2
   @return The difference between the angles (0..PI)
*/
template < typename ValueT >
inline
ValueT
angleDist( const ValueT& angle0 , 
           const ValueT& angle1 );
                  
/** Calculate the correct 2D angle if cos and sin of the angle are given
    This function calculates based on the signs of the acos and asin of the 
    given angles, in which quadrant the angle is and returns the full angle
    in radians
   @param _cos cos of angle
   @param _sin sin of angle
   @return angle
*/
template < typename ValueT >
inline
ValueT
getAngle( const ValueT& _cos , 
          const ValueT& _sin );      

/** Convert angle from radians to degree
   @param _angle in radians
   @return angle in degree
*/
template < typename ValueT >
inline
ValueT
radToDeg( const ValueT& _angle );      

/** Convert angle from degree to radians
   @param _angle in degree 
   @return angle in radians
*/
template < typename ValueT >
inline
ValueT
degToRad( const ValueT& _angle ); 


//=============================================================================
} // Geometry Namespace 
} // ACG Namespace 
//=============================================================================
#if defined(INCLUDE_TEMPLATES) && !defined(ALGORITHMSANGLE_C)
#define ALGORITHMSANGLE_TEMPLATES
#include "AlgorithmsAngleT.cc"
#endif
//=============================================================================
#endif // ALGORITHMSANGLE_HH defined
//=============================================================================

