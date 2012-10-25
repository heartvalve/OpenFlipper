
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
 *   $Revision: 15713 $                                                       *
 *   $LastChangedBy: moebius $                                                *
 *   $Date: 2012-10-25 12:58:58 +0200 (Do, 25 Okt 2012) $                     *
 *                                                                            *
\*===========================================================================*/




#include "MeshRepairPlugin.hh"
#include "MeshFixingT.hh"


//-----------------------------------------------------------------------------

void MeshRepairPlugin::fixNonManifoldVertices(int _objectId)
{
  TriMesh* triMesh = 0;
  PolyMesh* polyMesh = 0;

  PluginFunctions::getMesh(_objectId, triMesh);
  PluginFunctions::getMesh(_objectId, polyMesh);
  if (triMesh)
    fixNonManifoldVertices(triMesh);
  else if (polyMesh)
    fixNonManifoldVertices(polyMesh);
  else
  {
    emit log(LOGERR, tr("Unsupported Object Type."));
    return;
  }

  emit updatedObject(_objectId, UPDATE_ALL);
  emit createBackup(_objectId, "fixTopology", UPDATE_ALL);
  emit scriptInfo("fixTopology(" + QString::number(_objectId) + ")");
}

//-----------------------------------------------------------------------------

void
MeshRepairPlugin::fixMesh(int _objectId, double _epsilon) {
  // get the target mesh
   TriMesh* triMesh = 0;

   PluginFunctions::getMesh(_objectId,triMesh);

   if (triMesh) {
     MeshFixing<TriMesh> fixer(*triMesh,_epsilon);

     if ( !fixer.fix() )
       emit log(LOGERR, "Fixmesh encountered Problems! Object: " + QString::number(_objectId) + ".");

     // Recompute normals
     triMesh->update_normals();

     emit updatedObject(_objectId, UPDATE_ALL);
     emit createBackup(_objectId, "Fixed mesh", UPDATE_ALL);

     emit scriptInfo( "fixMesh(" + QString::number(_objectId) + ", " + QString::number(_epsilon) + ")" );

   } else
     emit log( LOGERR,tr("Unsupported Object Type for mesh fixing!") );

}

//-----------------------------------------------------------------------------
