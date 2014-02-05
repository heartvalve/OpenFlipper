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

