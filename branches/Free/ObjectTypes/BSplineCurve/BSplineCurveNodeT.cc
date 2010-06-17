/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2010 by Computer Graphics Group, RWTH Aachen      *
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
 *   $Revision$                                                         *
 *   $Author$                                                      *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/


//=============================================================================
//
//  CLASS BSplineCurveNodeT - IMPLEMENTATION
//  Author:  Ellen Dekkers <dekkers@cs.rwth-aachen.de>
//
//=============================================================================

#define ACG_BSPLINECURVENODET_C

//== INCLUDES =================================================================

#include "BSplineCurveNodeT.hh"
#include <ACG/GL/gl.hh>
#include <ACG/GL/GluError.hh>
#include <vector>


//== NAMESPACES ===============================================================

namespace ACG {
namespace SceneGraph {

//== IMPLEMENTATION ==========================================================

//----------------------------------------------------------------------------

template <class BSplineCurve>
void
BSplineCurveNodeT<BSplineCurve>::
boundingBox(Vec3d& _bbMin, Vec3d& _bbMax)
{
  for (unsigned int i = 0; i < bsplineCurve_.n_control_points(); ++i)
  {
    _bbMin.minimize(bsplineCurve_.get_control_point(i));
    _bbMax.maximize(bsplineCurve_.get_control_point(i));
  }
}


//----------------------------------------------------------------------------


template <class BSplineCurve>
DrawModes::DrawMode
BSplineCurveNodeT<BSplineCurve>::
availableDrawModes() const
{
  /*
  DrawModes::DrawMode drawModes(0);

  drawModes |= DrawModes::POINTS;
  drawModes |= DrawModes::WIREFRAME;
  drawModes |= DrawModes::HIDDENLINE;
  drawModes |= DrawModes::SOLID_SMOOTH_SHADED;
  drawModes |= DrawModes::SOLID_FLAT_SHADED;
  drawModes |= DrawModes::SOLID_PHONG_SHADED;
  drawModes |= DrawModes::SOLID_SHADER;
  drawModes |= DrawModes::SOLID_TEXTURED;
  drawModes |= DrawModes::SOLID_1DTEXTURED;

  return drawModes;*/

  return (DrawModes::WIREFRAME | DrawModes::POINTS);
}


//----------------------------------------------------------------------------


template <class BSplineCurve>
void
BSplineCurveNodeT<BSplineCurve>::
draw(GLState& _state,DrawModes::DrawMode _drawMode)
{
  glDepthFunc(depthFunc());
  glDisable(GL_LIGHTING);

  if (bspline_draw_mode_ == DIRECT)
    drawDirectMode(_drawMode, _state);
  else
    drawGluNurbsMode(_state);

  glDepthFunc(GL_LESS);
}

//----------------------------------------------------------------------------

template <class BSplineCurve>
void
BSplineCurveNodeT<BSplineCurve>::
drawGluNurbsMode(GLState& _state)
{
  int numKnots = bsplineCurve_.n_knots();
  GLfloat *knots = new GLfloat[numKnots];
  for (int i = 0; i < numKnots; ++i)
    knots[i] = bsplineCurve_.get_knot(i);

  const int numCPs = bsplineCurve_.n_control_points();

  GLfloat *ctlpoints = new GLfloat[numCPs * 3];
  for (int i = 0; i < numCPs; ++i)
  {
    Vec3d p = bsplineCurve_.get_control_point(i);
    ctlpoints[i * 3 + 0] = (GLfloat)p[0];
    ctlpoints[i * 3 + 1] = (GLfloat)p[1];
    ctlpoints[i * 3 + 2] = (GLfloat)p[2];
  }

  int order = bsplineCurve_.degree() + 1;

  int lineWidth = (int)_state.line_width();
  glLineWidth(lineWidth);
  glColor(curve_color_);

  GLUnurbsObj *theNurb;
  theNurb = gluNewNurbsRenderer();

  gluNurbsCallback(theNurb, GLU_ERROR, (GLvoid (*)()) nurbsError);
  gluNurbsProperty(theNurb, GLU_DISPLAY_MODE, GLU_FILL);
  gluNurbsProperty(theNurb, GLU_SAMPLING_TOLERANCE, 5.0);

  gluBeginCurve(theNurb);
  gluNurbsCurve(theNurb, numKnots, knots, 3, ctlpoints, order, GL_MAP1_VERTEX_3);
  gluEndCurve(theNurb);

  gluDeleteNurbsRenderer(theNurb);

  delete[] knots;
  delete[] ctlpoints;

}

//----------------------------------------------------------------------------

template <class BSplineCurve>
void
BSplineCurveNodeT<BSplineCurve>::
drawDirectMode(DrawModes::DrawMode _drawMode, GLState& _state)
{
  // draw control polygon

  // draw points
  if ((_drawMode & DrawModes::POINTS) && render_control_polygon_)
  {
    // draw selection
    if( bsplineCurve_.controlpoint_selections_available())
    {
      // save old values
      Vec4f base_color_old = _state.base_color();
      float point_size_old = _state.point_size();

      glColor(polygon_highlight_color_);
      glPointSize(10);
//       glPointSize(4 * point_size_old);

      glBegin(GL_POINTS);
      // draw control polygon
      for (unsigned int i = 0; i < bsplineCurve_.n_control_points(); ++i)
      {
        if( bsplineCurve_.controlpoint_selection(i))
          glVertex(bsplineCurve_.get_control_point(i) );
      }
      glEnd();

      glPointSize(point_size_old);
      glColor( base_color_old );
    }

    // draw all points
    Vec4f base_color_old = _state.base_color();
    glColor(polygon_color_);
    float point_size_old = _state.point_size();
    glPointSize(point_size_old + 4);

    glBegin(GL_POINTS);
    for (unsigned int i=0; i< bsplineCurve_.n_control_points(); ++i)
      glVertex(bsplineCurve_.get_control_point(i));
    glEnd();

    glColor( base_color_old );
    glPointSize(point_size_old);
  }


  // draw line segments
  if ((_drawMode & DrawModes::WIREFRAME) && render_control_polygon_)
  {
    // draw selection
    if( bsplineCurve_.edge_selections_available())
    {
      // save old values
      Vec4f base_color_old = _state.base_color();
      float line_width_old = _state.line_width();

      glColor(polygon_highlight_color_);
      glLineWidth(2*line_width_old);

      glBegin(GL_LINES);
      // draw bspline control polygon
      int num_edges = (int)(bsplineCurve_.n_control_points()) - 1;
      for (int i = 0; i < num_edges; ++i) // #edges
      {
        if( bsplineCurve_.edge_selection(i))
        {
          glVertex(bsplineCurve_.get_control_point( i    % bsplineCurve_.n_control_points()));
          glVertex(bsplineCurve_.get_control_point((i+1) % bsplineCurve_.n_control_points()));
        }
      }
      glEnd();

      glLineWidth(line_width_old);
      glColor( base_color_old );
    }

    // draw all line segments
    Vec4f base_color_old = _state.base_color();
    glColor(polygon_color_);

//     float line_width_old = _state.line_width();
//     glLineWidth(line_width_old+2.0);

    glBegin(GL_LINE_STRIP);
    // draw bspline control polygon
    for (unsigned int i=0; i< bsplineCurve_.n_control_points(); ++i)
      glVertex(bsplineCurve_.get_control_point(i % bsplineCurve_.n_control_points()));
    glEnd();

    glColor( base_color_old );
//     glLineWidth(line_width_old);
  }


  // draw the curve
  if ((_drawMode & DrawModes::WIREFRAME) && render_bspline_curve_)
  {
//     float line_width_old = _state.line_width();
//     glLineWidth(line_width_old + 2.0);

    glBegin(GL_LINE_STRIP);
    for (unsigned int i = 0; i < curve_samples_.size(); ++i)
    {
      Vec3d pos = curve_samples_[i].first;
      Vec4f col = curve_samples_[i].second;

      glColor(col);
      glVertex3f(pos[0], pos[1], pos[2]);
    }
    glEnd();

//     glLineWidth(line_width_old);

/*
    // save old values
    Vec4f base_color_old = _state.base_color();
    float line_width_old = _state.line_width();
    glColor(curve_color_);
//     glLineWidth(2*line_width_old);

    int d = bsplineCurve_.degree();
    int c = bsplineCurve_.n_control_points();
    int k = bsplineCurve_.n_knots();

    glBegin(GL_LINE_STRIP);

    for ( int l = d; l < k - d - 1; ++l )
    {
      for ( int s = 0; s <= resolution_; ++s )
      {
        double step = s / (float) resolution_ ;
        double u = bsplineCurve_.get_knot( l ) + step  * ( bsplineCurve_.get_knot( l+1 ) - bsplineCurve_.get_knot( l ) );

        // check if highlighted
        if ( bsplineCurve_.get_knotvector_ref()->selections_available() )
        {
          if (    bsplineCurve_.get_knotvector_ref()->selection(l)
               && bsplineCurve_.get_knotvector_ref()->selection(l+1))
            glColor(curve_highlight_color_);
          else
            glColor(curve_color_);
        }
        else
          glColor(curve_color_);

        Vec3d result = bsplineCurve_.curvePoint(u);
        glVertex3f(result[0], result[1], result[2]);
      } // end of resolution iter
    }

    glEnd();

    glLineWidth(line_width_old);
    glColor( base_color_old );
*/
  }
}


//----------------------------------------------------------------------------

template <class BSplineCurve>
void
BSplineCurveNodeT<BSplineCurve>::
updateGeometry()
{
  if (bspline_draw_mode_ == GLU_NURBS)
    return;

  curve_samples_.clear();

  std::pair< Vec3d, Vec4f > sample;

  int d = bsplineCurve_.degree();
  int k = bsplineCurve_.n_knots();

  for ( int l = d; l < k - d - 1; ++l )
  {
    for ( int s = 0; s <= resolution_; ++s )
    {
      double step = s / (float) resolution_ ;
      double u = bsplineCurve_.get_knot( l ) + step  * ( bsplineCurve_.get_knot( l+1 ) - bsplineCurve_.get_knot( l ) );

        // check if highlighted
      if ( bsplineCurve_.get_knotvector_ref()->selections_available() )
      {
        if (    bsplineCurve_.get_knotvector_ref()->selection(l)
             && bsplineCurve_.get_knotvector_ref()->selection(l+1))
          sample.second = curve_highlight_color_;
        else
          sample.second = curve_color_;
      }
      else
        sample.second = curve_color_;

      sample.first = bsplineCurve_.curvePoint(u);

      curve_samples_.push_back(sample);
    } // end of resolution iter
  }
}

//----------------------------------------------------------------------------


template <class BSplineCurve>
void
BSplineCurveNodeT<BSplineCurve>::
pick(GLState& _state, PickTarget _target)
{

  switch (_target)
  {
    case PICK_VERTEX:
    {
      _state.pick_set_maximum (bsplineCurve_.n_control_points());
      pick_vertices(_state);
      break;
    }

    case PICK_EDGE:
    {
      _state.pick_set_maximum (curve_samples_.size());
      pick_curve(_state, 0);
      break;
    }

    case PICK_SPLINE:
    {
      _state.pick_set_maximum (pick_texture_res_ );
      pick_spline(_state, 0);
      break;
    }


    case PICK_ANYTHING:
    {
      _state.pick_set_maximum (bsplineCurve_.n_control_points() + curve_samples_.size());
      pick_vertices(_state);
      pick_spline(_state, bsplineCurve_.n_control_points());
//       pick_curve(_state, bsplineCurve_.n_control_points());
      break;
    }

    default:
      _state.pick_set_maximum(1);
      _state.pick_set_name(0);
      break;
  }
}


//----------------------------------------------------------------------------

/*
template <class BSplineCurve>
void
BSplineCurveNodeT<BSplineCurve>::
pick_vertices( GLState& _state )
{
  // radius in pixels
  int psize = 7;

  _state.pick_set_maximum(bsplineCurve_.n_control_points());
  _state.pick_set_name (0);

  for (unsigned int i=0; i < bsplineCurve_.n_control_points(); ++i)
  {
    _state.pick_set_name (i);

    // compute 3d radius of sphere
    Vec3d window_pos = _state.project( (Vec3d) bsplineCurve_.get_control_point(i));
    int px = round( window_pos[0]);
    int py = round( window_pos[1]);
    double angle = acos(_state.viewing_direction(px, py).normalize()|_state.viewing_direction(px+psize, py).normalize());
    double l = (_state.eye() - (Vec3d)bsplineCurve_.get_control_point(i)).norm();
    double r = l*tan(angle);

    // draw 3d sphere
    draw_sphere( bsplineCurve_.get_control_point(i), r);
  }
}
*/
//----------------------------------------------------------------------------

template <class BSplineCurve>
void
BSplineCurveNodeT<BSplineCurve>::
pick_vertices( GLState& _state )
{
  // radius in pixels
  int psize = 7;

//   _state.pick_set_name (0);

  for (unsigned int i=0; i < bsplineCurve_.n_control_points(); ++i)
  {
    _state.pick_set_name (i);

    // compute 3d radius of sphere
    Vec3d window_pos = _state.project( (Vec3d) bsplineCurve_.get_control_point(i));
    int px = round( window_pos[0]);
    int py = round( window_pos[1]);
    double angle = acos(_state.viewing_direction(px, py).normalize()|_state.viewing_direction(px+psize, py).normalize());
    double l = (_state.eye() - (Vec3d)bsplineCurve_.get_control_point(i)).norm();
    double r = l*tan(angle);

    // draw 3d sphere
//     _state.push_modelview_matrix();
//     _state.translate( bsplineCurve_.get_control_point(i)[0],
//                       bsplineCurve_.get_control_point(i)[1],
//                       bsplineCurve_.get_control_point(i)[2]);

//     GLUquadricObj *qobj = gluNewQuadric();
//     gluSphere(qobj, r, slices, stacks);
//     gluDeleteQuadric(qobj);

//     _state.pop_modelview_matrix();
    draw_sphere(bsplineCurve_.get_control_point(i), r, _state);
  }
}

//----------------------------------------------------------------------------

template <class BSplineCurve>
void
BSplineCurveNodeT< BSplineCurve >::
pick_curve( GLState& _state, unsigned int _offset)
{
  // radius in pixels
  int psize = 7;

//   _state.pick_set_maximum(curve_samples_.size());
//   _state.pick_set_name (0);

  for (unsigned int i = 0; i < curve_samples_.size(); ++i)
  {
    _state.pick_set_name (i + _offset);

    Vec3d pos = curve_samples_[i].first;
    glVertex3f(pos[0], pos[1], pos[2]);

    Vec3d window_pos = _state.project( pos );
    int px = round( window_pos[0]);
    int py = round( window_pos[1]);
    double angle = acos(_state.viewing_direction(px, py).normalize()|_state.viewing_direction(px+psize, py).normalize());
    double l = (_state.eye() - pos).norm();
    double r = l*tan(angle);

    // draw 3d sphere
//     _state.push_modelview_matrix();
//     _state.translate( pos[0], pos[1], pos[2]);

//     GLUquadricObj *qobj = gluNewQuadric();
//     gluSphere(qobj, r, slices, stacks);
//     gluDeleteQuadric(qobj);

//     _state.pop_modelview_matrix();

    draw_sphere(pos, r, _state);
  }
}

//----------------------------------------------------------------------------

template <class BSplineCurve >
void
BSplineCurveNodeT<BSplineCurve>::
pick_spline( GLState& _state, unsigned int _offset )
{
  glPushAttrib(GL_ALL_ATTRIB_BITS);

  glEnable(GL_TEXTURE_1D);

  if( _state.pick_current_index () != pick_texture_baseidx_)
  {
    pick_texture_baseidx_ = _state.pick_current_index();
    pick_create_texture( _state);
  }
  else
    glBindTexture( GL_TEXTURE_1D, pick_texture_idx_);


  pick_draw_textured_nurbs( _state);

  glBindTexture( GL_TEXTURE_1D, 0);
  glDisable(GL_TEXTURE_1D);
  glPopAttrib( );
}

//----------------------------------------------------------------------------

template <class BSplineCurve>
void
BSplineCurveNodeT<BSplineCurve>::
draw_sphere( const Point& _p0, double _r, GLState& _state)
{
  // draw 3d sphere
  _state.push_modelview_matrix();
  _state.translate( _p0[0], _p0[1], _p0[2]);

  unsigned int slices(5);
  unsigned int stacks(5);

  GLUquadricObj *qobj = gluNewQuadric();
  gluSphere(qobj, _r, slices, stacks);
  gluDeleteQuadric(qobj);

  _state.pop_modelview_matrix();
}

//----------------------------------------------------------------------------

template <class BSplineCurve>
void
BSplineCurveNodeT<BSplineCurve>::
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

template <class BSplineCurve>
void
BSplineCurveNodeT<BSplineCurve>::
set_random_color()
{
  // set random color
#ifndef WIN32
  srand((unsigned) time(NULL));
#endif
  set_color(OpenMesh::Vec4f(0.2 + double(rand())/double(RAND_MAX)*0.8,
			    0.2 + double(rand())/double(RAND_MAX)*0.8,
			    0.2 + double(rand())/double(RAND_MAX)*0.8,
			    1.0));
}

//----------------------------------------------------------------------------

template <class BSplineCurve>
void
BSplineCurveNodeT<BSplineCurve>::
pick_init_texturing( )
{
  pick_texture_res_     = 256;
  pick_texture_baseidx_ = 0;

  // generate texture index
  glGenTextures( 1, &pick_texture_idx_ );
  // bind texture as current
  glBindTexture( GL_TEXTURE_1D, pick_texture_idx_ );
  // do not blend colors (else color picking breaks!)
  glTexParameterf( GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
  glTexParameterf( GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
  // avoid aliasing at patch boundaries
  glTexParameterf( GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
  // GL_REPLACE to avoid smearing colors (else color picking breaks!)
  glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  // unbind current texture
  glBindTexture( GL_TEXTURE_1D, 0);
}

//----------------------------------------------------------------------------

template <class BSplineCurve>
void
BSplineCurveNodeT<BSplineCurve>::
pick_create_texture( GLState& _state)
{
  std::cout << "[BSplineCurveNodeT] pick_create_texture" << std::endl;
  
//   QImage b(pick_texture_res_, pick_texture_res_, QImage::Format_ARGB32);
  QImage b(pick_texture_res_, 1, QImage::Format_ARGB32);
  
  std::cout << "texture of size " << b.width() << " x " << b.height() << std::endl;
    
  // fill with colors
  int cur_idx = 0;
  bool green = false;
  for( int i = 0; i < pick_texture_res_; ++i)
  {
    Vec4uc cur_col( _state.pick_get_name_color (cur_idx) );
//     b.setPixel (i, 0, qRgba((int)cur_col[0], (int)cur_col[1], (int)cur_col[2], (int)cur_col[3]));
    
    if (i % 10 == 0)
      green = !green;
    
    if (green)
      b.setPixel (i, 0, qRgba(0, 255, 0, 255));
    else
      b.setPixel (i, 0, qRgba(255, 0, 255, 255));
    
    cur_idx++;
  }

  // debug, output image (usually does not look as expected :\ )
  b.save("1Dcurvetexture.png", "PNG");
  
  pick_texture_image_ = QGLWidget::convertToGLFormat( b );

  // bind texture 
  glBindTexture( GL_TEXTURE_1D, pick_texture_idx_ );
  glTexImage1D(  GL_TEXTURE_1D,
                 0, GL_RGBA, pick_texture_image_.width(), 
                 0, GL_RGBA, GL_UNSIGNED_BYTE, 
                 pick_texture_image_.bits() );
                 
}

//----------------------------------------------------------------------------

template <class BSplineCurve>
void
BSplineCurveNodeT<BSplineCurve>::
pick_draw_textured_nurbs( GLState& _state)
{
  std::cout << "[BSplineCurveNodeT] pick_draw_textured_nurbs" << std::endl;
  
  int numKnots     = bsplineCurve_.n_knots();
  const int numCPs = bsplineCurve_.n_control_points();
  int order        = bsplineCurve_.degree() + 1;

  // get kntvector
  std::cout << "knots: " << std::flush;
  GLfloat *knots = new GLfloat[numKnots];
  for (int i = 0; i < numKnots; ++i)
  {
    knots[i] = bsplineCurve_.get_knot(i);
    std::cout << bsplineCurve_.get_knot(i) << ", " << std::flush;
  }
  std::cout << std::endl;
 
  // get control points
  GLfloat *ctlpoints = new GLfloat[numCPs * 3];
  for (int i = 0; i < numCPs; ++i)
  {
    Vec3d p = bsplineCurve_.get_control_point(i);
    ctlpoints[i * 3 + 0] = (GLfloat)p[0];
    ctlpoints[i * 3 + 1] = (GLfloat)p[1];
    ctlpoints[i * 3 + 2] = (GLfloat)p[2];
  }


  glLineWidth(5);

  GLUnurbsObj *theNurb;
  theNurb = gluNewNurbsRenderer();

  gluNurbsCallback(theNurb, GLU_ERROR, (GLvoid (*)()) nurbsError);

  // draw filled
  gluNurbsProperty(theNurb, GLU_DISPLAY_MODE, GLU_FILL);

  // object space -> fixed (non-adaptive) sampling
  gluNurbsProperty(theNurb, GLU_SAMPLING_METHOD, GLU_OBJECT_PARAMETRIC_ERROR);
  gluNurbsProperty(theNurb, GLU_PARAMETRIC_TOLERANCE, 0.2);

  // get min/max knots of domain defining patch (partition of unity)
  float  minu( knots[bsplineCurve_.degree()]);
  float  maxu( knots[numKnots - order]);
  std::cout << "minu = " << minu << ", maxu = " << maxu << std::endl;

  // control points of 2d texture ((0,0), (0,1), (1,0), (1,1) )
//   GLfloat   tcoords[8] = {0.0, 0.0, 0.0, 1.0, 1.0, 0.0, 1.0, 1.0};
  GLfloat   tcoords[2] = {0.0, 1.0};

  // knots of domain, over which tcoords shall be linearly interpolated
//   GLfloat   tknots[2] = {minu, maxu};
//   GLfloat   tknots[4] = {minu, minu, maxu, maxu};
  GLfloat   tknots[4] = {minu/(maxu - minu), minu/(maxu - minu), maxu/(maxu - minu), maxu/(maxu - minu)};

  // begin drawing nurbs
  gluBeginCurve(theNurb);

  // first enable texture coordinate mapping
  gluNurbsCurve(theNurb, 4, tknots, 1, &tcoords[0], 2, GL_MAP1_TEXTURE_COORD_1);

  // draw surface
  gluNurbsCurve(theNurb, numKnots, knots, 3, ctlpoints, order, GL_MAP1_VERTEX_3);
  gluEndCurve(theNurb);

  gluDeleteNurbsRenderer(theNurb);

  delete[] knots;
  delete[] ctlpoints;
}


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
