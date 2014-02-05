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
//  CLASS BSplineSurfaceNodeT - IMPLEMENTATION
//  Author:  Ellen Dekkers <dekkers@cs.rwth-aachen.de>
//
//=============================================================================

#define ACG_BSPLINESURFACENODET_C

//== INCLUDES =================================================================

#include "BSplineSurfaceNodeT.hh"
#include <ACG/GL/gl.hh>
#include <ACG/GL/GLError.hh>
#include <ACG/Utils/VSToolsT.hh>
#include <vector>


//== NAMESPACES ===============================================================

namespace ACG {
namespace SceneGraph {

//== IMPLEMENTATION ==========================================================


template <class BSplineSurfaceType>
void
BSplineSurfaceNodeT<BSplineSurfaceType>::
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
}

//----------------------------------------------------------------------------

template <class BSplineSurfaceType>
DrawModes::DrawMode
BSplineSurfaceNodeT<BSplineSurfaceType>::
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

template <class BSplineSurfaceType>
void
BSplineSurfaceNodeT<BSplineSurfaceType>::
draw(GLState& _state, const DrawModes::DrawMode& _drawMode)
{
  GLenum prev_depth = _state.depthFunc();

  glPushAttrib(GL_ENABLE_BIT);

  // check if textures are still valid
  if (    bspline_selection_draw_mode_ == CONTROLPOINT
       && controlPointSelectionTexture_valid_ == false)
    updateControlPointSelectionTexture(_state);
  if (    bspline_selection_draw_mode_ == KNOTVECTOR
       && knotVectorSelectionTexture_valid_ == false)
    updateKnotVectorSelectionTexture(_state);
  
  
  if (_drawMode & DrawModes::POINTS)
  {
    ACG::GLState::disable(GL_LIGHTING);
    ACG::GLState::shadeModel(GL_FLAT);
    render( _state, false);
  }

  if (_drawMode & DrawModes::WIREFRAME)
  {
    glPushAttrib(GL_ENABLE_BIT);

    ACG::GLState::disable( GL_CULL_FACE );
    ACG::GLState::disable(GL_LIGHTING);
    ACG::GLState::shadeModel(GL_FLAT);
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

    ACG::GLState::enable(GL_DEPTH_TEST);
    ACG::GLState::disable(GL_LIGHTING);
    ACG::GLState::shadeModel(GL_FLAT);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    _state.set_base_color(clear_color);

    ACG::GLState::depthRange(0.01, 1.0);

    render( _state, true);

    ACG::GLState::depthRange(0.0, 1.0);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    ACG::GLState::depthFunc(GL_LEQUAL);
    _state.set_base_color(base_color);

    render( _state, false);

    ACG::GLState::depthFunc(prev_depth);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }


  if ( ( _drawMode & DrawModes::SOLID_FLAT_SHADED ))
  {
    ACG::GLState::enable(GL_LIGHTING);
    ACG::GLState::shadeModel(GL_FLAT);

    ACG::GLState::enable(GL_AUTO_NORMAL);
    ACG::GLState::enable(GL_NORMALIZE);

    ACG::GLState::depthRange(0.01, 1.0);

    render( _state, true);

    ACG::GLState::depthRange(0.0, 1.0);
  }


  if ( ( _drawMode & DrawModes::SOLID_SMOOTH_SHADED ) )
  {
    ACG::GLState::enable(GL_AUTO_NORMAL);
    ACG::GLState::enable(GL_NORMALIZE);

    ACG::GLState::enable(GL_LIGHTING);
    ACG::GLState::shadeModel(GL_SMOOTH);
    ACG::GLState::depthRange(0.01, 1.0);

    render( _state, true);

    ACG::GLState::depthRange(0.0, 1.0);
  }

  if ( ( _drawMode & DrawModes::SOLID_PHONG_SHADED )  )
  {
    ACG::GLState::enable(GL_AUTO_NORMAL);
    ACG::GLState::enable(GL_NORMALIZE);

    ACG::GLState::enable(GL_LIGHTING);
    ACG::GLState::shadeModel(GL_SMOOTH);
    ACG::GLState::depthRange(0.01, 1.0);

    render( _state, true);

    ACG::GLState::depthRange(0.0, 1.0);
  }


    // If in shader mode, just draw, as the shader has to be set by a shadernode above this node
  if ( (_drawMode & DrawModes::SOLID_SHADER )  ) {
    ACG::GLState::enable(GL_AUTO_NORMAL);
    ACG::GLState::enable(GL_NORMALIZE);

    ACG::GLState::enable(GL_LIGHTING);
    ACG::GLState::shadeModel(GL_SMOOTH);
    ACG::GLState::depthRange(0.01, 1.0);

//     draw_faces(PER_VERTEX);
    render( _state, true);

    ACG::GLState::depthRange(0.0, 1.0);
  }


  if ( (_drawMode & DrawModes::SOLID_TEXTURED )  ) {
    ACG::GLState::enable(GL_AUTO_NORMAL);
    ACG::GLState::enable(GL_NORMALIZE);
    ACG::GLState::enable (GL_BLEND); 
    ACG::GLState::blendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    ACG::GLState::enable(GL_LIGHTING);
    ACG::GLState::shadeModel(GL_SMOOTH);
    ACG::GLState::depthRange(0.01, 1.0);

    arb_texture_used_ = true;
    drawTexturedSurface(_state, arb_texture_idx_ );
    arb_texture_used_ = false;

    ACG::GLState::depthRange(0.0, 1.0);
    ACG::GLState::disable(GL_BLEND);
  }

  glPopAttrib();
}

//----------------------------------------------------------------------------

template <class BSplineSurfaceType>
void
BSplineSurfaceNodeT<BSplineSurfaceType>::
render(GLState& _state, bool _fill)
{
  // draw the control net (includes selection on the net)
  if (render_control_net_)
  {
    if (bspline_draw_mode_ == NORMAL)
      drawControlNet(_state);
    else if (bspline_draw_mode_ == FANCY)
      drawFancyControlNet(_state);
  }
  
  // draw the spline curve itself, depending on the type of visualization
  if (render_bspline_surface_)
  {
    if (bspline_selection_draw_mode_ == NONE)
      drawSurface(_state, _fill);
    else if (bspline_selection_draw_mode_ == CONTROLPOINT)
      drawTexturedSurface(_state, cp_selection_texture_idx_);
    else if (bspline_selection_draw_mode_ == KNOTVECTOR)
      drawTexturedSurface(_state, knot_selection_texture_idx_);
  }
}

//----------------------------------------------------------------------------

template <class BSplineSurfaceType>
void
BSplineSurfaceNodeT<BSplineSurfaceType>::
drawSurface(GLState& _state, bool _fill)
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
    gluNurbsProperty(theNurb, GLU_PARAMETRIC_TOLERANCE, 0.2f);
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

template <class BSplineSurfaceType>
void
BSplineSurfaceNodeT<BSplineSurfaceType>::
drawTexturedSurface(GLState& _state, GLuint _texture_idx)
{
  glPushAttrib(GL_ALL_ATTRIB_BITS);
  ACG::GLState::enable( GL_COLOR_MATERIAL );
  glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    
  ACG::GLState::enable(GL_TEXTURE_2D);
  
  // blend colors (otherwise lighting does not affect the texture)
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  // avoid aliasing at patch boundaries
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

  // repeat if arbitrary texture mode
  if( arb_texture_used_ )
  {
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  }

  // GL_MODULATE to include lighting effects
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  
  ACG::GLState::bindTexture( GL_TEXTURE_2D, _texture_idx);

  draw_textured_nurbs( _state);

  ACG::GLState::bindTexture( GL_TEXTURE_2D, 0);
  ACG::GLState::disable(GL_TEXTURE_2D);
  ACG::GLState::disable( GL_COLOR_MATERIAL );
  glPopAttrib( );
}

//----------------------------------------------------------------------------

template <class BSplineSurfaceType>
void
BSplineSurfaceNodeT<BSplineSurfaceType>::
drawControlNet(GLState& _state)
{
  // remember old color
  Vec4f base_color_old = _state.base_color();
  
  // draw control net
//   glPushAttrib(GL_ENABLE_BIT);
  glPushAttrib(GL_ALL_ATTRIB_BITS);

  ACG::GLState::disable( GL_CULL_FACE );
  ACG::GLState::disable(GL_LIGHTING);
  ACG::GLState::shadeModel(GL_FLAT);

  // draw points
  
  // draw selection
  if( bsplineSurface_.controlpoint_selections_available())
  {
    // save old values
    float point_size_old = _state.point_size();

    glColor(generateHighlightColor(controlnet_color_));
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
  }

  // draw all points
  glColor(controlnet_color_);

  float point_size_old = _state.point_size();
  glPointSize(point_size_old + 4);
  
  glBegin(GL_POINTS);
  for (unsigned int i = 0; i < bsplineSurface_.n_control_points_m(); ++i)
    for (unsigned int j = 0; j < bsplineSurface_.n_control_points_n(); ++j)
      glVertex(bsplineSurface_(i, j));
  glEnd();

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

template <class BSplineSurfaceType>
void
BSplineSurfaceNodeT<BSplineSurfaceType>::
drawFancyControlNet(GLState& _state)
{
  // remember old color
  Vec4f base_color_old = _state.base_color();
  
  // draw control net
//   glPushAttrib(GL_ENABLE_BIT);
  glPushAttrib(GL_ALL_ATTRIB_BITS);

  ACG::GLState::disable( GL_CULL_FACE );
  
  glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
  ACG::GLState::enable( GL_COLOR_MATERIAL );
  ACG::GLState::enable(GL_LIGHTING);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  ACG::GLState::shadeModel(GL_SMOOTH);


  // draw points
  double sphereRadius = _state.point_size() * 0.05;
  
  // draw selection
  if( bsplineSurface_.controlpoint_selections_available())
  {
    // save old values
    float point_size_old = _state.point_size();

    // save old values
//     glColor(controlnet_highlight_color_);
    glColor(generateHighlightColor(controlnet_color_));

    // draw control polygon
    for (unsigned int i = 0; i < bsplineSurface_.n_control_points_m(); ++i)
    {
      for (unsigned int j = 0; j < bsplineSurface_.n_control_points_n(); ++j)
      {
        if( bsplineSurface_.controlpoint_selection(i, j))
          draw_sphere(bsplineSurface_(i, j), sphereRadius, _state, fancySphere_);
      }
    }
    
    glPointSize(point_size_old);
  }

  // draw all points
  glColor(controlnet_color_);

  for (unsigned int i = 0; i < bsplineSurface_.n_control_points_m(); ++i)
    for (unsigned int j = 0; j < bsplineSurface_.n_control_points_n(); ++j)
      draw_sphere(bsplineSurface_(i, j), sphereRadius, _state, fancySphere_);


  // draw line segments

  double cylinderRadius = _state.line_width() * 0.05;

  glColor(controlnet_color_);

  // draw bspline control net
  for (unsigned int i = 0; i < bsplineSurface_.n_control_points_m(); ++i)
  {
    for (int j = 0; j < (int)bsplineSurface_.n_control_points_n() - 1; ++j)
    {
      Vec3d p      = bsplineSurface_(i, j);
      Vec3d p_next = bsplineSurface_(i, j+1);
      draw_cylinder(p, p_next - p, cylinderRadius, _state);
    }
  }

  for (int j = 0; j < (int)bsplineSurface_.n_control_points_n(); ++j)
  {
    for (int i = 0; i < (int)bsplineSurface_.n_control_points_m() - 1; ++i)
    {
      Vec3d p = bsplineSurface_(i, j);
      Vec3d p_next = bsplineSurface_(i+1,j);
      draw_cylinder(p, p_next - p, cylinderRadius, _state);
    }
  }

  // reset old color
  glColor( base_color_old );

  glPopAttrib();
}

//----------------------------------------------------------------------------

template <class BSplineSurfaceType>
void
BSplineSurfaceNodeT<BSplineSurfaceType>::
pick(GLState& _state, PickTarget _target)
{
  if(pick_texture_idx_ == 0)
      pick_init_texturing();
    
  ACG::GLState::disable(GL_COLOR_MATERIAL);
  
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

template <class BSplineSurfaceType>
void
BSplineSurfaceNodeT<BSplineSurfaceType>::
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
      draw_sphere( bsplineSurface_(i,j), r, _state, sphere_);
    }
  }
}

