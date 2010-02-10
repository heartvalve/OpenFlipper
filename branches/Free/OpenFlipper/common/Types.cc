/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2009 by Computer Graphics Group, RWTH Aachen      *
 *                           www.openflipper.org                             *
 *                                                                           *
 *---------------------------------------------------------------------------*
 *  This file is part of OpenFlipper.                                        *
 *                                                                           *
 *  OpenFlipper is free software: you can redistribute it and/or modify      *
 *  it under the terms of the GNU Lesser General Public License as           *
 *  published by the Free Software Foundation, either version 3 of           *
 *  the License, or (at your option) any later version with the              *
 *  following exceptions:                                                    *
 *                                                                           *
 *  If other files instantiate templates or use macros                       *
 *  or inline functions from this file, or you compile this file and         *
 *  link it with other files to produce an executable, this file does        *
 *  not by itself cause the resulting executable to be covered by the        *
 *  GNU Lesser General Public License. This exception does not however       *
 *  invalidate any other reasons why the executable file might be            *
 *  covered by the GNU Lesser General Public License.                        *
 *                                                                           *
 *  OpenFlipper is distributed in the hope that it will be useful,           *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU Lesser General Public License for more details.                      *
 *                                                                           *
 *  You should have received a copy of the GNU LesserGeneral Public          *
 *  License along with OpenFlipper. If not,                                  *
 *  see <http://www.gnu.org/licenses/>.                                      *
 *                                                                           *
\*===========================================================================*/

/*===========================================================================*\
 *                                                                           *
 *   $Revision$                                                         *
 *   $Author$                                                      *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/




//=============================================================================
//
//  Types
//
//=============================================================================

/**
 * \file Types.cc
 * This File contains the basic functions to handle datatypes
 */


//== INCLUDES =================================================================

#include "TypesInternal.hh"
#include "DataTypes.hh"
#include <map>
#include <QCoreApplication>
#include <OpenFlipper/common/GlobalOptions.hh>


/** This field defines the start id for custom datatypes. It starts high to avoid conflicts with previously
 * hardcoded versions.
 */
static int nextTypeId_ = 2;

/** This map maps an dataType id to an typeName
 */
static std::map< DataType, QString > typeToString;

/** This map maps an dataType name to its id in the types vector
 */
static std::map< QString , unsigned int > stringToTypeInfo;

/** This map maps an dataType id to its id in the types vector
 */
static std::map< DataType , unsigned int > typeToTypeInfo;

class TypeInfo {

  public:

  TypeInfo(DataType _type, QString _name, QString _iconName, QString _readableName ) :
    type(_type),
    name(_name),
    iconName(_iconName),
    readableName(_readableName)
  {
    // Use internal name if no external name is given
    if ( _readableName == "" )
      readableName = _name;
  }

  /// The id of the datatype
  DataType type;

  /// The name of the datatype
  QString  name;

  /// The icon of the datatype
  QString  iconName;
  
  QIcon    icon;

  /// Human readable name
  QString readableName;
};

static QIcon dummyIcon;

static std::vector< TypeInfo > types;

//== Functions =========================================================

std::ostream &operator<<(std::ostream &stream, DataType type)
{
  stream << type.value() ;
  
  return stream;
}

void initializeTypes() {
  stringToTypeInfo["Unknown"] = types.size();
  DataType test(DATA_UNKNOWN);
  typeToTypeInfo[test]   = types.size();
  types.push_back( TypeInfo(DATA_UNKNOWN            ,"Unknown"        ,"Unknown.png", QCoreApplication::translate("Types","Unknown")) );

  stringToTypeInfo["Group"]  = types.size();
  typeToTypeInfo[DATA_GROUP] = types.size();
  types.push_back( TypeInfo(DATA_GROUP           ,"Group"          ,"group.png", QCoreApplication::translate("Types","Group")) );

  stringToTypeInfo["All"]  = types.size();
  typeToTypeInfo[DATA_ALL] = types.size();
  types.push_back( TypeInfo(DATA_ALL             ,"All"            ,"Unknown.png", QCoreApplication::translate("Types","All")) );

  typeToString[DATA_UNKNOWN] = "Unknown";
  typeToString[DATA_GROUP] = "Group";
  typeToString[DATA_ALL] = "All";
  
  // Preload the static icons
  setTypeIcon(DATA_GROUP,"group.png");
}

/// Adds a datatype and returns the id for the new type
DataType addDataType(QString _name, QString _readableName) {
  
  int type = nextTypeId_;

  stringToTypeInfo[ _name ] = types.size();
  typeToTypeInfo[ type ] = types.size();
  types.push_back( TypeInfo(type, _name, "Unknown.png", _readableName ));

  typeToString[type] = _name;

  nextTypeId_ *= 2;
  return( type );
}

/// Get the id of a type with given name
DataType typeId(QString _name) {

  std::map<QString, unsigned int>::iterator index = stringToTypeInfo.find( _name );

  if ( index != stringToTypeInfo.end() )
    return types[ index->second ].type;
  else {
    #ifdef DEBUG
      std::cerr << "Unknown Data type with name " << _name.toStdString() << std::endl;
    #endif
    return DATA_UNKNOWN;
  }
}

