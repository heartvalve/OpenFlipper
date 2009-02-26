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
//  CLASS ColorTranslator - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================


#include "ColorTranslator.hh"
#include <iostream>


//== NAMESPACES ===============================================================


namespace ACG {
namespace SceneGraph {


//== IMPLEMENTATION ========================================================== 


void
ColorTranslator::
initialize()
{
  glGetIntegerv( GL_RED_BITS,   &red_bits_   );
  glGetIntegerv( GL_GREEN_BITS, &green_bits_ );
  glGetIntegerv( GL_BLUE_BITS,  &blue_bits_  );

  if (red_bits_   > 8)  red_bits_   = 8;
  if (green_bits_ > 8)  green_bits_ = 8;
  if (blue_bits_  > 8)  blue_bits_  = 8;

  red_mask_    = ((1 << red_bits_)   - 1);
  green_mask_  = ((1 << green_bits_) - 1);
  blue_mask_   = ((1 << blue_bits_)  - 1);    

  red_shift_   = 8 - red_bits_;
  green_shift_ = 8 - green_bits_;
  blue_shift_  = 8 - blue_bits_;    

  red_round_   = 1 << (red_shift_   - 1);
  green_round_ = 1 << (green_shift_ - 1);
  blue_round_  = 1 << (blue_shift_  - 1);

  initialized_ = true;
}

  
//-----------------------------------------------------------------------------


Vec3uc
ColorTranslator::
index2color(unsigned int _idx) const 
{
  assert(initialized());
  unsigned char  r, g, b;
  unsigned int   idx(_idx+1);
  
  b = ((idx & blue_mask_)  << blue_shift_)  | blue_round_;  
  idx >>= blue_bits_;
  g = ((idx & green_mask_) << green_shift_) | green_round_;  
  idx >>= green_bits_;
  r = ((idx & red_mask_)   << red_shift_)   | red_round_;  
  idx >>= red_bits_;
  
  if (!idx) 
    return  Vec3uc(r, g, b);

  else 
  {
    std::cerr << "Can't convert index " << _idx << " to RGB\n";
    return Vec3uc(0, 0, 0);
  }
}

  
//-----------------------------------------------------------------------------


int
ColorTranslator::
color2index(Vec3uc _rgb) const 
{
  assert(initialized());
  unsigned int result;

  result =   _rgb[0] >> red_shift_;
  result <<= green_bits_;
  result |=  _rgb[1] >> green_shift_;
  result <<= blue_bits_;
  result |=  _rgb[2] >> blue_shift_;    

  return (result-1);
}


//-----------------------------------------------------------------------------


unsigned int
ColorTranslator::max_index() const 
{
  assert(initialized());
  return (1 << (red_bits_+green_bits_+blue_bits_))-1;
}


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================

