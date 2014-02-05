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

#ifndef TEXTUREMATH_HH
#define TEXTUREMATH_HH

#include "TextureParameters.hh"

class TextureMath {
public:

  /** \ brief Initialize converter
   *
   * The values are modified in the following way:\n
   *
   * First the absolute value of the property is taken if requested (_abs)
   *
   * Then this value is clamped against the given values (_clamp,_clampMin,_clampMax)
   *
   * If the texture should be repeated, the values are translated such that the minimum is at min_val(0.0 by default)
   * and than scaled such that the maximum is at max_val. This data is than passed to the rendering (_repeat,_minRepeat,_maxRepeat)
   *
   * If the texture should not be repeated you could choose to center the data around 0.5. The negative values are than mapped to 0..0.5
   * and the positive values to 0.5..1 (_center)
   *
   * Otherwise, the values are mapped directly to 0..1. You can also disable that final scaling with (_scale)
   *
   * @param _abs           Take absolute value?
   * @param _clamp         Clamp values?
   * @param _clampMin      Minimal clamping value
   * @param _clampMax      Maximal Clamping value
   * @param _repeat        Repeat Texture?
   * @param _minRepeat     Minimal value for repeat
   * @param _maxRepeat     Maximal value for repeat
   * @param _center        Center values?
   * @param _scale         Scale values?
   * @param _minimalInput  Minimal input value given
   * @param _maximalInput  Maximal input value given
   */
  TextureMath(
      const bool _abs,
      const bool _clamp,
      const double _clampMin,
      const double _clampMax,
      const bool _repeat,
      const double _minRepeat,
      const double _maxRepeat,
      const bool _center,
      const bool _scale,
      const double _minimalInput,
      const double _maximalInput);

  /** \brief Convenience constructor
   *
   * @param _parameters texture parameters
   * @param _minimalInput
   * @param _maximalInput
   */
  TextureMath( const TexParameters& _parameters,
               const double _minimalInput,
               const double _maximalInput);

  /** \brief Modify given values based on the specified parameters given to the constructor.
   *
   *
   * @param _input Input value
   * @return       Modified value
   */
   double transform(const double _input) const;

private:
   bool   abs_;
   bool   clamp_;
   double clampMin_;
   double clampMax_;
   bool   repeat_;
   double repeatMin_;
   double repeatMax_;
   bool   center_;
   bool   scale_;

   double minInput_;
   double maxInput_;
};

#endif // TEXTUREMATH_HH
