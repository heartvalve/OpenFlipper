
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
#include <MeshTools/MeshSelectionT.hh>

//-----------------------------------------------------------------------------

void MeshRepairPlugin::selectEdgesShorterThan(int _objectId,double _length) {
  selectionEdgeLength(_objectId,_length,false);

  emit scriptInfo( "selectEdgesShorterThan(" + QString::number(_objectId) + ", " + QString::number(_length) + ")" );
}

//-----------------------------------------------------------------------------

void MeshRepairPlugin::selectEdgesLongerThan(int _objectId,double _length) {
  selectionEdgeLength(_objectId,_length,true);

  emit scriptInfo( "selectEdgesLongerThan(" + QString::number(_objectId) + ", " + QString::number(_length) + ")" );
}

//-----------------------------------------------------------------------------


void MeshRepairPlugin::selectionEdgeLength(int _objectId, double _length, bool _larger) {

  // get the target mesh
  TriMesh* triMesh = 0;

  PluginFunctions::getMesh(_objectId,triMesh);

  if ( triMesh ) {
    TriMesh::EdgeIter e_it, e_end=triMesh->edges_end();

    // Clear current edge selection
    MeshSelection::clearEdgeSelection(triMesh);

    // Iterate over all edges
    for (e_it = triMesh->edges_begin(); e_it != e_end ; ++e_it) {
      TriMesh::HalfedgeHandle he = triMesh->halfedge_handle( e_it, 0 );
      TriMesh::Point p1 = triMesh->point( triMesh->from_vertex_handle( he ) );
      TriMesh::Point p2 = triMesh->point( triMesh->to_vertex_handle( he )   );

      if ( _larger ) {
        if ( (p1 - p2).norm() > _length)
          triMesh->status(e_it).set_selected(true);
      } else {
        if ( (p1 - p2).norm() < _length)
          triMesh->status(e_it).set_selected(true);
      }
    }

    emit updatedObject(_objectId,UPDATE_SELECTION);
    emit createBackup( _objectId, "Select Edges", UPDATE_SELECTION);

    return;
  }

  // get the target mesh
  PolyMesh* polyMesh = 0;
  PluginFunctions::getMesh(_objectId,polyMesh);

  if ( polyMesh ) {
    PolyMesh::EdgeIter e_it, e_end=polyMesh->edges_end();

    // Clear current edge selection
    MeshSelection::clearEdgeSelection(polyMesh);

    // Iterate over all edges
    for (e_it = polyMesh->edges_begin(); e_it != e_end ; ++e_it) {
      PolyMesh::HalfedgeHandle he = polyMesh->halfedge_handle( e_it, 0 );
      PolyMesh::Point p1 = polyMesh->point( polyMesh->from_vertex_handle( he ) );
      PolyMesh::Point p2 = polyMesh->point( polyMesh->to_vertex_handle( he )   );

      if ( _larger ) {
        if ( (p1 - p2).norm() > _length)
          polyMesh->status(e_it).set_selected(true);
      } else {
        if ( (p1 - p2).norm() < _length)
          polyMesh->status(e_it).set_selected(true);
      }
    }

    emit updatedObject(_objectId,UPDATE_SELECTION);
    emit createBackup( _objectId, "Select Edges", UPDATE_SELECTION);

    return;
  }

  emit log( LOGERR,tr("Unsupported Object Type for edge selection") );

}

//-----------------------------------------------------------------------------

void MeshRepairPlugin::removeSelectedEdges(int _objectId) {

  // get the target mesh
  TriMesh* triMesh = 0;

  PluginFunctions::getMesh(_objectId, triMesh);

  if (triMesh) {

    TriMesh::EdgeIter e_it, e_end = triMesh->edges_end();

    for (e_it = triMesh->edges_begin(); e_it != e_end; ++e_it) {

      if (!triMesh->status(e_it).deleted() && triMesh->status(e_it).selected()) {

        const TriMesh::VHandle v0 = triMesh->to_vertex_handle(triMesh->halfedge_handle(e_it, 0));
        const TriMesh::VHandle v1 = triMesh->to_vertex_handle(triMesh->halfedge_handle(e_it, 1));

        const bool boundary0 = triMesh->is_boundary(v0);
        const bool boundary1 = triMesh->is_boundary(v1);

        const bool feature0 = triMesh->status(v0).feature();
        const bool feature1 = triMesh->status(v1).feature();
        const bool featureE = triMesh->status(e_it).feature();

        // Collapsing v1 into vo:
        // collapse is ok, if collapsed vertex is not a feature vertex or the target vertex is a feature
        // and if we collapse along an feature edge or if the collapsed vertex is not a feature
        if ((!boundary1 || boundary0) && (!feature1 || (feature0 && featureE)) && triMesh->is_collapse_ok(
            triMesh->halfedge_handle(e_it, 0)))
          triMesh->collapse(triMesh->halfedge_handle(e_it, 0));
        else if ((!boundary0 || boundary1) && (!feature0 || (feature1 && featureE)) && triMesh->is_collapse_ok(
            triMesh->halfedge_handle(e_it, 1)))
          triMesh->collapse(triMesh->halfedge_handle(e_it, 1));
      }
    }

    triMesh->garbage_collection();
    triMesh->update_normals();

    emit updatedObject(_objectId, UPDATE_ALL);
    emit createBackup(_objectId, "Removed selected Edges", UPDATE_ALL);
    emit scriptInfo("removeSelectedEdges(" + QString::number(_objectId) + ")");

    return;
  }

  emit log(LOGERR, tr("Unsupported Object Type for edge removal"));

}



