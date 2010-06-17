/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2010 by Computer Graphics Group, RWTH Aachen      *
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
//  BSplineSurface Selection
//  Author:  Ellen Dekkers <dekkers@cs.rwth-aachen.de>
//
//=============================================================================


#ifndef BSPLINESURFACESELECTION_HH
#define BSPLINESURFACESELECTION_HH


/*! \file bSplineSurfaceSelection.hh
    \brief Functions for selection on a BSpline Surface

*/

//== INCLUDES =================================================================

#include <vector>

//== NAMESPACES ===============================================================

namespace BSplineSurfaceSelection {

//== DEFINITIONS ==============================================================

//===========================================================================
/** @name Vertex Selection
 * @{ */
//===========================================================================

template< typename BSplineSurfaceT >
inline
void selectVertices(BSplineSurfaceT* _bSplineSurface, std::vector< int >& _vertices);

template< typename BSplineSurfaceT >
inline
void unselectVertices(BSplineSurfaceT* _bSplineSurface, std::vector< int >& _vertices);

template< typename BSplineSurfaceT >
inline
void selectAllVertices(BSplineSurfaceT* _bSplineSurface);

template< typename BSplineSurfaceT >
inline
void clearVertexSelection(BSplineSurfaceT* _bSplineSurface);

template< typename BSplineSurfaceT >
inline
void invertVertexSelection(BSplineSurfaceT* _bSplineSurface);

template< typename BSplineSurfaceT >
inline
std::vector< int > getVertexSelection(BSplineSurfaceT* _bSplineSurface);

/** @} */


//===========================================================================
/** @name Knot Selection
 * @{ */
//===========================================================================  

template< typename BSplineSurfaceT >
inline
void selectKnots(BSplineSurfaceT* _bSplineSurface, std::vector< int >& _knots_m, std::vector< int >& _knots_n);

template< typename BSplineSurfaceT >
inline
void unselectKnots(BSplineSurfaceT* _bSplineSurface, std::vector< int >& _knots_m, std::vector< int >& _knots_n);
  
template< typename BSplineSurfaceT >
inline
void selectAllKnots(BSplineSurfaceT* _bSplineSurface);

template< typename BSplineSurfaceT >
inline
void clearKnotSelection(BSplineSurfaceT* _bSplineSurface);

template< typename BSplineSurfaceT >
inline      
void invertKnotSelection(BSplineSurfaceT* _bSplineSurface);

template< typename BSplineSurfaceT >
inline    
std::vector< int > getKnotSelection(BSplineSurfaceT* _bSplineSurface);

/** @} */ 



//===========================================================================
/** @name Edge Selection
 * @{ */
//===========================================================================

template< typename BSplineSurfaceT >
inline
void selectEdges(BSplineSurfaceT* _bSplineSurface, std::vector< int >& _edges);

template< typename BSplineSurfaceT >
inline
void unselectEdges(BSplineSurfaceT* _bSplineSurface, std::vector< int >& _edges);

template< typename BSplineSurfaceT >
inline
void selectAllEdges(BSplineSurfaceT* _bSplineSurface);

template< typename BSplineSurfaceT >
inline
void clearEdgeSelection(BSplineSurfaceT* _bSplineSurface);

template< typename BSplineSurfaceT >
inline
void invertEdgeSelection(BSplineSurfaceT* _bSplineSurface);


template< typename BSplineSurfaceT >
inline
std::vector< int > getEdgeSelection(BSplineSurfaceT* _bSplineSurface);

/** @} */

//=============================================================================
} // BSplineSurfaceSelection Namespace
//=============================================================================

#if defined(INCLUDE_TEMPLATES) && !defined(BSPLINESURFACESELECTION_C)
#define BSPLINESURFACESELECTION_TEMPLATES
#include "bSplineSurfaceSelectionT.cc"
#endif

//=============================================================================
#endif // BSPLINESURFACESELECTION_HH defined
//=============================================================================
