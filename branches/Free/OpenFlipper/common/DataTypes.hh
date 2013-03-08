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

/** \brief the internal DataType class
*
* Normally we could use an unsigned int here. But QT can't register an
* typedef unsigned int DataType as DataType and will recognize it as unsigned int and
* therefore DataType will still be unknown to QtScript.
* To overcome this Problem, we reimplement a wrapper around the int and provide additional
* functionality such as returning the name of the type directly
*/ 
class DLLEXPORT DataType  {
  public:
    DataType();
    DataType(const unsigned int& _i);
    
    bool operator!=( const unsigned int& _i );
    bool operator!=( const DataType& _i );
    
    bool operator==( const unsigned int& _i );
    bool operator==(  const DataType& _i );
    
    DataType& operator=( const unsigned int& _i );
    DataType& operator=( const DataType& _i );
    
    bool operator<( const unsigned int& _i );
    bool operator<( const DataType& _i ) const;
    
    DataType& operator|=( const unsigned int& _i );
    DataType& operator|=( const DataType& _i );    
    
    bool operator&( const unsigned int& _i );
    bool operator&( const DataType& _i ) const;

    DataType operator!();
    
    bool contains( const DataType& _i );
    
    DataType operator|( const DataType& _i ) const;

    DataType operator++(int _unused);

    DataType& operator++();

    /** return the internal representation of the type which is an unsigned int at the moment.
    *
    * You should avoid using this directly as the internal representation might change in the future
    */
    unsigned int value() const;
      
    /// Return the name of this type as text
    QString name();
 
  private:
    unsigned int field;
};

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

/// Identifier for all available objects
const DataType DATA_ALL(UINT_MAX);

/// None of the other Objects
const DataType DATA_UNKNOWN(0);

/// Items used for Grouping
const DataType DATA_GROUP(1);

std::ostream &operator<<(std::ostream &stream, DataType type);


//== TYPEDEFS =================================================================

/// Standard Type for 3d Vector used for scripting
typedef ACG::Vec3d Vector;

/// Standard Type for 4d Vector used for scripting
typedef ACG::Vec4d Vector4;
/// Standard Type for id Lists used for scripting
typedef std::vector< int > IdList;
/// Standard Type for a 4x4 Matrix used for scripting
typedef ACG::Matrix4x4d Matrix4x4;

Q_DECLARE_METATYPE(IdList)
Q_DECLARE_METATYPE(DataType)
Q_DECLARE_METATYPE(QVector< int >)
Q_DECLARE_METATYPE(Vector)
Q_DECLARE_METATYPE(Vector4)
Q_DECLARE_METATYPE(Matrix4x4)

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

/** \brief Check if a type with the given name exists
*
*/
DLLEXPORT
bool typeExists( QString _name );

/** \brief Get the number of registered types
*
* This function will return the number of types registered to the core. You can use it to
* iterate over all types. 
*
* \note Remember that the types are organized in a bitfield!
*/
DLLEXPORT 
uint typeCount();

/** @} */

//================================================================================================
/** @name Type iterators
* @{ */
//================================================================================================

/// Get iterator pointing to the first element in the types list
DLLEXPORT
std::vector< TypeInfo >::const_iterator typesBegin();

/// Get iterator pointing to the last element in the types list
DLLEXPORT
std::vector< TypeInfo >::const_iterator typesEnd();

/** @} */

//================================================================================================
/** @name Datatype Name handling
* @{ */
//================================================================================================

/// Get DataType Human readable name ( this name might change. Use the typeName instead! )
DLLEXPORT
QString dataTypeName( DataType _id );

/// Get DataType Human readable name ( this name might change. Use the typeName instead! )
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

/** \brief Get an QIcon associated with the given DataType
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
