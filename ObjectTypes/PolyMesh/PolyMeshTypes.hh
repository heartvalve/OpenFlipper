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
//  PolyMeshTypes
//
//=============================================================================

/**
 * \file PolyMeshTypes.hh
 * This File contains the required types and typedefs for using Polygonal Meshes
 */

#ifndef POLYMESH_TYPES_HH
#define POLYMESH_TYPES_HH

 
//== INCLUDES =================================================================

#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include <ACG/Scenegraph/StatusNodesT.hh>


//== TYPEDEFS =================================================================
   
/** Default traits for the PolyMesh
*/
struct PolyTraits : public OpenMesh::DefaultTraits
{
  /// Use double precision points
  typedef OpenMesh::Vec3d Point;
  /// Use double precision Normals
  typedef OpenMesh::Vec3d Normal;

  /// Use RGBA Color
  typedef OpenMesh::Vec4f Color;
};

/// Simple Name for Mesh
typedef OpenMesh::PolyMesh_ArrayKernelT<PolyTraits>  PolyMesh;

//== TYPEDEFS FOR SCENEGRAPH ===============================================


//=============================================================================
#endif // POLYMESH_TYPES_HH defined
//=============================================================================
