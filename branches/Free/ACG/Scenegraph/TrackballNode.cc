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
//  CLASS TrackballNode - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================


#include "TrackballNode.hh"
#include "../Math/GLMatrixT.hh"
#include "../GL/gl.hh"

#include <QMouseEvent>
#include <QEvent>


//== NAMESPACES ===============================================================


namespace ACG {
namespace SceneGraph {


//== IMPLEMENTATION ========================================================== 


void
TrackballNode::draw(GLState& /* _state */ , const DrawModes::DrawMode& /* _drawMode */ )
{
  // draw the trackball
  if (drawTrackball_) 
  {
    ACG::GLState::disable(GL_LIGHTING);
    ACG::GLState::shadeModel( GL_FLAT );
    glPushMatrix();
    glTranslatef(center()[0], center()[1], center()[2]);
    glutWireSphere(radius_, 20, 20);
    glPopMatrix();
  }


  // draw the coord axes
  if (drawAxes_)
  {
    // store original settings
    GLfloat backupColor[4], backupLineWidth;
    glGetFloatv(GL_CURRENT_COLOR, backupColor);
    glGetFloatv(GL_LINE_WIDTH, &backupLineWidth);
    
    ACG::GLState::disable(GL_LIGHTING);
    ACG::GLState::shadeModel( GL_FLAT );
    glLineWidth(3.0);

    glColor3f(1.0, 0.0, 0.0);
    glBegin(GL_LINES);
    glVertex(center());
    glVertex(center() + radius_*xAxis_);
    glEnd();

    glColor3f(0.0, 1.0, 0.0);
    glBegin(GL_LINES);
    glVertex(center());
    glVertex(center() + radius_*yAxis_);
    glEnd();

    glColor3f(0.0, 0.0, 1.0);
    glBegin(GL_LINES);
    glVertex(center());
    glVertex(center() + radius_*zAxis_);
    glEnd();

    // restore original settings
    glColor4fv(backupColor);
    glLineWidth(backupLineWidth);
  }
}


//----------------------------------------------------------------------------


void
TrackballNode::mouseEvent(GLState& _state, QMouseEvent* _event)
{
  Vec3d         oldPoint3D;
  Vec2i         newPoint2D(_event->pos().x(), _event->pos().y());
  Vec3d         newPoint3D;


  switch (_event->type())
  {
    case QEvent::MouseButtonPress:
    {
      break;
    }


    case QEvent::MouseButtonDblClick:
    {
      if (mapToSphere(_state, oldPoint2D_, oldPoint3D))
      {
	// toggle drawTrackball_
        if (_event->button() == Qt::LeftButton)
  	  drawTrackball_ = !drawTrackball_;

        // toggle drawAxes_
        if (_event->button() == Qt::MidButton)
	  drawAxes_ = !drawAxes_;
      }
      break;
    }
    
    
    case QEvent::MouseMove:
    {
      bool hit0 = mapToSphere(_state, newPoint2D, newPoint3D);
      bool hit1 = mapToSphere(_state, oldPoint2D_, oldPoint3D);

      if (hit0 && hit1)
      {

	// scaling
	if ((_event->button() & Qt::LeftButton) &&
	    (_event->button() & Qt::MidButton))
	{
	  double s = 1.0 + ((double) (newPoint2D[1] - oldPoint2D_[1]) /
			   (double) _state.viewport_height());
	  scale(s);
	}


	// translation
	else if (_event->button() & Qt::MidButton)
	{
	  double value_x = (radius_ * ((newPoint2D[0] - oldPoint2D_[0])) 
			   * 2.0 / (double) _state.viewport_width());
	  
	  double value_y = (radius_ * ((newPoint2D[1] - oldPoint2D_[1]))
			   * 2.0 / (double) _state.viewport_height());
	  
	  
	  /* need inverse transposed matrix in order to
	     transform direction vectors */
	  GLMatrixd m = inverse_matrix();
	  m.transpose();


	  Vec3d dx, dy;

	  // axis aligned
	  if (drawAxes_)
	  {
 	    dx = m.transform_point(xAxis_);
 	    dy = m.transform_point(yAxis_);
	  }

	  // screen space translation
	  else 
	  {
 	    dx = m.transform_vector(_state.right());
 	    dy = m.transform_vector(_state.up());
	  }
	  
	  dx.normalize();
	  dy.normalize();
	  translate(value_x*dx - value_y*dy);
	}
      


	// rotation
	else if (_event->button() & Qt::LeftButton) 
	{
	  Vec3d axis = oldPoint3D % newPoint3D;	
	  double cos_angle = ( oldPoint3D | newPoint3D );

	  
	  if (fabs(cos_angle) < 1.0)
	  {
	    // rotate coord axes
	    if (_event->modifiers() & Qt::AltModifier)
	    {
	      GLMatrixd mat;
	      
	      mat.identity();
	      mat.rotate(acos(cos_angle)*180.0/M_PI, axis);
	      
	      /* transform_point works since only 
		 rotations and no translations are
		 involved */
	      xAxis_ = mat.transform_point(xAxis_);
	      yAxis_ = mat.transform_point(yAxis_);
	      zAxis_ = mat.transform_point(zAxis_);
	    }
	    
	    // normal rotation
	    else  rotate(acos(cos_angle)*180.0/M_PI, axis);
	  }
	}
      }


      break;
    }

    default: // avoid warning
      break;
  }


  // store 2D point
  oldPoint2D_ = newPoint2D;
}


//----------------------------------------------------------------------------


bool
TrackballNode::mapToSphere( GLState& _state,
			    const Vec2i& _v2, 
			    Vec3d& _v3 )
{
  // Qt -> GL coordinate systems
  unsigned int x = _v2[0];
  unsigned int y = _state.context_height() - _v2[1];


  // get ray from eye through pixel (trackball coords)
  Vec3d origin, direction;
  _state.viewing_ray(x, y, origin, direction);


  // translate and scale trackball to unit sphere
  origin -= center();
  origin /= radius_;


  // calc sphere-ray intersection
  // (sphere is centered at origin, has radius 1)
  double a = direction.sqrnorm(),
        b = 2.0 * (origin | direction),
        c = origin.sqrnorm() - 1.0,
        d = b*b - 4.0*a*c,
        t;

  if      (d <  0.0)  return false;
  else if (d == 0.0)  t = -b / (2.0*a);
  else 
  { 
    // t1 = (-b - sqrt(d)) / (2.0*a),  t2 = (-b + sqrt(d)) / (2.0*a)
    a = 1.0 / (2.0*a);
    d = sqrt(d);
    double t1 = (-b - d) * a;
    double t2 = (-b + d) * a;
    t = (t1 < t2) ? t1 : t2;
  }

  _v3 = origin + direction*t;
  
  return true;
}


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