/// Get the name of a type with given id
QString typeName(DataType _id) {

  std::map<DataType, QString>::iterator name = typeToString.find(_id);

  if ( name != typeToString.end() )
    return name->second;
  else {
    #ifdef DEBUG
    std::cerr << "Unable to retrieve typeName for id " << _id << std::endl;
    #endif
    return "Unknown";
  }
}


/// Return the number of registered types
uint typeCount() {
  return types.size();
}

/// Get the icon of a given dataType
QString typeIconName(QString  _name) {

  std::map<QString, unsigned int>::iterator index = stringToTypeInfo.find( _name );

  if ( index != stringToTypeInfo.end() )
    return types[ index->second ].iconName;
  else
    return "Unknown.png";
}

/// get the icon of a given dataType
QString typeIconName(DataType _id) {

  std::map<DataType, unsigned int>::iterator index = typeToTypeInfo.find(_id);

  if ( index != typeToTypeInfo.end() )
    return types[ index->second ].iconName;
  else
    return "Unknown.png";
}

/// get the icon of a given dataType
QIcon& typeIcon(DataType _id) {
  
  std::map<DataType, unsigned int>::iterator index = typeToTypeInfo.find(_id);
  
  if ( index != typeToTypeInfo.end() )
    return types[ index->second ].icon;
  else
    return dummyIcon;
}

/// Set the icon for a given dataType
void setTypeIcon( DataType _id   , QString _icon ) {

  std::map<DataType, unsigned int>::iterator index = typeToTypeInfo.find(_id);

  if ( index != typeToTypeInfo.end() ) {
    types[ index->second ].iconName = _icon;
    types[ index->second ].icon = QIcon( OpenFlipper::Options::iconDirStr() + QDir::separator() + _icon );
  } else
    std::cerr << "Could not set icon for DataType. Type not found!" << std::endl;
}

/// Set the icon for a given dataType
void setTypeIcon( QString  _name , QString _icon ) {

  std::map<QString, unsigned int>::iterator index = stringToTypeInfo.find( _name );

  if ( index != stringToTypeInfo.end() ) {
    types[ index->second ].iconName = _icon;
    types[ index->second ].icon = QIcon( OpenFlipper::Options::iconDirStr() + QDir::separator() + _icon );
  } else
    std::cerr << "Could not set icon for DataType. Type not found!" << std::endl;
}




/// Get DataType Human readable name ( this name might change. Use the typeName insted! )
QString dataTypeName( DataType _id ) {

  std::map<DataType, unsigned int>::iterator index = typeToTypeInfo.find(_id);

  if ( index != typeToTypeInfo.end() )
    return types[ index->second ].readableName ;
  else
    std::cerr << "Could not get human name for DataType. Type not found!" << std::endl;

  return QString(QCoreApplication::translate("Types","Unknown Type"));
}

/// Get DataType Human readable name ( this name might change. Use the typeName insted! )
QString dataTypeName( QString  _typeName ) {

  std::map<QString, unsigned int>::iterator index = stringToTypeInfo.find( _typeName );

  if ( index != stringToTypeInfo.end() )
    return types[ index->second ].readableName ;
  else
    std::cerr << "Could not get human name for DataType. Type not found!" << std::endl;

  return QString(QCoreApplication::translate("Types","Unknown Type"));
}



/// Set the icon for a given dataType
void setDataTypeName( DataType _id   , QString _name ) {

  std::map<DataType, unsigned int>::iterator index = typeToTypeInfo.find(_id);

  if ( index != typeToTypeInfo.end() )
    types[ index->second ].readableName = _name;
  else
    std::cerr << "Could not set human name for DataType. Type not found!" << std::endl;
}

/// Set the icon for a given dataType
void setDataTypeName( QString  _typeName , QString _name ) {

  std::map<QString, unsigned int>::iterator index = stringToTypeInfo.find( _typeName );

  if ( index != stringToTypeInfo.end() )
    types[ index->second ].readableName = _name;
  else
    std::cerr << "Could not set human name for DataType. Type not found!" << std::endl;
}


DataType::DataType():
  field(0)
{
};

DataType::DataType(const unsigned int& _i):
  field(_i)
{
};

//===========================================

bool DataType::operator!=( const unsigned int& _i ) { 
  return (_i != field); 
}

bool DataType::operator!=( const DataType& _i ) {
  return (field != _i.field); 
}    

//===========================================

bool DataType::operator==( const unsigned int& _i ) {
  return (_i == field); 
}

bool DataType::operator==(  const DataType& _i ) {
  return (_i.field == field); 
}

//===========================================

bool DataType::operator=( const unsigned int& _i ) {
  return (field = _i); 
}

bool DataType::operator=( const DataType& _i ) {
  return (field = _i.field); 
}    

//===========================================

bool DataType::operator<( const unsigned int& _i ) {
  return (field < _i); 
}

bool DataType::operator<( const DataType& _i ) const {
  return (field < _i.field); 
}

//===========================================

bool DataType::operator&( const unsigned int& _i ) {
  return (field & _i); 
}

bool DataType::operator&( const DataType& _i ) const {
  return (field & _i.field); 
}    

//===========================================

DataType DataType::operator|( const DataType& _i ) const {
  return (field | _i.field); 
}    

//===========================================

bool DataType::operator++(int _unused) {
  return (field *= 2); 
}  

//===========================================

unsigned int DataType::value() const {
  return( field );
}

QString DataType::name() {
  return typeName(field);
}


//=============================================================================
//=============================================================================
