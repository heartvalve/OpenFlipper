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

#include "SliceNode.hh"

//=============================================================================

namespace ACG {
namespace SceneGraph {

//=============================================================================


SliceNode::SliceNode( BaseNode    * _parent,
		      std::string   _name   ) :
  BaseNode( _parent, _name )
{
  set_enabled( NONE );

  set_visible_box( Vec3f( 0, 0, 0 ),
		   Vec3f( 1, 1, 1 ) );

  set_texture_box( Vec3f( 0, 0, 0 ),
		   Vec3f( 1, 1, 1 ) );

  set_cursor( Vec3f( 0.5, 0.5, 0.5 ) );

  view_frame( false );
}


//----------------------------------------------------------------------------


SliceNode::~SliceNode()
{
}


//----------------------------------------------------------------------------


void
SliceNode::view_frame( bool _view_frame )
{
  view_frame_ = _view_frame;
}


//----------------------------------------------------------------------------


void
SliceNode::boundingBox( Vec3f & _bbMin, Vec3f & _bbMax )
{
  _bbMin.minimize( visible_min_ );
  _bbMax.maximize( visible_max_ );
}


//----------------------------------------------------------------------------

  
unsigned int 
SliceNode::availableDrawModes() const
{
  return 0;
  return ( DrawModes::POINTS              |
	   DrawModes::WIREFRAME           |
	   DrawModes::HIDDENLINE          |
	   DrawModes::SOLID_FLAT_SHADED   |
	   DrawModes::SOLID_SMOOTH_SHADED );
}


//----------------------------------------------------------------------------


void
SliceNode::draw( GLState & /* _state */ , unsigned int /* _drawMode */ )
{
  if ( view_frame_ )
    draw_frame();

  if ( is_enabled( NONE ) )
    return;

  glEnable( GL_LIGHTING );
  glLightModeli( GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE );

  glEnable( GL_TEXTURE_3D );    

  glEnable( GL_BLEND );
  glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

  draw_planes();

  glDisable( GL_BLEND );
  glDisable( GL_TEXTURE_3D );    
  glDisable( GL_LIGHTING );

}


//----------------------------------------------------------------------------


void
SliceNode::draw_frame() const
{
  glDisable( GL_BLEND );
  glDisable( GL_TEXTURE_3D );    
  glDisable( GL_LIGHTING );

  glBegin( GL_LINES );

  glColor3f( 1, 1, 1 );

  glVertex3f( visible_min_[0], visible_min_[1], visible_min_[2] );
  glVertex3f( visible_min_[0], visible_min_[1], visible_max_[2] );
  glVertex3f( visible_min_[0], visible_max_[1], visible_min_[2] );
  glVertex3f( visible_min_[0], visible_max_[1], visible_max_[2] );
  glVertex3f( visible_max_[0], visible_max_[1], visible_min_[2] );
  glVertex3f( visible_max_[0], visible_max_[1], visible_max_[2] );
  glVertex3f( visible_max_[0], visible_min_[1], visible_min_[2] );
  glVertex3f( visible_max_[0], visible_min_[1], visible_max_[2] );

  glVertex3f( visible_min_[0], visible_min_[1], visible_min_[2] );
  glVertex3f( visible_min_[0], visible_max_[1], visible_min_[2] );
  glVertex3f( visible_min_[0], visible_min_[1], visible_max_[2] );
  glVertex3f( visible_min_[0], visible_max_[1], visible_max_[2] );
  glVertex3f( visible_max_[0], visible_min_[1], visible_min_[2] );
  glVertex3f( visible_max_[0], visible_max_[1], visible_min_[2] );
  glVertex3f( visible_max_[0], visible_min_[1], visible_max_[2] );
  glVertex3f( visible_max_[0], visible_max_[1], visible_max_[2] );

  glVertex3f( visible_min_[0], visible_min_[1], visible_min_[2] );
  glVertex3f( visible_max_[0], visible_min_[1], visible_min_[2] );
  glVertex3f( visible_min_[0], visible_min_[1], visible_max_[2] );
  glVertex3f( visible_max_[0], visible_min_[1], visible_max_[2] );
  glVertex3f( visible_min_[0], visible_max_[1], visible_min_[2] );
  glVertex3f( visible_max_[0], visible_max_[1], visible_min_[2] );
  glVertex3f( visible_min_[0], visible_max_[1], visible_max_[2] );
  glVertex3f( visible_max_[0], visible_max_[1], visible_max_[2] );

  glEnd();


  /*  
  glBegin( GL_LINES );

  glVertex3f( bbmin_[0], cursor_[1], cursor_[2] );
  glVertex3f( bbmax_[0], cursor_[1], cursor_[2] );

  glVertex3f( cursor_[0], bbmin_[1], cursor_[2] );
  glVertex3f( cursor_[0], bbmax_[1], cursor_[2] );

  glVertex3f( cursor_[0], cursor_[1], bbmin_[2] );
  glVertex3f( cursor_[0], cursor_[1], bbmax_[2] );

  glEnd();
  */

  glBegin( GL_LINES );
  
  glColor3f( 1, 1, 1 );

  if ( is_enabled( XY_PLANE ) )
  {
    glVertex3f( visible_min_[0], visible_min_[1], cursor_[2] );
    glVertex3f( visible_max_[0], visible_min_[1], cursor_[2] );
    glVertex3f( visible_max_[0], visible_min_[1], cursor_[2] );
    glVertex3f( visible_max_[0], visible_max_[1], cursor_[2] );
    glVertex3f( visible_max_[0], visible_max_[1], cursor_[2] );
    glVertex3f( visible_min_[0], visible_max_[1], cursor_[2] );
    glVertex3f( visible_min_[0], visible_max_[1], cursor_[2] );
    glVertex3f( visible_min_[0], visible_min_[1], cursor_[2] );
  }


  if ( is_enabled( YZ_PLANE ) )
  {
    glVertex3f( cursor_[0], visible_min_[1], visible_min_[2] );
    glVertex3f( cursor_[0], visible_min_[1], visible_max_[2] );
    glVertex3f( cursor_[0], visible_min_[1], visible_max_[2] );
    glVertex3f( cursor_[0], visible_max_[1], visible_max_[2] );
    glVertex3f( cursor_[0], visible_max_[1], visible_max_[2] );
    glVertex3f( cursor_[0], visible_max_[1], visible_min_[2] );
    glVertex3f( cursor_[0], visible_max_[1], visible_min_[2] );
    glVertex3f( cursor_[0], visible_min_[1], visible_min_[2] );
  }


  if ( is_enabled( XZ_PLANE ) )
  {
    glVertex3f( visible_min_[0], cursor_[1], visible_min_[2] );
    glVertex3f( visible_max_[0], cursor_[1], visible_min_[2] );
    glVertex3f( visible_max_[0], cursor_[1], visible_min_[2] );
    glVertex3f( visible_max_[0], cursor_[1], visible_max_[2] );
    glVertex3f( visible_max_[0], cursor_[1], visible_max_[2] );
    glVertex3f( visible_min_[0], cursor_[1], visible_max_[2] );
    glVertex3f( visible_min_[0], cursor_[1], visible_max_[2] );
    glVertex3f( visible_min_[0], cursor_[1], visible_min_[2] );
  }

  glEnd();
}


//----------------------------------------------------------------------------


void
SliceNode::draw_planes() const
{
  glShadeModel( GL_FLAT );


  glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

  glDepthRange(0.01, 1.0);

  Vec3f rel;
  Vec3f tcmin;
  Vec3f tcmax;

  for ( int i = 0; i < 3; ++i )
  {
    rel[i] = ( cursor_[i]-texture_min_[i] ) / ( texture_max_[i] - texture_min_[i] );
    tcmin[i] = ( visible_min_[i] - texture_min_[i] )
	     / ( texture_max_[i] - texture_min_[i] );
    tcmax[i] = ( visible_max_[i] - texture_min_[i] )
	     / ( texture_max_[i] - texture_min_[i] );
  }

  if ( cursor_[2] >= visible_min_[2] &&
       cursor_[2] <= visible_max_[2] &&
       is_enabled( XY_PLANE ) )
  {
    glBegin( GL_QUADS );
    
    glNormal3f( 0, 0, 1 );

    glTexCoord3f( tcmin[0], tcmin[1], rel[2] );
    glVertex3f( visible_min_[0], visible_min_[1], cursor_[2] );
    
    glTexCoord3f( tcmax[0], tcmin[1], rel[2] );
    glVertex3f( visible_max_[0], visible_min_[1], cursor_[2] );
    
    glTexCoord3f( tcmax[0], tcmax[1], rel[2] );
    glVertex3f( visible_max_[0], visible_max_[1], cursor_[2] );
    
    glTexCoord3f( tcmin[0], tcmax[1], rel[2] );
    glVertex3f( visible_min_[0], visible_max_[1], cursor_[2] );
    
    glEnd();
  }
  
  if ( cursor_[0] >= visible_min_[0] &&
       cursor_[0] <= visible_max_[0] &&
       is_enabled( YZ_PLANE ) )
  {
    glBegin( GL_QUADS );
    
    glNormal3f( -1, 0, 0 );

    glTexCoord3f( rel[0], tcmin[1], tcmin[2] );
    glVertex3f( cursor_[0], visible_min_[1], visible_min_[2] );

    glTexCoord3f( rel[0], tcmin[1], tcmax[2] );
    glVertex3f( cursor_[0], visible_min_[1], visible_max_[2] );
      
    glTexCoord3f( rel[0], tcmax[1], tcmax[2] );
    glVertex3f( cursor_[0], visible_max_[1], visible_max_[2] );
    
    glTexCoord3f( rel[0], tcmax[1], tcmin[2] );
    glVertex3f( cursor_[0], visible_max_[1], visible_min_[2] );
    
    glEnd();
  }

  if ( cursor_[1] >= visible_min_[1] &&
       cursor_[1] <= visible_max_[1] &&
       is_enabled( XZ_PLANE ) )
  {
    glBegin( GL_QUADS );
    
    glNormal3f( 0, -1, 0 );
    
    glTexCoord3f( tcmin[0], rel[1], tcmin[2] );
    glVertex3f( visible_min_[0], cursor_[1], visible_min_[2] );
    
    glTexCoord3f( tcmax[0], rel[1], tcmin[2] );
    glVertex3f( visible_max_[0], cursor_[1], visible_min_[2] );
    
    glTexCoord3f( tcmax[0], rel[1], tcmax[2] );
    glVertex3f( visible_max_[0], cursor_[1], visible_max_[2] );
    
    glTexCoord3f( tcmin[0], rel[1], tcmax[2] );
    glVertex3f( visible_min_[0], cursor_[1], visible_max_[2] );

    glEnd();
  }
  

  glDepthRange(0.0, 1.0);
}


//----------------------------------------------------------------------------


// void
// SliceNode::draw_planes() const
// {
//   glShadeModel( GL_FLAT );
//   glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

//   glDepthRange(0.01, 1.0);

//   Vec3f rel;
//   Vec3f tcmin;
//   Vec3f tcmax;

//   for ( int i = 0; i < 3; ++i )
//   {
//     rel[i] = ( cursor_[i]-texture_min_[i] ) / ( texture_max_[i] - texture_min_[i] );
//     tcmin[i] = ( visible_min_[i] - texture_min_[i] )
// 	     / ( texture_max_[i] - texture_min_[i] );
//     tcmax[i] = ( visible_max_[i] - texture_min_[i] )
// 	     / ( texture_max_[i] - texture_min_[i] );
//   }

//   if ( cursor_[2] >= visible_min_[2] &&
//        cursor_[2] <= visible_max_[2] &&
//        is_enabled( XY_PLANE ) )
//   {
//     glBegin( GL_QUADS );
    
//     glNormal3f( 0, 0, 1 );

//     glTexCoord3f( tcmin[0], tcmin[1], rel[2] );
//     glVertex3f( visible_min_[0], visible_min_[1], cursor_[2] );
    
//     glTexCoord3f( tcmax[0], tcmin[1], rel[2] );
//     glVertex3f( visible_max_[0], visible_min_[1], cursor_[2] );
    
//     glTexCoord3f( tcmax[0], tcmax[1], rel[2] );
//     glVertex3f( visible_max_[0], visible_max_[1], cursor_[2] );
    
//     glTexCoord3f( tcmin[0], tcmax[1], rel[2] );
//     glVertex3f( visible_min_[0], visible_max_[1], cursor_[2] );
    
//     glEnd();
//   }
  
//   if ( cursor_[0] >= visible_min_[0] &&
//        cursor_[0] <= visible_max_[0] &&
//        is_enabled( YZ_PLANE ) )
//   {
//     glBegin( GL_QUADS );
    
//     glNormal3f( -1, 0, 0 );

//     glTexCoord3f( rel[0], tcmin[1], tcmin[2] );
//     glVertex3f( cursor_[0], visible_min_[1], visible_min_[2] );

//     glTexCoord3f( rel[0], tcmin[1], tcmax[2] );
//     glVertex3f( cursor_[0], visible_min_[1], visible_max_[2] );
      
//     glTexCoord3f( rel[0], tcmax[1], tcmax[2] );
//     glVertex3f( cursor_[0], visible_max_[1], visible_max_[2] );
    
//     glTexCoord3f( rel[0], tcmax[1], tcmin[2] );
//     glVertex3f( cursor_[0], visible_max_[1], visible_min_[2] );
    
//     glEnd();
//   }

//   if ( cursor_[1] >= visible_min_[1] &&
//        cursor_[1] <= visible_max_[1] &&
//        is_enabled( XZ_PLANE ) )
//   {
//     glBegin( GL_QUADS );
    
//     glNormal3f( 0, -1, 0 );

//     glTexCoord3f( tcmin[0], rel[1], tcmin[2] );
//     glVertex3f( visible_min_[0], cursor_[1], visible_min_[2] );
      
//     glTexCoord3f( tcmax[0], rel[1], tcmin[2] );
//     glVertex3f( visible_max_[0], cursor_[1], visible_min_[2] );
      
//     glTexCoord3f( tcmax[0], rel[1], tcmax[2] );
//     glVertex3f( visible_max_[0], cursor_[1], visible_max_[2] );
      
//     glTexCoord3f( tcmin[0], rel[1], tcmax[2] );
//     glVertex3f( visible_min_[0], cursor_[1], visible_max_[2] );
    
//     glEnd();
//   }

//   glDepthRange(0.0, 1.0);
// }


//----------------------------------------------------------------------------


ACG::Vec3f
SliceNode::cursor() const
{
  return cursor_;
}


//----------------------------------------------------------------------------


void
SliceNode::set_cursor( const Vec3f & _cursor )
{
  cursor_ = _cursor;
}


//----------------------------------------------------------------------------


bool
SliceNode::is_enabled( Plane _plane ) const
{
  return enabled_ == _plane;
}


//----------------------------------------------------------------------------


void
SliceNode::set_enabled( Plane _plane )
{
  enabled_ = _plane;
}


//----------------------------------------------------------------------------


void
SliceNode::set_visible_box( const Vec3f & _box_min,
			    const Vec3f & _box_max )
{
  visible_min_ = _box_min;
  visible_max_ = _box_max;
}


//----------------------------------------------------------------------------


void
SliceNode::set_texture_box( const Vec3f & _box_min,
			    const Vec3f & _box_max )
{
  texture_min_ = _box_min;
  texture_max_ = _box_max;
}


//----------------------------------------------------------------------------


void
SliceNode::pick( GLState & _state, PickTarget /* _target */  )
{
//   if ( _target == PICK_ANYTHING )
//     return;

  _state.pick_set_maximum (1);
  _state.pick_set_name (0);
  glDisable(GL_LIGHTING);
  glPushMatrix();
  draw_planes();
  glPopMatrix();
}


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
