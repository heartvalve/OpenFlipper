/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2011 by Computer Graphics Group, RWTH Aachen       *
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
//  CLASS BSplineCurveNodeT - IMPLEMENTATION
//  Author:  Ellen Dekkers <dekkers@cs.rwth-aachen.de>
//
//=============================================================================

#define ACG_BSPLINECURVENODET_C

//== INCLUDES =================================================================

#include "BSplineCurveNodeT.hh"
#include <ACG/GL/gl.hh>
#include <ACG/GL/GluError.hh>
#include <ACG/Utils/VSToolsT.hh>
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
draw(GLState& _state, const DrawModes::DrawMode& _drawMode)
{  
  glPushAttrib(GL_ENABLE_BIT);
  
  // check if textures are still valid
  if (    bspline_selection_draw_mode_ == CONTROLPOINT
       && controlPointSelectionTexture_valid_ == false)
    updateControlPointSelectionTexture(_state);
  
  if (    bspline_selection_draw_mode_ == KNOTVECTOR
       && knotVectorSelectionTexture_valid_ == false)
    updateKnotVectorSelectionTexture(_state);
  
  
  if (_drawMode & DrawModes::WIREFRAME)
  {
    ACG::GLState::disable( GL_CULL_FACE );
    
    if (bspline_draw_mode_ == NORMAL)
    {
      ACG::GLState::disable(GL_LIGHTING);
    }
    else if (bspline_draw_mode_ == FANCY)
    {
//       ACG::GLState::enable(GL_AUTO_NORMAL);
//       ACG::GLState::enable(GL_NORMALIZE);
      glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
      ACG::GLState::enable( GL_COLOR_MATERIAL );
      ACG::GLState::enable(GL_LIGHTING);
      ACG::GLState::shadeModel(GL_SMOOTH);
    }

    render( _state, false, _drawMode);
  }
  else if (_drawMode & DrawModes::POINTS)
  {
    ACG::GLState::disable(GL_LIGHTING);
    ACG::GLState::shadeModel(GL_FLAT);
    
    render( _state, false, _drawMode);
  }

  
  glPopAttrib();
}

//----------------------------------------------------------------------------

template <class BSplineCurve>
void
BSplineCurveNodeT<BSplineCurve>::
render(GLState& _state, bool /*_fill*/, DrawModes::DrawMode _drawMode)
{
  // draw the control polygon (includes selection on the polygon)
  if (render_control_polygon_)
  {
    if (bspline_draw_mode_ == NORMAL)
      drawControlPolygon(_drawMode, _state);
    else if (bspline_draw_mode_ == FANCY)
      drawFancyControlPolygon(_drawMode, _state);
  }

  
  // draw the spline curve itself, depending on the type of visualization
  if ((_drawMode & DrawModes::WIREFRAME) && render_bspline_curve_)
  {
    if (bspline_selection_draw_mode_ == NONE)
    {
      if (bspline_draw_mode_ == NORMAL)
        drawCurve(_state);
      else
        drawFancyCurve(_state);
    }
    else
    {
      if (bspline_selection_draw_mode_ == CONTROLPOINT) {
        drawTexturedCurve(_state, cp_selection_texture_idx_);
      }
      else if (bspline_selection_draw_mode_ == KNOTVECTOR) {
        drawTexturedCurve(_state, knot_selection_texture_idx_);
      }
    }
  }
}

//----------------------------------------------------------------------------

