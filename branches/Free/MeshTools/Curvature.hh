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


#ifndef CURVATURE_HH
#define CURVATURE_HH


/*! \file Curvature.hh
    \brief Functions for calculating curvatures
    
*/

//== INCLUDES =================================================================

#include <vector>

//== FORWARDDECLARATIONS ======================================================

//== NAMESPACES ===============================================================

namespace curvature {

//== DEFINITIONS ==============================================================

/*! compute consistent dirscrete gaussian curvature (vertex is a small sphere patch, edges are small cylinders)
*/
template< typename MeshT >
double
gauss_curvature(MeshT& _mesh, const typename MeshT::VertexHandle& _vh);
   
/**
* Mean Curvature Normal Operator
* warning: if mean curvature < 0 _n points to the inside
* warning: if mean curvature = 0 -> no normal information
@param _n       mean_curvature(vit)*n(vit)
@param _area global vertex area
*/
template<class MeshT, class VectorT, class REALT>
void discrete_mean_curv_op( const MeshT&                           _m,
                                                const typename MeshT::VertexHandle& _vh, 
                                                VectorT&                         _n,
                                                REALT&                           _area );   

   
//=============================================================================
} 
//=============================================================================
#if defined(INCLUDE_TEMPLATES) && !defined(CURVATURE_C)
#define CURVATURE_TEMPLATES
#include "Curvature.cc"
#endif
//=============================================================================
#endif // CURVATURE_HH defined
//=============================================================================

