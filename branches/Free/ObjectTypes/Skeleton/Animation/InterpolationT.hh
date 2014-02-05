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

#ifndef SKELETON_INTERPOLATIONT_HH
#define SKELETON_INTERPOLATIONT_HH

#include <utility>
#include <vector>


template<typename Scalar>
class InterpolationT
{
  public:
    typedef std::vector<Scalar>         Target;
    typedef std::pair<Scalar, Target>   Point;
    
    InterpolationT<Scalar>(Point _P0, Point _P1) : P0_(_P0), P1_(_P1) {}
    
    virtual Target getValue(Scalar _atInput) = 0;
    virtual Scalar getMaxInput() = 0;
    virtual Scalar getMinInput() = 0;
    
  protected:
    Point P0_;
    Point P1_;
    
  private:
    
    
};

template<typename Scalar>
class BezierInterpolationT : public InterpolationT<Scalar>
{
  public:
    typedef std::vector<Scalar>         Target;
    typedef std::pair<Scalar, Target>   Point;
    
    BezierInterpolationT<Scalar>(Point _P0, Point _P1)
        : InterpolationT<Scalar>(_P0, _P1) {}
    BezierInterpolationT<Scalar>(Point _P0, Point _P1, Point _C0, Point _C1)
        : InterpolationT<Scalar>(_P0, _P1), C0_(_C0), C1_(_C1) {}
    
    Target getValue(Scalar atX);
    Scalar getMaxInput();
    Scalar getMinInput();
    
  protected:
    using InterpolationT<Scalar>::P0_;
    using InterpolationT<Scalar>::P1_;
    Point C0_;
    Point C1_;
    
    
  public:
    Point& P0();
    Point& C0();
    Point& C1();
    Point& P1();
};

//=============================================================================
#if defined(INCLUDE_TEMPLATES) && !defined(INTERPOLATIONT_CC)
#define INTERPOLATIONT_TEMPLATES
#include "BezierInterpolationT.cc"
#endif
//=============================================================================

#endif //SKELETON_INTERPOLATIONT_HH