template <class BSplineCurve>
void
BSplineCurveNodeT<BSplineCurve>::
drawCurve(GLState& /*_state*/)
{
  int numKnots = bsplineCurve_.n_knots();
  GLfloat *knots = new GLfloat[numKnots];
  for (int i = 0; i < numKnots; ++i)
    knots[i] = bsplineCurve_.get_knot(i);

  const int numCPs = bsplineCurve_.n_control_points();
  
  // check for incomplete curve
  if (numCPs < (int)bsplineCurve_.degree() + 1) {
    delete[] knots;
    return;
  }

  GLfloat *ctlpoints = new GLfloat[numCPs * 3];
  for (int i = 0; i < numCPs; ++i)
  {
    Vec3d p = bsplineCurve_.get_control_point(i);
    ctlpoints[i * 3 + 0] = (GLfloat)p[0];
    ctlpoints[i * 3 + 1] = (GLfloat)p[1];
    ctlpoints[i * 3 + 2] = (GLfloat)p[2];
  }

  int order = bsplineCurve_.degree() + 1;
 
  GLUnurbsObj *theNurb;
  theNurb = gluNewNurbsRenderer();

  #ifdef WIN32
    gluNurbsCallback(theNurb, GLU_ERROR, (void (__stdcall *)(void))(&nurbsErrorCallback) );
  #else
    gluNurbsCallback(theNurb, GLU_ERROR, (GLvoid (*)()) (&nurbsErrorCallback) );  
  #endif

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
drawFancyCurve(GLState& _state)
{
  // draw the curve
//  double cylinderRadius = _state.line_width() * 0.05;
  double cylinderRadius = _state.line_width() * 0.2;

  for (int i = 0; i < (int)curve_samples_.size() - 1; ++i)
  {
    Vec3d p      = curve_samples_[i].first;
    Vec3d p_next = curve_samples_[i+1].first;
    draw_cylinder(p, p_next - p, cylinderRadius, _state);
  }
}

//----------------------------------------------------------------------------

template <class BSplineCurve>
ACG::Vec4f
BSplineCurveNodeT<BSplineCurve>::
generateHighlightColor(ACG::Vec4f _color)
{
  float c1 = _color[0]*1.5; 
  c1 = c1 > 1.0 ? 1.0 : c1;
  
  float c2 = _color[1]*1.5; 
  c2 = c2 > 1.0 ? 1.0 : c2;
  
  float c3 = _color[2]*1.5; 
  c3 = c3 > 1.0 ? 1.0 : c3;
  
  return Vec4f( c1, c2, c3, _color[3]);
}

//----------------------------------------------------------------------------

template <class BSplineCurve>
void
BSplineCurveNodeT<BSplineCurve>::
drawControlPolygon(DrawModes::DrawMode _drawMode, GLState& _state)
{
  // remember current base color
  Vec4f base_color_old = _state.base_color();
  
  // draw line segments
  if (_drawMode & DrawModes::WIREFRAME)
  {
    // draw selection
    if( bsplineCurve_.edge_selections_available())
    {
      // save old values
      float line_width_old = _state.line_width();

      glColor(generateHighlightColor(polygon_color_));
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
    }

    // draw all line segments
    glColor(polygon_color_);

//     float line_width_old = _state.line_width();
//     glLineWidth(line_width_old+2.0);


    // draw bspline control polygon
    glBegin(GL_LINE_STRIP);
    
    for (unsigned int i = 0; i< bsplineCurve_.n_control_points(); ++i)
      glVertex(bsplineCurve_.get_control_point(i % bsplineCurve_.n_control_points()));
    glEnd();
    
//     glLineWidth(line_width_old);
  }
  
  
  // draw points
  if ((_drawMode & DrawModes::POINTS) && render_control_polygon_)
  {
    // draw selection
    if( bsplineCurve_.controlpoint_selections_available())
    {
      // save old values
      float point_size_old = _state.point_size();

      glColor(generateHighlightColor(polygon_color_));
      glPointSize(10);

      glBegin(GL_POINTS);
      // draw control polygon
      for (unsigned int i = 0; i < bsplineCurve_.n_control_points(); ++i)
      {
        if( bsplineCurve_.controlpoint_selection(i))
          glVertex(bsplineCurve_.get_control_point(i) );
      }
      glEnd();

      glPointSize(point_size_old);
    }

    // draw all points
    glColor(polygon_color_);
    float point_size_old = _state.point_size();
    glPointSize(point_size_old + 4);

    glBegin(GL_POINTS);
    for (unsigned int i=0; i< bsplineCurve_.n_control_points(); ++i)
      glVertex(bsplineCurve_.get_control_point(i));
    glEnd();

    glPointSize(point_size_old);
  }
  
  // reset olf color
  glColor( base_color_old );
}

//----------------------------------------------------------------------------

template <class BSplineCurve>
void
BSplineCurveNodeT<BSplineCurve>::
drawFancyControlPolygon(DrawModes::DrawMode _drawMode, GLState& _state)
{
  // save old base color
  Vec4f base_color_old = _state.base_color();
  
  // draw line segments
  if (_drawMode & DrawModes::WIREFRAME)
  {
//    double cylinderRadius = _state.line_width() * 0.05;
    double cylinderRadius = _state.line_width() * 0.2;

    // draw selection
    if( bsplineCurve_.edge_selections_available())
    {
      glColor(generateHighlightColor(polygon_color_));

      // draw bspline control polygon
      for (int i = 0; i < (int)bsplineCurve_.n_control_points()-1; ++i) // #edges
      {
        if (bsplineCurve_.edge_selection(i))
        {
          Point p    = bsplineCurve_.get_control_point(i);
          Point axis = bsplineCurve_.get_control_point(i+1) - bsplineCurve_.get_control_point(i);
          draw_cylinder(p, axis, cylinderRadius, _state);
        }
      }
    }

    // draw all line segments
    glColor(polygon_color_);

    // draw bspline control polygon
    for (unsigned int i = 0; i < bsplineCurve_.n_control_points() - 1; ++i)
    {
      Point p    = bsplineCurve_.get_control_point(i);
      Point axis = bsplineCurve_.get_control_point(i+1) - bsplineCurve_.get_control_point(i);
      draw_cylinder(p, axis, cylinderRadius, _state);
    }
  } // end of if wireframe
  
  
  // draw points
  if ((_drawMode & DrawModes::POINTS) && render_control_polygon_)
  {
    if (bsplineCurve_.n_control_points() == 0)
      return;
    
    // radius of sphere
//    double sphereRadius = _state.point_size() * 0.05;
    double sphereRadius = _state.point_size() * 0.25;

    // draw selection
    if( bsplineCurve_.controlpoint_selections_available())
    {
      glColor(generateHighlightColor(polygon_color_));

      // draw control polygon
      for (unsigned int i = 0; i < bsplineCurve_.n_control_points(); ++i)
        if (bsplineCurve_.controlpoint_selection(i))
          draw_sphere(bsplineCurve_.get_control_point(i), sphereRadius, _state, fancySphere_);
    }

    // draw all points
    glColor(polygon_color_);
    
    for (unsigned int i = 0; i < bsplineCurve_.n_control_points(); ++i)
      draw_sphere(bsplineCurve_.get_control_point(i), sphereRadius, _state, fancySphere_);
  }
  
   // reset color
   glColor( base_color_old );
}

//----------------------------------------------------------------------------

template <class BSplineCurve>
void
BSplineCurveNodeT<BSplineCurve>::
drawTexturedCurve(GLState& _state, GLuint _texture_idx)
{   
  glPushAttrib(GL_ALL_ATTRIB_BITS);
  ACG::GLState::enable( GL_COLOR_MATERIAL );
  glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
  
  ACG::GLState::enable(GL_TEXTURE_2D);
  
  ACG::GLState::bindTexture( GL_TEXTURE_2D, _texture_idx);
  
  // blend colors (otherwise lighting does not affect the texture)
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  // avoid aliasing at patch boundaries
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
  // GL_MODULATE to include lighting effects
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  
  float line_width_old = _state.line_width();
  draw_textured_nurbs( _state);
  glLineWidth(line_width_old);

  ACG::GLState::bindTexture( GL_TEXTURE_2D, 0);
  ACG::GLState::disable(GL_TEXTURE_2D);
  ACG::GLState::disable( GL_COLOR_MATERIAL );
  glPopAttrib( );
}

//----------------------------------------------------------------------------

template <class BSplineCurve>
void
BSplineCurveNodeT<BSplineCurve>::
updateGeometry()
{
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

    case PICK_SPLINE:
    {
      _state.pick_set_maximum (pick_texture_res_ );
      pick_spline(_state, 0);
      break;
    }


    case PICK_ANYTHING:
    {
      _state.pick_set_maximum (bsplineCurve_.n_control_points() + pick_texture_res_);
      pick_vertices(_state);
      pick_spline(_state, bsplineCurve_.n_control_points());
      break;
    }

    default:
      _state.pick_set_maximum(1);
      _state.pick_set_name(0);
      break;
  }
}

