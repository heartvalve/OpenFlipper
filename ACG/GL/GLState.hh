//=============================================================================
//
//                               OpenFlipper
//        Copyright (C) 2008 by Computer Graphics Group, RWTH Aachen
//                           www.openflipper.org
//
//-----------------------------------------------------------------------------
//
//                                License
//
//  OpenFlipper is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  OpenFlipper is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License
//  along with OpenFlipper.  If not, see <http://www.gnu.org/licenses/>.
//
//-----------------------------------------------------------------------------
//
//   $Revision$
//   $Author$
//   $Date$
//
//=============================================================================




//=============================================================================
//
//  CLASS GLState
//
//=============================================================================


#ifndef ACG_GLSTATE_HH
#define ACG_GLSTATE_HH


//== INCLUDES =================================================================


#include "gl.hh"
#include "../Math/GLMatrixT.hh"
#include "../Math/VectorT.hh"
#include "../Config/ACGDefines.hh"
#include "ColorStack.hh"
#include <stack>
#include <vector>

#ifdef WIN32
	#pragma warning(push)
	#pragma warning(disable:4251)
#endif

//== NAMESPACES ===============================================================


namespace ACG {


//== CLASS DEFINITION =========================================================


/** \class GLState GLState.hh <ACG/Scenegraph/GLState.hh>

    This class stores all relevant OpenGL states and can therefore
    provide some nice and efficient functions like projecting,
    unprojecting, eye point or viewing direction.

    \note This class needs an OpenGL context. This is currently
    given in a QGLContext.
**/

class ACGDLLEXPORT GLState
{
public:

  /// Default constructor
  GLState(bool _updateGL = true);

  /// destructor
  ~GLState() {}


  void makeCurrent() {  }

  /// initialize all state variables (called by constructor)
  void initialize();

  /// should GL matrices be updated after each matrix operation
  bool updateGL() const { return updateGL_; }
  /// should GL matrices be updated after each matrix operation
  void set_updateGL(bool _b) { updateGL_ = _b; }

  /// time passed since last redraw in milliseconds
  unsigned int msSinceLastRedraw () const { return msSinceLastRedraw_; }

  /// set time passed since last redraw in milliseconds
  void set_msSinceLastRedraw (unsigned int _ms) { msSinceLastRedraw_ = _ms; }

  /// set the whole stored gl state
  void setState ();

  /// clear buffers viewport rectangle
  void clearBuffers ();

  //---  set GL projection matrix ---------------------------------------------


  /// reset projection matrix (load identity)
  void reset_projection();

  /// set projection
  void set_projection(const GLMatrixd& _m) {
    GLMatrixd inv_m(_m);  inv_m.invert();  set_projection(_m, inv_m);
  }
  /// set projection and provide inverse projection matrix
  void set_projection(const GLMatrixd& _m, const GLMatrixd& _inv_m);

  /// orthographic projection
  void ortho( double _left, double _right,
	      double _bottom, double _top,
	      double _near_plane, double _far_plane );

  /// perspective projection
  void frustum( double _left, double _right,
		double _bottom, double _top,
		double _near_plane, double _far_plane );

  /// perspective projection
  void perspective( double _fovY, double _aspect,
		    double _near_plane, double _far_plane );

  /// set viewport (lower left corner, width, height, glcontext width, height)
  void viewport( int _left, int _bottom, int _width, int _height,
	         int _glwidth = 0, int _glheight = 0);





  //---  set GL modelview matrix ----------------------------------------------


  /// reset modelview matrix (load identity)
  void reset_modelview();

  /// set modelview
  void set_modelview(const GLMatrixd& _m) {
    GLMatrixd inv_m(_m);  inv_m.invert();  set_modelview(_m, inv_m);
  }
  /// set modelview and provide inverse modelview matrix
  void set_modelview(const GLMatrixd& _m, const GLMatrixd& _inv_m);

  /// set camera by lookAt
  void lookAt( const Vec3d& _eye, const Vec3d& _center, const Vec3d& _up );

  /// translate by (_x, _y, _z)
  void translate( double _x, double _y, double _z,
		  MultiplyFrom _mult_from = MULT_FROM_RIGHT );

  /// rotate around axis (_x, _y, _z) by _angle
  void rotate( double _angle, double _x, double _y, double _z,
	       MultiplyFrom _mult_from = MULT_FROM_RIGHT );

