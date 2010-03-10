
#ifndef UPDATETYPE_HH
#define UPDATETYPE_HH

#include <bitset>

typedef std::bitset<64> UpdateTypeSet;

// TYPE
class UpdateType {
  public:
    UpdateType(const UpdateType& _type);
    
    UpdateType(UpdateTypeSet _set);
    
    /// Exact compare operator
    bool operator==(const UpdateType& _type) const;
    
    UpdateType& operator++();
    
    bool operator<( const UpdateType& _i ) const;
    
    /// Check if this update contains the given UpdateType
    bool contains( const UpdateType& _type );
    
  protected:
    UpdateTypeSet type_;
};

/// Identifier for all updates
const UpdateType UPDATE_ALL(              UpdateTypeSet(1)       );

/// This is the update identifier for global Object selections(source/target)
const UpdateType UPDATE_OBJECT_SELECTION( UpdateTypeSet(1) << 1  );

/// This is the update identifier for global Object visibiölity ( show/hide )
const UpdateType UPDATE_VISIBILITY(       UpdateTypeSet(1) << 2  );


/// The following update types are predefined for the general use cases

/** \brief Geometry updated
*
* Updated Geometry ( This update type has to be used if you only modify vertex positions of 
* an object. Everything else has to stay as before the update.
*/
const UpdateType UPDATE_GEOMETRY( UpdateTypeSet(1) << 3 );

/** \brief Topology updated
*
* Updated Topology ( This update type has to be used if you modify the topology
* of an object. This includes adding vertices or removing them! )
*/
const UpdateType UPDATE_TOPOLOGY( UpdateTypeSet(1) << 4 );


/** \brief Selection updated
*
* Updated Selection ( This update type has to be used if you modify the internal
* selection of an object. Like selecting a single vertex or a set of faces. ). 
*/
const UpdateType UPDATE_SELECTION( UpdateTypeSet(1) << 5 );

/** \brief Vertex selection has changed
*
* This is a more fine grained selection update. UPDATE_SELECTION will also match this update type.
*/
const UpdateType UPDATE_SELECTION_VERTICES( UpdateTypeSet(1) << 6 );


/** \brief Edge selection has changed
*
* This is a more fine grained selection update. UPDATE_SELECTION will also match this update type.
*/
const UpdateType UPDATE_SELECTION_EDGES( UpdateTypeSet(1) << 7 );

/** \brief Face selection has changed
*
* This is a more fine grained selection update. UPDATE_SELECTION will also match this update type.
*/
const UpdateType UPDATE_SELECTION_FACES( UpdateTypeSet(1) << 8 );

/// marks the last used ID
const UpdateType UPDATE_UNUSED ( UpdateTypeSet(1) << 9 );

//=====================================================================================
// FUNCTIONS
//=====================================================================================

/** Adds a datatype and returns the id for the new type
*
* @param _name Internal name for the new DataType
* @param _readableName Human readable Name for this type ( Use tr to make it translatable )
*/
DLLEXPORT
UpdateType addUpdateType(QString _name);

/// Given a dataType Identifier string this function will return the id of the datatype
DLLEXPORT
UpdateType updateType(QString _name);

/** \brief Get the name of a type with given id
*
* The ids are organized in a bitfield. So use either the macro for getting the type id or
* use the id directly (they have to be power of 2! ... Bitfield)
*/
DLLEXPORT
QString updateTypeName(UpdateType _id);

/** \brief Get the number of registered types
*
* This function will return the number of types registered to the core. You can use it to
* iterate over all types. 
*/
DLLEXPORT 
uint updateTypeCount();


#endif // UPDATETYPE_HH
