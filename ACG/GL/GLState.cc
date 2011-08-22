/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2011 by Computer Graphics Group, RWTH Aachen      *
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
    bb_min_(ACG::Vec3d(0.0,0.0,0.0)),
    bb_max_(ACG::Vec3d(0.0,0.0,0.0)),
    multisampling_(false),    
    allow_multisampling_(true),
    mipmapping_(true),
    updateGL_(_updateGL),
    blending_(false),
    msSinceLastRedraw_ (1),
    depth_func_(GL_LESS),
    depthFuncLock_(false),
    blendFuncLock_(false),
    blendEquationLock_(false),
    blendColorLock_(false),
    alphaFuncLock_(false),
    shadeModelLock_(false),
    cullFaceLock_(false),
    vertexPointerLock_(false),
    normalPointerLock_(false),
    texcoordPointerLock_(false),
    colorPointerLock_(false),
    drawBufferLock_(false),
    programLock_(false)
{
  stateStack_.push_back(GLStateContext());

  // every state is unlocked at start
  for (int i = 0; i < 16; ++i)
    memset(glTextureTargetLock_[i], 0, 4 * sizeof(int));

  memset(glBufferTargetLock_, 0, sizeof(glBufferTargetLock_));

  framebufferLock_[0] = framebufferLock_[1] = false;

  glStateLock_.reset();


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

  // Get max number of texture units
  GLint value;
  glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS_ARB, &value);
  
  num_texture_units_ = value;
  
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


    // GetoriginalScissor settings
    GLboolean scissor =  glIsEnabled(GL_SCISSOR_TEST);
        
    GLint origBox[4];
    glGetIntegerv(GL_SCISSOR_BOX,&origBox[0]);
    
    //Enable scissor 
    if (!scissor)
      glEnable(GL_SCISSOR_TEST);
    
    // Restrict to our current viewport
    glScissor(  left_,bottom_,width_,height_ );
    
    // Clear restricted region
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    
    // Reset to originalsettings
    glScissor(  origBox[0], origBox[1], origBox[2], origBox[3] );
    
    if (!scissor)
      glDisable(GL_SCISSOR_TEST);
      
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

