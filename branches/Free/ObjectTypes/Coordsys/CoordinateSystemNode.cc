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
 *   $Revision$                                                       *
 *   $Author$                                                      *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/




//=============================================================================
//
//  CLASS CoordsysNode - IMPLEMENTATION
//
//=============================================================================

//== INCLUDES =================================================================

#include <ObjectTypes/Coordsys/CoordinateSystemNode.hh>
#include <ACG/GL/gl.hh>
#include <iostream>



//== NAMESPACES ===============================================================

namespace ACG {
namespace SceneGraph {


//== IMPLEMENTATION ==========================================================


CoordinateSystemNode::CoordinateSystemNode( BaseNode*    _parent,
                                            std::string  _name)
      : BaseNode(_parent, _name),
        coordsysSize_(1.0)
{
  rotation_.identity();
}

//----------------------------------------------------------------------------

void
CoordinateSystemNode::
boundingBox(Vec3d& _bbMin, Vec3d& _bbMax)
{
  //TODO!!
  Vec3d topLeft = position_ + coordsysSize_ * ACG::Vec3d(1.0,1.0,1.0);

	_bbMin.minimize( topLeft  );
	_bbMin.minimize( position_  );

	_bbMax.maximize( topLeft  );
	_bbMax.maximize( position_  );

}


//----------------------------------------------------------------------------


DrawModes::DrawMode
CoordinateSystemNode::
availableDrawModes()
{
  return ( DrawModes::SOLID_SMOOTH_SHADED );
}


//----------------------------------------------------------------------------

void
CoordinateSystemNode::
drawCoordsys( GLState&  _state) {

  double topRadius   = 0.1  * coordsysSize_;
  double arrowLength = 0.4  * coordsysSize_;
  double bodyRadius  = 0.04 * coordsysSize_;
  double bodyLength  = 0.6  * coordsysSize_;
  int slices = 10;
  int stacks = 10;
  int loops = 10;
  double sphereRadius = 0.1 * coordsysSize_;

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
CoordinateSystemNode::drawCoordsysPick( GLState&  _state) {

  double topRadius   = 0.1  * coordsysSize_;
  double arrowLength = 0.4  * coordsysSize_;
  double bodyRadius  = 0.04 * coordsysSize_;
  double bodyLength  = 0.6  * coordsysSize_;
  int slices = 10;
  int stacks = 10;
  int loops = 10;
  double sphereRadius = 0.1 * coordsysSize_;

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
CoordinateSystemNode::
draw(GLState&  _state  , const DrawModes::DrawMode& /*_drawMode*/)
{
  glPushAttrib(GL_ENABLE_BIT);

  // Push Modelview-Matrix
  _state.push_modelview_matrix();

  Vec4f lastBaseColor = _state.base_color();

  glColorMaterial ( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE ) ;
  ACG::GLState::enable(GL_COLOR_MATERIAL);
  ACG::GLState::enable(GL_LIGHTING);
  ACG::GLState::shadeModel(GL_SMOOTH);

  // Translate to right position  
  _state.translate(position_);

  // Apply rotation matrix
  GLMatrixd modelview = _state.modelview();	
  modelview *= rotation_;
  _state.set_modelview(modelview);

  // Draw coordsys
  drawCoordsys(_state);

  glColor4fv(lastBaseColor.data());

  // Reload old configuration
  _state.pop_modelview_matrix();

  glPopAttrib();
}


void
CoordinateSystemNode::
position(const Vec3d& _pos)
{
  position_ = _pos;
}


Vec3d
CoordinateSystemNode::
position() {
  return position_;
}

void CoordinateSystemNode::rotation(const Matrix4x4d & _rotation)
{
  rotation_ = _rotation;
}

Matrix4x4d CoordinateSystemNode::rotation()
{
  return rotation_;
} 


void
CoordinateSystemNode::
size(const double _size) {
  coordsysSize_ = _size;
}

double
CoordinateSystemNode::
size() {
  return coordsysSize_;
}


void
CoordinateSystemNode::pick(GLState& _state, PickTarget _target)
{

    
  if (_target == PICK_ANYTHING) {

    _state.pick_set_maximum(5);
    _state.pick_set_name(0);

    // Push Modelview-Matrix
    _state.push_modelview_matrix();

    // Translate to right position  
    _state.translate(position_);

    // Apply rotation matrix
    GLMatrixd modelview = _state.modelview(); 
    modelview *= rotation_;
    _state.set_modelview(modelview);    

    // Koordinatensystem zeichnen
    drawCoordsysPick(_state);

     // Reload old configuration
    _state.pop_modelview_matrix();

  }
}


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
