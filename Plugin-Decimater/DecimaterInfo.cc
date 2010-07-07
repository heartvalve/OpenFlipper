/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2010 by Computer Graphics Group, RWTH Aachen      *
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
 *   $Revision$                                                       *
 *   $Author$                                                      *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/

#include "DecimaterInfo.hh"

DecimaterInfo::DecimaterInfo() :
  decimater_(),
  normalDeviation_(false),
  distance_(false),
  roundness_(false) { }


DecimaterInfo::~DecimaterInfo() {
}

PerObjectData* DecimaterInfo::copyPerObjectData() {
    
    DecimaterInfo* di_copy = new DecimaterInfo(*this);
    
    return di_copy;
}

//-----------------------------------------------------------------------------------

void DecimaterInfo::initialize( ModQuadricH& _handle, TriMesh* _mesh ) {
    
    decimater_ = std::tr1::shared_ptr< DecimaterType >( new DecimaterType( *_mesh )  );

    decimater()->add( _handle );
    decimater()->module( _handle ).unset_max_err();
}

//-----------------------------------------------------------------------------------

void DecimaterInfo::setDistanceConstraint( ModQuadricH& _handle, double _value ){

  if (  decimater()->add( _handle ) ) {
    decimater()->module( _handle ).set_max_err( _value );
    distance_ = true;
  }
}


//-----------------------------------------------------------------------------------

void DecimaterInfo::setNormalDeviationConstraint( ModNormalFlippingH& _handle, int _value ){

  if ( decimater()->add( _handle ) ) {
    decimater()->module( _handle ).set_normal_deviation( _value );
    normalDeviation_ = true;
    normalDeviation_value_ = _value;
  }
}


//-----------------------------------------------------------------------------------

void DecimaterInfo::setRoundnessConstraint( ModRoundnessH& _handle, double _value ){

  if ( decimater()->add( _handle ) ) {
    decimater()->module( _handle ).set_min_roundness( _value , true );
    roundness_ = true;
    roundness_value_ = _value;
  }
}

//-----------------------------------------------------------------------------------

void DecimaterInfo::removeDistanceConstraint( ModQuadricH& _handle ){

  if (  distance_ ) {
    decimater()->remove( _handle );
    distance_ = false;
  }
}


//-----------------------------------------------------------------------------------

void DecimaterInfo::removeNormalDeviationConstraint( ModNormalFlippingH& _handle ){

  if ( normalDeviation_ ) {
    decimater()->remove( _handle );
    normalDeviation_ = false;
  }
}


//-----------------------------------------------------------------------------------

void DecimaterInfo::removeRoundnessConstraint( ModRoundnessH& _handle ){

  if ( roundness_ ) {
    decimater()->remove( _handle );
    roundness_ = false;
  }
}