//----------------------------------------------------------------------------

template <class BSplineCurve>
void
BSplineCurveNodeT<BSplineCurve>::
pick_vertices( GLState& _state )
{
  // radius in pixels
  int psize = 7;

//   _state.pick_set_name (0);

  for (unsigned int i = 0; i < bsplineCurve_.n_control_points(); ++i)
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
    draw_sphere(bsplineCurve_.get_control_point(i), r, _state, sphere_);
  }
}

//----------------------------------------------------------------------------

template <class BSplineCurve >
void
BSplineCurveNodeT<BSplineCurve>::
pick_spline( GLState& _state, unsigned int _offset )
{
  glPushAttrib(GL_ALL_ATTRIB_BITS);

  ACG::GLState::enable(GL_TEXTURE_2D);
//   ACG::GLState::enable(GL_TEXTURE_1D);
//   ACG::GLState::enable(GL_MAP1_TEXTURE_COORD_1);


  if( _state.pick_current_index () + _offset != pick_texture_baseidx_)
  {
    pick_texture_baseidx_ = _state.pick_current_index() + _offset;
    pick_create_texture( _state);
  }
  else
  {
    // do not blend colors (else color picking breaks!)
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    // avoid aliasing at patch boundaries
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    // GL_REPLACE to avoid smearing colors (else color picking breaks!)
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  
    ACG::GLState::bindTexture( GL_TEXTURE_2D, pick_texture_idx_);
//     ACG::GLState::bindTexture( GL_TEXTURE_1D, pick_texture_idx_);
  }

  float line_width_old = _state.line_width();
  glLineWidth(10);
  draw_textured_nurbs( _state);
  glLineWidth(line_width_old);

//   ACG::GLState::bindTexture( GL_TEXTURE_1D, 0);
//   ACG::GLState::disable(GL_TEXTURE_1D);
//   ACG::GLState::disable(GL_MAP1_TEXTURE_COORD_1);

  ACG::GLState::bindTexture( GL_TEXTURE_2D, 0);
  ACG::GLState::disable(GL_TEXTURE_2D);

  glPopAttrib( );
}

