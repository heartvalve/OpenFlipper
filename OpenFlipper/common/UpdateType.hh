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


#ifndef UPDATETYPE_HH
#define UPDATETYPE_HH

#include <OpenFlipper/common/GlobalDefines.hh>
#include <bitset>

typedef std::bitset<64> UpdateTypeSet;

/** \brief Update type class
 *
 *
 *  This class is used to specify the scope of updates. There are update types for
 *  selection,geometry,topology,colors,... specified here: \ref DefaultUpdateTypes
 *
 *  You can also find functions to add custom update types at runtime at
 *  \ref UpdateTypeFunctions
 *
 */
class DLLEXPORT UpdateType {
  public:
    UpdateType(const UpdateType& _type);
    
    UpdateType(UpdateTypeSet _set);
    
    /// Exact compare operator
    bool operator==(const UpdateType& _type) const;

    UpdateType operator|(const UpdateType& _type) const;

    UpdateType& operator|=(const UpdateType& _type);
    
    UpdateType& operator++();
    
    bool operator<( const UpdateType& _i ) const;
    
    /// Check if this update contains the given UpdateType
    bool contains( const UpdateType& _type ) const;
    
    friend std::ostream &operator<< (
            std::ostream &os, const UpdateType &type) {
        return os << type.type_;
    }

  protected:
    UpdateTypeSet type_;
};

/** \defgroup DefaultUpdateTypes Default update types
 * The following update types are predefined for the general use cases
 * @{
 *
 */

/// Identifier for all updates
const UpdateType UPDATE_ALL(              UpdateTypeSet(1)       );

/// This is the update identifier for global Object visibility ( show/hide )
const UpdateType UPDATE_VISIBILITY(       UpdateTypeSet(1) << 1  );


/** \brief Geometry updated
*
* Updated Geometry ( This update type has to be used if you only modify vertex positions of 
* an object. Everything else has to stay as before the update.
*/
const UpdateType UPDATE_GEOMETRY( UpdateTypeSet(1) << 2 );

/** \brief Topology updated
*
* Updated Topology ( This update type has to be used if you modify the topology
* of an object. This includes adding vertices or removing them! )
*/
const UpdateType UPDATE_TOPOLOGY( UpdateTypeSet(1) << 3 );


/** \brief Selection updated
*
* Updated Selection ( This update type has to be used if you modify the internal
* selection of an object. Like selecting a single vertex or a set of faces. ). 
*/
const UpdateType UPDATE_SELECTION( UpdateTypeSet(1) << 4 );

/** \brief Vertex selection has changed
*
* This is a more fine grained selection update. UPDATE_SELECTION will also match this update type.
*/
const UpdateType UPDATE_SELECTION_VERTICES( UpdateTypeSet(1) << 5 );


/** \brief Edge selection has changed
*
* This is a more fine grained selection update. UPDATE_SELECTION will also match this update type.
*/
const UpdateType UPDATE_SELECTION_EDGES( UpdateTypeSet(1) << 6 );


/** \brief Halfedge selection has changed
*
* This is a more fine grained selection update. UPDATE_SELECTION will also match this update type.
*/
const UpdateType UPDATE_SELECTION_HALFEDGES( UpdateTypeSet(1) << 7 );

/** \brief Face selection has changed
*
* This is a more fine grained selection update. UPDATE_SELECTION will also match this update type.
*/
const UpdateType UPDATE_SELECTION_FACES( UpdateTypeSet(1) << 8 );

/** \brief Knot selection has changed
*
* This is a more fine grained selection update. UPDATE_SELECTION will also match this update type.
*/
const UpdateType UPDATE_SELECTION_KNOTS( UpdateTypeSet(1) << 9 );

/** \brief Colors have changed
*
* Update the colors. This does not have to be called when topology is also updated
*/
const UpdateType UPDATE_COLOR( UpdateTypeSet(1) << 10 );

/** \brief Textures have changed
*
* Update the Textures.
*/
const UpdateType UPDATE_TEXTURE( UpdateTypeSet(1) << 11 );

/** \brief State has changed
*
* The object's state (target, source) has changed
*/
const UpdateType UPDATE_STATE( UpdateTypeSet(1) << 12 );

/// marks the last used ID
const UpdateType UPDATE_UNUSED ( UpdateTypeSet(1) << 13 );

/**@}*/

//=====================================================================================
// FUNCTIONS
//=====================================================================================


/** \defgroup UpdateTypeFunctions Runtime Update Types
 * Use these functions to add custom update types at runtime.
 * @{
 *
 */

/** Adds a UpdateType and returns the id for the new type
*
* @param _name Internal name for the new UpdateType
* @param _resetNeeded If a sceneGraph reset is needed for this update
*/
DLLEXPORT
UpdateType addUpdateType(QString _name, bool _resetNeeded = true);

/// Given an UpdateType Identifier string this function will return the id of the UpdateType
DLLEXPORT
UpdateType updateType(QString _name);

/** \brief Get the name of a type with given id
*
* The ids are organized in a bit field. So use either the macro for getting the type id or
* use the id directly (they have to be power of 2! ... Bit field)
*/
DLLEXPORT
QString updateTypeName(UpdateType _id);

/** \brief Get the number of registered types
*
* This function will return the number of types registered to the core. You can use it to
* iterate over all types. 
*/
DLLEXPORT 
size_t updateTypeCount();

/**@}*/

#endif // UPDATETYPE_HH
