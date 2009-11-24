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
//  CLASS GLState - IMPLEMENTATION
//
//=============================================================================

//== INCLUDES =================================================================

#include "GLState.hh"


//== NAMESPACES ===============================================================


namespace ACG {


//== IMPLEMENTATION ==========================================================


const Vec4f    GLState::default_clear_color(0.0, 0.0, 0.0, 1.0);
const Vec4f    GLState::default_base_color(1.0, 1.0, 1.0, 1.0);
const Vec4f    GLState::default_ambient_color(0.2, 0.2, 0.2, 1.0);
const Vec4f    GLState::default_diffuse_color(0.50, 0.53, 0.6, 1.0);
const Vec4f    GLState::default_specular_color(0.75, 0.8, 0.85, 1.0);
const float  GLState::default_shininess(100.0);


//-----------------------------------------------------------------------------


GLState::GLState(bool _updateGL)
  : render_pass_(1),
    max_render_passes_(1),
    multisampling_(false),
    allow_multisampling_(true),
    updateGL_(_updateGL),
    blending_(false),
    msSinceLastRedraw_ (1)
{
  initialize();
}


//-----------------------------------------------------------------------------


void GLState::initialize()
{
  // clear matrix stacks
  while (!stack_projection_.empty())
    stack_projection_.pop();
  while (!stack_modelview_.empty())
    stack_modelview_.pop();
  while (!stack_inverse_projection_.empty())
    stack_inverse_projection_.pop();
  while (!stack_inverse_modelview_.empty())
    stack_inverse_modelview_.pop();


  // load identity matrix
  reset_projection();
  reset_modelview();


  // colors
  set_clear_color(default_clear_color);
  set_base_color(default_base_color);
  set_ambient_color(default_ambient_color);
  set_diffuse_color(default_diffuse_color);
  set_specular_color(default_specular_color);
  set_shininess(default_shininess);


  // thickness
  set_point_size(1.0f);
  set_line_width(1.0f);

  // multisampling
  set_multisampling(true);

  // lighting
  set_twosided_lighting(true);
}

//-----------------------------------------------------------------------------

void GLState::setState ()
{
  makeCurrent();

  // projection matrix
  glMatrixMode(GL_PROJECTION);
  glLoadMatrixd(projection_.get_raw_data());
  glMatrixMode(GL_MODELVIEW);

  // modelview matrix
  glLoadMatrixd(modelview_.get_raw_data());

  // clear color
  glClearColor(clear_color_[0], clear_color_[1], clear_color_[2], clear_color_[3]);

  // base color
  glColor4fv(base_color_.data());

  // ambient color
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient_color_.data());

  // diffuaw color
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse_color_.data());

  // specular color
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular_color_.data());

  // shininess
  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess_);

  // point size
  glPointSize(point_size_);

  // line width
  glLineWidth(line_width_);

  // two sided lighting
  if (twosided_lighting_)
    glLightModeli( GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE );
  else
    glLightModeli( GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE );

  // viewport
  glViewport(left_, bottom_, width_, height_);
}


//-----------------------------------------------------------------------------

void GLState::clearBuffers ()
{
    glPushAttrib (GL_ALL_ATTRIB_BITS);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_DITHER);
    glShadeModel( GL_FLAT );

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity ();

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity ();

    glColor4fv(clear_color_.data());
    glBegin (GL_QUADS);
    glVertex3f (-1.0, -1.0, 1.0);
    glVertex3f (1.0, -1.0, 1.0);
    glVertex3f (1.0, 1.0, 1.0);
    glVertex3f (-1.0, 1.0, 1.0);
    glEnd ();

    glPopMatrix ();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopAttrib ();
}

//-----------------------------------------------------------------------------

void GLState::reset_projection()
{
  projection_.identity();
  inverse_projection_.identity();

  if (updateGL_)
  {
    makeCurrent();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
  }

  update_matrices();
}


//-----------------------------------------------------------------------------


