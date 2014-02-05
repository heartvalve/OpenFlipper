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

#include "TextureParameters.hh"
#include <cfloat>

TexParameters::TexParameters() :
        abs(false),
        scale(true),
        clamp(false),
        clampMin(FLT_MIN),
        clampMax(FLT_MAX),
        repeat(false),
        repeatMax(1.0),
        repeatMin(0.0),
        center(false)
{
}


bool TexParameters::operator==(const TexParameters& _comp) const {
  return ( (scale     == _comp.scale)     &&
           (clampMin  == _comp.clampMin)  &&
           (clampMax  == _comp.clampMax)  &&
           (clamp     == _comp.clamp)     &&
           (repeat    == _comp.repeat)    &&
           (center    == _comp.center)    &&
           (abs       == _comp.abs)       &&
           (repeatMax == _comp.repeatMax) &&
           (repeatMin == _comp.repeatMin) );
}

bool TexParameters::operator!=(const TexParameters& _comp) const {
  return ( !( (*this) == _comp) );
}
