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
//  CLASS CoordsysNode - IMPLEMENTATION
//
//=============================================================================

//== INCLUDES =================================================================

#include "CoordsysNode.hh"
#include "../GL/gl.hh"
#include <iostream>
#include <math.h>


//== NAMESPACES ===============================================================

namespace ACG {
namespace SceneGraph {


//== IMPLEMENTATION ==========================================================


void
CoordsysNode::
boundingBox(Vec3f& _bbMin, Vec3f& _bbMax)
{
	//_bbMin.minimize( Vect3f  )
}


//----------------------------------------------------------------------------


unsigned int
CoordsysNode::
availableDrawModes() const
{
  return ( DrawModes::POINTS |
	        DrawModes::POINTS_SHADED |
	        DrawModes::POINTS_COLORED );
}


//----------------------------------------------------------------------------

void
CoordsysNode::
drawCoordsys( GLState&  _state) {

  double topRadius = 0.01;
  double arrowLength = 0.04;
  double bodyRadius = 0.004;
  double bodyLength = 0.06;
  int slices = 10;
  int stacks = 10;
  int loops = 10;
  double sphereRadius = 0.01;

  GLUquadricObj *quadric = gluNewQuadric();

  GLMatrixd modelview = _state.modelview();

  modelview(0,3) = 0.0;
  modelview(1,3) = 0.0;
  modelview(2,3) = 0.0;

  // Origin
  glColor3f(0.5, 0.5, 0.5);
  glLoadIdentity();
  gluSphere( quadric, sphereRadius, slices, stacks );

  // X-Axis
  glColor3f(1.0, 0.0, 0.0);
  glLoadIdentity();

  glMultMatrixd(modelview.get_raw_data());
  glRotatef(-90, 0, 1, 0);
  glTranslatef( 0, 0, -bodyLength );
  gluCylinder( quadric, bodyRadius, bodyRadius, bodyLength, slices, stacks );
  glLoadIdentity();
  glMultMatrixd(modelview.get_raw_data());
  glRotatef(-90, 0, 1, 0);
  glTranslatef( 0, 0, -bodyLength );
  gluDisk( quadric, 0, topRadius, slices, loops );
  glTranslatef( 0, 0, -arrowLength );
  gluCylinder( quadric, 0, topRadius, arrowLength, slices, stacks );

  // Y-Axis
  glColor3f(0.0, 1.0, 0.0);
  glLoadIdentity();
  glMultMatrixd(modelview.get_raw_data());
  glRotatef(90, 1, 0, 0);
  glTranslatef( 0, 0, -bodyLength );
  gluCylinder( quadric, bodyRadius, bodyRadius, bodyLength, slices, stacks );
  glLoadIdentity();
  glMultMatrixd(modelview.get_raw_data());
  glRotatef(90, 1, 0, 0);
  glTranslatef( 0, 0, -bodyLength );
  gluDisk( quadric, 0, topRadius, slices, loops );
  glTranslatef( 0, 0, -arrowLength );
  gluCylinder( quadric, 0, topRadius, arrowLength, slices, stacks );

  // Z-Axis
  glColor3f(0.0, 0.0, 1.0);
  glLoadIdentity();
  glMultMatrixd(modelview.get_raw_data());
  glRotatef(180, 0, 1, 0);
  glTranslatef( 0, 0, -bodyLength );
  gluCylinder( quadric, bodyRadius, bodyRadius, bodyLength, slices, stacks );
  glLoadIdentity();
  glMultMatrixd(modelview.get_raw_data());
  glRotatef(180, 0, 1, 0);
  glTranslatef( 0, 0, -bodyLength );
  gluDisk( quadric, 0, topRadius, slices, loops );
  glTranslatef( 0, 0, -arrowLength );
  gluCylinder( quadric, 0, topRadius, arrowLength, slices, stacks );

  gluDeleteQuadric(quadric);
}

//============================================================================

void
CoordsysNode::drawCoordsysPick( GLState&  _state) {

    double topRadius = 0.01;
    double arrowLength = 0.04;
    double bodyRadius = 0.004;
    double bodyLength = 0.06;
    int slices = 10;
    int stacks = 10;
    int loops = 10;
    double sphereRadius = 0.01;

    GLUquadricObj *quadric = gluNewQuadric();
    GLMatrixd modelview = _state.modelview();

    modelview(0,3) = 0.0;
    modelview(1,3) = 0.0;
    modelview(2,3) = 0.0;

  // Origin
    glLoadIdentity();
    glLoadName(0);
    gluSphere( quadric, sphereRadius, slices, stacks );

  // X-Axis
    glLoadIdentity();
    glLoadName(1);
    glMultMatrixd(modelview.get_raw_data());
    glRotatef(-90, 0, 1, 0);
    glTranslatef( 0, 0, -bodyLength );
    gluCylinder( quadric, bodyRadius, bodyRadius, bodyLength, slices, stacks );
    glLoadIdentity();
    glMultMatrixd(modelview.get_raw_data());
    glRotatef(-90, 0, 1, 0);
    glTranslatef( 0, 0, -bodyLength );
    gluDisk( quadric, 0, topRadius, slices, loops );
    glTranslatef( 0, 0, -arrowLength );
    gluCylinder( quadric, 0, topRadius, arrowLength, slices, stacks );

  // Y-Axis
    glLoadIdentity();
    glLoadName(2);
    glMultMatrixd(modelview.get_raw_data());
    glRotatef(90, 1, 0, 0);
    glTranslatef( 0, 0, -bodyLength );
    gluCylinder( quadric, bodyRadius, bodyRadius, bodyLength, slices, stacks );
    glLoadIdentity();
    glMultMatrixd(modelview.get_raw_data());
    glRotatef(90, 1, 0, 0);
    glTranslatef( 0, 0, -bodyLength );
    gluDisk( quadric, 0, topRadius, slices, loops );
    glTranslatef( 0, 0, -arrowLength );
    gluCylinder( quadric, 0, topRadius, arrowLength, slices, stacks );

  // Z-Axis
    glLoadIdentity();
    glLoadName(3);
    glMultMatrixd(modelview.get_raw_data());
    glRotatef(180, 0, 1, 0);
    glTranslatef( 0, 0, -bodyLength );
    gluCylinder( quadric, bodyRadius, bodyRadius, bodyLength, slices, stacks );
    glLoadIdentity();
    glMultMatrixd(modelview.get_raw_data());
    glRotatef(180, 0, 1, 0);
    glTranslatef( 0, 0, -bodyLength );
    gluDisk( quadric, 0, topRadius, slices, loops );
    glTranslatef( 0, 0, -arrowLength );
    gluCylinder( quadric, 0, topRadius, arrowLength, slices, stacks );

    gluDeleteQuadric(quadric);

}


//============================================================================


void
CoordsysNode::
draw(GLState&  _state  , unsigned int /*_drawMode*/)
{
  // Push Modelview-Matrix
  _state.push_modelview_matrix();

  Vec4f lastBaseColor = _state.base_color();

  glPushAttrib( GL_LIGHTING_BIT ); // STACK_ATTRIBUTES <- LIGHTING_ATTRIBUTE
  glEnable(GL_LIGHTING);
  glColorMaterial ( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE ) ;
  glEnable(GL_COLOR_MATERIAL);
  glShadeModel(GL_SMOOTH);

  // Init state - changes when mode_ != POSITION
  Vec3d pos3D(0.0,0.0,0.0);

  if ( mode_ == SCREENPOS ) {

    int left, bottom, width, height;
    _state.get_viewport(left, bottom, width, height);

    // Projection reset
    _state.push_projection_matrix();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, _state.aspect(), 0.8, 20.0);

    glMatrixMode(GL_MODELVIEW);

    float posx = left + width - 30.0 ;
    float posy = bottom + height - 30.0 ;

    Vec3d screenposCenterPoint( posx , posy , 0.0);

    double pm[16], mvm[16];
    double x,y,z;
    GLint viewport[4];

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glGetDoublev( GL_MODELVIEW_MATRIX, mvm);
    glPopMatrix();
    glGetDoublev( GL_PROJECTION_MATRIX, pm);

    viewport[0] = left;
    viewport[1] = bottom;
    viewport[2] = width;
    viewport[3] = height;


    gluUnProject(posx, posy, 0.5, mvm, pm, viewport, &x, &y, &z);

    pos3D = Vec3d(x, y, z);

    // Projection Translation
    glMatrixMode(GL_PROJECTION);
    // Translation in rechte obere Ecke
    glTranslatef(pos3D[0], pos3D[1], pos3D[2]-0.3);
    glMatrixMode(GL_MODELVIEW);

    // Koordinatensystem zeichnen
    drawCoordsys(_state);

    // Projection reload
    _state.pop_projection_matrix();
    glMatrixMode(GL_MODELVIEW);

  } else if (mode_ == POSITION) { /* mode_ == POSITION */

    drawCoordsys(_state);

  }

