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
*   $Revision: 16055 $                                                       *
*   $LastChangedBy: moebius $                                                *
*   $Date: 2013-01-08 10:57:31 +0100 (Tue, 08 Jan 2013) $                     *
*                                                                            *
\*===========================================================================*/

#include "VolumeMeshObjectInfoPlugin.hh"

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>

#include <ObjectTypes/HexahedralMesh/HexahedralMesh.hh>
#include <ObjectTypes/PolyhedralMesh/PolyhedralMesh.hh>

//------------------------------------------------------------------------------

/** \brief set the descriptions for scripting slots
 * 
 */
void InfoVolumeMeshObjectPlugin::setDescriptions(){

  emit setSlotDescription("vertexCount(int)",tr("get total number of vertices for a given object"),
                          QStringList(tr("objectID")), QStringList(tr("id of an object")));

  emit setSlotDescription("edgeCount(int)",tr("get total number of edges for a given object"),
                          QStringList(tr("objectID")), QStringList(tr("id of an object")));

  emit setSlotDescription("faceCount(int)",tr("get total number of faces for a given object"),
                          QStringList(tr("objectID")), QStringList(tr("id of an object")));

  emit setSlotDescription("cellCount(int)",tr("get total number of cells for a given object"),
                          QStringList(tr("objectID")), QStringList(tr("id of an object")));
}


//------------------------------------------------------------------------------

/** \brief get total number of vertices for a given object
 * 
 * @param _id object id
 * @return number of vertices or -1 if an error occured
 */
int InfoVolumeMeshObjectPlugin::vertexCount(int _id)
{

  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(_id,object) )
    return -1;

  if ( object == 0){
    emit log(LOGERR, tr("Unable to get object"));
    return -1;
  }

  if ( object->dataType(DATA_HEXAHEDRAL_MESH) ) {
    HexahedralMesh* mesh = PluginFunctions::hexahedralMesh(object);

    if ( mesh == 0 ) {
      emit log(LOGERR, tr("Unable to get mesh"));
      return -1;
    }

    return mesh->n_vertices();

  } else {
    PolyhedralMesh* mesh = PluginFunctions::polyhedralMesh(object);

    if ( mesh == 0 ) {
      emit log(LOGERR, tr("Unable to get mesh"));
      return -1;
    }

    return mesh->n_vertices();
  }
}


//------------------------------------------------------------------------------

/** \brief get total number of edges for a given object
 * 
 * @param _id object id
 * @return number of edges or -1 if an error occured
 */
int InfoVolumeMeshObjectPlugin::edgeCount(int _id)
{

  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(_id,object) )
    return -1;

  if ( object == 0){
    emit log(LOGERR, tr("Unable to get object"));
    return -1;
  }

  if ( object->dataType(DATA_HEXAHEDRAL_MESH) ) {
    HexahedralMesh* mesh = PluginFunctions::hexahedralMesh(object);

    if ( mesh == 0 ) {
      emit log(LOGERR, tr("Unable to get mesh"));
      return -1;
    }

    return mesh->n_edges();

  } else {
    PolyhedralMesh* mesh = PluginFunctions::polyhedralMesh(object);

    if ( mesh == 0 ) {
      emit log(LOGERR, tr("Unable to get mesh"));
      return -1;
    }

    return mesh->n_edges();
  }
}


//------------------------------------------------------------------------------

/** \brief get total number of faces for a given object
 *
 * @param _id object id
 * @return number of faces or -1 if an error occured
 */
int InfoVolumeMeshObjectPlugin::faceCount(int _id)
{

  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(_id,object) )
    return -1;

  if ( object == 0){
    emit log(LOGERR, tr("Unable to get object"));
    return -1;
  }

  if ( object->dataType(DATA_HEXAHEDRAL_MESH) ) {
    HexahedralMesh* mesh = PluginFunctions::hexahedralMesh(object);

    if ( mesh == 0 ) {
      emit log(LOGERR, tr("Unable to get mesh"));
      return -1;
    }

    return mesh->n_faces();

  } else {
    PolyhedralMesh* mesh = PluginFunctions::polyhedralMesh(object);

    if ( mesh == 0 ) {
      emit log(LOGERR, tr("Unable to get mesh"));
      return -1;
    }

    return mesh->n_faces();
  }
}


//------------------------------------------------------------------------------

/** \brief get total number of cells for a given object
 *
 * @param _id object id
 * @return number of cells or -1 if an error occured
 */
int InfoVolumeMeshObjectPlugin::cellCount(int _id)
{

  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(_id,object) )
    return -1;

  if ( object == 0){
    emit log(LOGERR, tr("Unable to get object"));
    return -1;
  }

  if ( object->dataType(DATA_HEXAHEDRAL_MESH) ) {
    HexahedralMesh* mesh = PluginFunctions::hexahedralMesh(object);

    if ( mesh == 0 ) {
      emit log(LOGERR, tr("Unable to get mesh"));
      return -1;
    }

    return mesh->n_faces();

  } else {
    PolyhedralMesh* mesh = PluginFunctions::polyhedralMesh(object);

    if ( mesh == 0 ) {
      emit log(LOGERR, tr("Unable to get mesh"));
      return -1;
    }

    return mesh->n_cells();
  }
}

//------------------------------------------------------------------------------


