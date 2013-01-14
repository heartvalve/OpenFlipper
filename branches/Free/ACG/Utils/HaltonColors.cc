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
 *   $Revision$                                                      *
 *   $Author$                                                      *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/

//=============================================================================
//
//  CLASS HaltonColors (by Marcel Campen)
//
//=============================================================================


//== INCLUDES =================================================================


#include <ACG/Utils/HaltonColors.hh>


//== NAMESPACES ===============================================================

namespace ACG {

//== CLASS IMPLEMENTATION =========================================================


HaltonColors::HaltonColors(int skip)
{
  // skip first 250 sequence elements to lower discrepancy even further.
  current[0] = skip;
  current[1] = skip;
  current[2] = skip;

  // initialize prime bases for H,S,L. Especially the first should be small such that already
  // small numbers of generated colors are distributed over the whole color circle.
  bases[0] = 5;
  bases[1] = 13;
  bases[2] = 17;

  inverse_bases[0] = 1.0f / bases[0];
  inverse_bases[1] = 1.0f / bases[1];
  inverse_bases[2] = 1.0f / bases[2];
}

float HaltonColors::halton(int index)
{
  int base = bases[index];
  float inverse_base = inverse_bases[index];
  float H = 0;
  float half = inverse_base;
  int I = current[index];
  current[index] += 1;
  while (I > 0) {
    int digit = I % base;
    H = H + half * digit;
    I = inverse_base * (I - digit);
    half *= inverse_base;
  }
  return H;
}

float HaltonColors::random_interval(int index, float min, float max)
{
  return halton(index) * (max - min) + min;
}

ACG::Vec4f HaltonColors::HSL2RGB(double h, double sl, double l)
{
  double v;
  double r, g, b;

  r = l;
  g = l;
  b = l;

  v = (l <= 0.5) ? (l * (1.0 + sl)) : (l + sl - l * sl);

  if (v > 0) {
    double m;
    double sv;
    int sextant;
    double fract, vsf, mid1, mid2;

    m = l + l - v;
    sv = (v - m) / v;
    h *= 6.0;
    sextant = (int) h;
    fract = h - sextant;
    vsf = v * sv * fract;
    mid1 = m + vsf;
    mid2 = v - vsf;

    switch (sextant) {
      case 0:
        r = v;
        g = mid1;
        b = m;
        break;
      case 1:
        r = mid2;
        g = v;
        b = m;
        break;
      case 2:
        r = m;
        g = v;
        b = mid1;
        break;
      case 3:
        r = m;
        g = mid2;
        b = v;
        break;
      case 4:
        r = mid1;
        g = m;
        b = v;
        break;
      case 5:
        r = v;
        g = m;
        b = mid2;
        break;
    }
  }

  return Vec4f(r, g, b, 1.0f);
}

ACG::Vec4f HaltonColors::get_next_color()
{
  float h = random_interval(0, 0.0f , 0.9f ); // 0.9 instead of 1.0 to suppress natural bias towards red
  float s = random_interval(1, 0.40f, 0.80f); // saturation between 40% and 80%
  float l = random_interval(2, 0.30f, 0.60f); // lightness between 30% and 60%
  return HSL2RGB(h, s, l);
}


//=============================================================================
} // ACG namespace end
//=============================================================================
//=============================================================================

