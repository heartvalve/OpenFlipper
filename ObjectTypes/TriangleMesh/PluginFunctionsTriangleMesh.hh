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
//  Standard Functions
//
//=============================================================================

/**
 * \file PluginFunctionsTriangleMesh.hh
 * This file contains functions which can be used by plugins to access Triangle Meshes in the framework.
 */

//
#ifndef PLUGINFUNCTIONSTRIANGLEMESH_HH
#define PLUGINFUNCTIONSTRIANGLEMESH_HH

#include <OpenFlipper/common/Types.hh>
#include <ObjectTypes/TriangleMesh/TriangleMesh.hh>

/** The Namespace PluginFunctions contains functions for all plugins. These functions should be used to get the
 *  objects to work on or to set modes in the examiner widget. */
namespace PluginFunctions {

//=======================================
// Get Source/Target objects
/** @name Active Objects
* @{ */
//=======================================

/** \brief Get a pointer to every Triangle Mesh which is marked as a source mesh.
 *
 * @param _meshes ( vector returning the source meshes )
 * @return false, if no mesh is selected as source
*/
DLLEXPORT
bool getSourceMeshes( std::vector<TriMesh*>& _meshes  );

/** \brief Get a pointer to every Triangle Mesh which is marked as a target mesh.
 *
 * @param _meshes ( vector returning the target meshes )
 * @return false, if no mesh is selected as target
*/
DLLEXPORT
bool getTargetMeshes( std::vector<TriMesh*>& _meshes  );

/** @} */

//=======================================
// Get Objects by their identifier
    /** @name Identifier handling
    * @{ */
//=======================================

/** This functions returns the object with the given id if it is a TriMeshObject.
 * See get_object(  int _identifier , BaseObject*& _object ) for more details.
 */
DLLEXPORT
bool getObject(  int _identifier , TriMeshObject*& _object );


/** \brief Get the Triangle Mesh which has the given identifier.
 *
 *   Every loaded object has a unique identifier which is stored in the id field of the object container.
 *   Use this function to get the mesh which has this id. This can be used for a consistent mapping
 *   even if the data objects change during plugin operations (e.g. selection and main algorithm).\n
 *   This function checks, if the object requested contains a mesh.
 * @param _identifier Object id to search for
 * @param _mesh  returns the mesh
 * @return Mesh found?
 */
DLLEXPORT
bool getMesh(  int _identifier , TriMesh*& _mesh );

/** @} */

//=======================================
    /** @name Getting data from objects and casting between them
     * @{ */
//=======================================

/** \brief Get a triangle mesh from an object.
 *
 * @param _object The object should be of type BaseDataObject. If the content is a triangle Mesh, a
 *                triangle mesh will be returned. Otherwise a NULL pointer is returned.
 */
DLLEXPORT
TriMesh* triMesh( BaseObjectData* _object );

/** \brief Get a triangle mesh from an object id.
 *
 * @param _identifier Identifier of the object. If its a triangle mesh, the function will return the pointer to the mesh
 *                    otherwise 0
 */
DLLEXPORT
TriMesh* triMesh( int _identifier );

/** \brief Cast an BaseObject to a TriMeshObject if possible
 *
 * @param _object The object should be of type BaseDataObject. If the content is a triangle Mesh, a
 *                a TriMeshObject is returned. Otherwise a NULL pointer is returned.
 */
DLLEXPORT
TriMeshObject* triMeshObject( BaseObjectData* _object );

/** \brief Get an TriMeshObject from the given id If possible
*
* @param _objectId Id of the requested Object. 
* @return If the content is a volume, a pointer to the TriMeshObject is returned, otherwise a NULL pointer.
*/
DLLEXPORT
TriMeshObject* triMeshObject( int _objectId );

/** @} */
}

#endif //PLUGINFUNCTIONSTRIANGLEMESH_HH
