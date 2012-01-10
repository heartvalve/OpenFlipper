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
//  Plugin Functions
//
//=============================================================================

#include <OpenFlipper/common/Types.hh>

#include "PluginFunctions.hh"

namespace PluginFunctions {


ObjectIterator::ObjectIterator( IteratorRestriction _restriction , DataType _dataType) {

  // Initialize with invalid pos
  pos_ = 0;

  // Store the restriction for the operator ( Source/Target )
  restriction_ = _restriction;

  // Store the requested DataType
  dataType_ = _dataType;

  // Start at the root Node
  BaseObject* currentPos = objectRoot();

  // Take the first element which is an baseObjectData
  proceedToNextBaseObjectData(currentPos);

  while ( (currentPos != objectRoot()) ) {

    // Return only target objects if requested
    if ( (restriction_ == TARGET_OBJECTS) && (! currentPos->target() ) ) {
      proceedToNextBaseObjectData(currentPos);
      continue;
    }

    // Return only source objects if requested
    if ( (restriction_ == SOURCE_OBJECTS) && (! currentPos->source() ) ) {
      proceedToNextBaseObjectData(currentPos);
      continue;
    }

    // Return only the right dataType
    if ( _dataType != DATA_ALL )
      if ( ! (currentPos->dataType( dataType_ ) ) ) {
        proceedToNextBaseObjectData(currentPos);
        continue;
      }

    // found a valid object
    pos_ = dynamic_cast<BaseObjectData* > (currentPos);
    break;
  }
}

ObjectIterator::ObjectIterator(BaseObjectData* pos, IteratorRestriction _restriction , DataType _data)
{
   restriction_ = _restriction;
   pos_         = pos;
   dataType_    = _data;
};


bool ObjectIterator::operator==( const ObjectIterator& _rhs) const {
   return ( _rhs.pos_ == pos_ );
}

bool ObjectIterator::operator!=( const ObjectIterator& _rhs) const {
   return ( _rhs.pos_ != pos_ );
}

ObjectIterator& ObjectIterator::operator=( const ObjectIterator& _rhs) {
   pos_         = _rhs.pos_;
   dataType_    = _rhs.dataType_;
   restriction_ = _rhs.restriction_;
   return *this;
}


ObjectIterator::pointer ObjectIterator::operator->(){
   return pos_;
}

ObjectIterator& ObjectIterator::operator++() {

  // Convert our pointer to the basic one
  BaseObject* currentPos = dynamic_cast< BaseObject* >(pos_);

  // Get the next objectData element in the tree
  proceedToNextBaseObjectData(currentPos);

  while ( (currentPos != objectRoot() ) ) {

    // Return only target objects if requested
    if ( (restriction_ == TARGET_OBJECTS) && (! currentPos->target() ) ) {
      proceedToNextBaseObjectData(currentPos);
      continue;
    }

    // Return only source objects if requested
    if ( (restriction_ == SOURCE_OBJECTS) && (! currentPos->source() ) ) {
      proceedToNextBaseObjectData(currentPos);
      continue;
    }

    // Return only the right dataType
    if ( ! (currentPos->dataType( dataType_ ) ) ) {
      proceedToNextBaseObjectData(currentPos);
      continue;
    }

    // found a valid object
    pos_ = dynamic_cast<BaseObjectData* > (currentPos);
    return *this;
  }

  // No valid object found
  pos_ = 0;
  return *this;
}

ObjectIterator& ObjectIterator::operator--() {
   std::cerr << "TODO :--" << std::endl;
   return *this;
}

/** This operator returns a pointer to the current object the iterator
 * points to.
 *
 * @return Pointer to the current ObjectData
 */
BaseObjectData* ObjectIterator::operator*() {
   return pos_;
}

/// Return Iterator to Object End
ObjectIterator objectsEnd() {
   return ObjectIterator(0);
}

void ObjectIterator::proceedToNextBaseObjectData(BaseObject*& _object) {

  _object = _object->next();

  // Go through the tree and stop at the root node or if we found a baseObjectData Object
  while ( (_object != objectRoot()) && !dynamic_cast<BaseObjectData* > (_object)  )
     _object = _object->next();
}



}
