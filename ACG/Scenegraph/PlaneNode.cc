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
 *   $Revision$                                                         *
 *   $Author$                                                      *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/

#include "PlaneNode.hh"
#include "../GL/gl.hh"
#include <iostream>
#include <math.h>

#include "TranslationManipulatorNode.hh"

namespace ACG {
namespace SceneGraph {


//== IMPLEMENTATION ==========================================================


void PlaneNode::boundingBox(Vec3d& _bbMin, Vec3d& _bbMax)
{

  Vec3f pos = position_ - xDirection_*0.5 - yDirection_*0.5;

  //add a little offset in normal direction
  Vec3d pos0 = ACG::Vec3d( pos + normal_*0.1 );
  Vec3d pos1 = ACG::Vec3d( pos - normal_*0.1 );
  
  Vec3d xDird = ACG::Vec3d( xDirection_ );
  Vec3d yDird = ACG::Vec3d( yDirection_ );

  _bbMin.minimize( pos0 );
  _bbMin.minimize( pos0 + xDird);
  _bbMin.minimize( pos0 + yDird);
  _bbMin.minimize( pos0 + xDird + yDird);
  _bbMax.maximize( pos1 );
  _bbMax.maximize( pos1 + xDird);
  _bbMax.maximize( pos1 + yDird);
  _bbMax.maximize( pos1 + xDird + yDird);
}

//----------------------------------------------------------------------------

DrawModes::DrawMode
PlaneNode::availableDrawModes() const
{
  return ( DrawModes::POINTS |
	        DrawModes::POINTS_SHADED |
	        DrawModes::POINTS_COLORED );
}

//----------------------------------------------------------------------------

void PlaneNode::drawPlane( GLState&  /*_state*/) {

  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  Vec3f origin(0.0, 0.0, 0.0);
  Vec3f xy = xDirection_ + yDirection_;

  //first draw the lines
  glColor3f( 1.0, 1.0, 1.0 );
  glLineWidth(2.0);

  //draw the plane
  glBegin(GL_QUADS);
    glVertex3fv( &origin[0] );
    glVertex3fv( &xDirection_[0] );
    glVertex3fv( &xy[0] );
    glVertex3fv( &yDirection_[0] );
  glEnd();


  glLineWidth(1.0);

  //then the red front side

  glEnable (GL_BLEND);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glPolygonMode(GL_FRONT, GL_FILL);

  glColor4f( 0.6, 0.15, 0.2, 0.5 );

  //draw the plane
  glBegin(GL_QUADS);
    glVertex3fv( &origin[0] );
    glVertex3fv( &xDirection_[0] );
    glVertex3fv( &xy[0] );
    glVertex3fv( &yDirection_[0] );
  glEnd();

  //finally the green back side

  glPolygonMode(GL_BACK, GL_FILL);

  glColor4f( 0.1, 0.8, 0.2, 0.5 );

  //draw the plane
  glBegin(GL_QUADS);
    glVertex3fv( &origin[0] );
    glVertex3fv( &xDirection_[0] );
    glVertex3fv( &xy[0] );
    glVertex3fv( &yDirection_[0] );
  glEnd();

}

//----------------------------------------------------------------

void PlaneNode::drawPlanePick( GLState&  _state) {

  _state.pick_set_maximum(1);
  _state.pick_set_name(0);

  Vec3f origin(0.0, 0.0, 0.0);
  Vec3f xy = xDirection_ + yDirection_;

  //draw the plane
  glBegin(GL_QUADS);
    glVertex3fv( &origin[0] );
    glVertex3fv( &xDirection_[0] );
    glVertex3fv( &xy[0] );
    glVertex3fv( &yDirection_[0] );
  glEnd();
}

//----------------------------------------------------------------

void PlaneNode::draw(GLState&  _state  , DrawModes::DrawMode /*_drawMode*/)
{

  _state.push_modelview_matrix();
  glPushAttrib(GL_COLOR_BUFFER_BIT);
  glPushAttrib(GL_LIGHTING_BIT);

  glColorMaterial ( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE ) ;
  glEnable(GL_COLOR_MATERIAL);

  Vec3f pos = position_ - xDirection_*0.5 - yDirection_*0.5;

  _state.translate(pos[0], pos[1], pos[2]);

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
  xDirection_[comp] = (-_normal[ (comp + 1) % 3 ] - _normal[(comp + 2) % 3]) / _normal[comp];
  xDirection_[ (comp + 1) % 3 ] = 1;
  xDirection_[ (comp + 2) % 3 ] = 1;
  xDirection_ = xDirection_.normalize();

  yDirection_ = _normal % xDirection_;
  yDirection_ = yDirection_.normalize();

  position_ = _position;
  normal_   = _normal;
}

//----------------------------------------------------------------


void PlaneNode::setPosition(const Vec3f& _position, const Vec3f& _xDirection, const Vec3f& _yDirection)
{
  position_ = _position;
  xDirection_ = _xDirection;
  yDirection_ = _yDirection;
  normal_ = (_xDirection % _yDirection).normalize();
}

//----------------------------------------------------------------

void PlaneNode::transform(const ACG::Matrix4x4d& _mat)
{
  position_ = _mat.transform_point(position_);
  xDirection_ = _mat.transform_vector(xDirection_);
  yDirection_ = _mat.transform_vector(yDirection_);

  normal_ = (xDirection_ % yDirection_).normalize();
}

//----------------------------------------------------------------

void PlaneNode::setSize(double _xDirection, double _yDirection)
{
  xDirection_ = xDirection_.normalize() * _xDirection;
  yDirection_ = yDirection_.normalize() * _yDirection;
}

//----------------------------------------------------------------

void
PlaneNode::pick(GLState& _state, PickTarget _target)
{
  if (_target == PICK_ANYTHING) {

	  _state.push_modelview_matrix();
	  
    Vec3f pos = position_ - xDirection_*0.5 - yDirection_*0.5;

	  _state.translate(pos[0], pos[1], pos[2]);
	  
	  drawPlanePick(_state);
	  
	  _state.pop_modelview_matrix();
  }
}

//----------------------------------------------------------------

Vec3f PlaneNode::position()
{
    return position_;
}

//----------------------------------------------------------------

Vec3f PlaneNode::normal()
{
    return normal_;
}

//----------------------------------------------------------------

Vec3f PlaneNode::xDirection()
{
  return xDirection_;
}

//----------------------------------------------------------------

Vec3f PlaneNode::yDirection()
{
  return yDirection_;
}

//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