//----------------------------------------------------------------------------

template <class BSplineSurfaceType>
void
BSplineSurfaceNodeT<BSplineSurfaceType>::
pick_spline( GLState& _state )
{
  glPushAttrib(GL_ALL_ATTRIB_BITS);

  ACG::GLState::enable(GL_TEXTURE_2D);
  
  ACG::GLState::disable(GL_COLOR_MATERIAL);
  ACG::GLState::disable(GL_LIGHTING);
  ACG::GLState::shadeModel(GL_FLAT);

  std::cout << "[BSplineSurface] pick_spline: \n"
            << "pick_texture_baseidx_ = " << pick_texture_baseidx_
            << ", _state.pick_current_index () = " << _state.pick_current_index ()
            << ", pick_texture_idx_ = " << pick_texture_idx_
            << std::endl;
            
  if( _state.pick_current_index () != pick_texture_baseidx_)
  {
    pick_texture_baseidx_ = _state.pick_current_index();
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
  }

  draw_textured_nurbs( _state);

  ACG::GLState::bindTexture( GL_TEXTURE_2D, 0);
  ACG::GLState::disable(GL_TEXTURE_2D);
  glPopAttrib( );
}

//----------------------------------------------------------------------------

template <class BSplineSurfaceType>
void
BSplineSurfaceNodeT<BSplineSurfaceType>::
pick_surface( GLState& _state, unsigned int _offset )
{
  bool sampling_mode_backup = adaptive_sampling_;
  adaptive_sampling_ = false;

  // pick the whole surface
  _state.pick_set_name ( _offset );
  drawSurface( _state, true);

  adaptive_sampling_ = sampling_mode_backup;
}