void GLState::translate( Vec3d _vector,
                MultiplyFrom _mult_from ) {
  translate( _vector[0] , _vector[1] , _vector[2] ,_mult_from);
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

void GLState::set_bounding_box(ACG::Vec3d _min, ACG::Vec3d _max ) {
  bb_min_ = _min;
  bb_max_ = _max;
}

//-----------------------------------------------------------------------------

void GLState::get_bounding_box(ACG::Vec3d& _min, ACG::Vec3d& _max ) {
  _min = bb_min_;
  _max = bb_max_;
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
    assert(projection_(1,1) != 0.0);

    return atan(1.0/projection_(1,1))*2.0;
}

//-----------------------------------------------------------------------------

double GLState::aspect() const
{
    assert(projection_(0,0) != 0.0);

    return projection_(1,1) / projection_(0,0);
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

const GLenum& GLState::depthFunc() const
{
  ///\todo Remove this additional check if there are no errors here ever.
  GLenum real_depth;
  glGetIntegerv (GL_DEPTH_FUNC, (GLint*) &real_depth);
  if (depth_func_ != real_depth)
    std::cerr << "GLState depth_func_ ("<<depth_func_<<") doesn't match actual enabled GL_DEPTH_FUNC ("<<real_depth<<")!" << std::endl;

  return depth_func_;
}

//-----------------------------------------------------------------------------

void GLState:: set_depthFunc(const GLenum& _depth_func)
{
  glDepthFunc(_depth_func);
  depth_func_ = _depth_func;
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

//-----------------------------------------------------------------------------

GLenum GLState::glStateCaps[95] = {GL_ALPHA_TEST,
GL_AUTO_NORMAL,
GL_MAP2_VERTEX_3,
GL_MAP2_VERTEX_4,
GL_BLEND,
GL_CLIP_PLANE0,
GL_CLIP_PLANE1,
GL_CLIP_PLANE2,
GL_CLIP_PLANE3,
GL_CLIP_PLANE4,
GL_CLIP_PLANE5,
GL_COLOR_LOGIC_OP,
GL_COLOR_MATERIAL,
GL_COLOR_SUM,
GL_COLOR_TABLE,
GL_CONVOLUTION_1D,
GL_CONVOLUTION_2D,
GL_CULL_FACE,
GL_DEPTH_TEST,
GL_DITHER,
GL_FOG,
GL_HISTOGRAM,
GL_INDEX_LOGIC_OP,
GL_LIGHT0,
GL_LIGHT1,
GL_LIGHT2,
GL_LIGHT3,
GL_LIGHT4,
GL_LIGHT5,
GL_LIGHT6,
GL_LIGHT7,
GL_LIGHTING,
GL_LINE_SMOOTH,
GL_LINE_STIPPLE,
GL_MAP1_COLOR_4,
GL_MAP1_INDEX,
GL_MAP1_NORMAL,
GL_MAP1_TEXTURE_COORD_1,
GL_MAP1_TEXTURE_COORD_2,
GL_MAP1_TEXTURE_COORD_3,
GL_MAP1_TEXTURE_COORD_4,
GL_MAP1_VERTEX_3,
GL_MAP1_VERTEX_4,
GL_MAP2_COLOR_4,
GL_MAP2_INDEX,
GL_MAP2_NORMAL,
GL_MAP2_TEXTURE_COORD_1,
GL_MAP2_TEXTURE_COORD_2,
GL_MAP2_TEXTURE_COORD_3,
GL_MAP2_TEXTURE_COORD_4,
GL_MAP2_VERTEX_3,
GL_MAP2_VERTEX_4,
GL_MINMAX,
GL_MULTISAMPLE,
GL_NORMALIZE,
GL_RESCALE_NORMAL,
GL_POINT_SMOOTH,
GL_POINT_SPRITE,
GL_POLYGON_OFFSET_FILL,
GL_FILL,
GL_POLYGON_OFFSET_LINE,
GL_LINE,
GL_POLYGON_OFFSET_POINT,
GL_POINT,
GL_POLYGON_SMOOTH,
GL_POLYGON_STIPPLE,
GL_POST_COLOR_MATRIX_COLOR_TABLE,
GL_POST_CONVOLUTION_COLOR_TABLE,
GL_RESCALE_NORMAL,
GL_NORMALIZE,
GL_SAMPLE_ALPHA_TO_COVERAGE,
GL_SAMPLE_ALPHA_TO_ONE,
GL_SAMPLE_COVERAGE,
GL_SAMPLE_COVERAGE_INVERT,
GL_SEPARABLE_2D,
GL_SCISSOR_TEST,
GL_STENCIL_TEST,
GL_TEXTURE_1D,
GL_TEXTURE_2D,
GL_TEXTURE_3D,
GL_TEXTURE_CUBE_MAP,
GL_TEXTURE_GEN_Q,
GL_TEXTURE_GEN_R,
GL_TEXTURE_GEN_S,
GL_TEXTURE_GEN_T,
GL_VERTEX_PROGRAM_POINT_SIZE,
GL_VERTEX_PROGRAM_TWO_SIDE,
GL_COLOR_ARRAY,
GL_EDGE_FLAG_ARRAY,
GL_FOG_COORD_ARRAY,
GL_INDEX_ARRAY,
GL_NORMAL_ARRAY,
GL_SECONDARY_COLOR_ARRAY,
GL_TEXTURE_COORD_ARRAY,
GL_VERTEX_ARRAY};

void GLState::syncFromGL()
{
  // get enabled states
  GLenum caps[] = {GL_ALPHA_TEST,
    GL_AUTO_NORMAL,
    GL_MAP2_VERTEX_3,
    GL_MAP2_VERTEX_4,
    GL_BLEND,
    GL_CLIP_PLANE0,
    GL_CLIP_PLANE1,
    GL_CLIP_PLANE2,
    GL_CLIP_PLANE3,
    GL_CLIP_PLANE4,
    GL_CLIP_PLANE5,
    GL_COLOR_LOGIC_OP,
    GL_COLOR_MATERIAL,
    GL_COLOR_SUM,
    GL_COLOR_TABLE,
    GL_CONVOLUTION_1D,
    GL_CONVOLUTION_2D,
    GL_CULL_FACE,
    GL_DEPTH_TEST,
    GL_DITHER,
    GL_FOG,
    GL_HISTOGRAM,
    GL_INDEX_LOGIC_OP,
    GL_LIGHT0,
    GL_LIGHT1,
    GL_LIGHT2,
    GL_LIGHT3,
    GL_LIGHT4,
    GL_LIGHT5,
    GL_LIGHT6,
    GL_LIGHT7,
    GL_LIGHTING,
    GL_LINE_SMOOTH,
    GL_LINE_STIPPLE,
    GL_MAP1_COLOR_4,
    GL_MAP1_INDEX,
    GL_MAP1_NORMAL,
    GL_MAP1_TEXTURE_COORD_1,
    GL_MAP1_TEXTURE_COORD_2,
    GL_MAP1_TEXTURE_COORD_3,
    GL_MAP1_TEXTURE_COORD_4,
    GL_MAP1_VERTEX_3,
    GL_MAP1_VERTEX_4,
    GL_MAP2_COLOR_4,
    GL_MAP2_INDEX,
    GL_MAP2_NORMAL,
    GL_MAP2_TEXTURE_COORD_1,
    GL_MAP2_TEXTURE_COORD_2,
    GL_MAP2_TEXTURE_COORD_3,
    GL_MAP2_TEXTURE_COORD_4,
    GL_MAP2_VERTEX_3,
    GL_MAP2_VERTEX_4,
    GL_MINMAX,
    GL_MULTISAMPLE,
    GL_NORMALIZE,
    GL_RESCALE_NORMAL,
    GL_POINT_SMOOTH,
    GL_POINT_SPRITE,
    GL_POLYGON_OFFSET_FILL,
    GL_FILL,
    GL_POLYGON_OFFSET_LINE,
    GL_LINE,
    GL_POLYGON_OFFSET_POINT,
    GL_POINT,
    GL_POLYGON_SMOOTH,
    GL_POLYGON_STIPPLE,
    GL_POST_COLOR_MATRIX_COLOR_TABLE,
    GL_POST_CONVOLUTION_COLOR_TABLE,
    GL_RESCALE_NORMAL,
    GL_NORMALIZE,
    GL_SAMPLE_ALPHA_TO_COVERAGE,
    GL_SAMPLE_ALPHA_TO_ONE,
    GL_SAMPLE_COVERAGE,
    GL_SAMPLE_COVERAGE_INVERT,
    GL_SEPARABLE_2D,
    GL_SCISSOR_TEST,
    GL_STENCIL_TEST,
    GL_TEXTURE_1D,
    GL_TEXTURE_2D,
    GL_TEXTURE_3D,
    GL_TEXTURE_CUBE_MAP,
    GL_TEXTURE_GEN_Q,
    GL_TEXTURE_GEN_R,
    GL_TEXTURE_GEN_S,
    GL_TEXTURE_GEN_T,
    GL_VERTEX_PROGRAM_POINT_SIZE,
    GL_VERTEX_PROGRAM_TWO_SIDE,
    GL_COLOR_ARRAY,
    GL_EDGE_FLAG_ARRAY,
    GL_FOG_COORD_ARRAY,
    GL_INDEX_ARRAY,
    GL_NORMAL_ARRAY,
    GL_SECONDARY_COLOR_ARRAY,
    GL_TEXTURE_COORD_ARRAY,
    GL_VERTEX_ARRAY};

  for (unsigned int i = 0; i < sizeof(caps) / sizeof(GLenum); ++i)
  {
    if (glIsEnabled(caps[i])) stateStack_.back().glStateEnabled_.set(caps[i]);
    else stateStack_.back().glStateEnabled_.reset(caps[i]);
  }
  
  GLint getparam;
  glGetIntegerv(GL_BLEND_SRC, &getparam);
  stateStack_.back().blendFuncState_[0] = getparam;

  glGetIntegerv(GL_BLEND_DST, &getparam);
  stateStack_.back().blendFuncState_[1] = getparam;


  glGetIntegerv(GL_BLEND_EQUATION_RGB, &getparam);
  stateStack_.back().blendEquationState_ = getparam;
  
  glGetFloatv(GL_BLEND_COLOR, stateStack_.back().blendColorState_);
  
  glGetIntegerv(GL_ALPHA_TEST_FUNC, &getparam);
  stateStack_.back().alphaFuncState_ = getparam;

  glGetFloatv(GL_ALPHA_TEST_REF, &stateStack_.back().alphaRefState_);

  // bound buffers

  GLenum bufGets[8] = {
    GL_ARRAY_BUFFER_BINDING, GL_ARRAY_BUFFER,
    GL_ELEMENT_ARRAY_BUFFER_BINDING, GL_ELEMENT_ARRAY_BUFFER, 
    GL_PIXEL_PACK_BUFFER_BINDING, GL_PIXEL_PACK_BUFFER,
    GL_PIXEL_UNPACK_BUFFER_BINDING, GL_PIXEL_UNPACK_BUFFER};

  for (int i = 0; i < 4; ++i)
    glGetIntegerv(bufGets[i*2], (GLint*)stateStack_.back().glBufferTargetState_ + getBufferTargetIndex(bufGets[i*2+1]));


  // bound textures
  glGetIntegerv(GL_ACTIVE_TEXTURE, &getparam);
  stateStack_.back().activeTexture_ = getparam;

  GLenum texBufGets[] = {
    GL_TEXTURE_BINDING_1D, GL_TEXTURE_1D,
    GL_TEXTURE_BINDING_2D, GL_TEXTURE_2D,
    GL_TEXTURE_BINDING_3D, GL_TEXTURE_3D,
    GL_TEXTURE_BINDING_CUBE_MAP, GL_TEXTURE_CUBE_MAP};

  glGetIntegerv(GL_MAX_TEXTURE_COORDS, &maxTextureCoords_);
  glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxCombinedTextureImageUnits_);

  // safe clamp
  if (maxTextureCoords_ > 16) maxTextureCoords_ = 16;
  if (maxCombinedTextureImageUnits_ > 16) maxCombinedTextureImageUnits_ = 16;

  int numTexUnits = maxTextureCoords_;
  if (numTexUnits < maxCombinedTextureImageUnits_) numTexUnits = maxCombinedTextureImageUnits_;

  for (int i = 0; i < numTexUnits; ++i)
  {
    glActiveTexture(GL_TEXTURE0 + i);

    // for each texture stage query 4 texture types: 1D, 2D, 3D, Cube
    for (int k = 0; k < 4; ++k)
      glGetIntegerv(texBufGets[k*2],
        glTextureTargetLock_[i] + getTextureTargetIndex(texBufGets[k*2+1]));
  }

  // restore active texture unit
  if (numTexUnits > 0)
    glActiveTexture(stateStack_.back().activeTexture_);


  // shade model
  glGetIntegerv(GL_SHADE_MODEL, &getparam);
  stateStack_.back().shadeModel_ = getparam;

  // cull face
  glGetIntegerv(GL_CULL_FACE_MODE, &getparam);
  stateStack_.back().cullFace_ = getparam;


  // vertex pointers
  {
    GLenum ptrEnums[] = {
      GL_VERTEX_ARRAY_SIZE, GL_VERTEX_ARRAY_TYPE,
      GL_VERTEX_ARRAY_STRIDE, GL_VERTEX_ARRAY_POINTER,
      GL_COLOR_ARRAY_SIZE,  GL_COLOR_ARRAY_TYPE,
      GL_COLOR_ARRAY_STRIDE, GL_COLOR_ARRAY_POINTER,
      GL_TEXTURE_COORD_ARRAY_SIZE, GL_TEXTURE_COORD_ARRAY_TYPE,
      GL_TEXTURE_COORD_ARRAY_STRIDE, GL_TEXTURE_COORD_ARRAY_POINTER};

    GLStateContext::GLVertexPointer* ptrs[] = {&stateStack_.back().vertexPointer_, 
      &stateStack_.back().colorPointer_, &stateStack_.back().texcoordPointer_};

    for (int i = 0; i < 3 ; ++i)
    {
      glGetIntegerv(ptrEnums[i*4], &getparam);
      ptrs[i]->size = getparam;
      glGetIntegerv(ptrEnums[i*4+1], &getparam);
      ptrs[i]->type = getparam;
      glGetIntegerv(ptrEnums[i*4+2], &getparam);
      ptrs[i]->stride = getparam;
      glGetPointerv(ptrEnums[i*4+3], (GLvoid**)&ptrs[i]->pointer);
    }

    glGetIntegerv(GL_NORMAL_ARRAY_STRIDE, &getparam);
    stateStack_.back().normalPointer_.size = getparam;
    glGetIntegerv(GL_NORMAL_ARRAY_TYPE, &getparam);
    stateStack_.back().normalPointer_.type = getparam;
    glGetPointerv(GL_NORMAL_ARRAY_POINTER, (GLvoid**)&stateStack_.back().normalPointer_.pointer);
  }


  // draw buffer state
  glGetIntegerv(GL_MAX_DRAW_BUFFERS, &maxDrawBuffers_);
  if (maxDrawBuffers_ > 16) maxDrawBuffers_ = 16;

  for (int i = 0; i < maxDrawBuffers_; ++i)
  {
    glGetIntegerv(GL_DRAW_BUFFER0 + i, &getparam);
    stateStack_.back().drawBufferState_[i] = getparam;
  }

  glGetIntegerv(GL_DRAW_BUFFER, &getparam);
  stateStack_.back().drawBufferSingle_ = getparam;

  // framebuffer
  glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &getparam);
  stateStack_.back().framebuffers_[0] = getparam;
  glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &getparam);
  stateStack_.back().framebuffers_[1] = getparam;

  // shader program
  glGetIntegerv(GL_CURRENT_PROGRAM, &getparam);
  stateStack_.back().program_ = getparam;
}

