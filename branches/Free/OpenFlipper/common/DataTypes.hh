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
 *   $Revision: 7697 $                                                         *
 *   $Author: moebius $                                                      *
 *   $Date: 2009-11-30 17:40:00 +0100 (Mo, 30. Nov 2009) $                   *
 *                                                                           *
\*===========================================================================*/




//=============================================================================
//
//  Types
//
//=============================================================================

/**
 * \file DataTypes.hh
 * This File contains the basic type informations
 */


#ifndef DATATYPES_HH
#define DATATYPES_HH

//== includes =================================================================


#include <OpenFlipper/common/GlobalDefines.hh>

#include <ACG/Math/Matrix4x4T.hh>
#include <ACG/Math/VectorT.hh>
#include <limits.h>
#include <QIcon>
#include <QMetaType>

//== Global Typedefs  =================================================================

/** \brief Predefined datatypes
*
* Here are several datatypes which have predefined ids. This might be changed to runtime added
* datatypes in the future.
*/

/// None of the other Objects
#define DATA_UNKNOWN 0

/// Items used for Grouping
#define DATA_GROUP 1

/// Triangle Mesh objects
#define DATA_TRIANGLE_MESH typeId("TriangleMesh")
#define DATA_TRIANGLE_MESH_CONST_ID 2

/// Poly Mesh Objects
#define DATA_POLY_MESH typeId("PolyMesh")
#define DATA_POLY_MESH_CONST_ID 4

/// Identifier for all available objects
const unsigned int DATA_ALL           = UINT_MAX;

typedef unsigned int DataType;


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

//================================================================================================
/** @name Functions for adding dataTypes
* @{ */
//================================================================================================

/** Adds a datatype and returns the id for the new type
*
* @param _name Internal name for the new DataType
* @param _readableName Human readable Name for this type ( Use tr to make it translatable )
*/
DLLEXPORT
DataType addDataType(QString _name, QString _readableName);

/// Given a dataType Identifier string this function will return the id of the datatype
DLLEXPORT
DataType typeId(QString _name);

/** \brief Get the name of a type with given id
*
* The ids are organized in a bitfield. So use either the macro for getting the type id or
* use the id directly (they have to be power of 2! ... Bitfield)
*/
DLLEXPORT
QString typeName(DataType _id);

/** \brief Get the number of registered types
*
* This function will return the number of types registered to the core. You can use it to
* iterate over all types. 
*
* \Note Remember that the types are organized in a bitfield!
*/
DLLEXPORT 
uint typeCount();

/** @} */

//================================================================================================
/** @name Datatype Name handling
* @{ */
//================================================================================================

/// Get DataType Human readable name ( this name might change. Use the typeName insted! )
DLLEXPORT
QString dataTypeName( DataType _id );

/// Get DataType Human readable name ( this name might change. Use the typeName insted! )
DLLEXPORT
QString dataTypeName( QString _typeName);

/// Set DataType Human readable name
DLLEXPORT
void setDataTypeName( DataType _id, QString _name );

/// Set DataType Human readable name
DLLEXPORT
void setDataTypeName( QString _typeName, QString _name );


/** @} */

//================================================================================================
/** @name Datatype Icons
* @{ */
//================================================================================================

/// Get a string with the filename of the icon for the DataType name
DLLEXPORT
QString typeIconName(QString  _name);

/// Get a string with the filename of the icon for the DataType
DLLEXPORT
QString typeIconName(DataType _id);

/** \brief Get an QIcon associated with the given datatype
* 
* The icons are loaded once when set and then the reference is returned here.
* This reduces the time when frequently requesting the icons (e.g. DataControl)
*/
DLLEXPORT
QIcon& typeIcon(DataType _id);

/// Set an Icon for a given DataType
DLLEXPORT
void setTypeIcon( DataType _id   , QString _icon);

/// Set an Icon for a given DataType
DLLEXPORT
void setTypeIcon( QString  _name , QString _icon );

/** @} */





//=============================================================================
#endif // DATATYPES_HH defined
//=============================================================================
