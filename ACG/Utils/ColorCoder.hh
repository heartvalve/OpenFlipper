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
 *   $Revision$                                                      *
 *   $Author$                                                      *
 *   $Date$                   *
 *                                                                           *
 \*===========================================================================*/

//=============================================================================
//
//  CLASS ColorCoder
//
//=============================================================================

#ifndef ACG_COLORCODER_HH
#define ACG_COLORCODER_HH

//== INCLUDES =================================================================

#include <ACG/Math/VectorT.hh>
#include <ACG/Config/ACGDefines.hh>
#include <QColor>

//== NAMESPACES ===============================================================

namespace ACG {

//== CLASS DEFINITION =========================================================

/** \brief Class for generating nice colors for doubles
 *
 *
 */
class ACGDLLEXPORT ColorCoder {
public:

  /// Default constructor.
  ColorCoder(float _min = 0.0, float _max = 1.0, bool _signed = false);

  /// set the color coding range for unsigned coding
  void set_range(float _min, float _max, bool _signed);

  /// color coding
  ACG::Vec4uc color4(float _v) const;

  /// color coding
  ACG::Vec3uc color(float _v) const;

  /// color coding
  ACG::Vec3f color_float(float _v) const;

  /// color coding
  ACG::Vec4f color_float4(float _v) const;

  /// color coding
  QColor color_qcolor(float _v) const;

  /// min scalar value
  float min() const;

  /// max scalar value
  float max() const;

  // Make the color coder usable as a function operator.
  inline ACG::Vec4f operator() (float _v) const {
      return color_float4(_v);
  }

private:

  ACG::Vec4uc color_unsigned(float _v) const;
  ACG::Vec4uc color_signed(float _v) const;

  float val0_, val1_, val2_, val3_, val4_;
  bool signed_mode_;
};

//=============================================================================
}// namespace ACG
//=============================================================================
#endif // ACG_COLORCODER_HH defined
//=============================================================================

