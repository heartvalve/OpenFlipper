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
//
//  CLASS TransformNode - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================

#include "TransformNode.hh"


//== IMPLEMENTATION ==========================================================


namespace ACG {
namespace SceneGraph {


//----------------------------------------------------------------------------


TransformNode::
TransformNode(BaseNode* _parent, const std::string& _name)
  : BaseNode(_parent,_name),
    center_(0.0, 0.0, 0.0),
    applyTransformation_(true)
{
  // ortho 2d stuff
  is2DObject_ = false;
  isPerSkeletonObject_ = false;
  scaleFactor2D_ = 1.0;
  offset_ = ACG::Vec2d(0.0,0.0);
  imageDimensions_ = ACG::Vec2i(-1,-1); // no image per default

  loadIdentity();
}


//----------------------------------------------------------------------------


void
TransformNode::
loadIdentity()
{
  matrix_.identity();
  inverse_matrix_.identity();

  rotation_matrix_.identity();
  inverse_rotation_matrix_.identity();
  quaternion_.identity();

  scale_matrix_.identity();
  inverse_scale_matrix_.identity();

  translation_ = Vec3d(0.0, 0.0, 0.0);
}


//----------------------------------------------------------------------------


void
TransformNode::
setIdentity()
{
  Vec3d v3 = matrix_.transform_point(center());
  set_center(v3);
  loadIdentity();
}


//----------------------------------------------------------------------------


void
TransformNode::
translate(const Vec3d& _v)
{
  translation_ += _v;
  updateMatrix();
}


//----------------------------------------------------------------------------


void
TransformNode::
rotate(double _angle, const Vec3d& _axis)
{
  quaternion_ *= Quaterniond(_axis, _angle/180.0*M_PI);
  quaternion_.normalize();
  rotation_matrix_ = quaternion_.rotation_matrix();
  inverse_rotation_matrix_ = rotation_matrix_;
  inverse_rotation_matrix_.transpose();
  updateMatrix();
}


//----------------------------------------------------------------------------


void
TransformNode::
scale(const Vec3d& _s)
{
  scale_matrix_.scale(_s);
  inverse_scale_matrix_ = scale_matrix_;
  inverse_scale_matrix_.invert();
  updateMatrix();
}

//----------------------------------------------------------------------------


void
TransformNode::
scale(const GLMatrixd& _m)
{
  scale_matrix_ *= _m;
  inverse_scale_matrix_ = scale_matrix_;
  inverse_scale_matrix_.invert();
  updateMatrix();
}


//----------------------------------------------------------------------------


void
TransformNode::
updateMatrix()
{
  // build matrix
  matrix_.identity();
  matrix_.translate(translation_);
  matrix_.translate(center_);
  matrix_ *= rotation_matrix_;
  matrix_ *= scale_matrix_;
  matrix_.translate(-center_);


  // build inverse matrix
  inverse_matrix_ = matrix_;
  inverse_matrix_.invert();
}


//----------------------------------------------------------------------------


void
TransformNode::
enter(GLState& _state, const DrawModes::DrawMode& /* _drawmode */ )
{
  _state.push_modelview_matrix();
  _state.push_projection_matrix();

  /*
  double modelview[16];
  glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
  double projection[16];
  glGetDoublev( GL_PROJECTION_MATRIX, projection );
  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);


  std::cout << "****************************** enter " << name() << std::endl;

  std::cout << "\nenter: mv:" << std::endl;
  for (uint i = 0; i < 16; ++i)
  {
    if (i%4==0)
      std::cerr << std::endl;
    std::cerr << modelview[i] << ", " << std::flush;
  }


  std::cout << "\nenter: pr:" << std::endl;
  for (uint i = 0; i < 16; ++i)
  {
    if (i%4==0)
      std::cerr << std::endl;
    std::cerr << projection[i] << ", " << std::flush;
  }

  std::cout << "\nenter: vp:" << std::endl;
  for (uint i = 0; i < 4; ++i)
  {
    std::cerr << viewport[i] << ", " << std::flush;
  }


  std::cout << "enter: mv = " << _state.modelview() << std::endl;
  std::cout << "enter: pr = " << _state.projection() << std::endl;
  std::cout << "enter: vp = " << _state.viewport() << std::endl;
  */


  if ( applyTransformation_ )
    _state.mult_matrix(matrix_, inverse_matrix_);

  if (is2DObject_)
    ortho2DMode(_state);
  
  if(isPerSkeletonObject_)
    perSkeletonMode(_state);
}


//----------------------------------------------------------------------------


void
TransformNode::
leave(GLState& _state, const DrawModes::DrawMode& /* _drawmode */ )
{
//   _state.pop_projection_matrix();
  _state.pop_modelview_matrix();
  _state.pop_projection_matrix();
}

//----------------------------------------------------------------------------
/*
void
TransformNode::
ortho2DMode(GLState& _state)
{
//   return;

  double modelview[16];
  glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
  double projection[16];
  glGetDoublev( GL_PROJECTION_MATRIX, projection );
  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);

  std::cout << "\n**************************************" << std::endl;
  std::cout << "modelview: " << std::flush;
  for (uint i = 0; i < 16; ++i)
    std::cout << modelview[i] << ", " << std::flush;

  std::cout << "\nprojection: " << std::flush;
  for (uint i = 0; i < 16; ++i)
    std::cout << projection[i] << ", " << std::flush;



//   std::cout << "****************************** ortho2DMode *******************************" << std::endl;
  // set the whole GL matrices such that subsequent rendering of 2d
  // image coordinates produces correct results
  int width  = _state.viewport_width();
  int height = _state.viewport_height();

  glViewport(0, 0, width, height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  gluOrtho2D(0.0, (GLdouble)width, (GLdouble)height, 0.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  // move image center to window center
  glTranslatef( 0.5*(width-1),  0.5*(height-1), 0);
  glScalef(scaleFactor2D_, scaleFactor2D_, 1.0);

  if (imageDimensions_[0] != -1)
    glTranslatef( -0.5*(imageDimensions_[0]-1),  -0.5*(imageDimensions_[1]-1), 0);
  glTranslatef(offset_[0], offset_[1], 0);


  glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
  glGetDoublev( GL_PROJECTION_MATRIX, projection );
  glGetIntegerv(GL_VIEWPORT, viewport);

  std::cout << "\nmodelview: " << std::flush;
  for (uint i = 0; i < 16; ++i)
    std::cout << modelview[i] << ", " << std::flush;

  std::cout << "\nprojection: " << std::flush;
  for (uint i = 0; i < 16; ++i)
    std::cout << projection[i] << ", " << std::flush;

}
*/

//----------------------------------------------------------------------------



void
TransformNode::
perSkeletonMode(GLState& _state)
{
  _state.set_modelview(perSkeletonModelView_);
}



void
TransformNode::
ortho2DMode(GLState& _state)
{
  // set ortho 2D mode in glstate
  int width  = _state.viewport_width();
  int height = _state.viewport_height();

  if (width == 0 || height == 0)
    return;
  
//   _state.viewport(0,0,width, height);
  _state.reset_projection();
  
  _state.ortho(-(GLdouble)width/2.0, (GLdouble)width/2.0, -(GLdouble)height/2.0, (GLdouble)height/2.0, 0.01, 20.0);
//   _state.ortho(0.0, (GLdouble)width, (GLdouble)height, 0.0, 0.01,20.0);
  
  _state.reset_modelview();
  
  _state.lookAt( Vec3d(0.0,0.0,0.0), 
                 Vec3d(0.0,0.0,1.0), 
                 Vec3d(0.0,-1.0,0.0));  // flip up direction (y-axis) s.t. opengl coordsys matches image coordsys
  
  _state.scale(scaleFactor2D_, scaleFactor2D_, 1.0);

  //   move image center to window center
  if (imageDimensions_[0] != -1)
    _state.translate(-0.5*(imageDimensions_[0]-1),  -0.5*(imageDimensions_[1]-1), 0);

  _state.translate(offset_[0], offset_[1], 0);
}

//----------------------------------------------------------------------------

//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