//-----------------------------------------------------------------------------


void GLState::enable(GLenum _cap)
{
  if (!glStateLock_.test(_cap))
  {
    if (!stateStack_.back().glStateEnabled_.test(_cap))
    {
      glEnable(_cap);
      stateStack_.back().glStateEnabled_.set(_cap);
    }
  }
}

void GLState::disable(GLenum _cap)
{
  if (!glStateLock_.test(_cap))
  {
    if (stateStack_.back().glStateEnabled_.test(_cap))
    {
      glDisable(_cap);
      stateStack_.back().glStateEnabled_.reset(_cap);
    }
  }
}

void GLState::lockState(GLenum _cap)
{
  glStateLock_.set(_cap);
}

void GLState::unlockState(GLenum _cap)
{
  glStateLock_.reset(_cap);
}

bool GLState::isStateLocked(GLenum _cap)
{
  return glStateLock_.test(_cap);
}

bool GLState::isStateEnabled(GLenum _cap)
{
  return stateStack_.back().glStateEnabled_.test(_cap);
}

//-----------------------------------------------------------------------------
// client state functions

void GLState::enableClientState(GLenum _cap)
{
  if (!glStateLock_.test(_cap))
  {
    if (!stateStack_.back().glStateEnabled_.test(_cap))
    {
      glEnableClientState(_cap);
      stateStack_.back().glStateEnabled_.set(_cap);
    }
  }
}

