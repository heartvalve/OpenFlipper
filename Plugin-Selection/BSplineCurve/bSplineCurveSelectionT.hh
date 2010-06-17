//=============================================================================
//
//  BSplineCurve Selection
//
//  Author:  Ellen Dekkers <dekkers@cs.rwth-aachen.de>
//
//  $Date$
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
