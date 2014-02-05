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
// PluginFunctions BSplineSurface
// Author:  Ellen Dekkers <dekkers@cs.rwth-aachen.de>
//
//=============================================================================


//=============================================================================
//
//  Standard Functions
//
//=============================================================================

/**
 * \file PluginFunctionsBSplineSurface.hh
 * This file contains functions which can be used by plugins to access bspline surfaces in the framework.
 */

//
#ifndef PLUGINFUNCTIONSBSPLINESURFACE_HH
#define PLUGINFUNCTIONSBSPLINESURFACE_HH

#include <OpenFlipper/common/Types.hh>

/** The Namespace PluginFunctions contains functions for all plugins.  */
namespace PluginFunctions {

/** This functions returns the object with the given id if it is a BSplineSurfaceObject.
  * See get_object(  int _identifier , BaseObject*& _object ) for more details.
  */
DLLEXPORT
bool getObject(  int _identifier , BSplineSurfaceObject*& _object );

/** \brief Get a Bspline Surface from an object.
  *
  * @param _object The object should be of type BaseDataObject. If the content is a BSpline Surface , a
  *               spline surface will be returned. Otherwise a NULL pointer is returned.
  */
DLLEXPORT
BSplineSurface* splineSurface( BaseObjectData* _object );

/** \brief Cast an BaseObject to a BSplineSurfaceObject if possible
  *
  * @param _object The object should be of type BaseDataObject. If the content is a bspline surface, a
  *                a BSplineSurfaceObject is returned. Otherwise a NULL pointer is returned.
  */
DLLEXPORT
BSplineSurfaceObject* bsplineSurfaceObject( BaseObjectData* _object);

}

#endif // PLUGINFUNCTIONSBSPLINESURFACE_HH
