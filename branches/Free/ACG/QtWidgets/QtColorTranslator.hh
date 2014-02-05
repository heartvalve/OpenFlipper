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
//  CLASS QtColorTranslator
//
//=============================================================================


#ifndef ACG_QTCOLORTRANSLATOR_HH
#define ACG_QTCOLORTRANSLATOR_HH


//== INCLUDES =================================================================

#undef QT_NO_OPENGL
#include <QGLContext>
#define QT_NO_OPENGL

#include "../Config/ACGDefines.hh"

//== NAMESPACES ===============================================================

namespace ACG {


//== CLASS DEFINITION =========================================================


/** \class QtColorTranslator QtColorTranslator.hh <ACG/QtWidgets/QtColorTranslator.hh>

    This class translates index <-> RGB color.

    The QtColorTranslator is attached to a QGLContext and translates
    index to color and vice versa. If the RGB bits of one buffer are
    not sufficient, both front and back buffer can be used.
**/

class ACGDLLEXPORT QtColorTranslator
{
public:

  /// Default constructor.
  QtColorTranslator()
    : initialized_(false) {};

  /// construct with QGLcontext
  QtColorTranslator(QGLContext& _context)
  { initialize(_context); }

  /// Destructor.
  ~QtColorTranslator(){};


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
#endif // ACG_QTCOLORTRANSLATOR_HH defined
//=============================================================================

