/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2014 by Computer Graphics Group, RWTH Aachen       *
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


BaseObjectIterator::BaseObjectIterator( IteratorRestriction _restriction , DataType _dataType) :
    pos_(0),
    dataType_(_dataType),
    restriction_(_restriction)

{
  // Start at the root Node
  BaseObject* currentPos = objectRoot();

  currentPos = currentPos->next();

  while ( (currentPos != objectRoot()) ) {

    // Return only selected objects if requested
    if (!restriction_.isEmpty ()) {
      bool found = false;
      foreach (QString rest, restriction_)
        if (currentPos->flags().contains(rest))
        {
          found = true;
          break;
        }

      if (!found)
      {
        currentPos = currentPos->next();
        continue;
      }
    }

    // Return only the right dataType
    if ( _dataType != DATA_ALL )
      if ( ! (currentPos->dataType( dataType_ ) ) ) {
        currentPos = currentPos->next();
        continue;
      }

    // found a valid object
    pos_ = dynamic_cast<BaseObject* > (currentPos);
    break;
  }
}

BaseObjectIterator::BaseObjectIterator(BaseObject* pos, IteratorRestriction _restriction , DataType _data) :
    pos_(pos),
    dataType_(_data),
    restriction_(_restriction)
{
};


bool BaseObjectIterator::operator==( const BaseObjectIterator& _rhs) {
   return ( _rhs.pos_ == pos_ );
}

bool BaseObjectIterator::operator!=( const BaseObjectIterator& _rhs) {
   return ( _rhs.pos_ != pos_ );
}

BaseObjectIterator& BaseObjectIterator::operator=( const BaseObjectIterator& _rhs) {
   pos_         = _rhs.pos_;
   dataType_    = _rhs.dataType_;
   restriction_ = _rhs.restriction_;
   return *this;
}


BaseObjectIterator::pointer BaseObjectIterator::operator->(){
   return pos_;
}

BaseObjectIterator& BaseObjectIterator::operator++() {

  // Convert our pointer to the basic one
  BaseObject* currentPos = dynamic_cast< BaseObject* >(pos_);

  // Get the next objectData element in the tree
  currentPos = currentPos->next();

  while ( (currentPos != objectRoot() ) ) {

    // Return only selected objects if requested
    if (!restriction_.isEmpty ()) {
      bool found = false;
      foreach (QString rest, restriction_)
        if (currentPos->flags().contains(rest))
        {
          found = true;
          break;
        }

      if (!found)
      {
        currentPos = currentPos->next();
        continue;
      }
    }

    // Return only the right dataType
    if ( ! (currentPos->dataType( dataType_ ) ) ) {
      currentPos = currentPos->next();
      continue;
    }

    // found a valid object
    pos_ = dynamic_cast<BaseObject* > (currentPos);
    return *this;
  }

  // No valid object found
  pos_ = 0;
  return *this;
}

BaseObjectIterator& BaseObjectIterator::operator--() {
   std::cerr << "TODO :--" << std::endl;
   return *this;
}

/** This operator returns a pointer to the current object the iterator
 * points to.
 *
 * @return Pointer to the current ObjectData
 */
BaseObject* BaseObjectIterator::operator*() {
   return pos_;
}

/// Return Iterator to Object End
BaseObjectIterator baseObjectsEnd() {
   return BaseObjectIterator(0);
}


}
