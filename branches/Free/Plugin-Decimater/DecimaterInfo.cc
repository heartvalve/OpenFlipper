/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2009 by Computer Graphics Group, RWTH Aachen      *
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
 *   $Revision$                                                         *
 *   $Author$                                                      *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/

#include "DecimaterInfo.hh"

DecimaterInfo::DecimaterInfo(TriMesh* _mesh) :
  decimater_(0),
  normalDeviation_(false),
  distance_(false),
  roundness_(false)
{
  decimater_ = new DecimaterType( *_mesh );

  decimater_->add( hModQuadric_priority );
  decimater_->module( hModQuadric_priority ).unset_max_err();

}


DecimaterInfo::~DecimaterInfo() {
  if ( decimater_ )
    delete decimater_;
}

//-----------------------------------------------------------------------------------


void DecimaterInfo::removeConstraints(){

  decimater_->module( hModQuadric_priority ).unset_max_err();

  //remove modules

  if ( normalDeviation_ ) {
    decimater_->remove( hModNormalFlipping );
    normalDeviation_ = false;
  }

  if ( distance_ ) {
    decimater_->remove( hModQuadric );
    distance_ = false;
  }

  if ( roundness_ ) {
    decimater_->remove( hModRoundness );
    roundness_ = false;
  }

}


//-----------------------------------------------------------------------------------

void DecimaterInfo::setDistanceConstraint( double _value ){

  if (  decimater_->add( hModQuadric ) ) {
    decimater_->module( hModQuadric ).set_max_err( _value );
    distance_ = true;
  }
}


//-----------------------------------------------------------------------------------

void DecimaterInfo::setNormalDeviationConstraint( int _value ){

  if ( decimater_->add( hModNormalFlipping ) ) {
    decimater_->module( hModNormalFlipping ).set_normal_deviation( _value );
    normalDeviation_ = true;
  }
}


//-----------------------------------------------------------------------------------

void DecimaterInfo::setRoundnessConstraint( double _value ){

  if ( decimater_->add( hModRoundness ) ) {
    decimater_->module( hModRoundness ).set_min_roundness( _value , true );
    roundness_ = true;
  }
}


