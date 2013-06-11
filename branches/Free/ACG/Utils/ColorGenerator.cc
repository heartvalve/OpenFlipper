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

#include "ColorGenerator.hh"

#include <cmath>
#include <QColor>

namespace ACG {

#define DEFAULT_SATURATION 1.0f
#define DEFAULT_VALUE      1.0f

ColorGenerator::ColorGenerator(float _alpha, float _baseHue) :
        currentSubdiv_(2), currentIt_(0), currentTriadIt_(0),
        alpha_(_alpha), baseHue_(_baseHue),defaultSaturation_(DEFAULT_SATURATION),defaultValue_(DEFAULT_VALUE)  {
}

static inline float wrap01(float v) {
    float dummy;
    if (v > 1.0f) v = modff(v, &dummy);
    if (v < 0.0f) v = 1.0f - modff(v, &dummy);
    return v;
}

Vec4f ColorGenerator::generateNextColor() {
    const float resultHue =
            baseHue_
            + .33333333f / currentSubdiv_ * currentIt_
            + .33333333f * currentTriadIt_;

    // Convert color to RGB and store result.
    double r, g, b, a;
    QColor::fromHsvF(wrap01(resultHue), defaultSaturation_, defaultValue_, alpha_).getRgbF(
            &r, &g, &b, &a);
    const Vec4f result(r, g, b, alpha_);

    /*
     * Increment the three level iterator state.
     */

    // Level 1: Increment triad it.
    if (++currentTriadIt_ <= 2)
        return result;

    // Level 2: Increment subdiv it.
    currentTriadIt_ = 0;

    // Starting from the second subdivison, only visit odd numbers.
    currentIt_ += (currentSubdiv_ == 2 ? 1 : 2);
    if (currentIt_ < currentSubdiv_)
        return result;

    // Level 3: Increment subdivision level.
    currentIt_ = 1;
    currentSubdiv_ *= 2;

    return result;
}

} /* namespace ACG */
