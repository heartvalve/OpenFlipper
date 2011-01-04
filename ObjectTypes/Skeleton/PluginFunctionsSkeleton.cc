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
//   $Revision: 1720 $
//   $Author: moebius $
//   $Date: 2008-05-09 14:15:53 +0200 (Fri, 09 May 2008) $
//
//=============================================================================




//=============================================================================
//
//  Plugin Functions for Skeleton
//
//=============================================================================

#include <OpenFlipper/common/Types.hh>
#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include "Skeleton.hh"

namespace PluginFunctions {

// ===============================================================================
// Get objects
// ===============================================================================

bool getObject(  int _identifier , SkeletonObject*& _object ) {
  if  (_identifier == -1)
    return false;

  BaseObject* object = objectRoot()->childExists( _identifier );
  _object = dynamic_cast< SkeletonObject* >(object);
  return ( _object != 0 );
}


// ===============================================================================
// Getting data from objects and casting between them
// ===============================================================================

Skeleton* skeleton( BaseObjectData* _object ) {
  if ( _object && _object->dataType(DATA_SKELETON) ) {
    SkeletonObject* object = dynamic_cast< SkeletonObject* >(_object);
    return object->skeleton();
  } else
    return NULL;
}


SkeletonObject* skeletonObject( BaseObjectData* _object ) {
  if ( !_object || ! _object->dataType(DATA_SKELETON) )
    return NULL;
  return dynamic_cast< SkeletonObject* >( _object );
}


}
