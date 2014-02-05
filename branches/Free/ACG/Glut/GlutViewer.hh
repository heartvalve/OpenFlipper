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
//  CLASS GlutViewer
//
//=============================================================================


#ifndef ACG_GLUTVIEWER_HH
#define ACG_GLUTVIEWER_HH


//== INCLUDES =================================================================


#include "../GL/gl.hh"
#include "../GL/GLState.hh"
#include <map>
#include "../Config/ACGDefines.hh"


//== NAMESPACES ===============================================================


namespace ACG {


//== CLASS DEFINITION =========================================================



/** \class GlutViewer GlutViewer.hh <ACG/Glut/GlutViewer.hh>
    Simple Glut viewer.
    Based on C++ glut interface of George Stetten and Korin Crawford.
**/

class ACGDLLEXPORT GlutViewer
{
public:

  GlutViewer(const char* _title, int _width, int _height);
  virtual ~GlutViewer();


protected:

  virtual void draw() = 0;
  virtual void display(void);
  virtual void idle(void);
  virtual void keyboard(int key, int x, int y);
  virtual void motion(int x, int y);
  virtual void mouse(int button, int state, int x, int y);
  virtual void passivemotion(int x, int y);
  virtual void reshape(int w, int h);
  virtual void visibility(int visible);

  void update_projection();

  GLState  glstate_;
  int      width_, height_;
  GLfloat  near_, far_, fovy_;


private:

  void init();

  static void display__(void);
  static void idle__(void);
  static void keyboard__(unsigned char key, int x, int y);
  static void motion__(int x, int y);
  static void mouse__(int button, int state, int x, int y);
  static void passivemotion__(int x, int y);
  static void reshape__(int w, int h);
  static void special__(int key, int x, int y);
  static void visibility__(int visible);

  static std::map<int, GlutViewer*>  windows__;
  static GlutViewer* current_window();


  int  windowID_;
  bool fullscreen_;
  int  bak_left_, bak_top_, bak_width_, bak_height_;
};


//=============================================================================
} // namespace ACG
//=============================================================================
#endif // ACG_GLUTVIEWER_HH defined
//=============================================================================

