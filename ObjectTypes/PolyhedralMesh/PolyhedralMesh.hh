/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2011 by Computer Graphics Group, RWTH Aachen       *
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
*   $Revision: 13620 $                                                       *
*   $LastChangedBy: moebius $                                                 *
*   $Date: 2012-02-01 14:51:25 +0100 (Mi, 01 Feb 2012) $                    *
*                                                                            *
\*===========================================================================*/

/**
 * \file PolyhedralMesh/PolyhedralMesh.hh
 * This file contains all required includes for using polyhedral meshes
*/

#ifndef POLYHEDRAL_MESH_INCLUDE_HH
#define POLYHEDRAL_MESH_INCLUDE_HH

//== INCLUDES =================================================================

/** \def DATA_POLYHEDRAL_MESH
 * Use this macro to reference Polyhedral Meshes
 */
#define DATA_POLYHEDRAL_MESH typeId("PolyhedralMesh")
#include <ObjectTypes/VolumeMeshObject/VolumeMeshObject.hh>
#include <OpenVolumeMesh/Mesh/PolyhedralMesh.hh>
#include <OpenVolumeMesh/Core/PropertyDefines.hh>

typedef OpenVolumeMesh::GeometryKernel<ACG::Vec3d, OpenVolumeMesh::TopologyKernel> PolyhedralMesh;

/// Typedef for a mesh object containing a polyhedral mesh
typedef VolumeMeshObject<PolyhedralMesh> PolyhedralMeshObject;

#include <ObjectTypes/PolyhedralMesh/PluginFunctionsPolyhedralMesh.hh>

//=============================================================================
#endif // POLYHEDRAL_MESH_INCLUDE_HH defined
//=============================================================================

