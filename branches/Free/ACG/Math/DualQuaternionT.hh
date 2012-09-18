/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2011 by Computer Graphics Group, RWTH Aachen      *
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
 *   $Revision: 9595 $                                                       *
 *   $Author: wilden $                                                       *
 *   $Date: 2010-06-17 12:48:23 +0200 (Thu, 17 Jun 2010) $                   *
 *                                                                           *
\*===========================================================================*/



//=============================================================================
//
//  CLASS DualQuaternion
//
//=============================================================================

#ifndef ACG_DUALQUATERNION_HH
#define ACG_DUALQUATERNION_HH


//== INCLUDES =================================================================

#include "QuaternionT.hh"


//== NAMESPACES  ==============================================================

namespace ACG {


//== CLASS DEFINITION =========================================================


/**
    \brief DualQuaternion class for representing rigid motions in 3d

    This is an implementation of:

    techreport{kavan-06-dual,
    author = "Ladislav Kavan and Steven Collins and Carol O'Sullivan and Jiri Zara",
    series = "Technical report TCD-CS-2006-46, Trinity College Dublin",
    title = "{D}ual {Q}uaternions for {R}igid {T}ransformation {B}lending",
    url = "http://www.cgg.cvut.cz/~kavanl1/",
    year = "2006"
    }
*/

template <class Scalar>
class DualQuaternionT
{
public:

  typedef QuaternionT<Scalar>      Quaternion;
  typedef DualQuaternionT<Scalar>  DualQuaternion;
  typedef VectorT<Scalar,3>        Vec3;
  typedef VectorT<Scalar,4>        Vec4;
  typedef Matrix4x4T<Scalar>       Matrix;

  /// real and dual quaternion parts
  Quaternion real_;
  Quaternion dual_;
  
  
  // Constructors
  //
  
  /// Default constructor ( constructs an identity dual quaternion )
  DualQuaternionT();
  
  /// Copy constructor
  DualQuaternionT(const DualQuaternion& _other);
  
  /// Construct from given real,dual parts
  DualQuaternionT(const Quaternion& _real, const Quaternion& _dual);
  
  /// Construct from 8 scalars
  DualQuaternionT(Scalar _Rw, Scalar _Rx, Scalar _Ry, Scalar _Rz,
                  Scalar _Dw, Scalar _Dx, Scalar _Dy, Scalar _Dz);

  /// Construct from a rotation given as quaternion
  DualQuaternionT(Quaternion _rotation);
  
  /// Construct from a translatation given as a vector
  DualQuaternionT(const Vec3& _translation);
  
  /// Construct from a translation+rotation
  DualQuaternionT(const Vec3& _translation, const Quaternion& _rotation);
  
  /// Construct from a rigid transformation given as matrix
  DualQuaternionT(const Matrix& _transformation);
  
  // default quaternions
  
  /// identity dual quaternion [ R(1, 0, 0, 0), D(0,0,0,0) ]
  static DualQuaternion identity();

  /// zero dual quaternion [ R(0, 0, 0, 0), D(0,0,0,0) ]
  static DualQuaternion zero();
  
  // Operators
  //
  
  /// conjugate dual quaternion
  DualQuaternion conjugate() const;

  /// invert dual quaternion
  DualQuaternion invert() const;
  
  /// normalize dual quaternion
  void normalize();
  
  /// dual quaternion comparison
  bool operator==(const DualQuaternion& _other) const;
  bool operator!=(const DualQuaternion& _other) const;
  
  /// addition
  DualQuaternion operator+(const DualQuaternion& _other) const;
  DualQuaternion& operator+=(const DualQuaternion& _other);
  
  /// substraction 
  DualQuaternion operator-(const DualQuaternion& _other) const;
  DualQuaternion& operator-=(const DualQuaternion& _other);

  /// dualQuaternion * dualQuaternion
  DualQuaternion operator*(const DualQuaternion& _q) const;

  /// dualQuaternion * scalar
  DualQuaternion operator*(const Scalar& _scalar) const;
  
  /// dualQuaternion *= dualQuaternion
  DualQuaternion& operator*=(const DualQuaternion& _q);

  /// Access as one big vector
  Scalar& operator [](const unsigned int& b);

  /// linear interpolation of dual quaternions. Result is normalized afterwards
  template <typename VectorType>
  static DualQuaternion interpolate(VectorType& _weights, const std::vector<DualQuaternion>& _dualQuaternions);
  
  /// Transform a point with the dual quaternion
  Vec3 transform_point(const Vec3& _point) const;
  
  /// Transform a vector with the dual quaternion
  Vec3 transform_vector(const Vec3& _point) const;
  
  /// print some info about the DQ
  void printInfo();

};


typedef DualQuaternionT<float>  DualQuaternionf;
typedef DualQuaternionT<double> DualQuaterniond;


//=============================================================================
} // namespace ACG
//=============================================================================
#if defined(INCLUDE_TEMPLATES) && !defined(ACG_DUALQUATERNIONT_C)
#define ACG_QUATERNIONT_TEMPLATES
#include "DualQuaternionT.cc"
#endif
//=============================================================================
#endif // ACG_DUALQUATERNION_HH defined
//=============================================================================

