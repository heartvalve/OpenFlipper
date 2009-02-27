//=============================================================================
//
//                               OpenFlipper
//        Copyright (C) 2008 by Computer Graphics Group, RWTH Aachen
//                           www.openflipper.org
//
//-----------------------------------------------------------------------------
//
//                                License
//
//  OpenFlipper is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  OpenFlipper is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License
//  along with OpenFlipper.  If not, see <http://www.gnu.org/licenses/>.
//
//-----------------------------------------------------------------------------
//
//   $Revision$
//   $Author$
//   $Date$
//
//=============================================================================




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

  return (_meshes.size() > 0 );
}

bool getTargetMeshes( std::vector<TriMesh*>& _meshes  ) {
  _meshes.clear();

  for ( ObjectIterator o_it(PluginFunctions::ALL_OBJECTS,DATA_TRIANGLE_MESH) ; o_it != PluginFunctions::objectsEnd(); ++o_it) {
    if (! o_it->target() )
      continue;
    if ( dynamic_cast< TriMeshObject* >( *o_it )->mesh() )
      _meshes.push_back ( dynamic_cast< TriMeshObject* >( *o_it )->mesh() );
  }

  return (_meshes.size() > 0 );
}


bool getObject(  int _identifier , TriMeshObject*& _object ) {

  if ( _identifier == -1 )
    return false;

  BaseObject* object = objectRoot()->childExists( _identifier );
  _object = dynamic_cast< TriMeshObject* >(object);
  return ( _object != 0 );
}

// ===============================================================================
// ===============================================================================


bool getMesh(  int _identifier , TriMesh*& _mesh ) {

  if ( _identifier == -1 )
    return false;

  BaseObject* object = objectRoot()->childExists( _identifier );

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
    return NULL;
}

TriMeshObject* triMeshObject( BaseObjectData* _object ) {
  if ( _object == 0 )
    return 0;

  if ( ! _object->dataType(DATA_TRIANGLE_MESH) )
    return NULL;
  return dynamic_cast< TriMeshObject* >( _object );
}

}
