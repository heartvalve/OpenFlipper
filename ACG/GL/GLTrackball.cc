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
//  CLASS GLTrackball - IMPLEMENTATION
//
//=============================================================================

//== INCLUDES =================================================================


#include "GLTrackball.hh"


//== NAMESPACES ===============================================================


namespace ACG {


//== IMPLEMENTATION ========================================================== 


GLTrackball::
GLTrackball(GLState& _state) 
  : glstate_(_state), 
    center_(0,0,0) ,
    last_point_ok_(false),
    action_(ROTATION)
{
  for (int i=0; i<10; ++i)
    button_down_[i] = false;
}


//-----------------------------------------------------------------------------


void 
GLTrackball::mouse_press(int button, int x, int y)
{
  last_point_2D_ = ACG::Vec2i(x, y);
  last_point_ok_ = map_to_sphere(last_point_2D_, last_point_3D_);

  button_down_[button] = true;
}


//-----------------------------------------------------------------------------


void 
GLTrackball::mouse_release(int button, int /* x */ , int /* y */ )
{
  last_point_ok_ = false;
  button_down_[button] = false;

  // GLUT: button 3 or 4 -> mouse wheel clicked
  if (button == 3)       
    zoom(0, (int)(last_point_2D_[1] - 0.05*glstate_.viewport_width()));
  else if (button == 4)
    zoom(0, (int)(last_point_2D_[1] + 0.05*glstate_.viewport_width()));
}


//-----------------------------------------------------------------------------


void 
GLTrackball::mouse_move(int x, int y)
{
  if (button_down_[0] && button_down_[1])
    action_ = ZOOM;
  else if (button_down_[0])
    action_ = ROTATION;
  else if (button_down_[1])
    action_ = TRANSLATION;

  switch (action_)
  {
    case ROTATION:    rotation(x, y);     break;
    case TRANSLATION: translation(x, y);  break;
    case ZOOM:        zoom(x, y);         break;
  }

  last_point_2D_ = ACG::Vec2i(x, y);
  last_point_ok_ = map_to_sphere(last_point_2D_, last_point_3D_);
}


//-----------------------------------------------------------------------------


void 
GLTrackball::rotation(int x, int y)
{
  if (last_point_ok_) 
  {
    Vec2i  new_point_2D_;
    Vec3f  new_point_3D_;
    bool   new_point_ok_;

    new_point_2D_ = ACG::Vec2i(x, y);
    new_point_ok_ = map_to_sphere(new_point_2D_, new_point_3D_);
    
    if (new_point_ok_)
    {
      Vec3f axis      = (last_point_3D_ % new_point_3D_);
      float cos_angle = (last_point_3D_ | new_point_3D_);

      if (fabs(cos_angle) < 1.0) 
      {
	    double angle = 2.0*acos(cos_angle) * 180.0 / M_PI;

	    Vec3d t = glstate_.modelview().transform_point(center_);
	    glstate_.translate(-t[0], -t[1], -t[2], MULT_FROM_LEFT);
    	glstate_.rotate(angle, axis[0], axis[1], axis[2], MULT_FROM_LEFT);
	    glstate_.translate( t[0],  t[1],  t[2], MULT_FROM_LEFT);
      }
    }
  }
}


//-----------------------------------------------------------------------------


void 
GLTrackball::translation(int x, int y)
{
  double dx     = double(x - last_point_2D_[0]);
  double dy     = double(y - last_point_2D_[1]);

  double z      = glstate_.modelview().transform_point(center_)[2];
  double w      = double(glstate_.viewport_width());
  double h      = double(glstate_.viewport_height());
  double fovy   = double(glstate_.fovy());
  double nearpl = double(glstate_.near_plane());

  double aspect = w / h;
  double top    = double(tan(fovy/2.0*M_PI/180.0) * nearpl);
  double right  = aspect*top;

  glstate_.translate(-2.0*dx/w*right/nearpl*z, 
		              2.0*dy/h*top/nearpl*z, 
		              0.0,
		              MULT_FROM_LEFT);
}


//-----------------------------------------------------------------------------


void 
GLTrackball::zoom(int /* x */ , int y)
{
  double dy = double(y - last_point_2D_[1]);
  double z  = glstate_.modelview().transform_point(center_)[2];
  double h  = double(glstate_.viewport_height());

  glstate_.translate(0.0,
		     0.0,
		     -2.0*dy/h*z, 
		     MULT_FROM_LEFT);
}


//-----------------------------------------------------------------------------


bool
GLTrackball::map_to_sphere(const Vec2i& _point, Vec3f& _result)
{
  float width  = float(glstate_.viewport_width());
  float height = float(glstate_.viewport_height());
  
  if ( (_point[0] >= 0) && (_point[0] <= width) &&
       (_point[1] >= 0) && (_point[1] <= height) ) 
  {
    double x            = (_point[0] - 0.5*width)  / width;
    double y            = (0.5*height - _point[1]) / height;
    double sinx         = sin(M_PI * x * 0.5);
    double siny         = sin(M_PI * y * 0.5);
    double sinx2siny2   = sinx * sinx + siny * siny;
    
    _result[0] = float(sinx);
    _result[1] = float(siny);
    _result[2] = sinx2siny2 < 1.0 ? float( sqrt(1.0 - sinx2siny2) ) : 0.0f;
    
    return true;
  }
  else return false;
}


//=============================================================================
} // namespace ACG
//=============================================================================
