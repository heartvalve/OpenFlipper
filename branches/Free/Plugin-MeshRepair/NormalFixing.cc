
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




#include "MeshRepairPlugin.hh"


//-----------------------------------------------------------------------------


void MeshRepairPlugin::updateFaceNormals(int _objectId) {
  BaseObjectData* object = 0;
  PluginFunctions::getObject(_objectId,object);

  if ( object == 0) {
    emit log(LOGERR,tr("updateFaceNormals: Unable to get object %1. ").arg(_objectId) );
    return;
  }

  if ( object->dataType(DATA_TRIANGLE_MESH) ) {
    TriMesh* mesh = PluginFunctions::triMesh(object);
    mesh->update_face_normals();
    emit updatedObject(_objectId, UPDATE_ALL);
    emit createBackup( _objectId, "Updated Face Normals", UPDATE_ALL);
    emit scriptInfo( "updateFaceNormals(" + QString::number(_objectId) + ")" );
  } else if ( object->dataType(DATA_POLY_MESH) ) {
    PolyMesh* mesh = PluginFunctions::polyMesh(object);
    mesh->update_face_normals();
    emit updatedObject(_objectId, UPDATE_ALL);
    emit createBackup( _objectId, "Updated Face Normals", UPDATE_ALL);
    emit scriptInfo( "updateFaceNormals(" + QString::number(_objectId) + ")" );
  } else
    emit log(LOGERR,tr("updateFaceNormals: MeshRepair only works on triangle and poly meshes!") );

}


//-----------------------------------------------------------------------------

void MeshRepairPlugin::updateHalfedgeNormals(int _objectId) {
  BaseObjectData* object = 0;
  PluginFunctions::getObject(_objectId,object);

  if ( object == 0) {
    emit log(LOGERR,tr("updateFaceNormals: Unable to get object %1. ").arg(_objectId) );
    return;
  }

  if ( object->dataType(DATA_TRIANGLE_MESH) ) {
    TriMesh* mesh = PluginFunctions::triMesh(object);
    mesh->request_halfedge_normals();
    mesh->update_halfedge_normals();
    emit updatedObject(_objectId, UPDATE_ALL);
    emit createBackup( _objectId, "Updated Face Normals", UPDATE_ALL);
    emit scriptInfo( "updateFaceNormals(" + QString::number(_objectId) + ")" );
  } else if ( object->dataType(DATA_POLY_MESH) ) {
    PolyMesh* mesh = PluginFunctions::polyMesh(object);
    mesh->request_halfedge_normals();
    mesh->update_halfedge_normals();
    emit updatedObject(_objectId, UPDATE_ALL);
    emit createBackup( _objectId, "Updated Face Normals", UPDATE_ALL);
    emit scriptInfo( "updateFaceNormals(" + QString::number(_objectId) + ")" );
  } else
    emit log(LOGERR,tr("updateFaceNormals: MeshRepair only works on triangle and poly meshes!") );

}


//-----------------------------------------------------------------------------


void MeshRepairPlugin::updateVertexNormals(int _objectId){
  BaseObjectData* object = 0;
  PluginFunctions::getObject(_objectId,object);

  if ( object == 0) {
    emit log(LOGERR,tr("updateVertexNormals: Unable to get object %1. ").arg(_objectId) );
    return;
  }

  if ( object->dataType(DATA_TRIANGLE_MESH) ) {
    TriMesh* mesh = PluginFunctions::triMesh(object);
    mesh->update_vertex_normals();
    emit updatedObject(_objectId, UPDATE_ALL);
    emit createBackup( _objectId, "Updated Vertex Normals", UPDATE_ALL);
    emit scriptInfo( "updateVertexNormals(" + QString::number(_objectId) + ")" );
  } else if ( object->dataType(DATA_POLY_MESH) ) {
    PolyMesh* mesh = PluginFunctions::polyMesh(object);
    mesh->update_vertex_normals();
    emit updatedObject(_objectId, UPDATE_ALL);
    emit createBackup( _objectId, "Updated Vertex Normals", UPDATE_ALL);
    emit scriptInfo( "updateVertexNormals(" + QString::number(_objectId) + ")" );
  } else
    emit log(LOGERR,tr("updateVertexNormals: MeshRepair only works on triangle and poly meshes!") );
}

//-----------------------------------------------------------------------------

void MeshRepairPlugin::updateNormals(int _objectId) {
  BaseObjectData* object = 0;
  PluginFunctions::getObject(_objectId,object);

  if ( object == 0) {
    emit log(LOGERR,tr("updateNormals: Unable to get object %1. ").arg(_objectId) );
    return;
  }

  if ( object->dataType(DATA_TRIANGLE_MESH) ) {
    TriMesh* mesh = PluginFunctions::triMesh(object);
    mesh->update_normals();
    emit scriptInfo( "updateNormals(" + QString::number(_objectId) + ")" );
  } else if ( object->dataType(DATA_POLY_MESH) ) {
    PolyMesh* mesh = PluginFunctions::polyMesh(object);
    mesh->update_normals();
    emit updatedObject(_objectId, UPDATE_ALL);
    emit createBackup( _objectId, "Updated All Normals", UPDATE_ALL);
    emit scriptInfo( "updateNormals(" + QString::number(_objectId) + ")" );
  } else
    emit log(LOGERR,tr("updateNormals: MeshRepair only works on triangle and poly meshes!") );
}


