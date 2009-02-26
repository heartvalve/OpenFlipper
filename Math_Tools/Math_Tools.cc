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