//----------------------------------------------------------------------------

template <class BSplineCurve>
void
BSplineCurveNodeT<BSplineCurve>::
draw_sphere( const Point& _p0, double _r, GLState& _state, GLSphere* _sphere)
{
  // draw 3d sphere
  _state.push_modelview_matrix();
  _state.translate( _p0[0], _p0[1], _p0[2]);

  _sphere->draw(_state,_r);

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

  cylinder_->setBottomRadius(_r);
  cylinder_->setTopRadius(_r);
  cylinder_->draw(_state,_axis.norm());

  _state.pop_modelview_matrix();
}

//----------------------------------------------------------------------------

template <class BSplineCurve>
void
BSplineCurveNodeT<BSplineCurve>::
updateControlPointSelectionTexture(GLState& _state)
{
  create_cp_selection_texture(_state);
  controlPointSelectionTexture_valid_ = true;
}

//----------------------------------------------------------------------------

template <class BSplineCurve>
void
BSplineCurveNodeT<BSplineCurve>::
updateKnotVectorSelectionTexture(GLState& _state)
{
  create_knot_selection_texture(_state);
  knotVectorSelectionTexture_valid_ = true;
}

//----------------------------------------------------------------------------

template <class BSplineCurve>
void
BSplineCurveNodeT<BSplineCurve>::
selection_init_texturing(GLuint & _texture_idx)
{
  // generate texture index
  glGenTextures( 1, &_texture_idx );
  // bind texture as current
  ACG::GLState::bindTexture( GL_TEXTURE_2D, _texture_idx );
  // blend colors (otherwise lighting does not affect the texture)
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  // avoid aliasing at patch boundaries
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
  // GL_MODULATE to include lighting effects
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  // unbind current texture
  ACG::GLState::bindTexture( GL_TEXTURE_2D, 0);
}

