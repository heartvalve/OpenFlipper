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
//  CLASS PBuffer
//
//=============================================================================
#ifdef ARCH_LINUX
//=============================================================================


#ifndef ACG_PBUFFER_HH
#define ACG_PBUFFER_HH


//== INCLUDES =================================================================

#include <GL/glxew.h>
#include <../GL/gl.hh>
#include <vector>
#include "../Config/ACGDefines.hh"


//== CLASS DEFINITION =========================================================



/** \class PBuffer PBuffer.hh <ACG/Utils/PBuffer.hh>
  Sets up a pbuffer and provides functions for working with it. Per
  default the pbuffer is double-buffered, 8-bit rgba values, 24-bit
  depth and preserved contents.
*/
class ACGDLLEXPORT PBuffer
{
public:

  /// Default constructor
  PBuffer(int _bits);

  /** Constructor with arguments for pbuffer dimension.
      \param _w specifies the pbuffer's width in pixel.
      \param _h specifies the pbuffer's height in pixel.
  */
  PBuffer(int _w, int _h, int _bits);

  /// Destructor
  ~PBuffer();

  /** Change size of pbuffer by destroying it and recreating it with
      the desired width and height.
      \param _w specifies the pbuffer's width in pixel.
      \param _h specifies the pbuffer's height in pixel.
  */
  void resize(int _w, int _h);

  /** Activate the pbuffer. This means the pbuffer is the current rendering
      context and all subsequent OpenGL-commands are executed in this context.
  */
  void activate();

  /** Switch back to the original rendering context. No further OpenGL-commands
      are directed to the pbuffer.
  */
  void deactivate();

  /// Get current width of pbuffer.
  const int width() const;

  /// Get current height of pbuffer.
  const int height() const;

  int bits();


private:

  Display           *dpy_;
  GLXFBConfig       *fbc_;
  GLXPbuffer        pbuf_;
  GLXDrawable       currdraw_;
  GLXContext        currctx_, pbufctx_;
  std::vector<int>  sbAttrib_, pbAttrib_;
};


//=============================================================================
#endif // ACG_PBUFFER_HH defined
//=============================================================================
#endif // Linux only
//=============================================================================

