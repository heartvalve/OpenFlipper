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
 *   $Revision: 13489 $                                                       *
 *   $LastChangedBy: kremer $                                                 *
 *   $Date: 2012-01-25 12:30:09 +0100 (Wed, 25 Jan 2012) $                    *
 *                                                                            *
 \*===========================================================================*/

/**
 * \file PluginFunctionsHexahedralMesh.hh
 * This file contains functions which can be used by plugins to access
 * HexahedralMeshes in the framework.
 */

//
#ifndef PLUGINFUNCTIONSHEXAHEDRALMESH_HH
#define PLUGINFUNCTIONSHEXAHEDRALMESH_HH

#include <ObjectTypes/VolumeMeshObject/VolumeMeshObject.hh>
#include <ObjectTypes/HexahedralMesh/HexahedralMesh.hh>
#include <OpenFlipper/common/Types.hh>

//== NAMESPACES ===============================================================

/** The Namespace PluginFunctions contains functions for all plugins. */
namespace PluginFunctions {


/** \brief Get a pointer to every HexahedralMesh which is marked as a source.
 *
 * @param _meshes ( vector returning the source hexahedral meshes )
 * @return false, if no hexahedral mesh is selected as source
*/
DLLEXPORT
bool getSourceMeshes( std::vector<HexahedralMesh*>& _meshes  );

/** \brief Get a pointer to every HexahedralMesh which is marked as a target.
 *
 * @param _meshes ( vector returning the target hexahedral meshes )
 * @return false, if no hexahedral mesh is selected as target
*/
DLLEXPORT
bool getTargetMeshes( std::vector<HexahedralMesh*>& _meshes  );


/** This functions returns the object with the given id if it is an HexahedralMeshObject.
 * See get_object(  int _identifier , BaseObject*& _object ) for more details.
 */
DLLEXPORT
bool getObject( int _identifier, HexahedralMeshObject*& _object );

/** \brief Get an HexahedralMesh from an object.
 *
 * @param _object The object should be of type BaseDataObject. If the content is
 * a hexahedral mesh, a hexahedral mesh will be returned. Otherwise a NULL pointer is returned.
 */
DLLEXPORT
HexahedralMesh* hexahedralMesh( BaseObjectData* _object );

/** \brief Get a HexahedralMesh from an object id.
 *
 * @param _identifier Identifier of the object. If its a HexahedralMesh mesh, the function will return the pointer to the mesh
 *                    otherwise 0
 */
DLLEXPORT
HexahedralMesh* hexahedralMesh( int _identifier );

/** \brief Cast an BaseObject to an HexahedralMeshObject if possible
 *
 * @param _object The object should be of type BaseDataObject. If the content is
 * an HexahedralMesh, an HexahedralMeshObject is returned. Otherwise a NULL pointer is returned.
 */
DLLEXPORT
HexahedralMeshObject* hexahedralMeshObject( BaseObjectData* _object );


/** \brief Cast an int to an HexahedralMeshObject if possible
 *
 * @param _identifier
 */

DLLEXPORT
HexahedralMeshObject* hexahedralMeshObject( int _identifier );

}

#endif // PLUGINFUNCTIONSHEXAHEDRALMESH_HH