//----------------------------------------------------------------------------

template <class BSplineCurve>
void
BSplineCurveNodeT<BSplineCurve>::
create_cp_selection_texture(GLState& /*_state*/)
{
  if (bsplineCurve_.n_knots() == 0)
    return;
  
  if(cp_selection_texture_idx_ == 0)
      selection_init_texturing(cp_selection_texture_idx_);
  
  QImage b(cp_selection_texture_res_, 2, QImage::Format_ARGB32);
  
  int degree   = bsplineCurve_.degree();
  int numKnots = bsplineCurve_.n_knots();
  
  double minu  = bsplineCurve_.get_knot( degree );
  double maxu  = bsplineCurve_.get_knot( numKnots - degree -1 );
  double diffu = maxu - minu;
  if (diffu == 0.0) return;

  // get the colors to create the texture
//   Vec4f curveColor     = _state.base_color();
//   Vec4f highlightColor = generateHighlightColor(curveColor);
  Vec4f curveColor     = curve_color_;
  Vec4f highlightColor = curve_highlight_color_;
  

  int texelIdx = 0;
  for ( int m = 0; m < cp_selection_texture_res_; ++m)
  {
    double step_m = (double)m / (double)cp_selection_texture_res_;
    double u = step_m * diffu;
  
    // get the span and check which knots are selected
    ACG::Vec2i span = bsplineCurve_.span(u);
    // check for incomple spline 
    if (span[0] < 0 || span[1] < 0) 
      return; 

    float alpha = 0.0; // blends between curve and highlight colors
    for (int i = 0; i < degree+1; ++i) // degree+1 basis functions (those in the span) contribute
    {
      int idx = span[0] + i;
    
      // basis functions sum up to 1. hence, we only have to sum up those with selected control point to get the blending weight
      if (bsplineCurve_.controlpoint_selected(idx))
        alpha += bsplineCurve_.basisFunction(idx, degree, u);
    }
  
    // compute color
    Vec4f color =  curveColor * (1.0 - alpha) + highlightColor * alpha;

    // fill texture
    b.setPixel (texelIdx, 0, qRgba((int)(color[0]*255.0), (int)(color[1]*255.0), (int)(color[2]*255.0), 255));
    b.setPixel (texelIdx, 1, qRgba((int)(color[0]*255.0), (int)(color[1]*255.0), (int)(color[2]*255.0), 255));
    
    ++texelIdx;
  }
  
  // debug, output image
  //b.save("curveCPSelectionTexture.png", "PNG");
  
  cp_selection_texture_image_ = QGLWidget::convertToGLFormat( b );

  // bind texture 
  ACG::GLState::bindTexture( GL_TEXTURE_2D, cp_selection_texture_idx_ );
  glTexImage2D(  GL_TEXTURE_2D,
                 0, GL_RGBA, cp_selection_texture_image_.width(), cp_selection_texture_image_.height(), 
                 0, GL_RGBA, GL_UNSIGNED_BYTE, cp_selection_texture_image_.bits() );
}