void MeshRepairPlugin::detectSkinnyTriangleByAngle(int _objectId, double _angle, bool _remove) {
  // get the target mesh
  TriMesh* triMesh = 0;

  PluginFunctions::getMesh(_objectId, triMesh);

  if (triMesh) {

    // Clear current edge selection
    MeshSelection::clearEdgeSelection(triMesh);

    double maxAngle = cos(_angle * M_PI / 180.0);
    double angle = 0.0;
    TriMesh::EdgeIter e_it, e_end = triMesh->edges_end();

    for (e_it = triMesh->edges_begin(); e_it != e_end; ++e_it) {

      // Check prerequisites
      if (!triMesh->status(e_it).deleted() && !triMesh->status(e_it).feature() && triMesh->is_flip_ok(e_it)) {

        // For both halfedges
        for (unsigned int h = 0; h < 2; ++h) {
          TriMesh::HalfedgeHandle hh = triMesh->halfedge_handle(e_it.handle(), h);
          const TriMesh::Point& a = triMesh->point(triMesh->from_vertex_handle(hh));
          const TriMesh::Point& b = triMesh->point(triMesh->to_vertex_handle(hh));
          hh = triMesh->next_halfedge_handle(hh);
          const TriMesh::Point& c = triMesh->point(triMesh->to_vertex_handle(hh));

          angle = ((a - c).normalize() | (b - c).normalize());

          if (angle < maxAngle) {

            // selcet it
            triMesh->status(e_it).set_selected(true);

            // remove it if requested
            if (_remove)
              triMesh->flip(e_it);
          }
        }
      }
    }

    if (_remove) {
      emit updatedObject(_objectId, UPDATE_ALL);
      emit createBackup(_objectId, tr("Removed cap edges"), UPDATE_ALL);
    } else {
      emit updatedObject(_objectId, UPDATE_SELECTION);
      emit createBackup(_objectId, tr("Selected cap edges"), UPDATE_ALL);
    } emit
    scriptInfo(
        "detectSkinnyTriangleByAngle(" + QString::number(_objectId) + "," + QString::number(_angle) + ","
        + _remove + ")");

    return;
  }

  emit log(LOGERR, tr("Unsupported Object Type for Cap detection!"));
}

//-----------------------------------------------------------------------------


void MeshRepairPlugin::detectFoldover(int _objectId, float _angle) {

  // get the target mesh
  TriMeshObject* trimesh_o = 0;
  PolyMeshObject* polymesh_o = 0;

  PluginFunctions::getObject(_objectId, trimesh_o);
  PluginFunctions::getObject(_objectId, polymesh_o);

  unsigned int count = 0;

  if (trimesh_o != 0) {

    // get the target mesh
    TriMesh* mesh = trimesh_o->mesh();

    if (mesh) {

      // Clear current edge selection
      MeshSelection::clearEdgeSelection(mesh);

      TriMesh::EdgeIter e_it, e_end(mesh->edges_end());
      TriMesh::FaceHandle fh;
      TriMesh::Scalar a, cosa = cos(_angle / 180.0 * M_PI);

      for (e_it = mesh->edges_begin(); e_it != e_end; ++e_it) {
        if (!mesh->is_boundary(e_it)) {
          a = (mesh->normal(mesh->face_handle(mesh->halfedge_handle(e_it, 0))) | mesh->normal(
              mesh->face_handle(mesh->halfedge_handle(e_it, 1))));

          if (a < cosa) {
            mesh->status(mesh->edge_handle(mesh->halfedge_handle(e_it, 0))). set_selected(true);
            ++count;
          }
        }
      }
    }
  } else if (polymesh_o != 0) {

    // get the target mesh
    PolyMesh* mesh = polymesh_o->mesh();

    if (mesh) {

      // Clear current edge selection
      MeshSelection::clearEdgeSelection(mesh);

      PolyMesh::EdgeIter e_it, e_end(mesh->edges_end());
      PolyMesh::FaceHandle fh;
      PolyMesh::Scalar a, cosa = cos(_angle / 180.0 * M_PI);

      for (e_it = mesh->edges_begin(); e_it != e_end; ++e_it) {
        if (!mesh->is_boundary(e_it)) {
          a = (mesh->normal(mesh->face_handle(mesh->halfedge_handle(e_it, 0))) | mesh->normal(
              mesh->face_handle(mesh->halfedge_handle(e_it, 1))));

          if (a < cosa) {
            mesh->status(mesh->edge_handle(mesh->halfedge_handle(e_it, 0))). set_selected(true);
            ++count;
          }
        }
      }
    }
  }

  if (count > 0) {
    emit updatedObject(_objectId, UPDATE_SELECTION);
    emit createBackup(_objectId, "Select edges", UPDATE_SELECTION);
    emit scriptInfo("detectFoldover(" + QString::number(_objectId) + ", " + QString::number(_angle) + ")");
  }
  emit log(
      "Selected " + QString::number(count) + " fold-overs on object " + QString::number(_objectId)
  + " with angle greater than " + QString::number(_angle) + ".");
}


//-----------------------------------------------------------------------------
