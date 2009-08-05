//=============================================================================
//
//                               OpenFlipper
//        Copyright (C) 2008 by Computer Graphics Group, RWTH Aachen
//                           www.openflipper.org
//
//-----------------------------------------------------------------------------
//
//                                License
//
//  OpenFlipper is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  OpenFlipper is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License
//  along with OpenFlipper.  If not, see <http://www.gnu.org/licenses/>.
//
//-----------------------------------------------------------------------------
//
//   $Revision: 1720 $
//   $Author: moebius $
//   $Date: 2008-05-09 14:15:53 +0200 (Fri, 09 May 2008) $
//
//=============================================================================




//=============================================================================
//
//  Standard Functions
//
//=============================================================================

/**
 * \file PluginFunctionsPlane.hh
 * This file contains functions which can be used by plugins to access planes in the framework.
 */

//
#ifndef PLUGINFUNCTIONSPLANE_HH
#define PLUGINFUNCTIONSPLANE_HH

#include <OpenFlipper/common/Types.hh>

/** The Namespace PluginFunctions contains functions for all plugins. */
namespace PluginFunctions {


/** \brief Get a pointer to every Plane which is marked as a source.
 *
 * @param _planes ( vector returning the source planes )
 * @return false, if no plane is selected as source
*/
DLLEXPORT
bool getSourcePlanes( std::vector<PlaneNode*>& _planes  );

/** \brief Get a pointer to every Plane which is marked as a target.
 *
 * @param _planes ( vector returning the target planes )
 * @return false, if no plane is selected as target
*/
DLLEXPORT
bool getTargetPlanes( std::vector<PlaneNode*>& _planes  );


/** This functions returns the object with the given id if it is a PlaneObject.
 * See get_object(  int _identifier , BaseObject*& _object ) for more details.
 */
DLLEXPORT
bool getObject(  int _identifier , PlaneObject*& _object );

/** \brief Get a PlaneNode from an object.
 *
 * @param _object The object should be of type BaseDataObject. If the content is a plane, a
 *                PlaneNode will be returned. Otherwise a NULL pointer is returned.
 */
DLLEXPORT
PlaneNode* planeNode( BaseObjectData* _object );

/** \brief Cast an BaseObject to a PlaneObject if possible
 *
 * @param _object The object should be of type BaseDataObject. If the content is a plane, a
 *                a PlaneObject is returned. Otherwise a NULL pointer is returned.
 */
DLLEXPORT
PlaneObject* planeObject( BaseObjectData* _object );

}

#endif // PLUGINFUNCTIONSPLANE_HH
