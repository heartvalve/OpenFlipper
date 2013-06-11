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
//  CLASS GlutViewer - IMPLEMENTATION
//
//=============================================================================

//== INCLUDES =================================================================


#include "GlutViewer.hh"
#include <cstdio>


//== NAMESPACES ===============================================================


namespace ACG {


//== IMPLEMENTATION ========================================================== 


std::map<int, GlutViewer*>  GlutViewer::windows__;


//-----------------------------------------------------------------------------


GlutViewer::GlutViewer(const char* _title, int _width, int _height) :
        width_(_width),
        height_(_height),
        fullscreen_(false),
        bak_left_(0),
        bak_top_(0),
        bak_width_(0),
        bak_height_(0)
{
  // create window
  glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE | GLUT_ALPHA);
  glutInitWindowSize(_width, _height);
  glViewport(0, 0, _width, _height); 
  windowID_ = glutCreateWindow(_title);
  windows__[windowID_] = this;


  // register callbacks
  glutDisplayFunc(display__);
  glutKeyboardFunc(keyboard__);
  glutSpecialFunc(special__);
  glutMouseFunc(mouse__);
  glutMotionFunc(motion__);
  glutPassiveMotionFunc(passivemotion__);
  glutReshapeFunc(reshape__); 
  glutVisibilityFunc(visibility__);


  // init GL
  init();
}
  

//-----------------------------------------------------------------------------


GlutViewer::
~GlutViewer()
{
  glutDestroyWindow(windowID_);
}


//-----------------------------------------------------------------------------


void
GlutViewer::init()
{
  // init GL state
  glstate_.initialize();


  // OpenGL state
  ACG::GLState::enable(GL_DEPTH_TEST);
  ACG::GLState::enable(GL_LIGHTING);
  ACG::GLState::disable(GL_DITHER);
  ACG::GLState::shadeModel(GL_FLAT);
  glFrontFace(GL_CCW);


  // light sources
  glLoadIdentity();
  GLfloat pos[4], col[4];
  col[0] = col[1] = col[2] = 0.6;
  pos[3] = 0.0;
  col[3] = 1.0;

#define SET_LIGHT(i,x,y,z) { 			\
  pos[0]=x; pos[1]=y; pos[2]=z;			\
  glLightfv(GL_LIGHT##i, GL_POSITION, pos);	\
  glLightfv(GL_LIGHT##i, GL_DIFFUSE,  col);	\
  glLightfv(GL_LIGHT##i, GL_SPECULAR, col);	\
  ACG::GLState::enable(GL_LIGHT##i);			\
}

  SET_LIGHT(0,  0.0,  0.0, 1.0);
  SET_LIGHT(1, -1.0,  1.0, 0.7);
  SET_LIGHT(2,  1.0,  1.0, 0.7);



  // projection
  near_ = 0.1;
  far_  = 100.0;
  fovy_ = 45.0;
  update_projection();
  glstate_.viewport(0, 0, width_, height_);
  glstate_.translate(0,0,-3);
}


//-----------------------------------------------------------------------------


void 
GlutViewer::update_projection()
{
  glstate_.reset_projection();
  glstate_.perspective(fovy_, 
		       (GLfloat) width_ / (GLfloat) height_,
		       near_, far_);
}


//-----------------------------------------------------------------------------


GlutViewer* GlutViewer::current_window() { 
  return windows__[glutGetWindow()]; 
}

void GlutViewer::display__(void) {
  current_window()->display();
}

void GlutViewer::idle__(void) {
  current_window()->idle();
} 

void GlutViewer::keyboard__(unsigned char key, int x, int y) {
  current_window()->keyboard((int)key, x, y);
}

void GlutViewer::motion__(int x, int y) {
  current_window()->motion(x, y);
}

void GlutViewer::mouse__(int button, int state, int x, int y) {
  current_window()->mouse(button, state, x, y);
}

void GlutViewer::passivemotion__(int x, int y) {
  current_window()->passivemotion(x, y);
}

void GlutViewer::reshape__(int w, int h) {
  current_window()->reshape(w, h);
}

void GlutViewer::special__(int key, int x, int y) {
  current_window()->keyboard(key, x, y);
}   

void GlutViewer::visibility__(int visible) {
  current_window()->visibility(visible);
}


//-----------------------------------------------------------------------------


void GlutViewer::idle(void) {
} 

void GlutViewer::keyboard(int key, int /* x */ , int /* y */ ) 
{
  switch (key) {
    case 27: {
      exit(0);
      break;
    }

    case GLUT_KEY_F12: {
      if (!fullscreen_) {
        bak_left_ = glutGet(GLUT_WINDOW_X);
        bak_top_ = glutGet(GLUT_WINDOW_Y);
        bak_width_ = glutGet(GLUT_WINDOW_WIDTH);
        bak_height_ = glutGet(GLUT_WINDOW_HEIGHT);
        glutFullScreen();
        fullscreen_ = true;
      } else {
        glutReshapeWindow(bak_width_, bak_height_);
        glutPositionWindow(bak_left_, bak_top_);
        fullscreen_ = false;
      }
      break;
    }
  }
} 

void GlutViewer::motion(int /* x */ , int /* y */ ) {
}

void GlutViewer::mouse(int /* button */ , int /* state */ , int /* x */ , int /* y */ ) {
}

void GlutViewer::passivemotion(int /* x */ , int /* y */ ) {
}

void GlutViewer::visibility(int /* visible */ ) {
}

void GlutViewer::reshape(int w, int h) 
{
  width_=w; height_=h;
  glstate_.viewport(0, 0, width_, height_);

  glstate_.reset_projection();
  glstate_.perspective(fovy_, 
		       (GLfloat) width_ / (GLfloat) height_,
		       near_, far_);

  glutPostRedisplay();
}

void GlutViewer::display(void) 
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  draw();
  glutSwapBuffers();
}


//=============================================================================
} // namespace ACG
//=============================================================================
