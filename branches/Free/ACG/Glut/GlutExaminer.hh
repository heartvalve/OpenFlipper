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
//  CLASS GlutExaminer
//
//=============================================================================


#ifndef ACG_GLUTEXAMINER_HH
#define ACG_GLUTEXAMINER_HH


//== INCLUDES =================================================================


#include "GlutViewer.hh"
#include "../GL/GLTrackball.hh"
#include "../Config/ACGDefines.hh"


//== NAMESPACES ===============================================================

namespace ACG {

//== CLASS DEFINITION =========================================================



class ACGDLLEXPORT GlutExaminer : public GlutViewer
{
public:

  GlutExaminer(const char* _title, int _width, int _height);
  ~GlutExaminer() {}


  void   setup_scene(const Vec3f& _center, float _radius);
  void   view_all();
  double measure_fps();


protected:

  virtual void init();
  virtual void draw();
  virtual void motion(int x, int y);
  virtual void mouse(int button, int state, int x, int y);
  virtual void keyboard(int key, int x, int y);


protected:

  GLTrackball  trackball_;
  Vec3f        center_;
  float        radius_;
};


//=============================================================================
} // namespace ACG
//=============================================================================
#endif // ACG_GLUTEXAMINER_HH defined
//=============================================================================

