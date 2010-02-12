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
enter(GLState& _state, DrawModes::DrawMode /* _drawmode */ )
{
  _state.push_modelview_matrix();

  if ( applyTransformation_ )
    _state.mult_matrix(matrix_, inverse_matrix_);
}


//----------------------------------------------------------------------------


void
TransformNode::
leave(GLState& _state, DrawModes::DrawMode /* _drawmode */ )
{
  _state.pop_modelview_matrix();
}


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
