/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2010 by Computer Graphics Group, RWTH Aachen      *
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


//=============================================================================
//
//  BSplineCurveSelection - Implementation
//  Author:  Ellen Dekkers <dekkers@cs.rwth-aachen.de>
//
//=============================================================================


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
