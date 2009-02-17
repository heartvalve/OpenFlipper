
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
