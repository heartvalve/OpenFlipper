#define KNOTVECTORSELECTION_C


//== INCLUDES =================================================================

#include "knotvectorSelectionT.hh"

//== NAMESPACES ===============================================================

namespace KnotvectorSelection {

//== DEFINITIONS ==============================================================

//=========================================================
//== Knot Selection =======================================
//=========================================================  
  
  
template< typename KnotvectorT >
inline
void 
selectKnots(KnotvectorT* _knotvector, std::vector< int >& _knots) 
{
  const int max = _knotvector->size();
  if(_knotvector->selections_available())
    for ( unsigned int i = 0 ; i < _knots.size(); ++i ) 
      if ( (_knots[i] > 0) && ( _knots[i] < max ) )
        _knotvector->selection(_knots[i]) = true;  
}
  
//-----------------------------------------------------------------------------    
  
template< typename KnotvectorT >
inline
void 
unselectKnots(KnotvectorT* _knotvector, std::vector< int >& _knots) 
{
  const int max = _knotvector->size();
  if(_knotvector->selections_available())
    for ( unsigned int i = 0 ; i < _knots.size(); ++i ) 
      if ( (_knots[i] > 0) && ( _knots[i] < max ) )
        _knotvector->selection(_knots[i]) = false;  
}
  
//-----------------------------------------------------------------------------  
  
template< typename KnotvectorT >
inline
void 
selectAllKnots(KnotvectorT* _knotvector) 
{
  if(_knotvector->selections_available())
    for(unsigned int i = 0; i < _knotvector->size(); ++i)
      _knotvector->selection(i) = true;
}

//-----------------------------------------------------------------------------

template< typename KnotvectorT >
inline
void 
clearKnotSelection(KnotvectorT* _knotvector)
{
  if(_knotvector->selections_available())
    for(unsigned int i = 0; i < _knotvector->size(); ++i)
      _knotvector->selection(i) = false;
}

//-----------------------------------------------------------------------------

template< typename KnotvectorT >
inline      
void 
invertKnotSelection(KnotvectorT* _knotvector)
{
  if(_knotvector->selections_available())
    for(unsigned int i = 0; i < _knotvector->size(); ++i)
      _knotvector->selection(i) = !_knotvector->selection(i);
}

//-----------------------------------------------------------------------------

template< typename KnotvectorT >
inline    
std::vector< int > 
getKnotSelection(KnotvectorT* _knotvector) 
{
  std::vector< int > selection;  
  if( ! _knotvector->selections_available() )
    return selection;
  
  for(unsigned int i = 0; i < _knotvector->size(); ++i)
    if (_knotvector->selection(i) )
      selection.push_back(i);
  
  return selection;
}

//------------------------------------------------------------------------------

template< typename KnotvectorT >
inline    
int 
closestKnot(KnotvectorT * _knotvector, double _param)
{
  int closestIdx = -1;
  double minDist = DBL_MAX;
  
  // find the closest knot oin a knotvector for a given parameter
  for (unsigned int i = 0; i < _knotvector->size(); ++i)
  {
    double dist = fabs(_knotvector->getKnot(i) - _param);
    if (dist < minDist)
    {
      minDist = dist;
      closestIdx = i;
    }
  }
  return closestIdx;
}

//------------------------------------------------------------------------------

} // end of namespace KnotvectorSelection
