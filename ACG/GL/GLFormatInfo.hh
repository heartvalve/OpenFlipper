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
*                                                                            *
 *   $Revision$                                                       *
 *   $LastChangedBy$                                                *
 *   $Date$                     *
 *                                                                            *
 \*===========================================================================*/



//=============================================================================
//
//  OpenGL texture format info
//
//=============================================================================


#ifndef GL_FORMATINFO_HH
#define GL_FORMATINFO_HH


//== INCLUDES =================================================================

// GL
#include <ACG/GL/gl.hh>

// C++
#include <map>


//== NAMESPACES ===============================================================

namespace ACG {

//== CLASS DEFINITION =========================================================

class ACGDLLEXPORT GLFormatInfo
{
public:
  GLFormatInfo(GLenum _internalFormat);

  GLFormatInfo();
  ~GLFormatInfo();

  // size in bytes of the i'th channel
  int channelSize(int i = 0) const {assert(i >= 0 && i < channelCount_); return channelBits_[i] >> 3;}

  // size in bits of the i'th channel
  int channelBits(int i) const {assert(i >= 0 && i < channelCount_); return channelBits_[i];}

  // number of channels
  int channelCount() const {return channelCount_;}

  // size in bytes of one element = num channels * channel size
  int elemSize() const {return bpp_ >> 3;}

  // bits per pixel
  int bpp() const {return bpp_;}

  // get internal format
  GLenum internalFormat() const {return internalFormat_;}

  // get a fitting (external) format for the internalfmt, ie. GL_RED, GL_RGBA, GL_RGBA_INTEGER etc.
  GLenum format() const {return format_;}

  // get a fitting data-type for the internalfmt, ie GL_UNSIGNED_BYTE, GL_FLOAT etc.
  GLenum type() const {return type_;}

  // some formats such as GL_RGBA, GL_R8 etc. are normalized to [0,1] when sampled
  bool isNormalized() const {return normalized_;}

  // data can be one of 3 unsized base types: floating pt, signed integer or unsigned integer
  enum BaseType
  {
    FloatingPt,
    SignedInt,
    UnsignedInt
  };

  bool isFloat() const {return baseType_ == FloatingPt;}
  bool isUint() const {return baseType_ == UnsignedInt;}
  bool isInt() const {return baseType_ == SignedInt;}

  bool isValid() const {return bpp_ != 0;}

  BaseType baseType() const {return baseType_;}

  // return string of the sized internalFormat, ie. GL_RGBA8, GL_RG32F etc.
  const char* sizedFormatString() const {return sizedName_;}

private:
  GLFormatInfo( GLenum _intfmt, GLenum _fmt, GLenum _type, int _r, int _g, int _b, int _a, BaseType _bt, bool _nrm);
  static void registerFmt(GLenum _intfmt, GLenum _fmt, GLenum _type, int _r, int _g, int _b, int _a, BaseType _bt, bool _nrm);

  GLenum internalFormat_,
    format_,
    type_;

  int channelBits_[4];

  int channelCount_,
    bpp_;

  // 0 -> floating point,  1 -> unsigned integer,  2 -> signed integer
  BaseType baseType_;

  bool normalized_;

  char sizedName_[32];

  // map from internalfmt to info
  static std::map<GLenum, GLFormatInfo> formatMap_;
};


//=============================================================================
} // namespace ACG
//=============================================================================
#endif // GL_FORMATINFO_HH defined
//=============================================================================
