/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2014 by Computer Graphics Group, RWTH Aachen       *
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


//=============================================================================
//
// BSPlineCurveIncludes
// Author:  Ellen Dekkers <dekkers@cs.rwth-aachen.de>
//
//=============================================================================


/**
 * \file BSplineCurve.hh
 * This File contains all required includes for BSpline Curves
*/

#ifndef BSPLINECURVE_INCLUDE_HH
#define BSPLINECURVE_INCLUDE_HH


//== INCLUDES =================================================================

/** \def DATA_BSPLINE_CURVE
 * Use this macro to reference B-Spline curves
 */
#define DATA_BSPLINE_CURVE typeId("BSplineCurve")

#include <ObjectTypes/BSplineCurve/BSplineCurveT.hh>
#include <ObjectTypes/BSplineCurve/BSplineCurveObject.hh>
#include <ObjectTypes/BSplineCurve/PluginFunctionsBSplineCurve.hh>
#include <ObjectTypes/BSplineCurve/BSplineCurveTypes.hh>


//=============================================================================
#endif // BSPLINECURVE_INCLUDE_HH defined
//=============================================================================
