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


/**
 * \file TriangleMesh.hh
 * This File contains all required includes for using Triangle Meshes
*/

#ifndef TRIANGLE_MESH_INCLUDE_HH
#define TRIANGLE_MESH_INCLUDE_HH


//== INCLUDES =================================================================


/** \def DATA_TRIANGLE_MESH
 * Use this macro to reference triangle meshes.
 */
#define DATA_TRIANGLE_MESH typeId("TriangleMesh")
#include <ObjectTypes/MeshObject/MeshObjectT.hh>
#include <ObjectTypes/TriangleMesh/TriangleMeshTypes.hh>
#include <OpenFlipper/common/GlobalDefines.hh>


/// Type for a MeshObject containing a triangle mesh
class DLLEXPORTONLY TriMeshObject : public MeshObject< TriMesh > {

public:
  /** \brief copy constructor
   *
   *  Create a copy of this object
   */
  TriMeshObject(const TriMeshObject& _object);

  /** \brief Constructor
  *
  * This is the standard constructor for MeshObjects. As triangle and Poly Meshes are handled by this class, the
  * typeId is passed to the MeshObject to specify it.
  *
  * @param _typeId   This is the type Id the Object will use. Should be typeId("TriangleMesh") or typeId("PolyMesh")
  */
  TriMeshObject(DataType _typeId);

  /// destructor
  virtual ~TriMeshObject();

 public:
    /// Refine picking on triangle meshes
    ACG::Vec3d refinePick(ACG::SceneGraph::PickTarget _pickTarget, const ACG::Vec3d _hitPoint, const ACG::Vec3d _start , const ACG::Vec3d _dir,  const unsigned int _targetIdx  );

};

#include <ObjectTypes/TriangleMesh/PluginFunctionsTriangleMesh.hh>



//=============================================================================
#endif // TRIANGLE_MESH_INCLUDE_HH defined
//=============================================================================

