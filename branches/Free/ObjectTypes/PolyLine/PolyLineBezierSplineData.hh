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

#pragma once

#include <OpenFlipper/common/GlobalDefines.hh>
#include <OpenFlipper/common/perObjectData.hh>
#include <ACG/Math/VectorT.hh>
#include <vector>

#define BEZSPLINE_DATA "BezierSpline_Data"

class DLLEXPORT PolyLineBezierSplineData : public PerObjectData
{

public:

	struct InterpolatePoint {
		ACG::Vec3d position;
		ACG::Vec3d normal;
	};

	std::vector<InterpolatePoint> points_;
	std::vector<ACG::Vec3d>       handles_;
    /// Index of the corresponding mesh
	unsigned int meshIndex_;

public:

  /** \brief Creates a new PolyLineBezierSplineData object with no points.
   *
   */
  PolyLineBezierSplineData(unsigned int _meshIndex);

	/** \brief Adds a point to the end of the list and inserts control points.
	 *
	 * @param _position The location(on the mesh) of the new control point.
	 * @param _normal The normal at the location.
	 */
	void addInterpolatePoint(ACG::Vec3d _position, ACG::Vec3d _normal);

	/** \brief If possible calculates handles.
	 *
	 * @return True if it was possible to create handles.
	 */
	bool finishSpline();

	/** \brief Retrieves the interpolate point based on the handle.
	 *
	 * @param _handleIndex The index of the handle.
	 */
	InterpolatePoint& getInterpolatePoint(unsigned int _handleIndex);
};
