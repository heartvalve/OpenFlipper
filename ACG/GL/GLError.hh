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
 *   $Revision: 10745 $                                                         *
 *   $Author: moebius $                                                      *
 *   $Date: 2011-01-26 10:23:50 +0100 (Wed, 26 Jan 2011) $                   *
 *                                                                           *
\*===========================================================================*/


//=============================================================================
//
// GLError
//
//=============================================================================


#ifndef ACGGLERROR_H
#define ACGGLERROR_H

//=============================================================================
//
// Includes
//
//=============================================================================

#include <ACG/GL/gl.hh>
#include <ACG/Config/ACGDefines.hh>

//=============================================================================
//
// Macros
//
//=============================================================================

#ifndef NDEBUG
#define checkGLError() \
    { int error; if ( (error = glGetError()) != GL_NO_ERROR ) std::cout << "GLError " << __FILE__ << ":" << __LINE__ << " - " << error << std::endl; }

#define checkGLError2(str) \
    { int error; if ( (error = glGetError()) != GL_NO_ERROR ) std::cout << "GLError " << __FILE__ << ":" << __LINE__ << " - " << error << " - " << str <<std::endl; }

#else

#define checkGLError()
#define checkGLError2(str)

#endif

//=============================================================================
namespace ACG {
//=============================================================================

/** \brief replaces the gluErrorToString function at least for the standard OpenGL Headers
 *
 */
std::string ACGDLLEXPORT glErrorToString( GLenum _error );


/** Nice wrapper that outputs all current OpenGL errors to std::cerr.
    If no error is present nothing is printed.
**/
inline void glCheckErrors()
{
  GLenum error;
  while ((error = glGetError()) != GL_NO_ERROR)
  {
    std::cerr << "GL error: " << glErrorToString(error) << std::endl;
  }
}

void ACGDLLEXPORT nurbsErrorCallback(GLenum errorCode);

//=============================================================================
}  // namespace ACG
//=============================================================================
#endif //ACGGLERROR_H defined
//=============================================================================
