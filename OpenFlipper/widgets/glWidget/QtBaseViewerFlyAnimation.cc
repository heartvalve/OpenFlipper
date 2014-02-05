/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2014 by Computer Graphics Group, RWTH Aachen       *
*                           www.openflipper.org                              *
*                                                                            *
*--------------------------------------------------------------------------- *
*  This file is part of OpenFlipper.                                         *
*                                                                            *
*  OpenFlipper is free software: you can redistribute it and/or modify       *
*  it under the terms of the GNU Lesser General Public License as            *
*  published by the Free Software Foundation, either version 3 of            *
*  the License, or (at your option) any later version with the               *
*  following exceptions:                                                     *
*                                                                            *
*  If other files instantiate templates or use macros                        *
*  or inline functions from this file, or you compile this file and          *
*  link it with other files to produce an executable, this file does         *
*  not by itself cause the resulting executable to be covered by the         *
*  GNU Lesser General Public License. This exception does not however        *
*  invalidate any other reasons why the executable file might be             *
*  covered by the GNU Lesser General Public License.                         *
*                                                                            *
*  OpenFlipper is distributed in the hope that it will be useful,            *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
*  GNU Lesser General Public License for more details.                       *
*                                                                            *
*  You should have received a copy of the GNU LesserGeneral Public           *
*  License along with OpenFlipper. If not,                                   *
*  see <http://www.gnu.org/licenses/>.                                       *
*                                                                            *
\*===========================================================================*/

/*===========================================================================*\
*                                                                            *
*   $Revision$                                                       *
*   $LastChangedBy$                                                *
*   $Date$                     *
*                                                                            *
\*===========================================================================*/




//=============================================================================
//
//  CLASS glViewer - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================

#include "QtBaseViewer.hh"


//== NAMESPACES ===============================================================

//== IMPLEMENTATION ==========================================================


void glViewer::flyTo(const QPoint& _pos, bool _moveBack)
{
  makeCurrent();

  unsigned int nodeIdx, targetIdx;
  ACG::Vec3d hitPoint;

  if (pick( ACG::SceneGraph::PICK_ANYTHING, _pos, nodeIdx, targetIdx, &hitPoint))
  {
    if (projectionMode_ == PERSPECTIVE_PROJECTION)
    {
      ACG::Vec3d eye(glstate_->eye());
      ACG::Vec3d t = hitPoint - eye;
      ACG::Vec3d e = eye + t * (_moveBack ? -0.5f : 0.5f);
      flyTo(e, hitPoint, 300);
    }
    else
    {
      // Project hitpoint to get depth
      ACG::Vec3d hitPointProjected = glstate_->project(hitPoint);

      // Create projected center point with same depth as hitpoint
      ACG::Vec3d centerPointProjected = hitPointProjected;

      // Get viewport data
      int w = 0, h = 0,left = 0, bottom = 0;
      glstate_->get_viewport(left, bottom, w, h);

      // Compute the center point. Note that the project function includes the viewport matrix.
      // As we have different viewports for the viewer but one global coord system,
      // we need to set the real center coordinates and therefore add the lower left corner position
      // which is the left and bottom of the viewport
      centerPointProjected[0] = left   + glstate_->viewport_width() / 2.0 ;
      centerPointProjected[1] = bottom + glstate_->viewport_height() / 2.0 ;

      // unproject center point
      ACG::Vec3d centerPointUnProjected = glstate_->unproject(centerPointProjected);

      // translation vector to make hit point project to center point (both need same depth)
      ACG::Vec3d t = hitPoint - centerPointUnProjected;

      // Transform to correct translation vector with modelview.
      flyTranslation_ = glstate_->modelview().transform_vector(t);

      // remember originalWidth
      flyOrthoWidthOriginal_ = properties_.orthoWidth();

      // Initialize as we start at 0.0
      lastAnimationPos_ = 0.0;

      // store the direction for the actual animation
      flyMoveBack_ = _moveBack;

      // Set the double click point as the new trackball center
      // Rotations will use this point as the center.
      properties_.trackballCenter( hitPoint );

      // Create animation object
      if ( flyAnimationOrthogonal_ == 0) {
        flyAnimationOrthogonal_ = new QPropertyAnimation(this, "currentAnimationPosition");


        // Range is from 0 to one, as we linearly interpolate the animation
        flyAnimationOrthogonal_->setStartValue(0.0);
        flyAnimationOrthogonal_->setEndValue(1.0);

        // Connect signals for the animation and its end
        connect(flyAnimationOrthogonal_, SIGNAL(valueChanged(QVariant)), this, SLOT(flyAnimationOrthogonal(QVariant)));
        connect(flyAnimationOrthogonal_, SIGNAL(finished()), this, SLOT(flyAnimationOrthogonalFinished()));
      }

      // Set duration
      flyAnimationOrthogonal_->setDuration(300);

      // Start it
      flyAnimationOrthogonal_->start();

    }
  }
}