void GLState::set_projection(const GLMatrixd& _m, const GLMatrixd& _inv_m)
{
  projection_ = _m;
  inverse_projection_ = _inv_m;

  if (updateGL_)
  {
    makeCurrent();
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixd(projection_.get_raw_data());
    glMatrixMode(GL_MODELVIEW);
  }

  update_matrices();
}


//-----------------------------------------------------------------------------


void GLState::reset_modelview()
{
  modelview_.identity();
  inverse_modelview_.identity();

  if (updateGL_)
  {
    makeCurrent();
    glLoadIdentity();
  }

  update_matrices();
}


//-----------------------------------------------------------------------------


void GLState::set_modelview(const GLMatrixd& _m, const GLMatrixd& _inv_m)
{
  modelview_ = _m;
  inverse_modelview_ = _inv_m;

  if (updateGL_)
  {
    makeCurrent();
    glLoadMatrixd(modelview_.get_raw_data());
  }

  update_matrices();
}


//-----------------------------------------------------------------------------


void GLState::ortho( double _left, double _right,
		     double _bottom, double _top,
		     double _n, double _f )
{
  near_plane_ = _n;
  far_plane_  = _f;

  projection_.ortho(_left, _right, _bottom, _top, _n, _f);
  inverse_projection_.inverse_ortho(_left,_right,_bottom,_top,_n,_f);

  if (updateGL_)
  {
    makeCurrent();
    glMatrixMode(GL_PROJECTION);
    glOrtho(_left, _right, _bottom, _top, _n, _f);
    glMatrixMode(GL_MODELVIEW);
  }

  update_matrices();
}


//-----------------------------------------------------------------------------


void GLState::frustum( double _left, double _right,
		       double _bottom, double _top,
		       double _n, double _f )
{
  near_plane_ = _n;
  far_plane_  = _f;

  projection_.frustum(_left, _right, _bottom, _top, _n, _f);
  inverse_projection_.inverse_frustum(_left,_right,_bottom,_top,_n,_f);

  if (updateGL_)
  {
    makeCurrent();
    glMatrixMode(GL_PROJECTION);
    glFrustum(_left, _right, _bottom, _top, _n, _f);
    glMatrixMode(GL_MODELVIEW);
  }

  update_matrices();
}


//-----------------------------------------------------------------------------


void GLState::perspective( double _fovY, double _aspect,
			   double _n, double _f )
{
  near_plane_ = _n;
  far_plane_  = _f;

  projection_.perspective(_fovY, _aspect, _n, _f);
  inverse_projection_.inverse_perspective(_fovY, _aspect, _n, _f);

  if (updateGL_)
  {
    makeCurrent();
    glMatrixMode(GL_PROJECTION);
    gluPerspective(_fovY, _aspect, _n, _f);
    glMatrixMode(GL_MODELVIEW);
  }

  update_matrices();
}


//-----------------------------------------------------------------------------


void GLState::viewport( int _left, int _bottom,
			int _width, int _height,
			int _glwidth, int _glheight)
{
  left_   = _left;
  bottom_ = _bottom;
  width_  = _width;
  height_ = _height;

  if (_glwidth < _width || _glheight < _height)
  {
    glwidth_ = _width;
    glheight_ = _height;
  } else {
    glwidth_ = _glwidth;
    glheight_ = _glheight;
  }

  window2viewport_.identity();
  window2viewport_(0,0) = 0.5f * width_;
  window2viewport_(0,3) = 0.5f * width_ + left_;
  window2viewport_(1,1) = 0.5f * height_;
  window2viewport_(1,3) = 0.5f * height_ + bottom_;
  window2viewport_(2,2) = 0.5f;
  window2viewport_(2,3) = 0.5f;

  inverse_window2viewport_.identity();
  inverse_window2viewport_(0,0) =  2.0f / width_;
  inverse_window2viewport_(0,3) = -(2.0*left_ + width_) / width_;
  inverse_window2viewport_(1,1) =  2.0f / height_;
  inverse_window2viewport_(1,3) = -(2.0*bottom_ + height_) / height_;
  inverse_window2viewport_(2,2) =  2.0f;
  inverse_window2viewport_(2,3) = -1.0f;

  if (updateGL_)
  {
    makeCurrent();
    glViewport(_left, _bottom, _width, _height);
  }

  update_matrices();
}


