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

//================================================================
//
//  Standard Functions
//
//================================================================


/**
 * \file PluginFunctionsSplatCloud.hh
 * This file contains functions which can be used by plugins to access splat clouds in the framework
 */


#ifndef PLUGINFUNCTIONSSPLATCLOUD_HH
#define PLUGINFUNCTIONSSPLATCLOUD_HH


//== INCLUDES ====================================================


#include "SplatCloud.hh"

#include <OpenFlipper/common/Types.hh>


//== NAMESPACES ==================================================


/** The Namespace PluginFunctions contains functions for all plugins. */
namespace PluginFunctions {


//== DEFINITIONS =================================================


/** This functions returns the object with the given id if it is a SplatCloudObject.
 * See get_object(  int _identifier , BaseObject*& _object ) for more details.
 */
DLLEXPORT
bool getObject( int _identifier, SplatCloudObject *&_object );


/** \brief Get a ShaderNode from an object.
 *
 * @param _object The object should be of type BaseDataObject. If the content is a SplatCloud, a
 *                ShaderNode will be returned. Otherwise a NULL pointer is returned.
 */
DLLEXPORT
ShaderNode* splatShaderNode( BaseObjectData *_object );


/** \brief Get a SplatCloudNode from an object.
 *
 * @param _object The object should be of type BaseDataObject. If the content is a SplatCloud, a
 *                SplatCloudNode will be returned. Otherwise a NULL pointer is returned.
 */
DLLEXPORT
SplatCloudNode* splatCloudNode( BaseObjectData *_object );


/** \brief Get a SplatCloud from an object.
 *
 * @param _object The object should be of type BaseDataObject. If the content is a SplatCloud, a
 *                SplatCloud will be returned. Otherwise a NULL pointer is returned.
 */
DLLEXPORT
SplatCloud* splatCloud( BaseObjectData *_object );


/** \brief Cast an SplatCloudObject to a SplatCloudObject if possible
 *
 * @param _object The object should be of type BaseDataObject. If the content is a SplatCloud, a
 *                SplatCloudObject is returned. Otherwise a NULL pointer is returned.
 */
DLLEXPORT
SplatCloudObject* splatCloudObject( BaseObjectData *_object );


/** \brief Get an SplatCloudObject by its id
 *
 * @param _objectId Id of the object. If the object exists and is a SplatCloud, a
 *                SplatCloudObject is returned. Otherwise a NULL pointer is returned.
 */
DLLEXPORT
SplatCloudObject* splatCloudObject(  int _objectId );


//================================================================


} // namespace PluginFunctions


//================================================================


#endif // PLUGINFUNCTIONSSPLATCLOUD_HH
