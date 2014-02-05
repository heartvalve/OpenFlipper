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

#define ANIMATIONHELPER_CC

#include "AnimationHelper.hh"
#include <cmath>
#include <cassert>

#define APPROXIMATION_EPSILON 1.0e-09
#define VERYSMALL 1.0e-20
#define MAXIMUM_ITERATIONS 1000

template<typename Scalar>
Scalar AnimationHelper::clampTo01Interval(Scalar _value) {
  if (_value < 0.0)
    return 0.0;
  else if (_value > 1.0)
    return 1.0;
  else
    return _value;
}

template<typename Scalar>
Scalar AnimationHelper::abs(Scalar _value) {
  if (_value < 0.0)
    return _value * -1.0;
  else
    return _value;
}

template<typename Scalar>
float AnimationHelper::approximateCubicBezierParameter(Scalar _atX, Scalar _P0X, Scalar _P1X, Scalar _C0X, Scalar _C1X) {
   if (_atX - _P0X < VERYSMALL)
      return 0.0;
   
   if (_P1X - _atX < VERYSMALL) 
      return 1.0;
   
   long iterationStep = 0;
   
   float u = 0.0f; float v = 1.0f;
   
   //iteratively apply subdivision to approach value atX
   while (iterationStep < MAXIMUM_ITERATIONS) {
      
      // de Casteljau Subdivision.
      Scalar a = (_P0X + _C0X)*0.5;
      Scalar b = (_C0X + _C1X)*0.5;
      Scalar c = (_C1X + _P1X)*0.5;
      Scalar d = (a + b)*0.5;
      Scalar e = (b + c)*0.5;
      Scalar f = (d + e)*0.5; //this one is on the curve!
      
      //The curve point is close enough to our wanted atX
      if (abs<Scalar>(f - _atX) < APPROXIMATION_EPSILON)
         return clampTo01Interval<Scalar>((u + v)*0.5f);
      
      //dichotomy
      if (f < _atX) {
         _P0X = f;
         _C0X = e;
         _C1X = c;
         u = (u + v)*0.5f;
      } else {
         _C0X = a; _C1X = d; _P1X = f; v = (u + v)*0.5f;
      }
      
      iterationStep++;
   }
   
   return clampTo01Interval<Scalar>((u + v)*0.5f); 
}

template<typename Scalar>
std::vector<Scalar> AnimationHelper::evaluateBezier(float at, std::vector<Scalar> _P0, std::vector<Scalar> _P1, std::vector<Scalar> _C0, std::vector<Scalar> _C1) {
  unsigned int size = _P0.size();
  assert(size == _P1.size() && size == _C0.size() && size == _C1.size());
  
  float s = at;
  float sinv = (1-s);

  std::vector<Scalar> result;
  
  for (unsigned int i=0;i<size;++i) {
    result.push_back( _P0[i]*sinv*sinv*sinv + 3*_C0[i]*s*sinv*sinv + 3*_C1[i]*s*s*sinv + _P1[i]*s*s*s );
  }
  
  return result;
}
