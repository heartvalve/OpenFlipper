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

  // Origin
  glColor3f(0.5, 0.5, 0.5);
  gluSphere( quadric, sphereRadius, slices, stacks );

  // X-Axis
  glColor3f(1.0, 0.0, 0.0);
  _state.push_modelview_matrix ();
  _state.rotate (-90, 0, 1, 0);
  _state.translate ( 0, 0, -bodyLength );
  gluCylinder( quadric, bodyRadius, bodyRadius, bodyLength, slices, stacks );
  gluDisk( quadric, 0, topRadius, slices, loops );
  _state.translate ( 0, 0, -arrowLength );
  gluCylinder( quadric, 0, topRadius, arrowLength, slices, stacks );
  _state.pop_modelview_matrix ();

  // Y-Axis
  glColor3f(0.0, 1.0, 0.0);
  _state.push_modelview_matrix ();
  _state.rotate (90, 1, 0, 0);
  _state.translate ( 0, 0, -bodyLength );
  gluCylinder( quadric, bodyRadius, bodyRadius, bodyLength, slices, stacks );
  gluDisk( quadric, 0, topRadius, slices, loops );
  _state.translate ( 0, 0, -arrowLength );
  gluCylinder( quadric, 0, topRadius, arrowLength, slices, stacks );
  _state.pop_modelview_matrix ();

  // Z-Axis
  glColor3f(0.0, 0.0, 1.0);
  _state.push_modelview_matrix ();
  _state.rotate (180, 0, 1, 0);
  _state.translate ( 0, 0, -bodyLength );
  gluCylinder( quadric, bodyRadius, bodyRadius, bodyLength, slices, stacks );
  gluDisk( quadric, 0, topRadius, slices, loops );
  _state.translate ( 0, 0, -arrowLength );
  gluCylinder( quadric, 0, topRadius, arrowLength, slices, stacks );
  _state.pop_modelview_matrix ();

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

  // Origin
  _state.pick_set_name (1);
  gluSphere( quadric, sphereRadius, slices, stacks );

  // X-Axis
  _state.pick_set_name (2);
  _state.push_modelview_matrix ();
  _state.rotate (-90, 0, 1, 0);
  _state.translate ( 0, 0, -bodyLength );
  gluCylinder( quadric, bodyRadius, bodyRadius, bodyLength, slices, stacks );
  gluDisk( quadric, 0, topRadius, slices, loops );
  _state.translate ( 0, 0, -arrowLength );
  gluCylinder( quadric, 0, topRadius, arrowLength, slices, stacks );
  _state.pop_modelview_matrix ();

  // Y-Axis
  _state.pick_set_name (3);
  _state.push_modelview_matrix ();
  _state.rotate (90, 1, 0, 0);
  _state.translate ( 0, 0, -bodyLength );
  gluCylinder( quadric, bodyRadius, bodyRadius, bodyLength, slices, stacks );
  gluDisk( quadric, 0, topRadius, slices, loops );
  _state.translate ( 0, 0, -arrowLength );
  gluCylinder( quadric, 0, topRadius, arrowLength, slices, stacks );
  _state.pop_modelview_matrix ();

  // Z-Axis
  _state.pick_set_name (4);
  _state.push_modelview_matrix ();
  _state.rotate (180, 0, 1, 0);
  _state.translate ( 0, 0, -bodyLength );
  gluCylinder( quadric, bodyRadius, bodyRadius, bodyLength, slices, stacks );
  gluDisk( quadric, 0, topRadius, slices, loops );
  _state.translate ( 0, 0, -arrowLength );
  gluCylinder( quadric, 0, topRadius, arrowLength, slices, stacks );
  _state.pop_modelview_matrix ();


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
    _state.reset_projection();
    _state.perspective(45.0, _state.aspect(), 0.8, 20.0);

    float posx = left + width - 30.0 ;
    float posy = bottom + height - 30.0 ;

    Vec3d screenposCenterPoint( posx , posy , 0.0);

    _state.push_modelview_matrix();
    _state.reset_modelview();

    pos3D = _state.unproject (Vec3d (posx, posy, 0.5));
    _state.pop_modelview_matrix();

    // reset scene translation
    GLMatrixd modelview = _state.modelview();

    modelview(0,3) = 0.0;
    modelview(1,3) = 0.0;
    modelview(2,3) = 0.0;

    _state.set_modelview (modelview);
    _state.translate (pos3D[0], pos3D[1], pos3D[2]-0.3, MULT_FROM_LEFT);

    // Koordinatensystem zeichnen
    drawCoordsys(_state);

    // Projection reload
    _state.pop_projection_matrix();


  } else if (mode_ == POSITION) { /* mode_ == POSITION */

    GLMatrixd modelview = _state.modelview();

    modelview(0,3) = 0.0;
    modelview(1,3) = 0.0;
    modelview(2,3) = 0.0;

    _state.set_modelview (modelview);
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
    _state.pick_set_maximum (5);
    _state.pick_set_name (0);

    // Init state - changes when mode_ != POSITION
    Vec3d pos3D(0.0,0.0,0.0);

    if ( mode_ == SCREENPOS ) {

      int left, bottom, width, height;
      _state.get_viewport(left, bottom, width, height);

      // Projection reset
      _state.push_projection_matrix();
      _state.reset_projection();
      _state.perspective(45.0, _state.aspect(), 0.8, 20.0);

      float posx = left + width - 30.0 ;
      float posy = bottom + height - 30.0 ;

      Vec3d screenposCenterPoint( posx , posy , 0.0);

      _state.push_modelview_matrix();
      _state.reset_modelview();

      pos3D = _state.unproject (Vec3d (posx, posy, 0.5));
      _state.pop_modelview_matrix();

      // reset scene translation
      GLMatrixd modelview = _state.modelview();

      modelview(0,3) = 0.0;
      modelview(1,3) = 0.0;
      modelview(2,3) = 0.0;

      _state.set_modelview (modelview);
      _state.translate (pos3D[0], pos3D[1], pos3D[2]-0.3, MULT_FROM_LEFT);

      // Koordinatensystem zeichnen
      drawCoordsysPick(_state);

      // Projection reload
      _state.pop_projection_matrix();


    } else if (mode_ == POSITION) { /* mode_ == POSITION */

      GLMatrixd modelview = _state.modelview();

      modelview(0,3) = 0.0;
      modelview(1,3) = 0.0;
      modelview(2,3) = 0.0;

     
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
