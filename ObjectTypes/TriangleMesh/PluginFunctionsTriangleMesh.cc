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
//  Plugin Functions
//
//=============================================================================

#include <OpenFlipper/common/Types.hh>

#include <ObjectTypes/TriangleMesh/TriangleMesh.hh>
#include <OpenFlipper/BasePlugin/PluginFunctions.hh>

namespace PluginFunctions {

bool getSourceMeshes( std::vector<TriMesh*>& _meshes  ) {
  _meshes.clear();

  for ( ObjectIterator o_it(PluginFunctions::ALL_OBJECTS,DATA_TRIANGLE_MESH) ; o_it != PluginFunctions::objectsEnd(); ++o_it) {
    if (! o_it->source() )
      continue;
    _meshes.push_back ( dynamic_cast< TriMeshObject* >( *o_it )->mesh() );
  }

  return ( !_meshes.empty() );
}

bool getTargetMeshes( std::vector<TriMesh*>& _meshes  ) {
  _meshes.clear();

  for ( ObjectIterator o_it(PluginFunctions::ALL_OBJECTS,DATA_TRIANGLE_MESH) ; o_it != PluginFunctions::objectsEnd(); ++o_it) {
    if (! o_it->target() )
      continue;
    if ( dynamic_cast< TriMeshObject* >( *o_it )->mesh() )
      _meshes.push_back ( dynamic_cast< TriMeshObject* >( *o_it )->mesh() );
  }

  return ( !_meshes.empty() );
}


bool getObject(  int _identifier , TriMeshObject*& _object ) {

  if ( _identifier == BaseObject::NOOBJECT ) {
    _object = 0;
    return false;
  }

  // Get object by using the map accelerated plugin function
  BaseObjectData* object = 0;
  PluginFunctions::getObject(_identifier,object);

  _object = dynamic_cast< TriMeshObject* >(object);
  return ( _object != 0 );
}

// ===============================================================================
// ===============================================================================


bool getMesh(  int _identifier , TriMesh*& _mesh ) {

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

  TriMeshObject* triangleMeshObject = dynamic_cast< TriMeshObject* > (object);

  // Object is not a triangle mesh
  if ( triangleMeshObject == 0)
    return false;

  _mesh = triangleMeshObject->mesh();
  return true;
}


// ===============================================================================
// Getting data from objects and casting between them
// ===============================================================================

TriMesh* triMesh( BaseObjectData* _object ) {
  
  if ( _object == 0 )
    return 0;

  if ( _object->dataType(DATA_TRIANGLE_MESH) ) {
    TriMeshObject* object = dynamic_cast< TriMeshObject* >(_object);
    return object->mesh();
  } else
    return 0;
}


TriMesh* triMesh( int _identifier ) {
   TriMeshObject* object = triMeshObject(_identifier);

   if ( object == 0)
     return 0;
   else
     return object->mesh();
}

TriMeshObject* triMeshObject( BaseObjectData* _object ) {
  if ( _object == 0 )
    return 0;

  if ( ! _object->dataType(DATA_TRIANGLE_MESH) )
    return 0;
  return dynamic_cast< TriMeshObject* >( _object );
}


TriMeshObject* triMeshObject( int _objectId ) {
  if  (_objectId == BaseObject::NOOBJECT)
    return 0;
  
  // Get object by using the map accelerated plugin function
  BaseObjectData* object = 0;
  PluginFunctions::getObject(_objectId,object);
  
  if ( object == 0 )
    return 0;
  
  TriMeshObject* meshObject = dynamic_cast< TriMeshObject* >(object);
  
  return meshObject;
}


}
