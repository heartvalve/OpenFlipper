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
 *   $Revision$                                                       *
 *   $Author$                                                      *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/



//=============================================================================
//
//  CLASS Quaternion
//
//=============================================================================

#ifndef ACG_QUATERNION_HH
#define ACG_QUATERNION_HH


//== INCLUDES =================================================================

#include "VectorT.hh"
#include "Matrix4x4T.hh"


//== NAMESPACES  ==============================================================

namespace ACG {


//== CLASS DEFINITION =========================================================


/**
    Quaternion class for representing rotations.
*/

template <class Scalar>
class QuaternionT : public VectorT<Scalar,4>
{
public:

#define W Base::values_[0]
#define X Base::values_[1]
#define Y Base::values_[2]
#define Z Base::values_[3]


  typedef VectorT<Scalar,4>    Base;
  typedef QuaternionT<Scalar>  Quaternion;
  typedef VectorT<Scalar,3>    Vec3;
  typedef VectorT<Scalar,4>    Vec4;
  typedef Matrix4x4T<Scalar>   Matrix;


  /// construct from 4 Scalars (default)
  QuaternionT(Scalar _w=1.0, Scalar _x=0.0, Scalar _y=0.0, Scalar _z=0.0)
    : Vec4(_w, _x, _y, _z) {}

  /// construct from 3D point (pure imaginary quaternion)
  QuaternionT(const Vec3& _p)
    : Vec4(0, _p[0], _p[1], _p[2]) {}

  /// construct from 4D vector
  QuaternionT(const Vec4& _p)
    : Vec4(_p[0], _p[1], _p[2], _p[3]) {}

  /// construct from rotation axis and angle (in radians)
  QuaternionT(Vec3 _axis, Scalar _angle)
  {
    _axis.normalize();
    Scalar theta = 0.5 * _angle;
    Scalar sin_theta = sin(theta);
    W = cos(theta);
    X = sin_theta * _axis[0];
    Y = sin_theta * _axis[1];
    Z = sin_theta * _axis[2];
  }

  /// construct from rotation matrix (only valid for rotation matrices!)
  template <class MatrixT>
  QuaternionT(const MatrixT& _rot)
  { init_from_matrix( _rot); }
  

  /// identity rotation
  void identity() { W=1.0; X=Y=Z=0.0; }


  /// conjugate quaternion
  Quaternion conjugate() const
  { return Quaternion(W, -X, -Y, -Z); }


  /// invert quaternion
  Quaternion invert() const
  { return conjugate()/Base::sqrnorm(); }


  /// quaternion * quaternion
  Quaternion operator*(const Quaternion& _q) const
  { return Quaternion(W*_q.W - X*_q.X - Y*_q.Y - Z*_q.Z,
		      W*_q.X + X*_q.W + Y*_q.Z - Z*_q.Y,
		      W*_q.Y - X*_q.Z + Y*_q.W + Z*_q.X,
		      W*_q.Z + X*_q.Y - Y*_q.X + Z*_q.W); }


  /// quaternion *= quaternion
  Quaternion& operator*=(const Quaternion& _q)
  { return *this = *this * _q; }


  /// rotate vector
  template <class Vec3T>
  Vec3T rotate(const Vec3T& _v) const
  { 
    Quaternion q = *this * Quaternion(0,_v[0],_v[1],_v[2]) * conjugate();
    return Vec3T(q[1], q[2], q[3]);
  }


  /// get rotation axis and angle (only valid for unit quaternions!)
  void axis_angle(Vec3& _axis, Scalar& _angle) const
  {
    if (fabs(W) > 0.999999)
    {
      _axis  = Vec3(1,0,0);
      _angle = 0;
    }
    else
    {
      _angle = 2.0 * acos(W);
      _axis  = Vec3(X, Y, Z).normalize();
    }
  }



  /// cast to rotation matrix
  Matrix rotation_matrix() const
  {
    Scalar
      ww(W*W), xx(X*X), yy(Y*Y), zz(Z*Z), wx(W*X),
      wy(W*Y), wz(W*Z), xy(X*Y), xz(X*Z), yz(Y*Z);

    Matrix m;

    m(0,0) = ww + xx - yy - zz;
    m(1,0) = 2.0*(xy + wz);
    m(2,0) = 2.0*(xz - wy);

    m(0,1) = 2.0*(xy - wz);
    m(1,1) = ww - xx + yy - zz;
    m(2,1) = 2.0*(yz + wx);

    m(0,2) = 2.0*(xz + wy);
    m(1,2) = 2.0*(yz - wx);
    m(2,2) = ww - xx - yy + zz;

    m(0,3) = m(1,3) = m(2,3) = m(3,0) = m(3,1) = m(3,2) = 0.0;
    m(3,3) = 1.0;

    return m;
  }



