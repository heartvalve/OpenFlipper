//=============================================================================
//
//                               OpenFlipper
//        Copyright (C) 2008 by Computer Graphics Group, RWTH Aachen
//                           www.openflipper.org
//
//-----------------------------------------------------------------------------
//
//                                License
//
//  OpenFlipper is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
// 
//  OpenFlipper is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
// 
//  You should have received a copy of the GNU Lesser General Public License
//  along with OpenFlipper.  If not, see <http://www.gnu.org/licenses/>.
//
//-----------------------------------------------------------------------------
//
//   $Revision$
//   $Author$
//   $Date$
//
//=============================================================================




//=============================================================================
//
//  CLASS NormalConeT - IMPLEMENTATION
//
//=============================================================================

#define ACG_NORMALCONE_C

//== INCLUDES =================================================================

#include <math.h>
#include "NormalConeT.hh"

#ifdef max
#  undef max
#endif

#ifdef min
#  undef min
#endif


//== NAMESPACES ===============================================================


namespace ACG {
namespace Geometry {


//== IMPLEMENTATION ========================================================== 

template <typename Scalar>
NormalConeT<Scalar>::
NormalConeT(const Vec3& _center_normal, Scalar _angle)
  : center_normal_(_center_normal), angle_(_angle)
{
}


//----------------------------------------------------------------------------


template <typename Scalar>
Scalar 
NormalConeT<Scalar>::
max_angle(const Vec3& _norm) const
{
  Scalar dotp = (center_normal_ | _norm);
  return (dotp >= 1.0 ? 0.0 : (dotp <= -1.0 ? M_PI : acos(dotp)))
    + angle_;
}


//----------------------------------------------------------------------------


template <typename Scalar>
Scalar 
NormalConeT<Scalar>::
max_angle(const NormalConeT& _cone) const
{
  Scalar dotp = (center_normal_ | _cone.center_normal_);
  Scalar centerAngle = dotp >= 1.0 ? 0.0 : (dotp <= -1.0 ? M_PI : acos(dotp));
  Scalar sideAngle0 = std::max(angle_-centerAngle, _cone.angle_);
  Scalar sideAngle1 = std::max(_cone.angle_-centerAngle, angle_);

  return centerAngle + sideAngle0 + sideAngle1;
}


//----------------------------------------------------------------------------


template <typename Scalar>
void 
NormalConeT<Scalar>::
merge(const NormalConeT& _cone)
{
  Scalar dotp = (center_normal_ | _cone.center_normal_);

  if (fabs(dotp) < 0.99999)
  {
    // new angle
    Scalar centerAngle = acos(dotp);
    Scalar minAngle    = std::min(-angle(), centerAngle - _cone.angle());
    Scalar maxAngle    = std::max( angle(), centerAngle + _cone.angle());
    angle_     = 0.5 * (maxAngle - minAngle);

    // axis by SLERP
    Scalar axisAngle = 0.5*(minAngle + maxAngle);
    center_normal_ = ((center_normal_ * sin(centerAngle-axisAngle)
		       + _cone.center_normal_ * sin(axisAngle))
		       / sin(centerAngle));
  }
  else
  {
    // axes point in same direction
    if (dotp > 0.0)
      angle_ = std::max(angle_, _cone.angle_);

    // axes point in opposite directions
    else
      angle_ = 2*M_PI;
  }
}


//=============================================================================
} // namespace Geometry
} // namespace ACG
//=============================================================================