//----------------------------------------------------------------------------

template <class BSplineSurfaceType>
void
BSplineSurfaceNodeT<BSplineSurfaceType>::
draw_sphere( const Point& _p0, double _r, GLState& _state, GLSphere* _sphere)
{
  // draw 3d sphere
  _state.push_modelview_matrix();
  _state.translate( _p0[0], _p0[1], _p0[2]);

  _sphere->draw(_state,_r);

  _state.pop_modelview_matrix();
}

//----------------------------------------------------------------------------

template <class BSplineSurfaceType>
void
BSplineSurfaceNodeT<BSplineSurfaceType>::
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

template <class BSplineSurfaceType>
void
BSplineSurfaceNodeT<BSplineSurfaceType>::
updateControlPointSelectionTexture(GLState& _state)
{
  create_cp_selection_texture(_state);
  controlPointSelectionTexture_valid_ = true;
}

//----------------------------------------------------------------------------

template <class BSplineSurfaceType>
void
BSplineSurfaceNodeT<BSplineSurfaceType>::
updateKnotVectorSelectionTexture(GLState& _state)
{
  create_knot_selection_texture(_state);
  knotVectorSelectionTexture_valid_ = true;
}

//----------------------------------------------------------------------------

template <class BSplineSurfaceType>
void
BSplineSurfaceNodeT<BSplineSurfaceType>::
selection_init_texturing(GLuint & _texture_idx )
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

