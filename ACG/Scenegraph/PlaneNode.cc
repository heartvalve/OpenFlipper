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
//   $Revision: 5600 $
//   $Author: wilden $
//   $Date: 2009-04-09 09:16:26 +0200 (Do, 09. Apr 2009) $
//
//=============================================================================

#include "PlaneNode.hh"
#include "../GL/gl.hh"
#include <iostream>
#include <math.h>

#include "TranslationManipulatorNode.hh"

namespace ACG {
namespace SceneGraph {


//== IMPLEMENTATION ==========================================================


void PlaneNode::boundingBox(Vec3f& /*_bbMin*/, Vec3f& /*_bbMax*/)
{
	//_bbMin.minimize( Vect3f  )
}

//----------------------------------------------------------------------------

unsigned int PlaneNode::availableDrawModes() const
{
  return ( DrawModes::POINTS |
	        DrawModes::POINTS_SHADED |
	        DrawModes::POINTS_COLORED );
}

//----------------------------------------------------------------------------

void PlaneNode::drawPlane( GLState&  /*_state*/) {

  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  Vec3f u1 = width_  * 0.5f * u_;
  Vec3f v1 = height_ * 0.5f * v_;
  Vec3f a = u1;
  Vec3f b = v1;
  Vec3f c = -u1;
  Vec3f d = -v1;

  //first draw the lines

  glColor3f( 1.0, 1.0, 1.0 );
  glLineWidth(2.0);

  //draw the plane
  glBegin(GL_QUADS);
    glVertex3fv( &a[0] );
    glVertex3fv( &b[0] );
    glVertex3fv( &c[0] );
    glVertex3fv( &d[0] );
  glEnd();


  glLineWidth(1.0);

  //then the red front side

  glEnable (GL_BLEND);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glPolygonMode(GL_FRONT, GL_FILL);

  glColor4f( 0.6, 0.15, 0.2, 0.5 );

  //draw the plane
  glBegin(GL_QUADS);
    glVertex3fv( &a[0] );
    glVertex3fv( &b[0] );
    glVertex3fv( &c[0] );
    glVertex3fv( &d[0] );
  glEnd();

  //finally the green back side

  glPolygonMode(GL_BACK, GL_FILL);

  glColor4f( 0.1, 0.8, 0.2, 0.5 );

  //draw the plane
  glBegin(GL_QUADS);
    glVertex3fv( &a[0] );
    glVertex3fv( &b[0] );
    glVertex3fv( &c[0] );
    glVertex3fv( &d[0] );
  glEnd();

}

//----------------------------------------------------------------

void PlaneNode::drawPlanePick( GLState&  _state) {

  _state.pick_set_maximum(1);
  _state.pick_set_name(0);

  Vec3f u1 = width_  * 0.5f * u_;
  Vec3f v1 = height_ * 0.5f * v_;
  Vec3f a = u1;
  Vec3f b = v1;
  Vec3f c = -u1;
  Vec3f d = -v1;

  //draw the plane
  glBegin(GL_QUADS);
    glVertex3fv( &a[0] );
    glVertex3fv( &b[0] );
    glVertex3fv( &c[0] );
    glVertex3fv( &d[0] );
  glEnd();
}

//----------------------------------------------------------------

void PlaneNode::draw(GLState&  _state  , unsigned int /*_drawMode*/)
{

  _state.push_modelview_matrix();
  glPushAttrib(GL_COLOR_BUFFER_BIT);
  glPushAttrib(GL_LIGHTING_BIT);

  glColorMaterial ( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE ) ;
  glEnable(GL_COLOR_MATERIAL);

  _state.translate(position_[0], position_[1], position_[2]);

  drawPlane(_state);

  glPopAttrib();
  glPopAttrib();
  _state.pop_modelview_matrix();
}

//----------------------------------------------------------------

void PlaneNode::setPosition(const Vec3f& _position, const Vec3f& _normal)
{
  //find a non zero component
  int comp = -1;
  for (int i=0; i < 2; i++)
    if ( _normal[i] != 0.0 ){
      comp = i;
      break;
    }

  if (comp == -1){
    std::cerr << "PlaneNode: normal is invalid!" << std::endl;
    return;
  }

  //compute orthogonal vectors in the plane
  u_[comp] = (-_normal[ (comp + 1) % 3 ] - _normal[(comp + 2) % 3]) / _normal[comp];
  u_[ (comp + 1) % 3 ] = 1;
  u_[ (comp + 2) % 3 ] = 1;
  u_ = u_.normalize();

  v_ = u_ % _normal;
  v_ = v_.normalize();

  position_ = _position;
  normal_   = _normal;
}

//----------------------------------------------------------------

void PlaneNode::setSize(double _width, double _height)
{
  width_ = _width;
  height_ = _height;
}

//----------------------------------------------------------------

void
PlaneNode::pick(GLState& _state, PickTarget _target)
{
  if (_target == PICK_ANYTHING) {

	  _state.push_modelview_matrix();
	  
	  _state.translate(position_[0], position_[1], position_[2]);
	  
	  drawPlanePick(_state);
	  
	  _state.pop_modelview_matrix();
  }
}

//----------------------------------------------------------------

Vec3f PlaneNode::position()
{
  TranslationManipulatorNode* manipNode = dynamic_cast< TranslationManipulatorNode* > (parent());

  if (manipNode)
    return manipNode->matrix().transform_point( position_ );
  else
    return position_;
}

//----------------------------------------------------------------

Vec3f PlaneNode::normal()
{
  TranslationManipulatorNode* manipNode = dynamic_cast< TranslationManipulatorNode* > (parent());

  if (manipNode)
    return manipNode->matrix().transform_vector( normal_ );
  else
    return normal_;
}

//----------------------------------------------------------------

int PlaneNode::objectID()
{
  return objectID_;
}

//----------------------------------------------------------------

void PlaneNode::objectID(int _id)
{
  objectID_ = _id;
}

//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