//----------------------------------------------------------------------------

template <class BSplineCurve>
void
BSplineCurveNodeT<BSplineCurve>::
create_knot_selection_texture(GLState& /*_state*/)
{
  if (bsplineCurve_.n_knots() == 0)
    return;
  
  if(knot_selection_texture_idx_ == 0)
      selection_init_texturing(knot_selection_texture_idx_);
  
  QImage b(knot_selection_texture_res_, 2, QImage::Format_ARGB32);

  int degree   = bsplineCurve_.degree();
  int numKnots = bsplineCurve_.n_knots();
  
  double minu  = bsplineCurve_.get_knot( degree );
  double maxu  = bsplineCurve_.get_knot( numKnots - degree -1 );
  double diffu = maxu - minu;
  if (diffu == 0.0) return;

  int texelIdx = 0;
  
  // if a knot is selected, select all knots in the span of this knot, too
  std::vector<bool> selectedKnotSpans(numKnots, false);  
  for (int i = 0; i < numKnots; ++i)
  {
    if (bsplineCurve_.get_knotvector_ref()->selection(i))
    {
      // get the span and check which knots are selected
      ACG::Vec2i span = bsplineCurve_.span(bsplineCurve_.get_knot(i));
      // check for incomple spline
      if (span[0] < 0 || span[1] < 0)
        return;

      for(int j = span[0]; j <= span[1]+degree; ++j)
        selectedKnotSpans[j] = true;
    }
  }

//   Vec4f curveColor     = _state.base_color();
//   Vec4f highlightColor = generateHighlightColor(curveColor);
  Vec4f curveColor     = curve_color_;
  Vec4f highlightColor = curve_highlight_color_;

  for ( int m = 0; m < knot_selection_texture_res_; ++m)
  {
    double step_m = (double)m / (double)knot_selection_texture_res_;
    double u = step_m * diffu;
  
    Vec4f color;
    Vec2i interval = bsplineCurve_.interval(u);
    // check if highlighted
    if (selectedKnotSpans[interval[0]] && selectedKnotSpans[interval[1]])
      color = highlightColor;
    else
      color = curveColor;

    // fill texture
    b.setPixel (texelIdx, 0, qRgba((int)(color[0]*255.0), (int)(color[1]*255.0), (int)(color[2]*255.0), 255));
    b.setPixel (texelIdx, 1, qRgba((int)(color[0]*255.0), (int)(color[1]*255.0), (int)(color[2]*255.0), 255));
    
    ++texelIdx;
  }
  
  // debug, output image
  //b.save("curveKnotSelectionTexture.png", "PNG");
  
  knot_selection_texture_image_ = QGLWidget::convertToGLFormat( b );

  // bind texture 
  ACG::GLState::bindTexture( GL_TEXTURE_2D, knot_selection_texture_idx_ );
  glTexImage2D(  GL_TEXTURE_2D,
                 0, GL_RGBA, knot_selection_texture_image_.width(), knot_selection_texture_image_.height(), 
                 0, GL_RGBA, GL_UNSIGNED_BYTE, knot_selection_texture_image_.bits() );
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
  ACG::GLState::bindTexture( GL_TEXTURE_2D, pick_texture_idx_ );
  // do not blend colors (else color picking breaks!)
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
  // avoid aliasing at patch boundaries
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
  // GL_REPLACE to avoid smearing colors (else color picking breaks!)
  glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  // unbind current texture
  ACG::GLState::bindTexture( GL_TEXTURE_2D, 0);
}

//----------------------------------------------------------------------------

