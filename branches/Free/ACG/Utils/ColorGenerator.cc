/*
 * ColorGenerator.cc
 *
 *  Created on: Sep 28, 2011
 *      Author: Hans-Christian Ebke
 */

#include "ColorGenerator.hh"

#include <cmath>
#include <qcolor.h>

namespace ACG {

const float ColorGenerator::defaultSaturation_  = 1.0f;
const float ColorGenerator::defaultValue_  = 1.0f;

ColorGenerator::ColorGenerator(float _alpha, float _baseHue) :
        currentSubdiv_(2), currentIt_(0), currentTriadIt_(0),
        alpha_(_alpha), baseHue_(_baseHue) {
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
