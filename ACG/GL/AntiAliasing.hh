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
 *   $Revision: 18130 $                                                       *
 *   $Author: moebius $                                                      *
 *   $Date: 2014-02-05 10:41:16 +0100 (Wed, 05 Feb 2014) $                   *
 *                                                                           *
\*===========================================================================*/



//=============================================================================
//
//  Helper classes for Anti-Aliasing of render targets
//
//=============================================================================


#ifndef ANTI_ALIASING_HH
#define ANTI_ALIASING_HH


//== INCLUDES =================================================================

// GL
#include <ACG/GL/gl.hh>
#include <ACG/GL/globjects.hh>

// C++
#include <vector>

//== NAMESPACES ===============================================================

namespace ACG {


//== CLASS DEFINITION =========================================================


/*
OpenGL MSAA:

A post-processing fragment shader has to be compute colors per sample rather than per pixel for full MSAA.
Sample values are then weighted by filter weights to obtain the final pixel value.

Filter weights can be computed with several methods, for example:
 - distance based filter weights (computed with MSFilterWeights)
 - constant filter weights: 1/numSamples

Recommended resources on implementing MSAA with render to texture:
 http://diaryofagraphicsprogrammer.blogspot.com/2009/06/multisample-anti-aliasing.html
 http://www.humus.name/index.php?page=3D&ID=81
*/


#ifdef GL_ARB_texture_multisample

class ACGDLLEXPORT MSFilterWeights
{
public:

  MSFilterWeights(int _numSamples);

  virtual ~MSFilterWeights() { }

#if defined(GL_ARB_texture_buffer_object)

  // Initializes a texture buffer with filter weights in format GL_R32F.
  // This can be used for reading from multisampled textures in a shader.
  void asTextureBuffer(TextureBuffer& out);

#endif

  // get ptr to weights as array, can be used for setUniform() for example
  const float* asDataPtr() const {return &weights_[0];}


  float operator [] (int i) const {return weights_[i];}

  float getWeight(int i) const {return weights_[i];}

  int getNumSamples() const {return numSamples_;}

private:

  int numSamples_;
  std::vector<float> weights_;
};

#endif // GL_ARB_texture_multisample

// maybe add:
// - automatic generation of MSAA variants of a post processing fragment shader
// - blitting of multisampled render buffers
// - downsampling of supersampled FBOs

//=============================================================================
} // namespace ACG
//=============================================================================
#endif // ANTI_ALIASING_HH defined
//=============================================================================
