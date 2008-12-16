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
static std::map< unsigned int, QString > typesToString;

/** This map maps an dataType name to its id
 */
static std::map< QString , unsigned int > StringToType;

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
    types.push_back( TypeInfo(DATA_NONE            ,"Unknown"        ,"Unknown.png") );
    types.push_back( TypeInfo(DATA_GROUP           ,"Group"          ,"Unknown.png") );
    types.push_back( TypeInfo(DATA_POINTS          ,"Points"         ,"Unknown.png") );

    types.push_back( TypeInfo(DATA_POLY_LINE       ,"PolyLine"       ,"PolyLineType.png") );
    types.push_back( TypeInfo(DATA_TRIANGLE_MESH   ,"TriangleMesh"   ,"TriangleType.png") );
    types.push_back( TypeInfo(DATA_POLY_MESH       ,"PolyMesh"       ,"PolyType.png") );

    types.push_back( TypeInfo(DATA_BSPLINE_CURVE   ,"BSplineCurve"   ,"BSplineCurveType.png") );
    types.push_back( TypeInfo(DATA_VOLUME          ,"Volume"         ,"Unknown.png") );
    types.push_back( TypeInfo(DATA_BSPLINE_SURFACE ,"BSplineSurface" ,"Unknown.png") );

    types.push_back( TypeInfo(DATA_SKELETON        ,"Skeleton"       ,"Unknown.png") );
    types.push_back( TypeInfo(DATA_GIS             ,"GIS"            ,"Unknown.png") );
    types.push_back( TypeInfo(DATA_ALL             ,"All"            ,"Unknown.png") );

    std::cerr << "Todo : Initialize DataType system" << std::endl;
    initialized_ = true;
  }
}

// Adds a datatype and returns the id for the new type
DataType addDataType(QString _name) {
  initialize();
  std::cerr << "addDataType in Types.cc not implemented yet" << std::endl;
  nextTypeId_ *= 2;
  return( nextTypeId_ /2 );
}

DataType typeId(QString _name) {
  initialize();
  std::cerr << "typeId in Types.cc not implemented yet" << std::endl;
  return (DATA_NONE);
}

QString typeIcon(QString  _name) {
  initialize();
  return QString("");
}

QString typeIcon(DataType _id) {
  initialize();
  return QString("");
}

void setTypeIcon( DataType _id   , QString _icon ) {
  initialize();

}

void setTypeIcon( QString  _name , QString _icon ) {
  initialize();

}

//=============================================================================
//=============================================================================
