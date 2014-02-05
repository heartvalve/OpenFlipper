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

#ifndef SKELETON_ANIMATIONHELPER_HH
#define SKELETON_ANIMATIONHELPER_HH
#include <vector>

class AnimationHelper
{
  public:
        
    /** \brief Approximates the parameter value for a Bezier curve to get a certain x value
    * Does an iterative DeCasteljau search for the correct value.
    *
    *  ^
    *  |     C0----------------------------C1       
    *  |    /                               \                   
    *  |   /                                 \           
    *  |  /                                   \        
    *  | P0                                   P1
    *  |             
    *  +------------------atX------------------------------------>
    *
    * @param _atX The X value that the parameter value should be found for
    * @param _P0X The X value of the first key point
    * @param _P1X The X value of the second key point
    * @param _C0X The X value of the first control point
    * @param _C1X The X value of the second control point
    * @return The parameter value that has to be used to get the specified X value
    */
    template<typename Scalar>
    static float approximateCubicBezierParameter(Scalar _atX, Scalar _P0X, Scalar _P1X, Scalar _C0X, Scalar _C1X);
    
    /**
    * \brief Evaluates the cubic Bezier curve parameterized by P0, P1, C0 and C1 at the parameter value "at"
    */
    template<typename Scalar>
    static std::vector<Scalar> evaluateBezier(float at, std::vector<Scalar> _P0, std::vector<Scalar> _P1, std::vector<Scalar> _C0, std::vector<Scalar> _C1);
    
  private:
    template<typename Scalar>
    static Scalar clampTo01Interval(Scalar _value);
    
    template<typename Scalar>
    static Scalar abs(Scalar _value);
};

//=============================================================================
#if defined(INCLUDE_TEMPLATES) && !defined(ANIMATIONHELPER_CC)
#define ANIMATIONHELPER_TEMPLATES
#include "AnimationHelper.cc"
#endif
//=============================================================================

#endif //SKELETON_ANIMATIONHELPER_HH