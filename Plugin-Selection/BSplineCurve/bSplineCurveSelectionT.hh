
#ifndef BSPLINESELECTION_HH
#define BSPLINESELECTION_HH


/*! \file bSplineCurveSelection.hh
    \brief Functions for selection on a BSpline
    
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
  
template< typename BSplineT >
inline
void selectVertices(BSplineT* _bSpline, std::vector< int >& _vertices);

template< typename BSplineT >
inline
void unselectVertices(BSplineT* _bSpline, std::vector< int >& _vertices);
  
template< typename BSplineT >
inline
void selectAllVertices(BSplineT* _bSpline);

template< typename BSplineT >
inline
void clearVertexSelection(BSplineT* _bSpline);

template< typename BSplineT >
inline      
void invertVertexSelection(BSplineT* _bSpline);

template< typename BSplineT >
inline    
std::vector< int > getVertexSelection(BSplineT* _bSpline);

/** @} */  

//===========================================================================
/** @name Edge Selection
 * @{ */
//===========================================================================  

template< typename BSplineT >
inline
void selectEdges(BSplineT* _bSpline, std::vector< int >& _edges);

template< typename BSplineT >
inline
void unselectEdges(BSplineT* _bSpline, std::vector< int >& _edges);

template< typename BSplineT >
inline
void selectAllEdges(BSplineT* _bSpline);

template< typename BSplineT >
inline
void clearEdgeSelection(BSplineT* _bSpline);

template< typename BSplineT >
inline      
void invertEdgeSelection(BSplineT* _bSpline);


template< typename BSplineT >
inline    
std::vector< int > getEdgeSelection(BSplineT* _bSpline);

/** @} */ 

//=============================================================================
} // BSplineCurveSelection Namespace 
//=============================================================================

#if defined(INCLUDE_TEMPLATES) && !defined(BSPLINESELECTION_C)
#define BSPLINESELECTION_TEMPLATES
#include "bSplineCurveSelectionT.cc"
#endif
             
//=============================================================================
#endif // BSPLINESELECTION_HH defined
//=============================================================================
