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
//  CLASS ColorTranslator
//
//=============================================================================

#ifndef ACG_COLORTRANSLATOR_HH
#define ACG_COLORTRANSLATOR_HH


//== INCLUDES =================================================================


#include "gl.hh"
#include <ACG/Config/ACGDefines.hh>
#include <ACG/Math/VectorT.hh>


//== NAMESPACES ===============================================================


namespace ACG {


//== CLASS DEFINITION =========================================================


/** This class can be used to translate colors to integers. Use this
    e.g. for color index picking, i.e. drawing objects using a color
    corresponding to an unique index.
*/
class ACGDLLEXPORT ColorTranslator
{
public:
   
  /// Default constructor.
  ColorTranslator() : initialized_(false) {};
   /// Destructor.
  ~ColorTranslator() {};

  
  /// init (takes current GL context)
  void initialize();
  /// has it been initialized?
  bool initialized() const { return initialized_; }


  /// index -> color (one buffer)
  Vec4uc index2color(unsigned int _idx) const;
  /// color -> index (one buffer)
  int color2index(Vec4uc _rgba) const;


  /// returns maximal convertable index
  unsigned int max_index() const;
  
  
private:

  bool     initialized_;
  GLint    red_bits_, green_bits_, blue_bits_, alpha_bits_;
  GLuint   red_mask_, green_mask_, blue_mask_, alpha_mask_;
  GLuint   red_shift_, green_shift_, blue_shift_, alpha_shift_;
  GLuint   red_round_, green_round_, blue_round_, alpha_round_;
};


//=============================================================================
} // namespace ACG
//=============================================================================
#endif // ACG_COLORTRANSLATOR_HH defined
//=============================================================================