//-----------------------------------------------------------------------------


void GLState::lookAt( const Vec3d& _eye,
		      const Vec3d& _center,
		      const Vec3d& _up )
{
  modelview_.lookAt(_eye, _center, _up);
  inverse_modelview_.inverse_lookAt(_eye, _center, _up);

  if (updateGL_)
  {
    makeCurrent();
    gluLookAt(_eye[0],
	      _eye[1],
	      _eye[2],
	      _center[0],
	      _center[1],
	      _center[2],
	      _up[0],
	      _up[1],
	      _up[2]);
  }

  update_matrices();
}


//-----------------------------------------------------------------------------


void GLState::translate( double _x, double _y, double _z,
			 MultiplyFrom _mult_from )
{
  if (_mult_from == MULT_FROM_RIGHT)
  {
    modelview_.translate(_x, _y, _z);
    inverse_modelview_.translate(-_x, -_y, -_z, MULT_FROM_LEFT);
  }
  else
  {
    modelview_.translate(_x, _y, _z, MULT_FROM_LEFT);
    inverse_modelview_.translate(-_x, -_y, -_z);
  }

  if (updateGL_)
  {
    makeCurrent();
    glLoadMatrixd(modelview_.get_raw_data());
  }

  update_matrices();
}


//-----------------------------------------------------------------------------


void GLState::rotate( double _angle, double _x, double _y, double _z,
		      MultiplyFrom _mult_from )
{
  if (_mult_from == MULT_FROM_RIGHT)
  {
    modelview_.rotate(_angle, _x, _y, _z);
    inverse_modelview_.rotate(-_angle, _x, _y, _z, MULT_FROM_LEFT);
  }
  else
  {
    modelview_.rotate(_angle, _x, _y, _z, MULT_FROM_LEFT);
    inverse_modelview_.rotate(-_angle, _x, _y, _z);
  }

  if (updateGL_)
  {
    makeCurrent();
    glLoadMatrixd(modelview_.get_raw_data());
  }

  update_matrices();
}


//-----------------------------------------------------------------------------


void GLState::scale( double _sx, double _sy, double _sz,
		     MultiplyFrom _mult_from )
{
  if (_mult_from == MULT_FROM_RIGHT)
  {
    modelview_.scale(_sx, _sy, _sz, MULT_FROM_RIGHT);
    inverse_modelview_.scale(1.0f/_sx, 1.0f/_sy, 1.0f/_sz, MULT_FROM_LEFT);
  }
  else
  {
    modelview_.scale(_sx, _sy, _sz, MULT_FROM_LEFT);
    inverse_modelview_.scale(1.0f/_sx, 1.0f/_sy, 1.0f/_sz, MULT_FROM_RIGHT);
  }

  if (updateGL_)
  {
    makeCurrent();
    glLoadMatrixd(modelview_.get_raw_data());
  }

  update_matrices();
}


//-----------------------------------------------------------------------------


void GLState::mult_matrix( const GLMatrixd& _m, const GLMatrixd& _inv_m,
			   MultiplyFrom _mult_from )
{
  if (_mult_from == MULT_FROM_RIGHT)
  {
    modelview_ *= _m;
    inverse_modelview_.leftMult(_inv_m);
  }
  else
  {
    modelview_.leftMult(_m);
    inverse_modelview_ *= _inv_m;
  }

  if (updateGL_)
  {
    makeCurrent();
    glLoadMatrixd(modelview_.get_raw_data());
  }

  update_matrices();
}


//-----------------------------------------------------------------------------


void GLState::update_matrices()
{
  forward_projection_ =  window2viewport_;
  forward_projection_ *= projection_;
  forward_projection_ *= modelview_;

  backward_projection_ = inverse_modelview_;
  backward_projection_ *= inverse_projection_;
  backward_projection_ *= inverse_window2viewport_;
}


//-----------------------------------------------------------------------------


Vec3d GLState::project(const Vec3d& _point) const
{
  return forward_projection_.transform_point(_point);
}


//-----------------------------------------------------------------------------