template <class BSplineCurve>
void
BSplineCurveNodeT<BSplineCurve>::
pick_create_texture( GLState& _state)
{
  if(pick_texture_idx_ == 0)
    pick_init_texturing();
    
  QImage b(pick_texture_res_, 2, QImage::Format_ARGB32);
    
  // fill with colors
  int cur_idx=0;
  for( int i = 0; i < pick_texture_res_; ++i)
  {
    Vec4uc cur_col( _state.pick_get_name_color (cur_idx) );
    b.setPixel (i, 0, qRgba((int)cur_col[0], (int)cur_col[1], (int)cur_col[2], (int)cur_col[3]));
    b.setPixel (i, 1, qRgba((int)cur_col[0], (int)cur_col[1], (int)cur_col[2], (int)cur_col[3]));
    cur_idx++;
  }

/*
  // create stripe or checkerboard texture
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
      
//       green = (odd_row && odd_col) || (!odd_row && !odd_col);      // checkerboard texture
      green = odd_row; // stripe texture
      if (green)
        b.setPixel (i, j, qRgba(0, 255, 0, 255));
      else
        b.setPixel (i, j, qRgba(255, 0, 255, 255));
    }
  }
*/

  // debug, output image
//   b.save("curveTexture.png", "PNG");
  
  pick_texture_image_ = QGLWidget::convertToGLFormat( b );

  // bind texture 
  ACG::GLState::bindTexture( GL_TEXTURE_2D, pick_texture_idx_ );
  glTexImage2D(  GL_TEXTURE_2D,
                 0, GL_RGBA, pick_texture_image_.width(), pick_texture_image_.height(), 
                 0, GL_RGBA, GL_UNSIGNED_BYTE, pick_texture_image_.bits() );
}

//----------------------------------------------------------------------------

