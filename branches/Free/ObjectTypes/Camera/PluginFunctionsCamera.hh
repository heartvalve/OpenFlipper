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
//   $Revision$
//   $Author$
//   $Date$
//
//=============================================================================




//=============================================================================
//
//  Standard Functions
//
//=============================================================================

/**
 * \file PluginFunctionsCamera.hh
 * This file contains functions which can be used by plugins to access cameras in the framework.
 */

//
#ifndef PLUGINFUNCTIONSCAMERA_HH
#define PLUGINFUNCTIONSCAMERA_HH

#include <OpenFlipper/common/Types.hh>

/** The Namespace PluginFunctions contains functions for all plugins. */
namespace PluginFunctions {


/** \brief Get a pointer to every Camera which is marked as a source.
 *
 * @param _cameras ( vector returning the source cameras )
 * @return false, if no camera is selected as source
*/
DLLEXPORT
bool getSourceCameras( std::vector<CameraNode*>& _cameras  );

/** \brief Get a pointer to every Camera which is marked as a target.
 *
 * @param _cameras ( vector returning the target cameras )
 * @return false, if no camera is selected as target
*/
DLLEXPORT
bool getTargetCameras( std::vector<CameraNode*>& _cameras  );


/** This functions returns the object with the given id if it is a CameraObject.
 * See get_object(  int _identifier , BaseObject*& _object ) for more details.
 */
DLLEXPORT
bool getObject(  int _identifier , CameraObject*& _object );

/** \brief Get a CameraObject from an object id if possible
*
* @param _object If the object is a camera, a CameraObject is returned. Otherwise a NULL pointer is returned.
*/
DLLEXPORT
CameraObject* cameraObject( int _objectId );

/** \brief Get a CameraNode from an object.
 *
 * @param _object The object should be of type BaseDataObject. If the content is a camera, a
 *                CameraNode will be returned. Otherwise a NULL pointer is returned.
 */
DLLEXPORT
CameraNode* cameraNode( BaseObjectData* _object );

/** \brief Cast an BaseObject to a CameraObject if possible
 *
 * @param _object The object should be of type BaseDataObject. If the content is a camera, a
 *                a CameraObject is returned. Otherwise a NULL pointer is returned.
 */
DLLEXPORT
CameraObject* cameraObject( BaseObjectData* _object );



}

#endif // PLUGINFUNCTIONSCAMERA_HH
