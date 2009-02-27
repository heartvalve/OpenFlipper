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
//  CLASS TranslationManipulatorNode - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================


#include "TranslationManipulatorNode.hh"
#include "../Math/GLMatrixT.hh"
#include "../GL/gl.hh"

#include <QMouseEvent>
#include <QEvent>
#include <float.h>

#include <Math_Tools/Math_Tools.hh>
#include <OpenMesh/Core/Geometry/MathDefs.hh>

//== NAMESPACES ===============================================================


namespace ACG {
namespace SceneGraph {


//== IMPLEMENTATION ==========================================================


  const Vec4f origin_color (0.8, 0.8, 0.8, 1.0);
  const Vec4f outer_ring_x_color (0.8, 0.0, 0.0, 1.0);
  const Vec4f outer_ring_y_color (0.0, 0.8, 0.0, 1.0);
  const Vec4f outer_ring_z_color (0.0, 0.0, 0.8, 1.0);
  const Vec4f x_axis_color (1.0, 0.0, 0.0, 1.0);
  const Vec4f y_axis_color (0.0, 1.0, 0.0, 1.0);
  const Vec4f z_axis_color (0.2, 0.2, 1.0, 1.0);
  const Vec4f select_color (1.0, 0.1, 0.1, 1.0);
  const Vec4f nonactive_color (0.4, 0.4, 0.4, 0.9);


//----------------------------------------------------------------------------


TranslationManipulatorNode::
TranslationManipulatorNode( BaseNode* _parent, const std::string& _name )
  : TransformNode(_parent, _name),
    draw_manipulator_(false),
    dirX_(1.0,0.0,0.0),
    dirY_(0.0,1.0,0.0),
    dirZ_(0.0,0.0,1.0),
    axis_(0),
    manipulator_radius_(20.0),
    manipulator_height_(20),
    manipulator_slices_(10),
    manipulator_stacks_(10),
    axis_x_clicked_(false),
    axis_y_clicked_(false),
    axis_z_clicked_(false),
    top_x_clicked_(false),
    top_y_clicked_(false),
    top_z_clicked_(false),
    outer_ring_xy_clicked_(false),
    outer_ring_yz_clicked_(false),
    outer_ring_zx_clicked_(false),
    any_axis_clicked_(false),
    any_top_clicked_(false),
    origin_clicked_(false),
    outer_ring_clicked_(false),
    axis_x_over_(false),
    axis_y_over_(false),
    axis_z_over_(false),
    top_x_over_(false),
    top_y_over_(false),
    top_z_over_(false),
    outer_ring_xy_over_(false),
    outer_ring_yz_over_(false),
    outer_ring_zx_over_(false),
    any_axis_over_(false),
    any_top_over_(false),
    origin_over_(false),
    outer_ring_over_(false)
{
  localTransformation_.identity();
  axis_ = gluNewQuadric();
  setTraverseMode( BaseNode::ChildrenFirst );

  updateTargetColors ();
  for (unsigned int i = 0; i < TranslationManipulatorNode::NumElements; i++)
  {
    active_current_color[i] = active_target_color[i];
    inactive_current_color[i] = inactive_target_color[i];
  }

}


//----------------------------------------------------------------------------

TranslationManipulatorNode::
~TranslationManipulatorNode()
{
  if (axis_)
    gluDeleteQuadric(axis_);
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
  _state.scale(1.0/scaling()[0], 1.0/scaling()[1], 1.0/scaling()[2]); // Adapt scaling

  update_rotation(_state);
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
  // set colors according to current (clicked/over/none) state
  if(origin_clicked_){
    active_target_color[Origin] = origin_color;
    inactive_target_color[Origin] = nonactive_color;
  } else if(origin_over_){
    active_target_color[Origin] = origin_color * 0.8;
    inactive_target_color[Origin] = nonactive_color * 0.8;
  } else {
    active_target_color[Origin] = origin_color * 0.2;
    inactive_target_color[Origin] = nonactive_color * 0.2;
  }

  if(axis_x_clicked_ || top_x_clicked_){
    active_target_color[XAxis] = x_axis_color;
    inactive_target_color[XAxis] = nonactive_color;
  } else if(axis_x_over_ || top_x_over_){
    active_target_color[XAxis] = x_axis_color * 0.8;
    inactive_target_color[XAxis] = nonactive_color * 0.8;
  } else {
    active_target_color[XAxis] = x_axis_color * 0.2;
    inactive_target_color[XAxis] = nonactive_color * 0.2;
  }
  if(axis_y_clicked_ || top_y_clicked_){
    active_target_color[YAxis] = y_axis_color;
    inactive_target_color[YAxis] = nonactive_color;
  } else if(axis_y_over_ || top_y_over_){
    active_target_color[YAxis] = y_axis_color * 0.8;
    inactive_target_color[YAxis] = nonactive_color * 0.8;
  } else {
    active_target_color[YAxis] = y_axis_color * 0.2;
    inactive_target_color[YAxis] = nonactive_color * 0.2;
  }
  if(axis_z_clicked_ || top_z_clicked_){
    active_target_color[ZAxis] = z_axis_color;
    inactive_target_color[ZAxis] = nonactive_color;
  } else if(axis_z_over_ || top_z_over_){
    active_target_color[ZAxis] = z_axis_color * 0.8;
    inactive_target_color[ZAxis] = nonactive_color * 0.8;
  } else {
    active_target_color[ZAxis] = z_axis_color * 0.2;
    inactive_target_color[ZAxis] = nonactive_color * 0.2;
  }

  if(outer_ring_yz_clicked_){
    active_target_color[YZRing] = outer_ring_x_color;
    inactive_target_color[YZRing] = nonactive_color;
  } else if(outer_ring_yz_over_){
    active_target_color[YZRing] = outer_ring_x_color * 0.8;
    inactive_target_color[YZRing] = nonactive_color * 0.8;
  } else {
    active_target_color[YZRing] = outer_ring_x_color * 0.4;
    inactive_target_color[YZRing] = nonactive_color * 0.4;
  }
  if(outer_ring_zx_clicked_){
    active_target_color[ZXRing] = outer_ring_y_color;
    inactive_target_color[ZXRing] = nonactive_color;
  } else if(outer_ring_zx_over_){
    active_target_color[ZXRing] = outer_ring_y_color * 0.8;
    inactive_target_color[ZXRing] = nonactive_color * 0.8;
  } else {
    active_target_color[ZXRing] = outer_ring_y_color * 0.4;
    inactive_target_color[ZXRing] = nonactive_color * 0.4;
  }
  if(outer_ring_xy_clicked_){
    active_target_color[XYRing] = outer_ring_z_color;
    inactive_target_color[XYRing] = nonactive_color;
  } else if(outer_ring_xy_over_){
    active_target_color[XYRing] = outer_ring_z_color * 0.8;
    inactive_target_color[XYRing] = nonactive_color * 0.8;
  } else {
    active_target_color[XYRing] = outer_ring_z_color * 0.4;
    inactive_target_color[XYRing] = nonactive_color * 0.4;
  }

}

//----------------------------------------------------------------------------

bool TranslationManipulatorNode::updateCurrentColors (GLState& _state)
{
    bool rv = false;

    float value = (float)_state.msSinceLastRedraw () / 1000.0;

    for (unsigned int i = 0; i < TranslationManipulatorNode::NumElements; i++)
    {
	Vec4f diff = active_target_color[i] - active_current_color[i];
	for (unsigned int j = 0; j < 4; j++)
	    if (diff[j] > value)
		diff[j] = value;
	    else if (diff[j] < -value)
		diff[j] = -value;
	active_current_color[i] += diff;
	diff = inactive_target_color[i] - inactive_current_color[i];
	for (unsigned int j = 0; j < 4; j++)
	    if (diff[j] > value)
		diff[j] = value;
	    else if (diff[j] < -value)
		diff[j] = -value;
	inactive_current_color[i] += diff;

	rv |= active_target_color[i] != active_current_color[i] ||
	      inactive_target_color[i] != inactive_current_color[i];
    }
    return rv;
}

//----------------------------------------------------------------------------

void TranslationManipulatorNode::drawMaipulator (GLState& _state, bool _active)
{
  glPushAttrib(GL_ENABLE_BIT );
  glDisable( GL_CULL_FACE );

  // Save modelview matrix
  _state.push_modelview_matrix();

  //================================================================================================
  // Sphere
  //================================================================================================

//     glDisable(GL_ALPHA_TEST);
//     glEnable(GL_BLEND);
//     glEnable(GL_CULL_FACE);
//     glColorMaterial ( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE ) ;
//     glEnable(GL_COLOR_MATERIAL);
//
//     glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_DST_ALPHA);
//
//     glColor4f( 1.0,0.0,0.0,0.3);
//     gluSphere( axis_, manipulator_height_ * 1.5 ,  manipulator_slices_ * 2, manipulator_stacks_ * 2);
//     glDisable(GL_COLOR_MATERIAL);
//
//
//     glEnable(GL_ALPHA_TEST);
//     glDisable(GL_BLEND);

  //================================================================================================
  // Z-Axis
  //================================================================================================
  // gluCylinder draws into z direction so z-Axis first

  if (_active)
  {
    _state.set_diffuse_color(active_current_color[ZAxis]);
    _state.set_specular_color(active_current_color[ZAxis]);
  } else {
    _state.set_diffuse_color(inactive_current_color[ZAxis]);
    _state.set_specular_color(inactive_current_color[ZAxis]);
  }

  gluCylinder(axis_,
              manipulator_radius_,
              manipulator_radius_,
              manipulator_height_,
              manipulator_slices_,
              manipulator_stacks_);

  // Draw Top of z-axis

  _state.translate(0.0, 0.0, manipulator_height_);
  gluCylinder(axis_,
              manipulator_radius_*2,
              0,
              manipulator_height_/2,
              manipulator_slices_,
              manipulator_stacks_);
  _state.translate(0.0, 0.0, -manipulator_height_);

  //================================================================================================
  // Y-Axis
  //================================================================================================
  _state.rotate(-90, 1.0, 0.0, 0.0);
  if (_active)
  {
    _state.set_diffuse_color(active_current_color[YAxis]);
    _state.set_specular_color(active_current_color[YAxis]);
  } else {
    _state.set_diffuse_color(inactive_current_color[YAxis]);
    _state.set_specular_color(inactive_current_color[YAxis]);
  }

  gluCylinder(axis_,
              manipulator_radius_,
              manipulator_radius_,
              manipulator_height_,
              manipulator_slices_,
              manipulator_stacks_);

  // Draw Top of y-axis
  _state.translate(0.0, 0.0, manipulator_height_);
  gluCylinder(axis_,
              manipulator_radius_*2,
              0,
              manipulator_height_/2,
              manipulator_slices_,
              manipulator_stacks_);
  _state.translate(0.0, 0.0, -manipulator_height_);


  //================================================================================================
  // X-Axis
  //================================================================================================
  _state.rotate(90, 0.0, 1.0, 0.0);
  if (_active)
  {
    _state.set_diffuse_color(active_current_color[XAxis]);
    _state.set_specular_color(active_current_color[XAxis]);
  } else {
    _state.set_diffuse_color(inactive_current_color[XAxis]);
    _state.set_specular_color(inactive_current_color[XAxis]);
  }

  gluCylinder(axis_,
              manipulator_radius_,
              manipulator_radius_,
              manipulator_height_,
              manipulator_slices_,
              manipulator_stacks_);

  // Draw Top of x-axis
  _state.translate(0.0, 0.0, manipulator_height_);
  gluCylinder(axis_,
              manipulator_radius_*2,
              0,
              manipulator_height_/2,
              manipulator_slices_,
              manipulator_stacks_);
  _state.translate(0.0, 0.0, -manipulator_height_);

  //=================================================================================================
  // Sphere
  //=================================================================================================
  if (_active)
  {
    _state.set_diffuse_color(active_current_color[Origin]);
    _state.set_specular_color(active_current_color[Origin]);
  } else {
    _state.set_diffuse_color(inactive_current_color[Origin]);
    _state.set_specular_color(inactive_current_color[Origin]);
  }

  gluSphere( gluNewQuadric(), manipulator_radius_*2, manipulator_slices_, manipulator_stacks_ );

  //=================================================================================================
  // Outer-Rings
  //=================================================================================================

  glPushAttrib(GL_LIGHTING_BIT);
  glDisable(GL_LIGHTING);

  if (_active)
  {
    glColor4f(active_current_color[YZRing][0],
	      active_current_color[YZRing][1],
	      active_current_color[YZRing][2],
	      active_current_color[YZRing][3]);
  } else {
    glColor4f(inactive_current_color[YZRing][0],
	      inactive_current_color[YZRing][1],
	      inactive_current_color[YZRing][2],
	      inactive_current_color[YZRing][3]);
  }

  drawCircle(2*manipulator_height_, 2*manipulator_height_ - manipulator_height_/4.0);

  if (_active)
  {
    glColor4f(active_current_color[ZXRing][0],
	      active_current_color[ZXRing][1],
	      active_current_color[ZXRing][2],
	      active_current_color[ZXRing][3]);
  } else {
    glColor4f(inactive_current_color[ZXRing][0],
	      inactive_current_color[ZXRing][1],
	      inactive_current_color[ZXRing][2],
	      inactive_current_color[ZXRing][3]);
  }

  _state.rotate(90, 0.0, 1.0, 0.0);
  drawCircle(2*manipulator_height_, 2*manipulator_height_ - manipulator_height_/4.0);

  if (_active)
  {
    glColor4f(active_current_color[XYRing][0],
	      active_current_color[XYRing][1],
	      active_current_color[XYRing][2],
	      active_current_color[XYRing][3]);
  } else {
    glColor4f(inactive_current_color[XYRing][0],
	      inactive_current_color[XYRing][1],
	      inactive_current_color[XYRing][2],
	      inactive_current_color[XYRing][3]);
  }

  _state.rotate(90, 1.0, 0.0, 0.0);
  drawCircle(2*manipulator_height_, 2*manipulator_height_ - manipulator_height_/4.0);

  glPopAttrib(); // ENABLE_BIT
  glPopAttrib(); // LIGHTING_BIT


  _state.pop_modelview_matrix();
}

//----------------------------------------------------------------------------


void
TranslationManipulatorNode::draw(GLState& _state, unsigned int /* _drawMode */ )
{
   if (draw_manipulator_) {
    // Store lighting bits and enable lighting
    glPushAttrib(GL_LIGHTING_BIT);
    glEnable(GL_LIGHTING);
    glShadeModel(GL_SMOOTH);

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

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_GREATER);
    glDepthMask(GL_FALSE);
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    drawMaipulator(_state, false);
    glDisable (GL_BLEND);

    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_TRUE);
    drawMaipulator(_state, true);