Vec3d GLState::unproject(const Vec3d& _winPoint) const
{
  return backward_projection_.transform_point(_winPoint);
}


//-----------------------------------------------------------------------------


void GLState::set_clear_color(const Vec4f& _col)
{
  clear_color_ = _col;

  if (updateGL_)
  {
    makeCurrent();
    glClearColor(_col[0], _col[1], _col[2], _col[3]);
  }
}


//-----------------------------------------------------------------------------


void GLState::set_base_color(const Vec4f& _col)
{
  base_color_ = _col;

  if (updateGL_)
  {
    makeCurrent();
    glColor4fv(_col.data());
  }
}


//-----------------------------------------------------------------------------


void GLState::set_ambient_color(const Vec4f& _col)
{
  ambient_color_ = _col;

  if (updateGL_)
  {
    makeCurrent();
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, _col.data());
  }
}


//-----------------------------------------------------------------------------


void GLState::set_diffuse_color(const Vec4f& _col)
{
  diffuse_color_ = _col;

  if (updateGL_)
  {
    makeCurrent();
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, _col.data());
  }
}


//-----------------------------------------------------------------------------


void GLState::set_specular_color(const Vec4f& _col)
{
  specular_color_ = _col;

  if (updateGL_)
  {
    makeCurrent();
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, _col.data());
  }
}


//-----------------------------------------------------------------------------


void GLState::set_shininess(float _shininess)
{
  shininess_ = _shininess;

  if (updateGL_)
  {
    makeCurrent();
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, _shininess);
  }
}


//-----------------------------------------------------------------------------


void GLState::set_point_size(float _f)
{
  point_size_ = _f;

  if (updateGL_)
  {
    makeCurrent();
    glPointSize(point_size_);
  }
}


//-----------------------------------------------------------------------------


void GLState::set_line_width(float _f)
{
  line_width_ = _f;

  if (updateGL_)
  {
    makeCurrent();
    glLineWidth(line_width_);
  }
}


//-----------------------------------------------------------------------------


void GLState::set_twosided_lighting(bool _b)
{
  twosided_lighting_ = _b;

  if (updateGL_)
  {
    makeCurrent();
    if (twosided_lighting_)
      glLightModeli( GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE );
    else
      glLightModeli( GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE );
  }
}


//-----------------------------------------------------------------------------

void GLState::set_multisampling(bool _b)
{

  multisampling_ = _b;

  if (updateGL_)
  {
    makeCurrent();
    if ( allow_multisampling_ ) {

      if ( _b )
        glEnable( GL_MULTISAMPLE );
      else
        glDisable( GL_MULTISAMPLE );

    } else {

      multisampling_ = false;

      if ( glIsEnabled( GL_MULTISAMPLE ) )
        glDisable( GL_MULTISAMPLE );

    }
  }

}

//-----------------------------------------------------------------------------

double GLState::fovy() const
{
    assert(projection_(0,0) != 0.0);

    return atan(1.0/projection_(0,0))*2.0;
}

//-----------------------------------------------------------------------------

double GLState::aspect() const
{
    assert(projection_(1,1) != 0.0);

    return projection_(0,0) / projection_(1,1);
}

//-----------------------------------------------------------------------------

Vec3d GLState::eye() const
{
  return inverse_modelview_.transform_point(Vec3d(0.0, 0.0, 0.0));
}

//-----------------------------------------------------------------------------


Vec3d GLState::viewing_direction(int _x, int _y) const
{
  Vec3d dir = ( unproject(Vec3d(_x, _y, 1.0)) -
		unproject(Vec3d(_x, _y, 0.0)) );
  dir.normalize();
  return dir;
}


//-----------------------------------------------------------------------------


Vec3d GLState::up() const
{
  Vec3d dir( unproject(Vec3d(0.5*width_, height_-1,   0.0)) -
	     unproject(Vec3d(0.5*width_, 0.5*height_, 0.0)) );
  dir.normalize();
  return dir;
}


//-----------------------------------------------------------------------------


