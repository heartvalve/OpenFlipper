//=============================================================================
//
//                               OpenFlipper
//        Copyright (C) 2008 by Computer Graphics Group, RWTH Aachen
//                           www.openflipper.org
//
//-----------------------------------------------------------------------------
//
//                                License
//
//  OpenFlipper is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  OpenFlipper is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License
//  along with OpenFlipper.  If not, see <http://www.gnu.org/licenses/>.
//
//-----------------------------------------------------------------------------
//
//   $Revision$
//   $Author$
//   $Date$
//
//=============================================================================




//=============================================================================
//
//  CLASS ColorTranslator
//
//=============================================================================


#ifndef ACG_COLORTRANSLATOR_HH
#define ACG_COLORTRANSLATOR_HH


//== INCLUDES =================================================================

#include <QGLContext>
#include "../Config/ACGDefines.hh"

//== NAMESPACES ===============================================================

namespace ACG {


//== CLASS DEFINITION =========================================================


/** \class ColorTranslator ColorTranslator.hh <ACG/QtWidgets/ColorTranslator.hh>

    This class translates index <-> RGB color.

    The ColorTranslator is attached to a QGLContext and translates
    index to color and vice versa. If the RGB bits of one buffer are
    not sufficient, both front and back buffer can be used.
**/

class ACGDLLEXPORT ColorTranslator
{
public:

  /// Default constructor.
  ColorTranslator()
    : initialized_(false) {};

  /// construct with QGLcontext
  ColorTranslator(QGLContext& _context)
  { initialize(_context); }

  /// Destructor.
  ~ColorTranslator(){};


  /// init (takes current QGLcontext)
  void initialize();

  /// init with given QGLcontext
  void initialize(QGLContext& _context)
  { _context.makeCurrent();  initialize(); }

  /// has it been initialized?
  bool initialized() const { return initialized_; }


  /// index -> color (one buffer)
  bool index2color( unsigned int _idx, QRgb& _col ) const;
  /// index -> color (two buffers)
  bool index2color( unsigned int _idx,
		    QRgb& _frontColor,
		    QRgb& _backColor ) const;


  /// color -> index (one buffer)
  int color2index( QRgb _c ) const;
  /// color -> index (two buffers)
  int color2index( QRgb _frontColor, QRgb _backColor ) const;


  /// returns max convertable index (using ONE buffer)
  unsigned int maxIndex() const;


private:

  bool     initialized_;
  GLint    redBits_, greenBits_, blueBits_;
  GLuint   redMask_, greenMask_, blueMask_,
           redShift_, greenShift_, blueShift_,
           redRound_, greenRound_, blueRound_;
};


//=============================================================================
} // namespace ACG
//=============================================================================
#endif // ACG_COLORTRANSLATOR_HH defined
//=============================================================================