  /*
  /// get matrix for mult from right (Qr = q*r)
  Matrix right_mult_matrix() const
  {
    Matrix m;
    m(0,0) =  W; m(0,1) = -X; m(0,2) = -Y; m(0,3) = -Z;
    m(1,0) =  X; m(1,1) =  W; m(1,2) = -Z; m(1,3) =  Y;
    m(2,0) =  Y; m(2,1) =  Z; m(2,2) =  W; m(2,3) = -X;
    m(3,0) =  Z; m(3,1) = -Y; m(3,2) =  X; m(3,3) =  W;
    return m;
  }


  /// get matrix for mult from left (lQ = l*q)
  Matrix left_mult_matrix() const
  {
    Matrix m;
    m(0,0) =  W; m(0,1) = -X; m(0,2) = -Y; m(0,3) = -Z;
    m(1,0) =  X; m(1,1) =  W; m(1,2) =  Z; m(1,3) = -Y;
    m(2,0) =  Y; m(2,1) = -Z; m(2,2) =  W; m(2,3) =  X;
    m(3,0) =  Z; m(3,1) =  Y; m(3,2) = -X; m(3,3) =  W;
    return m;
  }
  */
  /// get matrix for mult from right (p*q = Qp)
  Matrix right_mult_matrix() const
  {
    Matrix m;
    m(0,0) =  W; m(0,1) = -X; m(0,2) = -Y; m(0,3) = -Z;
    m(1,0) =  X; m(1,1) =  W; m(1,2) =  Z; m(1,3) = -Y;
    m(2,0) =  Y; m(2,1) = -Z; m(2,2) =  W; m(2,3) =  X;
    m(3,0) =  Z; m(3,1) =  Y; m(3,2) = -X; m(3,3) =  W;
    return m;
  }


  /// get matrix for mult from left (q*p = Qp)
  Matrix left_mult_matrix() const
  {
    Matrix m;
    m(0,0) =  W; m(0,1) = -X; m(0,2) = -Y; m(0,3) = -Z;
    m(1,0) =  X; m(1,1) =  W; m(1,2) = -Z; m(1,3) =  Y;
    m(2,0) =  Y; m(2,1) =  Z; m(2,2) =  W; m(2,3) = -X;
    m(3,0) =  Z; m(3,1) = -Y; m(3,2) =  X; m(3,3) =  W;
    return m;
  }


  /// get quaternion from rotation matrix
  template<class MatrixT>
  void init_from_matrix( const MatrixT& _rot)
  {
    Scalar trace = _rot(0,0) + _rot(1,1) + _rot(2,2); 
    if( trace > 0.0 ) 
    {
      Scalar s = 0.5 / sqrt(trace + 1.0);
      W = 0.25 / s;
      X = ( _rot(2,1) - _rot(1,2) ) * s;
      Y = ( _rot(0,2) - _rot(2,0) ) * s;
      Z = ( _rot(1,0) - _rot(0,1) ) * s;
    } 
    else
    {
      if ( _rot(0,0) > _rot(1,1) && _rot(0,0) > _rot(2,2) )
      {
	Scalar s = 2.0 * sqrt( 1.0 + _rot(0,0) - _rot(1,1) - _rot(2,2));
	W = (_rot(2,1) - _rot(1,2) ) / s;
	X = 0.25 * s;
	Y = (_rot(0,1) + _rot(1,0) ) / s;
	Z = (_rot(0,2) + _rot(2,0) ) / s;
      } 
      else 
	if (_rot(1,1) > _rot(2,2))
	{
	  Scalar s = 2.0 * sqrt( 1.0 + _rot(1,1) - _rot(0,0) - _rot(2,2));
	  W = (_rot(0,2) - _rot(2,0) ) / s;
	  X = (_rot(0,1) + _rot(1,0) ) / s;
	  Y = 0.25 * s;
	  Z = (_rot(1,2) + _rot(2,1) ) / s;
	} 
	else
	{
	  Scalar s = 2.0 * sqrt( 1.0 + _rot(2,2) - _rot(0,0) - _rot(1,1) );
	  W = (_rot(1,0) - _rot(0,1) ) / s;
	  X = (_rot(0,2) + _rot(2,0) ) / s;
	  Y = (_rot(1,2) + _rot(2,1) ) / s;
	  Z = 0.25 * s;
	}
    }
  }


  /// quaternion exponential (for unit quaternions)
  Quaternion exponential() const
  {
    Vec3   n(X,Y,Z);
    Scalar theta( n.norm());
    Scalar sin_theta = sin(theta);
    Scalar cos_theta = cos(theta);

    if( theta > 1e-6 )
      n *= sin_theta/theta;
    else
      n = Vec3(0,0,0);

    return Quaternion( cos_theta, n[0], n[1], n[2]);
  }


  /// quaternion logarithm (for unit quaternions)
  Quaternion logarithm() const
  {
    // clamp to valid input
    double w = W;
    if( w > 1.0) w = 1.0;
    else if( w < -1.0) w = -1.0;

    Scalar theta_half = acos(w);

    Vec3   n(X,Y,Z);
    Scalar n_norm( n.norm());
    
    if( n_norm > 1e-6 )
      n *= theta_half/n_norm;
    else
      n = Vec3(0,0,0);

    return Quaternion( 0, n[0], n[1], n[2]);
  }

  void print_info()
  {
    // get axis, angle and matrix
    Vec3 axis; 
    Scalar angle;
    this->axis_angle( axis, angle);
    Matrix m;
    m = this->rotation_matrix();

    std::cerr << "quaternion : " << (*this)      << std::endl;
    std::cerr << "length     : " << this->norm() << std::endl;
    std::cerr << "axis, angle: " << axis << ", " << angle*180.0/M_PI << "\n";
    std::cerr << "rot matrix :\n";
    std::cerr << m << std::endl;
  }


#undef W
#undef X
#undef Y
#undef Z
};


typedef QuaternionT<float>  Quaternionf;
typedef QuaternionT<double> Quaterniond;


//=============================================================================
} // namespace ACG
//=============================================================================
#endif // ACG_QUATERNION_HH defined
//=============================================================================

