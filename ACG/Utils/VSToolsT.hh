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

#ifndef VSTOOLS_HH
#define VSTOOLS_HH


//== FORWARDDECLARATIONS ======================================================

//== NAMESPACES ===============================================================

//== DEFINITION =========================================================

/** These functions are required for Visual Studio to work around missing 
    functions. Basic equivalent functions for double exist in the float 
    header but are named different. So this wrapper makes them standard compatible.
    */
#ifdef WIN32
 #include <cfloat>

 namespace std {

   inline int isnan(double x)
   {
     return _isnan(x);
   } 

   inline int isinf(double x)
   {
     return !_finite(x);
   } 

   inline int isfinite(double x)
   {
     return _finite(x);
   } 
  }

 inline double nearbyint(double x) {
   if( x >= 0.0 )
     return int( x + 0.5 );
   else
     return int( x - 0.5 );
 }

 inline double round ( double _value ) {
   return nearbyint(_value);
 }


#endif


//=============================================================================
#endif // VSTOOLS_HH defined
//=============================================================================

