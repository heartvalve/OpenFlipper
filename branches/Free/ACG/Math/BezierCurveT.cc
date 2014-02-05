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
//  CLASS BezierCurve - IMPLEMENTATION
//
//=============================================================================

#define ACG_BEZIERCURVE_C

//== INCLUDES =================================================================


#include "BezierCurveT.hh"


//== IMPLEMENTATION ========================================================== 


namespace ACG {


//-----------------------------------------------------------------------------


template <class Point>
Point
BezierCurveT<Point>::
operator()(Scalar _t) const
{
  // copy controll points
  std::vector<Point> b(*this);

  unsigned int n = b.size()-1, k;
  Scalar t0(1.0-_t), t1(_t);


  // de Casteljau
  unsigned int i, j;
  for (i=0; i<n; ++i)
    for (j=0, k=n-i; j<k; ++j)
      b[j] = t0*b[j] + t1*b[j+1];


  return b[0];
}


//-----------------------------------------------------------------------------


template <class Point>
void
BezierCurveT<Point>::
subdivide(Scalar _t, Self& _curve0, Self& _curve1) const
{
  // copy controll points
  std::vector<Point> b(*this);

  int n = degree();
  Scalar t0(1.0-_t), t1(_t);


  _curve0.clear();
  _curve0.reserve(n+1);
  _curve1.clear();
  _curve1.reserve(n+1);

  std::vector<Point>  tmp;
  tmp.reserve(n+1);


  // de Casteljau
  int i, j, k;
  for (i=0; i<n; ++i)
  {
    _curve0.push_back(b[0]);
    tmp.push_back(b[n-i]);

    for (j=0, k=n-i; j<k; ++j)
      b[j] = t0*b[j] + t1*b[j+1];
  }

  _curve0.push_back(b[0]);
  tmp.push_back(b[0]);


  for (i=n; i>=0; --i)
    _curve1.push_back(tmp[i]);


  assert(_curve0.degree() == n);
  assert(_curve1.degree() == n);
}


//=============================================================================
} // namespace ACG
//=============================================================================