void GLState::disableClientState(GLenum _cap)
{
  if (!glStateLock_.test(_cap))
  {
    if (stateStack_.back().glStateEnabled_.test(_cap))
    {
      glDisableClientState(_cap);
      stateStack_.back().glStateEnabled_.reset(_cap);
    }
  }
}

void GLState::lockClientState(GLenum _cap)
{
  glStateLock_.set(_cap);
}

void GLState::unlockClientState(GLenum _cap)
{
  glStateLock_.reset(_cap);
}

bool GLState::isClientStateLocked(GLenum _cap)
{
  return glStateLock_.test(_cap);
}

bool GLState::isClientStateEnabled(GLenum _cap)
{
  return stateStack_.back().glStateEnabled_.test(_cap);
}

//-----------------------------------------------------------------------------
// blending functions

void GLState::blendFunc(GLenum _sfactor, GLenum _dfactor)
{
  if (!blendFuncLock_)
  {
    if (stateStack_.back().blendFuncState_[0] != _sfactor && stateStack_.back().blendFuncState_[1] != _dfactor)
    {
      glBlendFunc(_sfactor, _dfactor);
      stateStack_.back().blendFuncState_[0] = _sfactor;
      stateStack_.back().blendFuncState_[1] = _dfactor;
    }
  }
}

