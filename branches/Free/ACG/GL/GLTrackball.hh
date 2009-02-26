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
//  CLASS GLTrackball
//
//=============================================================================


#ifndef ACG_GLTRACKBALL_HH
#define ACG_GLTRACKBALL_HH


//== INCLUDES =================================================================


#include "GLState.hh"
#include "../Math/VectorT.hh"


//== NAMESPACES ===============================================================


namespace ACG {


//== CLASS DEFINITION =========================================================

	      

class ACGDLLEXPORT GLTrackball
{
public:
   
  GLTrackball(GLState& _state);
  ~GLTrackball() {}

  void set_center(const Vec3f& _center) { center_ = _center; }

  void mouse_press(int button, int x, int y);
  void mouse_move(int x, int y);
  void mouse_release(int button, int x, int y);


private:

  void rotation(int x, int y);
  void translation(int x, int y);
  void zoom(int x, int y);

  bool map_to_sphere(const Vec2i& _point, Vec3f& _result);


  GLState&    glstate_;
  ACG::Vec3f  center_;

  Vec2i       last_point_2D_;
  Vec3f       last_point_3D_;
  bool        last_point_ok_;

  enum { ROTATION, TRANSLATION, ZOOM }  action_;
  bool button_down_[10];
};


//=============================================================================
} // namespace ACG
//=============================================================================
#endif // ACG_GLTRACKBALL_HH defined
//=============================================================================