    // Restore old attributes and modelview
    glPopAttrib();
    glPopAttrib();
    glPopAttrib();
    glPopAttrib();

    // restore active colors
    _state.set_diffuse_color(backup_diffuse);
    _state.set_specular_color(backup_specular);
  }
}


//----------------------------------------------------------------------------

/// Draws a circle with given radius
void
TranslationManipulatorNode::drawCircle(const float outerRadius, const float innerRadius)
{
  gluDisk(gluNewQuadric(), innerRadius, outerRadius, 30, 30);
}

//----------------------------------------------------------------------------

// void
// TranslationManipulatorNode::leave(GLState& _state, unsigned int /* _drawMode */ )
// {

// }

//----------------------------------------------------------------------------

void
TranslationManipulatorNode::mouseEvent(GLState& _state, QMouseEvent* _event)
{
  if(!draw_manipulator_) return; // Avoid transformation of object while transformator is invisible

  Vec3d         oldPoint3D;
  Vec2i         newPoint2D(_event->pos().x(), _event->pos().y());
  Vec3d         newPoint3D;
  double        old_axis_hit, new_axis_hit, new_axis_over;


  switch (_event->type()) {

    // If mouse is pressed check if any part of the manipulator is hit and store that state
    case QEvent::MouseButtonPress:
    {
      // hit origin ?
      origin_clicked_ = hitSphere(_state, newPoint2D);

      // hit any top ?
      any_top_clicked_ = mapToCylinderTop(_state, newPoint2D, new_axis_hit, Click);

      // hit any axis ?
      any_axis_clicked_ = mapToCylinder(_state, newPoint2D, new_axis_hit, Click);

      // hit one of the outer rings ?
      outer_ring_clicked_ = mapToSphere(_state, newPoint2D, newPoint3D, Click);

      // origin has been hit, ignore all other parts
      if ( origin_clicked_ ) {
        axis_x_clicked_     = false;
        axis_y_clicked_     = false;
        axis_z_clicked_     = false;
        top_x_clicked_      = false;
        top_y_clicked_      = false;
        top_z_clicked_      = false;
        outer_ring_xy_clicked_ = false;
        outer_ring_yz_clicked_ = false;
        outer_ring_zx_clicked_ = false;
        any_axis_clicked_   = false;
        any_top_clicked_    = false;
        outer_ring_clicked_ = false;
      } else if ( any_top_clicked_ ) { // tops override the axes
        axis_x_clicked_   = false;
        axis_y_clicked_   = false;
        axis_z_clicked_   = false;
        any_axis_clicked_ = false;
        outer_ring_xy_clicked_ = false;
        outer_ring_yz_clicked_ = false;
        outer_ring_zx_clicked_ = false;
        origin_clicked_   = false;
        outer_ring_clicked_ = false;
      } else if ( any_axis_clicked_ ) { // axes have been hit, disable rest ... should not be required
        top_x_clicked_    = false;
        top_y_clicked_    = false;
        top_z_clicked_    = false;
        outer_ring_xy_clicked_ = false;
        outer_ring_yz_clicked_ = false;
        outer_ring_zx_clicked_ = false;
        any_top_clicked_  = false;
        origin_clicked_   = false;
        outer_ring_clicked_ = false;
      } else if ( outer_ring_clicked_ ) { // Nothing except the outer ring has been hit
        axis_x_clicked_     = false;
        axis_y_clicked_     = false;
        axis_z_clicked_     = false;
        top_x_clicked_      = false;
        top_y_clicked_      = false;
        top_z_clicked_      = false;
        any_axis_clicked_   = false;
        any_top_clicked_    = false;
        origin_clicked_     = false;
      }

      // Reset local transformation:
      if( _event->modifiers() & Qt::ControlModifier && _event->modifiers() & Qt::AltModifier) {
        localTransformation_.identity();
      }

      oldPoint2D_   = newPoint2D;
      break;
    }


    // Reset all states as we stopped manipulating
    case QEvent::MouseButtonRelease:
    {

      origin_clicked_     = false;
      axis_x_clicked_     = false;
      axis_y_clicked_     = false;
      axis_z_clicked_     = false;
      top_x_clicked_      = false;
      top_y_clicked_      = false;
      top_z_clicked_      = false;
      any_axis_clicked_   = false;
      any_top_clicked_    = false;
      outer_ring_clicked_ = false;
      outer_ring_xy_clicked_ = false;
      outer_ring_yz_clicked_ = false;
      outer_ring_zx_clicked_ = false;
      break;
    }


    case QEvent::MouseButtonDblClick:
    {
      draw_manipulator_ = !draw_manipulator_;
      break;
    }


    // All real manipulation is done here
    case QEvent::MouseMove:
    {
      if(!draw_manipulator_) return; // Avoid manipulation if manipulator is invisible

      origin_over_     = false;
      axis_x_over_     = false;
      axis_y_over_     = false;
      axis_z_over_     = false;
      top_x_over_      = false;
      top_y_over_      = false;
      top_z_over_      = false;
      any_axis_over_   = false;
      any_top_over_    = false;
      outer_ring_over_ = false;
      outer_ring_xy_over_ = false;
      outer_ring_yz_over_ = false;
      outer_ring_zx_over_ = false;

      if (!(origin_clicked_ || any_top_clicked_ || any_axis_clicked_ ||
	    outer_ring_clicked_))
      {
        // over origin ?
        origin_over_ = hitSphere(_state, newPoint2D);

        // over any top ?
        any_top_over_ = mapToCylinderTop(_state, newPoint2D, new_axis_over, Over);

        // over any axis ?
        any_axis_over_ = mapToCylinder(_state, newPoint2D, new_axis_over, Over);

        // over one of the outer rings ?
        outer_ring_over_ = mapToSphere(_state, newPoint2D, newPoint3D, Over);

        // origin has been hit, ignore all other parts
        if ( origin_over_ ) {
          axis_x_over_     = false;
          axis_y_over_     = false;
          axis_z_over_     = false;
          top_x_over_      = false;
          top_y_over_      = false;
          top_z_over_      = false;
          outer_ring_xy_over_ = false;
          outer_ring_yz_over_ = false;
          outer_ring_zx_over_ = false;
          any_axis_over_   = false;
          any_top_over_    = false;
          outer_ring_over_ = false;
        } else if ( any_top_over_ ) { // tops override the axes
          axis_x_over_   = false;
          axis_y_over_   = false;
          axis_z_over_   = false;
          any_axis_over_ = false;
          outer_ring_xy_over_ = false;
          outer_ring_yz_over_ = false;
          outer_ring_zx_over_ = false;
          origin_over_   = false;
          outer_ring_over_ = false;
        } else if ( any_axis_over_ ) { // axes have been hit, disable rest ... should not be required
          top_x_over_    = false;
          top_y_over_    = false;
          top_z_over_    = false;
          outer_ring_xy_over_ = false;
          outer_ring_yz_over_ = false;
          outer_ring_zx_over_ = false;
          any_top_over_  = false;
          origin_over_   = false;
          outer_ring_over_ = false;
        } else if ( outer_ring_over_ ) { // Nothing except the outer ring has been hit
          axis_x_over_     = false;
          axis_y_over_     = false;
          axis_z_over_     = false;
          top_x_over_      = false;
          top_y_over_      = false;
          top_z_over_      = false;
          any_axis_over_   = false;
          any_top_over_    = false;
          origin_over_     = false;
        }
      }

      // If origin was clicked on
      if(origin_clicked_) {

        // translate along screen plane ( unproject to get world coords for translation and apply them )
        Vec3d d = _state.project(center());
        Vec3d oldvec = _state.unproject(Vec3d(oldPoint2D_[0], (_state.context_height() - oldPoint2D_[1]), d[2]));
        Vec3d newvec = _state.unproject(Vec3d(newPoint2D[0], (_state.context_height() - newPoint2D[1]), d[2]));
        Vec3d ntrans = newvec - oldvec;

        translate(ntrans);
      }


      // x axis clicked apply translation along axis
      if(axis_x_clicked_){

        mapToCylinder(_state, oldPoint2D_, old_axis_hit);
        mapToCylinder(_state, newPoint2D,  new_axis_hit);

        // translate
        // Get screen coordinates of current mouse position and unproject them
        // into world coordinates. Project onto selected axis and apply resulting
        // vector as translation

        Vec3d d = _state.project(center());
        Vec3d oldvec = _state.unproject(Vec3d(oldPoint2D_[0], (_state.context_height() - oldPoint2D_[1]), d[2]));
        Vec3d newvec = _state.unproject(Vec3d(newPoint2D[0], (_state.context_height() - newPoint2D[1]), d[2]));
        Vec3d ntrans = newvec - oldvec;

        // project to current direction
        ntrans       = ( ntrans | directionX() ) * directionX();

        if ( _event->modifiers() & Qt::ShiftModifier ){
          //scaling
          double positive = -1;
          if ( (directionX() | ntrans) > 0 ) positive = 1;

          if (_event->modifiers() & Qt::ControlModifier){
            //non-uniform
//             Vec3d sfactor;
//             sfactor[0] = 1.0;
//             sfactor[1] = 1.0;
//             sfactor[2] = 1.0;

            Vec3d xscale;
            xscale[0] = positive * ntrans.norm();
            xscale[1] = 0.0;
            xscale[2] = 0.0;

            xscale = localTransformation_.transform_vector(xscale);

            _state.push_modelview_matrix();
            _state.translate(center()[0], center()[1], center()[2]);

            scale( Vec3d(1.0, 1.0, 1.0) + xscale );

            _state.pop_modelview_matrix();

          }else
            //uniform
            scale( 1.0 + positive * ntrans.norm() );
        }else
          //translation
          translate(ntrans);
      }

      // y axis clicked change translation along axis
      if(axis_y_clicked_){

        mapToCylinder(_state, oldPoint2D_, old_axis_hit);
        mapToCylinder(_state, newPoint2D,  new_axis_hit);

        // translate
        // Get screen coordinates of current mouse position and unproject them
        // into world coordinates. Project onto selected axis and apply resulting
        // vector as translation

        Vec3d d = _state.project(center());
        Vec3d oldvec = _state.unproject(Vec3d(oldPoint2D_[0], (_state.context_height() - oldPoint2D_[1]), d[2]));
        Vec3d newvec = _state.unproject(Vec3d(newPoint2D[0], (_state.context_height() - newPoint2D[1]), d[2]));
        Vec3d ntrans = newvec - oldvec;

        // project to current direction
        ntrans       = ( ntrans | directionY() ) * directionY();

        if ( _event->modifiers() & Qt::ShiftModifier ){
          //scaling
          double positive = -1;
          if ( (directionY() | ntrans) > 0 ) positive = 1;

          if (_event->modifiers() & Qt::ControlModifier){
            //non-uniform
            Vec3d yscale;
            yscale[0] = 0.0;
            yscale[1] = positive * ntrans.norm();
            yscale[2] = 0.0;

            yscale = localTransformation_.transform_vector(yscale);

            _state.push_modelview_matrix();
            _state.translate(center()[0], center()[1], center()[2]);

            scale( Vec3d(1.0, 1.0, 1.0) + yscale );

            _state.pop_modelview_matrix();
          }else
            //uniform
            scale( 1.0 + positive * ntrans.norm() );
        }else
          //translation
          translate(ntrans);
      }

      // z axis clicked change translation along axis
      if(axis_z_clicked_){

        mapToCylinder(_state, oldPoint2D_, old_axis_hit);
        mapToCylinder(_state, newPoint2D,  new_axis_hit);

        // translate
        // Get screen coordinates of current mouse position and unproject them
        // into world coordinates. Project onto selected axis and apply resulting
        // vector as translation

        Vec3d d = _state.project(center());
        Vec3d oldvec = _state.unproject(Vec3d(oldPoint2D_[0], (_state.context_height() - oldPoint2D_[1]), d[2]));
        Vec3d newvec = _state.unproject(Vec3d(newPoint2D[0], (_state.context_height() - newPoint2D[1]), d[2]));
        Vec3d ntrans = newvec - oldvec;

        // project to current direction
        ntrans       = ( ntrans | directionZ() ) * directionZ();

        if ( _event->modifiers() & Qt::ShiftModifier ){
          //scaling
          double positive = -1;
          if ( (directionZ() | ntrans) > 0 ) positive = 1;

          if (_event->modifiers() & Qt::ControlModifier){
            //non-uniform
            Vec3d zscale;
            zscale[0] = 0.0;
            zscale[1] = 0.0;
            zscale[2] = positive * ntrans.norm();

            zscale = localTransformation_.transform_vector(zscale);

            _state.push_modelview_matrix();
            _state.translate(center()[0], center()[1], center()[2]);

            scale( Vec3d(1.0, 1.0, 1.0) + zscale );

            _state.pop_modelview_matrix();
          }else
            //uniform
            scale( 1.0 + positive * ntrans.norm() );
        }else
          //translation
          translate(ntrans);
      }

      // x top clicked: rotate around x axis
      if( top_x_clicked_ || outer_ring_yz_clicked_) {
        mapToCylinder(_state, oldPoint2D_, old_axis_hit);
        mapToCylinder(_state, newPoint2D,  new_axis_hit);


        Vec2i dist = oldPoint2D_ - newPoint2D;

        // Shift has been pressed
        // Rotate manipulator but not parent node
        if( _event->modifiers() & Qt::ShiftModifier ) {

          // Update only local rotation
          localTransformation_.rotate( -dist[1], directionX(),ACG::MULT_FROM_LEFT);

        } else {
          // Rotate parent node but not manipulator
          _state.push_modelview_matrix();
          _state.translate(center()[0], center()[1], center()[2]);
          update_rotation(_state);

          rotate(dist[1], directionX() );

          _state.pop_modelview_matrix();
        }

      }

      // y top clicked: rotate around y axis
      // or outer ring on zx axis has been clicked
      if( top_y_clicked_ || outer_ring_zx_clicked_) {
        mapToCylinder(_state, oldPoint2D_, old_axis_hit);
        mapToCylinder(_state, newPoint2D,  new_axis_hit);

        Vec2i dist = oldPoint2D_ - newPoint2D;


        if( _event->modifiers() & Qt::ShiftModifier ){

          // Update only local rotation
          localTransformation_.rotate( -dist[0], directionY(),ACG::MULT_FROM_LEFT);

        } else {
          _state.push_modelview_matrix();
          _state.translate(center()[0], center()[1], center()[2]);

          rotate(dist[0], directionY() );

          _state.pop_modelview_matrix();
        }
      }

      // z top clicked: rotate around z axis
      if( top_z_clicked_ || outer_ring_xy_clicked_) {
        mapToCylinder(_state, oldPoint2D_, old_axis_hit);
        mapToCylinder(_state, newPoint2D,  new_axis_hit);


        Vec2i dist = oldPoint2D_ - newPoint2D;

        if( _event->modifiers() & Qt::ShiftModifier ){

          // Update only local rotation
          localTransformation_.rotate( (dist[0]+dist[1])/2, directionZ(),ACG::MULT_FROM_LEFT);

        } else{
          _state.push_modelview_matrix();
          _state.translate(center()[0], center()[1], center()[2]);

          rotate((dist[0]+dist[1])/2, directionZ());

          _state.pop_modelview_matrix();
        }

      }

      break;
    }

    default: // avoid warning
      break;
  }

  setDirty ();

  // save old Point
  oldPoint2D_   = newPoint2D;
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
      axis_x_clicked_ = true;
    else
      axis_x_over_ = true;

  } else if ( ( orthodistanceY < manipulator_radius_ ) &&
	           (   axis_hitY    >= 0 )                  &&
	           (   axis_hitY    <= manipulator_height_))
  {

    // y axis has been hit
    _axis_hit = axis_hitY;
    if ( _updateStates == Click)
      axis_y_clicked_ = true;
    else
      axis_y_over_ = true;

  } else if ( ( orthodistanceZ < manipulator_radius_ ) &&
	           ( axis_hitZ      >= 0 )                  &&
	           ( axis_hitZ      <= manipulator_height_ ) )
  {
    // x axis has been hit
    _axis_hit = axis_hitZ;
    if ( _updateStates == Click)
      axis_z_clicked_ = true;
    else
      axis_z_over_ = true;

  } else
	 _axis_hit = 0.0;

  if ( _updateStates == Click)
    return (axis_x_clicked_ || axis_y_clicked_ || axis_z_clicked_);

  return (axis_x_over_ || axis_y_over_ || axis_z_over_);
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
      top_x_clicked_ = true;
    else
      top_x_over_ = true;

  } else if ( ( orthodistanceY <  manipulator_radius_ * 2.0 ) &&
	           ( axis_hitY      >= 0.0 )                      &&
	           ( axis_hitY      <= manipulator_height_ / 2.0 ) )
  {

    // y top has been hit
    _axis_hit = axis_hitY;
    if ( _updateStates == Click)
      top_y_clicked_ = true;
    else
      top_y_over_ = true;

  } else if ( ( orthodistanceZ <  manipulator_radius_ * 2.0 ) &&
	           ( axis_hitZ      >= 0.0 )                       &&
	           ( axis_hitZ      <= manipulator_height_ / 2.0 ) )
  {

    // x top has been hit
    _axis_hit = axis_hitZ;
    if ( _updateStates == Click)
      top_z_clicked_ = true;
    else
      top_z_over_ = true;

  } else
    _axis_hit = 0.0;

  if ( _updateStates == Click)
    return (top_x_clicked_ || top_y_clicked_ || top_z_clicked_);
  return (top_x_over_ || top_y_over_ || top_z_over_);
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
        outer_ring_xy_clicked_ = true;
      else if ( _updateStates == Over)
        outer_ring_xy_over_ = true;
      _v3 = hitPointXY;
      return true;
  }

  else if(yz_hit && (!more_than_one_hit || t2_near))
  {
      // Outer ring on yz plane has been hit
      if ( _updateStates == Click)
        outer_ring_yz_clicked_ = true;
      else if ( _updateStates == Over)
        outer_ring_yz_over_ = true;
      _v3 = hitPointYZ;
      return true;
  }

  else if(zx_hit && (!more_than_one_hit || t3_near))
  {
      // Outer ring around zx plane has been hit
      if ( _updateStates == Click)
        outer_ring_zx_clicked_ = true;
      else if ( _updateStates == Over)
        outer_ring_zx_over_ = true;
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
  if (_target == PICK_FACE ||
	   _target == PICK_ANYTHING) {

    if (draw_manipulator_) {
	// Enable depth test but store original status
	glPushAttrib(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// Save modelview matrix
	_state.push_modelview_matrix();

	// Correctly align nodes local coordinate system
	update_manipulator_system(_state);

	glLoadName(0);
	//================================================================================================
	// Z-Axis
	//================================================================================================
	// gluCylinder draws into z direction so z-Axis first

	gluCylinder(axis_,
	manipulator_radius_,
	manipulator_radius_,
	manipulator_height_,
	manipulator_slices_,
	manipulator_stacks_);

	// Draw Top of z-axis

	_state.translate(0.0, 0.0, manipulator_height_);
	gluCylinder(axis_,
        manipulator_radius_*2,
	0,
	manipulator_height_/2,
	manipulator_slices_,
	manipulator_stacks_);
	_state.translate(0.0, 0.0, -manipulator_height_);

	glLoadName(1);
	//================================================================================================
	// Y-Axis
	//================================================================================================
	_state.rotate(-90, 1.0, 0.0, 0.0);
	gluCylinder(axis_,
		    manipulator_radius_,
	manipulator_radius_,
	manipulator_height_,
	manipulator_slices_,
	manipulator_stacks_);

	// Draw Top of y-axis
	_state.translate(0.0, 0.0, manipulator_height_);
	gluCylinder(axis_,
		    manipulator_radius_*2,
	            0,
	            manipulator_height_/2,
	            manipulator_slices_,
	            manipulator_stacks_);
	_state.translate(0.0, 0.0, -manipulator_height_);


	glLoadName(2);
	//================================================================================================
	// X-Axis
	//================================================================================================
	_state.rotate(90, 0.0, 1.0, 0.0);

	gluCylinder(axis_,
		    manipulator_radius_,
	            manipulator_radius_,
	            manipulator_height_,
	            manipulator_slices_,
	            manipulator_stacks_);

	// Draw Top of x-axis
	_state.translate(0.0, 0.0, manipulator_height_);
	gluCylinder(axis_,
		    manipulator_radius_*2,
	0,
	manipulator_height_/2,
	manipulator_slices_,
	manipulator_stacks_);
	_state.translate(0.0, 0.0, -manipulator_height_);

	glLoadName(3);
	//=================================================================================================
	// Sphere
	//=================================================================================================

	gluSphere( gluNewQuadric(), manipulator_radius_*2, manipulator_slices_, manipulator_stacks_ );

	//=================================================================================================
	// Outer-Spheres
	//=================================================================================================

	glLoadName(4);
	drawCircle(2*manipulator_height_, 2*manipulator_height_ - manipulator_height_/4.0);

	_state.rotate(90, 0.0, 1.0, 0.0);
	drawCircle(2*manipulator_height_, 2*manipulator_height_ - manipulator_height_/4.0);

	_state.rotate(90, 1.0, 0.0, 0.0);
	drawCircle(2*manipulator_height_, 2*manipulator_height_ - manipulator_height_/4.0);


	// Restore old attributes and modelview
	glPopAttrib();

	_state.pop_modelview_matrix();
    }
  }
}


//----------------------------------------------------------------------------

void
TranslationManipulatorNode::set_direction(Vec3d& _directionX, Vec3d& _directionY)
{

  localTransformation_.identity();

  double angle;

//   std::cerr << "_directionX " << _directionX[0] << " " << _directionX[1] << " " << _directionX[2] << std::endl;
//   std::cerr << "_directionY " << _directionY[0] << " " << _directionY[1] << " " << _directionY[2] << std::endl;
//   std::cerr << "dirX_ " << dirX_[0] << " " << dirX_[1] << " " << dirX_[2] << std::endl;
//   std::cerr << "dirY_ " << dirY_[0] << " " << dirY_[1] << " " << dirY_[2] << std::endl;

  angle = OpenMesh::rad_to_deg(acos(OpenMesh::sane_aarg( MathTools::sane_normalized(_directionX) | MathTools::sane_normalized(dirX_) )));

// std::cerr << "X angle = " << angle << std::endl;

  Vec3d rotAxis = MathTools::sane_normalized( dirX_ % _directionX );

  if ( rotAxis.sqrnorm() == 0.0 )
    rotAxis = MathTools::sane_normalized( dirZ_ );

  localTransformation_.rotate( angle, rotAxis );

  Vec3d newDirY = MathTools::sane_normalized( localTransformation_.transform_vector(dirY_) );

  angle = OpenMesh::rad_to_deg(acos(OpenMesh::sane_aarg( MathTools::sane_normalized(_directionY) | MathTools::sane_normalized(newDirY) )));

// std::cerr << "Y angle = " << angle <<  std::endl;
  Vec3d rotAxis2 = MathTools::sane_normalized( newDirY % _directionY );

  if ( rotAxis2.sqrnorm() == 0.0 )
    rotAxis2 = MathTools::sane_normalized( localTransformation_.transform_vector(dirX_) );

  localTransformation_.rotate( angle, rotAxis2 );
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



//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
