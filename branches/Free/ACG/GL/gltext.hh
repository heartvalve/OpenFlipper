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
//  overload some GL functions
//=============================================================================

#ifndef ACG_GL_TEXT_HH
#define ACG_GL_TEXT_HH

//== INCLUDES =================================================================


#include "../Math/VectorT.hh"
#include "gl.hh"
#include <string>


//=============================================================================
namespace ACG {
//=============================================================================


/// Text output in OpenGL, given 2D text position
void glText(const Vec2i&        _pos, 
	    const std::string&  _text,
	    void*               _font = GLUT_BITMAP_8_BY_13);

/// Text output in OpenGL, given 3D text position
void glText(const Vec3f&        _pos, 
	    const std::string&  _text,
	    void*               _font = GLUT_BITMAP_8_BY_13);


//=============================================================================
}  // namespace ACG
//=============================================================================
#endif // ACG_GL_TEXT_HH defined
//=============================================================================
