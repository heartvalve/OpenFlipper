/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2014 by Computer Graphics Group, RWTH Aachen      *
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
//  CLASS QuadricT
//
//=============================================================================

#ifndef ACG_QUADRIC_HH
#define ACG_QUADRIC_HH


//== INCLUDES =================================================================


#include "../../Math/VectorT.hh"


//== NAMESPACE ================================================================


namespace ACG {
namespace Geometry {


//== CLASS DEFINITION =========================================================


/** /class QuadricT QuadricT.hh <ACG/Geometry/Types/QuadricT.hh>

    Stores a quadric as a 4x4 symmetrix matrix. Used by the
    error quadric based mesh decimation algorithms.
**/

template <class Scalar>
class ACGDLLEXPORT QuadricT
{
public:


  /// construct with upper triangle of symmetrix 4x4 matrix
  QuadricT(Scalar _a, Scalar _b, Scalar _c, Scalar _d,
	              Scalar _e, Scalar _f, Scalar _g,
	                         Scalar _h, Scalar _i,
	                                    Scalar _j)
    : a(_a), b(_b), c(_c), d(_d),
             e(_e), f(_f), g(_g),
                    h(_h), i(_i),
                           j(_j)
  {}


  /// constructor from given plane equation: ax+by+cz+d=0
  QuadricT( Scalar _a=0.0, Scalar _b=0.0, Scalar _c=0.0, Scalar _d=0.0 )
    :  a(_a*_a), b(_a*_b),  c(_a*_c),  d(_a*_d),
                 e(_b*_b),  f(_b*_c),  g(_b*_d),
                            h(_c*_c),  i(_c*_d),
                                       j(_d*_d)
  {}


  /// set all entries to zero
  void clear()  { a = b = c = d = e = f = g = h = i = j = 0.0; }


  /// add quadrics
  QuadricT<Scalar>& operator+=( const QuadricT<Scalar>& _q )
  {
    a += _q.a;  b += _q.b;  c += _q.c;  d += _q.d;
                e += _q.e;  f += _q.f;  g += _q.g;
                            h += _q.h;  i += _q.i;
			                j += _q.j;
    return *this;
  }


  /// multiply by scalar
  QuadricT<Scalar>& operator*=( Scalar _s)
  {
    a *= _s;  b *= _s;  c *= _s;  d *= _s;
              e *= _s;  f *= _s;  g *= _s;
                        h *= _s;  i *= _s;
                                  j *= _s;
    return *this;
  }


  /// multiply 4D vector from right: Q*v
  VectorT<Scalar,4> operator*(const VectorT<Scalar,4>& _v) const
  {
    return VectorT<Scalar,4>(_v[0]*a + _v[1]*b + _v[2]*c + _v[3]*d,
			     _v[0]*b + _v[1]*e + _v[2]*f + _v[3]*g,
			     _v[0]*c + _v[1]*f + _v[2]*h + _v[3]*i,
			     _v[0]*d + _v[1]*g + _v[2]*i + _v[3]*j);
  }


  /// evaluate quadric Q at vector v: v*Q*v
  Scalar operator()(const VectorT<Scalar,3> _v) const
  {
    Scalar x(_v[0]), y(_v[1]), z(_v[2]);
    return a*x*x + 2.0*b*x*y + 2.0*c*x*z + 2.0*d*x
                 +     e*y*y + 2.0*f*y*z + 2.0*g*y
	                     +     h*z*z + 2.0*i*z
                                         +     j;
  }


  /// evaluate quadric Q at vector v: v*Q*v
  Scalar operator()(const VectorT<Scalar,4> _v) const
  {
    Scalar x(_v[0]), y(_v[1]), z(_v[2]), w(_v[3]);
    return a*x*x + 2.0*b*x*y + 2.0*c*x*z + 2.0*d*x*w
                 +     e*y*y + 2.0*f*y*z + 2.0*g*y*w
	                     +     h*z*z + 2.0*i*z*w
                                         +     j*w*w;
  }


private:

  Scalar a, b, c, d,
            e, f, g,
               h, i,
                  j;
};


/// Quadric using floats
typedef QuadricT<float> Quadricf;

/// Quadric using double
typedef QuadricT<double> Quadricd;


//=============================================================================
} // namespace Geometry
} // namespace ACG
//=============================================================================
#endif // ACG_QUADRIC_HH defined
//=============================================================================
