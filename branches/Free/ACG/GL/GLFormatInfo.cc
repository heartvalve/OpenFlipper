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

#include <ACG/GL/GLFormatInfo.hh>
#include <iostream>
#include <QString>


namespace ACG {


// MACOS fix

std::map<GLenum, GLFormatInfo> GLFormatInfo::formatMap_;

GLFormatInfo::GLFormatInfo( GLenum _internalFormat )
  : internalFormat_(_internalFormat), format_(GL_NONE), type_(GL_NONE),
  channelCount_(0), bpp_(0), baseType_(UnsignedInt), normalized_(true)
{
  channelBits_[0] = 
    channelBits_[1] = 
    channelBits_[2] = 
    channelBits_[3] = 0;
  sizedName_[0] = 0;

  if (formatMap_.empty())
  {
    registerFmt(GL_RED, GL_RED, GL_UNSIGNED_BYTE, 8, 0, 0, 0, UnsignedInt, true);
    registerFmt(GL_RG, GL_RG, GL_UNSIGNED_BYTE, 8, 8, 0, 0, UnsignedInt, true);
    registerFmt(GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, 8, 8, 8, 0, UnsignedInt, true);
    registerFmt(GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, 8, 8, 8, 8, UnsignedInt, true);

    registerFmt(GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, 32, 0, 0, 0, UnsignedInt, false);
    registerFmt(GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, 32, 0, 0, 0, UnsignedInt, false);
    registerFmt(GL_DEPTH_STENCIL, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, 24, 8, 0, 0, UnsignedInt, false);


#ifdef GL_VERSION_3_0
    // single channel formats
    registerFmt(GL_R8, GL_RED, GL_UNSIGNED_BYTE, 8, 0, 0, 0, UnsignedInt, true);
    registerFmt(GL_R8_SNORM, GL_RED, GL_BYTE, 8, 0, 0, 0, SignedInt, true);
    registerFmt(GL_R8I, GL_RED_INTEGER, GL_BYTE, 8, 0, 0, 0, SignedInt, false);
    registerFmt(GL_R8UI, GL_RED_INTEGER, GL_UNSIGNED_BYTE, 8, 0, 0, 0, UnsignedInt, false);

    registerFmt(GL_R16, GL_RED, GL_UNSIGNED_SHORT, 16, 0, 0, 0, UnsignedInt, true);
    registerFmt(GL_R16I, GL_RED_INTEGER, GL_SHORT, 16, 0, 0, 0, SignedInt, false);
    registerFmt(GL_R16UI, GL_RED_INTEGER, GL_UNSIGNED_SHORT, 16, 0, 0, 0, UnsignedInt, false);
    registerFmt(GL_R16F, GL_RED, GL_HALF_FLOAT, 16, 0, 0, 0, FloatingPt, false);

    registerFmt(GL_R32I, GL_RED_INTEGER, GL_INT, 32, 0, 0, 0, SignedInt, false);
    registerFmt(GL_R32UI, GL_RED_INTEGER, GL_UNSIGNED_INT, 32, 0, 0, 0, UnsignedInt, false);
    registerFmt(GL_R32F, GL_RED, GL_FLOAT, 32, 0, 0, 0, FloatingPt, false);

    // dual channel formats
    registerFmt(GL_RG8, GL_RG, GL_UNSIGNED_BYTE, 8, 8, 0, 0, UnsignedInt, true);
    registerFmt(GL_RG8_SNORM, GL_RG, GL_BYTE, 8, 8, 0, 0, SignedInt, true);
    registerFmt(GL_RG8I, GL_RG_INTEGER, GL_BYTE, 8, 8, 0, 0, SignedInt, false);
    registerFmt(GL_RG8UI, GL_RG_INTEGER, GL_UNSIGNED_BYTE, 8, 8, 0, 0, UnsignedInt, false);

    registerFmt(GL_RG16, GL_RG, GL_UNSIGNED_SHORT, 16, 16, 0, 0, UnsignedInt, true);
    registerFmt(GL_RG16I, GL_RG_INTEGER, GL_SHORT, 16, 16, 0, 0, SignedInt, false);
    registerFmt(GL_RG16UI, GL_RG_INTEGER, GL_UNSIGNED_SHORT, 16, 16, 0, 0, UnsignedInt, false);
    registerFmt(GL_RG16F, GL_RG, GL_HALF_FLOAT, 16, 16, 0, 0, FloatingPt, false);

    registerFmt(GL_RG32I, GL_RG_INTEGER, GL_INT, 32, 32, 0, 0, SignedInt, false);
    registerFmt(GL_RG32UI, GL_RG_INTEGER, GL_UNSIGNED_INT, 32, 32, 0, 0, UnsignedInt, false);
    registerFmt(GL_RG32F, GL_RG, GL_FLOAT, 32, 32, 0, 0, FloatingPt, false);


    // triple channel
    registerFmt(GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE, 8, 8, 8, 0, UnsignedInt, true);
    registerFmt(GL_RGB8_SNORM, GL_RGB, GL_BYTE, 8, 8, 8, 0, SignedInt, true);

    // quad channel
    registerFmt(GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, 8, 8, 8, 8, UnsignedInt, true);
    registerFmt(GL_RGBA8_SNORM, GL_RGBA, GL_BYTE, 8, 8, 8, 8, SignedInt, true);

    registerFmt(GL_RGBA16I, GL_RGBA_INTEGER, GL_SHORT, 16, 16, 16, 16, SignedInt, false);
    registerFmt(GL_RGBA16UI, GL_RGBA_INTEGER, GL_UNSIGNED_SHORT, 16, 16, 16, 16, UnsignedInt, false);
    registerFmt(GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT, 16, 16, 16, 16, FloatingPt, false);

    registerFmt(GL_RGBA32I, GL_RGBA_INTEGER, GL_INT, 32, 32, 32, 32, SignedInt, false);
    registerFmt(GL_RGBA32UI, GL_RGBA_INTEGER, GL_UNSIGNED_INT, 32, 32, 32, 32, UnsignedInt, false);
    registerFmt(GL_RGBA32F, GL_RGBA, GL_FLOAT, 32, 32, 32, 32, FloatingPt, false);
#endif // GL_VERSION_3_0

    // depth
    registerFmt(GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, 24, 0, 0, 0, UnsignedInt, false);
    registerFmt(GL_DEPTH_COMPONENT16, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, 16, 0, 0, 0, UnsignedInt, false);
#ifdef GL_ARB_depth_buffer_float
    registerFmt(GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, GL_FLOAT, 32, 0, 0, 0, FloatingPt, false);
    registerFmt(GL_DEPTH32F_STENCIL8, GL_DEPTH_STENCIL, GL_FLOAT_32_UNSIGNED_INT_24_8_REV, 32, 8, -24, 0, FloatingPt, false);
#endif // GL_ARB_depth_buffer_float

#ifdef GL_ARB_texture_stencil8
    registerFmt(GL_STENCIL_INDEX8, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE, 8, 0, 0, 0,  UnsignedInt, false);
#endif // GL_ARB_texture_stencil8
  }

  std::map<GLenum, GLFormatInfo>::iterator f = formatMap_.find(_internalFormat);

  if (f != formatMap_.end())
    *this = f->second;
  else
  {
    // need to register new format
    std::cout << "GLFormatInfo: unknown internalFormat " << _internalFormat << std::endl;
  }
}

GLFormatInfo::GLFormatInfo( GLenum _intfmt, GLenum _fmt, GLenum _type, int _r, int _g, int _b, int _a, BaseType _bt, bool _nrm )
  : internalFormat_(_intfmt), format_(_fmt), type_(_type),
  channelCount_(0), bpp_(0), baseType_(_bt), normalized_(_nrm)
{
  // negative channel bits indicate unused channel of size abs(bits)
  // it's assumed that the unused bits are just padding bits appended to the end

  channelBits_[0] = std::max(_r, 0);
  channelBits_[1] = std::max(_g, 0);
  channelBits_[2] = std::max(_b, 0);
  channelBits_[3] = std::max(_a, 0);
  sizedName_[0] = 0;

  if (_r > 0) ++channelCount_;
  if (_g > 0) ++channelCount_;
  if (_b > 0) ++channelCount_;
  if (_a > 0) ++channelCount_;

  bpp_ = std::abs(_r) + std::abs(_g) + std::abs(_b) + std::abs(_a);

  if (channelCount_ >= 1 && channelCount_ <= 4)
  {
    switch (_intfmt)
    {
    case GL_DEPTH_STENCIL: strcpy(sizedName_, "GL_DEPTH_STENCIL"); break;
    case GL_DEPTH_COMPONENT: strcpy(sizedName_, "GL_DEPTH_COMPONENT"); break;
    case GL_DEPTH_COMPONENT32: strcpy(sizedName_, "GL_DEPTH_COMPONENT32"); break;

    default:
      {
        // construct sized format string
        QString strfmt = "GL_R";

        if (channelCount_ > 1)
          strfmt += "G";
        if (channelCount_ > 2)
          strfmt += "B";
        if (channelCount_ > 3)
          strfmt += "A";

        QString bitCount;
        bitCount.sprintf("%i", channelBits_[0]);

        strfmt += bitCount;

        if (isNormalized())
        {
          if (isInt())
            strfmt += "_SNORM";
        }
        else
        {
          if (isFloat())
            strfmt += "F";
          else if (isInt())
            strfmt += "I";
          else if (isUint())
            strfmt += "UI";
        }

        int len = std::min(strfmt.length(), 31);

        for (int i = 0; i < len; ++i)
          sizedName_[i] = strfmt.at(i).toLatin1();
        sizedName_[len] = 0;
      } break;
    }
  }
}

GLFormatInfo::GLFormatInfo()
  : internalFormat_(GL_NONE), format_(GL_NONE), type_(GL_NONE),
  channelCount_(0), bpp_(0), baseType_(UnsignedInt), normalized_(true)
{
  channelBits_[0] = 
    channelBits_[1] = 
    channelBits_[2] = 
    channelBits_[3] = 0;
  sizedName_[0] = 0;
}

GLFormatInfo::~GLFormatInfo()
{
}

void GLFormatInfo::registerFmt( GLenum _intfmt, GLenum _fmt, GLenum _type, int _r, int _g, int _b, int _a, BaseType _bt, bool _nrm )
{
  formatMap_[_intfmt] = GLFormatInfo(_intfmt, _fmt, _type, _r, _g, _b, _a, _bt, _nrm);
}


} /* namespace ACG */
