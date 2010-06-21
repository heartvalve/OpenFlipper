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
//  CLASS BSplineSurfaceNodeT - IMPLEMENTATION
//  Author:  Ellen Dekkers <dekkers@cs.rwth-aachen.de>
//
//=============================================================================

#define ACG_BSPLINESURFACENODET_C

//== INCLUDES =================================================================

#include "BSplineSurfaceNodeT.hh"
#include <ACG/GL/gl.hh>
#include <ACG/GL/GluError.hh>
#include <ACG/Utils/VSToolsT.hh>
#include <vector>


//== NAMESPACES ===============================================================

namespace ACG {
namespace SceneGraph {

//== IMPLEMENTATION ==========================================================


template <class BSplineSurface>
void
BSplineSurfaceNodeT<BSplineSurface>::
boundingBox(Vec3d& _bbMin, Vec3d& _bbMax)
{
  for (unsigned int i = 0; i < bsplineSurface_.n_control_points_m(); ++i)
  {
    for (unsigned int j = 0; j < bsplineSurface_.n_control_points_n(); ++j)
    {
      _bbMin.minimize(bsplineSurface_(i,j));
      _bbMax.maximize(bsplineSurface_(i,j));
    }
  }

  controlnet_color_ = Vec4f(34.0/255.0, 139.0/255.0, 34.0/255.0, 1.0);
  controlnet_highlight_color_ = Vec4f(0,1,0,1);

  surface_color_ = Vec4f(178.0/255.0, 34.0/255.0, 34.0/255.0, 1.0);
  surface_highlight_color_ = Vec4f(1, 69.0 / 255.0, 0, 1);
}


//----------------------------------------------------------------------------


template <class BSplineSurface>
DrawModes::DrawMode
BSplineSurfaceNodeT<BSplineSurface>::
availableDrawModes() const
{
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

  return drawModes;
}

//----------------------------------------------------------------------------

template <class BSplineSurface>
void
BSplineSurfaceNodeT<BSplineSurface>::
render(GLState& _state, bool _fill)
{
  if (render_bspline_surface_)
    drawGluNurbsMode(_state, _fill);

  if (render_control_net_)
    drawControlNet(_state);
}

//----------------------------------------------------------------------------

template <class BSplineSurface>
void
BSplineSurfaceNodeT<BSplineSurface>::
draw(GLState& _state, DrawModes::DrawMode _drawMode)
{
  glDepthFunc(depthFunc());

  glPushAttrib(GL_ENABLE_BIT);

  if (_drawMode & DrawModes::POINTS)
  {
    glDisable(GL_LIGHTING);
    glShadeModel(GL_FLAT);
    render( _state, false);
  }

  if (_drawMode & DrawModes::WIREFRAME)
  {
    glPushAttrib(GL_ENABLE_BIT);

    glDisable( GL_CULL_FACE );
    glDisable(GL_LIGHTING);
    glShadeModel(GL_FLAT);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    render( _state, false);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glPopAttrib();
  }


  if (_drawMode & DrawModes::HIDDENLINE)
  {
    Vec4f  clear_color = _state.clear_color();
    Vec4f  base_color  = _state.base_color();
    clear_color[3] = 1.0;

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glShadeModel(GL_FLAT);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    _state.set_base_color(clear_color);

    glDepthRange(0.01, 1.0);

    render( _state, true);

    glDepthRange(0.0, 1.0);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDepthFunc(GL_LEQUAL);
    _state.set_base_color(base_color);

    render( _state, false);

    glDepthFunc(depthFunc());
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }


  if ( ( _drawMode & DrawModes::SOLID_FLAT_SHADED ))
  {
    glEnable(GL_LIGHTING);
    glShadeModel(GL_FLAT);

    glEnable(GL_AUTO_NORMAL);
    glEnable(GL_NORMALIZE);

    glDepthRange(0.01, 1.0);

    render( _state, true);

    glDepthRange(0.0, 1.0);
  }


  if ( ( _drawMode & DrawModes::SOLID_SMOOTH_SHADED ) )
  {
    glEnable(GL_AUTO_NORMAL);
    glEnable(GL_NORMALIZE);

    glEnable(GL_LIGHTING);
    glShadeModel(GL_SMOOTH);
    glDepthRange(0.01, 1.0);

    render( _state, true);

    glDepthRange(0.0, 1.0);
  }

  if ( ( _drawMode & DrawModes::SOLID_PHONG_SHADED )  )
  {
    glEnable(GL_AUTO_NORMAL);
    glEnable(GL_NORMALIZE);

    glEnable(GL_LIGHTING);
    glShadeModel(GL_SMOOTH);
    glDepthRange(0.01, 1.0);

    render( _state, true);

    glDepthRange(0.0, 1.0);
  }


    // If in shader mode, just draw, as the shader has to be set by a shadernode above this node
  if ( (_drawMode & DrawModes::SOLID_SHADER )  ) {
    glEnable(GL_AUTO_NORMAL);
    glEnable(GL_NORMALIZE);

    glEnable(GL_LIGHTING);
    glShadeModel(GL_SMOOTH);
    glDepthRange(0.01, 1.0);

//     draw_faces(PER_VERTEX);
    render( _state, true);

    glDepthRange(0.0, 1.0);
  }


  glDepthFunc(GL_LESS);
  glPopAttrib();
}

//----------------------------------------------------------------------------

template <class BSplineSurface>
void
BSplineSurfaceNodeT<BSplineSurface>::
drawGluNurbsMode(GLState& _state, bool _fill)
{
  int numKnots_m = bsplineSurface_.n_knots_m();
  int numKnots_n = bsplineSurface_.n_knots_n();

  GLfloat *knots_m = new GLfloat[numKnots_m];
  for (int i = 0; i < numKnots_m; ++i)
    knots_m[i] = bsplineSurface_.get_knot_m(i);

  GLfloat *knots_n = new GLfloat[numKnots_n];
  for (int i = 0; i < numKnots_n; ++i)
    knots_n[i] = bsplineSurface_.get_knot_n(i);

  const int numCPs_m = bsplineSurface_.n_control_points_m();
  const int numCPs_n = bsplineSurface_.n_control_points_n();

  GLfloat *ctlpoints = new GLfloat[numCPs_m * numCPs_n * 3];
  for (int i = 0; i < numCPs_m; ++i)
  {
    for (int j = 0; j < numCPs_n; ++j)
    {
      Vec3d p = bsplineSurface_(i,j);
      int idx0 = i * numCPs_n * 3 + j * 3 + 0;
      int idx1 = i * numCPs_n * 3 + j * 3 + 1;
      int idx2 = i * numCPs_n * 3 + j * 3 + 2;
      ctlpoints[idx0] = (GLfloat)p[0];
      ctlpoints[idx1] = (GLfloat)p[1];
      ctlpoints[idx2] = (GLfloat)p[2];
    }
  }

  int order_m = bsplineSurface_.degree_m() + 1;
  int order_n = bsplineSurface_.degree_n() + 1;


  int lineWidth = (int)_state.line_width();
  glLineWidth(lineWidth);

  GLUnurbsObj *theNurb;
  theNurb = gluNewNurbsRenderer();

  #ifdef WIN32
    gluNurbsCallback(theNurb, GLU_ERROR, (void (__stdcall *)(void))(&nurbsErrorCallback) );
  #else
    gluNurbsCallback(theNurb, GLU_ERROR, (GLvoid (*)()) (&nurbsErrorCallback) );  
  #endif

  if (_fill)
    gluNurbsProperty(theNurb, GLU_DISPLAY_MODE, GLU_FILL);
  else
    gluNurbsProperty(theNurb, GLU_DISPLAY_MODE, GLU_OUTLINE_POLYGON );


  // set sampling method
  if (adaptive_sampling_)
  {
    // screen space -> adaptive subdivision
    gluNurbsProperty(theNurb, GLU_SAMPLING_TOLERANCE, 50.0);
  }
  else
  {
    #ifdef GLU_OBJECT_PARAMETRIC_ERROR
      // object space -> fixed (non-adaptive) sampling
      gluNurbsProperty(theNurb, GLU_SAMPLING_METHOD, GLU_OBJECT_PARAMETRIC_ERROR);
    #else
      gluNurbsProperty(theNurb, GLU_SAMPLING_METHOD, GLU_PARAMETRIC_ERROR);
    #endif
    gluNurbsProperty(theNurb, GLU_PARAMETRIC_TOLERANCE, 0.2);
  }


  gluBeginSurface(theNurb);
  gluNurbsSurface(theNurb, numKnots_m, knots_m, numKnots_n, knots_n, numCPs_n * 3, 3, ctlpoints, order_m, order_n, GL_MAP2_VERTEX_3);
  gluEndSurface(theNurb);

  gluDeleteNurbsRenderer(theNurb);

  delete[] knots_m;
  delete[] knots_n;
  delete[] ctlpoints;
}


//----------------------------------------------------------------------------

template <class BSplineSurface>
void
BSplineSurfaceNodeT<BSplineSurface>::
drawControlNet(GLState& _state)
{
  // draw control net
//   glPushAttrib(GL_ENABLE_BIT);
  glPushAttrib(GL_ALL_ATTRIB_BITS);

  glDisable( GL_CULL_FACE );
  glDisable(GL_LIGHTING);
  glShadeModel(GL_FLAT);
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  // draw points
  
  // draw selection
  if( bsplineSurface_.controlpoint_selections_available())
  {
    // save old values
    Vec4f base_color_old = _state.base_color();
    float point_size_old = _state.point_size();

    glColor(controlnet_highlight_color_);
    glPointSize(10);
//     glPointSize(point_size_old+2);

    glBegin(GL_POINTS);
    // draw control polygon
    for (unsigned int i = 0; i < bsplineSurface_.n_control_points_m(); ++i)
    {
      for (unsigned int j = 0; j < bsplineSurface_.n_control_points_n(); ++j)
      {
        if( bsplineSurface_.controlpoint_selection(i, j))
          glVertex(bsplineSurface_(i,j));
      }
    }
    glEnd();

    glPointSize(point_size_old);
    glColor( base_color_old );
  }

  // draw all points
  Vec4f base_color_old = _state.base_color();
  glColor(controlnet_color_);

  float point_size_old = _state.point_size();
  glPointSize(point_size_old + 4);
//   glPointSize(6);
  
  glBegin(GL_POINTS);
  for (unsigned int i = 0; i < bsplineSurface_.n_control_points_m(); ++i)
    for (unsigned int j = 0; j < bsplineSurface_.n_control_points_n(); ++j)
      glVertex(bsplineSurface_(i, j));
  glEnd();

  glColor( base_color_old );
  glPointSize((int)point_size_old);
  

  // draw line segments

  /*
  // draw selection
  if( bsplineSurface_.edge_selections_available())
  {
    // save old values
    Vec4f base_color_old = _state.base_color();
    float line_width_old = _state.line_width();

    glColor(controlnet_highlight_color_);
    glLineWidth(2*line_width_old);

    glBegin(GL_LINES);
    // draw bspline control net
    int num_edges_m = (int)(bsplineSurface_.n_control_points_m()) - 1;
    int num_edges_n = (int)(bsplineSurface_.n_control_points_n()) - 1;
    for (int i = 0; i < num_edges_m; ++i) // #edges
    {
      for (int j = 0; j < num_edges_n; ++j) // #edges
      {
        if( bsplineSurface_.edge_selection(i, j))
        {
          glVertex(bsplineSurface_(i,j));
          glVertex(bsplineSurface_(i+1, j));
        }
      }
    }
    glEnd();

    glLineWidth(line_width_old);
    glColor( base_color_old );
  }
*/
  // draw all line segments
  base_color_old = _state.base_color();
  glColor(controlnet_color_);

  float line_width_old = _state.line_width();
  glLineWidth(line_width_old+2.0);

  glBegin(GL_LINES);
  // draw bspline control net
  for (unsigned int i = 0; i < bsplineSurface_.n_control_points_m(); ++i)
  {
    for (int j = 0; j < (int)bsplineSurface_.n_control_points_n() - 1; ++j)
    {
      glVertex(bsplineSurface_(i, j  ));
      glVertex(bsplineSurface_(i, j+1));
    }
  }

  for (int j = 0; j < (int)bsplineSurface_.n_control_points_n(); ++j)
  {
    for (int i = 0; i < (int)bsplineSurface_.n_control_points_m() - 1; ++i)
    {
    glVertex(bsplineSurface_(i,  j));
    glVertex(bsplineSurface_(i+1,j));
    }
  }

  glEnd();

  glColor( base_color_old );
  glLineWidth(line_width_old);


  glPopAttrib();
}

//----------------------------------------------------------------------------

template <class BSplineSurface>
void
BSplineSurfaceNodeT<BSplineSurface>::
pick(GLState& _state, PickTarget _target)
{
  switch (_target)
  {
    case PICK_VERTEX:
    {
      if(render_control_net_)
      {
        _state.pick_set_maximum (bsplineSurface_.n_control_points_m() * bsplineSurface_.n_control_points_n());
        pick_vertices(_state);
      }
      break;
    }

    case PICK_FACE:
    {
      _state.pick_set_maximum (1);
      pick_surface(_state, 0);
      break;
    }

    case PICK_SPLINE:
    {
      _state.pick_set_maximum( pick_texture_res_ * pick_texture_res_);
      pick_spline(_state);
      break;
    }

    case PICK_ANYTHING:
    {
      _state.pick_set_maximum (bsplineSurface_.n_control_points_m() * bsplineSurface_.n_control_points_n() + 1);
      pick_vertices(_state);
      pick_surface(_state, bsplineSurface_.n_control_points_m() * bsplineSurface_.n_control_points_n());
      break;
    }

    default:
      break;
  }
}

//----------------------------------------------------------------------------

template <class BSplineSurface>
void
BSplineSurfaceNodeT<BSplineSurface>::
pick_vertices( GLState& _state )
{
  // radius in pixels
  int psize = 7;

  _state.pick_set_name (0);

  for (unsigned int i = 0; i < bsplineSurface_.n_control_points_m(); ++i)
  {
    for (unsigned int j = 0; j < bsplineSurface_.n_control_points_n(); ++j)
    {
      _state.pick_set_name (i * bsplineSurface_.n_control_points_n() + j);

      // compute 3d radius of sphere
      Vec3d window_pos = _state.project( (Vec3d)bsplineSurface_(i,j) );
      int px = round( window_pos[0]);
      int py = round( window_pos[1]);
      double angle = acos(_state.viewing_direction(px, py).normalize()|_state.viewing_direction(px+psize, py).normalize());
      double l = (_state.eye() - (Vec3d)bsplineSurface_(i,j)).norm();
      double r = l*tan(angle);

      // draw 3d sphere
      draw_sphere( bsplineSurface_(i,j), r, _state);
    }
  }
}

//----------------------------------------------------------------------------

template <class BSplineSurface>
void
BSplineSurfaceNodeT<BSplineSurface>::
pick_spline( GLState& _state )
{
  glPushAttrib(GL_ALL_ATTRIB_BITS);

  glEnable(GL_TEXTURE_2D);

  if( _state.pick_current_index () != pick_texture_baseidx_)
  {
    pick_texture_baseidx_ = _state.pick_current_index();
    pick_create_texture( _state);
  }
  else
    glBindTexture( GL_TEXTURE_2D, pick_texture_idx_);


  pick_draw_textured_nurbs( _state);

  glBindTexture( GL_TEXTURE_2D, 0);
  glDisable(GL_TEXTURE_2D);
  glPopAttrib( );
}

//----------------------------------------------------------------------------

template <class BSplineSurface>
void
BSplineSurfaceNodeT<BSplineSurface>::
pick_surface( GLState& _state, unsigned int _offset )
{
  bool sampling_mode_backup = adaptive_sampling_;
  adaptive_sampling_ = false;

//   _state.pick_set_maximum( 1 ); // pick the whole surface
  _state.pick_set_name ( _offset );
  drawGluNurbsMode( _state, true);

  adaptive_sampling_ = sampling_mode_backup;
}

//----------------------------------------------------------------------------

template <class BSplineSurface>
void
BSplineSurfaceNodeT<BSplineSurface>::
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

template <class BSplineSurface>
void
BSplineSurfaceNodeT<BSplineSurface>::
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

template <class BSplineSurface>
void
BSplineSurfaceNodeT<BSplineSurface>::
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

template <class BSplineSurface>
void
BSplineSurfaceNodeT<BSplineSurface>::
pick_init_texturing( )
{
  pick_texture_res_     = 256;
  pick_texture_baseidx_ = 0;

  // generate texture index
  glGenTextures( 1, &pick_texture_idx_ );
  // bind texture as current
  glBindTexture( GL_TEXTURE_2D, pick_texture_idx_ );
  // do not blend colors (else color picking breaks!)
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
  // avoid aliasing at patch boundaries
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
  // GL_REPLACE to avoid smearing colors (else color picking breaks!)
  glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  // unbind current texture
  glBindTexture( GL_TEXTURE_2D, 0);
}

//----------------------------------------------------------------------------

template <class BSplineSurface>
void
BSplineSurfaceNodeT<BSplineSurface>::
pick_create_texture( GLState& _state)
{
  QImage b(pick_texture_res_, pick_texture_res_, QImage::Format_ARGB32);
    
  // fill with colors
  int cur_idx=0;
  for( int i = 0; i < pick_texture_res_; ++i)
  {
    for( int j = pick_texture_res_ - 1; j >= 0; j--)
    {
      Vec4uc cur_col( _state.pick_get_name_color (cur_idx) );
      b.setPixel ( i,j, qRgba((int)cur_col[0], (int)cur_col[1], (int)cur_col[2], (int)cur_col[3]));
      cur_idx++;
    }
  }

/*
  // creates checkerboard texture for debugging purposes
  
  bool odd_row = true;
  bool odd_col = true;
  bool green = true;
  for( int i = 0; i < pick_texture_res_; ++i)
  {
    if (i % 20 == 0)
      odd_row = !odd_row;
    
    odd_col = true;
    for( int j = 0; j < pick_texture_res_; ++j)
    {
      if (j % 20 == 0)
        odd_col = !odd_col;
      
      green = (odd_row && odd_col) || (!odd_row && !odd_col);      
      
      if (green)
        b.setPixel (i, j, qRgba(0, 255, 0, 255));
      else
        b.setPixel (i, j, qRgba(255, 0, 255, 255));
    }
  }
*/

  // debug, output image (usually does not look as expected :\ )
  //b.save("/tmp/uvtexture.png");
//   b.save("surfaceTexture.png", "PNG");
  
  pick_texture_image_ = QGLWidget::convertToGLFormat( b );

  // bind texture 
  glBindTexture( GL_TEXTURE_2D, pick_texture_idx_ );
  glTexImage2D(  GL_TEXTURE_2D,
                 0, GL_RGBA, pick_texture_image_.width(), pick_texture_image_.height(), 
                 0, GL_RGBA, GL_UNSIGNED_BYTE, pick_texture_image_.bits() );
}

//----------------------------------------------------------------------------

template <class BSplineSurface>
void
BSplineSurfaceNodeT<BSplineSurface>::
pick_draw_textured_nurbs( GLState& _state)
{
  int numKnots_m = bsplineSurface_.n_knots_m();
  int numKnots_n = bsplineSurface_.n_knots_n();

  const int numCPs_m = bsplineSurface_.n_control_points_m();
  const int numCPs_n = bsplineSurface_.n_control_points_n();

  int order_m = bsplineSurface_.degree_m() + 1;
  int order_n = bsplineSurface_.degree_n() + 1;

  GLfloat *knots_m = new GLfloat[numKnots_m];
  for (int i = 0; i < numKnots_m; ++i)
    knots_m[i] = bsplineSurface_.get_knot_m(i);

  GLfloat *knots_n = new GLfloat[numKnots_n];
  for (int i = 0; i < numKnots_n; ++i)
    knots_n[i] = bsplineSurface_.get_knot_n(i);

//   std::cout << "knots_m: " << bsplineSurface_.get_knotvector_m() << std::endl;
//   std::cout << "knots_n: " << bsplineSurface_.get_knotvector_n() << std::endl;
  
  GLfloat *ctlpoints = new GLfloat[numCPs_m * numCPs_n * 3];
  for (int i = 0; i < numCPs_m; ++i)
  {
    for (int j = 0; j < numCPs_n; ++j)
    {
      Vec3d p = bsplineSurface_(i,j);
      int idx0 = i * numCPs_n * 3 + j * 3 + 0;
      int idx1 = i * numCPs_n * 3 + j * 3 + 1;
      int idx2 = i * numCPs_n * 3 + j * 3 + 2;
      ctlpoints[idx0] = (GLfloat)p[0];
      ctlpoints[idx1] = (GLfloat)p[1];
      ctlpoints[idx2] = (GLfloat)p[2];
    }
  }

  GLUnurbsObj *theNurb;
  theNurb = gluNewNurbsRenderer();

  #ifdef WIN32
    gluNurbsCallback(theNurb, GLU_ERROR, (void (__stdcall *)(void))(&nurbsErrorCallback) );
  #else
    gluNurbsCallback(theNurb, GLU_ERROR, (GLvoid (*)()) (&nurbsErrorCallback) );  
  #endif

  // draw filled
  gluNurbsProperty(theNurb, GLU_DISPLAY_MODE, GLU_FILL);

  #ifdef GLU_OBJECT_PARAMETRIC_ERROR
    // object space -> fixed (non-adaptive) sampling
    gluNurbsProperty(theNurb, GLU_SAMPLING_METHOD, GLU_OBJECT_PARAMETRIC_ERROR);
  #else
    gluNurbsProperty(theNurb, GLU_SAMPLING_METHOD,   GLU_PARAMETRIC_ERROR);
  #endif

  gluNurbsProperty(theNurb, GLU_PARAMETRIC_TOLERANCE, 0.2);

  // get min/max knots of domain defining patch (partition of unity)
  float  minu( knots_m[bsplineSurface_.degree_m()]);
  float  minv( knots_n[bsplineSurface_.degree_n()]);
  float  maxu( knots_m[numKnots_m  - order_m]);
  float  maxv( knots_n[numKnots_n  - order_n]);
//   std::cout << "minu = " << minu << ", maxu = " << maxu << std::endl;
//   std::cout << "minv = " << minv << ", maxv = " << maxv << std::endl;

  // control points of 2d texture ((0,0), (0,1), (1,0), (1,1) )
  GLfloat   tcoords[8] = {0.0, 0.0, 0.0, 1.0, 1.0, 0.0, 1.0, 1.0};

  // knots of domain, over which tcoords shall be linearly interpolated
  GLfloat   tknots[4] = {minu, minu, maxu, maxu};
  GLfloat   sknots[4] = {minv, minv, maxv, maxv};

  // begin drawing nurbs
  gluBeginSurface(theNurb);

  // first enable texture coordinate mapping
  gluNurbsSurface(theNurb, 4, tknots, 4, sknots, 2*2, 2, &tcoords[0], 2, 2, GL_MAP2_TEXTURE_COORD_2); 

  // draw surface
  gluNurbsSurface(theNurb, numKnots_m, knots_m, numKnots_n, knots_n, numCPs_n * 3, 3, ctlpoints, order_m, order_n, GL_MAP2_VERTEX_3);
  gluEndSurface(theNurb);

  gluDeleteNurbsRenderer(theNurb);

  delete[] knots_m;
  delete[] knots_n;
  delete[] ctlpoints;
}

//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