Vec3d GLState::right() const
{
  Vec3d dir( unproject(Vec3d(width_-1,   0.5*height_, 0.0)) -
	     unproject(Vec3d(0.5*width_, 0.5*height_, 0.0)) );
  dir.normalize();
  return dir;
}


//-----------------------------------------------------------------------------


void GLState::viewing_ray( int _x, int _y,
			   Vec3d& _origin, Vec3d& _direction) const
{
  _origin = unproject(Vec3d(_x, _y, 0.0));
  _direction = unproject(Vec3d(_x, _y, 1.0)) - _origin;
  _direction.normalize();
}


//-----------------------------------------------------------------------------


void GLState::push_projection_matrix()
{
  stack_projection_.push(projection_);
  stack_inverse_projection_.push(inverse_projection_);

  if (updateGL_)
  {
    makeCurrent();
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glMatrixMode(GL_MODELVIEW);
  }
}


//-----------------------------------------------------------------------------


void GLState::pop_projection_matrix()
{
  projection_ = stack_projection_.top();
  inverse_projection_ = stack_inverse_projection_.top();

  stack_projection_.pop();
  stack_inverse_projection_.pop();

  update_matrices();

  if (updateGL_)
  {
    makeCurrent();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
  }
}


//-----------------------------------------------------------------------------


void GLState::push_modelview_matrix()
{
  stack_modelview_.push(modelview_);
  stack_inverse_modelview_.push(inverse_modelview_);

  update_matrices();

  if (updateGL_)
  {
    makeCurrent();
    glPushMatrix();
  }
}


//-----------------------------------------------------------------------------


void GLState::pop_modelview_matrix()
{
  modelview_ = stack_modelview_.top();
  inverse_modelview_ = stack_inverse_modelview_.top();

  stack_modelview_.pop();
  stack_inverse_modelview_.pop();

  update_matrices();

  if (updateGL_)
  {
    makeCurrent();
    glPopMatrix();
  }
}

//-----------------------------------------------------------------------------

void GLState::pick_init (bool _color)
{
  colorPicking_ = _color;
  colorStack_.initialize ();
  glInitNames();
  glPushName((GLuint) 0);
}

//-----------------------------------------------------------------------------

bool GLState::pick_set_maximum (unsigned int _idx)
{
  bool rv = colorStack_.setMaximumIndex (_idx);
  if (colorPicking_)
    return rv;
  return true;
}

//-----------------------------------------------------------------------------

void GLState::pick_set_name (unsigned int _idx)
{
  colorStack_.setIndex (_idx);
  glLoadName (_idx);
}

//-----------------------------------------------------------------------------

Vec4uc GLState::pick_get_name_color (unsigned int _idx)
{
  if (colorPicking_)
    return colorStack_.getIndexColor (_idx);
  return Vec4uc (0, 0, 0, 0);
}

//-----------------------------------------------------------------------------

void GLState::pick_push_name (unsigned int _idx)
{
  colorStack_.pushIndex (_idx);
  glLoadName (_idx);
  glPushName (0);
}

//-----------------------------------------------------------------------------

void GLState::pick_pop_name ()
{
  colorStack_.popIndex ();
  glPopName ();
}

//-----------------------------------------------------------------------------

std::vector<unsigned int> GLState::pick_color_to_stack (Vec4uc _rgba) const
{
  if (colorPicking_ && colorStack_.initialized ())
    return colorStack_.colorToStack (_rgba);
  return std::vector<unsigned int> ();
}

//-----------------------------------------------------------------------------

unsigned int GLState::pick_free_indicies () const
{
  if (colorPicking_ && colorStack_.initialized ())
    return colorStack_.freeIndicies ();
  return -1;
}

//-----------------------------------------------------------------------------

bool GLState::pick_error () const
{
  if (colorPicking_)
    return colorStack_.error ();
  return false;
}

//-----------------------------------------------------------------------------

unsigned int GLState::pick_current_index () const
{
  if (colorPicking_)
    return colorStack_.currentIndex ();
  else
    return 0;
}

//-----------------------------------------------------------------------------

bool GLState::color_picking () const
{
  return colorPicking_;
}

//=============================================================================
} // namespace ACG
//=============================================================================
