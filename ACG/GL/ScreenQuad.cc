/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2011 by Computer Graphics Group, RWTH Aachen      *
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
 *   $Revision:  $                                                         *
 *   $Author:  $                                                      *
 *   $Date:  $                   *
 *                                                                           *
\*===========================================================================*/



//=============================================================================
//
//  CLASS ScreenQuad - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================


#include "ScreenQuad.hh"
#include <ACG/ShaderUtils/GLSLShader.hh>
#include <ACG/GL/GLError.hh>

//== NAMESPACES ===============================================================


namespace ACG {


//== IMPLEMENTATION ========================================================== 

ScreenQuad::ScreenQuad () :
  vbo_(0),
  decl_(0),
  texDrawProg_(0)
{
}

//----------------------------------------------------------------------------

ScreenQuad::~ScreenQuad ()
{
  if (vbo_)
    glDeleteBuffers(1, &vbo_);

  delete decl_;
  delete texDrawProg_;
}

//----------------------------------------------------------------------------

ScreenQuad& ScreenQuad::instance ()
{
  static ScreenQuad singleton;
  return singleton;
}

//----------------------------------------------------------------------------

void ScreenQuad::init ()
{
  if (!decl_)
  {
    decl_ = new VertexDeclaration();
    decl_->addElement(GL_FLOAT, 3, ACG::VERTEX_USAGE_POSITION);
  }

  if (!vbo_)
  {
    float quad[] = 
    {
      -1.0f, -1.0f, -1.0f, 
      1.0f, -1.0f, -1.0f,
      1.0f,  1.0f, -1.0f,
      -1.0f,  1.0f, -1.0f
    };

    glGenBuffers(1, &vbo_);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    ACG::glCheckErrors();
  }


  if (!texDrawProg_)
    texDrawProg_ = GLSL::loadProgram("ScreenQuad/screenquad.glsl", "ScreenQuad/tex2D.glsl");
}

//----------------------------------------------------------------------------

void ScreenQuad::draw (GLSL::Program* _prog)
{
  ScreenQuad& quad = instance();

  quad.intDraw(_prog);
}

//----------------------------------------------------------------------------

void ScreenQuad::intDraw (GLSL::Program* _prog)
{
  if (!vbo_)
  {
    init();
  }


  glBindBuffer(GL_ARRAY_BUFFER, vbo_);

  if (_prog)
    decl_->activateShaderPipeline(_prog);
  else
    decl_->activateFixedFunction();

  glPolygonMode(GL_FRONT, GL_FILL);

  glDrawArrays(GL_QUADS, 0, 4);

  if (_prog)
    decl_->deactivateShaderPipeline(_prog);
  else
    decl_->deactivateFixedFunction();
}

void ScreenQuad::drawTexture2D( GLuint _texture, const Vec2f& _offset /*= Vec2f(0.0f, 0.0f)*/, const Vec2f& _size /*= Vec2f(1.0f, 1.0f)*/ )
{
  ScreenQuad& quad = instance();

  if (!quad.texDrawProg_)
    quad.init();

  if (quad.texDrawProg_)
  {
    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, _texture);


    quad.texDrawProg_->use();

    quad.texDrawProg_->setUniform("Tex", 0); // texslot 0

    quad.texDrawProg_->setUniform("offset", _offset);
    quad.texDrawProg_->setUniform("size", _size);


    quad.intDraw(quad.texDrawProg_);


    quad.texDrawProg_->disable();
  }
}

//----------------------------------------------------------------------------

//=============================================================================
} // namespace ACG
//=============================================================================

