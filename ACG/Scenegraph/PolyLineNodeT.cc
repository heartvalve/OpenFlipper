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
//  CLASS PolyLineNodeT - IMPLEMENTATION
//
//=============================================================================

#define ACG_POLYLINENODET_C

//== INCLUDES =================================================================

#include "PolyLineNodeT.hh"
#include "../GL/gl.hh"
#include <ACG/Utils/VSToolsT.hh>
#include <vector>

//== NAMESPACES ===============================================================

namespace ACG {
namespace SceneGraph {

//== IMPLEMENTATION ==========================================================

template <class PolyLine>
void
PolyLineNodeT<PolyLine>::
boundingBox(Vec3d& _bbMin, Vec3d& _bbMax)
{
  for (unsigned int i=0; i< polyline_.n_vertices(); ++i)
  {
    _bbMin.minimize(polyline_.point(i));
    _bbMax.maximize(polyline_.point(i));
  }
}


//----------------------------------------------------------------------------


template <class PolyLine>
unsigned int
PolyLineNodeT<PolyLine>::
availableDrawModes() const
{
  return (DrawModes::WIREFRAME | DrawModes::POINTS);
}


//----------------------------------------------------------------------------


template <class PolyLine>
void
PolyLineNodeT<PolyLine>::
draw(GLState& /*_state*/, unsigned int _drawMode)
{
  glDepthFunc(depthFunc());

  glDisable(GL_LIGHTING);

  // draw points
  if (_drawMode & DrawModes::POINTS)
  {
    // draw selection
    if( polyline_.vertex_selections_available())
    {
      // save old values
      Vec4f base_color_old = base_color();
      float point_size_old = point_size();
      glColor3f(1,0,0);
      glPointSize(point_size_old+4);

      glBegin(GL_POINTS);
      // draw possibly closed PolyLine
      for (unsigned int i=0; i< polyline_.n_vertices(); ++i)
      {
	if( polyline_.vertex_selection(i))
	{
	  glVertex(polyline_.point( i) );
	}
      }
      glEnd();

      glPointSize(point_size_old);
      glColor( base_color_old );
    }

    // draw all points
    glBegin(GL_POINTS);
    for (unsigned int i=0; i< polyline_.n_vertices(); ++i)
      glVertex(polyline_.point(i));
    glEnd();
  }


  // draw line segments
  if (_drawMode & DrawModes::WIREFRAME)
  {
    // draw selection
    if( polyline_.edge_selections_available())
    {
      // save old values
      Vec4f base_color_old = base_color();
      float line_width_old = line_width();
      glColor3f(1,0,0);
      glLineWidth(2*line_width_old);

      glBegin(GL_LINES);
      // draw possibly closed PolyLine
      for (unsigned int i=0; i< polyline_.n_edges(); ++i)
      {
	if( polyline_.edge_selection(i))
	{
	  glVertex(polyline_.point( i    % polyline_.n_vertices()));
	  glVertex(polyline_.point((i+1) % polyline_.n_vertices()));
	}
      }
      glEnd();

      glLineWidth(line_width_old);
      glColor( base_color_old );
    }

    // draw all line segments
    glBegin(GL_LINE_STRIP);
    // draw possibly closed PolyLine
    if( polyline_.n_vertices())
      glVertex(polyline_.point( 0));
    for (unsigned int i=0; i< polyline_.n_edges(); ++i)
      glVertex(polyline_.point( (i+1) % polyline_.n_vertices()));
    glEnd();
  }

  glDepthFunc(GL_LESS);
}

//----------------------------------------------------------------------------


template <class PolyLine>
void
PolyLineNodeT<PolyLine>::
pick(GLState& _state, PickTarget _target)
{
  unsigned int n_end = polyline_.n_edges()+1;
  if( !polyline_.is_closed()) --n_end;

  switch (_target)
  {
    case PICK_VERTEX:
    {
      _state.pick_set_maximum (polyline_.n_vertices());
      pick_vertices( _state);
      break;
    }

    case PICK_EDGE:
    {
      _state.pick_set_maximum (n_end);
      pick_edges(_state, 0);
      break;
    }

    case PICK_ANYTHING:
    {
      _state.pick_set_maximum (polyline_.n_vertices() + n_end);
      pick_vertices( _state);
      pick_edges( _state, polyline_.n_vertices());
      break;
    }

    default:
      break;
  }
}


//----------------------------------------------------------------------------


template <class PolyLine>
void
PolyLineNodeT<PolyLine>::
pick_vertices( GLState& _state )
{
  // radius in pixels
  int psize = 7;
  unsigned int slices(4);
  unsigned int stacks(3);

  _state.pick_set_name (0);

  for (unsigned int i=0; i< polyline_.n_vertices(); ++i)
  {
    _state.pick_set_name (i);

    // compute 3d radius of sphere
    Vec3d window_pos = _state.project( (Vec3d) polyline_.point(i));
    int px = round( window_pos[0]);
    int py = round( window_pos[1]);
    double angle = acos(_state.viewing_direction(px, py).normalize()|_state.viewing_direction(px+psize, py).normalize());
    double l = (_state.eye() - (Vec3d) polyline_.point(i)).norm();
    double r = l*tan(angle);

    // draw 3d sphere
    _state.push_modelview_matrix();
    _state.translate(polyline_.point(i)[0], polyline_.point(i)[1], polyline_.point(i)[2]);

    GLUquadricObj *qobj = gluNewQuadric();
    gluSphere(qobj, r, slices, stacks);
    gluDeleteQuadric(qobj);

    _state.pop_modelview_matrix();
  }
}


//----------------------------------------------------------------------------


template <class PolyLine>
void
PolyLineNodeT<PolyLine>::
pick_edges( GLState& _state, unsigned int _offset)
{
  // radius in pixels
  int psize = 5;

  // Check if we have any edges to draw ( % 0 causes division by zero on windows)
  if ( polyline_.n_edges() == 0 )
    return;

  unsigned int n_end = polyline_.n_edges()+1;
  if( !polyline_.is_closed()) --n_end;

  _state.pick_set_name (0);

  // draw possibly closed PolyLine
  for (unsigned int i=0; i<n_end; ++i)
  {
    _state.pick_set_name (i+_offset);
    Point p0 = polyline_.point(i     % polyline_.n_vertices());
    Point p1 = polyline_.point((i+1) % polyline_.n_vertices());
    Point pm = 0.5*(p0+p1);

    // compute 3d radius of cylinder
    Vec3d window_pos = _state.project( (Vec3d) pm);
    int px = round( window_pos[0]);
    int py = round( window_pos[1]);
    double angle = acos(_state.viewing_direction(px, py).normalize()|_state.viewing_direction(px+psize, py).normalize());
    double l = (_state.eye() - pm).norm();
    double r = l*tan(angle);

    // draw cylinder
    draw_cylinder( p0, p1-p0, r, _state);
  }
}


//----------------------------------------------------------------------------


template <class PolyLine>
void
PolyLineNodeT<PolyLine>::
draw_cylinder( const Point& _p0, const Point& _axis, double _r, GLState& _state)
{
  _state.push_modelview_matrix();
  _state.translate(_p0[0], _p0[1], _p0[2]);

  unsigned int slices(6);
  unsigned int stacks(1);

  Point direction = _axis;
  Point z_axis(0,0,1);
  Point rot_normal;
  double rot_angle;

  direction.normalize();
  rot_angle  = acos((z_axis | direction))*180/M_PI;
  rot_normal = ((z_axis % direction).normalize());


  if( fabs( rot_angle ) > 0.0001 && fabs( 180 - rot_angle ) > 0.0001)
    _state.rotate(rot_angle,rot_normal[0], rot_normal[1], rot_normal[2]);
  else
    _state.rotate(rot_angle,1,0,0);

  GLUquadricObj *qobj = gluNewQuadric();
  gluCylinder(qobj, _r, _r, _axis.norm(), slices, stacks);
  gluDeleteQuadric(qobj);

  _state.pop_modelview_matrix();
}

//----------------------------------------------------------------------------


template <class PolyLine>
void
PolyLineNodeT<PolyLine>::
set_random_color()
{
  // set random color
// #ifndef WIN32
//   srand((unsigned) time(NULL));
// #endif
  set_color(OpenMesh::Vec4f(0.2 + double(rand())/double(RAND_MAX)*0.8,
			    0.2 + double(rand())/double(RAND_MAX)*0.8,
			    0.2 + double(rand())/double(RAND_MAX)*0.8,
			    1.0));
}


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
