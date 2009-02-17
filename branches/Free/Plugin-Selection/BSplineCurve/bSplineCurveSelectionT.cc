#define BSPLINESELECTION_C


//== INCLUDES =================================================================

#include "bSplineCurveSelectionT.hh"

//== NAMESPACES ===============================================================

namespace BSplineCurveSelection {

//== DEFINITIONS ==============================================================

//=========================================================
//== Vertex Selection =======================================
//=========================================================  
  
  
template< typename BSplineT >
inline
void selectVertices(BSplineT* _bSpline, std::vector< int >& _vertices) {
  const int max = _bSpline->n_control_points();
  if(_bSpline->vertex_selections_available())
    for ( unsigned int i = 0 ; i < _vertices.size(); ++i ) 
      if ( (_vertices[i] > 0) && ( _vertices[i] < max ) )
        _bSpline->vertex_selection(_vertices[i]) = true;  
}
  
//-----------------------------------------------------------------------------    
  
template< typename BSplineT >
inline
void unselectVertices(BSplineT* _bSpline, std::vector< int >& _vertices) {
  const int max = _bSpline->n_control_points();
  if(_bSpline->vertex_selections_available())
    for ( unsigned int i = 0 ; i < _vertices.size(); ++i ) 
      if ( (_vertices[i] > 0) && ( _vertices[i] < max ) )
        _bSpline->vertex_selection(_vertices[i]) = false;  
}
  
//-----------------------------------------------------------------------------  
  
template< typename BSplineT >
inline
void selectAllVertices(BSplineT* _bSpline) 
{
  if(_bSpline->vertex_selections_available())
    for(unsigned int i=0; i< _bSpline->n_control_points(); ++i)
      _bSpline->vertex_selection(i) = true;
}


//-----------------------------------------------------------------------------

template< typename BSplineT >
inline
void clearVertexSelection(BSplineT* _bSpline)
{
  if(_bSpline->vertex_selections_available())
    for(unsigned int i=0; i< _bSpline->n_control_points(); ++i)
      _bSpline->vertex_selection(i) = false;
}


//-----------------------------------------------------------------------------

template< typename BSplineT >
inline      
void invertVertexSelection(BSplineT* _bSpline)
{
  if(_bSpline->vertex_selections_available())
    for(unsigned int i=0; i< _bSpline->n_control_points(); ++i)
      _bSpline->vertex_selection(i) = !_bSpline->vertex_selection(i);
}

//-----------------------------------------------------------------------------

template< typename BSplineT >
inline    
std::vector< int > getVertexSelection(BSplineT* _bSpline) {
  std::vector< int > selection;  
  if( ! _bSpline->vertex_selections_available() )
    return selection;
  
  for(unsigned int i=0; i< _bSpline->n_control_points(); ++i)
    if (_bSpline->vertex_selection(i) )
      selection.push_back(i);
  
  return selection;
}


//=========================================================
//== EdgeSelection =======================================
//=========================================================

template< typename BSplineT >
inline
void selectEdges(BSplineT* _bSpline, std::vector< int >& _edges) {
  const int max = _bSpline->n_control_points();
  if(_bSpline->edge_selections_available())
    for ( unsigned int i = 0 ; i < _edges.size(); ++i ) 
      if ( (_edges[i] > 0) && ( _edges[i] < max ) )
        _bSpline->edge_selection(_edges[i]) = true;  
}
  
//-----------------------------------------------------------------------------    
  
template< typename BSplineT >
inline
void unselectEdges(BSplineT* _bSpline, std::vector< int >& _edges) {
  const int max = _bSpline->n_control_points();
  if(_bSpline->edge_selections_available())
    for ( unsigned int i = 0 ; i < _edges.size(); ++i ) 
      if ( (_edges[i] > 0) && ( _edges[i] < max ) )
        _bSpline->edge_selection(_edges[i]) = false;  
}
  
//-----------------------------------------------------------------------------  

template< typename BSplineT >
inline
void selectAllEdges(BSplineT* _bSpline) 
{
  if(_bSpline->edge_selections_available())
    for(unsigned int i=0; i< _bSpline->n_control_points(); ++i)
      _bSpline->edge_selection(i) = true;
}


//-----------------------------------------------------------------------------

template< typename BSplineT >
inline
void clearEdgeSelection(BSplineT* _bSpline)
{
  if(_bSpline->edge_selections_available())
    for(unsigned int i=0; i< _bSpline->n_control_points(); ++i)
      _bSpline->edge_selection(i) = false;
}


//-----------------------------------------------------------------------------

template< typename BSplineT >
inline      
void invertEdgeSelection(BSplineT* _bSpline)
{
  if(_bSpline->edge_selections_available())
    for(unsigned int i=0; i< _bSpline->n_control_points(); ++i)
      _bSpline->edge_selection(i) = !_bSpline->edge_selection(i);
}

//-----------------------------------------------------------------------------

template< typename BSplineT >
inline    
std::vector< int > getEdgeSelection(BSplineT* _bSpline) {
  std::vector< int > selection;  
  if( ! _bSpline->edge_selections_available() )
    return selection;
  
  for(unsigned int i=0; i< _bSpline->n_control_points(); ++i)
    if (_bSpline->edge_selection(i) )
      selection.push_back(i);
  
  return selection;
}


}


//------------------------------------------------------------------------------



