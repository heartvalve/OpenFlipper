//=============================================================================
//
//  Plugin Functions for PolyhedralMeshes
//
//=============================================================================

#include <OpenFlipper/common/Types.hh>

#include <ObjectTypes/PolyhedralMesh/PluginFunctionsPolyhedralMesh.hh>
#include <OpenFlipper/BasePlugin/PluginFunctions.hh>

namespace PluginFunctions {

// ===============================================================================
// Get source meshes
// ===============================================================================


bool getSourceMeshes( std::vector<PolyhedralMesh*>& _meshes  )
{
  _meshes.clear();

  for ( ObjectIterator o_it(PluginFunctions::SOURCE_OBJECTS,DATA_POLYHEDRAL_MESH) ;
    o_it != PluginFunctions::objectsEnd(); ++o_it) {
    _meshes.push_back ( PluginFunctions::polyhedralMesh( *o_it ) );
    if( _meshes.back() == NULL)
      std::cerr << "ERROR: PolyhedralMesh getSourceMeshes fatal error\n";
  }

  return (_meshes.size() > 0 );
}


// ===============================================================================
// Get target meshes
// ===============================================================================


bool getTargetMeshes( std::vector<PolyhedralMesh*>& _meshes  )
{
  _meshes.clear();

  for ( ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS,DATA_POLYHEDRAL_MESH ) ;
    o_it != PluginFunctions::objectsEnd(); ++o_it) {
    _meshes.push_back ( PluginFunctions::polyhedralMesh( *o_it ) );
    if( _meshes.back() == NULL)
      std::cerr << "ERROR: PolyhedralMesh getTargetMeshes fatal error\n";
  }

  return (_meshes.size() > 0 );
}


// ===============================================================================
// Get objects
// ===============================================================================

bool getObject( int _identifier, PolyhedralMeshObject*& _object ) {

  if ( _identifier == -1 ) {
    _object = 0;
    return false;
  }

  BaseObject* object = objectRoot()->childExists( _identifier );
  _object = dynamic_cast< PolyhedralMeshObject* >(object);
  return ( _object != 0 );
}


// ===============================================================================
// Getting data from objects and casting between them
// ===============================================================================

PolyhedralMesh* polyhedralMesh( BaseObjectData* _object ) {

  if ( _object->dataType(DATA_POLYHEDRAL_MESH) ) {
    PolyhedralMeshObject* object = dynamic_cast< PolyhedralMeshObject* >(_object);
    return object->mesh();
  } else
    return 0;
}


PolyhedralMeshObject* polyhedralMeshObject( BaseObjectData* _object ) {
  if ( ! _object->dataType(DATA_POLYHEDRAL_MESH) )
    return 0;
  return dynamic_cast< PolyhedralMeshObject* >( _object );
}


PolyhedralMeshObject* polyhedralMeshObject( int _identifier )
{
  PolyhedralMeshObject* pol_obj;
  if(getObject(_identifier, pol_obj))
    return pol_obj;
  else
    return 0;
}


}
