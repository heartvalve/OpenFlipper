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

#include "TextureMath.hh"

#include <cmath>

TextureMath::TextureMath(const bool   _abs,
                         const bool   _clamp,
                         const double _clampMin,
                         const double _clampMax,
                         const bool   _repeat,
                         const double _minRepeat,
                         const double _maxRepeat,
                         const bool   _center,
                         const bool   _scale,
                         const double _minimalInput,
                         const double _maximalInput):
        abs_(_abs),
        clamp_(_clamp),
        clampMin_(_clampMin),
        clampMax_(_clampMax),
        repeat_(_repeat),
        repeatMin_(_minRepeat),
        repeatMax_(_maxRepeat),
        center_(_center),
        scale_(_scale),
        minInput_(_minimalInput),
        maxInput_(_maximalInput)
{

}

TextureMath::TextureMath(const TexParameters& _parameters, const double _minimalInput, const double _maximalInput) :
        abs_(_parameters.abs),
        clamp_(_parameters.clamp),
        clampMin_(_parameters.clampMin),
        clampMax_(_parameters.clampMax),
        repeat_(_parameters.repeat),
        repeatMin_(_parameters.repeatMin),
        repeatMax_(_parameters.repeatMax),
        center_(_parameters.center),
        scale_(_parameters.scale),
        minInput_(_minimalInput),
        maxInput_(_maximalInput)
{
}

double TextureMath::transform(const double _input) const
{
  double value = _input;


  // Use absolute value as requested by plugin
  if ( abs_ )
    value = fabs(value);

  // Clamp if requested
  if ( clamp_ ) {
    if ( value > clampMax_ )
      value = clampMax_;
    if (value < clampMin_)
      value = clampMin_;
  }

  // if all texCoords have the same value
  if ( minInput_ == maxInput_){

    if ( ! repeat_ )
      value = 0.0;
    else
      value = maxInput_;

    return value;
  }


  // if the texture should not be repeated, scale to 0..1
  if ( ! repeat_ ) {
    if (! center_ ) {
      if ( scale_) {
        value /=  fabs(maxInput_) + fabs(minInput_);           //scaleFactor is != 0.0 (otherwise _min==_max)
        value -= minInput_/(fabs(maxInput_) + fabs(minInput_));
      }
    } else {
      // the values above zero are mapped to 0.5..1 the negative ones to 0.5..0
      if (value > 0.0) {
        value /= ( maxInput_ * 2.0); //_max >= _value > 0.0
        value += 0.5;
      } else {
        if ( minInput_ == 0.0 ){
          value = 0.0;
        } else {
          value /= ( minInput_ * 2.0);
          value = 0.5 - value;
        }
      }
    }
  } else {
    value -= minInput_;
    value *= (repeatMax_ - repeatMin_) / (maxInput_ - minInput_);
    value += repeatMin_;
  }

  return value;
}
