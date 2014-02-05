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
//  Math_Tools - IMPLEMENTATION
//
//=============================================================================

#define MATH_TOOLS_C

//== INCLUDES =================================================================

#include "Math_Tools.hh"
#include <OpenMesh/Core/Geometry/MathDefs.hh>

//== NAMESPACES ===============================================================

namespace MathTools {

template < typename VectorT > 
inline
void
sane_normalize( VectorT & _vec ) {
   
    // Normalize all vectors (Save version)
    if ( ! OpenMesh::is_zero( _vec.sqrnorm() ) )
       _vec.normalize();      
    else 
       _vec = VectorT(0.0,0.0,0.0);
}

/// Normalize Vector (if vector size is zero, return zero point);
template < typename VectorT > 
inline
VectorT
sane_normalized( VectorT _vec) {
     // Normalize all vectors (Save version)
    if ( ! OpenMesh::is_zero( _vec.sqrnorm() ) )
       return (_vec.normalize());      
    else 
       return ( VectorT(0.0,0.0,0.0) );
}



//=============================================================================
} // namespace MathTools
//=============================================================================
