//=============================================================================
//
//  BSplineSurface Selection
//
//  Author:  Ellen Dekkers <dekkers@cs.rwth-aachen.de>
//
//  $Date$
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
