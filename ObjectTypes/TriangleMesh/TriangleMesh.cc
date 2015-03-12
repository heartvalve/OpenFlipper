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
*   $Revision: 18128 $                                                       *
*   $LastChangedBy: moebius $                                                *
*   $Date: 2014-02-05 10:20:28 +0100 (Mi, 05. Feb 2014) $                     *
*                                                                            *
\*===========================================================================*/


/**
 * \file TriangleMesh.cc
 * This File contains all required includes for using Triangle Meshes
*/




//== INCLUDES =================================================================

#include <ObjectTypes/TriangleMesh/TriangleMesh.hh>
#include <ACG/Geometry/Algorithms.hh>



TriMeshObject::TriMeshObject(const TriMeshObject& _object) : MeshObject< TriMesh >(_object) {

}


TriMeshObject::TriMeshObject(DataType _typeId) : MeshObject< TriMesh >(_typeId) {

}


TriMeshObject::~TriMeshObject() {

}

/// Refine picking on triangle meshes
ACG::Vec3d TriMeshObject::refinePick(ACG::SceneGraph::PickTarget _pickTarget, const ACG::Vec3d _hitPoint, const ACG::Vec3d _start , const ACG::Vec3d _dir,  const unsigned int _targetIdx  ) {
  if ( _pickTarget == ACG::SceneGraph::PICK_FACE) {
    std::cerr << "Refine Face picking" << std::endl;

    // get picked face handle
    TriMesh::FaceHandle fh = mesh()->face_handle(_targetIdx);

    TriMesh::FaceVertexIter fv_it = mesh()->fv_begin(fh);

    // Get vertices of the face
    ACG::Vec3d p1 = mesh()->point(*fv_it);
    ++fv_it;

    ACG::Vec3d p2 = mesh()->point(*fv_it);
    ++fv_it;

    ACG::Vec3d p3 = mesh()->point(*fv_it);
    ++fv_it;

    ACG::Vec3d hitpointNew = _hitPoint;

    TriMesh::Scalar t,u,v;
    if ( ACG::Geometry::triangleIntersection( _start, _dir, p1 , p2 , p3 , t , u , v) ) {
      hitpointNew = _start + t * _dir;
    } else {
      std::cerr << "Refine Picking failed" << std::endl;
    }





    std::cerr << "Refine on Triangle meshes" << std::endl;
    std::cerr << "Original: " <<  _hitPoint << std::endl;
    std::cerr << "MousePos : " << _start << std::endl;
    std::cerr << "Update:   " <<  hitpointNew << std::endl;

    return hitpointNew;

  }
  std::cerr << "No Refine on Triangle meshes" << std::endl;
  return _hitPoint;
}


//=============================================================================

