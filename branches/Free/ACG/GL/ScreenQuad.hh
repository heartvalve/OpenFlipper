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
 *   $Revision: $                                                         *
 *   $Author:  $                                                      *
 *   $Date:  $                   *
 *                                                                           *
\*===========================================================================*/



//=============================================================================
//
//  CLASS ScreenQuad
//
//=============================================================================

#ifndef ACG_SCREENQUAD_HH
#define ACG_SCREENQUAD_HH


//== INCLUDES =================================================================

#include <vector>

#include <ACG/GL/gl.hh>
#include <ACG/GL/VertexDeclaration.hh>


//== NAMESPACES ===============================================================


namespace ACG {


//== CLASS DEFINITION =========================================================


/** This class provides a simple method to draw a screen-aligned quad.
*/
class ACGDLLEXPORT ScreenQuad
{
public:
   /// Destructor.
  ~ScreenQuad();


  /** \brief Draw the screen quad
  *
  * The quad is in projected space with coordinates in [-1, 1].
  * @param _prog GLSL shader to bind attribute id's if needed. Pass null-pointer for fixed function rendering
  */ 
  static void draw(GLSL::Program* _prog = 0);



  /** \brief Draw a 2D texture to screen
  *
  * Useful for debugging, copying a texture..
  *
  * @param _texture Texture that should be bound when drawing the quad
  * @param _offset  Offset passed to the uniform called offset in the shader
  * @param _size    Size passed to the uniform called size in the shader
  */ 
  static void drawTexture2D(GLuint _texture, const Vec2f& _offset = Vec2f(0.0f, 0.0f),
                                             const Vec2f& _size   = Vec2f(1.0f, 1.0f));

private:

  /// Default constructor.
  ScreenQuad();


  /// Get singleton instance
  static ScreenQuad& instance();

  /// Initialize vbo and format
  void init();

  /// Internal draw function
  void intDraw(GLSL::Program* _prog);



  /// vbo containing the quad in projected coordinates
  GLuint vbo_;

  /// vertex format of screen quad (float2 pos)
  VertexDeclaration* decl_;


  /// Simple texture drawing shader
  GLSL::Program* texDrawProg_;
};


//=============================================================================
} // namespace ACG
//=============================================================================
#endif // ACG_SCREENQUAD_HH defined
//=============================================================================

