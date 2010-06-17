#define BSPLINECURVESELECTION_C


//== INCLUDES =================================================================

#include "bSplineCurveSelectionT.hh"
#include "../Knotvector/knotvectorSelectionT.hh"

//== NAMESPACES ===============================================================

namespace BSplineCurveSelection {

//== DEFINITIONS ==============================================================

//=========================================================
//== Vertex Selection (selects control points) ============
//=========================================================  
  
  
template< typename BSplineCurveT >
inline
void 
selectVertices(BSplineCurveT* _bSplineCurve, std::vector< int >& _vertices) 
{
  const int max = _bSplineCurve->n_control_points();
  if(_bSplineCurve->controlpoint_selections_available())
    for ( unsigned int i = 0 ; i < _vertices.size(); ++i ) 
      if ( (_vertices[i] > 0) && ( _vertices[i] < max ) )
        _bSplineCurve->controlpoint_selection(_vertices[i]) = true;  
}
  
//-----------------------------------------------------------------------------    
  
template< typename BSplineCurveT >
inline
void 
unselectVertices(BSplineCurveT* _bSplineCurve, std::vector< int >& _vertices) 
{
  const int max = _bSplineCurve->n_control_points();
  if (_bSplineCurve->controlpoint_selections_available())
    for ( unsigned int i = 0 ; i < _vertices.size(); ++i ) 
      if ( (_vertices[i] > 0) && ( _vertices[i] < max ) )
        _bSplineCurve->controlpoint_selection(_vertices[i]) = false;  
}
  
//-----------------------------------------------------------------------------  
  
template< typename BSplineCurveT >
inline
void 
selectAllVertices(BSplineCurveT* _bSplineCurve) 
{
  if (_bSplineCurve->controlpoint_selections_available())
    for (unsigned int i = 0; i< _bSplineCurve->n_control_points(); ++i)
      _bSplineCurve->controlpoint_selection(i) = true;
}

//-----------------------------------------------------------------------------

template< typename BSplineCurveT >
inline
void 
clearVertexSelection(BSplineCurveT* _bSplineCurve)
{
  if (_bSplineCurve->controlpoint_selections_available())
    for(unsigned int i = 0; i < _bSplineCurve->n_control_points(); ++i)
      _bSplineCurve->controlpoint_selection(i) = false;
}

//-----------------------------------------------------------------------------

template< typename BSplineCurveT >
inline      
void 
invertVertexSelection(BSplineCurveT* _bSplineCurve)
{
  if (_bSplineCurve->controlpoint_selections_available())
    for (unsigned int i = 0; i < _bSplineCurve->n_control_points(); ++i)
      _bSplineCurve->controlpoint_selection(i) = !_bSplineCurve->controlpoint_selection(i);
}

//-----------------------------------------------------------------------------

template< typename BSplineCurveT >
inline    
std::vector< int > 
getVertexSelection(BSplineCurveT* _bSplineCurve) 
{
  std::vector< int > selection;  
  if ( !_bSplineCurve->controlpoint_selections_available() )
    return selection;
  
  for (unsigned int i = 0; i < _bSplineCurve->n_control_points(); ++i)
    if (_bSplineCurve->controlpoint_selection(i) )
      selection.push_back(i);
  
  return selection;
}


//=========================================================
//== Knot Selection =======================================
//=========================================================

  
template< typename BSplineCurveT >
inline
void 
selectKnots(BSplineCurveT* _bSplineCurve, std::vector< int >& _knots) 
{
  KnotvectorSelection::selectKnots(_bSplineCurve->get_knotvector_ref(), _knots);
}
  
//-----------------------------------------------------------------------------    
  
template< typename BSplineCurveT >
inline
void 
unselectKnots(BSplineCurveT* _bSplineCurve, std::vector< int >& _knots) 
{
  KnotvectorSelection::unselectKnots(_bSplineCurve->get_knotvector_ref(), _knots);
}
  
//-----------------------------------------------------------------------------  
  
template< typename BSplineCurveT >
inline
void 
selectAllKnots(BSplineCurveT* _bSplineCurve) 
{
  KnotvectorSelection::selectAllKnots(_bSplineCurve->get_knotvector_ref());
}

//-----------------------------------------------------------------------------

template< typename BSplineCurveT >
inline
void 
clearKnotSelection(BSplineCurveT* _bSplineCurve)
{
  KnotvectorSelection::clearKnotSelection(_bSplineCurve->get_knotvector_ref());
}

//-----------------------------------------------------------------------------

template< typename BSplineCurveT >
inline      
void 
invertKnotSelection(BSplineCurveT* _bSplineCurve)
{
  KnotvectorSelection::invertKnotSelection(_bSplineCurve->get_knotvector_ref());
}

//-----------------------------------------------------------------------------

template< typename BSplineCurveT >
inline    
std::vector< int > 
getKnotSelection(BSplineCurveT* _bSplineCurve) 
{
  std::vector< int > selection;  
  selection = KnotvectorSelection::getKnotSelection(_bSplineCurve->get_knotvector_ref());
  return selection;
}


//=========================================================
//== Edge Selection =======================================
//=========================================================

template< typename BSplineCurveT >
inline
void 
selectEdges(BSplineCurveT* _bSplineCurve, std::vector< int >& _edges) 
{
  const int max = _bSplineCurve->n_control_points();
  if (_bSplineCurve->edge_selections_available())
    for ( unsigned int i = 0 ; i < _edges.size(); ++i ) 
      if ( (_edges[i] > 0) && ( _edges[i] < max ) )
        _bSplineCurve->edge_selection(_edges[i]) = true;  
}
  
//-----------------------------------------------------------------------------    
  
template< typename BSplineCurveT >
inline
void 
unselectEdges(BSplineCurveT* _bSplineCurve, std::vector< int >& _edges) 
{
  const int max = _bSplineCurve->n_control_points();
  if (_bSplineCurve->edge_selections_available())
    for ( unsigned int i = 0 ; i < _edges.size(); ++i ) 
      if ( (_edges[i] > 0) && ( _edges[i] < max ) )
        _bSplineCurve->edge_selection(_edges[i]) = false;  
}
  
//-----------------------------------------------------------------------------  

template< typename BSplineCurveT >
inline
void 
selectAllEdges(BSplineCurveT* _bSplineCurve) 
{
  if (_bSplineCurve->edge_selections_available())
    for (unsigned int i = 0; i < _bSplineCurve->n_control_points(); ++i)
      _bSplineCurve->edge_selection(i) = true;
}

//-----------------------------------------------------------------------------

template< typename BSplineCurveT >
inline
void 
clearEdgeSelection(BSplineCurveT* _bSplineCurve)
{
  if (_bSplineCurve->edge_selections_available())
    for (unsigned int i = 0; i < _bSplineCurve->n_control_points(); ++i)
      _bSplineCurve->edge_selection(i) = false;
}

//-----------------------------------------------------------------------------

template< typename BSplineCurveT >
inline      
void 
invertEdgeSelection(BSplineCurveT* _bSplineCurve)
{
  if (_bSplineCurve->edge_selections_available())
    for (unsigned int i = 0; i < _bSplineCurve->n_control_points(); ++i)
      _bSplineCurve->edge_selection(i) = !_bSplineCurve->edge_selection(i);
}

//-----------------------------------------------------------------------------

template< typename BSplineCurveT >
inline    
std::vector< int > 
getEdgeSelection(BSplineCurveT* _bSplineCurve) 
{
  std::vector< int > selection;  
  if (!_bSplineCurve->edge_selections_available())
    return selection;
  
  for (unsigned int i= 0 ; i < _bSplineCurve->n_control_points(); ++i)
    if (_bSplineCurve->edge_selection(i) )
      selection.push_back(i);
  
  return selection;
}

//------------------------------------------------------------------------------

}
