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
//  CLASS TranslationManipulatorNode - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================


#include "TranslationManipulatorNode.hh"
#include "../Math/GLMatrixT.hh"
#include "../GL/gl.hh"

#include <ACG/GL/IRenderer.hh>

#include <QMouseEvent>
#include <QEvent>
#include <float.h>

#include <Math_Tools/Math_Tools.hh>
#include <OpenMesh/Core/Geometry/MathDefs.hh>

//== NAMESPACES ===============================================================


namespace ACG {
namespace SceneGraph {


//== IMPLEMENTATION ==========================================================

// How many pixels should the cursor be away from the dragging center
// for the translation operation to affect the geometry's position
#define SNAP_PIXEL_TOLERANCE 30

// Node colors (normal, over, clicked, occluded normal, occluded over, occluded clicked)

const Vec4f colors[4][6] = {
  // origin
  {
    // normal
    Vec4f(0.2,0.2,0.2,1.0), Vec4f(0.5,0.5,0.5,1.0), Vec4f(0.8,0.8,0.8,1.0),
    // occluded
    Vec4f(0.2,0.2,0.2,0.2), Vec4f(0.5,0.5,0.5,0.4), Vec4f(0.8,0.8,0.8,0.6)
  },
  // X
  {
    // normal
    Vec4f(0.2,0.0,0.0,1.0), Vec4f(0.5,0.0,0.0,1.0), Vec4f(0.8,0.0,0.0,1.0),
    // occluded
    Vec4f(0.3,0.1,0.1,0.2), Vec4f(0.5,0.2,0.2,0.4), Vec4f(0.8,0.4,0.4,0.6)
  },
  // Y
  {
    // normal
    Vec4f(0.0,0.2,0.0,1.0), Vec4f(0.0,0.5,0.0,1.0), Vec4f(0.0,0.8,0.0,1.0),
    // occluded
    Vec4f(0.1,0.3,0.1,0.2), Vec4f(0.2,0.5,0.2,0.4), Vec4f(0.4,0.8,0.4,0.6)
  },
  // Z
  {
    // normal
    Vec4f(0.0,0.0,0.2,1.0), Vec4f(0.0,0.0,0.5,1.0), Vec4f(0.0,0.0,0.8,1.0),
    // occluded
    Vec4f(0.1,0.1,0.3,0.2), Vec4f(0.2,0.2,0.5,0.4), Vec4f(0.4,0.4,0.8,0.6)
  }
};


//----------------------------------------------------------------------------

TranslationManipulatorNode::Element::Element () :
  active_target_color_ (0.0, 0.0, 0.0, 1.0),
  active_current_color_ (0.0, 0.0, 0.0, 1.0),
  inactive_target_color_ (0.0, 0.0, 0.0, 1.0),
  inactive_current_color_ (0.0, 0.0, 0.0, 1.0),
  clicked_ (false),
  over_ (false)
{
}

//----------------------------------------------------------------------------

TranslationManipulatorNode::
TranslationManipulatorNode( BaseNode* _parent, const std::string& _name )
  : TransformNode(_parent, _name),
    draw_manipulator_(false),
    dirX_(1.0,0.0,0.0),
    dirY_(0.0,1.0,0.0),
    dirZ_(0.0,0.0,1.0),
    axisBottom_(0),
    axisCenter_(0),
    axisTop_(0),
    circle_(0),
    sphere_(0),
    manipulator_radius_(20.0),
    manipulator_height_(20),
    set_manipulator_radius_(1.0),
    set_manipulator_height_(1.0),
    manipulator_slices_(10),
    manipulator_stacks_(10),
    any_axis_clicked_(false),
    any_top_clicked_(false),
    outer_ring_clicked_(false),
    any_axis_over_(false),
    any_top_over_(false),
    outer_ring_over_(false),
    resize_current_ (0.0),
    mode_ (TranslationRotation),
    ignoreTime_ (false),
    dragging_(false),
    auto_size_(TranslationManipulatorNode::Never),
    auto_size_length_(1.0),
    activeRotations_(ALL_AXIS)
{
  localTransformation_.identity();

  // Create GLPrimitives
  axisBottom_ = new ACG::GLCone(manipulator_slices_, manipulator_stacks_,
      (1.0 - resize_current_) * manipulator_radius_,
      (1.0 + resize_current_) * manipulator_radius_, false, true);
  axisCenter_ = new ACG::GLCone(manipulator_slices_, manipulator_stacks_,
    (1.0 - resize_current_) * manipulator_radius_,
    (1.0 + resize_current_) * manipulator_radius_, false, true);
  axisTop_ = new ACG::GLCone(manipulator_slices_, manipulator_stacks_,
    (1.0 - resize_current_) * manipulator_radius_,
    (1.0 + resize_current_) * manipulator_radius_, false, true);

  sphere_ = new ACG::GLSphere(manipulator_slices_, manipulator_stacks_);
  circle_ = new ACG::GLDisk(30, 30, 2.0f*manipulator_height_ - manipulator_height_/4.0f, 2.0f*manipulator_height_);

  setTraverseMode( BaseNode::ChildrenFirst );

  updateTargetColors ();
  for (unsigned int i = 0; i < TranslationManipulatorNode::NumElements; i++)
  {
    element_[i].active_current_color_ = element_[i].active_target_color_;
    element_[i].inactive_current_color_ = element_[i].inactive_target_color_;
  }
/*  
  setMultipassNode(0);
  multipassNodeSetActive(2,true);
  
  setMultipassStatus(0);
  multipassStatusSetActive(1,true);
  multipassStatusSetActive(2,true);*/

}


//----------------------------------------------------------------------------

TranslationManipulatorNode::~TranslationManipulatorNode() {

  delete axisBottom_;
  delete axisCenter_;
  delete axisTop_;

  delete sphere_;

  delete circle_;
}


//----------------------------------------------------------------------------


void
TranslationManipulatorNode::
setIdentity()
{
  TransformNode::setIdentity();
}


//----------------------------------------------------------------------------
void
TranslationManipulatorNode::
update_manipulator_system(GLState& _state)
{
  _state.translate(center()[0], center()[1], center()[2]); // Follow translation of parent node
  _state.mult_matrix(inverse_scale (), scale ()); // Adapt scaling

  update_rotation(_state);
  updateSize (_state);
}

//----------------------------------------------------------------------------
// Local rotation of the manipulator
void
TranslationManipulatorNode::update_rotation(GLState& _state){

  // Get global transformation
  glMatrixMode(GL_MODELVIEW);
  GLMatrixd model = _state.modelview();

  // revert global transformation as we want to use our own
  model *= inverse_rotation();

  // apply local transformation to adjust our coordinate system
  model *= localTransformation_;

  // And update current matrix
  _state.set_modelview(model);

}

//----------------------------------------------------------------------------

void TranslationManipulatorNode::updateTargetColors ()
{
  // reset all color to default values
  element_[Origin].active_target_color_ = colors[0][0];
  element_[XAxis].active_target_color_ = colors[1][0];
  element_[YAxis].active_target_color_ = colors[2][0];
  element_[ZAxis].active_target_color_ = colors[3][0];
  element_[XTop].active_target_color_ = colors[1][0];
  element_[YTop].active_target_color_ = colors[2][0];
  element_[ZTop].active_target_color_ = colors[3][0];
  element_[XRing].active_target_color_ = colors[1][0];
  element_[YRing].active_target_color_ = colors[2][0];
  element_[ZRing].active_target_color_ = colors[3][0];

  element_[Origin].inactive_target_color_ = colors[0][3];
  element_[XAxis].inactive_target_color_ = colors[1][3];
  element_[YAxis].inactive_target_color_ = colors[2][3];
  element_[ZAxis].inactive_target_color_ = colors[3][3];
  element_[XTop].inactive_target_color_ = colors[1][3];
  element_[YTop].inactive_target_color_ = colors[2][3];
  element_[ZTop].inactive_target_color_ = colors[3][3];
  element_[XRing].inactive_target_color_ = colors[1][3];
  element_[YRing].inactive_target_color_ = colors[2][3];
  element_[ZRing].inactive_target_color_ = colors[3][3];


  // blending is enabled for ring so we have to set alpha correctly
  element_[XRing].active_target_color_[3] = 1.0;
  element_[YRing].active_target_color_[3] = 1.0;
  element_[ZRing].active_target_color_[3] = 1.0;

  // hide rings in resize mode
  if (mode_ == Resize || mode_ == Place)
    for (unsigned int i = 0; i < 3; i++)
    {
      element_[XRing + i].active_target_color_[3] = 0.0;
      element_[XRing + i].inactive_target_color_[3] = 0.0;
    }

  // set colors according to current (clicked/over/none) state
  if(element_[Origin].clicked_){
    element_[Origin].active_target_color_ = colors[0][2];
    element_[Origin].inactive_target_color_ = colors[0][5];
    for (unsigned int i = 1; i < NumElements - 3; i++)
    {
      element_[i].active_target_color_ = (colors[0][2] * 0.5) + (colors[((i-1)%3) + 1][2] * 0.5);
      element_[i].inactive_target_color_ = (colors[0][5] * 0.5) + (colors[((i-1)%3) + 1][5] * 0.5);
    }
    return;
  } else if(element_[Origin].over_){
    element_[Origin].active_target_color_ = colors[0][1];
    element_[Origin].inactive_target_color_ = colors[0][4];
    for (unsigned int i = 1; i < NumElements - 3; i++)
    {
      element_[i].active_target_color_ = (colors[0][1] * 0.5) + (colors[((i-1)%3) + 1][1] * 0.5);
      element_[i].inactive_target_color_ = (colors[0][4] * 0.5) + (colors[((i-1)%3) + 1][4] * 0.5);
    }
    return;
  }

  for (unsigned int i = 0; i < 3; i++)
    if (element_[i + XTop].clicked_)
    {
      element_[i + XTop].active_target_color_ = colors[i+1][2];
      element_[i + XTop].inactive_target_color_ = colors[i+1][5];
      if (mode_ != TranslationRotation)
      {
        element_[i + XAxis].active_target_color_ = colors[i+1][2];
        element_[i + XAxis].inactive_target_color_ = colors[i+1][5];
      }
      if (mode_ != Resize) {
        element_[i + XRing].active_target_color_ = colors[i+1][2];
        element_[i + XRing].inactive_target_color_ = colors[i+1][5];
      }
      return;
    } else if (element_[i + XTop].over_)
    {
      element_[i + XTop].active_target_color_ = colors[i+1][1];
      element_[i + XTop].inactive_target_color_ = colors[i+1][4];
      if (mode_ != TranslationRotation)
      {
        element_[i + XAxis].active_target_color_ = colors[i+1][1];
        element_[i + XAxis].inactive_target_color_ = colors[i+1][4];
      }
      if (mode_ != Resize) {
        element_[i + XRing].active_target_color_ = colors[i+1][1];
        element_[i + XRing].inactive_target_color_ = colors[i+1][4];
      }
      return;
    }

  for (unsigned int i = 0; i < 3; i++)
    if (element_[i + XAxis].clicked_)
    {
      element_[i + XTop].active_target_color_ = colors[i+1][2];
      element_[i + XTop].inactive_target_color_ = colors[i+1][5];
      element_[i + XAxis].active_target_color_ = colors[i+1][2];
      element_[i + XAxis].inactive_target_color_ = colors[i+1][5];
      if (mode_ == LocalRotation) {
        element_[i + XRing].active_target_color_ = colors[i+1][2];
        element_[i + XRing].inactive_target_color_ = colors[i+1][5];
      }
      return;
    } else if (element_[i + XAxis].over_)
    {
      element_[i + XTop].active_target_color_ = colors[i+1][1];
      element_[i + XTop].inactive_target_color_ = colors[i+1][4];
      element_[i + XAxis].active_target_color_ = colors[i+1][1];
      element_[i + XAxis].inactive_target_color_ = colors[i+1][4];
      if (mode_ == LocalRotation) {
        element_[i + XRing].active_target_color_ = colors[i+1][1];
        element_[i + XRing].inactive_target_color_ = colors[i+1][4];
      }
      return;
    }

  if (mode_ != Resize)
  for (unsigned int i = 0; i < 3; i++)
    if (element_[i + XRing].clicked_)
    {
      element_[i + XRing].active_target_color_ = colors[i+1][2];
      element_[i + XRing].inactive_target_color_ = colors[i+1][5];
      return;
    } else if (element_[i + XRing].over_)
    {
      element_[i + XRing].active_target_color_ = colors[i+1][1];
      element_[i + XRing].inactive_target_color_ = colors[i+1][4];
      return;
    }

}

//----------------------------------------------------------------------------

bool TranslationManipulatorNode::updateCurrentColors (GLState& _state)
{
  bool rv = false;

  float value = (float)_state.msSinceLastRedraw () / 1000.0;

  if (ignoreTime_)
  {
    value = 0;
    ignoreTime_ = false;
  }

  for (unsigned int i = 0; i < NumElements; i++)
  {
    Vec4f diff = element_[i].active_target_color_ -
      element_[i].active_current_color_;

    for (unsigned int j = 0; j < 4; j++)
      if (diff[j] > value)
        diff[j] = value;
      else if (diff[j] < -value)
        diff[j] = -value;

    element_[i].active_current_color_ += diff;
    
    diff = element_[i].inactive_target_color_ -
      element_[i].inactive_current_color_;
    
    for (unsigned int j = 0; j < 4; j++)
      if (diff[j] > value)
        diff[j] = value;
      else if (diff[j] < -value)
        diff[j] = -value;
    
    element_[i].inactive_current_color_ += diff;

    rv |= element_[i].active_target_color_ != element_[i].active_current_color_ ||
      element_[i].inactive_target_color_ != element_[i].inactive_current_color_;
  }

  float diff = ((mode_ == Resize) ? 1.0 : 0.0) - resize_current_;
  
  if (diff > value)
    diff = value;
  else if (diff < -value)
    diff = -value;
  
  resize_current_ += diff;
  rv |= resize_current_ != ((mode_ == Resize) ? 1.0 : 0.0);

  return rv;
}

//----------------------------------------------------------------------------

void TranslationManipulatorNode::drawManipulator (GLState& _state, bool _active)
{
  glPushAttrib(GL_ENABLE_BIT );
  ACG::GLState::disable( GL_CULL_FACE );

  // Save modelview matrix
  _state.push_modelview_matrix();

  //================================================================================================
  // Sphere
  //================================================================================================

//     ACG::GLState::disable(GL_ALPHA_TEST);
//     ACG::GLState::enable(GL_BLEND);
//     ACG::GLState::enable(GL_CULL_FACE);
//     glColorMaterial ( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE ) ;
//     ACG::GLState::enable(GL_COLOR_MATERIAL);
//
//     ACG::GLState::blendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_DST_ALPHA);
//
//     glColor4f( 1.0,0.0,0.0,0.3);
//     gluSphere( axis_, manipulator_height_ * 1.5 ,  manipulator_slices_ * 2, manipulator_stacks_ * 2);
//     ACG::GLState::disable(GL_COLOR_MATERIAL);
//
//
//     ACG::GLState::enable(GL_ALPHA_TEST);
//     ACG::GLState::disable(GL_BLEND);

  //================================================================================================
  // Z-Axis
  //================================================================================================
  // gluCylinder draws into z direction so z-Axis first

  if (_active)
  {
    _state.set_diffuse_color(element_[ZAxis].active_current_color_);
    _state.set_specular_color(element_[ZAxis].active_current_color_);
  } else {
    _state.set_diffuse_color(element_[ZAxis].inactive_current_color_);
    _state.set_specular_color(element_[ZAxis].inactive_current_color_);
  }

  // Draw Bottom of z-axis
  axisBottom_->setBottomRadius((1.0 - resize_current_) * manipulator_radius_);
  axisBottom_->setTopRadius((1.0 + resize_current_) * manipulator_radius_);
  axisBottom_->draw(_state, manipulator_height_/2.0);

  // Draw center of z-axis
  _state.translate(0.0, 0.0, manipulator_height_/2);

  axisCenter_->setBottomRadius(manipulator_radius_);
  axisCenter_->setTopRadius(manipulator_radius_);
  axisCenter_->draw(_state, manipulator_height_/2.0);


  // Draw Top of z-axis
  if (_active)
  {
    _state.set_diffuse_color(element_[ZTop].active_current_color_);
    _state.set_specular_color(element_[ZTop].active_current_color_);
  } else {
    _state.set_diffuse_color(element_[ZTop].inactive_current_color_);
    _state.set_specular_color(element_[ZTop].inactive_current_color_);
  }

  _state.translate(0.0, 0.0, manipulator_height_/2);
  axisTop_->setBottomRadius(manipulator_radius_*2.0);
  axisTop_->setTopRadius(0.0);
  axisTop_->draw(_state, manipulator_height_/2.0);
  _state.translate(0.0, 0.0, -manipulator_height_);

  //================================================================================================
  // Y-Axis
  //================================================================================================
  _state.rotate(-90, 1.0, 0.0, 0.0);
  if (_active)
  {
    _state.set_diffuse_color(element_[YAxis].active_current_color_);
    _state.set_specular_color(element_[YAxis].active_current_color_);
  } else {
    _state.set_diffuse_color(element_[YAxis].inactive_current_color_);
    _state.set_specular_color(element_[YAxis].inactive_current_color_);
  }

  // Draw Bottom of y-axis
  axisBottom_->setBottomRadius((1.0 - resize_current_) * manipulator_radius_);
  axisBottom_->setTopRadius((1.0 + resize_current_) * manipulator_radius_);
  axisBottom_->draw(_state, manipulator_height_/2.0);

  // Draw center of y-axis
  _state.translate(0.0, 0.0, manipulator_height_/2);

  axisCenter_->setBottomRadius(manipulator_radius_);
  axisCenter_->setTopRadius(manipulator_radius_);
  axisCenter_->draw(_state, manipulator_height_/2.0);


  // Draw Top of y-axis
  if (_active)
  {
    _state.set_diffuse_color(element_[YTop].active_current_color_);
    _state.set_specular_color(element_[YTop].active_current_color_);
  } else {
    _state.set_diffuse_color(element_[YTop].inactive_current_color_);
    _state.set_specular_color(element_[YTop].inactive_current_color_);
  }

  _state.translate(0.0, 0.0, manipulator_height_/2);
  axisTop_->setBottomRadius(manipulator_radius_*2.0);
  axisTop_->setTopRadius(0.0);
  axisTop_->draw(_state, manipulator_height_/2.0);
  _state.translate(0.0, 0.0, -manipulator_height_);


  //================================================================================================
  // X-Axis
  //================================================================================================
  _state.rotate(90, 0.0, 1.0, 0.0);
  if (_active)
  {
    _state.set_diffuse_color(element_[XAxis].active_current_color_);
    _state.set_specular_color(element_[XAxis].active_current_color_);
  } else {
    _state.set_diffuse_color(element_[XAxis].inactive_current_color_);
    _state.set_specular_color(element_[XAxis].inactive_current_color_);
  }

  // Draw Bottom of x-axis
  axisBottom_->setBottomRadius((1.0 - resize_current_) * manipulator_radius_);
  axisBottom_->setTopRadius((1.0 + resize_current_) * manipulator_radius_);
  axisBottom_->draw(_state, manipulator_height_/2.0);

  // Draw center of x-axis
  _state.translate(0.0, 0.0, manipulator_height_/2);

  axisCenter_->setBottomRadius(manipulator_radius_);
  axisCenter_->setTopRadius(manipulator_radius_);
  axisCenter_->draw(_state, manipulator_height_/2.0);


  // Draw Top of x-axis
  if (_active)
  {
    _state.set_diffuse_color(element_[XTop].active_current_color_);
    _state.set_specular_color(element_[XTop].active_current_color_);
  } else {
    _state.set_diffuse_color(element_[XTop].inactive_current_color_);
    _state.set_specular_color(element_[XTop].inactive_current_color_);
  }

  _state.translate(0.0, 0.0, manipulator_height_/2);
  axisTop_->setBottomRadius(manipulator_radius_*2.0);
  axisTop_->setTopRadius(0.0);
  axisTop_->draw(_state, manipulator_height_/2.0);
  _state.translate(0.0, 0.0, -manipulator_height_);

  //=================================================================================================
  // Sphere
  //=================================================================================================
  if (_active)
  {
    _state.set_diffuse_color(element_[Origin].active_current_color_);
    _state.set_specular_color(element_[Origin].active_current_color_);
  } else {
    _state.set_diffuse_color(element_[Origin].inactive_current_color_);
    _state.set_specular_color(element_[Origin].inactive_current_color_);
  }

  sphere_->draw(_state, manipulator_radius_*2);

  //=================================================================================================
  // Outer-Rings
  //=================================================================================================

  ACG::GLState::enable (GL_BLEND);
  glPushAttrib(GL_LIGHTING_BIT);
  ACG::GLState::disable(GL_LIGHTING);

  // update circle size
  circle_->setInnerRadius(2.0f*manipulator_height_ - manipulator_height_/4.0f);
  circle_->setOuterRadius(2.0f*manipulator_height_);

  if ( activeRotations_ & X_AXIS) {
    if (_active)
    {
      _state.set_diffuse_color(element_[XRing].active_current_color_);
      _state.set_ambient_color(element_[XRing].active_current_color_);
      _state.set_color(element_[XRing].active_current_color_);

    } else {
      _state.set_diffuse_color(element_[XRing].inactive_current_color_);
      _state.set_ambient_color(element_[XRing].inactive_current_color_);
      _state.set_color(element_[XRing].inactive_current_color_);

    }

    circle_->draw(_state);
  }


  _state.rotate(90, 0.0, 1.0, 0.0);
  if ( activeRotations_ & Y_AXIS) {
    if (_active)
    {
      _state.set_diffuse_color(element_[YRing].active_current_color_);
      _state.set_ambient_color(element_[YRing].active_current_color_);
      _state.set_color(element_[YRing].active_current_color_);

    } else {
      _state.set_diffuse_color(element_[YRing].inactive_current_color_);
      _state.set_ambient_color(element_[YRing].inactive_current_color_);
      _state.set_color(element_[YRing].inactive_current_color_);

    }

    circle_->draw(_state);
  }

  _state.rotate(90, 1.0, 0.0, 0.0);
  if ( activeRotations_ & Z_AXIS) {
    if (_active)
    {
      _state.set_diffuse_color(element_[ZRing].active_current_color_);
      _state.set_ambient_color(element_[ZRing].active_current_color_);
      _state.set_color(element_[ZRing].active_current_color_);

    } else {
      _state.set_diffuse_color(element_[ZRing].inactive_current_color_);
      _state.set_ambient_color(element_[ZRing].inactive_current_color_);
      _state.set_color(element_[ZRing].inactive_current_color_);

    }

    circle_->draw(_state);
  }

  glPopAttrib(); // ENABLE_BIT
  glPopAttrib(); // LIGHTING_BIT


  _state.pop_modelview_matrix();
}

//----------------------------------------------------------------------------


void
TranslationManipulatorNode::draw(GLState& _state, const DrawModes::DrawMode& /* _drawMode */ )
{
   GLenum prev_depth = _state.depthFunc();
    
   if (draw_manipulator_) {

    // Store lighting bits and enable lighting
    glPushAttrib(GL_LIGHTING_BIT);
    ACG::GLState::enable(GL_LIGHTING);
    ACG::GLState::shadeModel(GL_SMOOTH);

    // Store original depth status
    glPushAttrib(GL_DEPTH_BUFFER_BIT);


    // Alpha Test things
    glPushAttrib(GL_COLOR_BUFFER_BIT);

    glPushAttrib(GL_ENABLE_BIT);

    // backup colors
    Vec4f backup_diffuse  = _state.diffuse_color();
    Vec4f backup_specular = _state.specular_color();


    // Correctly align nodes local coordinate system
    update_manipulator_system(_state);

    updateTargetColors ();
    if (updateCurrentColors (_state))
      setDirty ();

    ACG::GLState::enable(GL_DEPTH_TEST);
    ACG::GLState::depthFunc(GL_GREATER);
    glDepthMask(GL_FALSE);
    ACG::GLState::enable (GL_BLEND);
    ACG::GLState::blendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    drawManipulator(_state, false);
    ACG::GLState::disable (GL_BLEND);

    ACG::GLState::depthFunc(GL_LEQUAL);
    glDepthMask(GL_TRUE);
    drawManipulator(_state, true);

    // Restore old attributes and modelview
    glPopAttrib();
    glPopAttrib();
    glPopAttrib();
    glPopAttrib();

    // restore active colors
    _state.set_diffuse_color(backup_diffuse);
    _state.set_specular_color(backup_specular);
    
    ACG::GLState::depthFunc(prev_depth);
  }
}

//----------------------------------------------------------------------------

GLMatrixd TranslationManipulatorNode::computeWorldMatrix()
{
  GLMatrixd world;
  world.identity();

  world.translate(center()[0], center()[1], center()[2]); // Follow translation of parent node
  world *= inverse_scale();

  // revert global transformation as we want to use our own
  world *= inverse_rotation();

  // apply local transformation to adjust our coordinate system
  world *= localTransformation_;

  return world;
}

//----------------------------------------------------------------------------

void TranslationManipulatorNode::getRenderObjects(IRenderer* _renderer,
                                                  GLState& _state,
                                                  const DrawModes::DrawMode& _drawMode,
                                                  const Material* _mat)
{
  if (draw_manipulator_)
  {
    RenderObject ro;
    ro.initFromState(&_state);

    // compute model-view matrix for the center of manipulator
    ro.modelview *= computeWorldMatrix();
    updateSize(_state);

    updateTargetColors ();
    if (updateCurrentColors (_state))
      setDirty ();


    // unlit, use emissive color only
    ro.shaderDesc.shadeMode = SG_SHADE_UNLIT;
    ro.shaderDesc.vertexColors = false;
    ro.shaderDesc.textured = false;

    // we need the scene zbuffer for the transparent overdraw effect
    // -> defer as long as possible
    ro.priority = 1000;

    // 1st pass
    // draw occluded areas on top via alpha blending
    ro.depthTest = true;
    ro.depthFunc = GL_GREATER;
    ro.depthWrite = false;

    ro.blending = true;
    ro.blendSrc = GL_SRC_ALPHA;
    ro.blendDest = GL_ONE_MINUS_SRC_ALPHA;
    ro.alpha = 0.5f;

    addManipulatorToRenderer(_renderer, &ro, false);

    // 2nd pass
    // draw rest as usual
    ro.priority = 1001;
    ro.blending = false;
    ro.depthFunc = GL_LEQUAL;
    ro.depthWrite = true;
    addManipulatorToRenderer(_renderer, &ro, true);
  }
}

//----------------------------------------------------------------------------

void TranslationManipulatorNode::addAxisToRenderer (IRenderer* _renderer, RenderObject* _baseRO, bool _active, int _axis)
{
  assert(_axis >= XAxis && _axis - 3 >= 0 && _axis <= ZAxis);

  for (int i = 0; i < 3; ++i)
    _baseRO->emissive[i] = _active ? element_[_axis].active_current_color_[i] : element_[_axis].inactive_current_color_[i];

  // Draw Bottom
  axisBottom_->setBottomRadius((1.0 - resize_current_) * manipulator_radius_);
  axisBottom_->setTopRadius((1.0 + resize_current_) * manipulator_radius_);
  axisBottom_->addToRenderer(_renderer, _baseRO, manipulator_height_/2.0f);

  // Draw center
  _baseRO->modelview.translate(0.0, 0.0, manipulator_height_/2);

  axisCenter_->setBottomRadius(manipulator_radius_);
  axisCenter_->setTopRadius(manipulator_radius_);
  axisCenter_->addToRenderer(_renderer, _baseRO, manipulator_height_/2.0f);


  // Draw top
  
  // _axis - 3 computes id of axis-top in 'Elements' enum
  for (int i = 0; i < 3; ++i)
    _baseRO->emissive[i] = _active ? (element_[_axis-3].active_current_color_[i]) : (element_[_axis-3].inactive_current_color_[i]);

  _baseRO->modelview.translate(0.0, 0.0, manipulator_height_/2);
  axisTop_->setBottomRadius(manipulator_radius_*2.0);
  axisTop_->setTopRadius(0.0);
  axisTop_->addToRenderer(_renderer, _baseRO, manipulator_height_/2.0f);
  _baseRO->modelview.translate(0.0, 0.0, -manipulator_height_);

}

//----------------------------------------------------------------------------

void TranslationManipulatorNode::addManipulatorToRenderer (IRenderer* _renderer, RenderObject* _baseRO, bool _active)
{
  _baseRO->culling = false;

  GLMatrixf oldModelview = _baseRO->modelview;

  //================================================================================================
  // Z-Axis
  //================================================================================================
  // gluCylinder draws into z direction so z-Axis first

  addAxisToRenderer(_renderer, _baseRO, _active, ZAxis);

  //================================================================================================
  // Y-Axis
  //================================================================================================

  _baseRO->modelview.rotate(-90, 1.0, 0.0, 0.0);
  addAxisToRenderer(_renderer, _baseRO, _active, YAxis);

  //================================================================================================
  // X-Axis
  //================================================================================================

  _baseRO->modelview.rotate(90, 0.0, 1.0, 0.0);
  addAxisToRenderer(_renderer, _baseRO, _active, XAxis);

  //=================================================================================================
  // Sphere
  //=================================================================================================

  for (int i = 0; i < 3; ++i)
    _baseRO->emissive[i] = _active ? (element_[Origin].active_current_color_[i]) : (element_[Origin].inactive_current_color_[i]);

  sphere_->addToRenderer(_renderer, _baseRO, manipulator_radius_ * 2.0f);

  //=================================================================================================
  // Outer-Rings
  //=================================================================================================

//  _baseRO->blending = true;

  // update circle size
  circle_->setInnerRadius(2.0f*manipulator_height_ - manipulator_height_/4.0f);
  circle_->setOuterRadius(2.0f*manipulator_height_);

  if ( activeRotations_ & X_AXIS) 
  {
    for (int i = 0; i < 3; ++i)
      _baseRO->emissive[i] = _active ? (element_[XRing].active_current_color_[i]) : (element_[XRing].inactive_current_color_[i]);

    circle_->addToRenderer(_renderer, _baseRO);
  }


  _baseRO->modelview.rotate(90, 0.0, 1.0, 0.0);
  if ( activeRotations_ & Y_AXIS) 
  {
    for (int i = 0; i < 3; ++i)
      _baseRO->emissive[i] = _active ? (element_[YRing].active_current_color_[i]) : (element_[YRing].inactive_current_color_[i]);
    circle_->addToRenderer(_renderer, _baseRO);
  }

  _baseRO->modelview.rotate(90, 1.0, 0.0, 0.0);
  if ( activeRotations_ & Z_AXIS) 
  {
    for (int i = 0; i < 3; ++i)
      _baseRO->emissive[i] = _active ? (element_[ZRing].active_current_color_[i]) : (element_[ZRing].inactive_current_color_[i]);
    circle_->addToRenderer(_renderer, _baseRO);
  }

  _baseRO->modelview = oldModelview;
}

//----------------------------------------------------------------------------

// void
// TranslationManipulatorNode::leave(GLState& _state, const DrawModes::DrawMode& /* _drawMode */ )
// {

// }

//----------------------------------------------------------------------------

void
TranslationManipulatorNode::mouseEvent(GLState& _state, QMouseEvent* _event)
{
  if (!draw_manipulator_)
    return; // Avoid transformation of object while transformator is invisible

  Vec3d oldPoint3D;
  Vec2i newPoint2D(_event->pos().x(), _event->pos().y());
  Vec3d newPoint3D;
  double old_axis_hit, new_axis_hit, new_axis_over;
  bool rot[3], trans[3];
  unsigned int i;
  bool lockOldPoint = false;

  updateSize(_state);

  switch (_event->type()) {

    // If mouse is pressed check if any part of the manipulator is hit and store that state
    case QEvent::MouseButtonPress: {
      for (i = 0; i < NumElements; i++)
        element_[i].clicked_ = false;

      // Start dragging process
      if (!dragging_) {
        draggingOrigin3D_ = center();
        dragging_ = true;
      }

      // hit origin ?
      if ((mode_ != LocalRotation) && (mode_ != Rotation))
        element_[Origin].clicked_ = hitSphere(_state, newPoint2D);
      else
        element_[Origin].clicked_ = false;

      // hit any top ?
      any_top_clicked_ = mapToCylinderTop(_state, newPoint2D, new_axis_hit, Click);

      // hit any axis ?
      any_axis_clicked_ = mapToCylinder(_state, newPoint2D, new_axis_hit, Click);

      // hit one of the outer rings ?
      if (mode_ != Resize)
        outer_ring_clicked_ = mapToSphere(_state, newPoint2D, newPoint3D, Click);
      else
        outer_ring_clicked_ = false;

      // origin has been hit, ignore all other parts
      if (element_[Origin].clicked_) {
        for (i = 1; i < NumElements; i++)
          element_[i].clicked_ = false;
        any_axis_clicked_ = false;
        any_top_clicked_ = false;
        outer_ring_clicked_ = false;
      } else if (any_top_clicked_) { // tops override the axes
        for (i = XAxis; i < NumElements; i++)
          element_[i].clicked_ = false;
        any_axis_clicked_ = false;
        outer_ring_clicked_ = false;
      } else if (any_axis_clicked_) { // axes have been hit, disable rest ... should not be required
        for (i = XRing; i < NumElements; i++)
          element_[i].clicked_ = false;
        outer_ring_clicked_ = false;
      } else if (outer_ring_clicked_) { // Nothing except the outer ring has been hit
        for (i = 0; i < XRing; i++)
          element_[i].clicked_ = false;
        any_axis_clicked_ = false;
        any_top_clicked_ = false;
      }

      // Reset local transformation:
      if ( (_event->modifiers() & Qt::ControlModifier) && (_event->modifiers() & Qt::AltModifier) ) {
        localTransformation_.identity();
      }

      oldPoint2D_ = newPoint2D;
      currentScale_ = Vec3d(1.0, 1.0, 1.0);
      ignoreTime_ = true;
      break;
    }

      // Reset all states as we stopped manipulating
    case QEvent::MouseButtonRelease: {

      for (i = 0; i < NumElements; i++) {
        element_[i].clicked_ = false;
      }
      any_axis_clicked_ = false;
      any_top_clicked_ = false;
      outer_ring_clicked_ = false;
      ignoreTime_ = true;
      dragging_ = false;
      break;
    }

    case QEvent::MouseButtonDblClick: {
      draw_manipulator_ = !draw_manipulator_;
      break;
    }

      // All real manipulation is done here
    case QEvent::MouseMove: {
      if (!draw_manipulator_)
        return; // Avoid manipulation if manipulator is invisible

      // Get pressed modifiers
      Qt::KeyboardModifiers mods = Qt::ShiftModifier | Qt::ControlModifier;

      for (i = 0; i < NumElements; i++)
        element_[i].over_ = false;
      any_axis_over_ = false;
      any_top_over_ = false;
      outer_ring_over_ = false;

      if (!(element_[Origin].clicked_ || any_top_clicked_ || any_axis_clicked_ || outer_ring_clicked_)) {
        // over origin ?
        if ((mode_ != LocalRotation) && (mode_ != Rotation))
          element_[Origin].over_ = hitSphere(_state, newPoint2D);
        else
          element_[Origin].over_ = false;

        // over any top ?
        if (mode_ != Place) {
          any_top_over_ = mapToCylinderTop(_state, newPoint2D, new_axis_over, Over);
        }

        // over any axis ?
        if (mode_ != Place) {
          any_axis_over_ = mapToCylinder(_state, newPoint2D, new_axis_over, Over);
        }

        // over one of the outer rings ?
        if (mode_ != Resize) {
          outer_ring_over_ = mapToSphere(_state, newPoint2D, newPoint3D, Over);
        } else {
          outer_ring_over_ = false;
        }

        // origin has been hit, ignore all other parts
        if (element_[Origin].over_) {
          for (i = 1; i < NumElements; i++)
            element_[i].over_ = false;
          any_axis_over_ = false;
          any_top_over_ = false;
          outer_ring_over_ = false;
        } else if (any_top_over_) { // tops override the axes
          for (i = XAxis; i < NumElements; i++)
            element_[i].over_ = false;
          any_axis_over_ = false;
          outer_ring_over_ = false;
        } else if (any_axis_over_) { // axes have been hit, disable rest ... should not be required
          for (i = XRing; i < NumElements; i++)
            element_[i].over_ = false;
          outer_ring_over_ = false;
        } else if (outer_ring_over_) { // Nothing except the outer ring has been hit
          for (i = 0; i < XRing; i++)
            element_[i].over_ = false;
          any_axis_over_ = false;
          any_top_over_ = false;
        }
      }

      // set action for the different modes
      switch (mode_) {
        case Rotation:
          for (i = 0; i < 3; i++) {
            rot[i] = element_[XTop + i].clicked_ || element_[XRing + i].clicked_;
            trans[i] = false;
          }
          break;
        case TranslationRotation:
          for (i = 0; i < 3; i++) {
            rot[i] = element_[XTop + i].clicked_ || element_[XRing + i].clicked_;
            trans[i] = element_[XAxis + i].clicked_;
          }
          break;
        case LocalRotation:
          for (i = 0; i < 3; i++) {
            rot[i] = element_[XTop + i].clicked_ || element_[XRing + i].clicked_ || element_[XAxis + i].clicked_;
            trans[i] = false;
          }
          break;
        case Place:
          break;
        case Resize:
          for (i = 0; i < 3; i++) {
            rot[i] = false;
            trans[i] = element_[XTop + i].clicked_ || element_[XAxis + i].clicked_;
          }
          break;
      }

      // If origin was clicked on
      if (element_[Origin].clicked_) {

        // translate along screen plane ( unproject to get world coords for translation and apply them )

        Vec3d d = _state.project(center());
        Vec3d d_origin = _state.project(draggingOrigin3D_);

        // Snap back to origin position if mouse cursor
        // is near enough
        if (abs(d_origin[0] - newPoint2D[0]) < SNAP_PIXEL_TOLERANCE
            && abs(_state.context_height() - d_origin[1] - newPoint2D[1]) < SNAP_PIXEL_TOLERANCE
            && (_event->modifiers() & Qt::AltModifier) ) {
          newPoint2D = oldPoint2D_;
          Vec3d backtrans = draggingOrigin3D_ - center();
          if (mode_ != Resize) {
            translate(backtrans);
          }
        }

        // translate along screen plane ( unproject to get world coords for translation and apply them )
        Vec3d oldvec = _state.unproject(Vec3d(oldPoint2D_[0], (_state.context_height() - oldPoint2D_[1]), d[2]));
        Vec3d newvec = _state.unproject(Vec3d(newPoint2D[0], (_state.context_height() - newPoint2D[1]), d[2]));
        Vec3d ntrans = newvec - oldvec;

        if (mode_ != Resize)
          translate(ntrans);
        else {
          // revert last scale
          scale(Vec3d(1.0 / currentScale_[0], 1.0 / currentScale_[1], 1.0 / currentScale_[2]));
          double positive = -1;
          if (newPoint2D[0] - oldPoint2D_[0] + oldPoint2D_[1] - newPoint2D[1] > 0)
            positive = 1;

          Vec2d div = Vec2d(newPoint2D[0], newPoint2D[1]) - Vec2d(oldPoint2D_[0], oldPoint2D_[1]);

          double scaleValue = div.norm() * 3.0 / qMin(_state.context_height(), _state.context_width());
          scaleValue *= positive;
          currentScale_ += Vec3d(scaleValue, scaleValue, scaleValue);

          scale(currentScale_);
        }
      }

      // x axis clicked apply translation along axis
      if (trans[0]) {

        mapToCylinder(_state, oldPoint2D_, old_axis_hit);
        mapToCylinder(_state, newPoint2D, new_axis_hit);

        // translate
        // Get screen coordinates of current mouse position and unproject them
        // into world coordinates. Project onto selected axis and apply resulting
        // vector as translation

        Vec3d d = _state.project(center());
        Vec3d d_origin = _state.project(draggingOrigin3D_);

        // Snap back to origin position if mouse cursor
        // is near enough
        if (abs(d_origin[0] - newPoint2D[0]) < SNAP_PIXEL_TOLERANCE
            && abs(_state.context_height() - d_origin[1] - newPoint2D[1]) < SNAP_PIXEL_TOLERANCE
            && (_event->modifiers() & Qt::AltModifier) ) {
          newPoint2D = oldPoint2D_;
          Vec3d backtrans = draggingOrigin3D_ - center();
          if (mode_ != Resize) {
            translate(backtrans);
          }
        }
        Vec3d oldvec = _state.unproject(Vec3d(oldPoint2D_[0], (_state.context_height() - oldPoint2D_[1]), d[2]));
        Vec3d newvec = _state.unproject(Vec3d(newPoint2D[0], (_state.context_height() - newPoint2D[1]), d[2]));
        Vec3d ntrans = newvec - oldvec;

        // project to current direction
        ntrans = (ntrans | directionX()) * directionX();

        if (mode_ == Resize) {
          //scaling
          double positive = -1;
          if ((directionX() | ntrans) > 0)
            positive = 1;

          if (currentScale_[0] < 0)
            positive *= -1;

          Vec3d proj = _state.project(ntrans + oldvec);

          Vec2d div = Vec2d(proj[0], proj[1]) - Vec2d(oldPoint2D_[0], (_state.context_height() - oldPoint2D_[1]));

          double scaleValue = div.norm() * 3.0 / qMin(_state.context_height(), _state.context_width());
          scaleValue *= positive;

          // revert last scale
          GLMatrixd m = localTransformation_;
          GLMatrixd mi = localTransformation_;
          mi.invert();

          m.scale(Vec3d(1.0 / currentScale_[0], 1.0 / currentScale_[1], 1.0 / currentScale_[2]));
          m *= mi;

          scale(m);

          currentScale_ += Vec3d(scaleValue, 0.0, 0.0);

          m = localTransformation_;
          m.scale(currentScale_);
          m *= mi;

          scale(m);
        } else
          //translation
          translate(ntrans);
      }

      // y axis clicked change translation along axis
      if (trans[1]) {

        mapToCylinder(_state, oldPoint2D_, old_axis_hit);
        mapToCylinder(_state, newPoint2D, new_axis_hit);

        // translate
        // Get screen coordinates of current mouse position and unproject them
        // into world coordinates. Project onto selected axis and apply resulting
        // vector as translation

        Vec3d d = _state.project(center());
        Vec3d d_origin = _state.project(draggingOrigin3D_);

        // Snap back to origin position if mouse cursor
        // is near enough
        if (abs(d_origin[0] - newPoint2D[0]) < SNAP_PIXEL_TOLERANCE
            && abs(_state.context_height() - d_origin[1] - newPoint2D[1]) < SNAP_PIXEL_TOLERANCE
            && (_event->modifiers() & Qt::AltModifier) ) {
          newPoint2D = oldPoint2D_;
          Vec3d backtrans = draggingOrigin3D_ - center();
          if (mode_ != Resize) {
            translate(backtrans);
          }
        }
        Vec3d oldvec = _state.unproject(Vec3d(oldPoint2D_[0], (_state.context_height() - oldPoint2D_[1]), d[2]));
        Vec3d newvec = _state.unproject(Vec3d(newPoint2D[0], (_state.context_height() - newPoint2D[1]), d[2]));
        Vec3d ntrans = newvec - oldvec;

        // project to current direction
        ntrans = (ntrans | directionY()) * directionY();

        if (mode_ == Resize) {
          //scaling
          double positive = -1;
          if ((directionY() | ntrans) > 0)
            positive = 1;

          if (currentScale_[1] < 0)
            positive *= -1;

          Vec3d proj = _state.project(ntrans + oldvec);

          Vec2d div = Vec2d(proj[0], proj[1]) - Vec2d(oldPoint2D_[0], (_state.context_height() - oldPoint2D_[1]));

          double scaleValue = div.norm() * 3.0 / qMin(_state.context_height(), _state.context_width());
          scaleValue *= positive;

          // revert last scale
          GLMatrixd m = localTransformation_;
          GLMatrixd mi = localTransformation_;
          mi.invert();

          m.scale(Vec3d(1.0 / currentScale_[0], 1.0 / currentScale_[1], 1.0 / currentScale_[2]));
          m *= mi;

          scale(m);

          currentScale_ += Vec3d(0.0, scaleValue, 0.0);

          m = localTransformation_;
          m.scale(currentScale_);
          m *= mi;

          scale(m);

        } else
          //translation
          translate(ntrans);
      }

      // z axis clicked change translation along axis
      if (trans[2]) {

        mapToCylinder(_state, oldPoint2D_, old_axis_hit);
        mapToCylinder(_state, newPoint2D, new_axis_hit);

        // translate
        // Get screen coordinates of current mouse position and unproject them
        // into world coordinates. Project onto selected axis and apply resulting
        // vector as translation

        Vec3d d = _state.project(center());
        Vec3d d_origin = _state.project(draggingOrigin3D_);

        // Snap back to origin position if mouse cursor
        // is near enough
        if (abs(d_origin[0] - newPoint2D[0]) < SNAP_PIXEL_TOLERANCE
            && abs(_state.context_height() - d_origin[1] - newPoint2D[1]) < SNAP_PIXEL_TOLERANCE
            && (_event->modifiers() & Qt::AltModifier) ) {
          newPoint2D = oldPoint2D_;
          Vec3d backtrans = draggingOrigin3D_ - center();
          if (mode_ != Resize) {
            translate(backtrans);
          }
        }
        Vec3d oldvec = _state.unproject(Vec3d(oldPoint2D_[0], (_state.context_height() - oldPoint2D_[1]), d[2]));
        Vec3d newvec = _state.unproject(Vec3d(newPoint2D[0], (_state.context_height() - newPoint2D[1]), d[2]));
        Vec3d ntrans = newvec - oldvec;

        // project to current direction
        ntrans = (ntrans | directionZ()) * directionZ();

        if (mode_ == Resize) {
          //scaling
          double positive = -1;
          if ((directionZ() | ntrans) > 0)
            positive = 1;

          if (currentScale_[2] < 0)
            positive *= -1;

          Vec3d proj = _state.project(ntrans + oldvec);

          Vec2d div = Vec2d(proj[0], proj[1]) - Vec2d(oldPoint2D_[0], (_state.context_height() - oldPoint2D_[1]));

          double scaleValue = div.norm() * 3.0 / qMin(_state.context_height(), _state.context_width());
          scaleValue *= positive;

          // revert last scale
          GLMatrixd m = localTransformation_;
          GLMatrixd mi = localTransformation_;
          mi.invert();

          m.scale(Vec3d(1.0 / currentScale_[0], 1.0 / currentScale_[1], 1.0 / currentScale_[2]));
          m *= mi;

          scale(m);

          currentScale_ += Vec3d(0.0, 0.0, scaleValue);

          m = localTransformation_;
          m.scale(currentScale_);
          m *= mi;

          scale(m);

        } else
          //translation
          translate(ntrans);
      }

      // x top clicked: rotate around x axis
      if (rot[0] && (activeRotations_ & X_AXIS)) {

        mapToCylinder(_state, oldPoint2D_, old_axis_hit);
        mapToCylinder(_state, newPoint2D, new_axis_hit);

        Vec2i dist = oldPoint2D_ - newPoint2D;
        int rotation = 0;

        // Rasterize movement if shift is pressed
        if (_event->modifiers() == mods) {
          if (abs(dist[1]) < (int) (_state.viewport_height() / 16)) {
            rotation = 0;
            lockOldPoint = true;
          } else {
            // Rotate exactly 45 degrees
            if (dist[1] < 0)
              rotation = -45;
            else
              rotation = 45;
            ;
          }
        } else {
          rotation = dist[1];
        }

        // Shift has been pressed
        // Rotate manipulator but not parent node
        if (mode_ == LocalRotation) {

          // Update only local rotation
          localTransformation_.rotate(-rotation, directionX(), ACG::MULT_FROM_LEFT);

        } else {
          // Rotate parent node but not manipulator
          _state.push_modelview_matrix();
          _state.translate(center()[0], center()[1], center()[2]);
          update_rotation(_state);

          rotate(rotation, directionX());

          _state.pop_modelview_matrix();
        }

      }

      // y top clicked: rotate around y axis
      // or outer ring on zx axis has been clicked
      if (rot[1] && (activeRotations_ & Y_AXIS)) {

        mapToCylinder(_state, oldPoint2D_, old_axis_hit);
        mapToCylinder(_state, newPoint2D, new_axis_hit);

        Vec2i dist = oldPoint2D_ - newPoint2D;
        int rotation = 0;

        // Rasterize movement if shift is pressed
        if (_event->modifiers() == mods) {
          if (abs(dist[1]) < (int) (_state.viewport_width() / 16)) {
            rotation = 0;
            lockOldPoint = true;
          } else {
            // Rotate exactly 45 degrees
            if (dist[1] < 0)
              rotation = -45;
            else
              rotation = 45;
            ;
          }
        } else {
          rotation = dist[1];
        }

        if (mode_ == LocalRotation) {

          // Update only local rotation
          localTransformation_.rotate(-rotation, directionY(), ACG::MULT_FROM_LEFT);

        } else {
          _state.push_modelview_matrix();
          _state.translate(center()[0], center()[1], center()[2]);

          rotate(rotation, directionY());

          _state.pop_modelview_matrix();
        }
      }

      // z top clicked: rotate around z axis
      if (rot[2] && (activeRotations_ & Z_AXIS)) {

        mapToCylinder(_state, oldPoint2D_, old_axis_hit);
        mapToCylinder(_state, newPoint2D, new_axis_hit);

        Vec2i dist = oldPoint2D_ - newPoint2D;

        int rotation = 0;

        // Rasterize movement if shift is pressed
        if (_event->modifiers() == mods) {
          if (abs(dist[1]) < (int) (_state.viewport_width() / 16)) {
            rotation = 0;
            lockOldPoint = true;
          } else {
            // Rotate exactly 45 degrees
            if (dist[1] < 0)
              rotation = 45;
            else
              rotation = -45;
            ;
          }
        } else {
          rotation = -dist[1];
        }

        if (mode_ == LocalRotation) {

          // Update only local rotation
          localTransformation_.rotate(-rotation, directionZ(), ACG::MULT_FROM_LEFT); // (dist[0]+dist[1])/2

        } else {
          _state.push_modelview_matrix();
          _state.translate(center()[0], center()[1], center()[2]);

          rotate(rotation, directionZ());

          _state.pop_modelview_matrix();
        }

      }

      break;
    }

    default: // avoid warning
      break;
  }

  setDirty();

  // save old Point
  if (!lockOldPoint)
    oldPoint2D_ = newPoint2D;
}


//----------------------------------------------------------------------------

/// Determine whether the origin sphere has been hit
bool TranslationManipulatorNode::hitSphere( GLState& _state,
              const Vec2i& _v2)
{
  // Qt -> GL coordinate systems
  unsigned int x = _v2[0];
  unsigned int y = _state.context_height() - _v2[1];


  // get ray from eye through pixel, in sphere coords
  Vec3d origin, direction;

  _state.set_updateGL(false);
  _state.push_modelview_matrix();

  update_manipulator_system(_state);
  _state.scale(2*manipulator_radius_);

  _state.viewing_ray(x, y, origin, direction);

  _state.pop_modelview_matrix();
  _state.set_updateGL(true);



  // calc sphere-ray intersection
  // (sphere is centered at origin, has radius 1)
  double a = direction.sqrnorm(),
  b = 2.0 * (origin | direction),
  c = origin.sqrnorm() - 1.0,
  d = b*b - 4.0*a*c;

  return (d >= 0.0);
}


//------------------------------------------------------------------------------------


/// Determine whether the outer sphere has been hit by the cursor

bool TranslationManipulatorNode::hitOuterSphere( GLState& _state,
                                 const Vec2i& _v2)
{
  // Qt -> GL coordinate systems
  unsigned int x = _v2[0];
  unsigned int y = _state.context_height() - _v2[1];


  // get ray from eye through pixel, in sphere coords
  Vec3d origin, direction;

  _state.set_updateGL(false);
  _state.push_modelview_matrix();

  update_manipulator_system(_state);
  _state.scale(manipulator_height_+4*manipulator_radius_);

  _state.viewing_ray(x, y, origin, direction);

  _state.pop_modelview_matrix();
  _state.set_updateGL(true);


  // calc sphere-ray intersection
  // (sphere is centered at origin, has radius 1)
  double a = direction.sqrnorm(),
        b = 2.0 * (origin | direction),
        c = origin.sqrnorm() - 1.0,
        d = b*b - 4.0*a*c;

  return (d >= 0.0);
}


//------------------------------------------------------------------------------------


/// If ray from mouse pointer intersects with one
/// of the three axes?
bool
TranslationManipulatorNode::mapToCylinder( GLState&       _state,
                                           const Vec2i&   _v1,
                                           double&        _axis_hit,
                                           StateUpdates   _updateStates )
{
  // Qt -> GL coordinate systems
  unsigned int x = _v1[0];
  unsigned int y = _state.context_height() - _v1[1];


  // get ray from eye through pixel (cylinder coords)
  Vec3d originX, directionX;
  Vec3d originY, directionY;
  Vec3d originZ, directionZ;

  _state.set_updateGL(false);
  _state.push_modelview_matrix();

  // Now that we have 3 different axes we have to test intersection with each of them
  // Z-Axis:
  update_manipulator_system(_state);
  _state.viewing_ray(x, y, originZ, directionZ);

  // Y-Axis:
  _state.rotate(-90, 1.0, 0.0, 0.0);
  _state.viewing_ray(x, y, originY, directionY);

  // X-Axis:
  _state.rotate(90, 0.0, 1.0, 0.0);
  _state.viewing_ray(x, y, originX, directionX);

  _state.pop_modelview_matrix();
  _state.set_updateGL(true);


  // get cylinder axis ray: it's in its own coord system!
  // Viewing ray function unprojects screen coordinates thorugh inverse viewport, projection and modelview
  // such that we end in local coordinate system of the manipulator.
  // local transformation of the manipulator is included in update_manipulator_system which applies the
  // local transformation to the current glstate used by viewing_ray
  const Vec3d origin2(0,0,0),
              cylinderAxis(0.0, 0.0, 1.0); // Always same but we change the coordinate system for the viewer


  // compute pseude-intersection (x axis)
  Vec3d normalX = (directionX % cylinderAxis).normalize();
  Vec3d vdX = ((origin2 - originX) % directionX);
  double axis_hitX = (normalX | vdX);
  double orthodistanceX = fabsf( ( origin2 - originX ) | normalX);

  // compute pseude-intersection (y axis)
  Vec3d normalY = (directionY % cylinderAxis).normalize();
  Vec3d vdY = ((origin2 - originY) % directionY);
  double axis_hitY = (normalY | vdY);
  double orthodistanceY = fabsf( ( origin2 - originY ) | normalY);

  // compute pseude-intersection (z axis)
  Vec3d normalZ = (directionZ % cylinderAxis).normalize();
  Vec3d vdZ = ((origin2 - originZ) % directionZ);
  double axis_hitZ = (normalZ | vdZ);
  double orthodistanceZ = fabsf( ( origin2 - originZ ) | normalZ);

  if ( _updateStates == None )
    return false;

  if ( ( orthodistanceX < manipulator_radius_ ) &&
	    ( axis_hitX      >= 0 )                  &&
	    ( axis_hitX      <= manipulator_height_ ) )
  {

    // z axis has been hit
    _axis_hit = axis_hitX;
    if ( _updateStates == Click)
      element_[XAxis].clicked_ = true;
    else
      element_[XAxis].over_ = true;

  } else if ( ( orthodistanceY < manipulator_radius_ ) &&
	           (   axis_hitY    >= 0 )                  &&
	           (   axis_hitY    <= manipulator_height_))
  {

    // y axis has been hit
    _axis_hit = axis_hitY;
    if ( _updateStates == Click)
      element_[YAxis].clicked_ = true;
    else
      element_[YAxis].over_ = true;

  } else if ( ( orthodistanceZ < manipulator_radius_ ) &&
	           ( axis_hitZ      >= 0 )                  &&
	           ( axis_hitZ      <= manipulator_height_ ) )
  {
    // x axis has been hit
    _axis_hit = axis_hitZ;
    if ( _updateStates == Click)
      element_[ZAxis].clicked_ = true;
    else
      element_[ZAxis].over_ = true;

  } else
	 _axis_hit = 0.0;

  if ( _updateStates == Click)
    return (element_[XAxis].clicked_ || element_[YAxis].clicked_ || element_[ZAxis].clicked_);

  return (element_[XAxis].over_ || element_[YAxis].over_ || element_[ZAxis].over_);
}


//----------------------------------------------------------------------------
/// If ray from mouse pointer intersects with one
/// of the three tops?
// This method treats the top as cylinder
// TODO: Adapt intersection test to cone shape
bool
TranslationManipulatorNode::mapToCylinderTop( GLState&       _state,
                                              const Vec2i&   _v1,
                                              double&        _axis_hit,
                                              StateUpdates   _updateStates )
{
  // Qt -> GL coordinate systems
  unsigned int x = _v1[0];
  unsigned int y = _state.context_height() - _v1[1];


  // get ray from eye through pixel (cylinder coords)
  Vec3d originX, directionX;
  Vec3d originY, directionY;
  Vec3d originZ, directionZ;

  _state.set_updateGL(false);
  _state.push_modelview_matrix();

  // Z-Axis:
  update_manipulator_system(_state);
  _state.translate( 0.0, 0.0, manipulator_height_);
  _state.viewing_ray(x, y, originZ, directionZ);
  _state.translate( 0.0, 0.0, -manipulator_height_);

  // Y-Axis:
  _state.rotate(-90, 1.0, 0.0, 0.0);
  _state.translate(0.0, 0.0 , manipulator_height_ );
  _state.viewing_ray(x, y, originY, directionY);
  _state.translate(0.0, 0.0, -manipulator_height_);

  // X-Axis:
  _state.rotate(90, 0.0, 1.0, 0.0);
  _state.translate(0.0, 0.0, manipulator_height_);
  _state.viewing_ray(x, y, originX, directionX);
  _state.translate(0.0, 0.0, -manipulator_height_);

  _state.pop_modelview_matrix();
  _state.set_updateGL(true);


  // get cylinder axis ray: it's in its own coord system!
  // Viewing ray function unprojects screen coordinates thorugh inverse viewport, projection and modelview
  // such that we end in local coordinate system of the manipulator.
  // local transformation of the manipulator is included in update_manipulator_system which applies the
  // local transformation to the current glstate used by viewing_ray
  const Vec3d origin2(0,0,0),
              cylinderAxis(0.0, 0.0, 1.0);

  // compute pseude-intersection (x axis)
  Vec3d normalX = (directionX % cylinderAxis).normalize();
  Vec3d vdX = ((origin2 - originX) % directionX );
  double axis_hitX = (normalX | vdX);
  double orthodistanceX = fabsf( ( origin2 - originX ) | normalX);

  // compute pseude-intersection (y axis)
  Vec3d normalY = (directionY % cylinderAxis).normalize();
  Vec3d vdY = ((origin2 - originY) % directionY);
  double axis_hitY = (normalY | vdY);
  double orthodistanceY = fabsf( ( origin2 - originY ) | normalY);

  // compute pseude-intersection (z axis)
  Vec3d normalZ = (directionZ % cylinderAxis).normalize();
  Vec3d vdZ = ((origin2 - originZ) % directionZ);
  double axis_hitZ = (normalZ | vdZ);
  double orthodistanceZ = fabsf( ( origin2 - originZ ) | normalZ);

  if ( _updateStates == None )
    return false;

  // TODO: Work with cones :

  if ( ( orthodistanceX <  manipulator_radius_ * 2.0 ) &&
	    ( axis_hitX      >= 0.0 )                       &&
	    ( axis_hitX      <= manipulator_height_ / 2.0 ) )
  {

    // z top has been hit
    _axis_hit = axis_hitX;
    if ( _updateStates == Click)
      element_[XTop].clicked_ = true;
    else
      element_[XTop].over_ = true;

  } else if ( ( orthodistanceY <  manipulator_radius_ * 2.0 ) &&
	           ( axis_hitY      >= 0.0 )                      &&
	           ( axis_hitY      <= manipulator_height_ / 2.0 ) )
  {

    // y top has been hit
    _axis_hit = axis_hitY;
    if ( _updateStates == Click)
      element_[YTop].clicked_ = true;
    else
      element_[YTop].over_ = true;

  } else if ( ( orthodistanceZ <  manipulator_radius_ * 2.0 ) &&
	           ( axis_hitZ      >= 0.0 )                       &&
	           ( axis_hitZ      <= manipulator_height_ / 2.0 ) )
  {

    // x top has been hit
    _axis_hit = axis_hitZ;
    if ( _updateStates == Click)
      element_[ZTop].clicked_ = true;
    else
      element_[ZTop].over_ = true;

  } else
    _axis_hit = 0.0;

  if ( _updateStates == Click)
    return (element_[XTop].clicked_ || element_[YTop].clicked_ || element_[ZTop].clicked_);
  return (element_[XTop].over_ || element_[YTop].over_ || element_[ZTop].over_);
}

//----------------------------------------------------------------------------

/// Determine whether one of the outer rings has been hit.
/// Return true if intersection with either of the rings
/// occurs. _v3 contains the point in world coordinates
bool
TranslationManipulatorNode::mapToSphere( GLState& _state,
					 const Vec2i& _v2,
					 Vec3d& _v3,
				         StateUpdates _updateStates)
{
  // Qt -> GL coordinate systems
  unsigned int x = _v2[0];
  unsigned int y = _state.context_height() - _v2[1];

  // get ray from eye through pixel
  Vec3d originXY, directionXY,
        originYZ, directionYZ,
	originZX, directionZX;

  _state.set_updateGL(false);
  _state.push_modelview_matrix();

  update_manipulator_system(_state);

  _state.viewing_ray(x, y, originXY, directionXY);
  _state.rotate(90, 0.0, 1.0, 0.0);
  _state.viewing_ray(x, y, originYZ, directionYZ);
  _state.rotate(90, 1.0, 0.0, 0.0);
  _state.viewing_ray(x, y, originZX, directionZX);

  _state.pop_modelview_matrix();
  _state.set_updateGL(true);

  // origin + direction * t = (x, y, 0) <=> t = -origin_z / direction_z
  // => Point on xy-plane p = (origin_x + direction_x*t, origin_y + direction_y*t, 0)
  double t1 = -originXY[2]/directionXY[2];
  Vec3d hitPointXY = originXY + directionXY*t1;

  double t2 = -originYZ[2]/directionYZ[2];
  Vec3d hitPointYZ = originYZ + directionYZ*t2;

  double t3 = -originZX[2]/directionZX[2];
  Vec3d hitPointZX = originZX + directionZX*t3;

  // Depth test: Take nearest object
  bool t1_near = false, t2_near = false, t3_near = false;
  if( t1 <= t2 && t1 <= t3)
    t1_near = true;
  if( t2 <= t1 && t2 <= t3)
    t2_near = true;
  if( t3 <= t1 && t3 <= t2)
      t3_near = true;

  bool xy_hit = hitPointXY.length() > 2*manipulator_height_ - manipulator_height_/4.0 &&
	  hitPointXY.length() < 2*manipulator_height_;

  bool yz_hit = hitPointYZ.length() > 2*manipulator_height_ - manipulator_height_/4.0 &&
	  hitPointYZ.length() < 2*manipulator_height_;

  bool zx_hit = hitPointZX.length() > 2*manipulator_height_ - manipulator_height_/4.0 &&
	  hitPointZX.length() < 2*manipulator_height_;


  bool more_than_one_hit = (xy_hit && yz_hit) || (xy_hit && zx_hit) || (yz_hit && zx_hit);
//   std::cerr << 2*manipulator_height_ - manipulator_height_/3.0 << " < " <<
// 	  hitPointXY << " < " << 2*manipulator_height_ << std::endl;
//
//   std::cerr << 2*manipulator_height_ - manipulator_height_/3.0 << " < " <<
// 	  hitPointYZ << " < " << 2*manipulator_height_ << std::endl;
//
//   std::cerr << 2*manipulator_height_ - manipulator_height_/3.0 << " < " <<
// 	  hitPointZX << " < " << 2*manipulator_height_ << std::endl;

  // Test if hit point on x-y plane matches length of
  // manipulator radius_state
  if(xy_hit && (!more_than_one_hit || t1_near))
  {
      // Outer ring on xy plane has been hit
      if ( _updateStates == Click)
        element_[ZRing].clicked_ = true;
      else if ( _updateStates == Over)
        element_[ZRing].over_ = true;
      _v3 = hitPointXY;
      return true;
  }

  else if(yz_hit && (!more_than_one_hit || t2_near))
  {
      // Outer ring on yz plane has been hit
      if ( _updateStates == Click)
        element_[XRing].clicked_ = true;
      else if ( _updateStates == Over)
        element_[XRing].over_ = true;
      _v3 = hitPointYZ;
      return true;
  }

  else if(zx_hit && (!more_than_one_hit || t3_near))
  {
      // Outer ring around zx plane has been hit
      if ( _updateStates == Click)
        element_[YRing].clicked_ = true;
      else if ( _updateStates == Over)
        element_[YRing].over_ = true;
      _v3 = hitPointZX;
      return true;
  }

  return false;
}

//----------------------------------------------------------------------------

void
TranslationManipulatorNode::
pick(GLState& _state, PickTarget _target)
{
  GLenum prev_depth = _state.depthFunc();

  if (_target == PICK_FACE ||
      _target == PICK_ANYTHING) {

    if (draw_manipulator_) {

      updateSize (_state);

      _state.pick_set_maximum(5);

      // Enable depth test but store original status
      glPushAttrib(GL_DEPTH_BUFFER_BIT);
      ACG::GLState::enable(GL_DEPTH_TEST);
      ACG::GLState::depthFunc(GL_LEQUAL);

      // Save modelview matrix
      _state.push_modelview_matrix();

      // Correctly align nodes local coordinate system
      update_manipulator_system(_state);

      _state.pick_set_name(0);
      //================================================================================================
      // Z-Axis
      //================================================================================================
      // gluCylinder draws into z direction so z-Axis first

      axisBottom_->setBottomRadius(manipulator_radius_);
      axisBottom_->setTopRadius(manipulator_radius_);
      axisBottom_->draw(_state, manipulator_height_);

      // Draw Top of z-axis
      _state.translate(0.0, 0.0, manipulator_height_);
      axisTop_->setBottomRadius(manipulator_radius_*2.0);
      axisTop_->setTopRadius(manipulator_radius_*2.0);
      axisTop_->draw(_state, manipulator_height_/2.0);
      _state.translate(0.0, 0.0, -manipulator_height_);

      _state.pick_set_name(1);
      //================================================================================================
      // Y-Axis
      //================================================================================================
      _state.rotate(-90, 1.0, 0.0, 0.0);
      axisBottom_->setBottomRadius(manipulator_radius_);
      axisBottom_->setTopRadius(manipulator_radius_);
      axisBottom_->draw(_state, manipulator_height_);

      // Draw Top of y-axis
      _state.translate(0.0, 0.0, manipulator_height_);
      axisTop_->setBottomRadius(manipulator_radius_*2.0);
      axisTop_->setTopRadius(manipulator_radius_*2.0);
      axisTop_->draw(_state, manipulator_height_/2.0);
      _state.translate(0.0, 0.0, -manipulator_height_);


      _state.pick_set_name(2);
      //================================================================================================
      // X-Axis
      //================================================================================================
      _state.rotate(90, 0.0, 1.0, 0.0);

      axisBottom_->setBottomRadius(manipulator_radius_);
      axisBottom_->setTopRadius(manipulator_radius_);
      axisBottom_->draw(_state, manipulator_height_);

      // Draw Top of x-axis
      _state.translate(0.0, 0.0, manipulator_height_);
      axisTop_->setBottomRadius(manipulator_radius_*2.0);
      axisTop_->setTopRadius(manipulator_radius_*2.0);
      axisTop_->draw(_state, manipulator_height_/2.0);

      _state.translate(0.0, 0.0, -manipulator_height_);

      _state.pick_set_name(3);
      //=================================================================================================
      // Sphere
      //=================================================================================================

      sphere_->draw(_state, manipulator_radius_*2);

      //=================================================================================================
      // Outer-Spheres
      //=================================================================================================

      _state.pick_set_name(4);
      circle_->setInnerRadius(2.0f*manipulator_height_ - manipulator_height_/4.0f);
      circle_->setOuterRadius(2.0f*manipulator_height_);
      if ( activeRotations_ & X_AXIS)
        circle_->draw(_state);

      _state.rotate(90, 0.0, 1.0, 0.0);
      if ( activeRotations_ & Y_AXIS)
        circle_->draw(_state);

      _state.rotate(90, 1.0, 0.0, 0.0);
      if ( activeRotations_ & Z_AXIS)
        circle_->draw(_state);

      // Restore old attributes and modelview
      glPopAttrib();
      _state.pop_modelview_matrix();

      //restore original depth comparison function
      ACG::GLState::depthFunc(prev_depth);
    }
  }
}


//----------------------------------------------------------------------------

void
TranslationManipulatorNode::set_direction(const Vec3d& _directionX, const Vec3d& _directionY)
{

  localTransformation_.identity();

  const Vec3d cross = _directionX % _directionY;

  localTransformation_(0,0) = _directionX[0];
  localTransformation_(1,0) = _directionX[1];
  localTransformation_(2,0) = _directionX[2];
  localTransformation_(3,0) = 0.0;

  localTransformation_(0,1) = _directionY[0];
  localTransformation_(1,1) = _directionY[1];
  localTransformation_(2,1) = _directionY[2];
  localTransformation_(3,1) = 0.0;

  localTransformation_(0,2) = cross[0];
  localTransformation_(1,2) = cross[1];
  localTransformation_(2,2) = cross[2];
  localTransformation_(3,2) = 0.0;

}

//----------------------------------------------------------------------------


Vec3d
TranslationManipulatorNode::directionX() const
{
  return MathTools::sane_normalized( localTransformation_.transform_vector(dirX_) );
}

//----------------------------------------------------------------------------


Vec3d
TranslationManipulatorNode::directionY() const
{
  return MathTools::sane_normalized( localTransformation_.transform_vector(dirY_) );
}

//----------------------------------------------------------------------------


Vec3d
TranslationManipulatorNode::directionZ() const
{
  return MathTools::sane_normalized(localTransformation_.transform_vector(dirZ_));
}

//----------------------------------------------------------------------------

double TranslationManipulatorNode::get_screen_length (GLState& _state, Vec3d& _point) const
{
  Vec3d proj = _state.project (_point);
  proj[0] += 1.0;
  Vec3d uproj = _state.unproject (proj);
  uproj -= _point;
  return uproj.length ();
}

//----------------------------------------------------------------------------

void TranslationManipulatorNode::updateSize (GLState& _state)
{
  if (auto_size_ != TranslationManipulatorNode::Never)
  {
    Vec3d point = localTransformation_.transform_point(Vec3d (0.0, 0.0, 0.0));

    int tmp, width, height;

    _state.get_viewport (tmp, tmp, width, height);

    auto_size_length_ = get_screen_length (_state, point) * (width + height) * 0.02;

    if (auto_size_ == TranslationManipulatorNode::Once)
      auto_size_ = TranslationManipulatorNode::Never;
  }

  manipulator_radius_ = set_manipulator_radius_ * auto_size_length_;
  manipulator_height_ = set_manipulator_height_ * auto_size_length_;
}

//----------------------------------------------------------------------------


void TranslationManipulatorNode::boundingBox( Vec3d & _bbMin, Vec3d & _bbMax )
{
  if (!draw_manipulator_)
    return;

  float r = 2 * manipulator_height_;

  _bbMin.minimize(center()+Vec3d(-r,-r,-r));
  _bbMax.maximize(center()+Vec3d(r,r,r));
}

//----------------------------------------------------------------------------

void TranslationManipulatorNode::setMode (ManipulatorMode _mode)
{
  if (mode_ != _mode)
    ignoreTime_ = true;
  mode_ = _mode;
  setDirty ();
}

//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
