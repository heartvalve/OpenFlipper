//=============================================================================
//
//  Types
//
//=============================================================================

/**
 * \file PolyLineObjectSerializer.hh
 * This File contains serialization routines for poly line objects
 */

#ifndef POLYLINEOBJECTSERIALIZER_HH_
#define POLYLINEOBJECTSERIALIZER_HH_

#include "PolyLineObject.hh"

//== CLASS DEFINITION =========================================================

/** This class provides the functionality for serializing PolyLineObjects including all known object data
 */
class DLLEXPORT PolyLineObjectSerializer
{
private:
    PolyLineObject* instance;
public:
    PolyLineObjectSerializer(PolyLineObject* _object);
    void serialize(std::ostream& _stream);
    void deserialize(std::istream& _stream);
};


#endif /* POLYLINEOBJECTSERIALIZER_HH_ */
