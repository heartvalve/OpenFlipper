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

//=============================================================================
//
// Includes
//
//=============================================================================

#include "GLError.hh"
#include <string>

//=============================================================================
namespace ACG {
//=============================================================================

std::string glErrorToString( GLenum _error ) {
  switch (_error) {
    case GL_NO_ERROR:                       return "No Error";
    case GL_INVALID_ENUM:                   return "invalid enumerant";
    case GL_INVALID_VALUE:                  return "invalid value";
    case GL_INVALID_OPERATION:              return "invalid operation";
# if defined(GL_INVALID_FRAMEBUFFER_OPERATION)
    case GL_INVALID_FRAMEBUFFER_OPERATION:  return "invalid framebuffer operation";
#endif
    case GL_STACK_OVERFLOW:                 return "Stack overflow";
    case GL_STACK_UNDERFLOW:                return "stack underflow";
    case GL_OUT_OF_MEMORY:                  return "out of memory";
    default:
      return "Unknown OpenGL Error! glErrorToString is unable to convert this error number to a string!" ;
  }
}


void nurbsErrorCallback(GLenum errorCode)
{
  const GLubyte *estring;
  estring = gluErrorString(errorCode);
  std::cerr << "Nurbs Error: " << estring << std::endl;;
}

//=============================================================================
}  // namespace ACG
//=============================================================================

