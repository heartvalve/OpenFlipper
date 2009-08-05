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


#ifndef POLYLINESELECTION_HH
#define POLYLINESELECTION_HH


/*! \file polyLineSelection.hh
    \brief Functions for selection on a polyLine
    
*/

//== INCLUDES =================================================================

#include <vector>

//== NAMESPACES ===============================================================

namespace PolyLineSelection {

//== DEFINITIONS ==============================================================

//===========================================================================
/** @name Vertex Selection
 * @{ */
//===========================================================================    
  
template< typename PolyLineT >
inline
void selectVertices(PolyLineT* _pline, std::vector< int> _vertices );  

template< typename PolyLineT >
inline
void unselectVertices(PolyLineT* _pline, std::vector< int> _vertices );
  
template< typename PolyLineT >
inline
void selectAllVertices(PolyLineT* _pline);

template< typename PolyLineT >
inline
void clearVertexSelection(PolyLineT* _pline);

template< typename PolyLineT >
inline      
void invertVertexSelection(PolyLineT* _pline);

template< typename PolyLineT >
inline    
std::vector< int > getVertexSelection(PolyLineT* _pline);

/** @} */  

//===========================================================================
/** @name Edge Selection
 * @{ */
//===========================================================================  

template< typename PolyLineT >
inline
void selectEdges(PolyLineT* _pline, std::vector< int> _edges );  

template< typename PolyLineT >
inline
void unselectEdges(PolyLineT* _pline, std::vector< int> _edges );

template< typename PolyLineT >
inline
void selectAllEdges(PolyLineT* _pline);

template< typename PolyLineT >
inline
void clearEdgeSelection(PolyLineT* _pline);

template< typename PolyLineT >
inline      
void invertEdgeSelection(PolyLineT* _pline);

template< typename PolyLineT >
inline    
std::vector< int > getEdgeSelection(PolyLineT* _pline);

/** @} */ 

//=============================================================================
} // PolyLineSelection Namespace 
//=============================================================================

#if defined(INCLUDE_TEMPLATES) && !defined(POLYLINESELECTION_C)
#define POLYLINESELECTION_TEMPLATES
#include "polyLineSelectionT.cc"
#endif
             
//=============================================================================
#endif // POLYLINESELECTION_HH defined
//=============================================================================
