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
//  Plugin Functions for Cameras
//
//=============================================================================

#include <OpenFlipper/common/Types.hh>
#include "Camera.hh"

#include "PluginFunctionsCamera.hh"
#include <OpenFlipper/BasePlugin/PluginFunctions.hh>

namespace PluginFunctions {

// ===============================================================================
// Get source cameras
// ===============================================================================


bool getSourceCameras( std::vector<CameraNode*>& _cameras  )
{
  _cameras.clear();

  for ( ObjectIterator o_it(PluginFunctions::SOURCE_OBJECTS,DATA_CAMERA ) ;
  o_it != PluginFunctions::objectsEnd(); ++o_it) {
    _cameras.push_back ( PluginFunctions::cameraNode( *o_it ) );
    if( _cameras.back() == NULL)
      std::cerr << "ERROR: Camera - get_source_cameras fatal error\n";
  }

  return ( !_cameras.empty() );
}


// ===============================================================================
// Get target cameras
// ===============================================================================


bool getTargetCameras( std::vector<CameraNode*>& _cameras  )
{
  _cameras.clear();

  for ( ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS,DATA_CAMERA ) ;
  o_it != PluginFunctions::objectsEnd(); ++o_it) {
    _cameras.push_back ( PluginFunctions::cameraNode( *o_it ) );
    if( _cameras.back() == NULL)
      std::cerr << "ERROR: Camera - getTargetPolylines fatal error\n";
  }

  return ( !_cameras.empty() );
}


// ===============================================================================
// Get objects
// ===============================================================================

bool getObject(  int _identifier , CameraObject*& _object ) {
  
  if ( _identifier == BaseObject::NOOBJECT ) {
    _object = 0;
    return false;
  }

  // Get object by using the map accelerated plugin function
  BaseObjectData* object = 0;
  PluginFunctions::getObject(_identifier,object);

  _object = dynamic_cast< CameraObject* >(object);
  return ( _object != 0 );
}


CameraObject* cameraObject( int _objectId ) {
  if ( _objectId == BaseObject::NOOBJECT ) {
    return 0;
  }
  
  // Get object by using the map accelerated plugin function
  BaseObjectData* object = 0;
  PluginFunctions::getObject(_objectId,object);

  CameraObject* cam  = dynamic_cast< CameraObject* >(object);
  return cam;
}


// ===============================================================================
// Getting data from objects and casting between them
// ===============================================================================

CameraNode* cameraNode( BaseObjectData* _object ) {
  if ( _object->dataType(DATA_CAMERA) ) {
    CameraObject* object = dynamic_cast< CameraObject* >(_object);
    return object->cameraNode();
  } else
    return 0;
}


CameraObject* cameraObject( BaseObjectData* _object ) {
  if ( ! _object->dataType(DATA_CAMERA) )
    return 0;
  return dynamic_cast< CameraObject* >( _object );
}


}
