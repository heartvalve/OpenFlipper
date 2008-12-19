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
//   $Revision: 3800 $
//   $Author: moebius $
//   $Date: 2008-11-26 10:10:45 +0100 (Mi, 26. Nov 2008) $
//
//=============================================================================




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
  BaseObject* currentPos = objectRoot()->next();

  // Go through the tree and stop at the root node or if we found a baseObjectData Object
  while ( (currentPos != objectRoot()) && !dynamic_cast<BaseObjectData* > (currentPos)  )
    currentPos = currentPos->next();

  while ( (currentPos != objectRoot()) ) {

    // Return only target objects if requested
    if ( (restriction_ == TARGET_OBJECTS) && (! currentPos->target() ) ) {

      // Go through the tree and stop at the root node or if we found a baseObjectData Object
      while ( (currentPos != objectRoot()) && !dynamic_cast<BaseObjectData* > (currentPos)  )
        currentPos = currentPos->next();

      continue;
    }

    // Return only source objects if requested
    if ( (restriction_ == SOURCE_OBJECTS) && (! currentPos->source() ) ) {

      // Go through the tree and stop at the root node or if we found a baseObjectData Object
      while ( (currentPos != objectRoot()) && !dynamic_cast<BaseObjectData* > (currentPos)  )
        currentPos = currentPos->next();

      continue;
    }

    // Return only the right dataType
    if ( _dataType != DATA_ALL )
      if ( ! (currentPos->dataType( dataType_ ) ) ) {

        // Go through the tree and stop at the root node or if we found a baseObjectData Object
        while ( (currentPos != objectRoot()) && !dynamic_cast<BaseObjectData* > (currentPos)  )
          currentPos = currentPos->next();

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


bool ObjectIterator::operator==( const ObjectIterator& _rhs) {
   std::cerr << "==" << std::endl;
   return ( _rhs.pos_ == pos_ );
}

bool ObjectIterator::operator!=( const ObjectIterator& _rhs) {
   return ( _rhs.pos_ != pos_ );
}

ObjectIterator& ObjectIterator::operator=( const ObjectIterator& _rhs) {
   pos_ = _rhs.pos_;
   return *this;
}


ObjectIterator::pointer ObjectIterator::operator->(){
   return pos_;
}

ObjectIterator& ObjectIterator::operator++() {

  // Convert our pointer to the basic one
  BaseObject* currentPos = dynamic_cast< BaseObject* >(pos_);

  // Get the next element in the tree
  currentPos = currentPos->next();

  // Go through the tree and stop at the root node or if we found a baseObjectData Object
  while ( (currentPos != objectRoot()) && !dynamic_cast<BaseObjectData* > (currentPos)  )
    currentPos = currentPos->next();

  while ( (currentPos != objectRoot() ) ) {

    // Return only target objects if requested
    if ( (restriction_ == TARGET_OBJECTS) && (! currentPos->target() ) ) {

      // Go through the tree and stop at the root node or if we found a baseObjectData Object
      while ( (currentPos != objectRoot()) && !dynamic_cast<BaseObjectData* > (currentPos)  )
        currentPos = currentPos->next();

      continue;
    }

    // Return only source objects if requested
    if ( (restriction_ == SOURCE_OBJECTS) && (! currentPos->source() ) ) {

      // Go through the tree and stop at the root node or if we found a baseObjectData Object
      while ( (currentPos != objectRoot()) && !dynamic_cast<BaseObjectData* > (currentPos)  )
        currentPos = currentPos->next();

      continue;
    }

    // Return only the right dataType
    if ( ! (currentPos->dataType( dataType_ ) ) ) {

      // Go through the tree and stop at the root node or if we found a baseObjectData Object
      while ( (currentPos != objectRoot()) && !dynamic_cast<BaseObjectData* > (currentPos)  )
        currentPos = currentPos->next();

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
ObjectIterator objects_end() {
   return ObjectIterator(0);
};



}
