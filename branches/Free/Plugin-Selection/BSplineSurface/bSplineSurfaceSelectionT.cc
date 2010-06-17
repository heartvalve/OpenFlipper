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
//  BSplineSurface Selection - IMPLEMENTATION
//  Author:  Ellen Dekkers <dekkers@cs.rwth-aachen.de>
//
//=============================================================================


#define BSPLINESURFACESELECTION_C

//== INCLUDES =================================================================

#include "bSplineSurfaceSelectionT.hh"

//== NAMESPACES ===============================================================

namespace BSplineSurfaceSelection {

//== DEFINITIONS ==============================================================

//=========================================================
//== Vertex Selection =======================================
//=========================================================


template< typename BSplineSurfaceT >
inline
void
selectVertices(BSplineSurfaceT* _bSplineSurface, std::vector< int >& _vertices)
{
  const int max = _bSplineSurface->n_control_points_m() * _bSplineSurface->n_control_points_n();

  if (_bSplineSurface->controlpoint_selections_available())
  {
    for ( unsigned int i = 0 ; i < _vertices.size(); ++i )
    {
      if ( (_vertices[i] > 0) && ( _vertices[i] < max ) )
      {
        int idx_m = _vertices[i] / _bSplineSurface->n_control_points_n();
        int idx_n = _vertices[i] % _bSplineSurface->n_control_points_n();

        _bSplineSurface->controlpoint_selection(idx_m, idx_n) = true;
        // TODO try global idx selection
//         _bSplineSurface->vertex_selection(_vertices[i]) = true;
      }
    }
  }
}

//-----------------------------------------------------------------------------

template< typename BSplineSurfaceT >
inline
void
unselectVertices(BSplineSurfaceT* _bSplineSurface, std::vector< int >& _vertices)
{
  const int max = _bSplineSurface->n_control_points_m() * _bSplineSurface->n_control_points_n();

  if (_bSplineSurface->controlpoint_selections_available())
  {
    for ( unsigned int i = 0 ; i < _vertices.size(); ++i )
    {
      if ( (_vertices[i] > 0) && ( _vertices[i] < max ) )
      {
        int idx_m = _vertices[i] / _bSplineSurface->n_control_points_n();
        int idx_n = _vertices[i] % _bSplineSurface->n_control_points_n();

        _bSplineSurface->controlpoint_selection(idx_m, idx_n) = false;
        // TODO try global idx selection
//         _bSplineSurface->vertex_selection(_vertices[i]) = true;
      }
    }
  }
}

//-----------------------------------------------------------------------------

template< typename BSplineSurfaceT >
inline
void
selectAllVertices(BSplineSurfaceT* _bSplineSurface)
{
  if (_bSplineSurface->controlpoint_selections_available())
    for (unsigned int i = 0; i < _bSplineSurface->n_control_points_m(); ++i)
      for(unsigned int j = 0; j < _bSplineSurface->n_control_points_n(); ++j)
        _bSplineSurface->controlpoint_selection(i,j) = true;
}


//-----------------------------------------------------------------------------

template< typename BSplineSurfaceT >
inline
void
clearVertexSelection(BSplineSurfaceT* _bSplineSurface)
{
  if(_bSplineSurface->controlpoint_selections_available())
    for(unsigned int i = 0; i < _bSplineSurface->n_control_points_m(); ++i)
      for(unsigned int j = 0; j < _bSplineSurface->n_control_points_n(); ++j)
        _bSplineSurface->controlpoint_selection(i,j) = false;
}


//-----------------------------------------------------------------------------

template< typename BSplineSurfaceT >
inline
void
invertVertexSelection(BSplineSurfaceT* _bSplineSurface)
{
  if(_bSplineSurface->controlpoint_selections_available())
    for(unsigned int i = 0; i < _bSplineSurface->n_control_points_m(); ++i)
      for(unsigned int j = 0; j < _bSplineSurface->n_control_points_n(); ++j)
        _bSplineSurface->controlpoint_selection(i,j) = !_bSplineSurface->controlpoint_selection(i,j);
}

//-----------------------------------------------------------------------------

template< typename BSplineSurfaceT >
inline
std::vector< int >
getVertexSelection(BSplineSurfaceT* _bSplineSurface)
{
  std::vector< int > selection;
  if( ! _bSplineSurface->controlpoint_selections_available() )
    return selection;

  for (unsigned int i = 0; i < _bSplineSurface->n_control_points_m(); ++i)
  {
    for (unsigned int j = 0; j < _bSplineSurface->n_control_points_n(); ++j)
    {
      int idx = i * _bSplineSurface->n_control_points_n() + j;
      if (_bSplineSurface->controlpoint_selection(i,j) )
        selection.push_back(idx);
    }
  }

  return selection;
}


//=========================================================
//== Knot Selection =======================================
//=========================================================





//=========================================================
//== Edge Selection =======================================
//=========================================================

template< typename BSplineSurfaceT >
inline
void
selectEdges(BSplineSurfaceT* _bSplineSurface, std::vector< int >& _edges)
{
  const int max = _bSplineSurface->n_control_points_m() * _bSplineSurface->n_control_points_n();

//   if(_bSplineSurface->edge_selections_available())
//     for ( unsigned int i = 0 ; i < _edges.size(); ++i )
//       if ( (_edges[i] > 0) && ( _edges[i] < max ) )
//         _bSplineSurface->edge_selection(_edges[i]) = true;
}

//-----------------------------------------------------------------------------

template< typename BSplineSurfaceT >
inline
void
unselectEdges(BSplineSurfaceT* _bSplineSurface, std::vector< int >& _edges)
{
  const int max = _bSplineSurface->n_control_points_m() * _bSplineSurface->n_control_points_n();

//   if(_bSplineSurface->edge_selections_available())
//     for ( unsigned int i = 0 ; i < _edges.size(); ++i )
//       if ( (_edges[i] > 0) && ( _edges[i] < max ) )
//         _bSplineSurface->edge_selection(_edges[i]) = false;
}

//-----------------------------------------------------------------------------

template< typename BSplineSurfaceT >
inline
void
selectAllEdges(BSplineSurfaceT* _bSplineSurface)
{
//   if(_bSplineSurface->edge_selections_available())
//     for(unsigned int i = 0; i < _bSplineSurface->n_control_points_m(); ++i)
//       for(unsigned int j = 0; j < _bSplineSurface->n_control_points_n(); ++j)
//         _bSplineSurface->edge_selection(i,j) = true;
}

//-----------------------------------------------------------------------------

template< typename BSplineSurfaceT >
inline
void
clearEdgeSelection(BSplineSurfaceT* _bSplineSurface)
{
//   if(_bSplineSurface->edge_selections_available())
//     for(unsigned int i = 0; i < _bSplineSurface->n_control_points_m(); ++i)
//       for(unsigned int j = 0; j < _bSplineSurface->n_control_points_n(); ++j)
//         _bSplineSurface->edge_selection(i,j) = false;
}

//-----------------------------------------------------------------------------

template< typename BSplineSurfaceT >
inline
void
invertEdgeSelection(BSplineSurfaceT* _bSplineSurface)
{
//   if(_bSplineSurface->edge_selections_available())
//     for(unsigned int i = 0; i < _bSplineSurface->n_control_points_m(); ++i)
//       for(unsigned int j = 0; j < _bSplineSurface->n_control_points_n(); ++j)
//         _bSplineSurface->edge_selection(i,j) = !_bSplineSurface->edge_selection(i,j);
}

//-----------------------------------------------------------------------------

template< typename BSplineSurfaceT >
inline
std::vector< int > getEdgeSelection(BSplineSurfaceT* _bSplineSurface)
{
  std::vector< int > selection;
  if( ! _bSplineSurface->edge_selections_available() )
    return selection;

//   for(unsigned int i = 0; i < _bSplineSurface->n_control_points_m(); ++i)
//     for(unsigned int j = 0; j < _bSplineSurface->n_control_points_n(); ++j)
//       if (_bSplineSurface->edge_selection(i,j) )
//         selection.push_back(i);

  return selection;
}

//------------------------------------------------------------------------------

}