template <class BSplineSurfaceType>
void
BSplineSurfaceNodeT<BSplineSurfaceType>::
create_cp_selection_texture(GLState& /*_state*/)
{
  if (bsplineSurface_.n_knots_m() == 0 || bsplineSurface_.n_knots_n() == 0)
    return;
  
  if(cp_selection_texture_idx_ == 0)
      selection_init_texturing(cp_selection_texture_idx_);
    
  QImage b(cp_selection_texture_res_, cp_selection_texture_res_, QImage::Format_ARGB32);
  
  int degree_m   = bsplineSurface_.degree_m();
  int degree_n   = bsplineSurface_.degree_n();
  
  int numKnots_m = bsplineSurface_.n_knots_m();
  int numKnots_n = bsplineSurface_.n_knots_n();
  
  Knotvector knotvec_m = bsplineSurface_.get_knotvector_m();
  Knotvector knotvec_n = bsplineSurface_.get_knotvector_n();
  
  double minu  = bsplineSurface_.get_knot_m( degree_m );
  double maxu  = bsplineSurface_.get_knot_m( numKnots_m - degree_m -1 );
  double diffu = maxu - minu;
  
  double minv  = bsplineSurface_.get_knot_n( degree_n );
  double maxv  = bsplineSurface_.get_knot_n( numKnots_n - degree_n -1 );
  double diffv = maxv - minv;
  
  if (diffu == 0.0 || diffv == 0.0 )
    return;
  
  int texelIdx_u = 0;
  
  for ( int m = 0; m < cp_selection_texture_res_; ++m)
  {
    double step_m = (double)m / (double)cp_selection_texture_res_;
    double u = step_m * diffu;

    // get the span and check which knots are selected
    ACG::Vec2i span_u = bsplineSurface_.spanm(u);
    // check for incomplete spline
    if (span_u[0] < 0 || span_u[1] < 0)
      return; 

    // reset texture v coord for every new u coord
    int texelIdx_v = 0;

    // iterate over n direction
    for ( int n = 0; n < cp_selection_texture_res_; ++n)
    {
      double step_n = double(n) / (double)cp_selection_texture_res_;
      double v = step_n * diffv;

      // get the span and check which knots are selected
      ACG::Vec2i span_v = bsplineSurface_.spann(v);
      // check for incomplete spline
      if (span_v[0] < 0 || span_v[1] < 0)
        return; 

      float alpha = 0.0; // blends between curve and highlight colors
      for (int i = 0; i < degree_m+1; ++i) // degree+1 basis functions (those in the span) contribute
      {
        int idx_m = span_u[0] + i;

        for (int j = 0; j < degree_n+1; ++j) // degree+1 basis functions (those in the span) contribute
        {
          int idx_n = span_v[0] + j;

          // basis functions sum up to 1. hence, we only have to sum up those with selected control point to get the blending weight
          if (bsplineSurface_.controlpoint_selection(idx_m, idx_n))
            alpha +=   bsplineSurface_.basisFunction( knotvec_m, idx_m, degree_m, u) 
            * bsplineSurface_.basisFunction( knotvec_n, idx_n, degree_n, v);
        }
      }

      // compute color
      Vec4f color = surface_color_ * (1.0 - alpha) + surface_highlight_color_ * alpha;

      // fill texture (switch v coord due to axis of texture image)
      b.setPixel (texelIdx_u, 255-texelIdx_v, qRgba((int)(color[0]*255.0), (int)(color[1]*255.0), (int)(color[2]*255.0), 255));

      ++texelIdx_v;
    } // end of n direction iter

    ++texelIdx_u;
  } // end of u direction iter


  // debug, output image
//   b.save("surfaceCPSelectionTexture.png", "PNG");
  
  cp_selection_texture_image_ = QGLWidget::convertToGLFormat( b );

  // bind texture 
  ACG::GLState::bindTexture( GL_TEXTURE_2D, cp_selection_texture_idx_ );
  glTexImage2D(  GL_TEXTURE_2D,
                 0, GL_RGBA, cp_selection_texture_image_.width(), cp_selection_texture_image_.height(), 
                 0, GL_RGBA, GL_UNSIGNED_BYTE, cp_selection_texture_image_.bits() );
}