void glViewer::flyAnimationOrthogonal(QVariant _pos) {

  const double pos = _pos.toDouble();

  // compute difference
  const double diff = pos - lastAnimationPos_;

  // zoom back one frame
  if ( flyMoveBack_ ) {
    // Move back by factor 2
    properties_.orthoWidth( flyOrthoWidthOriginal_ * (1.0 + pos ) );
  } else
    // Move forward with factor 0.5
    properties_.orthoWidth( flyOrthoWidthOriginal_ * (1.0 - 0.5 * pos ) );

  // apply translation
  translate(- flyTranslation_ * diff );

  // Store our current position for next loop
  lastAnimationPos_ = pos;

  // Pick cache is definitely invalid after that
  updatePickCache_ = true;

  // update rendering
  update();
}

void glViewer::flyAnimationPerspective(QVariant _pos) {

  const double pos = _pos.toDouble();

  // Animate pos from 0 to 1 so we need to calculate the difference and the resulting transformations
  ACG::Vec3d t = (pos - lastAnimationPos_) * flyTranslation_ ;
  double     a = (pos - lastAnimationPos_) * flyAngle_       ;

  translate(t);

  // Only rotate, if we have realistic values and if rotation is allowed
  if ( allowRotation_ )
    if (fabs(a) > FLT_MIN)
      rotate(flyAxis_, a, flyCenter_);

  // Pick cache is definitely invalid after that
  updatePickCache_ = true;

  // Store our current position for next loop
  lastAnimationPos_ = pos;

  // update rendering
  update();

}

void glViewer::flyAnimationOrthogonalFinished() {

  // Update the projection matrix
  updateProjectionMatrix();

  // Redraw scene
  updateGL();

  // Inform others that the current view has changed
  emit viewChanged();

}

void glViewer::flyAnimationPerspectiveFinished() {

  // Update the trackball to the final position
  properties_.trackballCenter( flyCenter_ );
  properties_.trackballRadius( std::max( properties_.sceneRadius(),( flyCenter_ - flyPosition_ ).norm() * 0.9f  ) );

}

void glViewer::flyTo(const ACG::Vec3d&  _position,
                     const ACG::Vec3d&  _center,
                     int                _time)
{
  makeCurrent();

  // compute rotation
  ACG::Vec3d c = glstate_->modelview().transform_point(_center);
  ACG::Vec3d p = glstate_->modelview().transform_point(_position);
  ACG::Vec3d view =(p-c).normalize();
  ACG::Vec3d z(0,0,1);

  flyAxis_ = (z % -view).normalize();

  flyAngle_ = acos(std::max(-1.0, std::min(1.0, (z | view)))) / M_PI * 180.0;

  if (flyAngle_ > 175)
    flyAxis_  = ACG::Vec3d(0,1,0);

  // compute translation
  ACG::Vec3d target = glstate_->modelview().transform_point(_center);
  flyTranslation_   = ACG::Vec3d( -target[0], -target[1], -target[2] - (_position-_center).norm() );

  // Store other values for animation
  flyCenter_        = _center;
  flyPosition_      = _position;

  // Initialize as we start at 0.0
  lastAnimationPos_ = 0.0;

  // Create animation object
  if ( flyAnimationPerspective_ == 0) {
    flyAnimationPerspective_ = new QPropertyAnimation(this, "currentAnimationPosition");

    // Range is from 0 to one, as we linearly interpolate the animation
    flyAnimationPerspective_->setStartValue(0.0);
    flyAnimationPerspective_->setEndValue(1.0);

    // Connect signals for the animation and its end
    connect(flyAnimationPerspective_, SIGNAL(valueChanged(QVariant)), this, SLOT(flyAnimationPerspective(QVariant)));
    connect(flyAnimationPerspective_, SIGNAL(finished()), this, SLOT(flyAnimationPerspectiveFinished()));
  }

  // Set duration
  flyAnimationPerspective_->setDuration(_time);

  // Start it
  flyAnimationPerspective_->start();
}


//=============================================================================

//=============================================================================
