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
//  BSplineCurve Selection
//  Author:  Ellen Dekkers <dekkers@cs.rwth-aachen.de>
//
//=============================================================================


#ifndef BSPLINECURVESELECTION_HH
#define BSPLINECURVESELECTION_HH


/*! \file bSplineCurveSelectionT.hh
    \brief Functions for selection on a BSpline curve
    
*/

//== INCLUDES =================================================================

#include <vector>

//== NAMESPACES ===============================================================

namespace BSplineCurveSelection {

//== DEFINITIONS ==============================================================

//===========================================================================
/** @name Vertex Selection
 * @{ */
//===========================================================================    
  
template< typename BSplineCurveT >
inline
void selectVertices(BSplineCurveT* _bSplineCurve, std::vector< int >& _vertices);

template< typename BSplineCurveT >
inline
void unselectVertices(BSplineCurveT* _bSplineCurve, std::vector< int >& _vertices);
  
template< typename BSplineCurveT >
inline
void selectAllVertices(BSplineCurveT* _bSplineCurve);

template< typename BSplineCurveT >
inline
void clearVertexSelection(BSplineCurveT* _bSplineCurve);

template< typename BSplineCurveT >
inline      
void invertVertexSelection(BSplineCurveT* _bSplineCurve);

template< typename BSplineCurveT >
inline    
std::vector< int > getVertexSelection(BSplineCurveT* _bSplineCurve);

/** @} */  

//===========================================================================
/** @name Edge Selection
 * @{ */
//===========================================================================  

template< typename BSplineCurveT >
inline
void selectEdges(BSplineCurveT* _bSplineCurve, std::vector< int >& _edges);

template< typename BSplineCurveT >
inline
void unselectEdges(BSplineCurveT* _bSplineCurve, std::vector< int >& _edges);

template< typename BSplineCurveT >
inline
void selectAllEdges(BSplineCurveT* _bSplineCurve);

template< typename BSplineCurveT >
inline
void clearEdgeSelection(BSplineCurveT* _bSplineCurve);

template< typename BSplineCurveT >
inline      
void invertEdgeSelection(BSplineCurveT* _bSplineCurve);


template< typename BSplineCurveT >
inline    
std::vector< int > getEdgeSelection(BSplineCurveT* _bSplineCurve);

/** @} */ 

//===========================================================================
/** @name Knot Selection
 * @{ */
//===========================================================================  

template< typename BSplineCurveT >
inline
void selectKnots(BSplineCurveT* _bSplineCurve, std::vector< int >& _knots);

template< typename BSplineCurveT >
inline
void unselectKnots(BSplineCurveT* _bSplineCurve, std::vector< int >& _knots);
  
template< typename BSplineCurveT >
inline
void selectAllKnots(BSplineCurveT* _bSplineCurve);

template< typename BSplineCurveT >
inline
void clearKnotSelection(BSplineCurveT* _bSplineCurve);

template< typename BSplineCurveT >
inline      
void invertKnotSelection(BSplineCurveT* _bSplineCurve);

template< typename BSplineCurveT >
inline    
std::vector< int > getKnotSelection(BSplineCurveT* _bSplineCurve);

/** @} */ 

//=============================================================================
} // BSplineCurveSelection Namespace 
//=============================================================================

#if defined(INCLUDE_TEMPLATES) && !defined(BSPLINECURVESELECTION_C)
#define BSPLINECURVESELECTION_TEMPLATES
#include "bSplineCurveSelectionT.cc"
#endif
             
//=============================================================================
#endif // BSPLINECURVESELECTION_HH defined
//=============================================================================
