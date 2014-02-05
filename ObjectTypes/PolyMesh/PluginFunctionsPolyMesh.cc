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




//=============================================================================
//
//  Plugin Functions PolyMesh
//
//=============================================================================

#include <OpenFlipper/common/Types.hh>

#include <ObjectTypes/PolyMesh/PluginFunctionsPolyMesh.hh>
#include <OpenFlipper/BasePlugin/PluginFunctions.hh>



namespace PluginFunctions {


bool getSourceMeshes( std::vector<PolyMesh*>& _meshes  ) {
  _meshes.clear();

  for ( ObjectIterator o_it(PluginFunctions::ALL_OBJECTS,DATA_POLY_MESH) ; o_it != PluginFunctions::objectsEnd(); ++o_it) {
    if (! o_it->source() )
      continue;
    _meshes.push_back ( dynamic_cast< PolyMeshObject* >( *o_it )->mesh() );
  }

  return ( !_meshes.empty() );
}

bool getTargetMeshes( std::vector<PolyMesh*>& _meshes  ) {
  _meshes.clear();

  for ( ObjectIterator o_it(PluginFunctions::ALL_OBJECTS,DATA_POLY_MESH) ; o_it != PluginFunctions::objectsEnd(); ++o_it) {
    if (! o_it->target() )
      continue;
    if ( dynamic_cast< PolyMeshObject* >( *o_it )->mesh() )
      _meshes.push_back ( dynamic_cast< PolyMeshObject* >( *o_it )->mesh() );
  }

  return ( !_meshes.empty() );
}

bool getObject(  int _identifier , PolyMeshObject*& _object ) {

  if ( _identifier == BaseObject::NOOBJECT ) {
    _object = 0;
    return false;
  }

  // Get object by using the map accelerated plugin function
  BaseObjectData* object = 0;
  PluginFunctions::getObject(_identifier,object);

  _object = dynamic_cast< PolyMeshObject* >(object);
  return ( _object != 0 );
}

bool getMesh(  int _identifier , PolyMesh*& _mesh ) {

  if ( _identifier == BaseObject::NOOBJECT ) {
    _mesh = 0;
    return false;
  }

  // Get object by using the map accelerated plugin function
  BaseObjectData* object = 0;
  PluginFunctions::getObject(_identifier,object);

  // Unable to find object
  if ( object == 0)
    return false;

  PolyMeshObject* polyMeshObject = dynamic_cast< PolyMeshObject* > (object);

  // Object is not a triangle mesh
  if ( polyMeshObject == 0) {
    _mesh = 0;
    return false;
  }

  _mesh = polyMeshObject->mesh();
  return true;
}

PolyMesh* polyMesh( BaseObjectData* _object ) {
  
  if ( _object == 0 )
    return 0;

  if ( _object->dataType(DATA_POLY_MESH) ) {
    PolyMeshObject* object = dynamic_cast< PolyMeshObject* >(_object);
    return object->mesh();
  } else
    return NULL;
}

PolyMesh* polyMesh( int _identifier ) {
  PolyMeshObject* object = polyMeshObject(_identifier);

   if ( object == 0)
     return 0;
   else
     return object->mesh();
}

PolyMeshObject* polyMeshObject( BaseObjectData* _object ) {
  
  if ( _object == 0 )
    return 0;

  if ( ! _object->dataType(DATA_POLY_MESH) )
    return NULL;
  return dynamic_cast< PolyMeshObject* >( _object );
}

PolyMeshObject* polyMeshObject( int _objectId ) {
  
  if  (_objectId == BaseObject::NOOBJECT)
    return 0;
  
  // Get object by using the map accelerated plugin function
  BaseObjectData* object = 0;
  PluginFunctions::getObject(_objectId,object);
  
  if ( object == 0 )
    return 0;
  
  PolyMeshObject* meshObject = dynamic_cast< PolyMeshObject* >(object);
  
  return meshObject;
}

}
