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
//  CLASS Texture3DNode
//
//=============================================================================

#ifndef ACG_TEXTURE3DNODE_HH
#define ACG_TEXTURE3DNODE_HH

//=============================================================================

#include "BaseNode.hh"

#include <string>
#include <qimage.h>


//=============================================================================

namespace ACG {
namespace SceneGraph {

//=============================================================================


class ACGDLLEXPORT Texture3DNode : public BaseNode
{
public:

  Texture3DNode( BaseNode*           _parent = 0,
		 const std::string&  _name = "<Texture3DNode>" );
  virtual ~Texture3DNode();

  /// set class name
  ACG_CLASSNAME( Texture3DNode );

  void set_texture( unsigned int    _width,
		    unsigned int    _height,
		    unsigned int    _depth,
		    unsigned char * _data );

  /// set texture
  void enter( GLState& _state, unsigned int _drawmode );

  /// restores original texture (or no-texture)
  void leave( GLState& _state, unsigned int _drawmode );


  void set_texture_mode( GLenum _mode )
  { tex_mode_ = _mode; }


  void set_border_color( const Vec4f & _border_color )
  {
    border_color_ = _border_color;
  }

  void set_wrap_mode( GLenum _wrap_mode )
  {
    // possible modes are:
    // GL_CLAMP
    // GL_CLAMP_TO_EDGE
    // GL_CLAMP_TO_BORDER
    // GL_REPEAT

    wrap_mode_ = _wrap_mode;
  }

  void set_filter( GLenum _filter )
  {
    // possible modes are:
    // GL_NEAREST
    // GL_LINEAR

    filter_ = _filter;
  }


private:

  GLuint  texture_;
  GLenum  tex_mode_;
  Vec4f   border_color_;
  GLenum  wrap_mode_;
  GLenum  filter_;


};


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
#endif // ACG_TEXTURE3DNODE_HH defined
//=============================================================================

