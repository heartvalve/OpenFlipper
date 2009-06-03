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
//  Types
//
//=============================================================================

/**
 * \file Types.hh
 * This File contains the basic available datatypes in the Framework.
 */


#ifndef TYPES_HH
#define TYPES_HH

#include <OpenFlipper/common/GlobalDefines.hh>
#include <limits.h>
#include <QIcon>


//== Global Typedefs  =================================================================

/** \brief Predefined datatypes
 *
 * Here are several datatypes which have predefined ids. This might be changed to runtime added
 * datatypes in the futere.
 */

/// None of the other Objects
#define DATA_NONE 0

/// Items used for Grouping
#define DATA_GROUP 1

/// Point based objects
#define DATA_POINTS 2

/// Triangle Mesh objects
#define DATA_TRIANGLE_MESH typeId("TriangleMesh")
//#define DATA_TRIANGLE_MESH 4
#define DATA_TRIANGLE_MESH_CONST_ID 4

/// Poly Mesh Objects
#define DATA_POLY_MESH typeId("PolyMesh")
//#define DATA_POLY_MESH 8
#define DATA_POLY_MESH_CONST_ID 8

/// PolyLine objects
const unsigned int DATA_POLY_LINE     = 16;

/// BSpline objects
const unsigned int DATA_BSPLINE_CURVE = 32;

/// Volumetric objects
const unsigned int DATA_VOLUME        = 64;

/// BSpline Surface objects
const unsigned int DATA_BSPLINE_SURFACE = 128;

/// Skeletons
const unsigned int DATA_SKELETON       = 256;

/// Gis Data
const unsigned int DATA_GIS            = 512;


/// Identifier for all available objects
const unsigned int DATA_ALL           = UINT_MAX;

typedef unsigned int DataType;

// TODO: Make types runtime addable

//== INCLUDES =================================================================

#include "BaseObject.hh"
#include "GroupObject.hh"
#include "BaseObjectData.hh"

//== TYPEDEFS =================================================================

/// Standard Type for 3d Vector used for scripting
typedef ACG::Vec3d Vector;
/// Standard Type for id Lists used for scripting
typedef std::vector< int > idList;
/// Standard Type for a 4x4 Matrix used for scripting
typedef ACG::Matrix4x4d Matrix4x4;

Q_DECLARE_METATYPE(idList);
Q_DECLARE_METATYPE(QVector< int >);
Q_DECLARE_METATYPE(Vector);
Q_DECLARE_METATYPE(Matrix4x4);

/// Vector Type containing the Data Objects
typedef std::vector<BaseObjectData*> DataContainer;

//== Functions for adding dataTypes =========================================================

/// Adds a datatype and returns the id for the new type
DLLEXPORT
DataType addDataType(QString _name);

/// Given a dataType Identifier string this function will return the id of the datatype
DLLEXPORT
DataType typeId(QString _name);

/// Get the name of a type with given id
DLLEXPORT
QString typeName(DataType _id);

//================================================================================================
/** @name Datatype Icons
* @{ */
//================================================================================================

/// Get an icon for a given DataType
DLLEXPORT
QString typeIcon(QString  _name);

/// Get an icon for a given DataType
DLLEXPORT
QString typeIcon(DataType _id);

/// Set an Icon for a given DataType
DLLEXPORT
void setTypeIcon( DataType _id   , QString _icon);

/// Set an Icon for a given DataType
DLLEXPORT
void setTypeIcon( QString  _name , QString _icon );

/** @} */


//=============================================================================
#endif // TYPES_HH defined
//=============================================================================