void GLState::getBlendFunc(GLenum* _sfactor, GLenum* _dfactor)
{
  if (_sfactor) *_sfactor = stateStack_.back().blendFuncState_[0];
  if (_dfactor) *_dfactor = stateStack_.back().blendFuncState_[1];
}

void GLState::blendEquation(GLenum _mode)
{
  if (!blendEquationLock_)
  {
    if (stateStack_.back().blendEquationState_ != _mode)
    {
      glBlendEquation(_mode);
      stateStack_.back().blendEquationState_ = _mode;
    }
  }
}

void GLState::blendColor(GLclampf _red, GLclampf _green, GLclampf _blue, GLclampf _alpha)
{
  if (!blendColorLock_)
  {
    if (stateStack_.back().blendColorState_[0] != _red && stateStack_.back().blendColorState_[1] != _green &&
        stateStack_.back().blendColorState_[2] != _blue && stateStack_.back().blendColorState_[3] != _alpha)
    {
      glBlendColor(_red, _green, _blue, _alpha);
      stateStack_.back().blendColorState_[0] = _red;  stateStack_.back().blendColorState_[1] = _green;
      stateStack_.back().blendColorState_[2] = _blue;  stateStack_.back().blendColorState_[3] = _alpha;
    }
  }
}

void GLState::getBlendColor(GLclampf* _col)
{
  for (int i = 0; i < 4; ++i) _col[i] = stateStack_.back().blendColorState_[i];
}


void GLState::alphaFunc(GLenum _func, GLclampf _ref)
{
  if (!alphaFuncLock_)
  {
    if (stateStack_.back().alphaFuncState_ != _func && stateStack_.back().alphaRefState_ != _ref)
    {
      glAlphaFunc(_func, _ref);
      stateStack_.back().alphaFuncState_ = _func;
      stateStack_.back().alphaRefState_ = _ref;
    }
  }
}

