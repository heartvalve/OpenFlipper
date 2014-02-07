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
 *   $Revision: 17990 $                                                       *
 *   $LastChangedBy: moeller $                                                *
 *   $Date: 2014-01-07 17:36:34 +0100 (Di, 07. Jan 2014) $                     *
 *                                                                            *
 \*===========================================================================*/

#include "ComponentsPlugin.hh"

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include <MeshTools/MeshInfoT.hh>

//------------------------------------------------------------------------------

/** \brief Set Descriptions for all scripting slots
 *
 */
void ComponentsPlugin::setDescriptions(){
    
  emit setSlotDescription("splitComponents(int)","Split the mesh into connected components. The original mesh is deleted.",
                          QStringList("objectId"), QStringList("Id of an object"));

  emit setSlotDescription("biggestComponent(int)","Get the biggest component and delete the smaller ones.",
                          QStringList("objectId"),QStringList("Id of an object"));

}

//------------------------------------------------------------------------------

/** \brief splits the object into components
 *
 * i.e. creates (#components) new meshes and deletes the old mesh
 *
 * @param _objectId id of the target object
 */
IdList ComponentsPlugin::splitComponents( int _objectId  ) {

  // get object
  BaseObjectData *obj;
  PluginFunctions::getObject(_objectId, obj);

  if (obj == 0){
    emit log(LOGERR,"Unable to get object");
    return IdList();
  }

  QFileInfo fi(obj->name());

  if ( obj->dataType(DATA_TRIANGLE_MESH) ) {
        TriMesh* mesh = PluginFunctions::triMesh(obj);

        if ( mesh == 0 ) {
          emit log(LOGERR,"Unable to get mesh");
          return IdList();
        }

        int components = MeshInfo::componentCount( mesh );

        //create empty meshes
        IdList newIDs;

        newIDs.push_back( _objectId );

        if (components == 1){
          return newIDs;
        }

        QString currentName = obj->name();
        QString extension   = currentName.section('.', -1);
        currentName = currentName.section('.',0,-2);

        // Update name of original object
        obj->setName(currentName+"_component_"+QString::number(0)+"."+extension);


        for(int i=0; i < components-1; i++){

          // Copy original Object
          int id;
          emit copyObject(obj->id(), id);

          if (id == -1){
            emit log(LOGERR, "Unable to generate a copy of object " + QString::number(obj->id()) );
            return IdList();
          }

          // Get the target Object which will contain one component and is the copy of the original
          BaseObjectData *curObj;
          PluginFunctions::getObject(id, curObj);
          curObj->setName(currentName+"_component_"+QString::number(i+1)+"."+extension);

          TriMesh* curMesh = PluginFunctions::triMesh(curObj);

          if ( curMesh == 0 ) {
            emit log(LOGERR,"Unable to get mesh");
            return IdList();
          }

          // Takes one component out of mesh, deletes it from mesh and adds it to curMesh
          splitComponent( mesh, curMesh);

          newIDs.push_back(id);
        }

        emit updatedObject(_objectId,UPDATE_ALL);

        for (uint i=0; i < newIDs.size(); i++)
          emit updatedObject(newIDs[i],UPDATE_ALL);

        emit updateView();
        return newIDs;

  } else if( obj->dataType(DATA_POLY_MESH) ) {
        PolyMesh* mesh = PluginFunctions::polyMesh(obj);

        if ( mesh == 0 ) {
          emit log(LOGERR,"Unable to get mesh");
          return IdList();
        }

        int components = MeshInfo::componentCount( mesh );

        //create empty meshes
        std::vector< int > newIDs;

        if (components == 1){
          newIDs.push_back( _objectId );
          return newIDs;
        }

        QString currentName = obj->name();
        QString extension   = currentName.section('.', -1);
        currentName = currentName.section('.',0,-2);

        // Update name of original object
        obj->setName(currentName+"_component_"+QString::number(0)+"."+extension);

        for(int i=0; i < components-1; i++){

          // Copy original Object
          int id;
          emit copyObject(obj->id(), id);

          if (id == -1){
            emit log(LOGERR, "Unable to generate a copy of object " + QString::number(obj->id()) );
            return IdList();
          }

          // Get the target Object which will contain one component and is the copy of the original
          BaseObjectData *curObj;
          PluginFunctions::getObject(id, curObj);
          curObj->setName(currentName+"_component_"+QString::number(i+1)+"."+extension);

          PolyMesh* curMesh = PluginFunctions::polyMesh(curObj);

          if ( curMesh == 0 ) {
            emit log(LOGERR,"Unable to get mesh");
            return IdList();
          }

          // Takes one component out of mesh, deletes it from mesh and adds it to curMesh
          splitComponent( mesh, curMesh);

          newIDs.push_back(id);
        }

        emit updatedObject(_objectId,UPDATE_ALL);

        for (uint i=0; i < newIDs.size(); i++)
          emit updatedObject(newIDs[i],UPDATE_ALL);

        emit updateView();

        return newIDs;

  }else {
        emit log(LOGERR,"Splitting into components currently only supported for meshes");
        return IdList();
  }
}

void ComponentsPlugin::biggestComponent(int _objId)
{

  BaseObjectData *obj;
  PluginFunctions::getObject(_objId, obj);

  if (obj == 0){
    emit log(LOGERR,"Unable to get object");
    return;
  }
  if ( obj->dataType(DATA_TRIANGLE_MESH) )
  {
    selectBiggestComponent(PluginFunctions::triMesh(obj));
    isolateBiggestComponent(PluginFunctions::triMesh(obj));
  }
  else if ( obj->dataType(DATA_POLY_MESH) )
  {
    selectBiggestComponent(PluginFunctions::polyMesh(obj));
    isolateBiggestComponent(PluginFunctions::polyMesh(obj));
  }
}
