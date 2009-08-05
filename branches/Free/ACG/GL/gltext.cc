/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2009 by Computer Graphics Group, RWTH Aachen      *
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


//== INCLUDES =================================================================

#include "gltext.hh"

//=============================================================================

namespace ACG {

//=============================================================================


void glText(const Vec3f&        _pos, 
	    const std::string&  _text,
	    void*               _font)
{
  glRasterPos3fv(_pos.data());
  
  std::string::const_iterator s_it(_text.begin()), s_end(_text.end());
  for (; s_it!=s_end; ++s_it)
    glutBitmapCharacter(_font, *s_it);
}


//-----------------------------------------------------------------------------


void glText( const Vec2i&        _pos,
	     const std::string&  _text,
	     void*               _font)
{
  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);

  // set raster pos
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D(0.0, (GLfloat) viewport[2], 0.0, (GLfloat) viewport[3]);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glRasterPos2i(_pos[0], _pos[1]);
  

  // draw characters
  std::string::const_iterator s_it(_text.begin()), s_end(_text.end());
  for (; s_it!=s_end; ++s_it)
    glutBitmapCharacter(_font, *s_it);


  // restore matrices
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
}


//=============================================================================
}  // namespace ACG
//=============================================================================