void GLState::getAlphaFunc(GLenum* _func, GLclampf* _ref)
{
  if (_func) *_func = stateStack_.back().alphaFuncState_;
  if (_ref) *_ref = stateStack_.back().alphaRefState_;
}

void GLState::shadeModel(GLenum _mode)
{
  if (!shadeModelLock_)
  {
    if (stateStack_.back().shadeModel_ != _mode)
    {
      glShadeModel(_mode);
      stateStack_.back().shadeModel_ = _mode;
    }
  }
}

void GLState::cullFace(GLenum _mode)
{
  if (!cullFaceLock_)
  {
    if (stateStack_.back().cullFace_ != _mode)
    {
      glCullFace(_mode);
      stateStack_.back().cullFace_ = _mode;
    }
  }
}

//-----------------------------------------------------------------------------

int GLState::getBufferTargetIndex(GLenum _target)
{
  switch (_target)
  {
  case GL_ARRAY_BUFFER: return 0;
  case GL_ELEMENT_ARRAY_BUFFER: return 1;
  case GL_PIXEL_PACK_BUFFER: return 2;
  case GL_PIXEL_UNPACK_BUFFER: return 3;
  }
  return -1;
}

void GLState::bindBuffer(GLenum _target, GLuint _buffer)
{
  int idx = getBufferTargetIndex(_target);
  if (!glBufferTargetLock_[idx])
  {
    if (stateStack_.back().glBufferTargetState_[idx] != _buffer)
    {
      glBindBuffer(_target, _buffer);
      stateStack_.back().glBufferTargetState_[idx] = _buffer;
    }
  }
}

void GLState::lockBufferTarget(GLenum _target)
{
  glBufferTargetLock_[getBufferTargetIndex(_target)] = 1;
}

void GLState::unlockBufferTarget(GLenum _target)
{
  glBufferTargetLock_[getBufferTargetIndex(_target)] = 0;
}

bool GLState::isBufferTargetLocked(GLenum _target)
{
  return glBufferTargetLock_[getBufferTargetIndex(_target)] != 0;
}

GLuint GLState::getBoundBuf(GLenum _target)
{
  return stateStack_.back().glBufferTargetState_[getBufferTargetIndex(_target)];
}

//-----------------------------------------------------------------------------

int GLState::getTextureTargetIndex(GLenum _target)
{
  switch (_target)
  {
  case GL_TEXTURE_2D: return 1;
  case GL_TEXTURE_CUBE_MAP: return 3;
  case GL_TEXTURE_1D: return 0;
  case GL_TEXTURE_3D: return 2;
  }
  return -1;
}

void GLState::bindTexture(GLenum _target, GLuint _buffer)
{
  int activeTex = getActiveTexture();

  int texTargetIdx = getTextureTargetIndex(_target);
  assert(texTargetIdx >= 0);

  if (!glTextureTargetLock_[activeTex][texTargetIdx])
  {
    if (_buffer != stateStack_.back().glTextureTargetState_[activeTex][texTargetIdx])
    {
      glBindTexture(_target, _buffer);
      stateStack_.back().glTextureTargetState_[activeTex][texTargetIdx] = _buffer;
    }
  }
}

void GLState::lockTextureTarget(GLenum _target)
{
  glTextureTargetLock_[getActiveTexture()][getTextureTargetIndex(_target)] = 1;
}

void GLState::unlockTextureTarget(GLenum _target)
{
  glTextureTargetLock_[getActiveTexture()][getTextureTargetIndex(_target)] = 0;
}

bool GLState::isTextureTargetLocked(GLenum _target)
{
  return glTextureTargetLock_[getActiveTexture()][getTextureTargetIndex(_target)] != 0;
}

GLuint GLState::getBoundTextureBuffer(GLenum _target)
{
  return stateStack_.back().glTextureTargetState_[getActiveTexture()][getTextureTargetIndex(_target)];
}


//----------------------------------------------------------
// vertex pointers

void GLState::vertexPointer(GLint _size, GLenum _type, GLsizei _stride, const GLvoid* _pointer)
{
  if (!vertexPointerLock_)
  {
    if (!stateStack_.back().vertexPointer_.equals(_size, _type, _stride, _pointer))
    {
      glVertexPointer(_size, _type, _stride, _pointer);
      stateStack_.back().vertexPointer_.set(_size, _type, _stride, _pointer);
    }
  }
}

void GLState::getVertexPointer(GLint* _size, GLenum* _type, GLsizei* _stride, const GLvoid** _pointer)
{
  if (_size) *_size = stateStack_.back().vertexPointer_.size;
  if (_stride) *_stride = stateStack_.back().vertexPointer_.stride;
  if (_type) *_type = stateStack_.back().vertexPointer_.type;
  if (_pointer) *_pointer = stateStack_.back().vertexPointer_.pointer;
}

