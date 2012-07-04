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
 * \file PluginFunctionsPolyLine.hh
 * This file contains functions which can be used by plugins to access polylines in the framework.
 */

//
#ifndef PLUGINFUNCTIONSPOLYLINE_HH
#define PLUGINFUNCTIONSPOLYLINE_HH

#include <OpenFlipper/common/Types.hh>

/** The Namespace PluginFunctions contains functions for all plugins. */
namespace PluginFunctions {


/** \brief Get a pointer to every Polyline which is marked as a source.
 *
 * @param _polylines ( vector returning the source polylines )
 * @return false, if no polyline is selected as source
*/
DLLEXPORT
bool getSourcePolylines( std::vector<PolyLine*>& _polylines  );

/** \brief Get a pointer to every Polyline which is marked as a target.
 *
 * @param _polylines ( vector returning the target polylines )
 * @return false, if no polyline is selected as target
*/
DLLEXPORT
bool getTargetPolylines( std::vector<PolyLine*>& _polylines  );


/** This functions returns the object with the given id if it is a PolyLineObject.
 * See get_object(  int _identifier , BaseObject*& _object ) for more details.
 */
DLLEXPORT
bool getObject(  int _identifier , PolyLineObject*& _object );

/** \brief Get a poly Line from an object.
 *
 * @param _object The object should be of type BaseDataObject. If the content is a poly Line, a
 *                poly line will be returned. Otherwise a NULL pointer is returned.
 */
DLLEXPORT
PolyLine* polyLine( BaseObjectData* _object );

/** \brief Cast an BaseObject to a PolyLineObject if possible
 *
 * @param _object The object should be of type BaseDataObject. If the content is a polyLine, a
 *                a PolyLineObject is returned. Otherwise a NULL pointer is returned.
 */
DLLEXPORT
PolyLineObject* polyLineObject( BaseObjectData* _object );

/** \brief Get an PolyLineObject from the given id If possible
*
* @param _objectId Id of the requested Object. If the content is a volume, a
*                  a PolyLineObject is returned. Otherwise a NULL pointer is returned.
*/
DLLEXPORT
PolyLineObject* polyLineObject( int _objectId );

}

#endif // PLUGINFUNCTIONSPOLYLINE_HH
