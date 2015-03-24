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
*   $Revision: 18131 $                                                       *
*   $LastChangedBy: moebius $                                                *
*   $Date: 2014-02-05 10:48:26 +0100 (Wed, 05 Feb 2014) $                     *
*                                                                            *
\*===========================================================================*/


#include <ACG/GL/acg_glew.hh>
#include "PostProcessorInterface.hh"



PostProcessorInput::PostProcessorInput( GLuint _colTex, GLuint _depthTex, int _width, int _height) : colorTex_(_colTex), depthTex_(_depthTex), width(_width), height(_height), sampleCount_(0)
{
  view_.identity(); 
  proj_.identity();
  depthRange_[0] = 0.0; depthRange_[1] = 1.0f;
}


void PostProcessorInput::bindColorTex( int _texSlot /*= 0*/ ) const
{
  glActiveTexture(GL_TEXTURE0 + _texSlot);
  glBindTexture(sampleCount_ ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D, colorTex_);
}

void PostProcessorInput::bindDepthTex( int _texSlot /*= 0*/ ) const
{
  glActiveTexture(GL_TEXTURE0 + _texSlot);
  glBindTexture(sampleCount_ ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D, depthTex_);
}


PostProcessorOutput::PostProcessorOutput( GLuint _fbo /*= 0*/, GLuint _drawBuffer /*= 0*/, int _width /*= 0*/, int _height /*= 0*/, const GLint* _viewport /*= 0*/ ) : fbo_(_fbo), drawBuffer_(_drawBuffer), width(_width), height(_height)
{
  if (_viewport)
  {
    viewport_[0] = _viewport[0];
    viewport_[1] = _viewport[1];
    viewport_[2] = _viewport[2];
    viewport_[3] = _viewport[3];
  }
  else
  {
    viewport_[0] = viewport_[1] = 0;
    viewport_[2] = _width; 
    viewport_[3] = _height;
  }
}

void PostProcessorOutput::bind() const
{
  glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
  glDrawBuffer(drawBuffer_);
  glViewport(viewport_[0], viewport_[1], viewport_[2], viewport_[3]);
}