void GLState::normalPointer(GLenum _type, GLsizei _stride, const GLvoid* _pointer)
{
  if (!normalPointerLock_)
  {
    if (!stateStack_.back().normalPointer_.equals(stateStack_.back().normalPointer_.size, _type, _stride, _pointer))
    {
      glNormalPointer(_type, _stride, _pointer);
      stateStack_.back().normalPointer_.set(3, _type, _stride, _pointer);
    }
  }
}

void GLState::getNormalPointer(GLenum* _type, GLsizei* _stride, const GLvoid** _pointer)
{
  if (_type) *_type = stateStack_.back().normalPointer_.type;
  if (_stride) *_stride = stateStack_.back().normalPointer_.stride;
  if (_pointer) *_pointer = stateStack_.back().normalPointer_.pointer;
}


void GLState::colorPointer(GLint _size, GLenum _type, GLsizei _stride, const GLvoid* _pointer)
{
  if (!colorPointerLock_)
  {
    if (!stateStack_.back().colorPointer_.equals(_size, _type, _stride, _pointer))
    {
      glColorPointer(_size, _type, _stride, _pointer);
      stateStack_.back().colorPointer_.set(_size, _type, _stride, _pointer);
    }
  }
}

void GLState::getColorPointer(GLint* _size, GLenum* _type, GLsizei* _stride, const GLvoid** _pointer)
{
  if (_size) *_size = stateStack_.back().colorPointer_.size;
  if (_stride) *_stride = stateStack_.back().colorPointer_.stride;
  if (_type) *_type = stateStack_.back().colorPointer_.type;
  if (_pointer) *_pointer = stateStack_.back().colorPointer_.pointer;
}

void GLState::texcoordPointer(GLint _size, GLenum _type, GLsizei _stride, const GLvoid* _pointer)
{
  if (!texcoordPointerLock_)
  {
    if (!stateStack_.back().texcoordPointer_.equals(_size, _type, _stride, _pointer))
    {
      glTexCoordPointer(_size, _type, _stride, _pointer);
      stateStack_.back().texcoordPointer_.set(_size, _type, _stride, _pointer);
    }
  }
}

void GLState::getTexcoordPointer(GLint* _size, GLenum* _type, GLsizei* _stride, const GLvoid** _pointer)
{
  if (_size) *_size = stateStack_.back().texcoordPointer_.size;
  if (_stride) *_stride = stateStack_.back().texcoordPointer_.stride;
  if (_type) *_type = stateStack_.back().texcoordPointer_.type;
  if (_pointer) *_pointer = stateStack_.back().texcoordPointer_.pointer;
}

//---------------------------------------------------------------------
// draw buffer functions

void GLState::drawBuffer(GLenum _mode)
{
  if (!drawBufferLock_)
  {
    if (stateStack_.back().drawBufferSingle_ != _mode || stateStack_.back().activeDrawBuffer_)
    {
      glDrawBuffer(_mode);
      stateStack_.back().drawBufferSingle_ = _mode;
      stateStack_.back().activeDrawBuffer_ = 0;
    }
  }
}

void GLState::drawBuffers(GLsizei _n, const GLenum* _bufs)
{
  if (!drawBufferLock_)
  {
    int bChange = !stateStack_.back().activeDrawBuffer_;
    for (int i = 0; i < _n && (!bChange); ++i)
    {
      if (stateStack_.back().drawBufferState_[i] != _bufs[i])
        bChange = 1;
    }

    if (bChange)
    {
      glDrawBuffers(_n, _bufs);

      for (int i = 0; i < _n; ++i)
        stateStack_.back().drawBufferState_[i] = _bufs[i];

      stateStack_.back().activeDrawBuffer_ = _n;
    }
  }
}



void GLState::framebuffer(GLenum _target, GLuint _framebuffer)
{
  int i = -1;
  switch (_target)
  {
  case GL_FRAMEBUFFER:
    {
      framebuffer(GL_READ_FRAMEBUFFER, _framebuffer);
      framebuffer(GL_DRAW_FRAMEBUFFER, _framebuffer);
      return;
    }
  case GL_DRAW_FRAMEBUFFER: i = 0; break;
  case GL_READ_FRAMEBUFFER: i = 1; break;
  }

  if (!framebufferLock_[i])
  {
    if (stateStack_.back().framebuffers_[i] != _framebuffer)
    {
      glBindFramebuffer(_target, _framebuffer);
      stateStack_.back().framebuffers_[i] = _framebuffer;
    }
  }
}

