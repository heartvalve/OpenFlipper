
#ifndef KNOTVECTORSELECTION_HH
#define KNOTVECTPRSELECTION_HH


/*! \file knotvectorSelectionT.hh
    \brief Functions for selections on a knotvector.
    
*/

//== INCLUDES =================================================================

#include <vector>

//== NAMESPACES ===============================================================

namespace KnotvectorSelection {

//== DEFINITIONS ==============================================================

//===========================================================================
/** @name Knot Selection
 * @{ */
//===========================================================================    
  
template< typename KnotvectorT >
inline
void selectKnots(KnotvectorT* _knotvector, std::vector< int >& _knots);

template< typename KnotvectorT >
inline
void unselectKnots(KnotvectorT* _knotvector, std::vector< int >& _knots);
  
template< typename KnotvectorT >
inline
void selectAllKnots(KnotvectorT* _knotvector);

template< typename KnotvectorT >
inline
void clearKnotSelection(KnotvectorT* _knotvector);

template< typename KnotvectorT >
inline      
void invertKnotSelection(KnotvectorT* _knotvector);

template< typename KnotvectorT >
inline    
std::vector< int > getKnotSelection(KnotvectorT* _knotvector);

/** @} */  

// helper function
template< typename KnotvectorT >
inline
int closestKnot(KnotvectorT * _knotvector, double _param);

//=============================================================================
} // KnotvectorSelection Namespace 
//=============================================================================

#if defined(INCLUDE_TEMPLATES) && !defined(KNOTVECTORSELECTION_C)
#define KNOTVECTORSELECTION_TEMPLATES
#include "knotvectorSelectionT.cc"
#endif
             
//=============================================================================
#endif // KNOTVECTORSELECTION_HH defined
//=============================================================================
