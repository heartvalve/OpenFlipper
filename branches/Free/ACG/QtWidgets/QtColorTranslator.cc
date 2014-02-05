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
//  CLASS QtColorTranslator - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================

#include "QtColorTranslator.hh"
#include <iostream>
#include <cassert>


//== NAMESPACES ===============================================================

namespace ACG {


//== IMPLEMENTATION ========================================================== 


void
QtColorTranslator::initialize()
{
  glGetIntegerv( GL_RED_BITS,   &redBits_ );
  glGetIntegerv( GL_GREEN_BITS, &greenBits_ );
  glGetIntegerv( GL_BLUE_BITS,  &blueBits_ );    

  if (redBits_ > 8)   redBits_ = 8;
  if (greenBits_ > 8) greenBits_ = 8;
  if (blueBits_ > 8)  blueBits_ = 8;

  redMask_    = ((1 << redBits_)   - 1);
  greenMask_  = ((1 << greenBits_) - 1);
  blueMask_   = ((1 << blueBits_)  - 1);    
  redShift_   = 8 - redBits_;
  greenShift_ = 8 - greenBits_;
  blueShift_  = 8 - blueBits_;    
  redRound_   = 1 << (redShift_   - 1);
  greenRound_ = 1 << (greenShift_ - 1);
  blueRound_  = 1 << (blueShift_  - 1);

  initialized_ = true;

  if (redBits_   > 8) std::cerr << "error: more than 8 red bits\n";
  if (greenBits_ > 8) std::cerr << "error: more than 8 green bits\n";
  if (blueBits_  > 8) std::cerr << "error: more than 8 blue bits\n";
}

  
//-----------------------------------------------------------------------------


bool
QtColorTranslator::index2color(unsigned int _idx, QRgb& _col) const
{
  assert(initialized());
  unsigned char  r, g, b;
  unsigned int idx(_idx+1);
  
  b = ((idx & blueMask_)  << blueShift_)  | blueRound_;  
  idx >>= blueBits_;
  g = ((idx & greenMask_) << greenShift_) | greenRound_;  
  idx >>= greenBits_;
  r = ((idx & redMask_)   << redShift_)   | redRound_;  
  idx >>= redBits_;
  
  if (!idx)
  {
    _col = qRgb(r, g, b);
    return true;
  }
  else {
    std::cerr << "Can't convert index " << _idx << " to RGB\n";
    _col = qRgb(0,0,0);
    return false;
  }
}

  
//-----------------------------------------------------------------------------


bool
QtColorTranslator::index2color(unsigned int _idx, QRgb& _fc, QRgb& _bc) const
{
  assert(initialized());
  unsigned char  r, g, b;
  unsigned int idx(_idx+1);
  
  b = ((idx & blueMask_)  << blueShift_)  | blueRound_;  
  idx >>= blueBits_;
  g = ((idx & greenMask_) << greenShift_) | greenRound_;  
  idx >>= greenBits_;
  r = ((idx & redMask_)   << redShift_)   | redRound_;  
  idx >>= redBits_;
  
  if (!idx)
  {
    _bc = qRgb(r, g, b);
    _fc = qRgb(0,0,0);
    return true;
  }
  else
  {
    _bc = qRgb(r, g, b);

    b = ((idx & blueMask_)  << blueShift_)  | blueRound_;  
    idx >>= blueBits_;
    g = ((idx & greenMask_) << greenShift_) | greenRound_;  
    idx >>= greenBits_;
    r = ((idx & redMask_)   << redShift_)   | redRound_;  
    idx >>= redBits_;

    if (!idx)
    {
      _fc = qRgb(r,g,b);
      return true;
    }
    else
    {
      std::cerr << "Can't convert index " << _idx << " to RGB\n";
      _bc = qRgb(0,0,0);
      _fc = qRgb(0,0,0);
      return false;
    }
  }
}

  
//-----------------------------------------------------------------------------


int
QtColorTranslator::color2index(QRgb _c) const
{
  assert(initialized());
  unsigned int result;

  result =   qRed(_c) >> redShift_;
  result <<= greenBits_;
  result |=  qGreen(_c) >> greenShift_;
  result <<= blueBits_;
  result |=  qBlue(_c) >> blueShift_;    

  return (result-1);
}


//-----------------------------------------------------------------------------


int
QtColorTranslator::color2index(QRgb _fc, QRgb _bc) const
{
  assert(initialized());
  unsigned int result;

  result =   qRed(_fc) >> redShift_;
  result <<= greenBits_;
  result |=  qGreen(_fc) >> greenShift_;
  result <<= blueBits_;
  result |=  qBlue(_fc) >> blueShift_;    

  result <<= redBits_;
  result |=  qRed(_bc) >> redShift_;
  result <<= greenBits_;
  result |=  qGreen(_bc) >> greenShift_;
  result <<= blueBits_;
  result |=  qBlue(_bc) >> blueShift_;
  
  return (result-1);
}


//-----------------------------------------------------------------------------


unsigned int
QtColorTranslator::maxIndex() const
{
  assert(initialized());
  unsigned int result(~0);
  result >>= 32 - redBits_ - greenBits_ - blueBits_;
  return (result-1);
}


//=============================================================================
} // namespace ACG
//=============================================================================

