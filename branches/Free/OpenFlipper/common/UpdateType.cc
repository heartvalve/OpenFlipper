#include "TypesInternal.hh"
#include "UpdateType.hh"
#include <map>
#include <QCoreApplication>
#include <OpenFlipper/common/GlobalOptions.hh>


/** This map maps an updateType id to an typeName
 */
static std::map< UpdateType, QString > updateTypeToString;

/** This map maps an updateType name to its id in the types vector
 */
static std::map< QString , unsigned int > stringToUpdateTypeInfo;

/** This map maps an updateType id to its id in the types vector
 */
static std::map< UpdateType , unsigned int > updateTypeToTypeInfo;

/** This field defines the start id for custom updatetypes.
*/
static UpdateType firstFreeID_(UPDATE_UNUSED);

UpdateType::UpdateType(const UpdateType& _type) 
:type_(_type.type_)
{
  
}

UpdateType::UpdateType(UpdateTypeSet _set)
: type_(_set)
{
  
}

bool UpdateType::operator==(const UpdateType& _type) const {
  return ((type_ & _type.type_).any());
};

/// Check if this update contains the given UpdateType
bool UpdateType::contains( const UpdateType& _type ) {
  // Catch the specialization of updates
  if ( _type == UPDATE_SELECTION ) {
    if ( type_ == UPDATE_SELECTION_VERTICES.type_ || type_ == UPDATE_SELECTION_EDGES.type_ || type_ == UPDATE_SELECTION_FACES.type_ )
      return true;
  }  
  
  return ((type_ & _type.type_).any());
}

UpdateType& UpdateType::operator++() {
  if ( type_.count() != 1 ) {
    std::cerr << "Operator ++ for UpdateType which is not atomic!!" << std::endl;
  }
  
  type_ << 1;
  
  return (*this);
}

bool UpdateType::operator<( const UpdateType& _i ) const {
  return (type_.to_ulong() < _i.type_.to_ulong()); 
}

class UpdateTypeInfo {

  public:

  UpdateTypeInfo(UpdateType _type, QString _name, bool _needsScenegraphReset ) :
    type(_type),
    name(_name),
    resetNeeded(_needsScenegraphReset)
  {
  }

  /// The id of the UpdateType
  UpdateType type;

  /// The name of the UpdateType
  QString  name;

  /// is a sceneGraph reset needed for this update
  bool resetNeeded;
};

static std::vector< UpdateTypeInfo > updateTypes;


// functions -------------------------------------------------


void initializeUpdateTypes() {

  stringToUpdateTypeInfo["All"]      = updateTypes.size();
  updateTypeToTypeInfo[UPDATE_ALL]   = updateTypes.size();
  updateTypes.push_back( UpdateTypeInfo(UPDATE_ALL, "All", true) );

  stringToUpdateTypeInfo["ObjectSelection"]      = updateTypes.size();
  updateTypeToTypeInfo[UPDATE_OBJECT_SELECTION]  = updateTypes.size();
  updateTypes.push_back( UpdateTypeInfo(UPDATE_OBJECT_SELECTION, "ObjectSelection", false) );

  stringToUpdateTypeInfo["Visibility"]      = updateTypes.size();
  updateTypeToTypeInfo[UPDATE_VISIBILITY]   = updateTypes.size();
  updateTypes.push_back( UpdateTypeInfo(UPDATE_VISIBILITY, "Visibility", true) );
  
  stringToUpdateTypeInfo["Geometry"]      = updateTypes.size();
  updateTypeToTypeInfo[UPDATE_GEOMETRY]   = updateTypes.size();
  updateTypes.push_back( UpdateTypeInfo(UPDATE_GEOMETRY, "Geometry", true) );
  
  stringToUpdateTypeInfo["Topology"]      = updateTypes.size();
  updateTypeToTypeInfo[UPDATE_TOPOLOGY]   = updateTypes.size();
  updateTypes.push_back( UpdateTypeInfo(UPDATE_TOPOLOGY, "Topology", true) );
  
  stringToUpdateTypeInfo["Selection"]      = updateTypes.size();
  updateTypeToTypeInfo[UPDATE_SELECTION]   = updateTypes.size();
  updateTypes.push_back( UpdateTypeInfo(UPDATE_SELECTION, "Selection", true) );
  
  stringToUpdateTypeInfo["VertexSelection"]      = updateTypes.size();
  updateTypeToTypeInfo[UPDATE_SELECTION_VERTICES]   = updateTypes.size();
  updateTypes.push_back( UpdateTypeInfo(UPDATE_SELECTION_VERTICES, "VertexSelection", true) );
  
  stringToUpdateTypeInfo["EdgeSelection"]      = updateTypes.size();
  updateTypeToTypeInfo[UPDATE_SELECTION_EDGES]   = updateTypes.size();
  updateTypes.push_back( UpdateTypeInfo(UPDATE_SELECTION_EDGES, "EdgeSelection", true) );
  
  stringToUpdateTypeInfo["FaceSelection"]      = updateTypes.size();
  updateTypeToTypeInfo[UPDATE_SELECTION_FACES]   = updateTypes.size();
  updateTypes.push_back( UpdateTypeInfo(UPDATE_SELECTION_FACES, "FaceSelection", true) );

  
  updateTypeToString[UPDATE_ALL]                = "All";
  updateTypeToString[UPDATE_OBJECT_SELECTION]   = "ObjectSelection";
  updateTypeToString[UPDATE_VISIBILITY]         = "Visibility";
  updateTypeToString[UPDATE_GEOMETRY]           = "Geometry";
  updateTypeToString[UPDATE_TOPOLOGY]           = "Topology";
  updateTypeToString[UPDATE_SELECTION]          = "Selection";
  updateTypeToString[UPDATE_SELECTION_VERTICES] = "VertexSelection";
  updateTypeToString[UPDATE_SELECTION_EDGES]    = "EdgeSelection";
  updateTypeToString[UPDATE_SELECTION_FACES]    = "FaceSelection";
}

/// Adds a updateType and returns the id for the new type
UpdateType addUpdateType(QString _name, bool _resetNeeded) {

  //first check if it's already available
  std::map<QString, unsigned int>::iterator index = stringToUpdateTypeInfo.find( _name );

  if ( index != stringToUpdateTypeInfo.end() )
    return updateTypes[ index->second ].type;
  else {
  
    UpdateType type = firstFreeID_;

    stringToUpdateTypeInfo[ _name ] = updateTypes.size();
    updateTypeToTypeInfo[ type ] = updateTypes.size();
    updateTypes.push_back( UpdateTypeInfo(type, _name, _resetNeeded ) );

    updateTypeToString[type] = _name;

    ++firstFreeID_;
    return( type );
  }
}

/// Get the id of a type with given name
UpdateType updateType(QString _name) {

  std::map<QString, unsigned int>::iterator index = stringToUpdateTypeInfo.find( _name );

  if ( index != stringToUpdateTypeInfo.end() )
    return updateTypes[ index->second ].type;
  else {
    #ifdef DEBUG
      std::cerr << "Unknown UpdateType with name " << _name.toStdString() << std::endl;
    #endif
    return UPDATE_ALL;
  }
}

/// Get the name of a type with given id
QString updateTypeName(UpdateType _id) {

  std::map<UpdateType, QString>::iterator name = updateTypeToString.find(_id);

  if ( name != updateTypeToString.end() )
    return name->second;
  else {
    #ifdef DEBUG
    std::cerr << "Unable to retrieve updateTypeName" << std::endl;
    #endif
    return "Unknown";
  }
}

/// Return the number of registered types
uint updateTypeCount() {
  return updateTypes.size();
}