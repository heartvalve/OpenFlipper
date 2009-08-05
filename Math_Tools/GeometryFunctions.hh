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
 *   $Revision$                                                         *
 *   $Author$                                                      *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/


//=============================================================================
//
//
//=============================================================================


#ifndef GEOMETRYFUNCTIONS_HH
#define GEOMETRYFUNCTIONS_HH


/*! \file GeometryFunctions.hh
    \brief Functions for geometric operations
    
    General file with template functions doing for example
    projections,...
*/

//== INCLUDES =================================================================

//== FORWARDDECLARATIONS ======================================================

//== NAMESPACES ===============================================================

/// Namespace providing different geometric functions
namespace GeometryFunctions {
   
//== CLASS DEFINITION =========================================================
   
/**
   project one point to an edge. If its projection is not on the edge itself, the start or the endpoint is returned
   @param _start beginning of edge
   @param _end   end of the edge
   @param _point point to be projected
*/
template < typename VectorT >
VectorT project_to_edge(const VectorT& _start , 
                                         const VectorT& _end , 
                                         const VectorT& _point );

/**
   Checks the distance from a point to a plane
   @param _porigin Planes origin
   @param _pnormal Plane normal
   @param _point   point to test
   @return         distance
*/
template < typename VectorT , typename ValueT >
inline
ValueT 
dist_plane(const VectorT& _porigin, 
                 const VectorT& _pnormal, 
                 const VectorT&  _point);
                 
/**
   projects a point to a plane
   @param _porigin Planes origin
   @param _pnormal Plane normal
   @param _point   point to project
   @return         projected point
*/
template < typename VectorT >
inline
VectorT
project_to_plane(const VectorT& _porigin, 
                           const VectorT& _pnormal, 
                           const VectorT&  _point);
                       
/** Return a fully parametrized angle
   @param _vector1 vector pointing away from center, angle = 0
   @param _vector2 vector for which the angle should be calculated                    
   @param _normal  the normal used to compute if vector2 is left or right of vecor1
   @param _skip    Flag to catch nan. If true nan occured and value should not be used
   @return the angle (0 .. 2 * PI)
*/
template < typename VectorT , typename ValueT >
ValueT
get_fullangle( VectorT _vector1 , 
                       VectorT _vector2 , 
                       const VectorT& _normal  , 
                       bool& skip );


/** Calculate the difference between two angles
   @param _angle0 angle1
   @param _angle1 angle2
   @return The difference between the angles (0..PI)
*/
template < typename ValueT >
inline
ValueT
angle_dist( const ValueT& angle0 , 
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
get_angle( const ValueT& _cos , 
                  const ValueT& _sin );      

/** Convert angle from radians to degree
   @param _angle in radians
   @return angle in degree
*/
template < typename ValueT >
inline
ValueT
rad_to_deg( const ValueT& _angle );      

/** Convert angle from degree to radians
   @param _angle in degree 
   @return angle in radians
*/
template < typename ValueT >
inline
ValueT
deg_to_rad( const ValueT& _angle ); 

/**
* test angles in triangle
* return 0 if all smaller than 90�
* return 1 if angle at _p0 ist greater than 90�
* return 2 if angle at _p1 ist greater than 90�
* return 3 if angle at _p2 ist greater than 90�
*/
template<class VectorT>
int  is_obtuse(const VectorT& _p0,
                      const VectorT& _p1,
                      const VectorT& _p2 );

//=============================================================================
} // GeometryFunctions Namespace 
//=============================================================================
#if defined(INCLUDE_TEMPLATES) && !defined(GEOMETRYFUNCTIONS_C)
#define GEOMETRYFUNCTIONS_TEMPLATES
#include "GeometryFunctions.cc"
#endif
//=============================================================================
#endif // GEOMETRYFUNCTIONS_HH defined
//=============================================================================

