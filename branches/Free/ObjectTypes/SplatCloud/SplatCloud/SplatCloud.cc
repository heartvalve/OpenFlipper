/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2011 by Computer Graphics Group, RWTH Aachen       *
*                           www.openflipper.org                              *
*                                                                            *
*--------------------------------------------------------------------------- *
*  This file is part of OpenFlipper.                                         *
*                                                                            *
*  OpenFlipper is free software: you can redistribute it and/or modify       *
*  it under the terms of the GNU Lesser General Public License as            *
*  published by the Free Software Foundation, either version 3 of            *
*  the License, or (at your option) any later version with the               *
*  following exceptions:                                                     *
*                                                                            *
*  If other files instantiate templates or use macros                        *
*  or inline functions from this file, or you compile this file and          *
*  link it with other files to produce an executable, this file does         *
*  not by itself cause the resulting executable to be covered by the         *
*  GNU Lesser General Public License. This exception does not however        *
*  invalidate any other reasons why the executable file might be             *
*  covered by the GNU Lesser General Public License.                         *
*                                                                            *
*  OpenFlipper is distributed in the hope that it will be useful,            *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
*  GNU Lesser General Public License for more details.                       *
*                                                                            *
*  You should have received a copy of the GNU LesserGeneral Public           *
*  License along with OpenFlipper. If not,                                   *
*  see <http://www.gnu.org/licenses/>.                                       *
*                                                                            *
\*===========================================================================*/

/*===========================================================================*\
*                                                                            *
*   $Revision$                                                       *
*   $LastChangedBy$                                                *
*   $Date$                     *
*                                                                            *
\*===========================================================================*/

//================================================================
//
//  CLASS SplatCloud - IMPLEMENTATION
//
//================================================================


//== INCLUDES ====================================================


#include "SplatCloud.hh"


//== IMPLEMENTATION ==============================================


unsigned int SplatCloud::countSelected() const
{
	unsigned int numSelected = 0;

	SelectionVector::const_iterator selectionIter;
	for( selectionIter = selections_.begin(); selectionIter != selections_.end(); ++selectionIter )
	{
		if( *selectionIter )
			++numSelected;
	}

	return numSelected;
}


//----------------------------------------------------------------


void SplatCloud::setSelections( const Selection &_selection )
{
	// select all entries...
	SelectionVector::iterator selectionIter;
	for( selectionIter = selections_.begin(); selectionIter != selections_.end(); ++selectionIter )
		(*selectionIter) = _selection;
}


//----------------------------------------------------------------


void SplatCloud::setSphereSelections( const Point &_center, float _sqRadius, const Selection &_selection )
{
	// set all selections within given radius from center
	PointVector::const_iterator pointIter     = points_.begin();
	SelectionVector::iterator   selectionIter = selections_.begin();
	for( ; pointIter != points_.end(); ++pointIter, ++selectionIter )
	{
		const Point &point = *pointIter;

		float dx = point[0] - _center[0];
		float dy = point[1] - _center[1];
		float dz = point[2] - _center[2];

		if( (dx*dx + dy*dy + dz*dz) <= _sqRadius )
			*selectionIter = _selection;
	}
}


//----------------------------------------------------------------


void SplatCloud::invertSelections()
{
	// invert all entries...
	SelectionVector::iterator selectionIter;
	for( selectionIter = selections_.begin(); selectionIter != selections_.end(); ++selectionIter )
		(*selectionIter) = !(*selectionIter);
}


//----------------------------------------------------------------


bool SplatCloud::deleteSelected()
{
	// count number of selected points
	unsigned int numSelected = countSelected();

	// if no point selected, abort
	if( numSelected == 0 )
		return false; // nothing has been modified

	unsigned int newSize = numPoints() - numSelected;

	bool hasNrm = hasNormals();
	bool hasPS  = hasPointsizes();
	bool hasCol = hasColors();
	bool hasIdx = hasIndices();
	bool hasSel = hasSelections();

	// create new (empty) data vectors
	PointVector     newPoints;
	NormalVector    newNormals;
	PointsizeVector newPointsizes;
	ColorVector     newColors;
	IndexVector     newIndices;
	SelectionVector newSelections;

	// reserve memory/space if data vector(s) in use
	/*        */ newPoints.reserve    ( newSize );
	if( hasNrm ) newNormals.reserve   ( newSize );
	if( hasPS  ) newPointsizes.reserve( newSize );
	if( hasCol ) newColors.reserve    ( newSize );
	if( hasIdx ) newIndices.reserve   ( newSize );
	if( hasSel ) newSelections.reserve( newSize );

	PointVector::const_iterator     pointIter     = points_.begin();
	NormalVector::const_iterator    normalIter    = normals_.begin();
	PointsizeVector::const_iterator pointsizeIter = pointsizes_.begin();
	ColorVector::const_iterator     colorIter     = colors_.begin();
	IndexVector::const_iterator     indexIter     = indices_.begin();
	SelectionVector::const_iterator selectionIter = selections_.begin();

	// add old data entry to new data vector if point is *not* selected
	while( pointIter != points_.end() )
	{
		bool unselected = !(*selectionIter);

		{
			if( unselected )
				newPoints.push_back    ( *pointIter     );
			++pointIter;
		}

		if( hasNrm )
		{
			if( unselected )
				newNormals.push_back   ( *normalIter    );
			++normalIter;
		}

		if( hasPS  )
		{
			if( unselected )
				newPointsizes.push_back( *pointsizeIter );
			++pointsizeIter;
		}

		if( hasCol )
		{
			if( unselected )
				newColors.push_back    ( *colorIter     );
			++colorIter;
		}

		if( hasIdx )
		{
			if( unselected )
				newIndices.push_back   ( *indexIter     );
			++indexIter;
		}

		if( hasSel )
		{
			if( unselected )
				newSelections.push_back( *selectionIter );
			++selectionIter;
		}
	}

	// replace old data vectors by new ones (even when data vector was *not* in use, so new vector has the right size)
	points_     = newPoints;
	normals_    = newNormals;
	pointsizes_ = newPointsizes;
	colors_     = newColors;
	indices_    = newIndices;
	selections_ = newSelections;

	return true; // data has been modified
}


