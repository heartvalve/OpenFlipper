/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2014 by Computer Graphics Group, RWTH Aachen       *
*                           www.openflipper.org                              *
*                                                                            *
*--------------------------------------------------------------------------- *
*  This file is part of OpenFlipper.                                         *
*                                                                            *
*  OpenFlipper is free software: you can redistribute it and/or modify       *
*  it under the terms of the GNU Lesser General Public License as            *
*  published by the Free Software Foundation, either version 3 of            *
*  the License, or (at your option) any later version with the               *
*  following exceptions:                                                     *
*                                                                            *
*  If other files instantiate templates or use macros                        *
*  or inline functions from this file, or you compile this file and          *
*  link it with other files to produce an executable, this file does         *
*  not by itself cause the resulting executable to be covered by the         *
*  GNU Lesser General Public License. This exception does not however        *
*  invalidate any other reasons why the executable file might be             *
*  covered by the GNU Lesser General Public License.                         *
*                                                                            *
*  OpenFlipper is distributed in the hope that it will be useful,            *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
*  GNU Lesser General Public License for more details.                       *
*                                                                            *
*  You should have received a copy of the GNU LesserGeneral Public           *
*  License along with OpenFlipper. If not,                                   *
*  see <http://www.gnu.org/licenses/>.                                       *
*                                                                            *
\*===========================================================================*/

/*===========================================================================*\
*                                                                            *
*   $Revision: 18692 $                                                       *
*   $LastChangedBy: moeller $                                                *
*   $Date: 2014-05-21 15:31:54 +0200 (Wed, 21 May 2014) $                     *
*                                                                            *
\*===========================================================================*/

#include <GL/glew.h>
#include <ACG/GL/GLError.hh>

#include "QtBaseViewer.hh"

/**
 * @file The intention of this file is to split QtBaseViewer up into two
 * compile units, one including glew.h and one including Qt. This is
 * necessary as glew and Qt5 conflict.
 */


void glViewer::readBackBuffer(ACG::GLState* _glstate)
{
  GLint curFbo = 0;
  glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &curFbo);

  ACG::GLState::bindFramebuffer(GL_READ_FRAMEBUFFER, curFbo);
  ACG::GLState::bindFramebuffer(GL_DRAW_FRAMEBUFFER, readBackFbo_.getFboID());

  glBlitFramebuffer(0, 0, readBackFbo_.width(), readBackFbo_.height(),
    0, 0, readBackFbo_.width(), readBackFbo_.height(),
    GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);

  checkGLError();

  ACG::GLState::bindFramebuffer(GL_FRAMEBUFFER, curFbo);
}
