/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2009 by Computer Graphics Group, RWTH Aachen      *
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
 *   $Revision: 17373 $                                                       *
 *   $Author: moebius $                                                      *
 *   $Date: 2013-08-22 17:54:51 +0200 (Thu, 22 Aug 2013) $                   *
 *                                                                           *
\*===========================================================================*/

//=============================================================================
//
//  CLASS PolyLineCircleData - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================

#include "PolyLineBezierSplineData.hh"

void PolyLineBezierSplineData::addInterpolatePoint(ACG::Vec3d _pos, ACG::Vec3d _nor)
{
	InterpolatePoint p;
	p.Pos_ = _pos;
	p.Nor_ = _nor;
	Points_.push_back(p);
}

bool PolyLineBezierSplineData::finishSpline()
{
	Handles_.clear();
	for(unsigned int i = 0; i < Points_.size() - 1; i++) {
		const ACG::Vec3d firstPoint = Points_[i].Pos_, sndPoint = Points_[i + 1].Pos_;
		double r = (firstPoint - sndPoint).norm() / 4.0;
		const ACG::Vec3d dir = sndPoint - firstPoint;
		const ACG::Vec3d ort0 = dir % Points_[i].Nor_, ort1 = dir % Points_[i + 1].Nor_;
		ACG::Vec3d f0 = ort0 % Points_[i].Nor_, f1 = ort1 % Points_[i + 1].Nor_;
		ACG::Vec3d near = firstPoint - f0.normalize() * r,
				   far  = sndPoint + f1.normalize() * r;
		Handles_.push_back(near);
		Handles_.push_back(far);
	}
	//handles will be degenerate up to now
	for(unsigned int i = 1; i < Handles_.size() - 1; i+=2) {
		ACG::Vec3d dir = (Handles_[i + 1] - Handles_[i]) / 2.0;
		InterpolatePoint& p = getInterpolatePoint(i);
		Handles_[i + 1] = p.Pos_ + dir;
		Handles_[i] = p.Pos_ - dir;
	}
	return true;
}

PolyLineBezierSplineData::InterpolatePoint& PolyLineBezierSplineData::getInterpolatePoint(unsigned int _handleIndex)
{
	if(!_handleIndex)
		return Points_[0];
	else if(_handleIndex == Handles_.size())
		return Points_.back();
	else return Points_[(_handleIndex - 1) / 2 + 1];
}
