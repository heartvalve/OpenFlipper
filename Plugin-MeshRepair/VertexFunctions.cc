
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
 *   $Revision$                                                       *
 *   $LastChangedBy$                                                *
 *   $Date$                     *
 *                                                                            *
\*===========================================================================*/




#include "MeshRepairPlugin.hh"

//-----------------------------------------------------------------------------


void MeshRepairPlugin::detectFlatValence3Vertices(int _objectId, double _angle) {

  unsigned int count(0);

  // get the target mesh
  TriMesh* mesh = 0;
  PluginFunctions::getMesh(_objectId,mesh);

  if ( mesh ) {

    // Clear current triangle selection
    MeshSelection::clearVertexSelection(mesh);

    TriMesh::VertexIter                 v_it, v_end(mesh->vertices_end());
    TriMesh::VVIter                     vv_it;
    TriMesh::VFIter                     vf_it;
    TriMesh::FaceHandle                 fh;
    std::vector<TriMesh::VertexHandle>  vh(3);
    TriMesh::Scalar                     cosangle(cos(_angle/180.0*M_PI));

    for (v_it=mesh->vertices_begin(); v_it!=v_end; ++v_it)
    {
      if (!mesh->status(v_it).deleted() && !mesh->is_boundary(v_it) && mesh->valence(v_it) == 3)
      {
        vf_it = mesh->vf_iter(v_it);
        const TriMesh::Normal& n0 = mesh->normal(vf_it);
        const TriMesh::Normal& n1 = mesh->normal(++vf_it);
        const TriMesh::Normal& n2 = mesh->normal(++vf_it);

        if ( (n0|n1) > cosangle &&
            (n0|n2) > cosangle &&
            (n1|n2) > cosangle )
        {

          mesh->status(v_it).set_selected(true);
          ++count;
        }
      }
    }
  }
  else {
    emit log(LOGERR, "Cannot detect flat triangles on non-trimesh " + QString::number(_objectId) + ".");
  }

  if (count > 0) {
    emit updatedObject(_objectId, UPDATE_SELECTION);
    emit createBackup(_objectId, "Select vertices", UPDATE_SELECTION);
  }

  emit log (LOGINFO,"Selected " + QString::number(count) + " vertices on object " + QString::number(_objectId) + " with face angle difference smaller than " + QString::number(_angle) + ".");
  emit scriptInfo( "detectFlatValence3Vertices(" + QString::number(_objectId) + ", " + QString::number(_angle) + ")" );

}

//-----------------------------------------------------------------------------

void MeshRepairPlugin::removeSelectedVal3Vertices(int _objectId) {

  unsigned int count = 0;

  // get the target mesh
  TriMesh* mesh = 0;
  PluginFunctions::getMesh(_objectId, mesh);

  if (mesh) {

    TriMesh::VertexIter v_it, v_end(mesh->vertices_end());
    TriMesh::VVIter vv_it;
    TriMesh::VFIter vf_it;
    int i;
    std::vector<TriMesh::VertexHandle> vh(3);

    for (v_it = mesh->vertices_begin(); v_it != v_end; ++v_it) {
      vf_it = mesh->vf_iter(v_it);
      if ((mesh->status(v_it).selected()) && !mesh->status(v_it).feature() && mesh->valence(v_it) == 3) {
        for (i = 0, vv_it = mesh->vv_iter(v_it); vv_it; ++vv_it, ++i)
          vh[2 - i] = vv_it.handle();

        mesh->delete_vertex(v_it, false);
        mesh->add_face(vh);

        ++count;
      }
    }
    if (count > 0)
      mesh->garbage_collection();
  }

  if (count > 0) {
    emit updatedObject(_objectId, UPDATE_ALL);
    emit createBackup(_objectId, "Delete/merge selected vertices", UPDATE_ALL);
  }
  emit log("Deleted " + QString::number(count) + " vertices on object " + QString::number(_objectId) + ".");
}

//-----------------------------------------------------------------------------