//----------------------------------------------------------------------------

template <class BSplineSurfaceType>
void
BSplineSurfaceNodeT<BSplineSurfaceType>::
create_knot_selection_texture(GLState& _state)
{
  if (bsplineSurface_.n_knots_m() == 0 ||bsplineSurface_.n_knots_n() == 0)
    return;
  
  if(knot_selection_texture_idx_ == 0)
      selection_init_texturing(knot_selection_texture_idx_);
  
  QImage b(knot_selection_texture_res_, knot_selection_texture_res_, QImage::Format_ARGB32);

  int degree_m   = bsplineSurface_.degree_m();
  int degree_n   = bsplineSurface_.degree_n();
  
  int numKnots_m = bsplineSurface_.n_knots_m();
  int numKnots_n = bsplineSurface_.n_knots_n();
  
  Knotvector knotvec_m = bsplineSurface_.get_knotvector_m();
  Knotvector knotvec_n = bsplineSurface_.get_knotvector_n();
  
  double minu  = bsplineSurface_.get_knot_m( degree_m );
  double maxu  = bsplineSurface_.get_knot_m( numKnots_m - degree_m -1 );
  double diffu = maxu - minu;
  
  double minv  = bsplineSurface_.get_knot_n( degree_n );
  double maxv  = bsplineSurface_.get_knot_n( numKnots_n - degree_n -1 );
  double diffv = maxv - minv;
  
  if (diffu == 0.0 || diffv == 0.0 )
    return;
  
  int texelIdx_u = 0;
  
  // if a knot is selected, select all knots in the span of this knot, too
  std::vector<bool> selectedKnotSpans_m(numKnots_m, false);  
  for (int i = 0; i < numKnots_m; ++i)
  {
    if (bsplineSurface_.get_knotvector_m_ref()->selection(i))
    {
      // get the span and check which knots are selected
      ACG::Vec2i span = bsplineSurface_.spanm(bsplineSurface_.get_knot_m(i));
      // check for incomple spline 
      if (span[0] < 0 || span[1] < 0) 
        return; 
    
      for(int j = span[0]; j <= span[1]+degree_m; ++j)
        selectedKnotSpans_m[j] = true;
    }
  }
//   std::cout << "selectedKnotSpans_m: " << std::flush;
//   for (unsigned int i = 0; i < selectedKnotSpans_m.size(); ++i)
//     std::cout << selectedKnotSpans_m[i] << ", " << std::flush;
//   std::cout << std::endl;


  std::vector<bool> selectedKnotSpans_n(numKnots_n, false);  
  for (int i = 0; i < numKnots_n; ++i)
  {
    if (bsplineSurface_.get_knotvector_n_ref()->selection(i))
    {
      // get the span and check which knots are selected
      ACG::Vec2i span = bsplineSurface_.spann(bsplineSurface_.get_knot_n(i));
      // check for incomple spline 
      if (span[0] < 0 || span[1] < 0) 
        return; 
    
      for(int j = span[0]; j <= span[1]+degree_n; ++j)
        selectedKnotSpans_n[j] = true;
    }
  }
//   std::cout << "selectedKnotSpans_n: " << std::flush;
//   for (unsigned int i = 0; i < selectedKnotSpans_n.size(); ++i)
//     std::cout << selectedKnotSpans_n[i] << ", " << std::flush;
//   std::cout << std::endl;


  for ( int m = 0; m < knot_selection_texture_res_; ++m)
  {
    double step_m = (double)m / (double)knot_selection_texture_res_;
    double u = step_m * diffu;

    Vec2i interval_m = bsplineSurface_.interval_m(u);
    
    // reset texture v coord for every new u coord
    int texelIdx_v = 0;
    
    for ( int n = 0; n < knot_selection_texture_res_; ++n)
    {
      double step_n = (double)n / (double)knot_selection_texture_res_;
      double v = step_n * diffv;
    
      Vec2i interval_n = bsplineSurface_.interval_n(v);
      
      // check if highlighted
      bool selected_m = (selectedKnotSpans_m[interval_m[0]] && selectedKnotSpans_m[interval_m[1]]);
      bool selected_n = (selectedKnotSpans_n[interval_n[0]] && selectedKnotSpans_n[interval_n[1]]); 
      
      Vec4f color;
      if (selected_m && selected_n)
//         color = _state.specular_color();
//         color = _state.base_color();
        color = surface_highlight_color_;
      else if ((selected_m && !selected_n) || (!selected_m && selected_n) )
//         color = _state.ambient_color() *0.5 + _state.specular_color() * 0.5;
//         color = _state.base_color() *0.5 + _state.specular_color() * 0.5;        
        color = surface_highlight_color_ * 0.5 + surface_color_ * 0.5;
      else
//         color = _state.ambient_color() *0.5 + _state.diffuse_color() * 0.5;
//         color = _state.base_color() *0.5 + _state.diffuse_color() * 0.5;
        color = surface_color_;


      // fill texture
      b.setPixel (texelIdx_u, 255-texelIdx_v, qRgba((int)(color[0]*255.0), (int)(color[1]*255.0), (int)(color[2]*255.0), 255));
    
      ++texelIdx_v;
    } // end of v direction
    
    ++texelIdx_u;
  } // end of u direction
  
  // debug, output image
//   b.save("surfaceKnotSelectionTexture.png", "PNG");
  
  knot_selection_texture_image_ = QGLWidget::convertToGLFormat( b );

  // bind texture 
  ACG::GLState::bindTexture( GL_TEXTURE_2D, knot_selection_texture_idx_ );
  glTexImage2D(  GL_TEXTURE_2D,
                 0, GL_RGBA, knot_selection_texture_image_.width(), knot_selection_texture_image_.height(), 
                 0, GL_RGBA, GL_UNSIGNED_BYTE, knot_selection_texture_image_.bits() );
}

