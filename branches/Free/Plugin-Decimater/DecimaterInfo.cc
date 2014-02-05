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

#include "DecimaterInfo.hh"

DecimaterInfo::DecimaterInfo() :
  normalDeviation_(false),
  normalFlipping_(false),
  distance_(false),
  edgeLength_(false),
  aspectRatio_(false),
  roundness_(false),
  independentSets_(false),
  decimationOrder_(DISTANCE),
  normalDeviation_value_(0),
  distance_value_(0),
  edgeLength_value_(0),
  aspectRatio_value_(0),
  roundness_value_(0) {}


DecimaterInfo::~DecimaterInfo() {
}

PerObjectData* DecimaterInfo::copyPerObjectData() {
    
    DecimaterInfo* di_copy = new DecimaterInfo(*this);
    
    return di_copy;
}

//-----------------------------------------------------------------------------------

void DecimaterInfo::setDistanceConstraint( double _value ){

  distance_ = true;
  distance_value_ = _value;
}


//-----------------------------------------------------------------------------------

void DecimaterInfo::setNormalDeviationConstraint( int _value ){

  normalDeviation_ = true;
  normalDeviation_value_ = _value;
}


//-----------------------------------------------------------------------------------

void DecimaterInfo::setNormalFlippingConstraint(){

  normalFlipping_ = true;
}


//-----------------------------------------------------------------------------------

void DecimaterInfo::setIndependentSetsConstraint(){

  independentSets_ = true;
}


//-----------------------------------------------------------------------------------

void DecimaterInfo::setRoundnessConstraint( double _value ){

  roundness_ = true;
  roundness_value_ = _value;
}

//-----------------------------------------------------------------------------------

void DecimaterInfo::setAspectRatioConstraint( double _value ){

  aspectRatio_ = true;
  aspectRatio_value_ = _value;
}

//-----------------------------------------------------------------------------------

void DecimaterInfo::setEdgeLengthConstraint( double _value ){

  edgeLength_ = true;
  edgeLength_value_ = _value;
}

//-----------------------------------------------------------------------------------

void DecimaterInfo::setDecimationOrder( DecimationOrder _order ){

  decimationOrder_ = _order;
}

//-----------------------------------------------------------------------------------

void DecimaterInfo::removeDistanceConstraint(){

  if (  distance_ ) {
    distance_ = false;
    distance_value_ = 0;
  }
}


//-----------------------------------------------------------------------------------

void DecimaterInfo::removeEdgeLengthConstraint(){

  if (  edgeLength_ ) {
    edgeLength_ = false;
    edgeLength_value_ = 0;
  }
}


//-----------------------------------------------------------------------------------

void DecimaterInfo::removeNormalDeviationConstraint(){

  if ( normalDeviation_ ) {
    normalDeviation_ = false;
    normalDeviation_value_ = 0;
  }
}


//-----------------------------------------------------------------------------------

void DecimaterInfo::removeNormalFlippingConstraint(){

  if ( normalFlipping_ ) {
    normalFlipping_ = false;
  }
}


//-----------------------------------------------------------------------------------

void DecimaterInfo::removeIndependentSetsConstraint(){

  if ( independentSets_ ) {
    independentSets_ = false;
  }
}


//-----------------------------------------------------------------------------------

void DecimaterInfo::removeAspectRatioConstraint(){

  if ( aspectRatio_ ) {
    aspectRatio_ = false;
    aspectRatio_value_ = 0;
  }
}


//-----------------------------------------------------------------------------------

void DecimaterInfo::removeRoundnessConstraint(){

  if ( roundness_ ) {
    roundness_ = false;
    roundness_value_ = 0;
  }
}