  /// scale by (_s, _s, _s)
  void scale( double _s )
  { scale(_s, _s, _s); }

  /// scale by (_s, _s, _s)
  void scale( double _s,
	      MultiplyFrom /* _mult_from = MULT_FROM_RIGHT  */ )
  { scale(_s, _s, _s); }

  /// scale by (_sx, _sy, _sz)
  void scale( double _sx, double _sy, double _sz,
	      MultiplyFrom _mult_from = MULT_FROM_RIGHT );

  /// multiply by a given transformation matrix
  void mult_matrix( const GLMatrixd& _m, const GLMatrixd& _inv_m,
		    MultiplyFrom _mult_from = MULT_FROM_RIGHT );


  /// push projection matrix
  void push_projection_matrix();
  /// pop projection matrix
  void pop_projection_matrix();

  /// push modelview matrix
  void push_modelview_matrix();
  /// pop modelview matrix
  void pop_modelview_matrix();


  //---  get GL states and matrices -------------------------------------------


  /// get projection matrix
  const GLMatrixd& projection() const {
    return projection_;
  }

  /// get modelview matrix
  const GLMatrixd& modelview() const {
    return modelview_;
  }

  /// get viewport matrix
  const GLMatrixd& viewport() const {
    return window2viewport_;
  }

  /// get forward projection matrix
  const GLMatrixd& forward_projection() const {
    return forward_projection_;
  }

  /// get inverse projection matrix
  const GLMatrixd& inverse_projection() const {
    return inverse_projection_;
  }

  /// get inverse modelview matrix
  const GLMatrixd& inverse_modelview() const {
    return inverse_modelview_;
  }

  /// get viewport
  void get_viewport( int& _left, int& _bottom,
		     int& _width, int& _height ) const {
    _left = left_; _bottom = bottom_; _width = width_; _height = height_;
  }

  /// get viewport width
  int viewport_width() const { return width_; }
  /// get viewport height
  int viewport_height() const { return height_; }

  /// get gl context width
  int context_width() const { return glwidth_; }

  /// get gl context height
  int context_height() const { return glheight_; }

  /// get near clipping distance
  double near_plane() const { return near_plane_; }

  /// get far clipping distance
  double far_plane() const { return far_plane_; }

  /// get field of view in y direction
  double fovy() const { return fovy_; }

  /// get aspect ratio
  double aspect() const { return aspect_; }

  /// get eye point
  Vec3d eye() const;

  /// get viewing ray
  Vec3d viewing_direction() const {
    return viewing_direction(width_>>1, height_>>1);
  }

  /// get viewing ray through pixel (_x,_y)
  Vec3d viewing_direction(int _x, int _y) const;

  /// get up-vector w.r.t. camera coordinates
  Vec3d up() const;

  /// get right-vector w.r.t. camera coordinates
  Vec3d right() const;

  /** get viewing ray (_orgin & _direction) through pixel (_x,_y).
      _direction will be nornalized.
  */
  void viewing_ray(int _x, int _y, Vec3d& _origin, Vec3d& _direction) const;


  //--- project and unproject points ------------------------------------------

  /// project point in world coordinates to window coordinates
  Vec3d project(const Vec3d& _point) const;

  /// unproject point in window coordinates _winPoint to world coordinates
  Vec3d unproject(const Vec3d& _winPoint) const;




  //--- color & material ------------------------------------------------------

  /// default value for clear color
  static const Vec4f default_clear_color;
  /// default value for base color
  static const Vec4f default_base_color;
  /// default value for ambient color
  static const Vec4f default_ambient_color;
  /// default value for diffuse color
  static const Vec4f default_diffuse_color;
  /// default value for speculat color
  static const Vec4f default_specular_color;
  /// default value for shininess
  static const float default_shininess;


  /// set background color
  void set_clear_color(const Vec4f& _col);
  /// get background color
  const Vec4f& clear_color() const { return clear_color_; }

  /// set base color (used when lighting is off)
  void set_base_color(const Vec4f& _col);
  /// get base color (used when lighting is off)
  const Vec4f& base_color() const { return base_color_; }

  /// set ambient color
  void set_ambient_color(const Vec4f& _col);
  /// get ambient color
  const Vec4f& ambient_color() const { return ambient_color_; }

