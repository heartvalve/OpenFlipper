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
//  CLASS Texture3DNode - IMPLEMENTATION
//
//=============================================================================

//=============================================================================


#include "Texture3DNode.hh"
#include "DrawModes.hh"

//=============================================================================

namespace ACG {
namespace SceneGraph {

//=============================================================================


Texture3DNode::Texture3DNode( BaseNode          * _parent, 
			      const std::string & _name )
  : BaseNode( _parent, _name ),
    texture_( 0 ), 
    tex_mode_( GL_MODULATE ),
    border_color_( 0, 0, 0, 0 ),
    wrap_mode_( GL_CLAMP ),
    filter_( GL_LINEAR )
{}

    
//----------------------------------------------------------------------------


Texture3DNode::~Texture3DNode()
{
  if ( glIsTexture( texture_ ) )
    glDeleteTextures( 1, &texture_ );
}


//----------------------------------------------------------------------------
  

void
Texture3DNode::enter( GLState &    /*_state */, const DrawModes::DrawMode& /* _drawmode */ ) 
{
  if ( glIsTexture( texture_ ) )
  {
    ACG::GLState::bindTexture( GL_TEXTURE_3D, texture_ );
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, tex_mode_);
    glTexParameterfv( GL_TEXTURE_3D, GL_TEXTURE_BORDER_COLOR, border_color_.data() );

    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, wrap_mode_ );
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, wrap_mode_ );
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, wrap_mode_ );

    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, filter_ );
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, filter_ );

  }
}


//----------------------------------------------------------------------------


void
Texture3DNode::leave( GLState &    /* _state */ , const DrawModes::DrawMode& /* _drawmode */  )
{
  ACG::GLState::bindTexture( GL_TEXTURE_3D, 0 );
}


//----------------------------------------------------------------------------


void
Texture3DNode::set_texture( unsigned int    _width,
			    unsigned int    _height,
			    unsigned int    _depth,
			    unsigned char * _data )
{
  glTexImage3D( GL_PROXY_TEXTURE_3D, 0, GL_RGBA,
 		_width, _height, _depth, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );

  GLint test_width;
  glGetTexLevelParameteriv( GL_PROXY_TEXTURE_3D, 0,
			    GL_TEXTURE_WIDTH, & test_width );

  GLint test_height;
  glGetTexLevelParameteriv( GL_PROXY_TEXTURE_3D, 0,
			    GL_TEXTURE_HEIGHT, & test_height );

  GLint test_depth;
  glGetTexLevelParameteriv( GL_PROXY_TEXTURE_3D, 0,
			    GL_TEXTURE_DEPTH, & test_depth );

  if ( ! test_width || ! test_height || ! test_depth ) 
  {
    std::cerr << "Can't load texture.\n";
    return;
  }

  glPixelStorei( GL_UNPACK_ROW_LENGTH,  0 );
  glPixelStorei( GL_UNPACK_SKIP_ROWS,   0 );
  glPixelStorei( GL_UNPACK_SKIP_PIXELS, 0 );
  glPixelStorei( GL_UNPACK_ALIGNMENT,   1 );
  glPixelStorei( GL_PACK_ROW_LENGTH,    0 );
  glPixelStorei( GL_PACK_SKIP_ROWS,     0 );
  glPixelStorei( GL_PACK_SKIP_PIXELS,   0 );
  glPixelStorei( GL_PACK_ALIGNMENT,     1 );    


  if ( glIsTexture( texture_ ) )
    glDeleteTextures( 1, & texture_ );

  glGenTextures( 1, & texture_ );
  ACG::GLState::bindTexture( GL_TEXTURE_3D, texture_ );

  glTexImage3D( GL_TEXTURE_3D, 0, GL_RGBA,
 		_width, _height, _depth, 0, GL_RGBA, GL_UNSIGNED_BYTE, _data );

  ACG::GLState::bindTexture( GL_TEXTURE_3D, 0 );

}

 
//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
