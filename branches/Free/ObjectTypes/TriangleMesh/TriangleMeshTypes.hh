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
//  Triangle Mesh Types
//
//=============================================================================

/**
 * \file TriangleMeshTypes.hh
 * This File contains the required types and typedefs for using Triangle Meshes
 */

#ifndef TRIANGLEMESH_TYPES_HH
#define TRIANGLEMESH_TYPES_HH

 
//== INCLUDES =================================================================

#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>


#include <ACG/Scenegraph/TriStripNodeT.hh>


//== TYPEDEFS =================================================================
   
/** Traits for the Meshes ( Use request ... calls to add other standard properties or add them in the via add_property.. )\n
 *   Dont add Properties here as they may prevent plugins from loading!!!!
*/
struct TriTraits : public OpenMesh::DefaultTraits
{
  /// Use double precision points
  typedef OpenMesh::Vec3d Point;
  /// Use double precision Normals
  typedef OpenMesh::Vec3d Normal;

  /// Use double precision rgba Color
//   typedef OpenMesh::Vec4f Color;

};

/// Simple Name for Mesh
typedef OpenMesh::TriMesh_ArrayKernelT<TriTraits>  TriMesh;


//== TYPEDEFS FOR SCENEGRAPH ===============================================


//=============================================================================
#endif // TRIANGLEMESH_TYPES_HH defined
//=============================================================================
