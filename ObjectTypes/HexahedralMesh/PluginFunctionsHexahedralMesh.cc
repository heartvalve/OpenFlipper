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
 *   $Revision: 13489 $                                                       *
 *   $LastChangedBy: kremer $                                                 *
 *   $Date: 2012-01-25 12:30:09 +0100 (Wed, 25 Jan 2012) $                    *
 *                                                                            *
 \*===========================================================================*/

#include <OpenFlipper/common/Types.hh>

#include <ObjectTypes/HexahedralMesh/PluginFunctionsHexahedralMesh.hh>
#include <OpenFlipper/BasePlugin/PluginFunctions.hh>

namespace PluginFunctions {

// ===============================================================================
// Get source meshes
// ===============================================================================


bool getSourceMeshes( std::vector<HexahedralMesh*>& _meshes  )
{
  _meshes.clear();

  for ( ObjectIterator o_it(PluginFunctions::SOURCE_OBJECTS,DATA_HEXAHEDRAL_MESH) ;
    o_it != PluginFunctions::objectsEnd(); ++o_it) {
    _meshes.push_back ( PluginFunctions::hexahedralMesh( *o_it ) );
    if( _meshes.back() == NULL)
      std::cerr << "ERROR: HexahedralMesh getSourceMeshes fatal error\n";
  }

  return ( !_meshes.empty() );
}


// ===============================================================================
// Get target meshes
// ===============================================================================


bool getTargetMeshes( std::vector<HexahedralMesh*>& _meshes  )
{
  _meshes.clear();

  for ( ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS,DATA_HEXAHEDRAL_MESH ) ;
    o_it != PluginFunctions::objectsEnd(); ++o_it) {
    _meshes.push_back ( PluginFunctions::hexahedralMesh( *o_it ) );
    if( _meshes.back() == NULL)
      std::cerr << "ERROR: HexahedralMesh getTargetMeshes fatal error\n";
  }

  return ( !_meshes.empty() );
}


// ===============================================================================
// Get objects
// ===============================================================================

bool getObject( int _identifier, HexahedralMeshObject*& _object ) {

  if ( _identifier == -1 ) {
    _object = 0;
    return false;
  }

  // Get object by using the map accelerated plugin function
  BaseObjectData* object = 0;
  PluginFunctions::getObject(_identifier,object);

  _object = dynamic_cast< HexahedralMeshObject* >(object);
  return ( _object != 0 );
}


// ===============================================================================
// Getting data from objects and casting between them
// ===============================================================================

HexahedralMesh* hexahedralMesh( BaseObjectData* _object ) {

  if ( _object->dataType(DATA_HEXAHEDRAL_MESH) ) {
    HexahedralMeshObject* object = dynamic_cast< HexahedralMeshObject* >(_object);
    return object->mesh();
  } else
    return 0;
}


HexahedralMeshObject* hexahedralMeshObject( BaseObjectData* _object ) {
  if ( ! _object->dataType(DATA_HEXAHEDRAL_MESH) )
    return 0;
  return dynamic_cast< HexahedralMeshObject* >( _object );
}


HexahedralMeshObject* hexahedralMeshObject( int _identifier )
{
  HexahedralMeshObject* pol_obj;
  if(getObject(_identifier, pol_obj))
    return pol_obj;
  else
    return 0;
}


}