  glPopAttrib();

  glColor4fv(lastBaseColor.data());

  // Reload old configuration
  _state.pop_modelview_matrix();
}


void
CoordsysNode::
setMode(const CoordsysMode _mode)
{
  mode_ = _mode;
}

void
CoordsysNode::
setPosition(const Vec3f& _pos)
{
  pos3f_ = _pos;
}

CoordsysNode::CoordsysMode
CoordsysNode::
getMode() const
{
  return mode_;
}

void
CoordsysNode::pick(GLState& _state, PickTarget _target)
{
  if (_target == PICK_ANYTHING) {

    // Push Modelview-Matrix
    _state.push_modelview_matrix();
    glLoadName(0);

    // Init state - changes when mode_ != POSITION
    Vec3d pos3D(0.0,0.0,0.0);

    if ( mode_ == SCREENPOS ) {

      int left, bottom, width, height;
      _state.get_viewport(left, bottom, width, height);

      // Projection reset
      _state.push_projection_matrix();
      glMatrixMode(GL_PROJECTION);
      glPushMatrix ();
      glLoadIdentity();
      gluPerspective(45.0, _state.aspect(), 0.8, 20.0);

      glMatrixMode(GL_MODELVIEW);

      float posx = left + width - 30.0 ;
      float posy = bottom + height - 30.0 ;

      Vec3d screenposCenterPoint( posx , posy , 0.0);

      double pm[16], mvm[16];
      double x,y,z;
      GLint viewport[4];

      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();
      glLoadIdentity();
      glGetDoublev( GL_MODELVIEW_MATRIX, mvm);
      glPopMatrix();
      glGetDoublev( GL_PROJECTION_MATRIX, pm);

      viewport[0] = left;
      viewport[1] = bottom;
      viewport[2] = width;
      viewport[3] = height;

      gluUnProject(posx, posy, 0.5, mvm, pm, viewport, &x, &y, &z);

      pos3D = Vec3d(x, y, z);

      // Projection Translation
      glMatrixMode(GL_PROJECTION);

      // go back to gluPickMatrix
      glPopMatrix ();
      glMultMatrixd( _state.inverse_projection().get_raw_data());
      // add our matrix
      gluPerspective(45.0, _state.aspect(), 0.8, 20.0);
      // Translation in rechte obere Ecke
      glTranslatef(pos3D[0], pos3D[1], pos3D[2]-0.3);
      glMatrixMode(GL_MODELVIEW);

      // Koordinatensystem zeichnen
      drawCoordsysPick(_state);

      // Projection reload
      _state.pop_projection_matrix();

    } else if (mode_ == POSITION) { /* mode_ == POSITION */

      drawCoordsysPick(_state);

    }
    // Reload old configuration
    _state.pop_modelview_matrix();

  }
}

//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
