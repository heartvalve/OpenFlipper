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


#ifndef ACG_HALTONCOLORS_HH
#define ACG_HALTONCOLORS_HH

//== INCLUDES =================================================================


#include <ACG/Math/VectorT.hh>
#include "../Config/ACGDefines.hh"


//== NAMESPACES ===============================================================

namespace ACG {

  //== CLASS DEFINITION =========================================================

/** \brief Implementation of halton colors
 Provides deterministic pseudo-random low-discrepancy colors with a
 uniform distribution over a visually pleasing subset of HSL space,
 independent of the number of colors required.
 Simply instantiate and use get_next_color().
*/
class ACGDLLEXPORT HaltonColors {

public:

  /// Default constructor
  HaltonColors(int skip = 250);

  /// Generate the next color
  ACG::Vec4f get_next_color();

private:

  float halton(int index);
  float random_interval(int index, float min, float max);
  ACG::Vec4f HSL2RGB(double h, double sl, double l);

  int current[3]; // current Halton index
  int bases[3];   // Halton prime bases
  float inverse_bases[3];
};


  //=============================================================================
} 
//=============================================================================
#endif // ACG_HALTONCOLORS_HH defined
//=============================================================================

