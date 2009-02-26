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


#ifndef MESHNAVIGATIONT_HH
#define MESHNAVIGATIONT_HH


/*! \file MeshNavigationT.hh
    \brief Functions for Navigation on a Mesh
    
*/

//== INCLUDES =================================================================

#include <vector>

//== FORWARDDECLARATIONS ======================================================

//== NAMESPACES ===============================================================

namespace MeshNavigation {

//== DEFINITIONS ==============================================================

/**
   For Valence 6 Vertices Only!! iterates 3 Halfedges around from Vertex of _he. Returns the he on the other side
   @param _he Outgoing halfedge of Vertex to iterate around
   @return Halfedge on the opposite side
  */
template < typename MeshT >
inline
typename MeshT::HalfedgeHandle
opposite_halfedge(MeshT& _mesh , typename MeshT::HalfedgeHandle _he);

template < typename MeshT >
typename MeshT::VertexHandle
findClosestBoundary(MeshT* _mesh , typename MeshT::VertexHandle _vh);

//=============================================================================
} // MeshNavigation Namespace 
//=============================================================================
#if defined(INCLUDE_TEMPLATES) && !defined(MESHNAVIGATIONT_C)
#define MESHNAVIGATIONT_TEMPLATES
#include "MeshNavigationT.cc"
#endif
//=============================================================================
#endif // MESHNAVIGATIONT_HH defined
//=============================================================================

