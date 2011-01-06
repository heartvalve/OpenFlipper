/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2010 by Computer Graphics Group, RWTH Aachen       *
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

#ifndef INTERPOLATIONMATRIXMANIPULATORT_HH
#define INTERPOLATIONMATRIXMANIPULATORT_HH

#include <ACG/Math/GLMatrixT.hh>
#include <vector>


/** \brief Knows how to apply the values generated by an interpolator to a matrix.
* When playing back an InterpolationAnimation, an InterpolationT interpolates between two points. This can be done linearly or via a bezier curve or whatever.
* The values coming out of this interpolation are just a vector of numbers that now have to be applied to a transformation in the pose.
* How this has to be done depends on the type of animation. For example, the interpolator might interpolate between two angles (i.e. output a single number).
* This would have to be applied to the matrix using a rotation function. The same is imaginable with a translation (3 values) and so on.
*
*/
template<typename Scalar>
class InterpolationMatrixManipulatorT
{
  public:    
    typedef ACG::GLMatrixT<Scalar> GLMatrixT;
    
    /**
    * This applies the given values to the given matrix in the fashion that is suitable.
    */
    virtual void doManipulation(GLMatrixT& _matrix, std::vector<Scalar> _value) = 0;
  protected:
    
  private:
};

#endif //INTERPOLATIONMATRIXMANIPULATORT_HH