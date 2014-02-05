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
// BSplineCurveTypes
// Author:  Ellen Dekkers <dekkers@cs.rwth-aachen.de>
//
//=============================================================================


/**
 * \file BSplineCurveTypes.hh
 * This file contains the required types and typedefs for using BSpline curves
 */

#ifndef BSPLINECURVE_TYPES_HH
#define BSPLINECURVE_TYPES_HH


//== INCLUDES =================================================================

#include <ObjectTypes/Knotvector/KnotvectorT.hh>
#include <ObjectTypes/BSplineCurve/BSplineCurveT.hh>

//== TYPEDEFS =================================================================

/// Simple name for knotvector
typedef  ACG::KnotvectorT< double > Knotvector;

/// Simple Name for BSpline curves
typedef  ACG::BSplineCurveT< ACG::Vec3d > BSplineCurve;

//== TYPEDEFS FOR SCENEGRAPH ===============================================


//=============================================================================
#endif // BSPLINECURVE_TYPES_HH defined
//=============================================================================
