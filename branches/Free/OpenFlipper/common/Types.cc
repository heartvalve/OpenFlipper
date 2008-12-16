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
//   $Revision: 3957 $
//   $Author: moebius $
//   $Date: 2008-12-12 15:45:50 +0100 (Fr, 12. Dez 2008) $
//
//=============================================================================




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

#include "Types.hh"
#include <map>


/** This field defines the start id for custom datatypes. It starts high to avoid conflicts with previously
 * hardcoded versions.
 */
static int nextTypeId_ = 2048;

/// Variable used to automatically initialize the maps
static bool initialized_ = false;

/** This map maps an dataType id to an typeName
 */
static std::map< unsigned int, QString > typeToString;

/** This map maps an dataType name to its id in the types vector
 */
static std::map< QString , unsigned int > stringToTypeInfo;

/** This map maps an dataType id to its id in the types vector
 */
static std::map< unsigned int , unsigned int > typeToTypeInfo;

class TypeInfo {

  public:

  TypeInfo(DataType _type, QString _name, QString _iconName) :
    type(_type),
    name(_name),
    iconName(_iconName)
  {}

  /// The id of the datatype
  DataType type;

  /// The name of the datatype
  QString  name;

  /// The icon of the datatype
  QString  iconName;
};

static std::vector< TypeInfo > types;

//== Functions =========================================================

void initialize() {
  if ( !initialized_ ) {
    stringToTypeInfo["Unknown"] = types.size();
    typeToTypeInfo[DATA_NONE]   = types.size();
    types.push_back( TypeInfo(DATA_NONE            ,"Unknown"        ,"Unknown.png") );
    stringToTypeInfo["Group"]  = types.size();
    typeToTypeInfo[DATA_GROUP] = types.size();
    types.push_back( TypeInfo(DATA_GROUP           ,"Group"          ,"Unknown.png") );
    stringToTypeInfo["Points"]  = types.size();
    typeToTypeInfo[DATA_POINTS] = types.size();
    types.push_back( TypeInfo(DATA_POINTS          ,"Points"         ,"Unknown.png") );

    stringToTypeInfo["PolyLine"]   = types.size();
    typeToTypeInfo[DATA_POLY_LINE] = types.size();
    types.push_back( TypeInfo(DATA_POLY_LINE       ,"PolyLine"       ,"PolyLineType.png") );
    stringToTypeInfo["TriangleMesh"]   = types.size();
    typeToTypeInfo[DATA_TRIANGLE_MESH] = types.size();
    types.push_back( TypeInfo(DATA_TRIANGLE_MESH   ,"TriangleMesh"   ,"TriangleType.png") );
    stringToTypeInfo["PolyMesh"]   = types.size();
    typeToTypeInfo[DATA_POLY_MESH] = types.size();
    types.push_back( TypeInfo(DATA_POLY_MESH       ,"PolyMesh"       ,"PolyType.png") );

    stringToTypeInfo["BSplineCurve"]   = types.size();
    typeToTypeInfo[DATA_BSPLINE_CURVE] = types.size();
    types.push_back( TypeInfo(DATA_BSPLINE_CURVE   ,"BSplineCurve"   ,"BSplineCurveType.png") );
    stringToTypeInfo["Volume"]  = types.size();
    typeToTypeInfo[DATA_VOLUME] = types.size();
    types.push_back( TypeInfo(DATA_VOLUME          ,"Volume"         ,"Unknown.png") );
    stringToTypeInfo["BSplineSurface"]   = types.size();
    typeToTypeInfo[DATA_BSPLINE_SURFACE] = types.size();
    types.push_back( TypeInfo(DATA_BSPLINE_SURFACE ,"BSplineSurface" ,"Unknown.png") );

    stringToTypeInfo["Skeleton"]  = types.size();
    typeToTypeInfo[DATA_SKELETON] = types.size();
    types.push_back( TypeInfo(DATA_SKELETON        ,"Skeleton"       ,"Unknown.png") );
    stringToTypeInfo["GIS"]  = types.size();
    typeToTypeInfo[DATA_GIS] = types.size();
    types.push_back( TypeInfo(DATA_GIS             ,"GIS"            ,"Unknown.png") );
    stringToTypeInfo["All"]  = types.size();
    typeToTypeInfo[DATA_ALL] = types.size();
    types.push_back( TypeInfo(DATA_ALL             ,"All"            ,"Unknown.png") );

    typeToString[DATA_NONE] = "Unknown";
    typeToString[DATA_GROUP] = "Group";
    typeToString[DATA_POINTS] = "Points";
    typeToString[DATA_POLY_LINE] = "PolyLine";
    typeToString[DATA_TRIANGLE_MESH] = "TriangleMesh";
    typeToString[DATA_POLY_MESH] = "PolyMesh";
    typeToString[DATA_BSPLINE_CURVE] = "BSplineCurve";
    typeToString[DATA_VOLUME] = "Volume";
    typeToString[DATA_BSPLINE_SURFACE] = "BSplineSurface";
    typeToString[DATA_SKELETON] = "Skeleton";
    typeToString[DATA_GIS] = "GIS";
    typeToString[DATA_ALL] = "All";

    initialized_ = true;
  }
}

/// Adds a datatype and returns the id for the new type
DataType addDataType(QString _name) {
  initialize();

  int type = nextTypeId_;

  stringToTypeInfo[ _name ] = types.size();
  typeToTypeInfo[ type ] = types.size();
  types.push_back( TypeInfo(type, _name, "Unknown.png") );

  typeToString[type] = _name;

  nextTypeId_ *= 2;
  return( type );
}

/// Get the id of a type with given name
DataType typeId(QString _name) {
  initialize();

  std::map<QString, unsigned int>::iterator index = stringToTypeInfo.find( _name );

  if ( index != stringToTypeInfo.end() )
    return types[ index->second ].type;
  else
    return -1;
}

/// Get the name of a type with given id
QString typeName(DataType _id) {
  initialize();

  std::map<unsigned int, QString>::iterator name = typeToString.find(_id);

  if ( name != typeToString.end() )
    return name->second;
  else
    return "Unknown";
}

/// Get the icon of a given dataType
QString typeIcon(QString  _name) {
  initialize();

  std::map<QString, unsigned int>::iterator index = stringToTypeInfo.find( _name );

  if ( index != stringToTypeInfo.end() )
    return types[ index->second ].iconName;
  else
    return "Unknown.png";
}

/// get the icon of a given dataType
QString typeIcon(DataType _id) {
  initialize();

  std::map<unsigned int, unsigned int>::iterator index = typeToTypeInfo.find(_id);

  if ( index != typeToTypeInfo.end() )
    return types[ index->second ].iconName;
  else
    return "Unknown.png";
}

/// Set the icon for a given dataType
void setTypeIcon( DataType _id   , QString _icon ) {
  initialize();

  std::map<unsigned int, unsigned int>::iterator index = typeToTypeInfo.find(_id);

  if ( index != typeToTypeInfo.end() )
    types[ index->second ].iconName = _icon;
  else
    std::cerr << "Could not set icon for DataType. Type not found!" << std::endl;
}

/// Set the icon for a given dataType
void setTypeIcon( QString  _name , QString _icon ) {
  initialize();

  std::map<QString, unsigned int>::iterator index = stringToTypeInfo.find( _name );

  if ( index != stringToTypeInfo.end() )
    types[ index->second ].iconName = _icon;
  else
    std::cerr << "Could not set icon for DataType. Type not found!" << std::endl;
}

//=============================================================================
//=============================================================================
