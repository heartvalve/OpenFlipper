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
static std::map< QString , size_t > stringToUpdateTypeInfo;

/** This map maps an updateType id to its id in the types vector
 */
static std::map< UpdateType , size_t > updateTypeToTypeInfo;

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

UpdateType 
UpdateType::operator|(const UpdateType& _type) const
{
  return (type_ | _type.type_);
}


UpdateType& 
UpdateType::operator|=(const UpdateType& _type)
{
  type_ |= _type.type_;

  return(*this);
}


/// Check if this update contains the given UpdateType
bool UpdateType::contains( const UpdateType& _type ) const {
  
  if ( type_ == UPDATE_ALL.type_ )
    return true;
  
  // Catch the specialization of updates
  if ( _type == UPDATE_SELECTION ) {
    if ( type_ == UPDATE_SELECTION_VERTICES.type_ || type_ == UPDATE_SELECTION_EDGES.type_ ||  type_ == UPDATE_SELECTION_HALFEDGES.type_ || type_ == UPDATE_SELECTION_FACES.type_ || type_ == UPDATE_SELECTION_KNOTS.type_ )
      return true;
  }  
  
  return ((type_ & _type.type_).any());
}

UpdateType& UpdateType::operator++() {
  if ( type_.count() != 1 ) {
    std::cerr << "Operator ++ for UpdateType which is not atomic!!" << std::endl;
  }
  
  type_ <<= 1;
  
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
  
  stringToUpdateTypeInfo["HalfedgeSelection"]    = updateTypes.size();
  updateTypeToTypeInfo[UPDATE_SELECTION_HALFEDGES]   = updateTypes.size();
  updateTypes.push_back( UpdateTypeInfo(UPDATE_SELECTION_HALFEDGES, "HalfedgeSelection", true) );

  stringToUpdateTypeInfo["FaceSelection"]      = updateTypes.size();
  updateTypeToTypeInfo[UPDATE_SELECTION_FACES]   = updateTypes.size();
  updateTypes.push_back( UpdateTypeInfo(UPDATE_SELECTION_FACES, "FaceSelection", true) );
  
  stringToUpdateTypeInfo["KnotSelection"]      = updateTypes.size();
  updateTypeToTypeInfo[UPDATE_SELECTION_KNOTS] = updateTypes.size();
  updateTypes.push_back( UpdateTypeInfo(UPDATE_SELECTION_KNOTS, "KnotSelection", true) );

  stringToUpdateTypeInfo["Color"]      = updateTypes.size();
  updateTypeToTypeInfo[UPDATE_COLOR]   = updateTypes.size();
  updateTypes.push_back( UpdateTypeInfo(UPDATE_COLOR, "Color", true) );
  
  stringToUpdateTypeInfo["Texture"]      = updateTypes.size();
  updateTypeToTypeInfo[UPDATE_TEXTURE]   = updateTypes.size();
  updateTypes.push_back( UpdateTypeInfo(UPDATE_TEXTURE, "Texture", true) );

  stringToUpdateTypeInfo["State"]      = updateTypes.size();
  updateTypeToTypeInfo[UPDATE_STATE]   = updateTypes.size();
  updateTypes.push_back( UpdateTypeInfo(UPDATE_STATE, "State", true) );

  updateTypeToString[UPDATE_ALL]                = "All";
  updateTypeToString[UPDATE_VISIBILITY]         = "Visibility";
  updateTypeToString[UPDATE_GEOMETRY]           = "Geometry";
  updateTypeToString[UPDATE_TOPOLOGY]           = "Topology";
  updateTypeToString[UPDATE_SELECTION]          = "Selection";
  updateTypeToString[UPDATE_SELECTION_VERTICES] = "VertexSelection";
  updateTypeToString[UPDATE_SELECTION_EDGES]    = "EdgeSelection";
  updateTypeToString[UPDATE_SELECTION_HALFEDGES]= "HalfedgeSelection";
  updateTypeToString[UPDATE_SELECTION_FACES]    = "FaceSelection";
  updateTypeToString[UPDATE_SELECTION_KNOTS]    = "KnotSelection";
  updateTypeToString[UPDATE_COLOR]              = "Color";
  updateTypeToString[UPDATE_TEXTURE]            = "Texture";
  updateTypeToString[UPDATE_STATE]              = "State";
}

/// Adds a updateType and returns the id for the new type
UpdateType addUpdateType(QString _name, bool _resetNeeded) {

  //first check if it's already available
  std::map<QString, size_t>::iterator index = stringToUpdateTypeInfo.find( _name );

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

  std::map<QString, size_t>::iterator index = stringToUpdateTypeInfo.find( _name );

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
size_t updateTypeCount() {
  return updateTypes.size();
}
