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

#include "CartesianClippingNode.hh"

//=============================================================================

namespace ACG {
namespace SceneGraph {

//=============================================================================


CartesianClippingNode::CartesianClippingNode( BaseNode*           _parent,
					      const std::string&  _name )
  : BaseNode( _parent, _name )
{
  set_cursor( Vec3f( 0, 0, 0 ) );
  enabled_ = NONE;
}

//-----------------------------------------------------------------------------


void
CartesianClippingNode::enter( GLState & _state, const DrawModes::DrawMode& /* _drawmode */ )
{
  Vec3d eye = _state.eye();

  if ( is_enabled( XY_PLANE ) )
  {
    GLdouble xy_plane[4];  
    if ( eye[2] > 0 )
    {  
      xy_plane[0] = 0;
      xy_plane[1] = 0;
      xy_plane[2] = -1;
      xy_plane[3] = cursor_[2];
    }
    else
    {
      xy_plane[0] = 0;
      xy_plane[1] = 0;
      xy_plane[2] = 1;
      xy_plane[3] = -cursor_[2];
    }
    
    glClipPlane( GL_CLIP_PLANE0, xy_plane );
    ACG::GLState::enable( GL_CLIP_PLANE0 );
  }

  if ( is_enabled( YZ_PLANE ) )
  {
    GLdouble yz_plane[4];  
    if ( eye[0] > 0 )
    {  
      yz_plane[0] = -1;
      yz_plane[1] = 0;
      yz_plane[2] = 0;
      yz_plane[3] = cursor_[0];
    }
    else
    {
      yz_plane[0] = 1;
      yz_plane[1] = 0;
      yz_plane[2] = 0;
      yz_plane[3] = -cursor_[0];
    }
    
    glClipPlane( GL_CLIP_PLANE1, yz_plane );
    ACG::GLState::enable( GL_CLIP_PLANE1 );
  }

  if ( is_enabled( XZ_PLANE ) )
  {
    GLdouble xz_plane[4];  
    if ( eye[1] > 0 )
    {  
      xz_plane[0] = 0;
      xz_plane[1] = -1;
      xz_plane[2] = 0;
      xz_plane[3] = cursor_[1];
    }
    else
    {
      xz_plane[0] = 0;
      xz_plane[1] = 1;
      xz_plane[2] = 0;
      xz_plane[3] = -cursor_[1];
    }
    
    glClipPlane( GL_CLIP_PLANE2, xz_plane );
    ACG::GLState::enable( GL_CLIP_PLANE2 );
  }


  
}


//-----------------------------------------------------------------------------


void
CartesianClippingNode::leave( GLState & /* _state */ , const DrawModes::DrawMode& /* _drawmode */ )
{
  ACG::GLState::disable( GL_CLIP_PLANE0 );
  ACG::GLState::disable( GL_CLIP_PLANE1 );
  ACG::GLState::disable( GL_CLIP_PLANE2 );
}


//-----------------------------------------------------------------------------


void
CartesianClippingNode::set_cursor( const Vec3f & _cursor )
{
  cursor_ = _cursor;
  
}


//-----------------------------------------------------------------------------


const
Vec3f &
CartesianClippingNode::cursor() const
{
  return cursor_;
}


//-----------------------------------------------------------------------------


void
CartesianClippingNode::set_enabled( Plane _plane ) 
{
  enabled_ = _plane;
}


//-----------------------------------------------------------------------------


bool
CartesianClippingNode::is_enabled( Plane _plane ) const
{
  return ( enabled_ == _plane );
}


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
