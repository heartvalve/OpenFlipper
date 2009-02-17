#define POLYLINESELECTION_C


//== INCLUDES =================================================================

#include "polyLineSelectionT.hh"

//== NAMESPACES ===============================================================

namespace PolyLineSelection {

//== DEFINITIONS ==============================================================

//=========================================================
//== Vertex Selection =======================================
//=========================================================  
  
template< typename PolyLineT >
inline
void selectVertices(PolyLineT* _pline, std::vector< int > _vertices ) {
  const int max = _pline->n_vertices();
  if(_pline->vertex_selections_available())
    for ( unsigned int i = 0 ; i < _vertices.size(); ++i ) 
      if ( (_vertices[i] > 0) && ( _vertices[i] < max ) )
        _pline->vertex_selection(_vertices[i]) = true;  
}
  
//-----------------------------------------------------------------------------
  
template< typename PolyLineT >
inline
void unselectVertices(PolyLineT* _pline, std::vector< int > _vertices ) {
  const int max = _pline->n_vertices();
  if(_pline->vertex_selections_available())
    for ( unsigned int i = 0 ; i < _vertices.size(); ++i ) 
      if ( (_vertices[i] > 0) && ( _vertices[i] < max ) )
        _pline->vertex_selection(_vertices[i]) = false;  
}
  
//-----------------------------------------------------------------------------
  
template< typename PolyLineT >
inline
void selectAllVertices(PolyLineT* _pline) 
{
  if(_pline->vertex_selections_available())
    for(unsigned int i=0; i< _pline->n_vertices(); ++i)
      _pline->vertex_selection(i) = true;
}


//-----------------------------------------------------------------------------

template< typename PolyLineT >
inline
void clearVertexSelection(PolyLineT* _pline)
{
  if(_pline->vertex_selections_available())
    for(unsigned int i=0; i< _pline->n_vertices(); ++i)
      _pline->vertex_selection(i) = false;
}


//-----------------------------------------------------------------------------

template< typename PolyLineT >
inline      
void invertVertexSelection(PolyLineT* _pline)
{
  if(_pline->vertex_selections_available())
    for(unsigned int i=0; i< _pline->n_vertices(); ++i)
      _pline->vertex_selection(i) = !_pline->vertex_selection(i);
}

//-----------------------------------------------------------------------------

template< typename PolyLineT >
inline    
std::vector< int > getVertexSelection(PolyLineT* _pline) {
  std::vector< int > selection;  
  if( ! _pline->vertex_selections_available() )
    return selection;
  
  for(unsigned int i=0; i< _pline->n_vertices(); ++i)
    if (_pline->vertex_selection(i) )
      selection.push_back(i);
  
  return selection;
}


//=========================================================
//== EdgeSelection =======================================
//=========================================================

template< typename PolyLineT >
inline
void selectEdges(PolyLineT* _pline, std::vector< int > _edges ) {
  const int max = _pline->n_edges();
  if(_pline->edge_selections_available())
    for ( unsigned int i = 0 ; i < _edges.size(); ++i ) 
      if ( (_edges[i] > 0) && ( _edges[i] < max ) )
        _pline->edge_selection(_edges[i]) = true;  
}
  
//-----------------------------------------------------------------------------
  
template< typename PolyLineT >
inline
void unselectEdges(PolyLineT* _pline, std::vector< int > _edges ) {
  const int max = _pline->n_edges();
  if(_pline->edge_selections_available())
    for ( unsigned int i = 0 ; i < _edges.size(); ++i ) 
      if ( (_edges[i] > 0) && ( _edges[i] < max ) )
        _pline->edge_selection(_edges[i]) = false;  
}
  
//-----------------------------------------------------------------------------

template< typename PolyLineT >
inline
void selectAllEdges(PolyLineT* _pline) 
{
  if(_pline->edge_selections_available())
    for(unsigned int i=0; i< _pline->n_edges(); ++i)
      _pline->edge_selection(i) = true;
}


//-----------------------------------------------------------------------------

template< typename PolyLineT >
inline
void clearEdgeSelection(PolyLineT* _pline)
{
  if(_pline->edge_selections_available())
    for(unsigned int i=0; i< _pline->n_edges(); ++i)
      _pline->edge_selection(i) = false;
}


//-----------------------------------------------------------------------------

template< typename PolyLineT >
inline      
void invertEdgeSelection(PolyLineT* _pline)
{
  if(_pline->edge_selections_available())
    for(unsigned int i=0; i< _pline->n_edges(); ++i)
      _pline->edge_selection(i) = !_pline->edge_selection(i);
}

//-----------------------------------------------------------------------------

template< typename PolyLineT >
inline    
std::vector< int > getEdgeSelection(PolyLineT* _pline) {
  std::vector< int > selection;  
  if( ! _pline->edge_selections_available() )
    return selection;
  
  for(unsigned int i=0; i< _pline->n_edges(); ++i)
    if (_pline->edge_selection(i) )
      selection.push_back(i);
  
  return selection;
}


}


//------------------------------------------------------------------------------



