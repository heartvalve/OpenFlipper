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
#include <ACG/Scenegraph/MaterialNode.hh>

//== IMPLEMENTATION ==========================================================


PlaneNode::PlaneNode(Plane& _plane, BaseNode *_parent, std::string _name)
:BaseNode(_parent, _name),
 plane_(_plane),
 vbo_(0)
{
  vertexDecl_.addElement(GL_FLOAT, 3, ACG::VERTEX_USAGE_POSITION);
}

PlaneNode::~PlaneNode()
{
  if ( vbo_)
    glDeleteBuffers(1,&vbo_);

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



//----------------------------------------------------------------------------

void PlaneNode::drawPlane( ACG::GLState&  _state) {

  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  const ACG::Vec3d xy = plane_.xDirection + plane_.yDirection;

  // Array of coordinates for the plane
  float vboData_[4* 3 ] = { 0.0,0.0,0.0,
                            (float)plane_.xDirection[0],(float)plane_.xDirection[1],(float)plane_.xDirection[2],
                            (float)xy[0],(float)xy[1],(float)xy[2],
                            (float)plane_.yDirection[0],(float)plane_.yDirection[1],(float)plane_.yDirection[2] };

   // Enable the arrays
  _state.enableClientState(GL_VERTEX_ARRAY);
  _state.vertexPointer(3,GL_FLOAT,0,&vboData_[0]);

  //first draw the lines
  glColor3f( 1.0, 1.0, 1.0 );
  glLineWidth(2.0);

  glDrawArrays(GL_QUADS,0,4);

  glLineWidth(1.0);

  //then the red front side
  ACG::GLState::enable (GL_BLEND);
  ACG::GLState::blendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glPolygonMode(GL_FRONT, GL_FILL);
  glColor4f( 0.6, 0.15, 0.2, 0.5 );

  glDrawArrays(GL_QUADS,0,4);


  //finally the green back side
  glPolygonMode(GL_BACK, GL_FILL);
  glColor4f( 0.1, 0.8, 0.2, 0.5 );

  glDrawArrays(GL_QUADS,0,4);

  // deactivate vertex arrays after drawing
  _state.disableClientState(GL_VERTEX_ARRAY);

}

//----------------------------------------------------------------

void PlaneNode::drawPlanePick( ACG::GLState&  _state) {

  _state.pick_set_maximum(1);
  _state.pick_set_name(0);

  const ACG::Vec3d xy = plane_.xDirection + plane_.yDirection;

  // Array of coordinates for the plane
  float vboData_[4* 3 ] = { 0.0,0.0,0.0,
                            (float)plane_.xDirection[0],(float)plane_.xDirection[1],(float)plane_.xDirection[2],
                            (float)xy[0],(float)xy[1],(float)xy[2],
                            (float)plane_.yDirection[0],(float)plane_.yDirection[1],(float)plane_.yDirection[2] };

   // Enable the arrays
  _state.enableClientState(GL_VERTEX_ARRAY);
  _state.vertexPointer(3,GL_FLOAT,0,&vboData_[0]);

  glDrawArrays(GL_QUADS,0,4);

  // deactivate vertex arrays after drawing
  _state.disableClientState(GL_VERTEX_ARRAY);

}

//----------------------------------------------------------------

void PlaneNode::draw(ACG::GLState&  _state  , const ACG::SceneGraph::DrawModes::DrawMode& /*_drawMode*/)
{

  _state.push_modelview_matrix();
  glPushAttrib(GL_COLOR_BUFFER_BIT);
  glPushAttrib(GL_LIGHTING_BIT);

  glColorMaterial ( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE ) ;
  ACG::GLState::enable(GL_COLOR_MATERIAL);

  // plane_.position represents the center of the plane.
  // Compute the corner position
  ACG::Vec3d pos = plane_.position - plane_.xDirection*0.5 - plane_.yDirection*0.5;

  // translate to corner position
  _state.translate(pos[0], pos[1], pos[2]);

  // draw the plane
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

//----------------------------------------------------------------------------

void
PlaneNode::
getRenderObjects(ACG::IRenderer* _renderer, ACG::GLState&  _state , const ACG::SceneGraph::DrawModes::DrawMode&  _drawMode , const ACG::SceneGraph::Material* _mat) {

//  // init base render object
//  ACG::RenderObject ro;
//  memset(&ro, 0, sizeof(ACG::RenderObject));
//
//_state.enable(GL_COLOR_MATERIAL);
//_state.disable(GL_LIGHTING);
//  ro.initFromState(&_state);
//
//
////   glColorMaterial ( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE ) ;
////   ACG::GLState::enable(GL_COLOR_MATERIAL);
//
//  // plane_.position represents the center of the plane.
//  // Compute the corner position
//  const ACG::Vec3d pos = plane_.position - plane_.xDirection*0.5 - plane_.yDirection*0.5;
//
//  // translate to corner position and store that in renderer
//  _state.push_modelview_matrix();
//  _state.translate(pos[0], pos[1], pos[2]);
//  ro.modelview = _state.modelview();
//  _state.pop_modelview_matrix();
//
//
//  // Set the local material
//  ACG::SceneGraph::Material localMaterial = *_mat;
//  localMaterial.baseColor(ACG::Vec4f(1.0,0.0,0.0,1.0));
//  localMaterial.ambientColor(ACG::Vec4f(1.0,0.0,0.0,1.0));
//  localMaterial.diffuseColor(ACG::Vec4f(1.0,0.0,0.0,1.0));
//  localMaterial.specularColor(ACG::Vec4f(1.0,0.0,0.0,1.0));
////  localMaterial.disableBackfaceCulling();
////  localMaterial.enableColorMaterial();
//
//  ro.setMaterial(_mat);
//
//
//  // draw the plane
//
//  const ACG::Vec3d xy = plane_.xDirection + plane_.yDirection;
//
//  // Array of coordinates for the plane ( duplicated due to front and back rendering )
//  float vboData_[8 * 3 ] = { 0.0,0.0,0.0,
//                            (float)plane_.xDirection[0],(float)plane_.xDirection[1],(float)plane_.xDirection[2],
//                            (float)xy[0],(float)xy[1],(float)xy[2],
//                            (float)plane_.yDirection[0],(float)plane_.yDirection[1],(float)plane_.yDirection[2],
//                            (float)plane_.yDirection[0],(float)plane_.yDirection[1],(float)plane_.yDirection[2],
//                            (float)xy[0],(float)xy[1],(float)xy[2],
//                            (float)plane_.xDirection[0],(float)plane_.xDirection[1],(float)plane_.xDirection[2],
//                            0.0,0.0,0.0 };
//
//  // Create buffer for vertex coordinates if necessary
//  if ( ! vbo_ ) {
//    glGenBuffersARB(1, &vbo_);
//  }
//
//  // Bind buffer
//  glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo_);
//
//  // Upload to buffer ( 4 vertices with 3 coordinates of 4 byte sized floats)
//  glBufferDataARB(GL_ARRAY_BUFFER_ARB, 8 * 3 * 4, &vboData_[0], GL_STATIC_DRAW_ARB);
//
//
//  ro.vertexBuffer = vbo_;
//  ro.vertexDecl   = &vertexDecl_;
//
////  ro.culling   = false;
////  ro.blending  = false;
////  ro.depthTest =false;
//
//  ro.glDrawArrays(GL_QUADS, 0, 8);
//
//  ro.debugName = "PlaneNode.plane";
//
//  _renderer->addRenderObject(&ro);


}


//=============================================================================
