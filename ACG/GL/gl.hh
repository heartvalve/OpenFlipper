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
//  overload some GL functions
//=============================================================================


#ifndef ACG_GLTEXT_HH
#define ACG_GLTEXT_HH


//== INCLUDES =================================================================

#include <cstdlib>
#include <sstream>


#if defined(ARCH_DARWIN)

  #include <glut.h>
  #include <OpenGL/gl.h>
  #include <OpenGL/glu.h>

#elif defined(WIN32)

  #include <windows.h>
  // Dont do this anymore! Use dll version. No problems with plugins and dll
  // but a lot with static linking
  // #  define GLEW_STATIC 1
  #include <gl/glew.h>
  #include <gl/glut.h>

#else

  #include <GL/glew.h>
  #include <GL/glut.h>
  #include <GL/gl.h>
  #include <GL/glu.h>

#endif

#include "../Math/VectorT.hh"


//=============================================================================
namespace ACG {
//=============================================================================


//-------------------------------------------------------------------- glVertex

/// Wrapper: glVertex for Vec2i
inline void glVertex(const Vec2i& _v)  { glVertex2i(_v[0], _v[1]); }
/// Wrapper: glVertex for Vec2f
inline void glVertex(const Vec2f& _v)  { glVertex2fv(_v.data()); }
/// Wrapper: glVertex for Vec2d
inline void glVertex(const Vec2d& _v)  { glVertex2dv(_v.data()); }

/// Wrapper: glVertex for Vec3f
inline void glVertex(const Vec3f& _v)  { glVertex3fv(_v.data()); }
/// Wrapper: glVertex for Vec3d
inline void glVertex(const Vec3d& _v)  { glVertex3dv(_v.data()); }

/// Wrapper: glVertex for Vec4f
inline void glVertex(const Vec4f& _v)  { glVertex4fv(_v.data()); }
/// Wrapper: glVertex for Vec4d
inline void glVertex(const Vec4d& _v)  { glVertex4dv(_v.data()); }



//------------------------------------------------------------------- glTexCoord

/// Wrapper: glTexCoord for Vec2f
inline void glTexCoord(const Vec2f& _t) { glTexCoord2fv(_t.data()); }
/// Wrapper: glTexCoord for Vec2d
inline void glTexCoord(const Vec2d& _t) { glTexCoord2dv(_t.data()); }

/// Wrapper: glTexCoord for Vec3f
inline void glTexCoord(const Vec3f& _t) { glTexCoord3fv(_t.data()); }
/// Wrapper: glTexCoord for Vec3d
inline void glTexCoord(const Vec3d& _t) { glTexCoord3dv(_t.data()); }

/// Wrapper: glTexCoord for Vec4f
inline void glTexCoord(const Vec4f& _t) { glTexCoord4fv(_t.data()); }
/// Wrapper: glTexCoord for Vec4d
inline void glTexCoord(const Vec4d& _t) { glTexCoord4dv(_t.data()); }



//--------------------------------------------------------------------- glNormal

/// Wrapper: glNormal for Vec3f
inline void glNormal(const Vec3f& _n)  { glNormal3fv(_n.data()); }
/// Wrapper: glNormal for Vec3d
inline void glNormal(const Vec3d& _n)  { glNormal3dv(_n.data()); }



//---------------------------------------------------------------------- glColor

/// Wrapper: glColor for Vec3f
inline void glColor(const Vec3f&  _v)  { glColor3fv(_v.data()); }
/// Wrapper: glColor for Vec3uc
inline void glColor(const Vec3uc& _v)  { glColor3ubv(_v.data()); }

/// Wrapper: glColor for Vec4f
inline void glColor(const Vec4f&  _v)  { glColor4fv(_v.data()); }
/// Wrapper: glColor for Vec4uc
inline void glColor(const Vec4uc&  _v) { glColor4ubv(_v.data()); }



//-------------------------------------------------------------- ACG::GLState::vertexPointer
/*
/// Wrapper: ACG::GLState::vertexPointer for Vec2f
inline void glVertexPointer(const Vec2f* _p)
{ ::ACG::GLState::vertexPointer(2, GL_FLOAT, 0, _p); }
/// Wrapper: ACG::GLState::vertexPointer for Vec2d
inline void ACG::GLState::vertexPointer(const Vec2d* _p)
{ ::ACG::GLState::vertexPointer(2, GL_DOUBLE, 0, _p); }

/// Wrapper: ACG::GLState::vertexPointer for Vec3f
inline void ACG::GLState::vertexPointer(const Vec3f* _p)
{ ::ACG::GLState::vertexPointer(3, GL_FLOAT, 0, _p); }
/// Wrapper: ACG::GLState::vertexPointer for Vec3d
inline void ACG::GLState::vertexPointer(const Vec3d* _p)
{ ::ACG::GLState::vertexPointer(3, GL_DOUBLE, 0, _p); }

/// Wrapper: ACG::GLState::vertexPointer for Vec4f
inline void ACG::GLState::vertexPointer(const Vec4f* _p)
{ ::ACG::GLState::vertexPointer(4, GL_FLOAT, 0, _p); }
/// Wrapper: ACG::GLState::vertexPointer for Vec4d
inline void ACG::GLState::vertexPointer(const Vec4d* _p)
{ ::ACG::GLState::vertexPointer(4, GL_DOUBLE, 0, _p); }

/// original method
inline void ACG::GLState::vertexPointer(GLint n, GLenum t, GLsizei s, const GLvoid *p)
{ ::ACG::GLState::vertexPointer(n, t, s, p); }



//-------------------------------------------------------------- ACG::GLState::normalPointer

/// Wrapper: ACG::GLState::normalPointer for Vec3f
inline void ACG::GLState::normalPointer(const Vec3f* _p)
{ ::ACG::GLState::normalPointer(GL_FLOAT, 0, _p); }
/// Wrapper: ACG::GLState::normalPointer for Vec3d
inline void ACG::GLState::normalPointer(const Vec3d* _p)
{ ::ACG::GLState::normalPointer(GL_DOUBLE, 0, _p); }

/// original method
inline void ACG::GLState::normalPointer(GLenum t, GLsizei s, const GLvoid *p)
{ ::ACG::GLState::normalPointer(t, s, p); }



//--------------------------------------------------------------- ACG::GLState::colorPointer

/// Wrapper: ACG::GLState::colorPointer for Vec3uc
inline void ACG::GLState::colorPointer(const Vec3uc* _p)
{ ::ACG::GLState::colorPointer(3, GL_UNSIGNED_BYTE, 0, _p); }
/// Wrapper: ACG::GLState::colorPointer for Vec3f
inline void ACG::GLState::colorPointer(const Vec3f* _p)
{ ::ACG::GLState::colorPointer(3, GL_FLOAT, 0, _p); }

/// Wrapper: ACG::GLState::colorPointer for Vec4uc
inline void ACG::GLState::colorPointer(const Vec4uc* _p)
{ ::ACG::GLState::colorPointer(4, GL_UNSIGNED_BYTE, 0, _p); }
/// Wrapper: ACG::GLState::colorPointer for Vec4f
inline void ACG::GLState::colorPointer(const Vec4f* _p)
{ ::ACG::GLState::colorPointer(4, GL_FLOAT, 0, _p); }

/// original method
inline void ACG::GLState::colorPointer(GLint n, GLenum t, GLsizei s, const GLvoid *p)
{ ::ACG::GLState::colorPointer(n, t, s, p); }



//------------------------------------------------------------ ACG::GLState::texcoordPointer

/// Wrapper: ACG::GLState::texcoordPointer for float
inline void ACG::GLState::texcoordPointer(const float* _p)
{ ::ACG::GLState::texcoordPointer(1, GL_FLOAT, 0, _p); }
/// Wrapper: ACG::GLState::texcoordPointer for Vec2d
inline void ACG::GLState::texcoordPointer(const double* _p)
{ ::ACG::GLState::texcoordPointer(1, GL_DOUBLE, 0, _p); }

/// Wrapper: ACG::GLState::texcoordPointer for Vec2f
inline void ACG::GLState::texcoordPointer(const Vec2f* _p)
{ ::ACG::GLState::texcoordPointer(2, GL_FLOAT, 0, _p); }
/// Wrapper: ACG::GLState::texcoordPointer for Vec2d
inline void ACG::GLState::texcoordPointer(const Vec2d* _p)
{ ::ACG::GLState::texcoordPointer(2, GL_DOUBLE, 0, _p); }

/// Wrapper: ACG::GLState::texcoordPointer for Vec3f
inline void ACG::GLState::texcoordPointer(const Vec3f* _p)
{ ::ACG::GLState::texcoordPointer(3, GL_FLOAT, 0, _p); }
/// Wrapper: ACG::GLState::texcoordPointer for Vec3d
inline void ACG::GLState::texcoordPointer(const Vec3d* _p)
{ ::ACG::GLState::texcoordPointer(3, GL_DOUBLE, 0, _p); }

/// Wrapper: ACG::GLState::texcoordPointer for Vec4f
inline void ACG::GLState::texcoordPointer(const Vec4f* _p)
{ ::ACG::GLState::texcoordPointer(4, GL_FLOAT, 0, _p); }
/// Wrapper: ACG::GLState::texcoordPointer for Vec4d
inline void ACG::GLState::texcoordPointer(const Vec4d* _p)
{ ::ACG::GLState::texcoordPointer(4, GL_DOUBLE, 0, _p); }

/// original method
inline void ACG::GLState::texcoordPointer(GLint n, GLenum t, GLsizei s, const GLvoid *p)
{ ::ACG::GLState::texcoordPointer(n, t, s, p); }
*/


//-----------------------------------------------------------------------------

/** Check if the extension given by a std::string is supported by the current OpenGL extension
*/
inline bool checkExtensionSupported( std::string _extension )  {
   std::string supported((const char*)glGetString(GL_EXTENSIONS));

   return (supported.find(_extension) != std::string::npos);
}

/** Check if OpenGL Version is greater or equal than the given values
*/
inline bool openGLVersion( const int _major, const int _minor  )  {

  // Read OpenGL Version string
  std::string glVersionString = (const char*)glGetString(GL_VERSION);

  // Use string stream to parse
  std::istringstream stream;
  stream.str(glVersionString);

  // Buffer for the dot between major and minor
  char dot;

  // Read Major version number
  int major ;
  stream >> major;
  stream >> dot;

  // Read minor version number
  int minor;
  stream >> minor;

  if ( (_major > major) || ( (_major == major) && (_minor > minor))  ) {
    std::cerr << "OpenGL Version check failed. Required  : " << _major << "." << _minor << std::endl;
    std::cerr << "OpenGL Version check failed. Available : " << major << "." << minor << std::endl;
    return false;
  }

  return true;
}


/** Nice wrapper that outputs all current OpenGL errors to std::cerr.
    If no error is present nothing is printed.
**/
inline void glCheckErrors()
{
  GLenum error;
  while ((error = glGetError()) != GL_NO_ERROR)
  {
    std::cerr << "GL error: " << gluErrorString(error) << std::endl;
  }
}


//=============================================================================
}  // namespace ACG
//=============================================================================
#endif // ACG_GLTEXT_HH defined
//=============================================================================