void GLState::lockFramebuffer(GLenum _target)
{
  switch (_target)
  {
  case GL_FRAMEBUFFER:
    framebufferLock_[0] = framebufferLock_[1] = true; break;

  case GL_DRAW_FRAMEBUFFER: framebufferLock_[0] = true; break;
  case GL_READ_FRAMEBUFFER: framebufferLock_[1] = true; break;
  }
}

void GLState::unlockFramebuffer(GLenum _target)
{
  switch (_target)
  {
  case GL_FRAMEBUFFER:
    framebufferLock_[0] = framebufferLock_[1] = false; break;

  case GL_DRAW_FRAMEBUFFER: framebufferLock_[0] = false; break;
  case GL_READ_FRAMEBUFFER: framebufferLock_[1] = false; break;
  }
}

bool GLState::isFramebufferLocked(GLenum _target)
{
  switch (_target)
  {
  case GL_FRAMEBUFFER:
    return framebufferLock_[0] && framebufferLock_[1];

  case GL_DRAW_FRAMEBUFFER: return framebufferLock_[0];
  case GL_READ_FRAMEBUFFER: return framebufferLock_[1];
  }
  return false;
}

void GLState::useProgram(GLuint _program)
{
  if (!programLock_)
  {
    if (stateStack_.back().program_ != _program)
    {
      glUseProgram(_program);
      stateStack_.back().program_ = _program;
    }
  }
}

//---------------------------------------------------------------------

void GLState::pushAttrib()
{
  GLStateContext newContext;
  memcpy(&newContext, &stateStack_.back(), sizeof(GLStateContext));
  
  stateStack_.push_back(newContext);
}

void GLState::popAttrib()
{
  assert(stateStack_.size() > 1);

  const GLStateContext* p = &stateStack_[stateStack_.size()-1];

  for (unsigned int i = 0; i < sizeof(glStateCaps) / sizeof(GLenum); ++i)
  {
    if (p->glStateEnabled_.test(glStateCaps[i]))
      enable(glStateCaps[i]);
    else
      disable(glStateCaps[i]);
  }

  blendFunc(p->blendFuncState_[0], p->blendFuncState_[1]);
  blendEquation(p->blendEquationState_);
  blendColor(p->blendColorState_[0], p->blendColorState_[1],
            p->blendColorState_[2], p->blendColorState_[2]);

  alphaFunc(p->alphaFuncState_, p->alphaRefState_);


  bindBuffer(GL_ARRAY_BUFFER, p->glBufferTargetState_[0]);
  bindBuffer(GL_ELEMENT_ARRAY_BUFFER, p->glBufferTargetState_[1]);
  bindBuffer(GL_PIXEL_PACK_BUFFER, p->glBufferTargetState_[2]);
  bindBuffer(GL_PIXEL_UNPACK_BUFFER, p->glBufferTargetState_[3]);

  {
    GLenum targets[4] = {GL_TEXTURE_1D, GL_TEXTURE_2D, GL_TEXTURE_3D, GL_TEXTURE_CUBE_MAP};

    for (int i = 0; i < num_texture_units_; ++i)
    {
      setActiveTexture(GL_TEXTURE0 + i);

      for (int t = 0; t < 4; ++t)
        bindTexture(targets[t], stateStack_.back().glTextureTargetState_[i][t]);

    }
  }

  setActiveTexture(p->activeTexture_);

  shadeModel(p->shadeModel_);

  cullFace(p->cullFace_);


  vertexPointer(p->vertexPointer_.size, p->vertexPointer_.type,
    p->vertexPointer_.stride, p->vertexPointer_.pointer);
  
  normalPointer(p->normalPointer_.type,
    p->normalPointer_.stride, p->normalPointer_.pointer);

  texcoordPointer(p->texcoordPointer_.size, p->texcoordPointer_.type,
    p->texcoordPointer_.stride, p->texcoordPointer_.pointer);

  colorPointer(p->colorPointer_.size, p->colorPointer_.type,
    p->colorPointer_.stride, p->colorPointer_.pointer);


  if (p->activeDrawBuffer_)
    drawBuffers(p->activeDrawBuffer_, p->drawBufferState_);
  else
    drawBuffer(p->drawBufferSingle_);

  framebuffer(GL_DRAW_FRAMEBUFFER, p->framebuffers_[0]);
  framebuffer(GL_READ_FRAMEBUFFER, p->framebuffers_[1]);

  useProgram(p->program_);


  stateStack_.pop_back();

//  -------------------
}




//=============================================================================
} // namespace ACG
//=============================================================================
