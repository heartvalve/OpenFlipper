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
 * @param _r red rgb color-component
 * @param _g green rgb color-component
 * @param _b blue rgb color-component
 */
void SplatCloudObjectSelectionPlugin::splatCloudColorizeSelection( SplatCloud *_splatCloud, PrimitiveType _primitiveTypes, int _r, int _g, int _b, int _a )
{
	if( _primitiveTypes & vertexType_ )
	{
		_splatCloud->initSelections();
		_splatCloud->colorizeSelected( SplatCloud::Color( _r, _g, _b ) ); // alpha-component will be dropped
    }
}
