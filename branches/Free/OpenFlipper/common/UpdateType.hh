
#ifndef UPDATETYPE_HH
#define UPDATETYPE_HH

// TYPE

typedef uint UpdateType;


/// Identifier for all updates
const UpdateType UPDATE_ALL(UINT_MAX);

const UpdateType UPDATE_OBJECT_SELECTION(0);
const UpdateType UPDATE_VISIBILITY(1);
const UpdateType UPDATE_GEOMETRY(2);
const UpdateType UPDATE_TOPOLOGY(4);
const UpdateType UPDATE_SELECTION(8);

// FUNCTIONS

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
