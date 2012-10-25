
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

void
MeshRepairPlugin::detectTriangleAspect(int _objectId, float _aspect) {

  // get the target mesh
  TriMesh* mesh = 0;

  PluginFunctions::getMesh(_objectId, mesh);

  if (mesh) {

    unsigned int count(0);

    // Clear current face selection
    MeshSelection::clearFaceSelection(mesh);

    TriMesh::FaceIter f_it, f_end(mesh->faces_end());
    TriMesh::FVIter fv_it;
    TriMesh::FEIter fe_it;

    for (f_it = mesh->faces_begin(); f_it != f_end; ++f_it) {
      fv_it = mesh->fv_iter(f_it);

      const TriMesh::Point& p0 = mesh->point(fv_it);
      const TriMesh::Point& p1 = mesh->point(++fv_it);
      const TriMesh::Point& p2 = mesh->point(++fv_it);

      if (ACG::Geometry::aspectRatio(p0, p1, p2) > _aspect) {
        mesh->status(f_it).set_selected(true);
        ++count;
      }
    }
    if (count > 0) {
      emit updatedObject(_objectId, UPDATE_SELECTION);
      emit createBackup(_objectId, "Select triangles", UPDATE_SELECTION);
      emit scriptInfo( "detectTriangleAspect(" + QString::number(_objectId) + ", " + QString::number(_aspect) + ")" );
    }
    emit log(
        "Selected " + QString::number(count) + " triangles on object " + QString::number(_objectId)
    + " with aspect ratio greater than " + QString::number(_aspect) + ".");
  } else {
    emit log("Cannot detect skinny triangles on non-trimesh " + QString::number(_objectId) + ".");
  }
}

//-----------------------------------------------------------------------------

void
MeshRepairPlugin::flipOrientation(int _objectId) {

  // get the target mesh
  TriMesh* triMesh = 0;
  PolyMesh* polyMesh = 0;

  PluginFunctions::getMesh(_objectId,triMesh);
  PluginFunctions::getMesh(_objectId,polyMesh);

  if (triMesh)
    flipOrientationSelected(triMesh);
  else if (polyMesh)
    flipOrientationSelected(polyMesh);
  else
    emit log( LOGERR,tr("Unsupported Object Type for normal flipping!") );


  emit updatedObject(_objectId, UPDATE_ALL);
  emit createBackup( _objectId, "Flipped Normals", UPDATE_ALL);
  emit scriptInfo( "flipOrientation(" + QString::number(_objectId) + ")" );
}


//-----------------------------------------------------------------------------
