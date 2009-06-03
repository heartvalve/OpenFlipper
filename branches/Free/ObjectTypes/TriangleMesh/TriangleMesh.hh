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


/**
 * \file TriangleMesh.hh
 * This File contains all required includes for using Triangle Meshes
*/

#ifndef TRIANGLE_MESH_INCLUDE_HH
#define TRIANGLE_MESH_INCLUDE_HH


//== INCLUDES =================================================================

#include <ObjectTypes/MeshObject/MeshObjectT.hh>
#include <ObjectTypes/TriangleMesh/TriangleMeshTypes.hh>

/// Typedef for a Meshobject containing a triangle mesh
typedef MeshObject< TriMesh , DATA_TRIANGLE_MESH_CONST_ID > TriMeshObject;

#include <ObjectTypes/TriangleMesh/PluginFunctionsTriangleMesh.hh>



//=============================================================================
#endif // TRIANGLE_MESH_INCLUDE_HH defined
//=============================================================================

