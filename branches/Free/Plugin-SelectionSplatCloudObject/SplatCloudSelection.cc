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
 *   $Revision$                                                      *
 *   $Author$                                                       *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/

//================================================================
//
//  SplatCloudSelection - IMPLEMENTATION
//
//================================================================


//== INCLUDES =================================================================


#include "SplatCloudSelection.hh"

#include <stack>


//== NAMESPACES ===============================================================


namespace SplatCloudSelection {


//== IMPLEMENTATION ==========================================================


//=========================================================
//== Vertex Selection =====================================
//=========================================================


void selectVertices( SplatCloud *_splatCloud, const std::vector<int> &_vertices )
{
  if( _splatCloud == 0 )
    return; // error

  if( _vertices.empty() )
    return; // done

  if( !_splatCloud->hasSelections() )
  {
    if( !_splatCloud->requestSelections() )
      return; // error

    unsigned int i, num = _splatCloud->numSplats();
    for( i=0; i<num; ++i )
      _splatCloud->selections( i ) = false;
  }

  int n_vertices = (int) _splatCloud->numSplats();

  unsigned int i;
  for( i=0; i<_vertices.size(); ++i )
  {
    int v = _vertices[ i ];
    if( (v >= 0) && (v < n_vertices) )
      _splatCloud->selections( v ) = true;
  }
}


//----------------------------------------------------------------


void unselectVertices( SplatCloud *_splatCloud, const std::vector<int> &_vertices )
{
  if( _splatCloud == 0 )
    return; // error

  if( _vertices.empty() )
    return; // done

  if( !_splatCloud->hasSelections() )
    return; // done

  int n_vertices = (int) _splatCloud->numSplats();

  unsigned int i;
  for( i=0; i<_vertices.size(); ++i )
  {
    int v = _vertices[ i ];
    if( (v >= 0) && (v < n_vertices) )
      _splatCloud->selections( v ) = false;
  }
}


//----------------------------------------------------------------


void selectAllVertices( SplatCloud *_splatCloud )
{
  if( _splatCloud == 0 )
    return; // error

  if( !_splatCloud->requestSelections() )
    return; // error

  unsigned int i, num = _splatCloud->numSplats();
  for( i=0; i<num; ++i )
    _splatCloud->selections( i ) = true;
}


//----------------------------------------------------------------


void clearVertexSelection( SplatCloud *_splatCloud )
{
  if( _splatCloud == 0 )
    return; // error

  if( !_splatCloud->hasSelections() )
    return; // done

  unsigned int i, num = _splatCloud->numSplats();
  for( i=0; i<num; ++i )
    _splatCloud->selections( i ) = false;
}


//----------------------------------------------------------------


void invertVertexSelection( SplatCloud *_splatCloud )
{
  if( _splatCloud == 0 )
    return; // error

  if( _splatCloud->hasSelections() )
  {
    unsigned int i, num = _splatCloud->numSplats();
    for( i=0; i<num; ++i )
      _splatCloud->selections( i ) = !_splatCloud->selections( i );
  }
  else
  {
    if( !_splatCloud->requestSelections() )
      return; // error

    unsigned int i, num = _splatCloud->numSplats();
    for( i=0; i<num; ++i )
      _splatCloud->selections( i ) = true;
  }
}


//----------------------------------------------------------------


static unsigned int countSelected( const SplatCloud *_splatCloud )
{
  unsigned int count = 0;

  unsigned int i, num = _splatCloud->numSplats();
  for( i=0; i<num; ++i )
  {
    if( _splatCloud->selections( i ) )
      ++count;
  }

  return count;
}


//----------------------------------------------------------------


std::vector<int> getVertexSelection( const SplatCloud *_splatCloud )
{
  std::vector<int> vertices;

  if( _splatCloud == 0 )
    return vertices; // error

  if( !_splatCloud->hasSelections() )
    return vertices; // done

  unsigned int numSelected = countSelected( _splatCloud );

  vertices.reserve( numSelected );

  unsigned int i, num = _splatCloud->numSplats();
  for( i=0; i<num; ++i )
  {
    if( _splatCloud->selections( i ) )
      vertices.push_back( i );
  }

  return vertices;
}


//----------------------------------------------------------------


std::vector<int> getVertexSelection( const SplatCloud *_splatCloud, bool &_inverted )
{
  _inverted = false;

  std::vector<int> vertices;

  if( _splatCloud == 0 )
    return vertices; // error

  if( !_splatCloud->hasSelections() )
    return vertices; // done

  unsigned int numSelected   = countSelected( _splatCloud );
  unsigned int numUnselected = _splatCloud->numSplats() - numSelected;

  if( numSelected <= numUnselected )
  {
    vertices.reserve( numSelected );

    unsigned int i, num = _splatCloud->numSplats();
    for( i=0; i<num; ++i )
    {
      if( _splatCloud->selections( i ) )
        vertices.push_back( i );
    }
  }
  else
  {
    _inverted = true;

    vertices.reserve( numUnselected );

    unsigned int i, num = _splatCloud->numSplats();
    for( i=0; i<num; ++i )
    {
      if( !_splatCloud->selections( i ) )
        vertices.push_back( i );
    }
  }

  return vertices;
}


//=============================================================================


} // namespace SplatCloudSelection


//=============================================================================
