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
//  CLASS SplatCloudObjectSelectionPlugin - IMPLEMENTATION (continued)
//
//================================================================


//== INCLUDES ====================================================


#include "SplatCloudObjectSelectionPlugin.hh"

#include "SplatCloudSelection.hh"

#include <set>


//== IMPLEMENTATION ==============================================


/** \brief Delete all selected elements of a SplatCloud
 *
 * @param _splatCloud a SplatCloud
 * @param _primitiveType bitmask of active primitive types
 * @return returns true if the SplatCloud was changed (i.e there was something selected that was deleted)
 */
bool SplatCloudObjectSelectionPlugin::splatCloudDeleteSelection( SplatCloud *_splatCloud, PrimitiveType _primitiveType )
{
	if( _primitiveType & vertexType_ )
	{
		_splatCloud->initSelections();
		return _splatCloud->deleteSelected();
	}

	return false;
}


//----------------------------------------------------------------


/** \brief Toggle the selection state of SplatCloud primitives
 *
 * @param _splatCloud a SplatCloud
 * @param _index index of vertex that was picked
 * @param _hit_point point that was picked
 * @param _primitiveType primitive types to be selected
 */
void SplatCloudObjectSelectionPlugin::splatCloudToggleSelection( SplatCloud *_splatCloud, uint _index, ACG::Vec3d &_hit_point, PrimitiveType _primitiveType )
{
	if( _primitiveType & vertexType_ )
	{
		_splatCloud->initSelections();
		_splatCloud->selections()[ _index ] = !_splatCloud->selections()[ _index ];

		if( _splatCloud->selections()[ _index ] )
			emit scriptInfo( "selectVertices(ObjectId , [" + QString::number( _index ) + "])" );
		else
			emit scriptInfo( "unselectVertices(ObjectId , [" + QString::number( _index ) + "])" );
    }
}


//----------------------------------------------------------------


/** \brief paint selection with a sphere
 *
 * @param _splatCloud a SplatCloud
 * @param _index index of vertex that was picked
 * @param _hit_point point that was picked
 * @param _radius selection sphere radius
 * @param _primitiveType primitive types to be selected
 * @param _deselection true, if entities should be deselected
 */
void SplatCloudObjectSelectionPlugin::splatCloudSphereSelection( SplatCloud *_splatCloud, uint _index, ACG::Vec3d &_hit_point, double _radius, PrimitiveType _primitiveType, bool _deselection )
{
	if( _primitiveType & vertexType_ )
	{
		_splatCloud->initSelections();
		_splatCloud->setSphereSelections( _splatCloud->points()[ _index ], _radius * _radius, !_deselection );
	}
}


//----------------------------------------------------------------


/** \brief Select all primitves that are projected to the given region
 *
 * @param _splatCloud a SplatCloud
 * @param _state current gl state
 * @param _region region
 * @param _primitiveType primitive types to be selected
 * @param _deselection true, if entities should be deselected
 * @return true, if something was selected
 */
bool SplatCloudObjectSelectionPlugin::splatCloudVolumeSelection( SplatCloud *_splatCloud, ACG::GLState &_state, QRegion *_region, PrimitiveType _primitiveType, bool _deselection )
{
	bool rv = false;

	if( _primitiveType & vertexType_ )
	{
		_splatCloud->initSelections();

		unsigned int i, num = _splatCloud->numPoints();
		for( i=0; i<num; ++i )
		{
			const SplatCloud::Point &p = _splatCloud->points()[ i ];
			ACG::Vec3d proj = _state.project( ACG::Vec3d( p[0], p[1], p[2] ) );

			if( _region->contains( QPoint( (int) proj[0], _state.context_height() - (int) proj[1] ) ) )
			{
				_splatCloud->selections()[ i ] = !_deselection;
				rv = true;
			}
		}
	}

	return rv;
}


//----------------------------------------------------------------


/** \brief Colorize a selection
 *
 * @param _splatCloud a SplatCloud
 * @param _primitiveTypes selection types
 * @param _r red   rgba color-component
 * @param _g green rgba color-component
 * @param _b blue  rgba color-component
 * @param _a blue  rgba color-component
 */
void SplatCloudObjectSelectionPlugin::splatCloudColorizeSelection( SplatCloud *_splatCloud, PrimitiveType _primitiveTypes, int _r, int _g, int _b, int _a )
{
	if( _primitiveTypes & vertexType_ )
	{
		_splatCloud->initSelections();
		_splatCloud->colorizeSelected( SplatCloud::Color( _r, _g, _b ) ); // alpha-component will be dropped
    }
}
