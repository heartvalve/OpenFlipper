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
 * \file PluginFunctionsLight.hh
 * This file contains functions which can be used by plugins to access lights in the framework.
 */

//
#ifndef PLUGINFUNCTIONSLIGHT_HH
#define PLUGINFUNCTIONSLIGHT_HH

#include <OpenFlipper/common/Types.hh>
#include "Light.hh"


/** The Namespace PluginFunctions contains functions for all plugins. */
namespace PluginFunctions {

/** This functions returns the object with the given id if it is a LightObject.
 * See get_object(  int _identifier , BaseObject*& _object ) for more details.
 */
DLLEXPORT
bool getObject(  int _identifier , LightObject*& _object );

/** \brief Get a LightNode from an object.
 *
 * @param _object The object should be of type BaseDataObject. If the content is a light, a
 *                LightNode will be returned. Otherwise a NULL pointer is returned.
 */
DLLEXPORT
LightNode* lightNode( BaseObjectData* _object );

/** \brief Cast an BaseObject to a LightObject if possible
 *
 * @param _object The object should be of type BaseDataObject. If the content is a light, a
 *                a LightObject is returned. Otherwise a NULL pointer is returned.
 */
DLLEXPORT
LightObject* lightObject( BaseObjectData* _object );

}

#endif // PLUGINFUNCTIONSLIGHT_HH
