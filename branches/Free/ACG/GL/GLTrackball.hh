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
  ACG::Vec3d  center_;

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

