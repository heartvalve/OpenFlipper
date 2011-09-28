/*
 * ColorGenerator.hh
 *
 *  Created on: Sep 28, 2011
 *      Author: Hans-Christian Ebke
 */

#ifndef COLORGENERATOR_HH_
#define COLORGENERATOR_HH_

#include <ACG/Math/VectorT.hh>

namespace ACG {

/**
 * The ColorGenerator tries to generate a set of well distinguishable
 * and esthetically somewhat pleasing colors.
 *
 * Note that it intentionally behaves totally deterministic. (Because
 * reproducibility rocks.)
 */
class ColorGenerator {
    public:
        /**
         * Constructor
         *
         * @param _alpha The alpha value for all the colors.
         *
         * @param _baseHue The HSV-hue from which to start. This is the
         * hue of the first requested color. Default is an utterly
         * delighting shade of blue awesomeness.
         */
        ColorGenerator(
                float _alpha = 1.0f,
                float _baseHue = 0.5694f);

        /**
         * @return A new color.
         */
        Vec4f generateNextColor();

        /**
         * Put a bunch of colors into the output iterator.
         *
         * Generates the same colors as if calling
         * generateNextColor() n times.
         *
         * Typically, you would use std::back_inserter(container) as
         * the argument for oit.
         *
         * @param n How many colors to generate.
         * @param oit An output iterator. Should support "*oit++ = <Vec4f>;".
         */
        template <class OUTPUT_ITERATOR>
        void generateNextNColors(int n, OUTPUT_ITERATOR oit) {
            for (int i = 0; i < n; ++i)
                *oit++ = generateNextColor();
        }

        /**
         * Convenience method if you just need a bunch of
         * colors and don't need to instantiate a ColorGenerator.
         *
         * See description of generateNextNColors() for details.
         */
        template <class OUTPUT_ITERATOR>
        static void generateNColors(int n, OUTPUT_ITERATOR oit) {
            ColorGenerator cg;
            cg.generateNextNColors(n, oit);
        }

    private:
        int currentSubdiv_;
        int currentIt_;
        int currentTriadIt_;
        float alpha_;
        float baseHue_;

        static const float defaultSaturation_, defaultValue_;
};

} /* namespace ACG */
#endif /* COLORGENERATOR_HH_ */
