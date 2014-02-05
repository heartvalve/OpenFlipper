/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2014 by Computer Graphics Group, RWTH Aachen      *
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
 *   $Revision$                                                       *
 *   $Author$                                                      *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/

//=============================================================================
//
//  CLASS PolyLineCircleData - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================

#include "PolyLineBezierSplineData.hh"

PolyLineBezierSplineData::PolyLineBezierSplineData(unsigned int _meshIndex)
: meshIndex_(_meshIndex)
{
}

void PolyLineBezierSplineData::addInterpolatePoint(ACG::Vec3d _position, ACG::Vec3d _normal)
{
	InterpolatePoint p;
	p.position = _position;
	p.normal   = _normal;

	points_.push_back(p);
}

bool PolyLineBezierSplineData::finishSpline()
{
    if(points_.size() < 2)
        return false;

	handles_.clear();

	for(unsigned int i = 0; i + 1 < points_.size(); i++) {

		const ACG::Vec3d firstPoint = points_[i].position, sndPoint = points_[i + 1].position;
		double r = (firstPoint - sndPoint).norm() / 4.0;
		const ACG::Vec3d dir = sndPoint - firstPoint;
		const ACG::Vec3d ort0 = dir % points_[i].normal, ort1 = dir % points_[i + 1].normal;
		ACG::Vec3d f0 = ort0 % points_[i].normal, f1 = ort1 % points_[i + 1].normal;

		ACG::Vec3d near = firstPoint - f0.normalize() * r,
				       far  = sndPoint   + f1.normalize() * r;

		handles_.push_back(near);
		handles_.push_back(far);

	}

	//handles will be degenerate up to now
	for(unsigned int i = 1; i + 1 < handles_.size(); i += 2) {

		const ACG::Vec3d dir = (handles_[i + 1] - handles_[i]) / 2.0;

		InterpolatePoint& p = getInterpolatePoint(i);
		handles_[i + 1]     = p.position + dir;
		handles_[i]         = p.position - dir;
	}

	return true;
}

PolyLineBezierSplineData::InterpolatePoint& PolyLineBezierSplineData::getInterpolatePoint(unsigned int _handleIndex)
{
	if(!_handleIndex)
		return points_[0];
	else if(_handleIndex == handles_.size())
		return points_.back();
	else return points_[(_handleIndex - 1) / 2 + 1];
}
