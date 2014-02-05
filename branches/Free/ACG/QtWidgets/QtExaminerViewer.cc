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
//  CLASS QtExaminerViewer - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================

#include "QtExaminerViewer.hh"

#include <QTimer>
#include <QPushButton>
#include <QClipboard>
#include <QApplication>
#include <QInputDialog>
#include <QStatusBar>

#include <QMouseEvent>
#include <QWheelEvent>
#include <QEvent>

#ifdef max
#  undef max
#endif


//== NAMESPACES ===============================================================


namespace ACG {
namespace QtWidgets {


//== IMPLEMENTATION ==========================================================


QtExaminerViewer::QtExaminerViewer( QWidget* _parent,
				    const char* _name,
				    QStatusBar *_statusBar,
				    const QGLFormat* _format,
				    const QtBaseViewer* _share,
				    Options _options ) :
  QtBaseViewer(_parent, _name, _statusBar, _format, _share, _options)
{

  // timer for animation
  timer_ = new QTimer( this );
  connect( timer_, SIGNAL(timeout()), this, SLOT( slotAnimation()) );

  allowRotation_ = true;

  //default wheel zoom factors
  wZoomFactor_ = 1.0;
  wZoomFactorShift_ = 0.2;
}


//-----------------------------------------------------------------------------


void
QtExaminerViewer::viewMouseEvent(QMouseEvent* _event)
{
  switch (_event->type())
  {
    case QEvent::MouseButtonPress:
    {
      // shift key -> set rotation center
      if (_event->modifiers() & Qt::ShiftModifier)
      {
        Vec3d c;
        if (fast_pick(_event->pos(), c))
        {
          trackball_center_ = c;
          trackball_radius_ = std::max(scene_radius_, (c-glState().eye()).norm()*0.9f);
        }
      }

      lastPoint_hitSphere_ = mapToSphere( lastPoint2D_=_event->pos(),
					  lastPoint3D_ );
      isRotating_ = true;
      timer_->stop();


      break;
    }


    case QEvent::MouseButtonDblClick:
    {
      if (allowRotation_)
        flyTo(_event->pos(), _event->button()==Qt::MidButton);
      break;
    }


    case QEvent::MouseMove:
    {
      double factor  = 1.0;

      if (_event->modifiers() == Qt::ShiftModifier)
        factor = wZoomFactorShift_;

      // mouse button should be pressed
      if (_event->buttons() & (Qt::LeftButton | Qt::MidButton))
      {
	     QPoint newPoint2D = _event->pos();

        if ( (newPoint2D.x()<0) || (newPoint2D.x() > (int)glWidth()) ||
             (newPoint2D.y()<0) || (newPoint2D.y() > (int)glHeight()) )
          return;

        double value_y;
        Vec3d  newPoint3D;
        bool   newPoint_hitSphere = mapToSphere( newPoint2D, newPoint3D );

        makeCurrent();

        // move in z direction
        if ( (_event->buttons() & Qt::LeftButton) &&
              (_event->buttons() & Qt::MidButton))
        {
          switch (projectionMode())
          {
            case PERSPECTIVE_PROJECTION:
            {
              value_y = scene_radius_ * ((newPoint2D.y() - lastPoint2D_.y())) * 3.0 / (double) glHeight();
              translate( Vec3d(0.0, 0.0, value_y * factor ) );
              updateGL();
              break;
            }

            case ORTHOGRAPHIC_PROJECTION:
            {
              value_y = ((newPoint2D.y() - lastPoint2D_.y())) * orthoWidth_ / (double) glHeight();
              orthoWidth_ -= value_y  * factor;
              updateProjectionMatrix();
              updateGL();
              break;
            }
          }
        }

        // move in x,y direction
        else if (_event->buttons() & Qt::MidButton)
        {
          double value_x;
          value_x = scene_radius_ * ((newPoint2D.x() - lastPoint2D_.x())) * 2.0 / (double) glWidth();
          value_y = scene_radius_ * ((newPoint2D.y() - lastPoint2D_.y())) * 2.0 / (double) glHeight();
          translate( Vec3d(value_x  * factor , -value_y  * factor , 0.0) );
        }

        // rotate
        else if (allowRotation_ && (_event->buttons() & Qt::LeftButton) )
        {
          Vec3d axis(1.0,0.0,0.0);
          double angle(0.0);

          if ( lastPoint_hitSphere_ ) {

            if ( ( newPoint_hitSphere = mapToSphere( newPoint2D,
                                newPoint3D ) ) ) {
              axis = lastPoint3D_ % newPoint3D;
              double cos_angle = ( lastPoint3D_ | newPoint3D );
              if ( fabs(cos_angle) < 1.0 ) {
                angle = acos( cos_angle ) * 180.0 / M_PI  * factor ;
                angle *= 2.0; // inventor rotation
              }
            }

            rotate(axis, angle);

          }

          lastRotationAxis_  = axis;
          lastRotationAngle_ = angle;
        }

        lastPoint2D_ = newPoint2D;
        lastPoint3D_ = newPoint3D;
        lastPoint_hitSphere_ = newPoint_hitSphere;

        updateGL();
        lastMoveTime_.restart();
      }
      break;
    }



    case QEvent::MouseButtonRelease:
    {
      lastPoint_hitSphere_ = false;

      // continue rotation ?
      if ( isRotating_ &&
	   (_event->button() == Qt::LeftButton) &&
	   (!(_event->buttons() & Qt::MidButton)) &&
	   (lastMoveTime_.elapsed() < 10) &&
	   animation() )
	     timer_->start(0);
      break;
    }

    default: // avoid warning
      break;
  }


  // sync views
  emit(signalSetView(glstate_->modelview(), glstate_->inverse_modelview()));
}


//-----------------------------------------------------------------------------


void
QtExaminerViewer::lightMouseEvent(QMouseEvent* _event)
{
  switch (_event->type()) {
    case QEvent::MouseButtonPress: {
      lastPoint_hitSphere_ = mapToSphere(lastPoint2D_ = _event->pos(), lastPoint3D_);
      isRotating_ = true;
      timer_->stop();
      break;
    }

    case QEvent::MouseMove: {

      // rotate lights
      if (_event->buttons() & Qt::LeftButton) {
        QPoint newPoint2D = _event->pos();

        if ((newPoint2D.x() < 0) || (newPoint2D.x() > (int) glWidth()) || (newPoint2D.y() < 0)
            || (newPoint2D.y() > (int) glHeight()))
          return;

        Vec3d newPoint3D;
        bool newPoint_hitSphere = mapToSphere(newPoint2D, newPoint3D);

        makeCurrent();

        if (lastPoint_hitSphere_) {
          Vec3d axis(1.0, 0.0, 0.0);
          double angle(0.0);

          if ((newPoint_hitSphere = mapToSphere(newPoint2D, newPoint3D))) {
            axis = lastPoint3D_ % newPoint3D;
            double cos_angle = (lastPoint3D_ | newPoint3D);
            if (fabs(cos_angle) < 1.0) {
              angle = acos(cos_angle) * 180.0 / M_PI;
              angle *= 2.0;
            }
          }
          rotate_lights(axis, angle);
        }

        lastPoint2D_ = newPoint2D;
        lastPoint3D_ = newPoint3D;
        lastPoint_hitSphere_ = newPoint_hitSphere;

        updateGL();
      }
      break;
    }

    default: // avoid warning
      break;
  }
}


//-----------------------------------------------------------------------------

double QtExaminerViewer::wheelZoomFactor(){
  return wZoomFactor_;
}

//-----------------------------------------------------------------------------

double QtExaminerViewer::wheelZoomFactorShift(){
  return wZoomFactorShift_;
}

//-----------------------------------------------------------------------------

void QtExaminerViewer::setWheelZoomFactor(double _factor){
  wZoomFactor_ = _factor;
}

//-----------------------------------------------------------------------------

void QtExaminerViewer::setWheelZoomFactorShift(double _factor){
  wZoomFactorShift_ = _factor;
}

//-----------------------------------------------------------------------------

void QtExaminerViewer::viewWheelEvent( QWheelEvent* _event)
{
  double factor = wZoomFactor_;

  if (_event->modifiers() == Qt::ShiftModifier)
    factor = wZoomFactorShift_;

  switch (projectionMode())
  {
    case PERSPECTIVE_PROJECTION:
    {
      double d = -(double)_event->delta() / 120.0 * 0.2 * factor * trackball_radius_/3;
      translate( Vec3d(0.0, 0.0, d) );
      updateGL();
      break;
    }

    case ORTHOGRAPHIC_PROJECTION:
    {
      double d = (double)_event->delta() / 120.0 * 0.2 * factor * orthoWidth_;
      orthoWidth_ += d;
      updateProjectionMatrix();
      updateGL();
      break;
    }
  }


  // sync views
  emit(signalSetView(glstate_->modelview(), glstate_->inverse_modelview()));
}


//-----------------------------------------------------------------------------


bool QtExaminerViewer::mapToSphere(const QPoint& _v2D, Vec3d& _v3D) const
{
  if ( (_v2D.x() >= 0) && (_v2D.x() < (int)glWidth()) &&
       (_v2D.y() >= 0) && (_v2D.y() < (int)glHeight()) )
  {
    double x   = (double)(_v2D.x() - ((double)glWidth() / 2.0))  / (double)glWidth();
    double y   = (double)(((double)glHeight() / 2.0) - _v2D.y()) / (double)glHeight();
    double sinx         = sin(M_PI * x * 0.5);
    double siny         = sin(M_PI * y * 0.5);
    double sinx2siny2   = sinx * sinx + siny * siny;

    _v3D[0] = sinx;
    _v3D[1] = siny;
    _v3D[2] = sinx2siny2 < 1.0 ? sqrt(1.0 - sinx2siny2) : 0.0;

    return true;
  }
  else return false;
}


//-----------------------------------------------------------------------------


void QtExaminerViewer::slotAnimation()
{

  QTime t;
  t.start();
  makeCurrent();
  rotate( lastRotationAxis_, lastRotationAngle_ );
  updateGL();

  if (!isUpdateLocked()) {

    static int msecs=0, count=0;

    msecs += t.elapsed();
    if (count==10) {
      assert(statusbar_!=0);
      char s[100];
      sprintf( s, "%.3f fps", (10000.0 / (float)msecs) );
      statusbar_->showMessage(s,2000);
      count = msecs = 0;
    }
    else
      ++count;
  }
}


//=============================================================================
} // namespace QtWidgets
} // namespace ACG
//=============================================================================