  /// set diffuse color
  void set_diffuse_color(const Vec4f& _col);
  /// get diffuse color
  const Vec4f& diffuse_color() const { return diffuse_color_; }

  /// set specular color
  void set_specular_color(const Vec4f& _col);
  /// get specular color
  const Vec4f& specular_color() const { return specular_color_; }

  /// set specular shininess (must be in [0, 128])
  void set_shininess(float _shininess);
  /// get specular shininess (must be in [0, 128])
  float shininess() const { return shininess_; }




  //--- thickness -------------------------------------------------------------

  /// set point size
  void set_point_size(float _f);
  /// get point size
  float point_size() const { return point_size_; }

  /// set line width
  void set_line_width(float _f);
  /// get line width
  float line_width() const { return line_width_; }




  //--- misc ------------------------------------------------------------------

  /// set whether transparent or solid objects should be drawn
  void set_blending(bool _b) { blending_ = _b; }
  /// get whether transparenet or solid objects should be drawn
  bool blending() { return blending_; }

  /// set whether transparent or solid objects should be drawn
  void set_twosided_lighting(bool _b);
  /// get whether transparenet or solid objects should be drawn
  bool twosided_lighting() { return twosided_lighting_; }
  
  //--- Multi Sampling --------------------------------------------------------
  
  /// Enable or disable multisampling
  void set_multisampling( bool _b );
  
  /// Get current multisampling state
  bool multisampling(){ return multisampling_; };
  
  /// Disable multisampling globally
  void allow_multisampling( bool _b ) { allow_multisampling_ = _b; };
  
  /// Check if Multisampling is globally disabled
  bool multisampling_alloowed(){ return allow_multisampling_; };

  //--- picking ---------------------------------------------------------------

  /// initialize name/color picking stack
  void pick_init (bool _color);

  /// sets the maximum used name index at current stack position (only used in color picking)
  bool pick_set_maximum (unsigned int _idx);

  /// sets the current name/color
  void pick_set_name (unsigned int _idx);

  /// get the current color for index
  Vec4uc pick_get_name_color (unsigned int _idx);

  /// creates a new name the stack
  void pick_push_name (unsigned int _idx);

  /// pops the current name from the stack
  void pick_pop_name ();

  /// converts the given color to index values on the stack (only used in color picking)
  std::vector<unsigned int> pick_color_to_stack (Vec4uc _rgba) const;

  /// returns maximal available index count (only used in color picking)
  unsigned int pick_free_indicies () const;

  /// Did an error occur during picking (only used in color picking)
  bool pick_error () const;

  /// returns the current color picking index (can be used for caching)
  unsigned int pick_current_index () const;

  /// Is color picking active?
  bool color_picking () const;


private: //--------------------------------------------------------------------

  // update forward projection and backward projection matrices
  void update_matrices();


  // matrix stacks
  std::stack<GLMatrixd>  stack_projection_,
                         stack_modelview_,
                         stack_inverse_projection_,
                         stack_inverse_modelview_;

  // current matrices
  GLMatrixd  projection_,
             inverse_projection_,
             modelview_,
             inverse_modelview_,
             window2viewport_,
             inverse_window2viewport_,
             forward_projection_,
             backward_projection_;

  // viewport
  int left_, bottom_, width_, height_;

  // gl context
  int glwidth_, glheight_;

  // projection
  double near_plane_, far_plane_, fovy_;
  double aspect_;

  // colors & materials
  Vec4f   clear_color_,
          base_color_,
          ambient_color_,
          diffuse_color_,
          specular_color_;

  float shininess_;

  // thickness
  float  point_size_, line_width_;

  // lighting
  bool twosided_lighting_;

  // Multisampling settings
  bool multisampling_;
  bool allow_multisampling_;

  // helper: should GL matrices be updated
  bool updateGL_;

  // true -> draw tranparent Objects
  bool blending_;

  // time since last redraw
  unsigned int msSinceLastRedraw_;

  // stack for color picking
  ColorStack colorStack_;

  // are we using color picking
  bool colorPicking_;

};

#ifdef WIN32
	#pragma warning(push)
#endif

//=============================================================================
} // namespace ACG
//=============================================================================
#endif // ACG_GLSTATE_HH defined
//=============================================================================
