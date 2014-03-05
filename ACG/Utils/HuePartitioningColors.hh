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

#ifndef HUEPARTITIONINGCOLORS_HH_
#define HUEPARTITIONINGCOLORS_HH_

#include <ACG/Math/VectorT.hh>
#include "../Config/ACGDefines.hh"
#include "ColorGenerator.hh"

namespace ACG {

/**
 * The HuePartitioningColors generator tries to generate a set of well
 * distinguishable and esthetically somewhat pleasing colors.
 *
 * Note that it intentionally behaves totally deterministic. (Because
 * reproducibility rocks.)
 */
class ACGDLLEXPORT HuePartitioningColors : public ColorGenerator {
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
        HuePartitioningColors(
                float _alpha = 1.0f,
                float _baseHue = 0.5694f);

        /**
         * @return A new color.
         */
        virtual Vec4f generateNextColor();

        /**
         * Convenience method if you just need a bunch of
         * colors and don't need to instantiate a ColorGenerator.
         *
         * See description of generateNextNColors() for details.
         */
        template <class OUTPUT_ITERATOR>
        static void generateNColors(int n, OUTPUT_ITERATOR oit) {
            HuePartitioningColors cg;
            cg.generateNextNColors(n, oit);
        }

    private:
        int currentSubdiv_;
        int currentIt_;
        int currentTriadIt_;
        float alpha_;
        float baseHue_;

        const float defaultSaturation_, defaultValue_;
};

} /* namespace ACG */
#endif /* HUEPARTITIONINGCOLORS_HH_ */