//----------------------------------------------------------------------------

template <class BSplineSurfaceType>
void
BSplineSurfaceNodeT<BSplineSurfaceType>::
pick_init_texturing( )
{
  std::cout << "[BSplineSurface] pick_init_texturing()" << std::endl;
  
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

template <class BSplineSurfaceType>
void
BSplineSurfaceNodeT<BSplineSurfaceType>::
pick_create_texture( GLState& _state)
{
  std::cout << "[BSplineSurface] pick_create_texture()" << std::endl;
  
  QImage b(pick_texture_res_, pick_texture_res_, QImage::Format_ARGB32);
  QImage texture(pick_texture_res_, pick_texture_res_, QImage::Format_ARGB32);
    
  // fill with colors
  int cur_idx=0;
  for( int i = 0; i < pick_texture_res_; ++i)
  {
    for( int j = pick_texture_res_ - 1; j >= 0; j--)
    {
      Vec4uc cur_col( _state.pick_get_name_color (cur_idx) );
      b.setPixel ( i,j, qRgba((int)cur_col[0], (int)cur_col[1], (int)cur_col[2], (int)cur_col[3]));
      
      Vec4f testcol = Vec4f((float)cur_col[0], (float)cur_col[1], (float)cur_col[2], (float)cur_col[3]);
      texture.setPixel ( i,j, qRgba((int)(testcol[0]*255.0), (int)(testcol[1]*255.0), (int)(testcol[2]*255.0), 255));
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

  // debug, output image
//   b.save("surfacePickingTexture.png", "PNG");
  texture.save("surfacePickingTexture.png", "PNG");
  
  pick_texture_image_ = QGLWidget::convertToGLFormat( b );

  // bind texture 
  ACG::GLState::bindTexture( GL_TEXTURE_2D, pick_texture_idx_ );
  glTexImage2D(  GL_TEXTURE_2D,
                 0, GL_RGBA, pick_texture_image_.width(), pick_texture_image_.height(), 
                 0, GL_RGBA, GL_UNSIGNED_BYTE, pick_texture_image_.bits() );
}

//----------------------------------------------------------------------------

template <class BSplineSurfaceType>
void
BSplineSurfaceNodeT<BSplineSurfaceType>::
set_arb_texture( const QImage& _texture, bool _repeat, float _u_repeat, float _v_repeat )
{
  if(arb_texture_idx_ == 0)
      selection_init_texturing(arb_texture_idx_);
    
  ACG::GLState::bindTexture( GL_TEXTURE_2D, 0);

  arb_texture_repeat_   = _repeat;
  arb_texture_repeat_u_ = _u_repeat;
  arb_texture_repeat_v_ = _v_repeat;

  arb_texture_image_ = QGLWidget::convertToGLFormat( _texture );
  int u_res          = arb_texture_image_.width();
  int v_res          = arb_texture_image_.height();

  // bind texture as current
  ACG::GLState::bindTexture( GL_TEXTURE_2D, arb_texture_idx_ );

  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glTexImage2D(  GL_TEXTURE_2D,
                 0, GL_RGBA, u_res, v_res,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, arb_texture_image_.bits() );

  // unbind current texture
  ACG::GLState::bindTexture( GL_TEXTURE_2D, 0);
}

//----------------------------------------------------------------------------

template <class BSplineSurfaceType>
void
BSplineSurfaceNodeT<BSplineSurfaceType>::
draw_textured_nurbs( GLState& /*_state*/)
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

  gluNurbsProperty(theNurb, GLU_PARAMETRIC_TOLERANCE, 0.2f);

  // get min/max knots of domain defining patch (partition of unity)
  float  minu( knots_m[bsplineSurface_.degree_m()]);
  float  minv( knots_n[bsplineSurface_.degree_n()]);
  float  maxu( knots_m[numKnots_m  - order_m]);
  float  maxv( knots_n[numKnots_n  - order_n]);

  // control points of 2d texture ((0,0), (0,1), (1,0), (1,1) )
  GLfloat   tcoords[8] = {0.0, 0.0, 0.0, 1.0, 1.0, 0.0, 1.0, 1.0};
/*
  if( arb_texture_repeat_ && arb_texture_used_ ) {
    tcoords[0] = 0.0;
    tcoords[1] = 0.0;
    tcoords[2] = 0.0;
    tcoords[3] = arb_texture_repeat_v_*(maxv-minv);
    tcoords[4] = arb_texture_repeat_u_*(maxu-minu);
    tcoords[5] = 0.0;
    tcoords[6] = arb_texture_repeat_u_*(maxu-minu);
    tcoords[7] = arb_texture_repeat_v_*(maxv-minv);
  }
*/
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

//----------------------------------------------------------------------------

template <class BSplineSurfaceType>
ACG::Vec4f
BSplineSurfaceNodeT<BSplineSurfaceType>::
generateHighlightColor(ACG::Vec4f _color)
{
  float c1 = _color[0]*1.5; 
  c1 = c1 > 255.0 ? 255 : c1;
  
  float c2 = _color[1]*1.5; 
  c2 = c2 > 255.0 ? 255 : c2;
  
  float c3 = _color[2]*1.5; 
  c3 = c3 > 255.0 ? 255 : c3;
  
  return Vec4f( c1, c2, c3, _color[3]);
}

//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
