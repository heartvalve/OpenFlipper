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
//
//=============================================================================


#ifndef MESHINFOT_HH
#define MESHINFOT_HH


/*! \file MeshNavigationT.hh
    \brief Functions for getting information about a mesh
    
*/

//== INCLUDES =================================================================

#include <vector>

//== FORWARDDECLARATIONS ======================================================

//== NAMESPACES ===============================================================

namespace MeshInfo {

//== DEFINITIONS ==============================================================

/** \brief Count boundaries
 * 
 * Get the number of independent boundaries
 *
 */
template < typename MeshT >
inline
int boundaryCount(MeshT* _mesh );

/** \brief Count components
 * 
 * Get the number of independent components
 *
 */
template < typename MeshT >
inline
int componentCount(MeshT* _mesh );

/**
    Calculate the axis aligned Boundingbox of a given Mesh
    @param _mesh Mesh to work on
    @param _min lower left corner of the box
    @param _max upper right corner of the box
*/ 
template < typename MeshT >
inline    
void getBoundingBox( MeshT&   _mesh, 
                     typename MeshT::Point& _min , 
                     typename MeshT::Point& _max);

/** Compute the center of gravity for a given Mesh
 * @param _mesh The mesh to work on
 * @return The cog of the mesh
 */
template < typename MeshT >
inline    
typename MeshT::Point
cog ( const MeshT& _mesh );    

//=============================================================================
} // MeshInfo Namespace 
//=============================================================================
#if defined(INCLUDE_TEMPLATES) && !defined(MESHINFOT_C)
#define MESHINFOT_TEMPLATES
#include "MeshInfoT.cc"
#endif
//=============================================================================
#endif // MESHINFOT_HH defined
//=============================================================================