template <class BSplineCurve>
void
BSplineCurveNodeT<BSplineCurve>::
draw_textured_nurbs( GLState& /*_state*/)
{
  int numKnots     = bsplineCurve_.n_knots();
  const int numCPs = bsplineCurve_.n_control_points();
  int order        = bsplineCurve_.degree() + 1;

  // gluNurbsSurface seems to crash for knots = 0
  if (!numKnots)
    return;

  // get knotvector
  GLfloat *knots = new GLfloat[numKnots];
  for (int i = 0; i < numKnots; ++i)
    knots[i] = bsplineCurve_.get_knot(i);

  int numCPs_dummy = 2;
  GLfloat *ctlpoints = new GLfloat[numCPs * numCPs_dummy * 3]; // dummy cps = 2
  for (int i = 0; i < numCPs; ++i)
  {
    Vec3d p = bsplineCurve_.get_control_point(i);
    
    for (int j = 0; j < numCPs_dummy; ++j)
    {
      int idx0 = i * numCPs_dummy * 3 + j * 3 + 0;
      int idx1 = i * numCPs_dummy * 3 + j * 3 + 1;
      int idx2 = i * numCPs_dummy * 3 + j * 3 + 2;
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
  // use GLU_OUTLINE_POLYGON instead of GLU_FILL since otherwise linewith is 0 when rendered from the side
//   gluNurbsProperty(theNurb, GLU_DISPLAY_MODE, GLU_FILL);
  gluNurbsProperty(theNurb, GLU_DISPLAY_MODE, GLU_OUTLINE_POLYGON ); 

  #ifdef GLU_OBJECT_PARAMETRIC_ERROR
    // object space -> fixed (non-adaptive) sampling
    gluNurbsProperty(theNurb, GLU_SAMPLING_METHOD, GLU_OBJECT_PARAMETRIC_ERROR);
  #else
    gluNurbsProperty(theNurb, GLU_SAMPLING_METHOD,   GLU_PARAMETRIC_ERROR);
  #endif

  gluNurbsProperty(theNurb, GLU_PARAMETRIC_TOLERANCE, 0.2);

  // get min/max knots of domain defining patch (partition of unity)
  float  minu( knots[bsplineCurve_.degree()]);
  float  maxu( knots[numKnots  - order]);
  
  float  minv = 0.0;
  float  maxv = 1.0;
  
  // control points of 2d texture ((0,0), (0,1), (1,0), (1,1) )
  GLfloat   tcoords[8] = {0.0, 0.0, 0.0, 1.0, 1.0, 0.0, 1.0, 1.0};

  // knots of domain, over which tcoords shall be linearly interpolated
  GLfloat   tknots[4] = {minu, minu, maxu, maxu};
  GLfloat   sknots[4] = {minv, minv, maxv, maxv};
  
  // begin drawing nurbs
  gluBeginSurface(theNurb);
  
  // first enable texture coordinate mapping
  gluNurbsSurface(theNurb, 4, tknots, 4, sknots, 2*2, 2, &tcoords[0], 2, 2, GL_MAP2_TEXTURE_COORD_2); 

  // draw surface (dummy direction
  int order_dummy = 1+1; // linear dummy
  int numKnots_dummy = 4;
  GLfloat *knots_dummy = new GLfloat[numKnots_dummy]; 
  knots_dummy[0] = 0.0; 
  knots_dummy[1] = 0.0;
  knots_dummy[2] = 1.0;
  knots_dummy[3] = 1.0;
  
  gluNurbsSurface(theNurb, numKnots, knots, numKnots_dummy, knots_dummy, numCPs_dummy * 3, 3, ctlpoints, order, order_dummy, GL_MAP2_VERTEX_3);
  gluEndSurface(theNurb);

  gluDeleteNurbsRenderer(theNurb);

  delete[] knots;
  delete[] knots_dummy;
  delete[] ctlpoints;
}

//----------------------------------------------------------------------------

/*
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
  ACG::GLState::bindTexture( GL_TEXTURE_1D, pick_texture_idx_ );
  // do not blend colors (else color picking breaks!)
  glTexParameterf( GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
  glTexParameterf( GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
  // avoid aliasing at patch boundaries
  glTexParameterf( GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
  // GL_REPLACE to avoid smearing colors (else color picking breaks!)
  glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  // unbind current texture
  ACG::GLState::bindTexture( GL_TEXTURE_1D, 0);
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
  ACG::GLState::bindTexture( GL_TEXTURE_1D, pick_texture_idx_ );
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
  float  minu( knots[bsplineCurve_.degree()]);
  float  maxu( knots[numKnots - order]);
  std::cout << "minu = " << minu << ", maxu = " << maxu << std::endl;

  // control points of 1d texture (0, 1)
  GLfloat   tcoords[2] = {0.0, 1.0};

  // knots of domain, over which tcoords shall be linearly interpolated
//   GLfloat   tknots[2] = {minu, maxu};
  GLfloat   tknots[4] = {minu, minu, maxu, maxu};
//   GLfloat   tknots[4] = {minu/(maxu - minu), minu/(maxu - minu), maxu/(maxu - minu), maxu/(maxu - minu)};

  // begin drawing nurbs
  gluBeginCurve(theNurb);

  // first enable texture coordinate mapping
  gluNurbsCurve(theNurb, 4, tknots, 1, tcoords, 2, GL_MAP1_TEXTURE_COORD_1);
//   gluNurbsCurve(theNurb, 4, tknots, 1, &tcoords[0], 2, GL_MAP1_TEXTURE_COORD_1);
//   gluNurbsCurve(theNurb, numKnots, knots, 3, ctlpoints, order, GL_MAP1_TEXTURE_COORD_1);

  // draw surface
  gluNurbsCurve(theNurb, numKnots, knots, 3, ctlpoints, order, GL_MAP1_VERTEX_3);
  gluEndCurve(theNurb);

  gluDeleteNurbsRenderer(theNurb);

  delete[] knots;
  delete[] ctlpoints;
}
*/

//----------------------------------------------------------------------------

/*
template <class BSplineCurve>
void
BSplineCurveNodeT<BSplineCurve>::
drawDirectMode(DrawModes::DrawMode _drawMode, GLState& _state)
{
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
  }
}
*/

//----------------------------------------------------------------------------

//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
