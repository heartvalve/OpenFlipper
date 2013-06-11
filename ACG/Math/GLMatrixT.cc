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
//  CLASS GLMatrixT - IMPLEMENTATION
//
//=============================================================================


#define ACG_GLMATRIX_C


//== INCLUDES =================================================================


#include "GLMatrixT.hh"
#include <cmath>


//== IMPLEMENTATION ========================================================== 


namespace ACG {


//-----------------------------------------------------------------------------


template<typename Scalar> 
void
GLMatrixT<Scalar>::
scale( Scalar _x, Scalar _y, Scalar _z, 
       MultiplyFrom _mult_from )
{
  GLMatrixT<Scalar> m;
  m.identity();

  m(0,0) = _x;
  m(1,1) = _y;
  m(2,2) = _z;
  
  if (_mult_from == MULT_FROM_RIGHT)  *this *= m;
  else                                leftMult(m);
}


//-----------------------------------------------------------------------------


template<typename Scalar> 
void
GLMatrixT<Scalar>::
translate( Scalar _x, Scalar _y, Scalar _z, 
	   MultiplyFrom _mult_from )
{
  GLMatrixT<Scalar> m;
  m.identity();

  m(0,3) = _x;
  m(1,3) = _y;
  m(2,3) = _z;

  if (_mult_from == MULT_FROM_RIGHT)  *this *= m;
  else                                leftMult(m);
}


//-----------------------------------------------------------------------------


template<typename Scalar> 
void
GLMatrixT<Scalar>::
rotateXYZ( Axis _axis, Scalar _angle, 
	   MultiplyFrom _mult_from )
{
  GLMatrixT<Scalar> m;
  m.identity();

  Scalar ca = cos(_angle * (M_PI/180.0));
  Scalar sa = sin(_angle * (M_PI/180.0));
    
  switch (_axis) 
  {
    case X:
      m(1,1) = m(2,2) = ca;  m(2,1) = sa;  m(1,2) = -sa;
      break;

    case Y:
      m(0,0) = m(2,2) = ca;  m(0,2) = sa;  m(2,0) = -sa;
      break;

    case Z:
      m(0,0) = m(1,1) = ca;  m(1,0) = sa;  m(0,1) = -sa;
      break;
  }


  if (_mult_from == MULT_FROM_RIGHT)  *this *= m;
  else                                leftMult(m);
}


//-----------------------------------------------------------------------------


/* Rotation matrix (taken from Mesa3.1)
   original function contributed by Erich Boleyn (erich@uruk.org) */
template <typename Scalar> 
void
GLMatrixT<Scalar>::
rotate( Scalar angle, Scalar x, Scalar y, Scalar z,
	MultiplyFrom _mult_from )
{
  GLMatrixT<Scalar> m;
  Scalar  mag, s, c;
  Scalar  xx, yy, zz, xy, yz, zx, xs, ys, zs, one_c;

  mag = sqrt( x*x + y*y + z*z );
  if(mag == 0.) {
    return;
  }
   
  s = sin( angle * ( M_PI /180.) );
  c = cos( angle * ( M_PI /180.) );

  x /= mag;
  y /= mag;
  z /= mag;
  
  xx = x * x;
  yy = y * y;
  zz = z * z;
  xy = x * y;
  yz = y * z;
  zx = z * x;
  xs = x * s;
  ys = y * s;
  zs = z * s;
  one_c = 1.0F - c;

  m(0,0) = (one_c * xx) + c;
  m(0,1) = (one_c * xy) - zs;
  m(0,2) = (one_c * zx) + ys;
  m(0,3) = 0.0F;

  m(1,0) = (one_c * xy) + zs;
  m(1,1) = (one_c * yy) + c;
  m(1,2) = (one_c * yz) - xs;
  m(1,3) = 0.0F;
  
  m(2,0) = (one_c * zx) - ys;
  m(2,1) = (one_c * yz) + xs;
  m(2,2) = (one_c * zz) + c;
  m(2,3) = 0.0F;
  
  m(3,0) = 0.0F;
  m(3,1) = 0.0F;
  m(3,2) = 0.0F;
  m(3,3) = 1.0F;

  if (_mult_from == MULT_FROM_RIGHT)  *this *= m;
  else                                leftMult(m);
}


//-----------------------------------------------------------------------------


template<typename Scalar> 
void
GLMatrixT<Scalar>::
lookAt(const VectorT<Scalar,3>& eye,
       const VectorT<Scalar,3>& center,
       const VectorT<Scalar,3>& up)
{
  VectorT<Scalar,3> z(eye);
  z -= center;
  z.normalize();

  VectorT<Scalar,3> x(up % z);
  x.normalize();

  VectorT<Scalar,3> y(z % x);
  y.normalize();

  GLMatrixT<Scalar> m;
  m(0,0)=x[0]; m(0,1)=x[1]; m(0,2)=x[2]; m(0,3)=0.0;
  m(1,0)=y[0]; m(1,1)=y[1]; m(1,2)=y[2]; m(1,3)=0.0;
  m(2,0)=z[0]; m(2,1)=z[1]; m(2,2)=z[2]; m(2,3)=0.0;
  m(3,0)=0.0;  m(3,1)=0.0;  m(3,2)=0.0;  m(3,3)=1.0;

  *this *= m;
  translate(-eye[0], -eye[1], -eye[2]);
}


//-----------------------------------------------------------------------------


template<typename Scalar> 
void
GLMatrixT<Scalar>::
inverse_lookAt(const VectorT<Scalar,3>& eye,
	       const VectorT<Scalar,3>& center,
	       const VectorT<Scalar,3>& up)
{
  VectorT<Scalar,3> z(eye);
  z -= center;
  z.normalize();

  VectorT<Scalar,3> x(up % z);
  x.normalize();

  VectorT<Scalar,3> y(z % x);
  y.normalize();

  GLMatrixT<Scalar> m;
  m(0,0)=x[0]; m(0,1)=y[0]; m(0,2)=z[0]; m(0,3)=eye[0];
  m(1,0)=x[1]; m(1,1)=y[1]; m(1,2)=z[1]; m(1,3)=eye[1];
  m(2,0)=x[2]; m(2,1)=y[2]; m(2,2)=z[2]; m(2,3)=eye[2];
  m(3,0)=0.0;  m(3,1)=0.0;  m(3,2)=0.0;  m(3,3)=1.0;

  leftMult(m);
}


//-----------------------------------------------------------------------------


template<typename Scalar> 
void
GLMatrixT<Scalar>::
perspective(Scalar fovY, Scalar aspect, 
	    Scalar near_plane, Scalar far_plane)
{
  Scalar ymax = near_plane * tan( fovY * M_PI / 360.0 );
  Scalar ymin = -ymax;
  Scalar xmin = ymin * aspect;
  Scalar xmax = ymax * aspect;
  frustum(xmin, xmax, ymin, ymax, near_plane, far_plane);
}


//-----------------------------------------------------------------------------


template<typename Scalar> 
void
GLMatrixT<Scalar>::
inverse_perspective(Scalar fovY, Scalar aspect, 
		    Scalar near_plane, Scalar far_plane)
{
  Scalar ymax = near_plane * tan( fovY * M_PI / 360.0 );
  Scalar ymin = -ymax;
  Scalar xmin = ymin * aspect;
  Scalar xmax = ymax * aspect;
  inverse_frustum(xmin, xmax, ymin, ymax, near_plane, far_plane);
}


//-----------------------------------------------------------------------------


template<typename Scalar> 
void
GLMatrixT<Scalar>::
frustum( Scalar left,Scalar right, 
	 Scalar bottom, Scalar top, 
	 Scalar near_plane, Scalar far_plane )
{
  assert(near_plane > 0.0 && far_plane > near_plane);

  GLMatrixT<Scalar> m;
  Scalar x, y, a, b, c, d;

  x = (2.0*near_plane) / (right-left);
  y = (2.0*near_plane) / (top-bottom);
  a = (right+left) / (right-left);
  b = (top+bottom) / (top-bottom);
  c = -(far_plane+near_plane) / ( far_plane-near_plane);
  d = -(2.0*far_plane*near_plane) / (far_plane-near_plane);

  m(0,0) = x;    m(0,1) = 0.0F; m(0,2) = a;     m(0,3) = 0.0F;
  m(1,0) = 0.0F; m(1,1) = y;    m(1,2) = b;     m(1,3) = 0.0F;
  m(2,0) = 0.0F; m(2,1) = 0.0F; m(2,2) = c;     m(2,3) = d;
  m(3,0) = 0.0F; m(3,1) = 0.0F; m(3,2) = -1.0F; m(3,3) = 0.0F;

  *this *= m;
}


//-----------------------------------------------------------------------------


template<typename Scalar> 
void
GLMatrixT<Scalar>::
inverse_frustum(Scalar left,Scalar right,
		Scalar bottom, Scalar top,
		Scalar near_plane, Scalar far_plane)
{
  assert(near_plane > 0.0 && far_plane > near_plane);

  GLMatrixT<Scalar> m;
  Scalar x, y, a, b, c, d;

  x = (right-left) / (2.0*near_plane);
  y = (top-bottom) / (2.0*near_plane);
  a = (right+left) / (2.0*near_plane);
  b = (top+bottom) / (2.0*near_plane);
  c = (far_plane+near_plane) / (2.0*far_plane*near_plane);
  d = (near_plane-far_plane) / (2.0*far_plane*near_plane);  

  m(0,0)=x;     m(0,1)= 0.0F;  m(0,2)= 0.0F;  m(0,3)= a;
  m(1,0)=0.0F;  m(1,1)= y;     m(1,2)= 0.0F;  m(1,3)= b;
  m(2,0)=0.0F;  m(2,1)= 0.0F;  m(2,2)= 0.0F;  m(2,3)= -1.0;
  m(3,0)=0.0F;  m(3,1)= 0.0F;  m(3,2)= d;     m(3,3)= c;

  leftMult(m);
}


//-----------------------------------------------------------------------------


template<typename Scalar> 
void
GLMatrixT<Scalar>::
ortho(Scalar left, Scalar right,
      Scalar bottom, Scalar top,
      Scalar near_plane, Scalar far_plane)
{
  GLMatrixT<Scalar> m;

  Scalar r_l = right-left;
  Scalar t_b = top - bottom;
  Scalar f_n = far_plane - near_plane;

//   assert(r_l > 0.0 && t_b > 0.0  && f_n > 0.0);

  m(1,0) = m(0,1) = m(0,2) =
  m(2,0) = m(2,1) = m(1,2) =
  m(3,0) = m(3,1) = m(3,2) = 0.0;
  
  m(0,0) =  2.0 / r_l;
  m(1,1) =  2.0 / t_b;
  m(2,2) = -2.0 / f_n;
  
  m(0,3) = -(right + left) / r_l;
  m(1,3) = -(top + bottom) / t_b;
  m(2,3) = -(far_plane + near_plane) / f_n;

  m(3,3) = 1.0;

  *this *= m;
}


//-----------------------------------------------------------------------------

template<typename Scalar> 
void
GLMatrixT<Scalar>::
inverse_ortho(Scalar left, Scalar right,
	      Scalar bottom, Scalar top,
	      Scalar near_plane, Scalar far_plane)
{
  GLMatrixT<Scalar> m;

  m(1,0) = m(0,1) = m(0,2) =
  m(2,0) = m(2,1) = m(1,2) =
  m(3,0) = m(3,1) = m(3,2) = 0.0;
  

  m(0,0) =  0.5 * (right - left);
  m(1,1) =  0.5 * (top - bottom);
  m(2,2) = -0.5 * (far_plane - near_plane);
  
  m(0,3) =  0.5 * (right + left);
  m(1,3) =  0.5 * (top + bottom);
  m(2,3) = -0.5 * (far_plane + near_plane);

  m(3,3) = 1.0;

  leftMult(m);
}


//-----------------------------------------------------------------------------


#undef MAT
#undef M


//=============================================================================
} // namespace ACG
//=============================================================================
