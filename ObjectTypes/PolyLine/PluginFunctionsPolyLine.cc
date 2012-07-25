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
//  Plugin Functions for PolyLines
//
//=============================================================================

#include <OpenFlipper/common/Types.hh>
#include "PolyLine.hh"

#include "PluginFunctionsPolyLine.hh"
#include <OpenFlipper/BasePlugin/PluginFunctions.hh>

namespace PluginFunctions {

// ===============================================================================
// Get source polylines
// ===============================================================================


bool getSourcePolylines( std::vector<PolyLine*>& _polylines  )
{
  _polylines.clear();

  for ( ObjectIterator o_it(PluginFunctions::SOURCE_OBJECTS,DATA_POLY_LINE ) ;
	o_it != PluginFunctions::objectsEnd(); ++o_it) {
    _polylines.push_back ( PluginFunctions::polyLine( *o_it ) );
    if( _polylines.back() == 0)
      std::cerr << "ERROR: Polyine get_source_polylines fatal error\n";
  }

  return (_polylines.size() > 0 );
}


// ===============================================================================
// Get target polylines
// ===============================================================================


bool getTargetPolylines( std::vector<PolyLine*>& _polylines  )
{
  _polylines.clear();

  for ( ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS,DATA_POLY_LINE ) ;
	o_it != PluginFunctions::objectsEnd(); ++o_it) {
    _polylines.push_back ( PluginFunctions::polyLine( *o_it ) );
    if( _polylines.back() == 0)
      std::cerr << "ERROR: Polyine getTargetPolylines fatal error\n";
  }

  return (_polylines.size() > 0 );
}


// ===============================================================================
// Get objects
// ===============================================================================

bool getObject(  int _identifier , PolyLineObject*& _object ) {
  
  if ( _identifier == -1 ) {
    _object = 0;
    return false;
  }

  // Get object by using the map accelerated plugin function
  BaseObjectData* object = 0;
  PluginFunctions::getObject(_identifier,object);

  _object = dynamic_cast< PolyLineObject* >(object);
  return ( _object != 0 );
}


// ===============================================================================
// Getting data from objects and casting between them
// ===============================================================================

PolyLine* polyLine( BaseObjectData* _object ) {
  if ( _object->dataType(DATA_POLY_LINE) ) {
    PolyLineObject* object = dynamic_cast< PolyLineObject* >(_object);
    return object->line();
  } else
    return 0;
}


PolyLineObject* polyLineObject( BaseObjectData* _object ) {
  if ( ! _object->dataType(DATA_POLY_LINE) )
    return 0;
  return dynamic_cast< PolyLineObject* >( _object );
}

PolyLineObject* polyLineObject( int _objectId ) {
  
  if  (_objectId == -1)
    return 0;
  
  // Get object by using the map accelerated plugin function
  BaseObjectData* object = 0;
  PluginFunctions::getObject(_objectId,object);
  
  if ( object == 0 )
    return 0;
  
  PolyLineObject* meshObject = dynamic_cast< PolyLineObject* >(object);
  
  return meshObject;
}


}
