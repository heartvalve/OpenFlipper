//=============================================================================
//
//  Standard Functions
//
//=============================================================================

/**
 * \file PluginFunctionsPolyhedralMesh.hh
 * This file contains functions which can be used by plugins to access
 * PolyhedralMeshes in the framework.
 */

//
#ifndef PLUGINFUNCTIONSPOLYHEDRALMESH_HH
#define PLUGINFUNCTIONSPOLYHEDRALMESH_HH

#include <ObjectTypes/VolumeMeshObject/VolumeMeshObject.hh>
#include <ObjectTypes/PolyhedralMesh/PolyhedralMesh.hh>
#include <OpenFlipper/common/Types.hh>

//== NAMESPACES ===============================================================

/** The Namespace PluginFunctions contains functions for all plugins. */
namespace PluginFunctions {


/** \brief Get a pointer to every PolyhedralMesh which is marked as a source.
 *
 * @param _meshes ( vector returning the source polyhedral meshes )
 * @return false, if no polyhedral mesh is selected as source
*/
DLLEXPORT
bool getSourceMeshes( std::vector<PolyhedralMesh*>& _meshes  );

/** \brief Get a pointer to every PolyhedralMesh which is marked as a target.
 *
 * @param _meshes ( vector returning the target polyhedral meshes )
 * @return false, if no polyhedral mesh is selected as target
*/
DLLEXPORT
bool getTargetMeshes( std::vector<PolyhedralMesh*>& _meshes  );


/** This functions returns the object with the given id if it is an PolyhedralMeshObject.
 * See get_object(  int _identifier , BaseObject*& _object ) for more details.
 */
DLLEXPORT
bool getObject( int _identifier, PolyhedralMeshObject*& _object );

/** \brief Get an PolyhedralMesh from an object.
 *
 * @param _object The object should be of type BaseDataObject. If the content is
 * a polyhedral mesh, a polyhedral mesh will be returned. Otherwise a NULL pointer is returned.
 */
DLLEXPORT
PolyhedralMesh* polyhedralMesh( BaseObjectData* _object );

/** \brief Cast an BaseObject to an PolyhedralMeshObject if possible
 *
 * @param _object The object should be of type BaseDataObject. If the content is
 * an PolyhedralMesh, an PolyhedralMeshObject is returned. Otherwise a NULL pointer is returned.
 */
DLLEXPORT
PolyhedralMeshObject* polyhedralMeshObject( BaseObjectData* _object );


/** \brief Cast an int to an PolyhedralMeshObject if possible
 *
 * @param _identifier
 */

DLLEXPORT
PolyhedralMeshObject* polyhedralMeshObject( int _identifier );

}

#endif // PLUGINFUNCTIONSOPENVOLUMEMESH_HH
