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
 *   $Revision$                                                         *
 *   $Author$                                                      *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/

#include "PlaneNode.hh"
#include <ACG/GL/gl.hh>
#include <iostream>
#include <math.h>

//== IMPLEMENTATION ==========================================================


PlaneNode::PlaneNode(Plane& _plane, BaseNode *_parent, std::string _name)
:BaseNode(_parent, _name),
 plane_(_plane)
{
}

PlaneNode::~PlaneNode()
{
}

void PlaneNode::boundingBox(ACG::Vec3d& _bbMin, ACG::Vec3d& _bbMax)
{

  ACG::Vec3d pos = plane_.position - plane_.xDirection * 0.5 - plane_.yDirection * 0.5;

  //add a little offset in normal direction
  ACG::Vec3d pos0 = ACG::Vec3d( pos + plane_.normal * 0.1 );
  ACG::Vec3d pos1 = ACG::Vec3d( pos - plane_.normal * 0.1 );
  
  ACG::Vec3d xDird = ACG::Vec3d( plane_.xDirection );
  ACG::Vec3d yDird = ACG::Vec3d( plane_.yDirection );

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

ACG::SceneGraph::DrawModes::DrawMode
PlaneNode::availableDrawModes() const
{
  return ( ACG::SceneGraph::DrawModes::POINTS |
           ACG::SceneGraph::DrawModes::POINTS_SHADED |
           ACG::SceneGraph::DrawModes::POINTS_COLORED );
}

//----------------------------------------------------------------------------

void PlaneNode::drawPlane( ACG::GLState&  /*_state*/) {

  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  ACG::Vec3d origin(0.0, 0.0, 0.0);
  ACG::Vec3d xy = plane_.xDirection + plane_.yDirection;

  //first draw the lines
  glColor3f( 1.0, 1.0, 1.0 );
  glLineWidth(2.0);

  //draw the plane
  glBegin(GL_QUADS);
    glVertex3dv( &origin[0] );
    glVertex3dv( &plane_.xDirection[0] );
    glVertex3dv( &xy[0] );
    glVertex3dv( &plane_.yDirection[0] );
  glEnd();


  glLineWidth(1.0);

  //then the red front side

  ACG::GLState::enable (GL_BLEND);
  ACG::GLState::blendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glPolygonMode(GL_FRONT, GL_FILL);

  glColor4f( 0.6, 0.15, 0.2, 0.5 );

  //draw the plane
  glBegin(GL_QUADS);
    glVertex3dv( &origin[0] );
    glVertex3dv( &plane_.xDirection[0] );
    glVertex3dv( &xy[0] );
    glVertex3dv( &plane_.yDirection[0] );
  glEnd();

  //finally the green back side

  glPolygonMode(GL_BACK, GL_FILL);

  glColor4f( 0.1, 0.8, 0.2, 0.5 );

  //draw the plane
  glBegin(GL_QUADS);
    glVertex3dv( &origin[0] );
    glVertex3dv( &plane_.xDirection[0] );
    glVertex3dv( &xy[0] );
    glVertex3dv( &plane_.yDirection[0] );
  glEnd();

}

//----------------------------------------------------------------

void PlaneNode::drawPlanePick( ACG::GLState&  _state) {

  _state.pick_set_maximum(1);
  _state.pick_set_name(0);

  ACG::Vec3d origin(0.0, 0.0, 0.0);
  ACG::Vec3d xy = plane_.xDirection + plane_.yDirection;

  //draw the plane
  glBegin(GL_QUADS);
    glVertex3dv( &origin[0] );
    glVertex3dv( &plane_.xDirection[0] );
    glVertex3dv( &xy[0] );
    glVertex3dv( &plane_.yDirection[0] );
  glEnd();
}

//----------------------------------------------------------------

void PlaneNode::draw(ACG::GLState&  _state  , const ACG::SceneGraph::DrawModes::DrawMode& /*_drawMode*/)
{

  _state.push_modelview_matrix();
  glPushAttrib(GL_COLOR_BUFFER_BIT);
  glPushAttrib(GL_LIGHTING_BIT);

  glColorMaterial ( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE ) ;
  ACG::GLState::enable(GL_COLOR_MATERIAL);

  ACG::Vec3d pos = plane_.position - plane_.xDirection*0.5 - plane_.yDirection*0.5;

  _state.translate(pos[0], pos[1], pos[2]);

  drawPlane(_state);

  glPopAttrib();
  glPopAttrib();
  _state.pop_modelview_matrix();
}


//----------------------------------------------------------------

void
PlaneNode::pick(ACG::GLState& _state, ACG::SceneGraph::PickTarget _target)
{
  if (_target == ACG::SceneGraph::PICK_ANYTHING) {

	  _state.push_modelview_matrix();
	  
	  ACG::Vec3d pos = plane_.position - plane_.xDirection*0.5 - plane_.yDirection*0.5;

	  _state.translate(pos[0], pos[1], pos[2]);
	  
	  drawPlanePick(_state);
	  
	  _state.pop_modelview_matrix();
  }
}

//----------------------------------------------------------------

ACG::Vec3d PlaneNode::position()
{
    return plane_.position;
}

//----------------------------------------------------------------

ACG::Vec3d PlaneNode::normal()
{
    return plane_.normal;
}

//----------------------------------------------------------------

ACG::Vec3d PlaneNode::xDirection()
{
  return plane_.xDirection;
}

//----------------------------------------------------------------

ACG::Vec3d PlaneNode::yDirection()
{
  return plane_.yDirection;
}

//----------------------------------------------------------------

Plane& PlaneNode::getPlane()
{
    return plane_;
}

//----------------------------------------------------------------

void PlaneNode::setPlane(Plane plane)
{
    plane_ = plane;
}


//=============================================================================