//----------------------------------------------------------------


bool SplatCloud::colorizeSelected( const Color &_color )
{
	// if colors_ vector is *not* of the right size, resize
	if( numColors() != numPoints() )
		colors_.resize( numPoints(), Color(255,255,255) ); // initialize with white color

	bool modified = false;

	ColorVector::iterator     colorIter     = colors_.begin();
	SelectionVector::iterator selectionIter = selections_.begin();

	// delete all selected entries (from selections_ vector and as well from points_, normals_, pointsizes_ and colors_ vectors)
	while( selectionIter != selections_.end() )
	{
		if( *selectionIter )
		{
			(*colorIter) = _color;
			modified = true;
		}

		++colorIter;
		++selectionIter;
	}

	return modified;
}


//----------------------------------------------------------------


void SplatCloud::normalizeSize()
{
	// check if there is nothing to do
	if( !hasPoints() )
		return;

	// calculate center-of-gravety
	float cogX = 0.0f;
	float cogY = 0.0f;
	float cogZ = 0.0f;
	PointVector::iterator pointIter;
	for( pointIter = points_.begin(); pointIter != points_.end(); ++pointIter )
	{
		const Point &p = *pointIter;

		cogX += p[0];
		cogY += p[1];
		cogZ += p[2];
	}

	float rcp_count = 1.0f / (float) numPoints();
	cogX *= rcp_count;
	cogY *= rcp_count;
	cogZ *= rcp_count;

	translation_ = Point( -cogX, -cogY, -cogZ );
	translate( translation_ );
	std::cout << "SplatCloud::normalizeSize(): translating points by: " << translation_ << std::endl;

	// calculate squared length
	float sqLength = 0.0f;
	for( pointIter = points_.begin(); pointIter != points_.end(); ++pointIter )
	{
		const Point &p = *pointIter;
		sqLength += p[0]*p[0] + p[1]*p[1] + p[2]*p[2];
	}

	float s = (float) sqrt( sqLength * rcp_count );

	if( s == 0.0f )
	  return;

	scaleFactor_ = 1.0f / s;
	scale( scaleFactor_ );
	std::cout << "SplatCloud::normalizeSize(): scaling points by factor: " << scaleFactor_ << std::endl;
}


//----------------------------------------------------------------


void SplatCloud::translate( const Point &_t )
{
	// translate points
	PointVector::iterator pointIter;
	for( pointIter = points_.begin(); pointIter != points_.end(); ++pointIter )
	{
		Point &p = *pointIter;

		p[0] += _t[0];
		p[1] += _t[1];
		p[2] += _t[2];
	}
}


//----------------------------------------------------------------


void SplatCloud::scale( float _s )
{
	// scale points (and pointsizes as well)
	if( hasPointsizes() )
	{

		PointsizeVector::iterator pointsizeIter = pointsizes_.begin();
		PointVector::iterator     pointIter;
		for( pointIter = points_.begin(); pointIter != points_.end(); ++pointIter, ++pointsizeIter )
		{
			Point     &p  = *pointIter;
			Pointsize &ps = *pointsizeIter;

			p[0] *= _s;
			p[1] *= _s;
			p[2] *= _s;
			ps   *= _s; // scale pointsize as well
		}

	}
	else
	{

		PointVector::iterator pointIter;
		for( pointIter = points_.begin(); pointIter != points_.end(); ++pointIter )
		{
			Point &p = *pointIter;

			p[0] *= _s;
			p[1] *= _s;
			p[2] *= _s;
		}

	}	
}
