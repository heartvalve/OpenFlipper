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

void selectVertices( SplatCloud *_splatCloud, std::vector<int> &_vertices )
{
	int n_vertices = (int) _splatCloud->numPoints();

	unsigned int i;
	for( i=0; i<_vertices.size(); ++i )
	{
		const int &v = _vertices[i];
		if( (v >= 0) && (v < n_vertices) )
			_splatCloud->selections()[ v ] = true;
	}
}


//----------------------------------------------------------------


void unselectVertices( SplatCloud *_splatCloud, std::vector<int> &_vertices )
{
	int n_vertices = (int) _splatCloud->numPoints();

	unsigned int i;
	for( i=0; i<_vertices.size(); ++i )
	{
		const int &v = _vertices[i];
		if( (v >= 0) && (v < n_vertices) )
			_splatCloud->selections()[ v ] = false;
	}
}


//----------------------------------------------------------------


void selectAllVertices( SplatCloud *_splatCloud )
{
	_splatCloud->setSelections( true );
}


//----------------------------------------------------------------


void clearVertexSelection( SplatCloud *_splatCloud )
{
	_splatCloud->setSelections( false );
}


//----------------------------------------------------------------


void invertVertexSelection( SplatCloud *_splatCloud )
{
	_splatCloud->invertSelections();
}


//----------------------------------------------------------------


std::vector<int> getVertexSelection( SplatCloud *_splatCloud )
{
	std::vector<int> selection;

	unsigned int i, num = _splatCloud->numPoints();
	for( i=0; i<num; ++i )
	{
		if( _splatCloud->selections()[ i ] )
			selection.push_back( i );
	}

	return selection;
}


//----------------------------------------------------------------


std::vector<int> getVertexSelection( SplatCloud *_splatCloud, bool &_invert)
{
	unsigned int numSelected = 0;

	unsigned int i, num = _splatCloud->numPoints();
	for( i=0; i<num; ++i )
	{
		if( _splatCloud->selections()[ i ] )
			++numSelected;
	}

	_invert = (numSelected > _splatCloud->numPoints() / 2);

	std::vector<int> selection;

	for( i=0; i<num; ++i )
	{
		if( _splatCloud->selections()[ i ] ^ _invert )
			selection.push_back( i );
	}

	return selection;
}


//=============================================================================


} // namespace SplatCloudSelection


//=============================================================================
