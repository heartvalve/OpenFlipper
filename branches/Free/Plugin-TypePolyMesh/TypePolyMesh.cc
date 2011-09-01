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


#include "TypePolyMesh.hh"

#include "OpenFlipper/BasePlugin/PluginFunctions.hh"
#include <OpenFlipper/common/BackupData.hh>
#include "PolyMeshBackup.hh"

//-----------------------------------------------------------------------------

TypePolyMeshPlugin::TypePolyMeshPlugin() {
  
}

//-----------------------------------------------------------------------------

bool TypePolyMeshPlugin::registerType() {
    
  addDataType("PolyMesh",tr("PolyMesh"));
  setTypeIcon( "PolyMesh", "PolyType.png");
  return true;
}

//-----------------------------------------------------------------------------

int TypePolyMeshPlugin::addEmpty(){
    
  // new object data struct
  PolyMeshObject * object = new PolyMeshObject( typeId("PolyMesh") );

  if ( PluginFunctions::objectCount() == 1 )
    object->target(true);

  if (PluginFunctions::targetCount() == 0 )
    object->target(true);

  QString name = get_unique_name(object);

  // call the local function to update names
  QFileInfo f(name);
  object->setName( f.fileName() );

  object->update();

  object->show();

  emit emptyObjectAdded (object->id() );

  return object->id();
}

//-----------------------------------------------------------------------------

QString TypePolyMeshPlugin::get_unique_name(PolyMeshObject* _object)
{
  bool name_unique = false;

  int cur_idx = _object->id();

  QString cur_name = QString(tr("PolyMesh new %1.off").arg( cur_idx + 1 ));

  return QString(tr("PolyMesh new %1.off").arg( cur_idx + 1 ));
}

//-----------------------------------------------------------------------------

void TypePolyMeshPlugin::generateBackup( int _id, QString _name, UpdateType _type ){
  
  BaseObjectData* object = 0;
  PluginFunctions::getObject(_id, object);
  
  PolyMeshObject* meshObj = PluginFunctions::polyMeshObject(object);
  
  if ( meshObj != 0 ){

    //get backup object data
    BackupData* backupData = 0;

    if ( object->hasObjectData( OBJECT_BACKUPS ) )
      backupData = dynamic_cast< BackupData* >(object->objectData(OBJECT_BACKUPS));
    else{
      //add backup data
      backupData = new BackupData(object);
      object->setObjectData(OBJECT_BACKUPS, backupData);
    }
    
    //store a new backup
    PolyMeshBackup* backup = new PolyMeshBackup(meshObj, _name, _type);
    
    backupData->storeBackup( backup );
  }
}

//-----------------------------------------------------------------------------

Q_EXPORT_PLUGIN2( typepolymeshplugin , TypePolyMeshPlugin );

